/*
 * EEPROM.h
 *
 * Created: 13/05/2025 10:53:41
 *  Author: ialma
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

void writeEEPROM(uint8_t dato, uint16_t direccion);
uint8_t readEEPROM(uint16_t direccion);

#endif /* EEPROM_H_ */