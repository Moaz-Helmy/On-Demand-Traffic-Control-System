/******************************************************************************
 *
 * Module: External Interrupts
 *
 * File Name: external_interrupts.c
 *
 * Description: Source file for External Interrupts
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "external_interrupts.h"
#include "../GPIO/gpio.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
static volatile void(*g_callBackPtr0)(void)=NULL_PTR;
static volatile void(*g_callBackPtr1)(void)=NULL_PTR;
static volatile void(*g_callBackPtr2)(void)=NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(INT0_vect)
{
	if(g_callBackPtr0!=NULL_PTR)
	{
		(*g_callBackPtr0)();
	}
}

ISR(INT1_vect)
{
	if(g_callBackPtr1!=NULL_PTR)
	{
		(*g_callBackPtr1)();
	}
}

ISR(INT2_vect)
{
	if(g_callBackPtr2!=NULL_PTR)
	{
		(*g_callBackPtr2)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : This function enables the desired external interrupt with the desired interrupt trigger, besides setting the callback function.
 */
EXT_Interrupts_FunctionState EXT_Interrupts_interruptSetup(EXT_Interrupts_ConfigType* ConfigStruct)
{

	if(ConfigStruct->INT>2 || ConfigStruct->INT<0 || ConfigStruct->Edge>3 || ConfigStruct->Edge<0 || ConfigStruct->callBackPtr==NULL_PTR)
	{
		return EXT_INTERRUPT_ERROR;
	}
	else
	{
		switch (ConfigStruct->INT)
		{
		case EXT_INT0:
			MCUCR=(MCUCR&0xFC)|(ConfigStruct->Edge);
			g_callBackPtr0=ConfigStruct->callBackPtr;
			GICR|=(1<<INT0);
			GPIO_setupPinDirection(PORTD_ID, PIN2_ID, PIN_INPUT);
			break;
		case EXT_INT1:
			MCUCR=(MCUCR&0xF3)|(ConfigStruct->Edge<<ISC10);
			g_callBackPtr1=ConfigStruct->callBackPtr;
			GICR|=(1<<INT1);
			GPIO_setupPinDirection(PORTD_ID, PIN3_ID, PIN_INPUT);
			break;
		case EXT_INT2:
			if(ConfigStruct->Edge == 2)
			{
				CLEAR_BIT(MCUCSR,ISC2);
				g_callBackPtr2=ConfigStruct->callBackPtr;
				GICR|=(1<<INT2);
				GPIO_setupPinDirection(PORTB_ID, PIN2_ID, PIN_INPUT);
			}
			else if(ConfigStruct->Edge==3)
			{
				SET_BIT(MCUCSR,ISC2);
				g_callBackPtr2=ConfigStruct->callBackPtr;
				GICR|=(1<<INT2);
				GPIO_setupPinDirection(PORTB_ID, PIN2_ID, PIN_INPUT);
			}
			else
			{
				return EXT_INTERRUPT_ERROR;
			}
			break;
		}
		return EXT_INTERRUPT_SUCCESS;
	}
}




