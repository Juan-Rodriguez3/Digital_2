#include "display.h"
#include <avr/io.h>

// Puntero al puerto del display
static volatile uint8_t *display_port;

// Tabla para display de 7 segmentos (cátodo común)
static const uint8_t seg_table[6] = {
	0b01111110, // 0 
	0b00100010, // 1
	0b01011011, // 2
	0b01101011, // 3
	0b00100111, // 4
	0b01101101, // 5
};

//PD7 --> pt
//PD6 --> d
//PD5 --> c
//PD4 --> e
//PD3 --> a
//PD2 --> f
//PD1 --> b
//PD0 --> g

void display7seg_init(volatile uint8_t *port)
{
	display_port = port;
}

void display7seg_write(uint8_t number)
{
	if (number < 10)
	*display_port = seg_table[number];
}
