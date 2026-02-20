/*
 * Stepper.h
 *	Peoyecto de digital II - Red de sensores
 * Created: 2/15/2026 11:19:12 AM
 *  Author: juan rodriguez
 * Esta libreria es para facilitat el uso de un motor STEPPER 24BYJ28
 */ 

#include "stepper.h"

static const uint8_t sequence[8] =
{
	0b0001,
	0b0011,
	0b0010,
	0b0110,
	0b0100,
	0b1100,
	0b1000,
	0b1001,
};

void Stepper_Init(Stepper_t *m)
{
	// Configurar PB2, PB3, PB4 como salida
	DDRB |= (1<<PB2) | (1<<PB3) | (1<<PB4);

	// Configurar PC2 como salida
	DDRC |= (1<<PC2);

	// Apagar bobinas
	PORTB &= ~((1<<PB2) | (1<<PB3) | (1<<PB4));
	PORTC &= ~(1<<PC2);

	m->steps_remaining = 0;
	m->step_index = 0;
	m->tick_counter = 0;
}

void Stepper_SetSpeed(Stepper_t *m, uint16_t rpm)
{
	m->step_delay = (60000UL / (STEPS_PER_REV * rpm));
}

void Stepper_Move(Stepper_t *m, int32_t steps)
{
	m->steps_remaining = steps;
}

static void Stepper_Output(uint8_t value)
{
	// IN1 -> PB2  (bit0)
	if(value & 0x01) PORTB |= (1<<PB2);
	else PORTB &= ~(1<<PB2);

	// IN2 -> PB3  (bit1)
	if(value & 0x02) PORTB |= (1<<PB3);
	else PORTB &= ~(1<<PB3);

	// IN3 -> PB4  (bit2)
	if(value & 0x04) PORTB |= (1<<PB4);
	else PORTB &= ~(1<<PB4);

	// IN4 -> PC2  (bit3)
	if(value & 0x08) PORTC |= (1<<PC2);
	else PORTC &= ~(1<<PC2);
}

void Stepper_Update(Stepper_t *m)
{
	if(m->steps_remaining == 0)
	return;

	m->tick_counter++;

	if(m->tick_counter >= m->step_delay)
	{
		m->tick_counter = 0;

		if(m->steps_remaining > 0)
		{
			m->step_index++;
			if(m->step_index > 7)
			m->step_index = 0;

			m->steps_remaining--;
		}
		else
		{
			if(m->step_index == 0)
			m->step_index = 7;
			else
			m->step_index--;

			m->steps_remaining++;
		}

		Stepper_Output(sequence[m->step_index]);
	}
}

