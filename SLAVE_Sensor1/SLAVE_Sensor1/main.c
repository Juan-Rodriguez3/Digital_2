/*
 * SLAVE_Sensor1.c
 *
 * Created: 2/5/2026 7:45:10 PM
 * Author : juana
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ADC_Libraries/ADC.h"
#include "UART_Libraries/UART_Library.h"


#define  Vref_5V 5
#define ON 1
#define OFF 0
#define prescaler_ADC 128
#define comparador 2
#define SlaveAddress 0x30 

/*** Variables globales ***/
volatile uint8_t canal_ADC=7;	// =0 --> D10/OC1B - !=0 --> D9/OC1A
volatile uint8_t valorADC = 0;	//Lectura del adc
uint8_t buffer =0;

void setup();

int main(void)
{
	setup();
    /* Replace with your application code */
    while (1) 
    {
		valorADC = ADCH;
		writeString("POT1: ");
		UART_sendUint8(valorADC);
		writeString("\n");
    }
}


void setup() {
	cli();
	
	
	initUART_9600();
	ADC_init(ON, Vref_5V,canal_ADC,ON,prescaler_ADC);
	I2C_init_Slave(SlaveAddress);
	
	sei();
	}

//Subrutinas de interrupcion
ISR(ADC_vect){
	
	ADC_init(ON, Vref_5V,canal_ADC,ON,prescaler_ADC);
}