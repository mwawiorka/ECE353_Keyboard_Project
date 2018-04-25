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

extern void hw1(void);

typedef enum 
{
  DEBOUNCE_ONE,
  DEBOUNCE_1ST_ZERO,
  DEBOUNCE_2ND_ZERO,
  DEBOUNCE_PRESSED
} DEBOUNCE_STATES;

typedef enum
{
	SOLID,
	STROBE,
	WAVE,
	OFF
} LED_MODE;
//*****************************************************************************
// Global Variables
//*****************************************************************************


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


//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
  DisableInterrupts();
  gp_timer_config_32(TIMER0_BASE, TIMER_TAMR_TAMR_1_SHOT, false, false);
  init_serial_debug(true, true);
  ft6x06_init();
	init_pwm();
  EnableInterrupts();
}
void initializeHardware(void)
{
	
	initialize_serial_debug();
	
	//// setup lcd GPIO, config the screen, and clear it ////
	lcd_config_gpio();
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
}

//*****************************************************************************
//*****************************************************************************
bool left_sw_debounce()
{
  static DEBOUNCE_STATES state_left = DEBOUNCE_ONE;
  bool pin_logic_level = left_sw_pressed();
  
  switch (state_left)
  {
    case DEBOUNCE_ONE:
    {
      if(pin_logic_level){
        state_left = DEBOUNCE_ONE;
      } else {
        state_left = DEBOUNCE_1ST_ZERO;
      }
      break;
    }
    case DEBOUNCE_1ST_ZERO:
    {
      if(pin_logic_level){
        state_left = DEBOUNCE_ONE;
      } else {
        state_left = DEBOUNCE_2ND_ZERO;
      }
      break;
    }
    case DEBOUNCE_2ND_ZERO:
    {
      if(pin_logic_level){
        state_left = DEBOUNCE_ONE;
      } else {
        state_left = DEBOUNCE_PRESSED;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(pin_logic_level){
        state_left = DEBOUNCE_ONE;
      } else {
        state_left = DEBOUNCE_PRESSED;
      }
      break;
    }
    default:
			for(;;);
	}
	
	if(state_left == DEBOUNCE_2ND_ZERO ){
    return true;
  } else {
    return false;
  }
}

//*****************************************************************************
//*****************************************************************************
bool right_sw_debounce()
{
  static DEBOUNCE_STATES state_right = DEBOUNCE_ONE;
  bool pin_logic_level = right_sw_pressed();
  
  switch (state_right)
  {
    case DEBOUNCE_ONE:
    {
      if(pin_logic_level){
        state_right = DEBOUNCE_ONE;
      } else {
        state_right = DEBOUNCE_1ST_ZERO;
      }
      break;
    }
    case DEBOUNCE_1ST_ZERO:
    {
      if(pin_logic_level){
        state_right = DEBOUNCE_ONE;
      } else {
        state_right = DEBOUNCE_2ND_ZERO;
      }
      break;
    }
    case DEBOUNCE_2ND_ZERO:
    {
      if(pin_logic_level){
        state_right = DEBOUNCE_ONE;
      } else {
        state_right = DEBOUNCE_PRESSED;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(pin_logic_level){
        state_right = DEBOUNCE_ONE;
      } else {
        state_right = DEBOUNCE_PRESSED;
      }
      break;
    }
    default:
			for(;;);
	}
	
	if(state_right == DEBOUNCE_2ND_ZERO ){
    return true;
  } else {
    return false;
  }
}

//*****************************************************************************
//*****************************************************************************
bool up_sw_debounce()
{
  static DEBOUNCE_STATES state_up = DEBOUNCE_ONE;
  bool pin_logic_level = up_sw_pressed();
  
  switch (state_up)
  {
    case DEBOUNCE_ONE:
    {
      if(pin_logic_level){
        state_up = DEBOUNCE_ONE;
      } else {
        state_up = DEBOUNCE_1ST_ZERO;
      }
      break;
    }
    case DEBOUNCE_1ST_ZERO:
    {
      if(pin_logic_level){
        state_up = DEBOUNCE_ONE;
      } else {
        state_up = DEBOUNCE_2ND_ZERO;
      }
      break;
    }
    case DEBOUNCE_2ND_ZERO:
    {
      if(pin_logic_level){
        state_up = DEBOUNCE_ONE;
      } else {
        state_up = DEBOUNCE_PRESSED;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(pin_logic_level){
        state_up = DEBOUNCE_ONE;
      } else {
        state_up = DEBOUNCE_PRESSED;
      }
      break;
    }
    default:
			for(;;);
	}
	
	if(state_up == DEBOUNCE_2ND_ZERO ){
    return true;
  } else {
    return false;
  }
}

//*****************************************************************************
//*****************************************************************************
bool down_sw_debounce()
{
  static DEBOUNCE_STATES state_down = DEBOUNCE_ONE;
  bool pin_logic_level = down_sw_pressed();
  
  switch (state_down)
  {
    case DEBOUNCE_ONE:
    {
      if(pin_logic_level){
        state_down = DEBOUNCE_ONE;
      } else {
        state_down = DEBOUNCE_1ST_ZERO;
      }
      break;
    }
    case DEBOUNCE_1ST_ZERO:
    {
      if(pin_logic_level){
        state_down = DEBOUNCE_ONE;
      } else {
        state_down = DEBOUNCE_2ND_ZERO;
      }
      break;
    }
    case DEBOUNCE_2ND_ZERO:
    {
      if(pin_logic_level){
        state_down = DEBOUNCE_ONE;
      } else {
        state_down = DEBOUNCE_PRESSED;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(pin_logic_level){
        state_down = DEBOUNCE_ONE;
      } else {
        state_down = DEBOUNCE_PRESSED;
      }
      break;
    }
    default:
			for(;;);
	}
	
	if(state_down == DEBOUNCE_2ND_ZERO ){
    return true;
  } else {
    return false;
  }
}

//*****************************************************************************
//*****************************************************************************
uint8_t check_buttons(){
	static LED_MODE mode = OFF;
	
	if(left_sw_debounce()){ /*GREEN LED*/ }
	if(down_sw_debounce()){ /*RED LED*/ }
	if(up_sw_debounce()){ /*BLUE LED*/ }
	if(right_sw_debounce()){ 
		switch(mode){
			case(OFF):
				mode = SOLID;
				break;
			case(SOLID):
				mode = STROBE;
				break;
			case(STROBE):
				mode = WAVE;
				break;
			case(WAVE):
				mode = OFF;
				break;
			default:
				for(;;);
		}
	}
}

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
  uint16_t x,y;
  uint8_t touch_event;
  i2c_status_t td_status;
  i2c_status_t x_status;
  i2c_status_t y_status;
	
	int colorIndex = 0;
	int noteIndex = 0;
	int nxtColorIndex = -1;
	int nxtNoteIndex = -1;
	
	//hw1_init();
	//hw1();	
	initializeHardware();
  lcd_draw_image(COLS/2, KEYBOARD_WIDTH, ROWS/2, KEYBOARD_HEIGHT, keyboardBitmap, LCD_COLOR_BLACK, LCD_COLOR_WHITE);

  initializeBoard();
  
  printf("\n\r");
  printf("**************************************\n\r");
  printf("* In Class - I2C FT6206\n\r");
  printf("**************************************\n\r");
 
  while(1){
    
    // ADD CODE
    // Determine how many active touch events there are.  If there are more than
    // 0, then read the x and y coordinates and print them out to the serial debug
    // teriminal using printf.
		touch_event = ft6x06_read_td_status();
		
		if(touch_event > 0 && touch_event < 3){
			x = ft6x06_read_x();
			y = ft6x06_read_y();
			
			printf("X: %d Y: %d\n\r",x,y);
		}
		
		if(x>120) {
			nxtColorIndex = 1;
		} else {
			nxtColorIndex = 0;
		}
		
		if(!nxtColorIndex) {
			if(y<29){
				//Nothing
				nxtNoteIndex = -1;
			}else if(y<59){
				nxtNoteIndex = 0;
			}else if(y<95){
				nxtNoteIndex = 4;
			}else if(y<151){
				nxtNoteIndex = 6;
			}else if(y<212){
				//Nothing
				nxtNoteIndex = -1;
			}else if(y<243){
				nxtNoteIndex = 8;
			}else if(y<289){
				nxtNoteIndex = 12;
			}else{
				//Nothing
				 nxtNoteIndex = -1;
			}
		} else {
			if(y<44){
				nxtNoteIndex = 1;
			}else if(y<90){
				nxtNoteIndex = 3;
			}else if(y<136){
				nxtNoteIndex = 5;
			}else if(y<182){
				nxtNoteIndex = 7;
			}else if(y<228){
				nxtNoteIndex = 9;
			}else if(y<274){
				nxtNoteIndex = 11;
			}else{
				nxtNoteIndex = 13;
			}
		}
		
		if(!(touch_event > 0 && touch_event < 3) || nxtColorIndex != colorIndex || nxtNoteIndex != noteIndex){
			if(colorIndex){
				lcd_draw_image(KEYBOARD_WHITE_CENTER, KEYBOARD_WHITE_WIDTH, keyboardLocation[noteIndex], KEYBOARD_WHITE_HEIGHT, keyboardBitmapWhite, LCD_COLOR_WHITE, LCD_COLOR_WHITE);
			} else {
				lcd_draw_image(KEYBOARD_BLACK_CENTER, KEYBOARD_BLACK_WIDTH, keyboardLocation[noteIndex], KEYBOARD_BLACK_HEIGHT, keyboardBitmapBlack, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
			}
			colorIndex = nxtColorIndex;
			noteIndex = nxtNoteIndex;
			stop_buzz();
		}
		
		if(touch_event > 0 && touch_event < 3 && noteIndex != -1){
			if(colorIndex){
				lcd_draw_image(KEYBOARD_WHITE_CENTER, KEYBOARD_WHITE_WIDTH, keyboardLocation[noteIndex], KEYBOARD_WHITE_HEIGHT, keyboardBitmapWhite, LCD_COLOR_RED, LCD_COLOR_WHITE);
			} else {
				lcd_draw_image(KEYBOARD_BLACK_CENTER, KEYBOARD_BLACK_WIDTH, keyboardLocation[noteIndex], KEYBOARD_BLACK_HEIGHT, keyboardBitmapBlack, LCD_COLOR_RED, LCD_COLOR_BLACK);
			}
			// Play Sound Accordingly
			if (nxtNoteIndex == 13) 
			{
				buzz(C);
			}	
			else if (nxtNoteIndex == 12) 
			{
				buzz(C_s);
			}
			else if (nxtNoteIndex == 11) 
			{
				buzz(D);
			}
			else if (nxtNoteIndex == 8) 
			{
				buzz(D_s);
			}
			else if (nxtNoteIndex == 9) 
			{
				buzz(E);
			}
			else if (nxtNoteIndex == 7) 
			{
				buzz(F);
			}
			else if (nxtNoteIndex == 6) 
			{
				buzz(F_s);
			}
			else if (nxtNoteIndex == 5) 
			{
				buzz(G);
			}
			else if (nxtNoteIndex == 4) 
			{
				buzz(G_s);
			}
			else if (nxtNoteIndex == 3) 
			{
				buzz(A);
			}
			else if (nxtNoteIndex == 0) 
			{
				buzz(A_s);
			}
			else if (nxtNoteIndex == 1) 
			{
				buzz(B);
			}
		}
    
    //gp_timer_wait(TIMER0_BASE, 5000000);
		gp_timer_wait(TIMER0_BASE, 500000);
  
  };

}
