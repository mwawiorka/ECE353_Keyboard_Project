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
volatile bool game_pause = true;

// What type are the volume and pitch, set accordingly
volatile uint8_t buzzerVolume = 50;
volatile uint8_t buzzerPitch = 50;
volatile uint8_t pitchMultiplier = 1;
volatile uint8_t volumeMultiplier = 1;

volatile int menu_index = 1;

volatile uint16_t note_index = 0;

char *menuList[] = {
	"Free Play/",
	"Mary n Lamb/",
	"Pokemon Theme/",
};

int menuSize = 3;

char *scoreList[] = {
	"abc/",
};

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

void initializeHardware()
{
	DisableInterrupts();
	init_serial_debug(true, true);
	ft6x06_init();
	mcp23017_init();
	init_pwm();
	eeprom_init();
	
	gp_timer_config_16(TIMER0_BASE, TIMER_TAMR_TAMR_PERIOD | TIMER_TBMR_TBMR_PERIOD, false, true);
	
	lcd_config_gpio();
	lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
	
	lp_io_init();
	
	ps2_initialize_ss2();
	initialize_adc_ss2(ADC0_BASE);
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
	
	/* 15 * 10ms have passed -> 1/16 note at 100BPM
		if (beatCount == 15) {		136.		if (beatCount == 15) {	
			music_beat = true;	137.			music_beat = true;
			beatCount = 0;	138.			beatCount = 0;
		}	139.		}
	*/
	// 18.75 * 10ms have passed -> 1/16 note at 80BPM
	if (beatCount == 19) {	
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
			if (pitchMultiplier == 2) {
				buzz(A*2);
			} else if (pitchMultiplier == 3) {
				buzz(A/2);
			} else {		
				buzz(A);
			}	
			break;
		case As:
			if (pitchMultiplier == 2) {
				buzz(A_s*2);
			} else if (pitchMultiplier == 3) {
				buzz(A_s/2);
			} else {
				buzz(A_s);
			}	
			break;
		case Bn:
			if (pitchMultiplier == 2) {
				buzz(B*2);
			} else if (pitchMultiplier == 3) {
				buzz(B/2);
			} else {
				buzz(B);
			}	
			break;
		case Cn:
			if (pitchMultiplier == 2) {
				buzz(C*2);
			} else if (pitchMultiplier == 3) {
				buzz(C/2);
			} else {
				buzz(C);
			}	
			break;
		case Cs:
			if (pitchMultiplier == 2) {
				buzz(C_s*2);
			} else if (pitchMultiplier == 3) {
				buzz(C_s/2);
			} else {
				buzz(C_s);
			}	
			break;
		case Dn:
			if (pitchMultiplier == 2) {
				buzz(D*2);
			} else if (pitchMultiplier == 3) {
				buzz(D/2);
			} else {
				buzz(D);
			}
			break;
		case Ds:
			if (pitchMultiplier == 2) {
				buzz(D_s*2);
			} else if (pitchMultiplier == 3) {
				buzz(D_s/2);
			} else {
				buzz(D_s);
			}	
			break;
		case En:
			if (pitchMultiplier == 2) {
				buzz(E*2);
			} else if (pitchMultiplier == 3) {
				buzz(E/2);
			} else {
				buzz(E);
			}	
			break;
		case Fn:
			if (pitchMultiplier == 2) {
				buzz(F*2);
			} else if (pitchMultiplier == 3) {
				buzz(F/2);
			} else {
				buzz(F);
			}
			break;
		case Fs:
			if (pitchMultiplier == 2) {
				buzz(F_s*2);
			} else if (pitchMultiplier == 3) {
				buzz(F_s/2);
			} else {
				buzz(F_s);
			}	
			break;
		case Gn:
			if (pitchMultiplier == 2) {
				buzz(G*2);
			} else if (pitchMultiplier == 3) {
				buzz(G/2);
			} else {
				buzz(G);
			}
			break;
		case Gs:
			if (pitchMultiplier == 2) {
				buzz(G_s*2);
			} else if (pitchMultiplier == 3) {
				buzz(G_s/2);
			} else {
				buzz(G_s);
			}
			break;
		default:
			break;
	}
}

