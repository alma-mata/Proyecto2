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

#define servo1_PIN (1 << PORTB3)
#define servo2_PIN (1 << PORTB4)
#define servo3_PIN (1 << PORTB5)

void PWM0_init(void);
void dutyCycle_PB3(uint16_t dutyCycle);
void dutyCycle_PB4(uint16_t dutyCycle);
void dutyCycle_PB5(uint16_t dutyCycle);

volatile uint16_t contador_pulso = 0;
volatile uint16_t pulse_SERVO1 = 0;
volatile uint16_t pulse_SERVO2 = 0;
volatile uint16_t pulse_SERVO3 = 0;

#endif /* PWM0_H_ */