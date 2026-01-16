/*
 * Lab4.c
 * Author: Juan Rodriguez
 * Description: Prelab4
 */
//Librerias y variables
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
volatile uint8_t pb=0;			//Estados de botones
volatile uint8_t ovf_count = 0;
volatile uint8_t puntero=0;

//Prototipos
void setup();


//Funcion principal
int main()
{
    setup();
    while (1) 
    {
		if (pb==1){
			PORTB |= (1<<PORTB0);
		}
		else {
			PORTB &= ~(1<<PORTB0);
		}
    }	
}

//Subrutinas NON Interrupt
void setup () {
	cli();				//Desactivar interrupciones globales
	CLKPR = (1<< CLKPCE);
	CLKPR |= (1<<CLKPS2);	//Configurar prescaler principal a 16
	
	DDRB |= (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3); //salidas
	DDRB &= ~(1<<PORTB4);	//entrada
	PORTB |= (1<<PORTB4);	//Pullup
	
	 PCICR  |= (1 << PCIE0);
	 PCMSK0 |= (1 << PCINT4);

	UCSR0B = 0x00;  // Desactiva transmisor y receptor
	DDRD = 0xFF;     // PD7–PD0 como salidas
	PORTD = 0x00;    // Todas las salidas en LOW

	TCCR0A = 0x00;                       // Modo NORMAL
	TCCR0B = (1<<CS02) | (1<<CS00);     // Prescaler 1024
	TIMSK0 |= (1<<TOIE0);               // Habilita overflow Timer0

	
	
	sei();			//Activar interrupciones globales
}








//Subrutinas de interrupciones


ISR(TIMER0_OVF_vect)
{
	ovf_count++;

	if (ovf_count >= 61) {
		ovf_count = 0;
		puntero++;
		// ?? AQUÍ se ejecuta cada 1 segundo
	}
}


ISR(TIMER0_OVF_vect)
{
	ovf_count++;

	if (ovf_count >= 61) {
		ovf_count = 0;
		// ?? AQUÍ se ejecuta cada 1 segundo
	}
}


ISR(PCINT0_vect){
	if (!(PINB & (1<<PINB4))){
		pb=1;
	}
	else{
		pb=0;
	}
}


