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
void set_motor(uint8_t n_motor, uint8_t valor);

// Variables globales
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
volatile uint8_t tipo_feed = 0;
volatile uint8_t binario_buffer[2];
volatile uint8_t binario_index = 0;
volatile uint8_t en_modo_binario = 0;
/****************************************/
// Función principal
int main(void)
{
	setup(); // Configuración inicial
	set_estado(); // Se establece por primera vez el estado
    while (1) 
    {
		if (dato_ENVIADO) // Si hay comunicacion serial
		{
			dato_ENVIADO = 0; // Reiniciar bandera
			if (tipo_feed == 'E')	// Comunicación de feed ESTADO
			{
				set_estadoUART(received_RX); // Cambiar estado por UART
			}
			else if ((tipo_feed == 'P') && (estado_actual == 2)) // Comunicación de feed EEPROM
			{
				leer_EEPROOM(received_RX); // leer EEPROM
			}
			else if ((tipo_feed == 'W') && (estado_actual == 0)) // Comunicación de feed Motor1
			{
				set_motor(1, received_RX);
			}
			else if ((tipo_feed == 'X') && (estado_actual == 0)) // Comunicación de feed Motor2
			{
				set_motor(2, received_RX);
			}
			else if ((tipo_feed == 'Y') && (estado_actual == 0)) // Comunicación de feed Motor3
			{
				set_motor(3, received_RX);
			}
			else if ((tipo_feed == 'Z') && (estado_actual == 0)) // Comunicación de feed Motor4
			{
				set_motor(4, received_RX);
			}
		}
		
		if (estado_actual == 1) // Si estado es 1
		{
			modo_MANUAL();	// Actualizar PWM constantemente
			if (accion_activa == 2)
			{
				accion_activa = 0;
				escribir_EEPROM(); // Guardar en EEPROM si se presiona PD6
			}
		}
	}
}

