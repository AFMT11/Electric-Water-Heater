/*
 * EEPROM.h
 *
 * Created: 8/3/2021 4:14:42 PM
 *  Author: ahmed
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include "I2C.h"

void EEPROM_INIT(void);

void EEPROM_WriteByte(Uint8 Device_Page, Uint8 Data_Add, Uint8 Data);
void EEPROM_WriteMultiBytes(Uint8 Device_Page, Uint8 Data_Add, Uint8* Data, Uint8 Data_Size);
void EEPROM_WriteString(Uint8 Device_Page, Uint8 Data_Add, Uint8* Data);

Uint8 EEPROM_ReadByte(Uint8 Device_Page, Uint8 Data_Add);
void EEPROM_ReadMultiByte(Uint8 Device_Page, Uint8 Data_Add, Uint8* Rec_Loc, Uint8 Data_Size);


#endif /* EEPROM_H_ */