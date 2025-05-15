/*
 * Universidad del Valle de Guatemala
 * IE2023: Programacion de Microcontroladores
 * Proyecto2.c
 * Autor: Alma Lisbeth Mata Ixcayau
 * Proyecto: Proyecto 2
 * Descripcion: Proyecto de vehiculo blindado da armamento pesado.
 * Creado: 2/05/2025 11:11:35
*/
/****************************************/
// Encabezado
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PWM0/PWM0.h"
#include "PWM1/PWM1.h"

/****************************************/
// Prototipos de función
void setup(void);
void ADC_init(void);
void mover_servos(void);
void set_estado(void);

// Variables
volatile uint8_t canal_ADC = 0;
volatile uint16_t servo1 = 0;
volatile uint16_t servo2 = 0;
volatile uint16_t servo3 = 0;
volatile uint16_t servo4 = 0;
volatile uint16_t servo5 = 0;
volatile uint8_t estado_flag = 0;
volatile uint8_t estado_actual = 1;
/****************************************/
// Función principal
int main(void)
{
    /* Replace with your application code */
	setup();
	set_estado();
    while (1) 
    {
		if (estado_flag == 1)
		{
			estado_flag = 0;
			estado_actual++;
			set_estado();
			
		}
		mover_servos();
    }
}

/****************************************/
// Subrutinas sin Interrupcion
void setup(void){
	cli();
	ADC_init();
	PWM0_init();
	PWM1_init();
	
	// Entradas y salidas
	DDRB &= ~(1 << PORTB0);
	PORTB |= (1 << PORTB0);
	
	DDRD = (1 << PIND4) | (1 << PIND3) | (1 << PIND2);
	DDRD &= ~((1 << PORTD7) | (1 << PORTD6) | (1 << PORTD5));
	PORTD |= (1 << PORTD7) | (1 << PORTD6) | (1 << PORTD5);
	
	// Habilitar interrupciones del Puerto D
	PCMSK0 |= (1 << PCINT0);  // PB0 (PCINT0)
	PCMSK2 |= (1 << PCINT23) | (1 << PCINT22) | (1 << PCINT21);  // PD5–PD7
	PCICR |= (1 << PCIE0) | (1 << PCIE2);
	
	sei();
}

void ADC_init(void){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (canal_ADC & 0x1F); // AVcc como referencia
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar ADC con prescaler de 128 (16MHz/128 = 125kHz)
	ADCSRA |= (1<<ADSC); // Inicia primera conversión del ADC
}

void mover_servos(void){
	dutyCycle_PB1(500 + (servo1 * 2000UL / 255));
	dutyCycle_PB2(500 + (servo2 * 2000UL / 255));
	dutyCycle_PB3(500 + (servo3 * 2000UL / 255));
}

void set_estado(void){
	switch (estado_actual){
		case 1: PORTD = (PORTD & 0xE3) | (1 << PORTD2); break;
		case 2: PORTD = (PORTD & 0xE3) | (1 << PORTD3); break;
		case 3: 
			PORTD = (PORTD & 0xE3) | (1 << PORTD4); 
			estado_actual = 0;
			break;
	}
}
/****************************************/
// Subrutinas de Interrupcion
ISR(ADC_vect){
	switch (canal_ADC){
		case 0: servo1 = ADCH; break;
		case 1: servo2 = ADCH; break;
		case 2: servo3 = ADCH; break;
	}
	canal_ADC = (canal_ADC + 1) % 3;
	ADMUX = (ADMUX & 0xF0) | (canal_ADC & 0x1F);
	ADCSRA |= (1 << ADSC);
}

ISR(PCINT0_vect){
	
}

ISR(PCINT2_vect){
	if (!(PIND & (1 << PIND5)))
	{
		estado_flag = 1;
	}
}