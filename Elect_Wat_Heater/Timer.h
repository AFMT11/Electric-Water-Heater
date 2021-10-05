/*
 * Timers.h
 *
 * Created: 7/10/2021 7:45:16 PM
 *  Author: ahmed
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include "Timer_CFG.h"

/*Timer0/2 Modes*/
#define NORMAL_MODE             0
#define PWM_MODE                1
#define CTC_MODE                2
#define FAST_PWM_MODE           3

/*Timer1 Modes*/


/*Timer0 Fast PWM Modes*/
#define OCR0_DISCONNECTED       0
#define RESERVED                1
#define F_OC0_CCOMP_STOP        2
#define F_OC0_SCOMP_CTOP        3

/*Timer0 Phase Correct PWM Modes*/
#define OCR0_DISCONNECTED       0
#define RESERVED                1
#define P_OC0_CCOMPU_SCOMPD     2
#define P_OC0_SCOMPU_CCOMPD     3

/*Timer0 Clocks*/
#define T0_NO_CLOCK                0
#define T0_PURE_CLOCK              1
#define T0_CLOCK_8                 2
#define T0_CLOCK_64                3
#define T0_CLOCK_256               4
#define T0_CLOCK_1024              5

/*Timer1 Clocks*/
#define T1_NO_CLOCK                0
#define T1_PURE_CLOCK              1
#define T1_CLOCK_8                 2
#define T1_CLOCK_64                3
#define T1_CLOCK_256               4
#define T1_CLOCK_1024              5

/*Timer2 Clocks*/
#define T2_NO_CLOCK                0
#define T2_PURE_CLOCK              1
#define T2_CLOCK_8                 2
#define T2_CLOCK_32                3
#define T2_CLOCK_64                4
#define T2_CLOCK_128               5
#define T2_CLOCK_256               6
#define T2_CLOCK_1024              7

void callback0(void(*p)(Uint32));
void callback1(void(*p)(Uint8, Uint8));
void callback2(void(*p)(void));

/*Timer0 Functions*/
void TIMER0_INIT(void);
void TIMER0_Start(void);
void TIMER0_Stop(void);
void TIMER0_SetDelay(Uint32 delay);
void PWM0_Gen(f32 duty_cycle, Uint8 switcher);

/*Timer1 Functions*/
void TIMER1_INIT(void);
void TIMER1_Start(void);
void TIMER1_Stop(void);
void TIMER1_SetDelay(Uint32 delay);

/*Timer2 Functions*/
void TIMER2_INIT(void);
void TIMER2_Start(void);
void TIMER2_Stop(void);
void TIMER2_SetDelay(Uint32 delay);


#endif /* TIMERS_H_ */