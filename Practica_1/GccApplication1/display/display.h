/*
 * display.h
 *
 * Created: 1/22/2026 12:00:38 AM
 *  Author: juana
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

// Inicializa el display (recibe puntero al puerto)
void display7seg_init(volatile uint8_t *port);

// Muestra un número del 0 al 9
void display7seg_write(uint8_t number);

#endif
