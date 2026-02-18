/*
 * Proyecto_red_de_sensores.c
 * Casa inteligente
 * Created: 2/5/2026 12:05:06 AM
 * Juan Rodriguez - 221593
 * Gerardo Lopez -
 *	Digital 2 - Seccion 11
 */ 
//Master

#define F_CPU 16000000
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
#define slave1W (0x30<<1) & 0b11111110
#define slave2R (0x40<<1)|0x01
#define slave2W (0x40<<1) & 0b11111110

#define ST_I2CR	(0x48<<1)|0x01
#define ST_I2cW  (0x48<<1) & 0b11111110




uint8_t command_ESP=0;

uint8_t lectura_S1=0;
uint8_t lectura_S2=0;
int16_t lectura_LM75 = 0;

/********* Listas para la función de LCD *********/
char lista1[8] = {'0', '0', '0', '0'};
char lista2[8] = {'0', '0', '0', '0'};
char lista3[8] = {'0', '0', '0', '0'};

/********* Prototipos *********/
void refresh_PORT(uint8_t bus_data);
void setup();
void actualizarLCD();
void actualizar_datos_slave(uint8_t addressW, uint8_t addressR , char comando, uint8_t sensor);
void actualizarS1(char *lista, uint8_t cod_time);
void actualizarS2(char *lista, uint8_t dato);
void actualizarS3(char *lista, float temp);

//Funciones para sensor I2C
uint8_t leer_LM75(void);
float obtenerTemperatura(); 

/********* Funcion principal *********/
int main(void)
{
    /* Replace with your application code */
	
	setup();
	
	Lcd_Clear();
	
    while (1) 
    {
		//Lectura de S1 - Sensor ultrasonico 
		actualizar_datos_slave(slave1W, slave1R,'R', 0);
		
		
		//Lectura de S2 - Sensor de luz
		//Cambiar el parametro crítico del sensor de LUZ
		if (command_ESP=='L'){
			/*
			El tres es para indicarle al Master que le esta mandando comando
			La lee le indica al esclavo que le van a reconfigurar el valor de la resistencia para mover el stepper.
			*/
			actualizar_datos_slave(slave2W,slave2R, 'C', 1);
			
			//Aca le configuro ese valor
			actualizar_datos_slave(slave2W,slave2R, 200, 1);	
			
			//Limpiar bandera de comando
			command_ESP=0;		
		}
		actualizar_datos_slave(slave2W, slave2R , 'W', 1);	//Pedir dato de luz
		
		//Lectura de S3 - Sensor de temperatura
		if(leer_LM75()){ 
			PINC |= (1<<PINC3);
			lectura_LM75 = obtenerTemperatura(); 
		}
		else {
			writeString("Lectura de sensor I2C fallo\n");
		}
		
		//Actualizar los datos
		actualizarLCD();
		_delay_ms(10);
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

	//PORTC 
	DDRC |= (1<<DDC3);
	PORTC &= ~(1<<PORTC3);
	
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
		writeString("Dato recibidio\n");
		break;
		case 1:
		I2C_read(&lectura_S2, 0);
		writeString("Dato recibidio\n");
		break;
		case 3:
		writeString("No leemos dato");
		break;
		default:
		break;
	}
	I2C_stop();
	
	writeString("comunicación terminada\n");
}

/*Funcion para comunicarse con el sensor I2C de temperatura LM75*/
uint8_t leer_LM75(void)
{
	uint8_t MSB, LSB;

	if(!I2C_Start()) return 0;

	if(!I2C_write(ST_I2cW)){
		I2C_stop();
		return 0;
	}

	// Pointer TEMP = 0x00
	if(!I2C_write(0x00)){
		I2C_stop();
		return 0;
	}

	if(!I2C_repeatedStart()){
		I2C_stop();
		return 0;
	}

	if(!I2C_write(ST_I2CR)){
		I2C_stop();
		return 0;
	}

	if(!I2C_read(&MSB, 1)){
		I2C_stop();
		return 0;
	}

	if(!I2C_read(&LSB, 0)){
		I2C_stop();
		return 0;
	}

	I2C_stop();

	lectura_LM75 = (MSB << 8) | LSB;

	return 1;
}

