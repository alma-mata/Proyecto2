/*
 * PWM1.h
 *
 * Created: 9/05/2025 13:46:39
 *  Author: ialma
 */ 


#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>

void PWM1_init(void);
void dutyCycle_PB1(uint16_t dutyCycle);
void dutyCycle_PB2(uint16_t dutyCycle);

#endif /* PWM1_H_ */