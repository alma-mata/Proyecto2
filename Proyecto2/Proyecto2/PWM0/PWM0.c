/*
 * PWM0.c
 *
 * Created: 9/05/2025 13:45:11
 *  Author: ialma
 */ 
// target_pulse_width = 1000 + (adc_value * 1000UL / 1023);
#include "PWM0.h"

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
		PORTB |= servo1_PIN | servo2_PIN | servo3_PIN;
	}
	else if (contador_pulso == (pulse_SERVO1 / 50))
	{
		PORTB &= ~servo1_PIN;
	}
	else if (contador_pulso == (pulse_SERVO2 / 50))
	{
		PORTB &= ~servo2_PIN;
	}
	else if (contador_pulso == (pulse_SERVO3 / 50))
	{
		PORTB &= ~servo3_PIN;
	}
}