/*
 * Proyecto_red_de_sensores.c
 * Casa inteligente
 * Created: 2/5/2026 12:05:06 AM
 * Juan Rodriguez - 221593
 * Gerardo Lopez -
 *	Digital 2 - Seccion 11
 */ 
//Master

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2C_Libraries/I2C.h"
#include "ADC_Libraries/ADC.h"
#include "UART_libraries/UART_Library.h"
#include "LCD_Libraries/LCD8bits.h"
#define slave1R (0x30<<1)|0x01
#define slave1W (0x30<<1)& 0b11111110


uint8_t direccion;
uint8_t temp;
uint8_t bufferI2C=0;


void refresh_PORT(uint8_t bus_data);
void setup();


int main(void)
{
    /* Replace with your application code */
	
	setup();
    while (1) 
    {
		writeString("Inicio de comunicacion\n");
		
		if(!I2C_Start()) continue;
		
		
		if (!I2C_write(slave1W)){
			I2C_stop();
			writeString("Fallo al iniciar\n");
			continue;
		}
		
		if(!I2C_write('R')){
			I2C_stop();
			writeString("Fallo al enviar\n");
			continue;
		}
		
		if (!I2C_repeatedStart()){
			I2C_stop();
			writeString("Fallo al reptir\n");
			continue;
		}
		
		if(!I2C_write(slave1R)){
			I2C_stop();
			writeString("fallo al escribir\n");
			continue;
		}
		
		I2C_read(&bufferI2C, 0);	//ACK		
		I2C_stop();
		
		writeString("No hay fallo\n");
		refresh_PORT(bufferI2C);
		
		
		_delay_ms(100);
		
    }
}

//Rutina para distribuir los datos de salida entre puertos
void refresh_PORT(uint8_t bus_data){
	// Bits D0 y D1 --> PORTB0 y PORTB1
	PORTB = (PORTB & 0xFC) | (bus_data & 0x03);

	// Bits D2?D7 ? PORTD2?PORTD7
	PORTD = (PORTD & 0x03) | (bus_data & 0xFC);
}

void setup(){
	cli();
	//Puerto D
	DDRD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7);		//Salidas
	PORTD &= ~((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7));
	
	//PUERTO B
	DDRB |= (1<<PORTB0)|(1<<PORTB1);
	PORTB &= ~((1<<PORTB0)|(1<<PORTB1));
	
	I2C_init_Master(1, 100000);
	initUART_9600();
	sei();
}
