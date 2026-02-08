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
#define slave2R (0x40<<1)|0x01
#define slave2W (0x40<<1)& 0b11111110



uint8_t lectura_S1=0;
uint8_t lectura_S2=0;

/********* Listas para la función de LCD *********/
char lista1[8] = {'0', '0', '0', '0'};
char lista2[8] = {'0', '0', '0', '0'};
char lista3[8] = {'0', '0', '0', '0'};

/********* Prototipos *********/
void refresh_PORT(uint8_t bus_data);
void setup();
void actualizarLCD();
void actualizar_datos_slave(uint8_t addressW, uint8_t addressR , char comando, uint8_t sensor);
void actualizarS1(char *lista, uint8_t dato);

/********* Funcion principal *********/
int main(void)
{
    /* Replace with your application code */
	
	setup();
	
	Lcd_Clear();
	
    while (1) 
    {
		actualizar_datos_slave(slave1W, slave1R,'R',0);
		//actualizar_datos_slave(slave2W, slave2R , 'W', 1);
		actualizarLCD();
		_delay_ms(50);
		
    }
}

/********* Subrutinas *********/
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
	DDRB |= (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3);
	PORTB &= ~((1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2)|(1<<PORTB3));

	
	Lcd_Init8bits();
	I2C_init_Master(1, 100000);
	initUART_9600();
	
	Lcd_Clear();
	
	sei();
}

//Funcion para comunicarse con el esclavo y actualizar los datos de los sensores
//addressW es la dirrecion del esclavo para escribirle
//addressR es la direccion del esclavo para leerlo
//dato es el dato o lectura del sensor
//comando es el caracter que enviara el Master al slave. (Instruccion que debe hacer)

void actualizar_datos_slave(uint8_t addressW, uint8_t addressR , char comando, uint8_t sensor){
	writeString("Inicio de comunicacion\n");
	if(!I2C_Start()) return;
	
	if (!I2C_write(addressW)){
		I2C_stop();
		writeString("Fallo al iniciar\n");
		return;
	}
	
	if(!I2C_write(comando)){
		I2C_stop();
		writeString("Fallo al enviar\n");
		return;
	}
	
	if (!I2C_repeatedStart()){
		I2C_stop();
		writeString("Fallo al reptir\n");
		return;
	}
	
	if(!I2C_write(addressR)){
		I2C_stop();
		writeString("fallo al escribir\n");
		return;
	}
	switch(sensor){
		case 0:
		I2C_read(&lectura_S1, 0);
		break;
		case 1:
		I2C_read(&lectura_S2, 0);
		break;
		default:
		break;
	}
	I2C_stop();
	
	writeString("No hay fallo\n");
}

void actualizarLCD() {
	Lcd_Clear();  // Limpiar pantalla
	Lcd_Set_Cursor(0, 0);
	Lcd_Write_String("S1:");  // Escribir etiqueta de Sensor 1
	
	Lcd_Set_Cursor(0, 6);
	Lcd_Write_String("S2:");  // Escribir etiqueta de Sensor 2
	/*Lcd_Set_Cursor(0, 11);
	Lcd_Write_String("S3:");  // Escribir etiqueta de Sensor 3
	*/
	actualizarS1(lista1, lectura_S1);
	actualizarS1(lista2,lectura_S2);
	//actualizarS3(lista3,lectura_S3)
	
	Lcd_Set_Cursor(1,0);
	Lcd_Write_String(lista1);
	Lcd_Set_Cursor(1,6);
	Lcd_Write_String(lista2);
	
	
}

void actualizarS1(char *lista, uint8_t dato){
	float voltaje = (dato * 5.0) / 255.0;
	uint16_t int_part = (uint16_t)voltaje;
	uint16_t dec_part = (uint16_t)((voltaje - int_part) * 100);  // Dos decimales

	if (int_part < 10) {
		lista[0] = '0' + int_part;
		lista[1] = '.';
		lista[2] = '0' + (dec_part / 10);
		lista[3] = '0' + (dec_part % 10);
		lista[4] = 'V';
		lista[5] = '\0';
	}
	else {
		lista[0] = '0' + (int_part / 10);
		lista[1] = '0' + (int_part % 10);
		lista[2] = '.';
		lista[3] = '0' + (dec_part / 10);
		lista[4] = '0' + (dec_part % 10);
		lista[5] = 'V';
		lista[6] = '\0';
	}
}

/********* Rutinas de interrupcion *********/