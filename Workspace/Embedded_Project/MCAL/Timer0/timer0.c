/******************************************************************************
 *
 * Module: Timer0 Driver
 *
 * File Name: timer0.c
 *
 * Description: Source file for AVR Timer0 Driver
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "timer0.h"
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
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr!=NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*ISR for the overflow mode*/
ISR(TIMER0_OVF_vect)
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
Timer0_FunctionState Timer0_init(const Timer0_ConfigType * Config_Ptr)
{
	if(Config_Ptr->mode>1 || Config_Ptr->prescaler == 0 || Config_Ptr->prescaler >5)
	{
		return TIMER0_ERROR;
	}
	else
	{
		switch (Config_Ptr->mode)
		{
		case TIMER0_NORMAL_MODE:
			/*
			 * FOC0=1  ----> NON-PWM
			 */
			TCCR0=(1<<FOC0);

			/*Assign initial value to TCNT0*/
			TCNT0=Config_Ptr->initial_value;

			/*Enable Overflow mode interrupt*/
			TIMSK|=(1<<TOIE0);

			/*Set the clock prescaler and start the timer*/
			TCCR0=(Config_Ptr->prescaler<<CS00);
			break;
		case TIMER0_COMPARE_MODE:
			/*
			 * FOC0=1  ----> NON-PWM
			 */
			TCCR0=(1<<FOC0)|(1<<WGM01);

			/*Assign the compare match value to OCR0*/
			OCR0=Config_Ptr->compare_value;

			/*Assign initial value to TCNT0*/
			TCNT0=Config_Ptr->initial_value;

			/*Enable compare mode interrupt*/
			TIMSK=(1<<OCIE0);

			/*
			 * Set the clock prescaler and start the timer.
			 */
			TCCR0=(Config_Ptr->prescaler<<CS00);
			break;
		}
		return TIMER0_SUCCESS;
	}
}

/*
 * Description :
 * Function to disable the Timer driver.
 */
void Timer0_deInit(void)
{
	/*Reset all registers*/
	TCCR0=0;
	TIMSK&=~(1<<TOIE0)&~(1<<OCIE0);
	TCNT0=0;
	OCR0=0;
}

/*
 * Description :
 * Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*a_ptr)(void))
{
		/*Assign the address of the call back function to the global pointer*/
		g_callBackPtr=a_ptr;
}
