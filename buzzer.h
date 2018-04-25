#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "pwm.h"

#define C		523
#define C_s		554
#define D		587
#define D_s		622
#define	E		659
#define	F		698
#define	F_s		740
#define	G		784
#define	G_s		831
#define	A		880
#define	A_s		932
#define	B		988

/******************************************************************************
 * Sends PWM signal to buzzer
 *
 *****************************************************************************/
void buzz( int frequency );

/******************************************************************************
 * Stops PWM signal to buzzer
 *
 *****************************************************************************/
void stop_buzz( );

#endif
