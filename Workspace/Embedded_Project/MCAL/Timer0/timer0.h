/******************************************************************************
 *
 * Module: Timer0 Driver
 *
 * File Name: timer0.h
 *
 * Description: Header file for AVR Timer0 Driver
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

#ifndef TIMER0_H_
#define TIMER0_H_

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "../../Definitions_and_Macros/std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	TIMER0_NO_PRESCALER=1,TIMER0_F_CPU_8,TIMER0_F_CPU_64,TIMER0_F_CPU_256,TIMER0_F_CPU_1024
}Timer0_Prescaler;

typedef enum{
	TIMER0_NORMAL_MODE,TIMER0_COMPARE_MODE
}Timer0_Mode;

typedef struct {
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	Timer0_Prescaler prescaler;
	Timer0_Mode mode;
} Timer0_ConfigType;

typedef enum {
	TIMER0_ERROR,TIMER0_SUCCESS
}Timer0_FunctionState;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function to initialize the Timer driver.
 */
Timer0_FunctionState Timer0_init(const Timer0_ConfigType * Config_Ptr);

/*
 * Description :
 * Function to disable the Timer driver.
 */
void Timer0_deInit(void);

/*
 * Description :
 * Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*a_ptr)(void));

#endif /* TIMER0_H_ */
