// Copyright (c) 2014, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include "main.h"

typedef enum {
	DEBOUNCE_WAIT,
	DEBOUNCE_ONE,
	DEBOUNCE_TWO,
	DEBOUNCE_PRESSED,
} DEBOUNCE_STATUS;

typedef enum {
	MENU,
	FOLLOW,
	PLAY,
} mode_t;

typedef enum {
	NA,
	UP_B,
	DOWN_B,
	LEFT_B,
	RIGHT_B,
} button_t;

mode_t mode;
key_t cur_key;
button_t button_pressed;

volatile bool music_beat = false;
volatile bool game_start = false;
volatile bool loading_led = false;
volatile bool read_touch = false;
volatile bool switch_detect = false;
volatile bool buzzer_update = false;
volatile bool joystick_read = false;
volatile bool button_detect = false;
volatile bool game_pause = false;

// What type are the volume and pitch, set accordingly
volatile uint8_t buzzerVolume = 50;
volatile uint8_t buzzerPitch = 50;

volatile int menu_index = 1;

volatile uint16_t note_index = 0;

char *menuList[] = {
	"Free Play/",
	"Mary n Lamb/",
	"Freedom Song/",
};

int menuSize = 3;
//*****************************************************************************
// 
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
// 
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}

bool debounce_switch()
{
  static DEBOUNCE_STATUS state = DEBOUNCE_WAIT;
  bool pin_logic_level = lp_io_read_pin(SW1_BIT);
  
  switch (state)
  {
    case DEBOUNCE_WAIT:
    {
      if(pin_logic_level){
        state = DEBOUNCE_WAIT;
      } else {
        state = DEBOUNCE_ONE;
      }
      break;
    }
    case DEBOUNCE_ONE:
    {
      if(pin_logic_level){
        state = DEBOUNCE_ONE;
      } else {
        state = DEBOUNCE_TWO;
      }
      break;
    }
    case DEBOUNCE_TWO:
    {
      if(pin_logic_level){
        state = DEBOUNCE_ONE;
      } else {
        state = DEBOUNCE_PRESSED;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(pin_logic_level){
        state = DEBOUNCE_ONE;
      } else {
        state = DEBOUNCE_PRESSED;
      }
      break;
    }
    default:
			for(;;);
	}
	
	if(state == DEBOUNCE_TWO ){
    return true;
  } else {
    return false;
  }
}

bool debounce_button()
{
  static DEBOUNCE_STATUS status = DEBOUNCE_WAIT;
	static uint8_t detect_button;
  uint8_t button_detect = mcp23017_read_reg(MCP23017_INTCAPB_R);
  
  switch (status)
  {
    case DEBOUNCE_WAIT:
    {
      if(button_detect != 0x0F && detect_button == button_detect){
        status = DEBOUNCE_ONE;
      } else {
				detect_button = button_detect;
        status = DEBOUNCE_WAIT;
      }
      break;
    }
    case DEBOUNCE_ONE:
    {
      if(button_detect != 0x0F && detect_button == button_detect){
        status = DEBOUNCE_TWO;
      } else {
				detect_button = 0xFF;
        status = DEBOUNCE_ONE;
      }
      break;
    }
    case DEBOUNCE_TWO:
    {
      if(button_detect != 0x0F && detect_button == button_detect){
        status = DEBOUNCE_PRESSED;
      } else {
				detect_button = 0xFF;
        status = DEBOUNCE_ONE;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(button_detect != 0x0F && detect_button == button_detect){
        status = DEBOUNCE_PRESSED;
      } else {
				detect_button = 0xFF;
        status = DEBOUNCE_ONE;
      }
      break;
    }
    default:
			for(;;);
	}
	
	if(status == DEBOUNCE_TWO ){
		if(!(detect_button & (1 <<DIR_BTN_DOWN_PIN))){
			button_pressed = LEFT_B;
		}else if(!(detect_button & (1 <<DIR_BTN_LEFT_PIN))){
			button_pressed = UP_B;
		}else if(!(detect_button & (1 <<DIR_BTN_RIGHT_PIN))){
			button_pressed = DOWN_B;
		}else if(!(detect_button & (1 <<DIR_BTN_UP_PIN))){
			button_pressed = RIGHT_B;
		}else{
			button_pressed = NA;
		}
    return true;
  } else {
		button_pressed = NA;
    return false;
  }
}

void initializeHardware()
{
	DisableInterrupts();
	init_serial_debug(true, true);
	//ft6x06_init();
	mcp23017_init();
	init_pwm();
	
	gp_timer_config_16(TIMER0_BASE, TIMER_TAMR_TAMR_PERIOD | TIMER_TBMR_TBMR_PERIOD, false, true);
	
	lcd_config_gpio();
	lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
	
	lp_io_init();
	
	//ps2_initialize_ss2();
	//initialize_adc_ss2(ADC0_BASE);
	EnableInterrupts();
}

// used for the FOLLOW mode
void TIMER0A_Handler(){
	static uint8_t beatCount = 0;
	
	if(game_pause){
		TIMER0->ICR = TIMER_ICR_TATOCINT;
		return;
	}
	
	beatCount++;
	
	// 15 * 10ms have passed -> 1/16 note at 100BPM
	if (beatCount == 15) {	
		music_beat = true;
		beatCount = 0;
	}
	
	TIMER0->ICR = TIMER_ICR_TATOCINT;
}

void TIMER0B_Handler(){
	
	switch_detect = true;	
	
	if(game_pause){
		TIMER0->ICR = TIMER_ICR_TBTOCINT;
		return;
	}
	
	read_touch = true;
	
	buzzer_update = true;
	
	TIMER0->ICR = TIMER_ICR_TBTOCINT;
}

void ADC0SS2_Handler(){
	
	if(game_pause){
		ADC0->ISC = ADC_ISC_IN2;
		return;
	}
	
	joystick_read = true;
	
	ADC0->ISC = ADC_ISC_IN2;
}

void GPIOF_Handler(){
	
	if(game_pause){
		GPIOF->ICR = GPIO_ICR_GPIO_M;
		return;
	}
	
	button_detect = true;
	GPIOF->ICR = GPIO_ICR_GPIO_M;
}

void buzzer_play(){
	// check the current key state
	// if cont then continue playing
	// if on silent then stop playing
	// else play the key state
	switch(cur_key){
		case Sil:
			stop_buzz();
			break;
		case An:
			buzz(A);
			break;
		case As:
			buzz(A_s);
			break;
		case Bn:
			buzz(B);
			break;
		case Cn:
			buzz(C);
			break;
		case Cs:
			buzz(C_s);
			break;
		case Dn:
			buzz(D);
			break;
		case Ds:
			buzz(D_s);
			break;
		case En:
			buzz(E);
			break;
		case Fn:
			buzz(F);
			break;
		case Fs:
			buzz(F_s);
			break;
		case Gn:
			buzz(G);
			break;
		case Gs:
			buzz(G_s);
			break;
		default:
			break;
	}
}

void pitchChange(uint8_t percentage){
	// percentage is from 0-100
	// 50 is normal 1
	// 0 is 1/2, 100 is 2x
}

void volumeChange(uint8_t percentage){
	// percentage is from 0-100
	// 50 is normal 1
	// 0 is 1/2, 100 is 2x
}

key_t checkKey(uint16_t x, uint16_t y){
	key_t retKey;
	// if x is less than half row, check sharp
		// case y for sharp and return
	if(x<COLS/2){
		if(y<29){
			retKey = Sil;
		}else if(y<59){
			retKey = As;
		}else if(y<95){
			retKey = Gs;
		}else if(y<151){
			retKey = Fs;
		}else if(y<212){
			retKey = Sil;
		}else if(y<243){
			retKey = Ds;
		}else if(y<289){
			retKey = Cs;
		}else{
			retKey = Sil;
		}
	}
	// else check standard
		// case y for standard and return
	else{
		if(y<44){
			retKey = Bn;
		}else if(y<90){
			retKey = An;
		}else if(y<136){
			retKey = Gn;
		}else if(y<182){
			retKey = Fn;
		}else if(y<228){
			retKey = En;
		}else if(y<274){
			retKey = Dn;
		}else{
			retKey = Cn;
		}
	}
	
	return retKey;
}

uint16_t getkeyboardLocation(key_t key){
	switch(key){
		case As:
			return As_Loc;
			break;
		case Gs:
			return Gs_Loc;
			break;
		case Fs:
			return Fs_Loc;
			break;
		case Ds:
			return Ds_Loc;
			break;
		case Cs:
			return Cs_Loc;
			break;
		case An:
			return A_Loc;
			break;
		case Bn:
			return B_Loc;
			break;
		case Cn:
			return C_Loc;
			break;
		case Dn:
			return D_Loc;
			break;
		case En:
			return E_Loc;
			break;
		case Fn:
			return F_Loc;
			break;
		case Gn:
			return G_Loc;
			break;
		default:
			for(;;)
			break;
	}
	
	return NULL;
}

void displayTouch(bool clear){
	// given key, highlight the board at the pressed spot
	uint16_t fdark = LCD_COLOR_RED;
	uint16_t flight = LCD_COLOR_RED;
	if(clear){
		fdark = LCD_COLOR_BLACK;
		LCD_COLOR_WHITE;
	}
	
	if(cur_key == As || cur_key == Gs || cur_key == Fs || cur_key == Ds || cur_key == Cs){
			lcd_draw_image(KEYBOARD_BLACK_CENTER, KEYBOARD_BLACK_WIDTH, keyboardLocation[getkeyboardLocation(cur_key)], KEYBOARD_BLACK_HEIGHT, keyboardBitmapBlack, fdark, LCD_COLOR_BLACK);
	}else if(cur_key == An || cur_key == Bn || cur_key == Cn || cur_key == Dn || cur_key == En || cur_key == Fn || cur_key == Gn){
			lcd_draw_image(KEYBOARD_WHITE_CENTER, KEYBOARD_WHITE_WIDTH, keyboardLocation[getkeyboardLocation(cur_key)], KEYBOARD_WHITE_HEIGHT, keyboardBitmapWhite, flight, LCD_COLOR_WHITE);
	}else{
	
	}
	
}

void displayKeytoPlay(key_t key){
	// given key, highlight the board at the pressed spot
	
	if(key == As || key == Gs || key == Fs || key == Ds || key == Cs){
			lcd_draw_image(KEYBOARD_BLACK_CENTER, KEYBOARD_BLACK_WIDTH, keyboardLocation[getkeyboardLocation(key)], KEYBOARD_BLACK_HEIGHT, keyboardBitmapBlack, LCD_COLOR_RED, LCD_COLOR_BLACK);
	}else if(key == An || key == Bn || key == Cn || key == Dn || key == En || key == Fn || key == Gn){
			lcd_draw_image(KEYBOARD_WHITE_CENTER, KEYBOARD_WHITE_WIDTH, keyboardLocation[getkeyboardLocation(key)], KEYBOARD_WHITE_HEIGHT, keyboardBitmapWhite, LCD_COLOR_RED, LCD_COLOR_WHITE);
	}else{
	
	}
	
}

// assign index-1 and index and index+1 to macro colomn locations MENU1 MENU2 MENU3
// assign macro to initial row location MENUROW
// index is at ascii-40
void displayMenu(void){
	char *string;
	char temp;
	uint16_t redirect;
	uint16_t row0; 
	const uint8_t *bitmap;
	// get pointer to index-1 string
	string = menuList[(menu_index == 0) ? menuSize-1: menu_index-1];
	row0 = MENUROW;
	// iterate throught the string and places at row/col using bitmap
	lcd_draw_image(MENU1, 32, ROWS/2, 316, selectBoxHor, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	for(;*string != '/';string++){
		bitmap = &letterBitmaps[letterRedirect[*string-32]];
		lcd_draw_image(MENU1, 16, row0, 16, bitmap, LCD_COLOR_WHITE, LCD_COLOR_BLACK); 
		row0 = row0-16;
	}
	// get pointer to index-1 string
	string = menuList[(menu_index)%menuSize];
	row0 = MENUROW;
	// iterate throught the string and places at row/col using bitmap
	lcd_draw_image(MENU2, 32, ROWS/2, 316, selectBoxHor, LCD_COLOR_ORANGE, LCD_COLOR_BLACK);
	for(;*string != '/';string++){
		bitmap = &letterBitmaps[letterRedirect[*string-32]];
		lcd_draw_image(MENU2, 16, row0, 16, bitmap, LCD_COLOR_BLUE, LCD_COLOR_BLACK); 
		row0 = row0-16;
	}
	// get pointer to index-1 string
	string = menuList[(menu_index+1)%menuSize];
	row0 = MENUROW;
	// iterate throught the string and places at row/col using bitmap
	lcd_draw_image(MENU3, 32, ROWS/2, 316, selectBoxHor, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	for(;*string != '/';string++){
		bitmap = &letterBitmaps[letterRedirect[*string-32]];
		lcd_draw_image(MENU3, 16, row0, 16, bitmap, LCD_COLOR_GREEN, LCD_COLOR_BLACK); 
		row0 = row0-16;
	}
}

int
main(void)
{
	// set up necessary variable
	bool setup = false;
	
	uint8_t buttons = 0xFF;
	uint8_t temp;

	uint16_t adc_x_val = 0;
	uint16_t adc_y_val = 0;
	uint8_t volumePercent = 50;
	uint8_t pitchPercent = 50;
	
	uint16_t x_touch = 0;
	uint16_t y_touch = 0;
	uint8_t touch_event = false;
	
	mode = MENU;
	cur_key = Sil;
	
	uint16_t songIndex = 0;
	bool songOver = false;
	
	// initialize the hardware
	initializeHardware();
	
	printf("**************************************\n\r");
	printf("PIANO - Project");
	printf("By - DAN and ");
	printf("**************************************\n\r");
	
	// infinite loop for game logic
	while(1){
		temp = mcp23017_read_reg(MCP23017_INTCAPB_R);
		// crude method of pausing a game
		/*
		while(game_pause){
			if(switch_detect){
				game_pause = !debounce_switch();
				switch_detect = false;
			}
		}
		*/
		// check if set up
		if(!setup){
			switch(mode){
				case MENU:
					// set menu index to 1
					menu_index = 1;
					// display arrow/selectbox
					lcd_draw_image(COLS/2, 32, ROWS/2, 316, selectBoxHor, LCD_COLOR_ORANGE, LCD_COLOR_BLACK);
					// display index-1, index, index+1 with %
					displayMenu();
					break;
				case FOLLOW:
					// display keyboard
					// load song at index
					break;
				case PLAY:
					// display keyboard
					break;
				default:
					break;
			}
			setup = true;
		}
		// main menu
		// horizontal orientation
		if(mode == MENU){
			switch(button_pressed){
				case UP_B:
					menu_index--;
					if(menu_index == -1){
						menu_index = menuSize-1;
					}
					displayMenu();
					button_pressed = NA;
					break;
				case DOWN_B:
					menu_index = (menu_index + 1)%menuSize;
					displayMenu();
					button_pressed = NA;
					break;
				case RIGHT_B:
					// change mode to follow if index != 0
					// else change mode to free
					if(menu_index == 0){
						mode = PLAY;
						lcd_draw_image(COLS/2, KEYBOARD_WIDTH, ROWS/2, KEYBOARD_HEIGHT, keyboardBitmap, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
					}else{
						mode = FOLLOW;
						lcd_draw_image(COLS/2, KEYBOARD_WIDTH, ROWS/2, KEYBOARD_HEIGHT, keyboardBitmap, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
					}
					button_pressed = NA;
					break;
				default:
					break;
			}
			
			// draw new 3 songs based on index if still menu
			// change box color on current index if not menu
		} 
		
		
		// PLAY ALONG MODE
		else if(mode == FOLLOW){
			// highlight necessary part of board
			
			if(button_pressed == LEFT_B){
				mode = MENU;
				setup = false;
				// change necessary variables
				// key to silent
				// menu to 1
				// note to 0;
				cur_key = Sil;
				menu_index = 1;
				note_index = 0;
				lcd_draw_image(COLS/2, KEYBOARD_WIDTH, ROWS/2, KEYBOARD_HEIGHT, keyboardBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
			}
		}
		
		// FREE PLAY 
		else{
			// highlight necessary part of board
			if(button_pressed == LEFT_B){
				mode = MENU;
				setup = false;
				// change necessary variables
				// key to silent
				// menu to 1
				cur_key = Sil;
				menu_index = 1;
				lcd_draw_image(COLS/2, KEYBOARD_WIDTH, ROWS/2, KEYBOARD_HEIGHT, keyboardBitmap, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
			}
		
		}	
		
		
		
		if(read_touch && mode != MENU){
			// check if screen is currently pressed and grab values
			touch_event = ft6x06_read_td_status();
			if(touch_event > 0 && touch_event < 3){
				x_touch = ft6x06_read_x();
				y_touch = ft6x06_read_y();
				
				displayTouch(true);
				cur_key = checkKey(x_touch, y_touch); // used to detect change in key for score
				displayTouch(false);
			}else{
				cur_key = Sil; // used to detect change in key for score
			}
				// check key type
				// display pressed key location
			read_touch = false;
		}
		
		if(buzzer_update){
			buzzer_play();
			
			if(music_beat && mode == FOLLOW){
				// if beat index is greater than 0 
					// check if previous played key state match previous key at beat index
						// increase match index for scoring
					// display led color accordingly
				// highlight key at current beat index	
				if (maryHadALittleLamb[songIndex] == End) {
					songOver = true;
				}	else {
					displayKeytoPlay(maryHadALittleLamb[songIndex]);
					songIndex++;
				}	
				
				music_beat = false;
			}
			
			buzzer_update = false;
		}
		
		if(joystick_read){
			// check adc_values
			get_adc_conversion(ADC0_BASE, &adc_x_val, &adc_y_val);
			
			// convert return to a 0-100 scale
			volumePercent = (uint8_t) 100 * (((float)adc_x_val) / 4096);
			pitchPercent = (uint8_t) 100 * (((float)adc_y_val) / 4096);
			// change volume based on x
			volumeChange(volumePercent);
			// change pitch based on y
			pitchChange(pitchPercent);
			joystick_read = false;
		}
		
		if(button_detect){
			//debounce_button();
			
			buttons = mcp23017_read_reg(MCP23017_INTCAPB_R);
			
			if(!(buttons & (1 <<DIR_BTN_DOWN_PIN))){
				button_pressed = LEFT_B;
			}else if(!(buttons & (1 <<DIR_BTN_LEFT_PIN))){
				button_pressed = UP_B;
			}else if(!(buttons & (1 <<DIR_BTN_RIGHT_PIN))){
				button_pressed = DOWN_B;
			}else if(!(buttons & (1 <<DIR_BTN_UP_PIN))){
				button_pressed = RIGHT_B;
			}else{
				button_pressed = NA;
			}
			
			button_detect = false;
		}
		
		if(switch_detect){
			game_pause = debounce_switch();
			switch_detect = false;
		}
	}
}