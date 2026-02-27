/*
 * LAB3.c
 *
 * Created: 1/28/2026 11:21:06 PM
 * Juan Rodriguez - 221593
 * Gerardo Lopez - 
 * Programa para SPI
 */ 

/*
	Para este laboratorio se utilizo el siguiente Pinout 
	Master:
	LED0-LED1 --> PB0-PB1
	LED2-LED7 --> PD2-PD7
	Slave:
	POT1,POT2 --> PC6-PC7
	LED0-LED1 --> PB0-PB1
	LED2-LED7 --> PD2-PD7	
*/

//Definiciones y librerias
#define F_CPU 16000000UL
#define MAX_LEN 3
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "SPI_Library/SPI.h"
#include "UART_library/UART_Library.h"

//Variables
volatile uint8_t POTX=0;
volatile uint8_t POT1=0;
volatile uint8_t POT2=0;
volatile uint8_t imprimir=0;

//variables para parte 2
volatile uint8_t length   =0;
volatile uint8_t caracter =0;;
volatile uint8_t flag_str=0;
volatile char string_recieved[MAX_LEN + 1]; // +1 para '\0'

uint8_t dato_aprrobed=0;
char str_save[MAX_LEN+1];
uint8_t str_approbed[MAX_LEN+1];
uint8_t str_to_int=0;
//Prototipos
void refresh_PORT(uint8_t bus_data);
void setup();

//******* rutina principal ******//

int main(void)
{
    /* Replace with your application code */
	setup();
	SPI_write('a');		//le pido al esclavo que me mando el dato del POT1
	SPI_write('H');
	//UART_sendUint8(entero); Prueba de libreria
    while (1) 
    {
		if (flag_str) {
			flag_str = 0;
			
			for (uint8_t i = 0; i < MAX_LEN+1; i++) {
				str_save[i] = string_recieved[i];
			}
			
			dato_aprrobed= str_to_uint8(str_save, str_approbed);
			if (dato_aprrobed==1){
				
				/*writeString("Cadena completa: ");
				writeString(str_save);
				writeString("\n");
				*/
				refresh_PORT(*str_approbed);
				
				SPI_write('c');
				_delay_ms(1);
				SPI_write(*str_approbed);
				length = 0;
			}
		}
		
		if(imprimir==1){
			writeString("POT1: ");
			UART_sendUint8(POT2); //Escribir en la terminal el valor del POT1
			writeString("       ");
			SPI_write('b');	//Pedir el valor del pot2
			imprimir=0;		//Esperar dato para volver a imprimir
		}
		else if (imprimir==2){
			writeString("POT2: ");
			UART_sendUint8(POT1); //Escribir en la terminal el valor del POT1
			writeString("\n");
			SPI_write('a');	//Volver a pedir el valor del POT1
			imprimir=0;		//Esperar dato para volver a imprimir
		}
		
		_delay_ms(10);
	}
}

//******* Subrutinas NO-INterrupt ******//7

//Rutina para inicializar o confugurar el ATmega
void setup(){
	cli();			//Deshabilitar interrupciones
	
	initUART_9600();
	SPI_init(1,0,1,0b00000010);
	
	//Puerto D
	DDRD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7);		//Salidas
	PORTD &= ~((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7));
	
	//PUERTO B
	DDRB |= (1<<PORTB0)|(1<<PORTB1);
	PORTB &= ~((1<<PORTB0)|(1<<PORTB1));
	
	
	sei();			//Habilitar interrupciones
}

//Rutina para distribuir los datos de salida entre puertos
void refresh_PORT(uint8_t bus_data){	
	// Bits D0 y D1 --> PORTB0 y PORTB1
	PORTB = (PORTB & 0xFC) | (bus_data & 0x03);

	// Bits D2?D7 ? PORTD2?PORTD7
	PORTD = (PORTD & 0x03) | (bus_data & 0xFC);
}


//******* Subrutinas INterrupt ******//
ISR(SPI_STC_vect){
	if (POTX==0){
		POT1=SPDR;		//En el primer ciclo se va recibir el pot1
		POTX=1;			//Cambiar de pot
		imprimir=1;		//Imprimir el valor del pot1
	}
	else if (POTX==1){
		POT2=SPDR;		//En el segundo ciclo se va recibir el pot2
		POTX=0;			//cambiar de pot
		imprimir=2;		//Imprimir el valor del pot2
	}
}

ISR(USART_RX_vect){
	caracter = UDR0;	// Leer caracter enviado desde la terminal
	// Si ya hay una cadena lista, ignorar nuevos datos
	if (flag_str) return;
	
	// Guardar carácter si hay espacio
	if (length<MAX_LEN) {
		string_recieved[length] = caracter;
		length++;
	}
	
	// Detectar delimitadores: ENTER o espacio
	if (length==MAX_LEN) {
		string_recieved[length] = '\0';   // terminar string
		flag_str = 1;        // avisar al main
		length = 0;              // preparar siguiente cadena
		return;
	}
}

