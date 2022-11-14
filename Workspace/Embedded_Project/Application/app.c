/******************************************************************************
 *
 * Module: Application
 *
 * File Name: app.c
 *
 * Description: Source file for the application layer
 *
 * Author: Moaz Mohamed
 *
 *******************************************************************************/

/*******************************************************************************
 *                                Includes                                     *
 *******************************************************************************/
#include "app.h"
#include "../HAL/led.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Global flags indicating which led is currently on in the car traffic lights*/
static boolean g_car_red_on=FALSE;
static boolean g_car_yellow_on=FALSE;
static boolean g_car_green_on=FALSE;

/*Global Timer0 configuration structure used in normal mode, it's configured to trigger an interrupt every second*/
static Timer0_ConfigType Timer0_Struct={
		0,0,TIMER0_F_CPU_256,TIMER0_NORMAL_MODE
};

/*Global Timer1 configuration structure used in normal mode, it's configured to trigger an interrupt every second*/
static Timer1_ConfigType Timer1_Struct={
		0,31250,TIMER1_F_CPU_256,TIMER1_COMPARE_MODE
};

/*Variables used in normal mode logic*/
/*This variable indicates which snippet of code to be executed in the switch statement*/
LED_State normal_state=GREEN;

/*Counter used to count the number of interrupts to reaching 1 second*/
uint16 normal_interrupt_counter=0;

/*Counter used in counting the number of yellow LED blinks*/
uint8 normal_yellow_LED_counter=0;
boolean normal_yellow_on=TRUE;

/*Flag to prevent multiple interrupts to be triggered by the button*/
boolean g_button_interrupt_flag=TRUE;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : This functions starts the application.
 */
void app_start()
{
	/*External interrupt 0 configuration*/
	EXT_Interrupts_ConfigType EXT_Interrupt_Struct={
			EXT_INT0,EXT_FALLING_EDGE,button_callBack
	};

	/*Enable the I-bit*/
	SREG|=(1<<7);

	/*Setup Timer1 callback, .i.e, the pedestrian mode logic*/
	Timer1_setCallBack(timer1_callBack);
	/*Setup Timer0 callback, .i.e, the normal mode logic*/
	Timer0_setCallBack(timer0_callBack);
	/*Configure the LEDs*/
	setup_LEDs();
	/*Configure INT0*/
	EXT_Interrupts_interruptSetup(&EXT_Interrupt_Struct);
	/*Start Timer0, thus start the system in normal mode*/
	Timer0_init(&Timer0_Struct);

	while(1);
}

/*
 * Description : This functions configure the pins that are connected to the leds as output pins
 */
void setup_LEDs()
{
	uint8 pin_index=0;

	for(pin_index=0; pin_index<3; pin_index++)
	{
		LED_setup(CAR_LEDS_PORT_ID, pin_index);
		LED_setup(PEDESTRIANS_PORT_ID, pin_index);
	}
}

/*
 * Description : This functions contains the logic of the instructions to be executed when the button triggers an interrupt.
 */
void button_callBack()
{
	if(g_button_interrupt_flag==TRUE)
	{
		g_button_interrupt_flag=FALSE;
		Timer0_deInit();
		Timer1_init(&Timer1_Struct);
	}
}

/*
 * Description : This functions contains the logic of the instructions to be executed when Timer1 triggers an interrupt.
 */
