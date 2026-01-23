/*
 * Lab2.c
 *
 * Created: 1/22/2026 4:12:06 PM
 * Author : juana
 */ 


//PINOUT
/*
	RS-->PORTC0
	RW-->GND
	E--> PORTC1
	D0-D7 --> PORD0-PORTD7
*/

//Librerías y definciones
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "ADC_Library/ADC.h"

#define  Vref_5V 5
#define ON 1
#define OFF 0

#define prescaler_ADC 128

#define periodo 39999
#define comparador 2

/*** Variables globales ***/
volatile uint8_t canal_ADC=0;	// =0 --> D10/OC1B - !=0 --> D9/OC1A
volatile uint8_t valorADC = 0;	//Lectura del adc
volatile uint8_t POT1=0;
volatile uint8_t POT2=0;


/*** Prototipos ***/
void setup();

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}




/***Subrutinas NON-Interrupt***/

void setup(){
	 cli();
	   UCSR0B = 0;                //Comunicación serial
	   
	   //Puerto D
	   
	   DDRD = 0xFF;		//Salidas
	   PORTD =0x00;		//
	   
	   //Puerto C
	   DDRC |= (1<<PORTC0)|(1<<PORTC1);		//Salidas
	   PORTC &= ~((1<<PORTC0)|(1<<PORTC1));
	   
	   ADC_init(ON, Vref_5V,canal_ADC,ON,prescaler_ADC);
	   
	   
}


/***Subrutinas Interrupt***/
ISR(ADC_vect){
	
	//Guardamos el valor de ADC
	valorADC = ADCH;        // Leemos solo ADCH por justificación izquierda
	
	//Actualizamos el DutyCycle dependiendo de que canal se haya leido
	switch(canal_ADC){
		case 0:
		 POT1 =valorADC;    //Guardamos el valor del pot1
		break;
		case 1:
		POT2=valorADC;		//Guardamos el valor del pot2
		break;
		default:
		break;
	}
	
	//Multiplexeo de canales de ADC para la proxuma lectura.
	if (canal_ADC>=1){
		canal_ADC=0;
	}
	else {
		canal_ADC++;	//pasamos al siguiente canal
	}
	
	//Reconfiguracion del ADC
	ADC_init(ON, Vref_5V,canal_ADC,ON,prescaler_ADC);
}