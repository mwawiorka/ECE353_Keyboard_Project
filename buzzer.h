#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "pwm.h"

#define C		1047
#define C_s		1109
#define D		1175
#define D_s		1245
#define	E		1319
#define	F		1397
#define	F_s		1480
#define	G		1568
#define	G_s		1661
#define	A		1760
#define	A_s		1865
#define	B		1976

/******************************************************************************
 * Sends PWM signal to buzzer
 *
 *****************************************************************************/
void buzz( int frequency );

/******************************************************************************
 * Stops PWM signal to buzzer
 *
 *****************************************************************************/
void stop_buzz(void);

#endif
