#include "Timer1.h"

#include <Arduino.h>
#include <util/atomic.h>

volatile uint32_t s_overflow_count;

ISR(TIMER1_OVF_vect)
{
	s_overflow_count++;
}

void Timer1::setup()
{
	TCCR1A &= 0b11111100;	// Disable WGM10, WGM11
	TCCR1B &= 0b11100000;	// Disable WGM12, WGM13 and clear clock settings
	TCCR1B |= 0b00000010;	// Set clock prescaler to 1/8 (2MHz aka 0.5us resolution)
	TIMSK1 |= 0b00000001;	// Enable overflow ISR
}

uint32_t Timer1::getCount() {
	uint16_t counter;
	uint32_t overflow_count;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		counter = TCNT1;
		if (TIFR1 & 0b00000001)	// Overflow detected while interrupt disabled, increment overflow, re-read, then reset overflow flag
		{
			counter = TCNT1;
			TIFR1 &= 0b00000001;
			s_overflow_count++;
		}
		overflow_count = s_overflow_count;
	}

	return (overflow_count << 16) + counter;
}