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
#include "EEPROM/EEPROM.h"
#include "UART/UART.h"

/****************************************/
// Prototipos de función
void setup(void);
void ADC_init(void);
void modo_MANUAL(void);
void set_estado(void);
void leer_EEPROOM(unsigned char posicion);
void escribir_EEPROM(void);
void set_estadoUART(unsigned char estado);

// Variables
volatile uint8_t canal_ADC = 0;
volatile uint16_t servo1 = 0;
volatile uint16_t servo2 = 0;
volatile uint16_t servo3 = 0;
volatile uint16_t servo4 = 0;
volatile uint8_t accion_activa = 0;
volatile uint8_t estado_actual = 1;

const uint8_t direcciones_EEPROM[4] = {0x00, 0x04, 0x08, 0x0C};
volatile uint8_t pos_write = 0;
volatile uint8_t pos_read = 0;
volatile uint8_t configuracion_flag = 0;

volatile uint8_t received_RX = 0;
volatile uint8_t dato_ENVIADO = 0;
/****************************************/
// Función principal
int main(void)
{
    /* Replace with your application code */
	setup();
	set_estado();
    while (1) 
    {
		if (dato_ENVIADO)
		{
			if ((received_RX == 'M') || (received_RX == 'E') || (received_RX == 'S'))
			{
				set_estadoUART(received_RX);
			}
			if (((received_RX == 'A') || (received_RX == 'B') || (received_RX == 'C') || (received_RX == 'D')) && (estado_actual == 2))
			{
				leer_EEPROOM(received_RX);
			}
		}
		
		if (estado_actual == 1)
		{
			modo_MANUAL();
			if (accion_activa == 2)
			{
				accion_activa = 0;
				escribir_EEPROM();
			}
		}
	}
}

/****************************************/
// Subrutinas sin Interrupcion
void setup(void){
	cli();
	ADC_init();
	PWM0_init();
	PWM1_init();
	UART_init();
	
	// Entradas y salidas	
	DDRD = (1 << PIND4) | (1 << PIND3) | (1 << PIND2);
	DDRD &= ~((1 << PORTD6) | (1 << PORTD5));
	PORTD |= (1 << PORTD6) | (1 << PORTD5);
	
	// Habilitar interrupciones del Puerto D
	PCMSK2 |= (1 << PCINT22) | (1 << PCINT21);  // PD5–PD7
	PCICR |= (1 << PCIE2);
	
	sei();
}

void ADC_init(void){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (canal_ADC & 0x1F); // AVcc como referencia
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar ADC con prescaler de 128 (16MHz/128 = 125kHz)
	ADCSRA |= (1<<ADSC); // Inicia primera conversión del ADC
}

void modo_MANUAL(void){
	dutyCycle_PB1(500 + (servo1 * 2000UL / 255));
	dutyCycle_PB2(500 + (servo2 * 2000UL / 255));
	dutyCycle_PB3(500 + (servo3 * 2000UL / 255));
	dutyCycle_PB4(500 + (servo4 * 2000UL / 255));
}

void set_estado(void){
	switch (estado_actual){
		case 1: 
			PORTD = (PORTD & 0xE3) | (1 << PORTD2); 
			send_STRING("M\n");
			break;
		case 2: 
			PORTD = (PORTD & 0xE3) | (1 << PORTD3); 
			send_STRING("E\n");
			break;
		case 3: 
			PORTD = (PORTD & 0xE3) | (1 << PORTD4); 
			estado_actual = 0;
			send_STRING("S\n");
			break;
	}
}

void set_estadoUART(unsigned char estado){
	switch(estado){
		case 'M': 
		PORTD = (PORTD & 0xE3) | (1 << PORTD2); 
		estado_actual = 1;
		break;
		case 'E': 
		PORTD = (PORTD & 0xE3) | (1 << PORTD3); 
		estado_actual = 2;
		break;
		case 'S': 
		PORTD = (PORTD & 0xE3) | (1 << PORTD4);
		estado_actual = 0;
		break;
	}
}

void leer_EEPROOM(unsigned char posicion) {
	switch (posicion){
		case 'A': pos_read = 0; break;
		case 'B': pos_read = 1; break;
		case 'C': pos_read = 2; break;
		case 'D': pos_read = 3; break;
	}
	uint8_t direccion = direcciones_EEPROM[pos_read];
	for (uint8_t i = 0; i < 4; i++)
	{
		uint8_t dato_leido = readEEPROM(direccion);
		switch (i){
		case 0: dutyCycle_PB1(500 + (dato_leido * 2000UL / 255)); break;
		case 1: dutyCycle_PB2(500 + (dato_leido * 2000UL / 255)); break;
		case 2: dutyCycle_PB3(500 + (dato_leido * 2000UL / 255)); break;
		case 3: dutyCycle_PB4(500 + (dato_leido * 2000UL / 255)); break;
		}
		direccion++;
	}
	
}
void escribir_EEPROM(void) {
	uint8_t direccion = direcciones_EEPROM[pos_write];
	for (uint8_t i = 0; i < 4; i++)
	{
		switch (i){
			case 0: writeEEPROM(servo1,direccion); break;
			case 1: writeEEPROM(servo2,direccion); break;
			case 2: writeEEPROM(servo3,direccion); break;
			case 3: writeEEPROM(servo4,direccion); break;
		}
		direccion++;
	}
	pos_write = (pos_write + 1) & 0x03;
}
/****************************************/
// Subrutinas de Interrupcion
ISR(ADC_vect){
	switch (canal_ADC){
		case 0: servo1 = ADCH; break;
		case 1: servo2 = ADCH; break;
		case 2: servo3 = ADCH; break;
		case 3: servo4 = ADCH; break;
	}
	canal_ADC = (canal_ADC + 1) % 4;
	ADMUX = (ADMUX & 0xF0) | (canal_ADC & 0x1F);
	ADCSRA |= (1 << ADSC);
}

ISR(PCINT2_vect){
	accion_activa = 0;
	if (!(PIND & (1 << PIND5))) // Cambiar ESTADO
	{
		estado_actual++;
		set_estado();
	}
	else if (!(PIND & (1 << PIND6))) // Guardar EEPROM
	{
		accion_activa = 2;
	}
}

ISR(USART_RX_vect){
	received_RX = UDR0;
	dato_ENVIADO = 1;
}