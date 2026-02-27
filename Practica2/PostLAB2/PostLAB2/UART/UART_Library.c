#include "UART_Library.h"
				
//Envía un solo carácter por el puerto serial (UART).
void write(char texto){
	while ((UCSR0A & (1<<UDRIE0))==0);	//Esperamos a que el registro de datos de UART este vacio.
	UDR0= texto;						//se envia el dato.
}

//envia una cadena de texto completa a la terminal, caracter por caracter por UART 
void writeString(char* texto){
	for(uint8_t i = 0; *(texto+i) !='\0'; i++)		//
	write(*(texto+i));								//
}


//Inicializa el USART a 9600 baudios, 8 bits, sin paridad, 1 bit de parada.
void initUART_9600(){
	//Configurar los pines PD1 Tx y PD0 Rx
	DDRD |= (1<<PORTD1);
	DDRD &= ~(1<<PORTD0);
	UCSR0A = 0;		//No se utiliza doble speed.
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);  //Habilitamos interrupciones al recibir, habilitar recepcion y transmision
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);	//Asincrono, deshabilitado el bit de paridad, un stop bit, 8 bits de datos.
	UBRR0=103;	//UBBRR0=103; -> 9600 con frecuencia de 16MHz
}