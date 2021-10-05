/*
 * _7_Segment_CFG.h
 *
 * Created: 10/1/2021 9:19:16 PM
 *  Author: ahmed
 */ 


#ifndef SEGMENT7_CFG_H_
#define SEGMENT7_CFG_H_

#include "Bit_Math.h"
#include "MC_Selector.h"
#include "std.h"
#include "DIO.h"

//Mode of 7-Segment
#define S7_MODE              TWO_S7

//Port of 4 inputs to BCD driver
#define S7_DATA_PORT         DIO_PORTA

//Port of switching inputs to 7-Segments
#define S7_CMD_PORT          DIO_PORTB

//Pins of 4 inputs to BCD driver
#define S7_DATA_PIN0         DIO_PIN_4
#define S7_DATA_PIN1         DIO_PIN_5
#define S7_DATA_PIN2         DIO_PIN_6
#define S7_DATA_PIN3         DIO_PIN_7

#define S7_CMD_EN1           DIO_PIN_2
#define S7_CMD_EN2           DIO_PIN_1

//Switching time
#define SWITCH_TIME          10

#endif /* 7_SEGMENT_CFG_H_ */