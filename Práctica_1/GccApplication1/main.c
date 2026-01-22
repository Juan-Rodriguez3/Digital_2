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

// ===== Función principal =====
int main(void)
{
    setup();

    display7seg_init(&PORTD);      // Inicializar display con puntero
    display7seg_write(display);    // Mostrar valor inicial
	
    while (1)
    {
		if (state<=1){
			display7seg_write(display);   // Mostrar valor actualizado
			if (display==0){
				display7seg_write(display);
				state=2;	//El contador llego a cero inicia la carrera.
			}
		}
		else if (state==2){
			/*
			0000 ? 0 décadas
			0001 ? 1 década
			0010 ? 2 décadas
			0100 ? 3 décadas
			1000 ? 4 décadas
			*/
			if (contador1==0)
			{
				PORTB &= ~0x0F; //Al iniciar las leds estan apagadas
			}
			else if (contador1>0){
				PORTB = (PORTB & 0xF0) | (1 << (contador1- 1)); // por cada pulso se van sumando decadas al contador.
				if (contador1==4){
					state=3; //Jugador 1 gano
					PORTB = (PORTB & 0xF0) | (1 << (contador1- 1));
				}
			}
			
			if (contador1==0)
			{
				PORTB &= ~0x0F; //Al iniciar las leds estan apagadas
			}
			else if (contador2>0){
				PORTC = (PORTC & 0xF0) | (1 << (contador2- 1)); // por cada pulso se van sumando decadas al contador.
				if (contador2==4){
					PORTC = (PORTC & 0xF0) | (1 << (contador2- 1));
					state=4; //Jugador 2 gano
				}	
			}
			
		}
		else if (state==3){
			PORTC &= ~0x0F; //Apagar las luces de quien perdio
			PORTB |= 0x0F; //Encender las luces de quien gano
			winner=1;
			display7seg_write(winner);
		}
		else if (state==4){
			PORTB &= ~0x0F; //Apagar las luces de quien perdio
			PORTC |= 0x0F; //Encender las luces de quien gano
			winner=2;
			display7seg_write(winner);
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
				display=5;
			}
		}
	}
	
	else if (state==5){ //este es para reiniciar el juego una vez ya acabado
		if (!(PINB & (1 << PINB4)))   // Botón presionado
		{
			state=1; //Estado donde inicia la cuenta regresiva
		}
	}
	
}

ISR(PCINT1_vect)
{
	if (!(PINC & (1 << PINC4)))
	{
		contador1++; // Acción cuando A4 se presiona
	}

	if (!(PINC & (1 << PINC5)))
	{
		contador2++; // Acción cuando A5 se presiona
	}
}	