void pitchChange(uint8_t percentage){
	// percentage is from 0-100
	// 50 is normal 1
	// 0 is 1/2, 100 is 2x
	if (percentage > 75) {
		pitchMultiplier = 2;
	} else if (percentage < 25) {
			pitchMultiplier = 3;
	} else {
		pitchMultiplier = 1;
	}
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
		case Gs:
			return Gs_Loc;
		case Fs:
			return Fs_Loc;
		case Ds:
			return Ds_Loc;
		case Cs:
			return Cs_Loc;
		case An:
			return A_Loc;
		case Bn:
			return B_Loc;
		case Cn:
			return C_Loc;
		case Dn:
			return D_Loc;
		case En:
			return E_Loc;
		case Fn:
			return F_Loc;
		case Gn:
			return G_Loc;
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
		flight = LCD_COLOR_WHITE;
	}
	
	if(cur_key == As || cur_key == Gs || cur_key == Fs || cur_key == Ds || cur_key == Cs){
			lcd_draw_image(KEYBOARD_BLACK_CENTER, KEYBOARD_BLACK_WIDTH, keyboardLocation[getkeyboardLocation(cur_key)], KEYBOARD_BLACK_HEIGHT, keyboardBitmapBlack, fdark, LCD_COLOR_BLACK);
	}else if(cur_key == An || cur_key == Bn || cur_key == Cn || cur_key == Dn || cur_key == En || cur_key == Fn || cur_key == Gn){
			lcd_draw_image(KEYBOARD_WHITE_CENTER, KEYBOARD_WHITE_WIDTH, keyboardLocation[getkeyboardLocation(cur_key)], KEYBOARD_WHITE_HEIGHT, keyboardBitmapWhite, flight, LCD_COLOR_WHITE);
	}else{
	
	}
	
}

void displayKeytoPlay(key_t key, bool clear){
	// given key, highlight the board at the pressed spot
	uint16_t fdark = LCD_COLOR_RED;
	uint16_t flight = LCD_COLOR_RED;
	if(clear){
		fdark = LCD_COLOR_BLACK;
		flight = LCD_COLOR_WHITE;
	}
	
	if(key == As || key == Gs || key == Fs || key == Ds || key == Cs){
			lcd_draw_image(KEYBOARD_BLACK_CENTER, KEYBOARD_BLACK_WIDTH, keyboardLocation[getkeyboardLocation(key)], KEYBOARD_BLACK_HEIGHT, keyboardBitmapBlacktoPlay, fdark, LCD_COLOR_BLACK);
	}else if(key == An || key == Bn || key == Cn || key == Dn || key == En || key == Fn || key == Gn){
			lcd_draw_image(KEYBOARD_WHITE_CENTER, KEYBOARD_WHITE_WIDTH, keyboardLocation[getkeyboardLocation(key)], KEYBOARD_WHITE_HEIGHT, keyboardBitmapWhitetoPlay, flight, LCD_COLOR_WHITE);
	}else{
	
	}
	
}

void save_score(uint8_t score)
{
  eeprom_byte_write(I2C1_BASE, ADDR_START, score);
}

uint8_t get_high_score()
{
	uint8_t 	read_val;
  eeprom_byte_read(I2C1_BASE, ADDR_START, &read_val);
	return read_val;
}

