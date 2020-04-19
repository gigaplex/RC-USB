#ifndef TIMER_1_H
#define TIMER_1_H

#include <stdint.h>

// ATmega32u4 Timer1 Counter
class Timer1
{
public:
	static void setup();
	// Returns a count of the 1/8 prescaled timer
	// This equates to 0.5us on a 16MHz board
	static uint32_t getCount();
};

#endif