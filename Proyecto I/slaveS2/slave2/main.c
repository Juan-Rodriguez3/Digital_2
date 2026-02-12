/*
 * slave2.c
 *
 * Created: 5/02/2026 19:39:57
 * Author : laloj
 */ 


#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "I2C_Libraries/I2C.h"
//C:\Users\juana\Downloads\slave_S\slave2\slave2\main.c
#define S2_Address 0x40

uint8_t buffer = 0;
uint8_t descartar = 1;
volatile uint8_t ADCUno = 0;
char string_buffer[6];
void initADC();
void setup();
void initUART();
void adc_a_string(uint8_t adc, char *str);
void wChar(char character);
void wStr(char* strng);



int main(void)
{
    setup();
    while (1) 
    {
		if (buffer == 'W')
		{
			PINC |= (1<<PINC2);
			buffer = 0;
		}
		//adc_a_string(ADCUno, string_buffer);
		//wStr(string_buffer);
		//wChar(TWDR);
		_delay_ms(100);
    }
}

void setup()
{
	cli();
	DDRC |= (1<<DDC2);
	PORTC &= ~(1<<PORTC2);
	initADC();
	I2C_init_Slave(S2_Address);
	initUART();
	
	sei();
}

void initADC()
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0); //referencia = avcc
	ADMUX |= (1 << ADLAR);
	ADMUX |= (1<<MUX2)|(1<<MUX1);
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADPS1) | (1 << ADPS1) | (1 << ADPS0);
	
	ADCSRA |= (1 << ADSC);
}

void initUART()
{
	DDRD |= (1 << 1);
	DDRD &= ~(1 << 0);
	
	UCSR0A = 0;
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	UBRR0 = 103; // BAUD 9600
}

void adc_a_string(uint8_t adc, char *str)
{
	uint16_t vCom = (adc * 500UL) / 255;
	uint8_t vEnt = vCom / 100;
	uint8_t vDec = vCom % 100;
	
	str[0] = vEnt + '0';
	str[1] = '.';
	str[2] = (vDec / 10) + '0';
	str[3] = (vDec % 10) + '0';
	str[4] = 'V';
	str[5] = '\0';
}

void wChar(char character)
{
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = character;
}

void wStr(char* strng)
{
	for (uint8_t i = 0; *(strng+i) != '\0'; i++) // (strng+i) <- direccion de un character | *(strng+i) <- lo que esta contenido dentro de la direcion, es decir el character
	{
		wChar(*(strng+i));
	}
}


ISR(ADC_vect)
{
	
	if (descartar)
	{
		descartar = 0;
		ADCSRA |= (1 << ADSC);
		return;
	}
	descartar = 1;
	ADCUno = ADCH;
	ADCSRA |= (1 << ADSC);
}

ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC; // Nos quedamos unicamente con los bits de estado TWI Status
	switch(estado){
		/*******************************/
		// Slave debe recibir dato
		/*******************************/
		case 0x60: // SLA+W recibido
		//wStr("SLA+W recibido");
		//break;
		case 0x70: // General Call
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("General Call");
		break;

		case 0x80: // Dato recibido, ACK enviado
		//wStr("Dato recibido, ACK enviado");
		//break;
		case 0x90: // Dato recibido General Call, ACK enviado
		buffer = TWDR;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("Dato recibido General Call, ACK enviado");
		break;

		/*******************************/
		// Slave debe transmitir dato
		/*******************************/
		case 0xA8: // SLA+R recibido
		//wStr("SLA+R recibido");
		//break;
		case 0xB8: // Dato transmitido, ACK recibido
		TWDR = ADCUno;   // Dato a enviar
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("Dato transmitido, ACK recibido");
		break;
		
		case 0xC0:
		case 0xC8: // Ultimo dato transmitido
		TWCR = 0;
		TWCR = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
		//wStr("Ultimo dato transmitido");
		break;

		case 0xA0: // STOP o repeated START recibido
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("STOP o repeated START recibido");
		break;
		
		default:
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("default");
		break;
	}
}	

