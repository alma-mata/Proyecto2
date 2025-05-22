/*
 * UART.h
 *
 * Created: 21/05/2025 16:05:39
 *  Author: ialma
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

void UART_init(void);
void send_CHAR(unsigned char caracter);
void send_STRING(char* texto);

#endif /* UART_H_ */