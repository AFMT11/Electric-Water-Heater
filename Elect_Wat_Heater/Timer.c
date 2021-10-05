/*
 * Timers.c
 *
 * Created: 7/10/2021 7:44:59 PM
 *  Author: ahmed
 */ 

#include "Timer.h"

#include <avr/interrupt.h>
#define Null           (void*)(0)

extern Uint8 Tens, Ones;
extern Uint8 Mode_Selector;
extern Uint8 counter;

/*Global Variables*/
//Timer_0
Uint32 T0_OVF_No = 1;        //Global number of overflows to be used in Timer.c, & also in ISR
Uint32 T0_Init_val;          //Global number of the initial value of the first OVF of the register (TCNT, or OCR)
Uint32 T0_Init_val_OVF;      //Global number of the initial value of the rest of OVFs of the register (TCNT, or OCR)
//Timer_1
Uint32 T1_OVF_No = 1;        //Global number of overflows to be used in Timer.c, & also in ISR
Uint32 T1_Init_val;          //Global number of the initial value of the first OVF of the register (TCNT, or OCR)
Uint32 T1_Init_val_OVF;      //Global number of the initial value of the rest of OVFs of the register (TCNT, or OCR)
//Timer_2
Uint32 T2_OVF_No = 1;        //Global number of overflows to be used in Timer.c, & also in ISR
Uint32 T2_Init_val;          //Global number of the initial value of the first OVF of the register (TCNT, or OCR)
Uint32 T2_Init_val_OVF;      //Global number of the initial value of the rest of OVFs of the register (TCNT, or OCR)
int T2_OVF_count_1 = 0; //counter to count
extern Uint8 Task_Timer_Switch;
extern Uint8 S7_Blinker;

/*Declaration of pointer to function pointers*/
void (*ptr0)(Uint32) = Null;
void (*ptr1)(Uint8, Uint8) = Null;
void (*ptr2)(void) = Null;

/*Implementation of call back functions*/
void callback0(void(*p)(Uint32))
{
	ptr0 = p;
}
void callback1(void(*p)(Uint8, Uint8))
{
	ptr1 = p;
}
void callback2(void(*p)(void))
{
	ptr2 = p;
}

///////////////****************************Timer0 functions****************************///////////////

