#include "LCD8bits.h"

//PINOUT
/*
	RS-->PORTC0
	RW-->GND
	E--> PORTC1
	D0-D7 --> PORD0-PORTD7
*/

void dato_a_mostrar(char a)
{
	//PORTD = a;
	// Bits D0 y D1 ? PORTB0 y PORTB1
	PORTB = (PORTB & 0xFC) | (a & 0x03);

	// Bits D2–D7 ? PORTD2–PORTD7
	PORTD = (PORTD & 0x03) | (a & 0xFC);
}

void inicio(char a)
{
	PORTC &= ~(1<<PORTC0);  //RS = 0, se le indica que es modo comando
	dato_a_mostrar(a);
	PORTC |= (1<<PORTC1);    // E = 1, se le indica que envie los datos
	_delay_ms(1);
	PORTC &= ~(1<<PORTC1);   // E = 0,  se le indica que se cierra el envio de datos
	_delay_ms(1);
}




void Lcd_Init8bits()
{
	PORTC &= ~(1<<PORTC0);   //RS = 0, se le indica que es modo comando
	PORTC &= ~(1<<PORTC1);   //E = 0
	_delay_ms(20);     //Peque?os delay que indica el fabricante del LCD
	inicio(0x30);     //Comando que se repite 3 veces, que indica el fabricante de la LCD
	_delay_ms(5);
	inicio(0x30);
	_delay_ms(5);
	inicio(0x30);
	_delay_ms(10);

	inicio(0x38);  //Comando que indica el fabricante del LCD, usando la matriz de 5X8
	inicio(0x0C);  //Comando que indica el fabricante del LCD, display encendido
	inicio(0x01);  //Comando que indica el fabricante del LCD, Limpiar LCD
	inicio(0x06);  //Comando que indica el fabricante del LCD, comenzar a almacenar en DDRAM
}

void Lcd_Set_Cursor(char a, char b)
{
	if(a == 0)
	inicio(0x80 + b);  //Posicionarse en la linea 1 y se suma la columna
	
	else if(a == 1)
	inicio(0xC0 + b);  //Posicionarse en la linea 2  y se suma la columna
}

void Lcd_Clear()    //Limpia la pantalla LCD
{
	inicio(1);  //Comando dado por el fabricante de la LCD
}

void Lcd_Write_Char(char data)
{
	PORTC |= (1<<PORTC0);        // RS = 1, se le indica que esta en modo caracter
	dato_a_mostrar(data);             //Escribir el caracter
	PORTC |= (1<<PORTC1);       // E = 1, enviar el dato
	_delay_ms(1);
	PORTC &= ~(1<<PORTC1);    // E = 0
	_delay_ms(1);
}


void Lcd_Write_String(char *a) //Usando punteros, ya que, estos almacenan por ejemplo *dato = hola\0, al final siempre llevan un caracter nulo
{
	int i;
	for(i=0; a[i]!='\0'; i++)   //Se recorre todo el puntero, hasta que el valor sea nulo
	Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right() //Activa el corrimiento hacia la derecha
{
	inicio(0x1C);    //Comando brindado por el fabricante de la LCD
}

void Lcd_Shift_Left()    //Activa el corrimiento hacia la izquierda
{
	inicio(0x18);  //Comando brindado por el fabricante de la LCD
}








