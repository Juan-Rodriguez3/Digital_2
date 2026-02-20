/*
 * Stepper.h
 *	Peoyecto de digital II - Red de sensores
 * Created: 2/15/2026 11:19:12 AM
 *  Author: juan rodriguez
 * Esta libreria es para facilitat el uso de un motor STEPPER 24BYJ28
 */ 

#ifndef STEPPER_H_
#define STEPPER_H_

#include <avr/io.h>
#include <stdint.h>

#define STEPS_PER_REV 4096

typedef struct
{
	int32_t steps_remaining; //Numero de pasos que faltan ejecutar
	uint8_t step_index;		//Indice de pasos (en que paso de la secuencia estamos)
	uint16_t step_delay;
	uint16_t tick_counter;	//Controlador de velocidad
} Stepper_t;

void Stepper_Init(Stepper_t *m);
void Stepper_SetSpeed(Stepper_t *m, uint16_t rpm);
void Stepper_Move(Stepper_t *m, int32_t steps);
void Stepper_Update(Stepper_t *m);

#endif
