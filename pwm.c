#include "pwm.h"

static TIMER0_Type *pwm = (TIMER0_Type *) PWM_TIMER_BASE;
int DUTY_CYCLE = 25000000;

void init_pwm() {
	SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R1;
	while((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R1) == 0) {};
	
	gpio_enable_port(PWM_GPIO_BASE);
	gpio_config_digital_enable(PWM_GPIO_BASE, PWM_PIN);
	gpio_config_alternate_function(PWM_GPIO_BASE, PWM_PIN);
	gpio_config_port_control(PWM_GPIO_BASE, PWM_PIN_PCTL_M, PWM_PIN_PCTL);
		
	// Disable timer before making any changes.
	pwm->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	
	// Configure as a 16-bit timer
	pwm->CFG = TIMER_CFG_16_BIT;
	
	// Set timer to PWM Mode, Edge-Count Mode, and Periodic Timer Mode.
	pwm->TAMR &= ~TIMER_TAMR_TACMR;
	pwm->TAMR |= TIMER_TAMR_TAAMS | TIMER_TAMR_TAMR_PERIOD;
	
	// Configure PWM signal to not be inverted
	pwm->CTL &= ~(TIMER_CTL_TAPWML);	
	
	// Initially set duty cycle to 50%
	set_duty_cycle(50);	
		
	disable_pwm();	
};

void set_duty_cycle( int duty_cycle ) {
	if (duty_cycle <= 0) {
		DUTY_CYCLE = 10000;
	}
	else if (duty_cycle > 100) {
		DUTY_CYCLE = CLOCK_FREQUENCY;
	} 
	else {
		DUTY_CYCLE = (((float) duty_cycle)/100) * CLOCK_FREQUENCY;
	}	
};

void gen_frequency( int frequency ) {
	pwm->TAPR = 0;
	pwm->TAILR = CLOCK_FREQUENCY/frequency;	// Set timer period as the # of clock cycles corresponding to the input frequency
	
	pwm->TAPMR = 0;
	pwm->TAMATCHR = (DUTY_CYCLE)/frequency;	// Invert PWM signal when duty cycle * period has passed
	
	pwm->CTL |= TIMER_CTL_TAEN;
};

void disable_pwm() {
	pwm->CTL &= ~TIMER_CTL_TAEN;
};
