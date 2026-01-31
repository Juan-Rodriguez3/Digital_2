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
volatile uint8_t length=0;
volatile uint8_t caracter=0;
volatile uint8_t flag_char=0;
uint8_t dato_aprrobed=0;
volatile uint8_t status=0;

volatile uint8_t flag_str=0;
//volatile char string_recieved[MAX_LEN + 1]; // +1 para '\0'
char str_save[MAX_LEN+1];


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
	//UART_sendUint8(entero); Prueba de libreria
    while (1) 
    {
		
		
		switch(status){
			case 0:
			writeString("Estado cero");
			//Los simbolos '+' y '-' sirviran para movernos entre estados
			if (caracter=='+'){
				status=1;	
				writeString("Cambio a estado 1");	
			}else if (caracter=='-'){
				status=2;
				writeString("Cambio a estado 2");	
			}
			
			break;
			
			//Modo de contador de datos
			case 1:
			//Los simbolos '+' y '-' sirviran para movernos entre estados
			if (caracter=='+'){
				status=2;
			}else if (caracter=='-'){
				status=0;
			}
			
			if (flag_char==1){
				flag_char=0;
				
				if (caracter=='.'){
					flag_str=1;
					writeString("cadena guardada");
				} else {
					str_save[length]=caracter;
				}
				
			}
			
			if (flag_str==1){
				flag_str=0;
				writeString(str_save);
				writeString("\n");
				
				dato_aprrobed= str_to_uint8(str_save,MAX_LEN, &str_to_int);
				
			}
			
			
			if (dato_aprrobed==1){
				dato_aprrobed = 0;
				refresh_PORT(str_to_int);//Imprimir en los leds
			}
			
			break;
			
			//Modo que muestra los potenciometros
			case 2:
			//Los simbolos '+' y '-' sirviran para movernos entre estados
			if (caracter=='+'){
				status=0;
			}else if (caracter=='-'){
				status=1;
			}
			
				/*
			if(imprimir==1){
				writeString("POT1: ");
				UART_sendUint8(POT1); //Escribir en la terminal el valor del POT1
				writeString("       ");
				SPI_write('b');	//Pedir el valor del pot2
				imprimir=0;		//Esperar dato para volver a imprimir
			}
			else if (imprimir==2){
				writeString("POT2: ");
				UART_sendUint8(POT2); //Escribir en la terminal el valor del POT1
				writeString("\n");
				SPI_write('a');	//Volver a pedir el valor del POT1
				imprimir=0;		//Esperar dato para volver a imprimir
			}
			*/
			break;
			
			default:
			break;
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
	flag_char=1;
	length++;			//será el indice este contara la cantidad de elementos de la cadena
}

