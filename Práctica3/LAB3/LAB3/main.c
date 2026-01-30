/* LAB3.c
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
uint8_t imprimir=0;

//Prototipos
void refresh_PORT(uint8_t bus_data);
void setup();

//******* rutina principal ******//

int main(void)
{
    /* Replace with your application code */
	setup();
	SPI_write('a');		//le pido al esclavo que me mando el dato del POT1
	//UART_sendUint8(entero); Prueba de libreria
    while (1) 
    {
		if(imprimir==1){
			writeString("POT1: ");
			UART_sendUint8(POT1); //Escribir en la terminal el valor del POT1
			writeString("       ");
			SPI_write('a');	//Pedir el valor del pot2
			imprimir=0;		//Esperar dato para volver a imprimir
		}
		else if (imprimir==2){
			writeString("POT2: ");
			UART_sendUint8(POT2); //Escribir en la terminal el valor del POT1
			writeString("\n");
			SPI_write('b');	//Volver a pedir el valor del POT1
			imprimir=0;		//Esperar dato para volver a imprimir
		}
		
		
		_delay_ms(100);
    }
}

//******* Subrutinas NO-INterrupt ******//7

//Rutina para inicializar o confugurar el ATmega
void setup(){
	cli();			//Deshabilitar interrupciones
	
	initUART_9600();
	SPI_init(1,0,1,0b00000010);
	
	
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


