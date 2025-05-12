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
#include "PWM1/PWM1.h"

/****************************************/
// Prototipos de función
void setup(void);
void ADC_init(void);
void mover_servos(void);

// Variables
volatile uint8_t canal_ADC = 0;
volatile uint16_t servo1 = 0;
volatile uint16_t servo2 = 0;
uint8_t estado_flag = 0;
/****************************************/
// Función principal
int main(void)
{
    /* Replace with your application code */
	setup();
    while (1) 
    {
		mover_servos();
    }
}

/****************************************/
// Subrutinas sin Interrupcion
void setup(void){
	cli();
	ADC_init();
	PWM1_init();
	
	DDRD = (1 << DDD7) | (1 << DDD6);
	sei();
}

void ADC_init(void){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (canal_ADC & 0x1F); // AVcc como referencia
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar ADC con prescaler de 128 (16MHz/128 = 125kHz)
	ADCSRA |= (1<<ADSC); // Inicia primera conversión del ADC
}

void mover_servos(void){
	dutyCycle_PB1(500 + (servo1 * 2000UL / 1023));
	dutyCycle_PB2(500 + (servo2 * 2000UL / 1023));
}
/****************************************/
// Subrutinas de Interrupcion
ISR(ADC_vect){
	switch (canal_ADC){
		case 0: servo1 = ADC; break;
		case 1: servo2 = ADC; break;
	}
	canal_ADC = (canal_ADC + 1) % 2;
	ADMUX = (ADMUX & 0xF0) | (canal_ADC & 0x03);
	ADCSRA |= (1 << ADSC);
}