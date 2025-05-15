/*
 * PWM0.c
 *
 * Created: 9/05/2025 13:45:11
 *  Author: ialma
 */ 
// target_pulse_width = 1000 + (adc_value * 1000UL / 1023);
#include "PWM0.h"

volatile uint16_t contador_pulso = 0;
volatile uint16_t pulse_SERVO1 = 0;
volatile uint16_t pulse_SERVO2 = 0;
volatile uint16_t pulse_SERVO3 = 0;
/****************************************/
// Subrutinas sin Interrupcion
void PWM0_init(void) {
	DDRB |= (1 << PINB5) | (1 << PINB4) | (1 << PINB3);
	
	// Señal PWM con modo CTC
	TCCR0A = (1 << WGM01);
	TCCR0B = (1 << CS01);	// Prescaler de 8
	OCR0A = 100;
	TIMSK0 = (1 << OCIE0A);		// Interrupciones por comparacion A
}

void dutyCycle_PB3(uint16_t dutyCycle) {
	pulse_SERVO1 = dutyCycle;
}

void dutyCycle_PB4(uint16_t dutyCycle) {
	pulse_SERVO2 = dutyCycle;
}

void dutyCycle_PB5(uint16_t dutyCycle) {
	pulse_SERVO3 = dutyCycle;
}

/****************************************/
// Subrutinas de Interrupcion
ISR(TIMER0_COMPA_vect) {
	contador_pulso++;
	if (contador_pulso >= 400) contador_pulso = 0;
	if (contador_pulso == 0)
	{
		PORTB |= (1 << PORTB3) | (1 << PORTB4) | (1 << PORTB5);
	}
	else if (contador_pulso == (pulse_SERVO1 / 50))
	{
		PORTB &= ~(1 << PORTB3);
	}
	else if (contador_pulso == (pulse_SERVO2 / 50))
	{
		PORTB &= ~(1 << PORTB4);
	}
	else if (contador_pulso == (pulse_SERVO3 / 50))
	{
		PORTB &= ~(1 << PORTB5);
	}
}