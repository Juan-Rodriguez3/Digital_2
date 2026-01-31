#ifndef USARTINIT_H_
#define USARTINIT_H_

#include <stdlib.h>
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>	// Para sprintf
#include <string.h> // Para strcat

void writeString(char* texto);
void initUART_9600(void);
void write(char texto);
void UART_sendUint8(uint8_t num);
uint8_t str_to_uint8(char *str, uint8_t len, uint8_t *result);
uint8_t str_len(char *str);

#endif /* USARTINIT_H_ */