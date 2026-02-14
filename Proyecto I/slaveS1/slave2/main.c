/*
 * slave2.c
 *
 * Created: 5/02/2026 19:39:57
 * Author : laloj
 */ 
//C:\Users\juana\OneDrive\Documentos\GitHub\Digital_2\Proyecto I\slaveS1\slave2\main.c

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include "I2C_Libraries/I2C.h"

#define S2_Address 0x30

#define TIME_MAX 25000UL //microsegundos 

uint8_t buffer = 0;
uint8_t descartar = 1;
volatile uint8_t ADCUno = 0;
char string_buffer[6];

//Variables para sennsor 
#define TIMEOUT_TICKS 60000

volatile uint16_t start_time = 0;
volatile uint16_t pulse_width = 0;
volatile uint8_t sensor_state = 0;
volatile uint8_t distancia_map = 0;
static uint8_t ready_to_trigger = 1;

//Funcion de 
//void initADC();
void setup();
void initUART();
void adc_a_string(uint8_t adc, char *str);
void wChar(char character);
void wStr(char* strng);
void Ultrasonico_Trigger(void);



int main(void)
{
    setup();
	
	
    while (1)
    {
		if (buffer == 'R')
		{
			PIND |= (1<<PIND2);
			buffer = 0;
		}
		
		// ---- DISPARO CONTROLADO CADA ~60 ms ----
		 // Disparar si está libre
		 if (sensor_state == 0 && ready_to_trigger && !(PINC & (1 << PC2)))
		 {
			 Ultrasonico_Trigger();
			 wStr("Lanzando trigger\n");
		 }
		else if (sensor_state==1){
			wStr("Trigger recibido ----> ");
			wStr("Esperando pulso de bajada\n");
		}
		else if (sensor_state==2){
			wStr("Lectura del sensor terminada\n");
			
			ready_to_trigger = 0;
			_delay_ms(60);
			sensor_state = 0;
			ready_to_trigger = 1;
			
			wStr("Volver a leer\n");
		}
		
		 
    }
}



void setup()
{
	cli();
	DDRD |= (1<<DDD2)|(1<<PORTD7);
	PORTD &= ~((1<<PORTD2)|(1<<PORTD7));
	
//*******Configuracion del sensor*******//	

	DDRC |= (1 << PC1);      // Trigger output
	DDRC &= ~(1 << PC2);     // Echo input

	PORTC &= ~(1 << PC1);

	TCCR1A = 0;
	TCCR1B = (1 << CS11);    // prescaler 8

	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT10);	

	sensor_state = 0;
	 
//*******Configuracion del sensor*******//		
	
	//initADC();
	I2C_init_Slave(S2_Address);
	initUART();
	
	sei();
}

void Ultrasonico_Trigger(void)
{
    PORTC &= ~(1<<PC1);
	_delay_us(10);
	
	
    PORTC |= (1 << PC1);
    _delay_us(10);
    PORTC &= ~(1 << PC1);
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


ISR(PCINT1_vect)
{
	 // FLANCO SUBIDA
	 if ((PINC & (1 << PC2)) && (sensor_state == 0))
	 {
		 sensor_state = 1;
	 }
	 // FLANCO BAJADA
	 else if (!(PINC & (1 << PC2)) && (sensor_state == 1))
	 {
		 sensor_state = 2;
	 }

}
ISR(TWI_vect){
	uint8_t estado = TWSR & 0xFC; // Nos quedamos unicamente con los bits de estado TWI Status
	switch(estado){
		/*******************************/
		// Slave debe recibir dato
		/*******************************/
		case 0x60:
		case 0x70: // General Call
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("General Call");
		break;

		case 0x80:
		case 0x90: // Dato recibido General Call, ACK enviado
		buffer = TWDR;
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
		//wStr("Dato recibido General Call, ACK enviado");
		break;

		/*******************************/
		// Slave debe transmitir dato
		/*******************************/
		case 0xA8:
		case 0xB8: // Dato transmitido, ACK recibido
		TWDR = distancia_map;   // Dato a enviar
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
		

