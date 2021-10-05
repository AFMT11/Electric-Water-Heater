/*
 * Timer_CFG.h
 *
 * Created: 7/10/2021 7:45:54 PM
 *  Author: ahmed
 */ 


#ifndef TIMER_CFG_H_
#define TIMER_CFG_H_

#include "MC_Selector.h"
#include "std.h"
#include "Bit_Math.h"
#include "Interrupts.h"
#include "DIO.h"

/*Timer0 Mode Selection*/
#define TIMER0_MODE          NORMAL_MODE
/*Timer1 Mode Selection*/
#define TIMER1_MODE          CTC_MODE
/*Timer2 Mode Selection*/
#define TIMER2_MODE          NORMAL_MODE

/*Timer0 Fast PWM Mode Selection*/
#define TIMER0_F_PWM_MODE    F_OC0_CCOMP_STOP
/*Timer0 Phase Correct PWM Mode Selection*/
#define TIMER0_P_PWM_MODE    P_OC0_CCOMPU_SCOMPD


/*Timer0 Clock Selection*/
#define TIMER0_CLOCK         T0_CLOCK_1024
/*Timer1 Clock Selection*/
#define TIMER1_CLOCK         T1_CLOCK_1024
/*Timer2 Clock Selection*/
#define TIMER2_CLOCK         T2_CLOCK_1024





#endif /* TIMER_CFG_H_ */