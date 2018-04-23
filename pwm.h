
#ifndef __PWM_H__
#define __PWM_H__

#include <stdint.h>
#include <stdbool.h>
#include "TM4C123GH6PM.h"
#include "driver_defines.h"
#include "timers.h"
#include "gpio_port.h"


#define CLOCK_FREQUENCY		50000000
#define PWM_TIMER_BASE		TIMER1_BASE
#define	PWM_GPIO_BASE		GPIOF_BASE
#define PWM_PIN				PF2
#define	PWM_PIN_PCTL_M		GPIO_PCTL_PF2_M
#define	PWM_PIN_PCTL			GPIO_PCTL_PF2_T1CCP0


/******************************************************************************
 * Initializes PF2 for PWM
 *
 *****************************************************************************/
void init_pwm(  );

/******************************************************************************
 * Sets the duty cycle for the PWM
 *
 * Parameters:
 *  duty_cycle - int representing duty cycle, 0-100%, saturates low to 0 or high to 100
 *
 *****************************************************************************/
void set_duty_cycle( int duty_cycle );

/******************************************************************************
 * Sets the frequency for the PWM, keeping the same duty cycle
 *
 * Parameters:
 *  frequency - int representing the frequency of the PWM, from 0 to 2000
 *
 *****************************************************************************/
void gen_frequency( int frequency );

/******************************************************************************
 * Turns the PWM off
 *
 *****************************************************************************/
void disable_pwm(  );

#endif