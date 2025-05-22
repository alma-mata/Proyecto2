/*
 * UART.c
 *
 * Created: 21/05/2025 16:05:26
 *  Author: ialma
 */ 

#include "UART.h"

void UART_init(void){
	UBRR0 = 103;
	
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //Rx int. | Rxen | Txen
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Async. | No parity | 1 stop | 8 data
}

void send_CHAR(unsigned char caracter){
	while (!(UCSR0A & (1 << UDRE0))); //Revisar si UDR está vacío
	UDR0 = caracter;
}

void send_STRING(char* texto){
	uint8_t i = 0;
	while (texto[i] != '\0'){
		send_CHAR(texto[i]);
		i++;
	}
}