/*Funcion para convertir la lectura del sensor a °C*/
float obtenerTemperatura(void)
{
	int16_t temp = lectura_LM75 >> 7;
	return temp * 0.5;
}

void actualizarLCD() {
	Lcd_Clear();  // Limpiar pantalla
	
	Lcd_Set_Cursor(0, 0);
	Lcd_Write_String("S1:");  // Escribir etiqueta de Sensor 1
	actualizarS1(lista1, lectura_S1);	//Actualizar dato de S1
	Lcd_Set_Cursor(1,0);
	Lcd_Write_String(lista1);
		
	Lcd_Set_Cursor(0, 6);
	Lcd_Write_String("S2:");  // Escribir etiqueta de Sensor S2
	actualizarS2(lista2,lectura_S2);	//Actualizar dato de S1
	Lcd_Set_Cursor(1,6);
	Lcd_Write_String(lista2);
		
	Lcd_Set_Cursor(0, 10);
	Lcd_Write_String("S3:");  // Escribir etiqueta de Sensor S2
	actualizarS3(lista3,lectura_LM75);	//Actualizar dato de S1
	Lcd_Set_Cursor(1,10);
	Lcd_Write_String(lista3);
}



void actualizarS1(char *lista, uint8_t cod_dist)
{
	uint16_t distancia = ((uint32_t)cod_dist * 448UL) / 255UL + 2;

	if (distancia > 450)
	distancia = 450;

	uint8_t centenas = distancia / 100;
	uint8_t decenas  = (distancia / 10) % 10;
	uint8_t unidades = distancia % 10;

	lista[0] = centenas + '0';
	lista[1] = decenas  + '0';
	lista[2] = unidades + '0';
	lista[3] = 'c';
	lista[4] = 'm';
	lista[5] = '\0';
}


void actualizarS2(char *lista, uint8_t dato)
{
	/*no hay luz --> 0%
	  Con luz --> 100%
	  Para ello hay que inverir el dato si hay mucha luz la lectura del ADC 
	  sera pequeña por lo tanto 255-dato sera mayor y si esta oscura el voltaje
	  sera alto por lo que el porcentaje sera menor
	*/
	uint8_t porcentaje = (( 255-dato)* 100UL) / 255;

	if (porcentaje == 100)
	{
		lista[0] = '1';
		lista[1] = '0';
		lista[2] = '0';
		lista[3] = '%';
		lista[4] = '\0';
	}
	else if (porcentaje >= 10)
	{
		lista[0] = (porcentaje / 10) + '0';
		lista[1] = (porcentaje % 10) + '0';
		lista[2] = '%';
		lista[3] = '\0';
	}
	else
	{
		lista[0] = porcentaje + '0';
		lista[1] = '%';
		lista[2] = '\0';
	}
}

void actualizarS3(char *lista, float temp)
{
	int entero;
	uint8_t decimal;
	uint8_t index = 0;

	// Manejo de negativo
	if(temp < 0){
		lista[index++] = '-';
		temp = -temp;
	}

	entero = (int)temp;

	// Parte decimal (solo 0 o 5)
	if((temp - entero) >= 0.5)
	decimal = 5;
	else
	decimal = 0;

	// Centenas (si existen)
	if(entero >= 100){
		lista[index++] = (entero / 100) + '0';
		entero %= 100;
	}

	// Decenas
	if(entero >= 10){
		lista[index++] = (entero / 10) + '0';
		} else {
		lista[index++] = '0';
	}

	// Unidades
	lista[index++] = (entero % 10) + '0';

	// Punto decimal
	lista[index++] = '.';

	// Decimal
	lista[index++] = decimal + '0';
	lista[index++] = 0xDF;   // Código típico para ° en HD44780
	lista[index++] = 'C';

	lista[index] = '\0';
}


/********* Rutinas de interrupcion *********/