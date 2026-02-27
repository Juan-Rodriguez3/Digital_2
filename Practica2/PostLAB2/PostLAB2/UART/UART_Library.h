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


#endif /* USARTINIT_H_ */