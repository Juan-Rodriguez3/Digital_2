#include "LCD8bits.h"

void inicio(char a)
{
	PORTC &= ~(1<<PORTC0);  //RS = 0, se le indica que es modo comando
	dato_a_mostrar(a);
	PORTC |= (1<<PORTC1);    // E = 1, se le indica que envie los datos
	_delay_ms(1);
	PORTC &= ~(1<<PORTC1);   // E = 0,  se le indica que se cierra el envio de datos
	_delay_ms(1);
}

void dato_a_mostrar(char a)
{
	PORTD = a;
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