void timer1_callBack()
{
	static uint8 seconds=0;
	static boolean yellow_on=TRUE;
	static Mode_Switching mode_state=ENTERING_PEDESTRIANS;
	static Mode_Switching mode_subState=ENTERING_PEDESTRIANS;
	if(g_car_red_on==TRUE)
	{
		switch(mode_state)
		{
		case ENTERING_PEDESTRIANS:
			LED_off(PEDESTRIANS_PORT_ID,RED_LED);
			LED_on(CAR_LEDS_PORT_ID,RED_LED);
			LED_on(PEDESTRIANS_PORT_ID,GREEN_LED);
			seconds++;
			if(seconds==5)
			{
				LED_off(CAR_LEDS_PORT_ID,RED_LED);
				LED_on(CAR_LEDS_PORT_ID,YELLOW_LED);
				LED_on(PEDESTRIANS_PORT_ID,YELLOW_LED);
				seconds=0;
				mode_state=RETURNING_TO_NORMAL;
			}
			break;
		case RETURNING_TO_NORMAL:
			seconds++;
			if(seconds==5)
			{
				seconds=0;
				mode_state=INITIALIZING_FOR_NORMAL;
				LED_off(CAR_LEDS_PORT_ID,YELLOW_LED);
				LED_off(PEDESTRIANS_PORT_ID,YELLOW_LED);
				LED_on(CAR_LEDS_PORT_ID,RED_LED);
				LED_on(PEDESTRIANS_PORT_ID,RED_LED);
				LED_off(PEDESTRIANS_PORT_ID,GREEN_LED);
			}
			else
			{
				LED_toggle(CAR_LEDS_PORT_ID,YELLOW_LED);
				LED_toggle(PEDESTRIANS_PORT_ID,YELLOW_LED);
			}
			break;
		case INITIALIZING_FOR_NORMAL:
			seconds++;
			if(seconds==5)
			{
				seconds=0;
				yellow_on=TRUE;
				mode_state=ENTERING_PEDESTRIANS;
				LED_off(CAR_LEDS_PORT_ID,RED_LED);
				reinitializeForNormalMode();
				g_button_interrupt_flag=TRUE;
				Timer1_deInit();
				Timer0_init(&Timer0_Struct);
			}
			break;
		}
	}
	else if(g_car_green_on==TRUE || g_car_yellow_on==TRUE)
	{
		switch (mode_state)
		{
		case ENTERING_PEDESTRIANS:
			switch (mode_subState)
			{
			case ENTERING_PEDESTRIANS:
				LED_on(PEDESTRIANS_PORT_ID, RED_LED);
				LED_off(CAR_LEDS_PORT_ID,GREEN_LED);
				if(yellow_on==TRUE)
				{
					yellow_on=FALSE;
					LED_on(PEDESTRIANS_PORT_ID,YELLOW_LED);
					LED_on(CAR_LEDS_PORT_ID,YELLOW_LED);
				}
				else
				{
					seconds++;
					if(seconds==5)
					{
						LED_off(PEDESTRIANS_PORT_ID,YELLOW_LED);
						LED_off(CAR_LEDS_PORT_ID,YELLOW_LED);
						LED_on(CAR_LEDS_PORT_ID,RED_LED);
						LED_off(CAR_LEDS_PORT_ID,GREEN_LED);
						LED_on(PEDESTRIANS_PORT_ID,GREEN_LED);
						LED_off(PEDESTRIANS_PORT_ID, RED_LED);
						seconds=0;
						yellow_on=TRUE;
						mode_subState = RETURNING_TO_NORMAL;
					}
					else
					{
						LED_toggle(PEDESTRIANS_PORT_ID,YELLOW_LED);
						LED_toggle(CAR_LEDS_PORT_ID,YELLOW_LED);
					}
				}
				break;
			case RETURNING_TO_NORMAL:
				seconds++;
				if(seconds==5)
				{
					seconds=0;
					LED_off(CAR_LEDS_PORT_ID,RED_LED);
					LED_on(PEDESTRIANS_PORT_ID,YELLOW_LED);
					LED_on(CAR_LEDS_PORT_ID,YELLOW_LED);
					mode_subState = ENTERING_PEDESTRIANS;
					mode_state=RETURNING_TO_NORMAL;
				}
				break;
			case INITIALIZING_FOR_NORMAL:
				/*Do Nothing*/
				break;
			}
			break;
			case RETURNING_TO_NORMAL:
				seconds++;
				if(seconds==5)
				{
					seconds=0;
					mode_state=INITIALIZING_FOR_NORMAL;
					LED_off(CAR_LEDS_PORT_ID,YELLOW_LED);
					LED_off(PEDESTRIANS_PORT_ID,YELLOW_LED);
					LED_on(CAR_LEDS_PORT_ID,RED_LED);
					LED_on(PEDESTRIANS_PORT_ID,RED_LED);
					LED_off(PEDESTRIANS_PORT_ID,GREEN_LED);
				}
				else
				{
					LED_toggle(CAR_LEDS_PORT_ID,YELLOW_LED);
					LED_toggle(PEDESTRIANS_PORT_ID,YELLOW_LED);
				}
				break;
			case INITIALIZING_FOR_NORMAL:
				seconds++;
				if(seconds==5)
				{
					seconds=0;
					yellow_on=TRUE;
					mode_state=ENTERING_PEDESTRIANS;
					mode_subState=ENTERING_PEDESTRIANS;
					LED_off(CAR_LEDS_PORT_ID,RED_LED);
					reinitializeForNormalMode();
					g_button_interrupt_flag=TRUE;
					Timer1_deInit();
					Timer0_init(&Timer0_Struct);
				}
				break;
		}
	}
	else
	{

	}
}

