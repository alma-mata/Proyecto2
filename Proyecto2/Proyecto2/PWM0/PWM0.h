/*
 * PWM0.h
 *
 * Created: 9/05/2025 13:45:30
 *  Author: ialma
 */ 


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>
#include <avr/interrupt.h>

void PWM0_init(void);
void dutyCycle_PB3(uint16_t dutyCycle);
void dutyCycle_PB4(uint16_t dutyCycle);
void dutyCycle_PB5(uint16_t dutyCycle);

#endif /* PWM0_H_ */