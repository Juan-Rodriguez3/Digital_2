/*
 * IncFile1.h
 *
 * Created: 1/22/2026 7:37:28 PM
 *  Author: juana
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_
#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>

void Lcd_Init8bits();

void inicio(char a);    //Función de repeticion de comandos

void dato_a_mostrar(char a);

void Lcd_Clear();

void Lcd_Set_Cursor(char a, char b);

void Lcd_Write_Char(char data);

void Lcd_Write_String(char *a);

void Lcd_Shift_Right();

void Lcd_Shift_Left();




#endif /* INCFILE1_H_ */