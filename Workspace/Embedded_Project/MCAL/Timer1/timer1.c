/******************************************************************************
 *
 * Module: Timer1 Driver
 *
 * File Name: timer1.c
 *
 * Description: Source file for AVR Timer1 Driver
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Global pointer to hold the address of the call back function*/
static volatile void(*g_callBackPtr)(void)=NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
/*ISR for the compare mode*/
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr!=NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*ISR for the overflow mode*/
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr!=NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Function to initialize the Timer driver.
 */
Timer1_FunctionState Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	if(Config_Ptr->mode>1 || Config_Ptr->prescaler == 0 || Config_Ptr->prescaler >5)
	{
		return TIMER1_ERROR;
	}
	else
	{
		switch (Config_Ptr->mode)
		{
		case TIMER1_NORMAL_MODE:
			/*
			 * FOC1A=1 , FOC1B=1  ----> NON-PWM
			 */
			TCCR1A=(1<<FOC1A)|(1<<FOC1B);

			/*Assign initial value to TCNT1*/
			TCNT1=Config_Ptr->initial_value;

			/*Enable Overflow mode interrupt*/
			TIMSK|=(1<<TOIE1);

			/*Set the clock prescaler and start the timer*/
			TCCR1B=(Config_Ptr->prescaler<<CS10);
			break;
		case TIMER1_COMPARE_MODE:
			/*
			 * FOC1A=1 , FOC1B=1  ----> NON-PWM
			 */
			TCCR1A=(1<<FOC1A)|(1<<FOC1B);

			/*Assign the compare match value to OCR1A*/
			OCR1A=Config_Ptr->compare_value;

			/*Assign initial value to TCNT1*/
			TCNT1=Config_Ptr->initial_value;

			/*Enable compare mode interrupt*/
			TIMSK=(1<<OCIE1A);

			/*
			 * Set the clock prescaler and start the timer.
			 * WGM12=1 --> COMPARE MODE
			 */
			TCCR1B=(Config_Ptr->prescaler<<CS10)|(1<<WGM12);
			break;
		}
		return TIMER1_SUCCESS;
	}
}

/*
 * Description :
 * Function to disable the Timer driver.
 */
void Timer1_deInit(void)
{
	/*Reset all registers*/
	TCCR1A=0;
	TCCR1B=0;
	TIMSK&=~(1<<TOIE1)&~(1<<OCIE1A);
	TCNT1=0;
	OCR1A=0;
}

/*
 * Description :
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
		/*Assign the address of the call back function to the global pointer*/
		g_callBackPtr=a_ptr;
}