void TIMER0_INIT(void)
{
	/*Global Interrupt*/
	SREG = GLOBAL_INT << 7;	
	
	/*Modes Initialization*/
	#if TIMER0_MODE == NORMAL_MODE   /////////////////////////////Normal mode
	
	/*Normal mode*/
	Clear_Bit(TCCR0,3);
	Clear_Bit(TCCR0,6);
	
	//Peripheral Interrupt
	TIMSK = TIMER0_OVF_INT | (TIMSK & 0xd4);  
	
	#elif TIMER0_MODE == PWM_MODE  /////////////////////////////Phase Correct PWM mode
	
	/*Phase Correct PWM mode*/
	Clear_Bit(TCCR0,3);
	Set_Bit(TCCR0,6);
	
	/*Selecting mode of Phase Correct PWM*/
	TCCR0 = (TIMER0_P_PWM_MODE << 4) | (TCCR0 & 0x48);	
	
	/*OC0 pin direction*/
	DIO_SetPinDir(DIO_PORTB, DIO_PIN_3, DIO_PIN_OUT);
	
	#elif TIMER0_MODE == CTC_MODE  /////////////////////////////CTC mode
	
	/*CTC mode*/
	Set_Bit(TCCR0,3);
	Clear_Bit(TCCR0,6);
	
	//Peripheral Interrupt
	TIMSK = (TIMER0_OCR_INT << 1) | (TIMSK & 0xd4);  
	
	#elif TIMER0_MODE == FAST_PWM_MODE  /////////////////////////////Fast PWM mode
	
	/*Fast PWM mode*/
	Set_Bit(TCCR0,3);
	Set_Bit(TCCR0,6);
	
	/*Selecting mode of Fast PWM*/
	TCCR0 = (TIMER0_F_PWM_MODE << 4) | (TCCR0 & 0x48);
	
	/*OC0 pin direction*/
	DIO_SetPinDir(DIO_PORTB, DIO_PIN_3, DIO_PIN_OUT);		
	
#endif

	
}
void TIMER0_Start(void)
{
	//Set_Bit(TCCR0, 1);
	TCCR0 = TIMER0_CLOCK | (TCCR0 & 0x78);
}
void TIMER0_Stop(void)
{
	TCCR0 = T0_NO_CLOCK | (TCCR0 & 0x78);
}
void TIMER0_SetDelay(Uint32 delay)
{
	Uint8 Tick_time, check = 1;
	Uint32 i; 
	Uint32 Tick_num;
	Uint8 OVF_buf; //variable to store the possible number of overflows
	Tick_time = 1024 / 16; //tick_time in micro seconds 
	Tick_num = (delay * 1000) / Tick_time; //total number of ticks needed to generate our time
	
	//Normal mode 
	#if TIMER0_MODE == NORMAL_MODE
	//case of tick_num is less than 256 (16.4 ms)
	if (Tick_num <= 256)
	{
		T0_Init_val = 256 - Tick_num; 
		TCNT0 = T0_Init_val;
	}
	//case of tick_num is bigger than 256 (16.4 ms)
	else
	{
		for (i = 130; i <= 256; i++) //check if the number of overflows is integer
		{
			OVF_buf = Tick_num / i; //buffer to store the number of OVF at every register size
			if (Tick_num == (OVF_buf * i)) 
			{
				T0_OVF_No = Tick_num / i;
				T0_Init_val = 256 - i; //initial value at the first OVF
				T0_Init_val_OVF = 256 - i; //initialize value at the rest of OVF
				TCNT0 = T0_Init_val; //initializing the TCNT0 with Init_val
				check = 0; //variable to use in checking of number of OVF being integer or decimal
			}
		}
		//
		if (check) //check if the number of OVF is decimal
		{
			T0_OVF_No += Tick_num / 256;
			T0_Init_val = 256 - (Tick_num % 256);
			T0_Init_val_OVF = 0;
			TCNT0 = T0_Init_val; //initializing the TCNT0	
		}
	}
	//CTC mode
	#elif TIMER0_MODE == CTC_MODE
	//case of tick_num is less than 256
	if (Tick_num <= 256)
	{
		T0_Init_val = Tick_num - 1;
		OCR0 = T0_Init_val;
	}
	//case of tick_num is bigger than 256
	else
	{
		for (i = 130; i <= 256; i++) //check if the number of overflows is integer
		{
			OVF_buf = Tick_num / i; //buffer o store the number of OVF at every register size
			if (Tick_num == (OVF_buf * i))
			{
				T0_OVF_No = Tick_num / i;
				T0_Init_val = i - 1;
				T0_Init_val_OVF = i - 1;
				OCR0 = T0_Init_val; //initializing the OCR0 with Init_val
				check = 0;
			}
		}
		if (check) //check if the number of OVF is decimal
		{
			T0_OVF_No += Tick_num / 256;
			T0_Init_val = (Tick_num % 256) - 1;
			T0_Init_val_OVF = 256 - 1;
			OCR0 = T0_Init_val;
		}
	}
#endif
}

void PWM0_Gen(f32 duty_cycle, Uint8 switcher)
{
	if (duty_cycle < 0)
	{
		duty_cycle = 0;
	}
	else if (duty_cycle > 100)
	{
		duty_cycle = 100;
	}
	
	#if TIMER0_MODE == FAST_PWM_MODE
	
		#if TIMER0_F_PWM_MODE == F_OC0_CCOMP_STOP  //Non-Inverted mode
		if (switcher == 0)
		{
			OCR0 = duty_cycle == 0 ? 0 : (duty_cycle * 256 / 100) - 1;
		}
		else if (switcher == 1)
		{
			OCR0 = duty_cycle;
		}
	
		#elif TIMER0_F_PWM_MODE == F_OC0_SCOMP_CTOP  //Inverted mode
		if (switcher == 0)
		{
			OCR0 = duty_cycle == 100 ? 0 : 255 - (duty_cycle * 256 / 100);
		}
		else if (switcher == 1)
		{
			OCR0 = duty_cycle;
		}

		#endif
	
	#elif TIMER0_MODE == PWM_MODE
	
		#if TIMER0_P_PWM_MODE == P_OC0_CCOMPU_SCOMPD  //Non-Inverted mode
		if (switcher == 0)
		{
			OCR0 = duty_cycle * 255 / 100;
		}
		else if (switcher == 1)
		{
			OCR0 = duty_cycle;
		}
	
		#elif TIMER0_P_PWM_MODE == P_OC0_SCOMPU_CCOMPD  //Inverted mode
		if (switcher == 0)
		{
			OCR0 = 255 - (duty_cycle * 255 / 100);
		}
		else if (switcher == 1)
		{
			OCR0 = duty_cycle;
		}

		#endif
	#endif
}

