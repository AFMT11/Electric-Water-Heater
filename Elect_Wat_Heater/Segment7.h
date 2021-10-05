/*
 * _7_Segment.h
 *
 * Created: 10/1/2021 9:18:59 PM
 *  Author: ahmed
 */ 


#ifndef SEGMENT7_H_
#define SEGMENT7_H_

#include "Segment7_CFG.h"

//Modes of 7-Segment
#define SINGLE_S7            0
#define TWO_S7               1
#define THREE_S7             2
#define FOUR_S7              3
#define FIVE_S7              4

void S7_INIT(void);
void S7_WriteINTEGER(Uint8 data);
void S7_WrtieCHAR(Uint8 data);





#endif /* 7_SEGMENT_H_ */