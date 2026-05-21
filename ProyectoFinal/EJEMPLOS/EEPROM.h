/*
 * EEPROM.h
 *
 * Created: 18/05/2026 09:40:00 a. m.
 *  Author: astri
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

unsigned char EEPROM_read(unsigned int uiAddress);
void          EEPROM_write(unsigned int uiAddress, unsigned char ucData);
void          EEPROM_update(unsigned int uiAddress, unsigned char ucData);

unsigned char EEPROM_read(unsigned int uiAddress)
{
    while(EECR & (1<<EEPE));
    EEAR = uiAddress;
    EECR |= (1<<EERE);
    return EEDR;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
    while(EECR & (1<<EEPE));
    EEAR  = uiAddress;
    EEDR  = ucData;
    EECR |= (1<<EEMPE);
    EECR |= (1<<EEPE);
}

void EEPROM_update(unsigned int uiAddress, unsigned char ucData)
{
    if(EEPROM_read(uiAddress) != ucData)
    {
        EEPROM_write(uiAddress, ucData);
    }
}

#endif /* EEPROM_H_ */