ISR(TIMER0_OVF_vect)
{
	static int count_0 = 0; //counter to count the number of overflows
	static int count_1 = 0; //counter to count 
	static int count_2 = 0; //counter to count 
	count_0++;
	count_1++;
	if (T0_OVF_No == 1)
	{
		if (count_0 == T0_OVF_No)
		{
			//write your code here
			
			count_2++;
			if ((Task_Timer_Switch == 0) && (count_2 % 100 == 0))
			{
				S7_Blinker++;
			}
			
				if (counter % 2)
				{
					if (S7_Blinker % 2 == 0)
					{			
						if (count_1 % 2 == 0)
						{
							DIO_SetPinVal(DIO_PORTB, DIO_PIN_2, DIO_PIN_OFF);
							PORTA = (Ones << 4) | (PORTA & 0X0F);
							DIO_SetPinVal(DIO_PORTB, DIO_PIN_1, DIO_PIN_ON);
						}
						else
						{
							DIO_SetPinVal(DIO_PORTB, DIO_PIN_1, DIO_PIN_OFF);
							PORTA = (Tens << 4) | (PORTA & 0X0F);
							DIO_SetPinVal(DIO_PORTB, DIO_PIN_2, DIO_PIN_ON);
						}
					}
					else
					{
						DIO_SetPinVal(DIO_PORTB, DIO_PIN_1, DIO_PIN_OFF);
						DIO_SetPinVal(DIO_PORTB, DIO_PIN_2, DIO_PIN_OFF);				
					}
				}			
			
			count_0 = 0;
			TCNT0 = T0_Init_val;
		}
	}
	else
	{
		if (count_0 == 1)
		{
			TCNT0 = T0_Init_val_OVF;
		}
		else if (count_0 == T0_OVF_No)
		{
			//write your code here
			
			count_0 = 0;	
			TCNT0 = T0_Init_val;
		}
	}
}

ISR(TIMER0_COMP_vect)
{
	static int count_0 = 0; //counter to count the number of overflows
	static int count_1 = 0; //counter to count
	count_0++;
	if (T0_OVF_No == 1)
	{
		if (count_0 == T0_OVF_No)
		{
			//write your code here

			count_0 = 0;
			OCR0 = T0_Init_val;
		}
	}
	else
	{
		if (count_0 == 1)
		{
			OCR0 = T0_Init_val_OVF;
		}
		else if (count_0 == T0_OVF_No)
		{
			//write your code here

			count_0 = 0;
			OCR0 = T0_Init_val;
		}
	}	
}


///////////////****************************Timer1 functions****************************///////////////

