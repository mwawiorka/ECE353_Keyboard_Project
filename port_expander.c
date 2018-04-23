#include "port_expander.h"

#define LEFT 		2
#define RIGHT 	3
#define UP 			0
#define DOWN 		1

void data_pin_clear(uint8_t pin_number){
	GPIOB->DATA &= ~(1 << pin_number);
}

bool left_sw_pressed(){
	if(GPIOB->DATA & (1 << LEFT)){
		data_pin_clear(LEFT);
		return true;
	} else {
		return false;
	}
}
bool right_sw_pressed(){
	if(GPIOB->DATA & (1 << RIGHT)){
		data_pin_clear(RIGHT);
		return true;
	} else {
		return false;
	}
}
bool up_sw_pressed(){
	if(GPIOB->DATA & (1 << UP)){
		data_pin_clear(UP);
		return true;
	} else {
		return false;
	}
}
bool down_sw_pressed(){
	if(GPIOB->DATA & (1 << DOWN)){
		data_pin_clear(DOWN);
		return true;
	} else {
		return false;
	}
}

void port_expander_init(){
	gpio_enable_port(GPIOB_BASE);
	gpio_config_digital_enable(GPIOB_BASE, 0x0F);
	gpio_config_enable_input(GPIOB_BASE, 0x0F);
	gpio_config_alternate_function_clear(GPIOB_BASE);
	gpio_config_enable_pullup(GPIOB_BASE, 0x0F);
}