/*
 * EEPROM.c
 *
 * Created: 8/3/2021 4:14:26 PM
 *  Author: ahmed
 */ 

#include "EEPROM.h"
#include <stdlib.h>

#define F_CPU 16000000
#include <util/delay.h>

#define Null           (void*)(0)

void EEPROM_INIT(void)
{
	I2C_INIT();
}

void EEPROM_WriteByte(Uint8 Device_Page, Uint8 Data_Add, Uint8 Data)
{
	if (Device_Page > 7)
	Device_Page = 7;
	//Device address calculation
	Uint8 Device_Add = 0X50 | Device_Page;
	//Sending start condition
	I2C_SendStartCond();
	//sending slave address with write operation
	I2C_SendSlaveAdd_WriteOP(Device_Add);
	//sending data byte address
	I2C_SendData(Data_Add);
	//sending the data byte
	I2C_SendData(Data);
	//Sending stop condition
	I2C_SendStopCond();
	_delay_ms(10);
}

void EEPROM_WriteMultiBytes(Uint8 Device_Page, Uint8 Data_Add, Uint8* Data, Uint8 Data_Size)
{
	if (Device_Page > 7)
	Device_Page = 7;
	//Device address calculation
	Uint8 Device_Add = 0X50 | Device_Page;
	//Sending start condition
	I2C_SendStartCond();
	//sending slave address with write operation
	I2C_SendSlaveAdd_WriteOP(Device_Add);
	//sending data byte address
	I2C_SendData(Data_Add);
	//sending the data byte
	I2C_SendMultiData(Data, Data_Size);
	//Sending stop condition
	I2C_SendStopCond();	
	_delay_ms(10);
}
void EEPROM_WriteString(Uint8 Device_Page, Uint8 Data_Add, Uint8* Data)
{
	if (Device_Page > 7)
	Device_Page = 7;
	//Device address calculation
	Uint8 Device_Add = 0X50 | Device_Page;
	//Sending start condition
	I2C_SendStartCond();
	//sending slave address with write operation
	I2C_SendSlaveAdd_WriteOP(Device_Add);
	//sending data byte address
	I2C_SendData(Data_Add);
	//sending the data byte
	I2C_SendString(Data);
	//Sending stop condition
	I2C_SendStopCond();	
	_delay_ms(10);
}


Uint8 EEPROM_ReadByte(Uint8 Device_Page, Uint8 Data_Add)
{
	Uint8 Data = 0;
	if (Device_Page > 7)
	Device_Page = 7;
	//Device address calculation
	Uint8 Device_Add = 0X50 | Device_Page;
	//Sending start condition
	I2C_SendStartCond();
	//sending slave address with write operation
	I2C_SendSlaveAdd_WriteOP(Device_Add);
	//sending data byte address
	I2C_SendData(Data_Add);
	
	//Sending repeated start condition
	I2C_SendRepStartCond();
	//sending slave address with read operation
	I2C_SendSlaveAdd_ReadOP(Device_Add);	
	//Receiving the data byte
	Data = I2C_ReadData();
	//Sending stop condition
	I2C_SendStopCond();	
	
	return Data;
}
void EEPROM_ReadMultiByte(Uint8 Device_Page, Uint8 Data_Add, Uint8* Rec_Loc, Uint8 Data_Size)
{
	Uint8 i;
	if (Device_Page > 7)
	Device_Page = 7;
	//Device address calculation
	Uint8 Device_Add = 0X50 | Device_Page;
	//Sending start condition
	I2C_SendStartCond();
	//sending slave address with write operation
	I2C_SendSlaveAdd_WriteOP(Device_Add);
	//sending data byte address
	I2C_SendData(Data_Add);
	
	//Sending repeated start condition
	I2C_SendRepStartCond();
	//sending slave address with read operation
	I2C_SendSlaveAdd_ReadOP(Device_Add);
	//Receiving the data byte
	for (i = 0; i < Data_Size; i++)
	{
		Rec_Loc[i] = I2C_ReadMultiData();
	}
	//Sending stop condition
	I2C_SendStopCond();
}