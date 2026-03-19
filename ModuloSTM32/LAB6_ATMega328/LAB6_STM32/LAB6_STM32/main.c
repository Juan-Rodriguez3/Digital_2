/*
 * LAB6_STM32.c
 *
 * Created: 3/12/2026 7:30:36 PM
 * Author : juan
 * Control de videojuego
 */ 

#include <avr/io.h>
#include "UART_library/UART_Library.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

//Variables
volatile uint8_t accion = 0;

// Prototipos
void setup();

int main(void)
{
	setup();
	
    /* Replace with your application code */
    while (1) 
    {
		if (!(PIND & (1<<PIND2)))
		{
			//accion=1; //Izquierda
			writeString("PJ2: Izquierda\r\n");
		}

		if (!(PIND & (1<<PIND3)))
		{
			//accion=2; //Derecha
			writeString("PJ2: Derecha\r\n");
		}

		if (!(PIND & (1<<PIND6)))
		{
			//accion=3; //Arriba
			writeString("PJ2: Arriba\r\n");
		}

		if (!(PIND & (1<<PIND5)))
		{
			//accion=4; //Abajo
			writeString("PJ2: Abajo\r\n");
		}
		if (!(PIND & (1<<PIND4))){
			//accion=5; //Accion A
			writeString("PJ2: Accion A\r\n");
		}
		if (!(PIND & (1<<PIND7))){
			//accion=6; //Accion B
			writeString("PJ2: Accion B\r\n");
		}
		/*
		switch(accion)
		{
			case 1:
			writeString("PJ2: Izquierda\r\n");
			accion = 0;
			break;

			case 2:
			writeString("PJ2: Derecha\r\n");
			accion = 0;
			break;

			case 3:
			writeString("PJ2: Arriba\r\n");
			accion = 0;
			break;

			case 4:
			writeString("PJ2: Abajo\r\n");
			accion = 0;
			break;

			case 5:
			writeString("PJ2: Accion A\r\n");
			accion = 0;
			break;
			
			case 6:
			writeString("PJ2: Accion B\r\n");
			accion=0;
			break;

			default:
			break;
		}
		*/
		_delay_ms(150);
		
    }
}

void setup(){
	//Deshabilitar interrupciones
	cli();
	
	initUART_9600();   // Inicializa UART a 9600
	
	// Configurar D2–D6 como entradas
	DDRD &= ~((1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7));  // Entrada
	PORTD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6)|(1<<PORTD7); // Pull-up ON
	/*
	// Habilitar grupo PCINT2 (PORTD)
	PCICR |= (1<<PCIE2);

	// Seleccionar pines D2–D6
	PCMSK2 |= (1<<PCINT18) |
	(1<<PCINT19) |
	(1<<PCINT20) |
	(1<<PCINT21) |
	(1<<PCINT22) |
	(1<<PCINT23);
	*/
	
	// Habilitar interrupciones globales
	sei();
}

/*
ISR(PCINT2_vect)
{
	if (!(PIND & (1<<PIND2)))
	{
		accion=1; //Izquierda
	}

	if (!(PIND & (1<<PIND3)))
	{
		accion=2; //Derecha
	}

	if (!(PIND & (1<<PIND6)))
	{
		accion=3; //Arriba
	}

	if (!(PIND & (1<<PIND5)))
	{
		accion=4; //Abajo
	}
	if (!(PIND & (1<<PIND4))){
		accion=5; //Accion A
	}
	if (!(PIND & (1<<PIND7))){
		accion=6; //Accion B
	}
}*/