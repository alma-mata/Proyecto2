/*
 * EEPROM.c
 *
 * Created: 13/05/2025 10:54:01
 *  Author: ialma
 */ 
#include "EEPROM.h"

void writeEEPROM(uint8_t dato, uint16_t direccion)
{
	// Espera que termine la escritura anterior
	while (EECR & (1 << EEPE));		
	// Asignar direccion de escritura
	EEAR = direccion;
	// Asignar dato a escribir
	EEDR = dato;
	// Setear en 1 el "master write enable"
	EECR |= (1 << EEMPE);
	// Empezar a escribir
	EECR |= (1 << EEPE);
}

uint8_t readEEPROM(uint16_t direccion)
{
	// Espera que termine la escritura anterior
	while (EECR & (1 << EEPE));
	// Asignar direccion de escritura
	EEAR = direccion;
	// Empezar a leer
	EECR |= (1 << EERE);
	return EEDR;
}