/****************************************/
// Subrutinas sin Interrupcion
void setup(void){
	cli();
	ADC_init(); // Configuración ADC
	PWM0_init(); // Configuración PWM simulados
	PWM1_init(); // Configuración PWM timer1
	UART_init(); // Configuración UART
	
	// Entradas y salidas	
	DDRD = (1 << PIND4) | (1 << PIND3) | (1 << PIND2); // PD2-PD4 son salidas
	DDRD &= ~((1 << PORTD6) | (1 << PORTD5));
	PORTD |= (1 << PORTD6) | (1 << PORTD5); // PD5-PD6 son entradas
	
	// Habilitar interrupciones del Puerto D
	PCMSK2 |= (1 << PCINT22) | (1 << PCINT21);  // PD5–PD6
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

void modo_MANUAL(void){ // Actualiza los ciclos de trabajo con el valor correspondiente
	dutyCycle_PB1(500 + (servo1 * 2000UL / 255));
	dutyCycle_PB2(500 + (servo2 * 2000UL / 255));
	dutyCycle_PB3(500 + (servo3 * 2000UL / 255));
	if (servo4 > 63) servo4 = 63; // Límite para no superar 45 grados
	dutyCycle_PB4(500 + (servo4 * 2000UL / 255));
}

void set_motor(uint8_t n_motor, uint8_t valor){
	// Recibe el número de motor y su valor y se lo asigna a la variable correspondiente
	switch (n_motor){
		case 1: servo1 = valor; break;
		case 2: servo2 = valor; break;
		case 3: servo3 = valor; break;
		case 4: servo4 = valor; break;
	}
	modo_MANUAL(); // Llama a modo manual para actualizar duty Cycle
}

void set_estado(void){
	// Evalua estado y dependiendo del caso enciende el led correspondiente y 
	// manda el valor por UART a Adafruit
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
	// Recibe el valor recibido en UART y actualiza el estado de los leds y del programa
	switch(estado){
		case 'M': // MANUAL
		PORTD = (PORTD & 0xE3) | (1 << PORTD2); 
		estado_actual = 1;
		break;
		case 'E': // EEPROM
		PORTD = (PORTD & 0xE3) | (1 << PORTD3); 
		estado_actual = 2;
		break;
		case 'S': // COMUNICACION SERIAL
		PORTD = (PORTD & 0xE3) | (1 << PORTD4);
		estado_actual = 0;
		break;
	}
}

void leer_EEPROOM(unsigned char posicion) {
	// Recibe posición de UART y selecciona la posición a leer en la EEPROM
	switch (posicion){
		case 'A': pos_read = 0; break;
		case 'B': pos_read = 1; break;
		case 'C': pos_read = 2; break;
		case 'D': pos_read = 3; break;
	}
	uint8_t direccion = direcciones_EEPROM[pos_read]; // Escoge dirección en la EEPROM
	for (uint8_t i = 0; i < 4; i++) // Ciclo para leer 4 posiciones
	{
		uint8_t dato_leido = readEEPROM(direccion); // Selecciona el dato
		switch (i){ // Dependiendo del ciclo actualiza el duty cycle correspondiente
		case 0: dutyCycle_PB1(500 + (dato_leido * 2000UL / 255)); break;
		case 1: dutyCycle_PB2(500 + (dato_leido * 2000UL / 255)); break;
		case 2: dutyCycle_PB3(500 + (dato_leido * 2000UL / 255)); break;
		case 3: dutyCycle_PB4(500 + (dato_leido * 2000UL / 255)); break;
		}
		direccion++; // Aumenta dirección para el siguiente ciclo
	}
	
}
void escribir_EEPROM(void) {
	uint8_t direccion = direcciones_EEPROM[pos_write]; // Selecciona la dirección en la EEPROM
	for (uint8_t i = 0; i < 4; i++) // Ciclo para escribir 4 posiciones
	{
		switch (i){ // Dependiendo del ciclo, escribe el valor correspondiente
			case 0: writeEEPROM(servo1,direccion); break;
			case 1: writeEEPROM(servo2,direccion); break;
			case 2: writeEEPROM(servo3,direccion); break;
			case 3: writeEEPROM(servo4,direccion); break;
		}
		direccion++; // Aumenta dirección para el siguiente ciclo
	}
	pos_write = (pos_write + 1) & 0x03; // Aumenta el indicador de posición para la siguiente ocasión
}
/****************************************/
// Subrutinas de Interrupcion
ISR(ADC_vect){
	// Dependiendo del canal de lectura asigna el valor a la variable correspondiente
	switch (canal_ADC){
		case 0: servo1 = ADCH; break;
		case 1: servo2 = ADCH; break;
		case 2: servo3 = ADCH; break;
		case 3: servo4 = ADCH; break;
	}
	canal_ADC = (canal_ADC + 1) % 4; // Cambio de canal
	ADMUX = (ADMUX & 0xF0) | (canal_ADC & 0x1F); // Se vuelve a configurar ADMUX
	ADCSRA |= (1 << ADSC); // Nueva lectura
}

ISR(PCINT2_vect){
	accion_activa = 0;
	if (!(PIND & (1 << PIND5))) // Cambiar ESTADO
	{
		estado_actual++; // Aumenta estado
		set_estado();	// Actualiza estado
	}
	else if (!(PIND & (1 << PIND6))) // Guardar EEPROM
	{
		accion_activa = 2; // Activa bandera
	}
}

ISR(USART_RX_vect){
	uint8_t byte = UDR0;	// Recibe dato
	binario_buffer[binario_index++] = byte; // Guarda nuevo valor
	if (binario_index >= 2) // Si el buffer es de 2 posiciones
	{
		tipo_feed = binario_buffer[0];	// Posición 0 indica el FEED
		received_RX = binario_buffer[1]; // Posición 1 indica el valor enviado desde Adafruit
		
		binario_index = 0; // Reincio de indicador
		dato_ENVIADO = 1; // Activa bandera
	}
}