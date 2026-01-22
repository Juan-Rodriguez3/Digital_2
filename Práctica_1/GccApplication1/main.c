/*
 * Lab4.c
 * Author: Juan Rodriguez
 * Description: Prelab4
 */

// ===== Librerías =====
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "display.h"

// ===== Variables globales =====
volatile uint8_t state = 0;        // Estado del botón
volatile uint8_t ovf_count = 0;
volatile uint8_t display = 5;
volatile uint8_t contador1=0;
volatile uint8_t contador2=0;
uint8_t winner=0;
// ===== Prototipos =====
void setup(void);
void setup_timer0(void);

// Interrupciones
// ===== ISRs =====
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 12;          // Recargar timer

	ovf_count++;

	if (ovf_count >= 4) // 4 overflows = 1 segundo
	{
		ovf_count = 0;
		if (state==1)
		{
			display--;
		}
	}
}

ISR(PCINT0_vect)
{
	if (state==0){
		if (!(PINB & (1 << PINB4)))   // Botón presionado
		{
			state=1; //Estado donde inicia la cuenta regresiva
		}
	}
	else if (state==1){
		//Reiniciar el contador de segundos mientras aun no llegue a cero
		if (display>0){
			if (!(PINB & (1 << PINB4)))   // Botón presionado
			{
				state=1; //Estado donde inicia la cuenta regresiva
				 
				
			}
		}
	}
	
	else if (state==3 || state==4){ //este es para reiniciar el juego una vez ya acabado
		if (!(PINB & (1 << PINB4)))   // Botón presionado
		{
			state=0; //Estado donde inicia la cuenta regresiva
		}
	}
	
}

ISR(PCINT1_vect)
{
	if (state==2){
		if (!(PINC & (1 << PINC4)))
		{
			contador1++; // Acción cuando A4 se presiona
		}

		if (!(PINC & (1 << PINC5)))
		{
			contador2++; // Acción cuando A5 se presiona
		}
	}
}

// ===== Subrutinas =====
void setup(void)
{
	cli();

	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);          // Prescaler CPU = 16
	//Puerto B
	DDRB |= (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3); //Salidas
	DDRB &= ~((1 << PORTB4)|(1<<PORTB5));
	PORTB |= (1 << PORTB4)|(1<<PORTB5);         // Pull-up botón
	PORTB &= ~((1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3));
	
	PCICR  |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT4)|(1<<PCINT5);

	//Puerto C
	DDRC &= ~((1 << DDC4) | (1 << DDC5));   // A4 y A5 como entrada
	DDRC |= (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC2)|(1<<PORTC3); //Salidas
	PORTC |= (1 << PORTC4) | (1 << PORTC5); // Pull-up interno
	PORTC &= ~((1<<PORTC0)|(1<<PORTC1)|(1<<PORTC2)|(1<<PORTC3));
	PCICR |= (1 << PCIE1);   // Habilita PCINT[14:8] (Puerto C)
	PCMSK1 |= (1 << PCINT12) | (1 << PCINT13);

	
	//Puerto D
	DDRD = 0xFF;                    // Display en PORTD
	PORTD = 0x00;
	UCSR0B = 0x00;					//Apagar comunicación serial
	setup_timer0();

	sei();
}

// ===== Timer0 =====
void setup_timer0(void)
{
	TCCR0A = 0x00;                         // Modo NORMAL
	TCCR0B = (1 << CS02) | (1 << CS00);    // Prescaler 1024
	TCNT0  = 12;                           // Precarga
	TIMSK0 |= (1 << TOIE0);                // Interrupción overflow
}

// ===== Función principal =====
int main(void)
{
    setup();

    display7seg_init(&PORTD);      // Inicializar display con puntero
    display7seg_write(display);    // Mostrar valor inicial
	
    while (1)
    {
		switch (state){
			case 0:
			//Reinicio de juego
				display=5;
				display7seg_write(display);
				contador1=0;
				contador2=0;
				PORTC &= ~0x0F;
				PORTB &= ~0x0F;
				
			case 1:
			//Estado de conteo
				display7seg_write(display);   // Mostrar valor actualizado
				if (display == 0)
				{
					display7seg_write(display); //Por algún motivo no se actualiza el cero alla arriba.
					state = 2;    // El contador llegó a cero ? inicia la carrera
				}
				break;

			case 2:
				/*
				0000 ? 0 décadas
				0001 ? 1 década
				0010 ? 2 décadas
				0100 ? 3 décadas
				1000 ? 4 décadas
				*/

				// ----- Jugador 1 (PORTB) -----
				if (contador1 == 0)
				{
					PORTB &= ~0x0F;   // LEDs apagados
				}
				else
				{
					PORTB = (PORTB & 0xF0) | (1 << (contador1 - 1)); //avanza o incrementa una decada
					if (contador1 == 4)
						state = 3;   // Jugador 1 gana
				}

				// ----- Jugador 2 (PORTC) -----
				if (contador2 == 0)
				{
					PORTC &= ~0x0F;   // LEDs apagados
				}
				else
				{
					PORTC = (PORTC & 0xF0) | (1 << (contador2 - 1)); //Avanza una decada
					if (contador2 == 4)
						state = 4;   // Jugador 2 gana
				}
				break;

			case 3:
				PORTC &= ~0x0F;   // Apagar LEDs del perdedor
				PORTB |= 0x0F;    // Encender LEDs del ganador
				winner = 1;		//Para desplegar el ganador en el display
				display7seg_write(winner);
				break;

			case 4:
				PORTB &= ~0x0F;   // Apagar LEDs del perdedor
				PORTC |= 0x0F;    // Encender LEDs del ganador
				winner = 2;			
				display7seg_write(winner);	//Para desplegar el ganador en el display
				break;

			default:
				state = 0;        // Estado seguro
				break;
		}
	}
}



