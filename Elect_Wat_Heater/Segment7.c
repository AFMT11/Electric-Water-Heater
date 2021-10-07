/*
 * _7_Segment.c
 *
 * Created: 10/1/2021 9:18:39 PM
 *  Author: ahmed
 */ 

#define F_CPU 16000000
#include <util/delay.h>
#include "Segment7.h"

void S7_INIT(void)
{
	//Data pin initialization
	DIO_SetPinDir(S7_DATA_PORT, S7_DATA_PIN0, DIO_PIN_OUT);
	DIO_SetPinDir(S7_DATA_PORT, S7_DATA_PIN1, DIO_PIN_OUT);
	DIO_SetPinDir(S7_DATA_PORT, S7_DATA_PIN2, DIO_PIN_OUT);
	DIO_SetPinDir(S7_DATA_PORT, S7_DATA_PIN3, DIO_PIN_OUT);
	
	//Command pins initialization
	DIO_SetPinDir(S7_CMD_PORT, S7_DATA_PIN1, DIO_PIN_OUT);	
	DIO_SetPinDir(S7_CMD_PORT, S7_DATA_PIN2, DIO_PIN_OUT);
	
	//Output zeros to data pins
	DIO_SetPinVal(S7_DATA_PORT, S7_DATA_PIN0, DIO_PIN_OFF);
	DIO_SetPinVal(S7_DATA_PORT, S7_DATA_PIN1, DIO_PIN_OFF);	
	DIO_SetPinVal(S7_DATA_PORT, S7_DATA_PIN2, DIO_PIN_OFF);
	DIO_SetPinVal(S7_DATA_PORT, S7_DATA_PIN3, DIO_PIN_OFF);	
	
	//Output zeros to command pins
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN1, DIO_PIN_OFF);
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN2, DIO_PIN_OFF);
	
}

#if S7_MODE == SINGLE_S7

void S7_WriteINTEGER(Uint8 data)
{
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN1, DIO_PIN_ON);
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN2, DIO_PIN_ON);
	A_PORTA = (data << 4);
}

#elif S7_MODE == TWO_S7

void S7_WriteINTEGER(Uint8 data)
{
	Uint8 Tens, Ones;
	Ones = data % 10;
	data /= 10;
	Tens = data % 10;
	
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN1, DIO_PIN_OFF);
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN2, DIO_PIN_ON);
	PORTA = (Ones << 4);
	_delay_ms(SWITCH_TIME);
	
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN2, DIO_PIN_OFF);
	DIO_SetPinVal(S7_CMD_PORT, S7_CMD_EN1, DIO_PIN_ON);
	PORTA = (Tens << 4);
	_delay_ms(SWITCH_TIME);
}

#endif

void S7_WrtieCHAR(Uint8 data)
{
	
}