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

//Libreriías y definiciones
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ADC_Library/ADC.h"
#include "LCD/LCD8bits.h"
	
#define  Vref_5V 5
#define ON 1
#define OFF 0

#define prescaler_ADC 128
#define comparador 2

/*** Variables globales ***/
volatile uint8_t canal_ADC=3;	// =0 --> D10/OC1B - !=0 --> D9/OC1A
volatile uint8_t valorADC = 0;	//Lectura del adc
volatile uint8_t POT1, prePOT1=0;
volatile uint8_t POT2=0;

char lista1[8] = {'0', '0', '0', '0'};


/*** Prototipos ***/
void setup();
void actualizarVoltaje(char *lista, uint8_t valor);
void actualizarLCD(void);
void actualizarLista(char *lista, int valor);

int main(void)
{
	setup();
    /* Replace with your application code */
	
	 // Posicionar cursor y escribir
	 Lcd_Set_Cursor(0, 0);            // Línea 1, columna 0
	 Lcd_Write_String("Hola");
    while (1) 
    {
		
		actualizarVoltaje(lista1,POT1);
		actualizarLista(lista1,POT1);
		actualizarLCD();
		_delay_ms(100);   // ?? delay al final del ciclo
		
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
	   
	   //Inicializar el LCD
	   Lcd_Init8bits();
	   Lcd_Clear();
	   
	   //Inicializar el ADC
	   ADC_init(ON, Vref_5V,canal_ADC,ON,prescaler_ADC);
	   sei();
	   
}

void actualizarVoltaje(char *lista, uint8_t valor) {
	float voltaje = (valor * 5.0) / 255.0;
	uint16_t int_part = (uint16_t)voltaje;
	uint16_t dec_part = (uint16_t)((voltaje - int_part) * 100);  // Dos decimales

	if (int_part < 10) {
		lista[0] = '0' + int_part;
		lista[1] = '.';
		lista[2] = '0' + (dec_part / 10);
		lista[3] = '0' + (dec_part % 10);
		lista[4] = 'V';
		lista[5] = '\0';
		} else {
		lista[0] = '0' + (int_part / 10);
		lista[1] = '0' + (int_part % 10);
		lista[2] = '.';
		lista[3] = '0' + (dec_part / 10);
		lista[4] = '0' + (dec_part % 10);
		lista[5] = 'V';
		lista[6] = '\0';
	}
}

void actualizarLista(char *lista, int valor) {
	lista[0] = '0' + (valor / 100);
	lista[1] = '0' + ((valor / 10) % 10);
	lista[2] = '0' + (valor % 10);
	lista[3] = '\0';
}

void actualizarLCD(void) {
	Lcd_Clear();  // Limpiar pantalla
	Lcd_Set_Cursor(0, 1);
	Lcd_Write_String("S1:");  // Escribir etiqueta de Sensor 1
	
	// Actualizar las cadenas con los valores actuales
	actualizarVoltaje(lista1, POT1);

	// Mostrar los valores en la LCD
	Lcd_Set_Cursor(1, 1);
	Lcd_Write_String(lista1);;
}


/***Subrutinas Interrupt***/
ISR(ADC_vect){
	
	//Guardamos el valor de ADC
	valorADC = ADCH;        // Leemos solo ADCH por justificación izquierda
	
	//Actualizamos el DutyCycle dependiendo de que canal se haya leido
	switch(canal_ADC){
		case 3:   // ADC3 = PC3
		POT1 = valorADC;
		break;
		case 4:   // si luego usas otro pot
		POT2 = valorADC;
		break;
		default:
		break;
	}

	
	//Multiplexeo de canales de ADC para la proxuma lectura.
	if (canal_ADC>=4){
		canal_ADC=3;
	}
	else {
		canal_ADC++;	//pasamos al siguiente canal
	}
	
	//Reconfiguracion del ADC
	ADC_init(ON, Vref_5V,canal_ADC,ON,prescaler_ADC);
}