// assign index-1 and index and index+1 to macro colomn locations MENU1 MENU2 MENU3
// assign macro to initial row location MENUROW
// index is at ascii-40
void displayMenu(void){
	char *string;
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

void displayScore(uint8_t score, uint8_t high_score) {
	uint8_t hundreds, tens, ones;
	char *string;
	uint16_t row0; 
	const uint8_t *bitmap;
	uint16_t topline = COLS/3;
	uint16_t bottomline = (COLS*2)/3;
	string = scoreList[0];
	
	
	hundreds = score/100;
	tens = (score - hundreds*100)/10;
	ones = score%10;
	
	lcd_draw_image(COLS/2, RESULT_WIDTH, ROWS/2, RESULT_HEIGHT, resultBitmap, LCD_COLOR_GREEN, LCD_COLOR_BLACK); 
	row0 = 120;
	if(hundreds != 0){
		bitmap = &numbersBitmaps[numbersDescriptors[hundreds]];
		lcd_draw_image(105, 16, row0, 13, bitmap, LCD_COLOR_ORANGE, LCD_COLOR_BLACK); 
	}
	row0 -= 16;
	bitmap = &numbersBitmaps[numbersDescriptors[tens]];
	lcd_draw_image(105, 16, row0, 13, bitmap, LCD_COLOR_ORANGE, LCD_COLOR_BLACK); 
	row0 -= 16;
	bitmap = &numbersBitmaps[numbersDescriptors[ones]];
	lcd_draw_image(105, 16, row0, 13, bitmap, LCD_COLOR_ORANGE, LCD_COLOR_BLACK); 
	
	row0 = 120;
	hundreds = high_score/100;
	tens = (high_score - hundreds*100)/10;
	ones = high_score%10;
	if(hundreds != 0){
		bitmap = &numbersBitmaps[numbersDescriptors[hundreds]];
		lcd_draw_image(130, 16, row0, 13, bitmap, LCD_COLOR_ORANGE, LCD_COLOR_BLACK); 
	}
	row0 -= 16;
	bitmap = &numbersBitmaps[numbersDescriptors[tens]];
	lcd_draw_image(130, 16, row0, 13, bitmap, LCD_COLOR_ORANGE, LCD_COLOR_BLACK); 
	row0 -= 16;
	bitmap = &numbersBitmaps[numbersDescriptors[ones]];
	lcd_draw_image(130, 16, row0, 13, bitmap, LCD_COLOR_ORANGE, LCD_COLOR_BLACK); 
}

int
main(void)
{
	// set up necessary variable
	bool songOver;
	uint16_t songIndex;
	const key_t* song;
	uint8_t necessary;
	key_t last_key = Sil;
	uint8_t delay, fin_score;
	uint16_t score, max_score;
	bool toggle_green_led = false;
	
	bool setup = false;
	uint8_t debounce_cnt = 0;
	uint8_t buttons = 0xFF;

	uint16_t adc_x_val = 0;
	uint16_t adc_y_val = 0;
	uint8_t volumePercent = 50;
	uint8_t pitchPercent = 50;
	
	uint16_t x_touch = 0;
	uint16_t y_touch = 0;
	uint8_t touch_event = false;
	
	mode = MENU;
	cur_key = Sil;
	// initialize the hardware
	initializeHardware();
	save_score(0);
	
	printf("**************************************\n\r");
	printf("PIANO - Project\n\r");
	printf("By - DAN and MATT\n\r");
	printf("**************************************\n\r");
	
	if(game_pause){
		lcd_draw_image(COLS/2, TITLE_WIDTH, ROWS/2, TITLE_HEIGHT, titleBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
		while(game_pause){
			if(switch_detect){
				switch_detect = false;
				if(!lp_io_read_pin(SW1_BIT)){
					debounce_cnt++;
					if(debounce_cnt == 5){
						game_pause = false;
					}
				} else {
					debounce_cnt = 0;
				}
			}
		}
		lcd_clear_screen(LCD_COLOR_BLACK);
	}
	
	// infinite loop for game logic
	while(1){
		// crude method of pausing a game
		while(game_pause){
			if(switch_detect){
				switch_detect = false;
				if(!lp_io_read_pin(SW1_BIT)){
					debounce_cnt++;
					if(debounce_cnt == 5){
						game_pause = false;
						if(mode == MENU){
							lcd_clear_screen(LCD_COLOR_BLACK);
							lcd_draw_image(COLS/2, 32, ROWS/2, 316, selectBoxHor, LCD_COLOR_ORANGE, LCD_COLOR_BLACK);
							displayMenu();
						}else{
							displayTouch(false);
							lcd_draw_image(COLS/2, KEYBOARD_WIDTH, ROWS/2, KEYBOARD_HEIGHT, keyboardBitmap, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
						}
					}
				} else {
					debounce_cnt = 0;
				}
			}
		}
		
		necessary = mcp23017_read_reg(MCP23017_INTCAPB_R);
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
						songOver = false;
						songIndex = 0;
						score = 0;
						max_score = 0;
						switch (menu_index) {
							case 1:
								song = maryHadALittleLamb;
								break;
							case 2:
								song = pokemonTheme;
								break;
							default:
								break;
						}
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
				
				if(checkKey(x_touch, y_touch) != cur_key){
					displayTouch(true);
				}
				cur_key = checkKey(x_touch, y_touch); // used to detect change in key for score
				displayTouch(false);
			}else{
				displayTouch(true);
				cur_key = Sil; // used to detect change in key for score
			}
				// check key type
				// display pressed key location
			read_touch = false;
		}
		
		if(buzzer_update){
			buzzer_play();
			
			printf("Score: %d\n\r", score);
			printf("Max Score: %d\n\r", max_score);
			if(music_beat && mode == FOLLOW){
				// if beat index is greater than 0 
					// check if previous played key state match previous key at beat index
						// increase match index for scoring
					// display led color accordingly
				// highlight key at current beat index
				if (songOver) {
					printf("Score: %d\n\r", score);
					printf("Max Score: %d\n\r", max_score);
					
					displayKeytoPlay(last_key, true);
					toggle_green_led = false;
					max_score = max_score - 16;
					score = score - 8;
					fin_score = 100*((float)score/max_score);
					if (fin_score > get_high_score()) {
						printf("Score: %d\n\r", fin_score);
						printf("High Score: %d\n\r", get_high_score());	
						save_score(fin_score);
					}	

					// Display score
					printf("Score: %d\n\r", fin_score);
					printf("Max Score: %d\n\r", max_score);
					printf("High Score: %d\n\r", get_high_score());	
					game_pause = true;
					displayScore(fin_score, get_high_score());
					while(game_pause){
						if(switch_detect){
							switch_detect = false;
							if(!lp_io_read_pin(SW1_BIT)){
								debounce_cnt++;
								if(debounce_cnt == 5){
									game_pause = false;
									mode = MENU;
									setup = false;
									lcd_clear_screen(LCD_COLOR_BLACK);
								}
							} else {
								debounce_cnt = 0;
							}
						}
					}
				}	
				
				if (song[songIndex] == End) {
					songOver = true;
				} else if (	song[songIndex] == Cont ) {						
					songIndex++;	
					if (cur_key != last_key) {
						displayKeytoPlay(last_key, false);
					}	
				}	else {
					if (last_key != song[songIndex]) {
						displayKeytoPlay(last_key, true);
					}	
					last_key = song[songIndex];
					delay = 0;
					while (delay < 7) {
						displayKeytoPlay(song[songIndex], true);
						delay++;
					}	
					displayKeytoPlay(song[songIndex], true);	
					displayKeytoPlay(song[songIndex], false);							
					songIndex++;
				}	
				if(!songOver){
					max_score++;
				}
				if (!songOver && cur_key == last_key) {
					score++;
					toggle_green_led = true;
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
			pitchPercent = (uint8_t) 100 * (((float)adc_x_val) / 4096);
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
			switch_detect = false;
			if(!lp_io_read_pin(SW1_BIT)){
				debounce_cnt++;
				if(debounce_cnt == 5){
					game_pause = true;
					lcd_draw_image(COLS/2, PAUSED_WIDTH, ROWS/2, PAUSED_HEIGHT, pausedBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
				}
			} else {
				debounce_cnt = 0;
			}
		}
	}
}