/*
 * Description : This functions contains the logic of the instructions to be executed when Timer0 triggers an interrupt.
 */
void timer0_callBack()
{
	LED_on(PEDESTRIANS_PORT_ID,RED_LED);
	switch (normal_state)
	{
	case GREEN:
		LED_on(CAR_LEDS_PORT_ID, GREEN_LED);
		g_car_green_on=TRUE;
		g_car_red_on=FALSE;
		g_car_yellow_on=FALSE;
		normal_interrupt_counter++;
		if(normal_interrupt_counter==610)
		{
			LED_off(CAR_LEDS_PORT_ID, GREEN_LED);
			g_car_green_on=FALSE;
			normal_interrupt_counter=0;
			normal_state=YELLOW;
		}
		else
		{

		}
		break;
	case YELLOW:
		if(normal_yellow_on==TRUE)
		{
			g_car_yellow_on=TRUE;
			g_car_red_on=FALSE;
			g_car_green_on=FALSE;
			LED_on(CAR_LEDS_PORT_ID, YELLOW_LED);
			normal_yellow_on=FALSE;
		}
		normal_interrupt_counter++;
		if(normal_interrupt_counter==122)
		{
			normal_yellow_LED_counter++;
			normal_interrupt_counter=0;
			if(normal_yellow_LED_counter==5)
			{
				LED_off(CAR_LEDS_PORT_ID, YELLOW_LED);
				g_car_yellow_on=FALSE;
				normal_yellow_LED_counter=0;
				normal_interrupt_counter=0;
				normal_yellow_on=TRUE;
				normal_state=RED;
			}
			else
			{
				LED_toggle(CAR_LEDS_PORT_ID, YELLOW_LED);
			}
		}
		else
		{

		}
		break;
	case RED:
		LED_on(CAR_LEDS_PORT_ID, RED_LED);
		g_car_yellow_on=FALSE;
		g_car_red_on=TRUE;
		g_car_green_on=FALSE;
		if(normal_interrupt_counter<610)
		{
			normal_interrupt_counter++;
		}
		else
		{
			LED_off(CAR_LEDS_PORT_ID, RED_LED);
			g_car_red_on=FALSE;
			normal_interrupt_counter=0;
			normal_state=GREEN;
		}
		break;
	}
}

/*
 * Description : This functions reinitialize global values used in the callback function of timer0.
 */
void reinitializeForNormalMode()
{
	normal_state=GREEN;
	normal_interrupt_counter=0;
	normal_yellow_LED_counter=0;
	normal_yellow_on=TRUE;
}


