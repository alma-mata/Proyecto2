/*
 * PWM1.c
 *
 * Created: 9/05/2025 13:46:52
 *  Author: ialma
 */ 

#include "PWM1.h"

void PWM1_init(void) {
	// PB1 (OCR1A) y PB2 (OCR1B)
	DDRB |= (1 << PINB1) | (1 << PINB2);
	
	// Modo Fast PWM con ICR1 como TOP (Modo 14)
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Clear OC1A on compare match, set at BOTTOM (non-inverting)
	TCCR1B = (1 << WGM13) | (1 << CS11); // Prescaler 8
	
	ICR1 = 20000;
}

void dutyCycle_PB1(uint16_t dutyCycle){
	OCR1A = dutyCycle;
}

void dutyCycle_PB2(uint16_t dutyCycle){
	OCR1B = dutyCycle;	
}