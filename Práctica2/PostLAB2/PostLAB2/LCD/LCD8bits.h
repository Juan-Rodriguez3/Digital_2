/*
 * IncFile1.h
 *
 * Created: 1/22/2026 7:37:28 PM
 *  Author: juana
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdint.h>

#include <util/delay.h>

#ifndef LCD_H_
#define LCD_H_

void Lcd_Init8bits(void);
void Lcd_Write_Char(char data);
void Lcd_Write_String(char *a);
void Lcd_Set_Cursor(char a, char b);
void Lcd_Clear(void);
void Lcd_Shift_Left(void);
void Lcd_Shift_Right(void);

#endif





#endif /* INCFILE1_H_ */