void TIMER1_INIT(void)
{
	/*Global Interrupt*/
	SREG = GLOBAL_INT << 7;	
	
	/*Peripheral Interrupt*/
	#if TIMER1_MODE == NORMAL_MODE
	TIMSK = (TIMER1_OVF_INT << 2) | (TIMSK & 0xc3);
	#elif TIMER1_MODE == PWM_MODE
	
	#elif TIMER1_MODE == CTC_MODE
	TIMSK = (TIMER1_OCR_INT << 4) | (TIMSK & 0xc3);
	#elif TIMER1_MODE == FAST_PWM_MODE

#endif

	
}
void TIMER1_Start(void)
{
	TCCR1B = TIMER1_CLOCK;
}
void TIMER1_Stop(void)
{
	TCCR1B = T1_NO_CLOCK;
}
void TIMER1_SetDelay(Uint32 delay)
{
	Uint8 Tick_time, check = 1;
	Uint32 i; 
	Uint32 Tick_num;
	Uint8 OVF_buf; //variable to store the possible number of overflows
	Tick_time = 1024 / 16; //tick_time in micro seconds 
	Tick_num = (delay * 1000) / Tick_time; //total number of ticks needed to generate our time
	
	//Normal mode 
	#if TIMER1_MODE == NORMAL_MODE
	//case of tick_num is less than 65535 (4194.24 ms)
	if (Tick_num <= 65536 )
	{
		T1_Init_val = 65535 - Tick_num; 
		TCNT1 = T1_Init_val;
	}
	//case of tick_num is bigger than 65535 (4194.24 ms)
	else
	{
		for (i = 32000; i <= 65536; i++) //check if the number of overflows is integer
		{
			OVF_buf = Tick_num / i; //buffer to store the number of OVF at every register size
			if (Tick_num == (OVF_buf * i)) 
			{
				T1_OVF_No = Tick_num / i;
				T1_Init_val = 65536 - i; //initial value at the first OVF
				T1_Init_val_OVF = 65536 - i; //initialize value at the rest of OVF
				TCNT1 = T1_Init_val; //initializing the TCNT0 with Init_val
				check = 0; //variable to use in checking of number of OVF being integer or decimal
			}
		}
		//
		if (check) //check if the number of OVF is decimal
		{
			T1_OVF_No += Tick_num / 65536;
			T1_Init_val = 65536 - (Tick_num % 65536);
			T1_Init_val_OVF = 0;
			TCNT1 = T1_Init_val; //initializing the TCNT0	
		}
	}
	//CTC mode
	#elif TIMER1_MODE == CTC_MODE
	//case of tick_num is less than 256
	if (Tick_num <= 65536)
	{
		T1_Init_val = Tick_num - 1;
		OCR1A = T1_Init_val;
	}
	//case of tick_num is bigger than 256
	else
	{
		for (i = 32000; i <= 65536; i++) //check if the number of overflows is integer
		{
			OVF_buf = Tick_num / i; //buffer to store the number of OVF at every register size
			if (Tick_num == (OVF_buf * i))
			{
				T1_OVF_No = Tick_num / i;
				T1_Init_val = i - 1;
				T1_Init_val_OVF = i - 1;
				OCR1A = T1_Init_val; //initializing the OCR0 with Init_val
				check = 0;
			}
		}
		if (check) //check if the number of OVF is decimal
		{
			T1_OVF_No += Tick_num / 65536;
			T1_Init_val = (Tick_num % 65536) - 1;
			T1_Init_val_OVF = 65536 - 1;
			OCR1A = T1_Init_val;
		}
	}
#endif

}
ISR(TIMER1_OVF_vect)
{
	static int count_0 = 0; //counter to count the number of overflows
	static int count_1 = 0; //counter to count the number of overflows
	count_0++;
	count_1++;
	if (T1_OVF_No == 1)
	{
		if (count_0 == T1_OVF_No)
		{
			//write your code here
			DIO_TogglePin(DIO_PORTD, DIO_PIN_3);
					
			count_0 = 0;
			TCNT1 = T1_Init_val;
		}
	}
	else
	{
		if (count_0 == 1)
		{
			TCNT1 = T1_Init_val_OVF;
		}
		else if (count_0 == T1_OVF_No)
		{
			//write your code here
			
			
			count_0 = 0;	
			TCNT1 = T1_Init_val;
			
			//TIMER1_Stop();
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	static int count_0 = 0; //counter to count the number of overflows
	count_0++;
	if (T1_OVF_No == 1)
	{
		if (count_0 == T1_OVF_No)
		{
			//write your code here
			DIO_TogglePin(DIO_PORTD, DIO_PIN_3);
			
			count_0 = 0;
			OCR1A = T1_Init_val;
		}
	}
	else
	{
		if (count_0 == 1)
		{
			OCR1A = T1_Init_val_OVF;
		}
		else if (count_0 == T1_OVF_No)
		{
			//write your code here

			count_0 = 0;
			OCR1A = T1_Init_val;
		}
	}	
}

///////////////****************************Timer2 functions****************************///////////////

void TIMER2_INIT(void)
{
	/*Global Interrupt*/
	SREG = GLOBAL_INT << 7;
	
	/*Peripheral Interrupt*/
	#if TIMER2_MODE == NORMAL_MODE
	TIMSK = (TIMER2_OVF_INT << 6) | (TIMSK & 0x17);
	#elif TIMER2_MODE == PWM_MODE
	
	#elif TIMER2_MODE == CTC_MODE
	TIMSK = (TIMER2_OCR_INT << 7) | (TIMSK & 0x17);
	#elif TIMER2_MODE == FAST_PWM_MODE

	#endif

	
}
void TIMER2_Start(void)
{
	TCCR2 = TIMER2_CLOCK;
}
void TIMER2_Stop(void)
{
	TCCR2 = T2_NO_CLOCK;
}
void TIMER2_SetDelay(Uint32 delay)
{
	Uint8 Tick_time, check = 1;
	Uint32 i;
	Uint32 Tick_num;
	Uint8 OVF_buf; //variable to store the possible number of overflows
	Tick_time = 1024 / 16; //tick_time in micro seconds
	Tick_num = (delay * 1000) / Tick_time; //total number of ticks needed to generate our time
	
	//Normal mode
	#if TIMER2_MODE == NORMAL_MODE
	//case of tick_num is less than 256 (16.4 ms)
	if (Tick_num <= 256)
	{
		T2_Init_val = 256 - Tick_num;
		TCNT2 = T2_Init_val;
	}
	//case of tick_num is bigger than 256 (16.4 ms)
	else
	{
		for (i = 130; i <= 256; i++) //check if the number of overflows is integer
		{
			OVF_buf = Tick_num / i; //buffer to store the number of OVF at every register size
			if (Tick_num == (OVF_buf * i))
			{
				T2_OVF_No = Tick_num / i;
				T2_Init_val = 256 - i; //initial value at the first OVF
				T2_Init_val_OVF = 256 - i; //initialize value at the rest of OVF
				TCNT2 = T2_Init_val; //initializing the TCNT0 with Init_val
				check = 0; //variable to use in checking of number of OVF being integer or decimal
			}
		}
		//
		if (check) //check if the number of OVF is decimal
		{
			T2_OVF_No += Tick_num / 256;
			T2_Init_val = 256 - (Tick_num % 256);
			T2_Init_val_OVF = 0;
			TCNT2 = T2_Init_val; //initializing the TCNT0
		}
	}
	//CTC mode
	#elif TIMER2_MODE == CTC_MODE
	//case of tick_num is less than 256
	if (Tick_num <= 256)
	{
		T2_Init_val = Tick_num - 1;
		OCR2 = T2_Init_val;
	}
	//case of tick_num is bigger than 256
	else
	{
		for (i = 130; i <= 256; i++) //check if the number of overflows is integer
		{
			OVF_buf = Tick_num / i; //buffer o store the number of OVF at every register size
			if (Tick_num == (OVF_buf * i))
			{
				T2_OVF_No = Tick_num / i;
				T2_Init_val = i - 1;
				T2_Init_val_OVF = i - 1;
				OCR2 = T2_Init_val; //initializing the OCR0 with Init_val
				check = 0;
			}
		}
		if (check) //check if the number of OVF is decimal
		{
			T2_OVF_No += Tick_num / 256;
			T2_Init_val = (Tick_num % 256) - 1;
			T2_Init_val_OVF = 256 - 1;
			OCR2 = T2_Init_val;
		}
	}
	#endif

}
ISR(TIMER2_OVF_vect)
{
	static int T2_OVF_count_0 = 0; //counter to count the number of overflows

	T2_OVF_count_0++;
	if (T2_OVF_No == 1)
	{
		if (T2_OVF_count_0 == T2_OVF_No)
		{
			//write your code here

			
			T2_OVF_count_0 = 0;
			TCNT2 = T2_Init_val;
		}
	}
	else
	{
		if (T2_OVF_count_0 == 1)
		{
			TCNT2 = T2_Init_val_OVF;
		}
		else if (T2_OVF_count_0 == T2_OVF_No)
		{
			//write your code here
			T2_OVF_count_1++;
			
			if ((T2_OVF_count_1 % 6 == 0) && Task_Timer_Switch == 1)
			{
				DIO_TogglePin(DIO_PORTC, DIO_PIN_2);
			}			
			else if ((T2_OVF_count_1 % 30 == 0) && Task_Timer_Switch == 0)
			{
				Mode_Selector = 1;
				TIMER2_Stop();
			}

			
			T2_OVF_count_0 = 0;
			TCNT2 = T2_Init_val;
			
		}
	}
}

ISR(TIMER2_COMP_vect)
{
	static int T2_COMP_count_0 = 0; //counter to count the number of overflows
	static int T2_COMP_count_1 = 0; //counter to count
	T2_COMP_count_0++;
	if (T2_OVF_No == 1)
	{
		if (T2_COMP_count_0 == T2_OVF_No)
		{
			//write your code here

			T2_COMP_count_0 = 0;
			OCR2 = T2_Init_val;
		}
	}
	else
	{
		if (T2_COMP_count_0 == 1)
		{
			OCR2 = T2_Init_val_OVF;
		}
		else if (T2_COMP_count_0 == T2_OVF_No)
		{
			//write your code here

			T2_COMP_count_0 = 0;
			OCR2 = T2_Init_val;
		}
	}
}