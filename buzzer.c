#include "buzzer.h"

void buzz( int frequency ) {
	gen_frequency( frequency );
};

void stop_buzz() {
	disable_pwm();
};
