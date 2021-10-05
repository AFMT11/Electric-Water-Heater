/*
 * Elect_Wat_Heater.c
 *
 * Created: 10/1/2021 12:47:43 PM
 * Author : ahmed
 */ 


#define F_CPU 16000000
#include <util/delay.h>
#include <stdio.h>

#include "DIO.h"
#include "Button.h"
#include "LED.h"
#include "LCD.h"
#include "Segment7.h"
#include "Timer.h"
#include "ds18b20.h"
#include "EEPROM.h"

Uint8 Tens = 0, Ones = 0;  // ones & tens of any number to be displayed on 7-segment
                           // Ones displayed on the right 7-segment
						   // Tens displayed on the left 7-segment

Uint8 Mode_Selector = 1;   // A variable to control which mode is currently on
                           // whether to switch between display temp, or setting temp

Uint8 counter = 0;         // ON | OFF counter  

Uint8 Task_Timer_Switch = 1;  // variable to control which task of each Timer can be implemented
Uint8 S7_Blinker = 2; // variable to control when 7-segment to blink based on if it is even or odd

int temp = 0; // variable to store the converted temp from DS18B20

extern int T2_OVF_count_1; // counter of ISR of Timer 2 which by its value we can
                           // control when the functions in ISR will be implemented

int main(void)
{
	//Setting control pins of Heating/Cooling elements to Zero volt
	DIO_SetPinVal(DIO_PORTC, DIO_PIN_3, DIO_PIN_OFF);
	DIO_SetPinVal(DIO_PORTC, DIO_PIN_4, DIO_PIN_OFF);
	LED0_OFF();
	
	// Initializations of drivers & peripherals
	S7_INIT();
	EEPROM_INIT();
	
 	TIMER0_INIT();
 	TIMER0_SetDelay(10);
 	TIMER0_Start();
	
	TIMER2_INIT();
	TIMER2_SetDelay(100);
	
	Uint8 d;
	
	Uint8 Mode_A = 0; // Two variables to control once we are in a Mode, only 
	Uint8 Mode_B = 0; // a couple of functions to be executed once, not for every time
	                  // the loop will execute. Mode_A for temp display mode, & Mode_B
					  // for setting display mode.
	
	int Temp_Convert_Count = 0; // the pointer of the array that will contains the 10 last reads from DS18B20
	int Temp_buffer_Array[10]; // Array consists of 10 elements to store the last 10 reads of DS18B20
	int Temp_Sum = 0;       // variable to store the sum of the last 10 reads from DS18B20
	Uint8 Average_Temp = 0; // variable to store the average value of the last 10 reads from DS18B20
	
	Uint8 EEPROM_data_Add = 0X20; // Address of the byte of EEPROM that we access
	                              // to set and to read the set desired temp in it
	Uint8 EEPROM_page = 1;        // page of EEPROM that we want to store our desired temp in it
	Uint8 Desired_Temp = EEPROM_ReadByte(EEPROM_page, EEPROM_data_Add); // variable to store the read temp from EEPROM
	                                                                    // based on the page and the address of the location we want to access
	
    /* Replace with your application code */
    while (1) 
    {
		counter = BUTTON2_GetPinVal() == 1? ++counter : counter;  
		
		if (counter % 2)  // Odd value (ON Mode)
		{
			if (Mode_Selector)  // Temperature Display Mode
			{
				if (Mode_A == 0) // one time executed block
				{
					Mode_A = 1;
					Mode_B = 0;
					Task_Timer_Switch = 1;
					S7_Blinker = 2;
					TIMER0_Start();
				}
				//Start conversion (without ROM matching)
				ds18b20convert( &PORTA, &DDRA, &PINA, ( 1 << 0 ), NULL );
				//Delay (sensor needs time to perform conversion)
				_delay_ms( 100 );

				//Read temperature (without ROM matching)
				ds18b20read( &PORTA, &DDRA, &PINA, ( 1 << 0 ), NULL, &temp );
				//Somehow use data stored in `temp` variable
				
				
				Temp_buffer_Array[Temp_Convert_Count] = temp; // store the current temp in the pointed element of
				                                              // Temp_buffer_Array array based on the pointer
															  // Temp_Convert_Count
				
				Temp_Convert_Count++; // pointer of Temp_buffer_Array array
				
				if (Temp_Convert_Count >= 10)
				{
					Temp_Convert_Count = 9; // Back pointer to the last element of array
					
					//calculate the sum of all 10 converted temp reads
					Temp_Sum = Temp_buffer_Array[0] + Temp_buffer_Array[1] + Temp_buffer_Array[2] + Temp_buffer_Array[3] + Temp_buffer_Array[4] + Temp_buffer_Array[5] + Temp_buffer_Array[6] + Temp_buffer_Array[7] + Temp_buffer_Array[8] + Temp_buffer_Array[9];
					
					Average_Temp = (Uint8) (Temp_Sum / 10); // Average temp of 10 temp reads
					
					Ones = Average_Temp % 10;
					Tens = Average_Temp / 10;			
					
					// loop to swap elements of array
					for (d = 0; d < 9; d++)
					{
						Temp_buffer_Array[d] = Temp_buffer_Array[d+1];
					}					
				}		
				
				if (Desired_Temp > Average_Temp) // conditions to control when to turn heating/ cooling element on/ off
				{                                   
					TIMER2_Start();
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_3, DIO_PIN_OFF);
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_4, DIO_PIN_ON);
					
				}
				else if (Desired_Temp < Average_Temp)
				{
					TIMER2_Stop();
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_3, DIO_PIN_ON);
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_4, DIO_PIN_OFF);		
					LED0_ON();		
				}
				else
				{
					TIMER2_Stop();
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_3, DIO_PIN_OFF);
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_4, DIO_PIN_OFF);
					LED0_OFF();
					LED1_OFF();
				}							
				
				// Switch to Setting Temperature Mode
				if (BUTTON0_GetPinVal() | BUTTON1_GetPinVal()) 
				{
					Mode_Selector = 0;
				}
						
			}
			else  // Setting Temperature Mode
			{
				if (Mode_B == 0) // one time executed block
				{
					Mode_A = 0;
					Mode_B = 1;
					
					T2_OVF_count_1 = 0;	
					Task_Timer_Switch = 0;
					TIMER2_Start();	
						
					// Turn off LEDs
					LED0_OFF();
					LED1_OFF();
					// Turn off Actuators
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_3, DIO_PIN_OFF);
					DIO_SetPinVal(DIO_PORTC, DIO_PIN_4, DIO_PIN_OFF);			
				}				
			
				Ones = Desired_Temp % 10;
				Tens = Desired_Temp / 10;	
							
				if (BUTTON0_GetPinVal()) // up button to increase the desired temp
				{
					T2_OVF_count_1 = 0;				
					Desired_Temp = (Desired_Temp - 5) >= 35? Desired_Temp - 5 : Desired_Temp;
					Ones = Desired_Temp % 10;
					Tens = Desired_Temp / 10;
					// onces we change the desired value, the newly desired value should be store at EEPROM
					EEPROM_WriteByte(EEPROM_page, EEPROM_data_Add, Desired_Temp);			
				}
				else if (BUTTON1_GetPinVal()) // down button to decrease the desired temp
				{
					T2_OVF_count_1 = 0;				
					Desired_Temp = (Desired_Temp + 5) <= 75? Desired_Temp + 5 : Desired_Temp;
					Ones = Desired_Temp % 10;
					Tens = Desired_Temp / 10;
					EEPROM_WriteByte(EEPROM_page, EEPROM_data_Add, Desired_Temp);						
				}
			}		
		}
		else  // Even value (Off Mode)
		{
			// Default values once we turn off the heater
			Mode_Selector = 1;
				
			// Turn off Timers
			//TIMER0_Stop();
			TIMER2_Stop();
			
			// Turn off 7-Segments
 			DIO_SetPinVal(DIO_PORTB, DIO_PIN_1, DIO_PIN_OFF);
 			DIO_SetPinVal(DIO_PORTB, DIO_PIN_2, DIO_PIN_OFF);
			
			// Turn off LEDs
			LED0_OFF();
			LED1_OFF();
			
			// Turn off Actuators
			DIO_SetPinVal(DIO_PORTC, DIO_PIN_3, DIO_PIN_OFF);
			DIO_SetPinVal(DIO_PORTC, DIO_PIN_4, DIO_PIN_OFF);
		}
    }
}

