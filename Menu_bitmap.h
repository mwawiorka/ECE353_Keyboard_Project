#ifndef __MENU_BITMAP_H__
#define __MENU_BITMAP_H__

#include <stdint.h>
#include <stdio.h>

#define       	MENU1 	60		// Line 1 of display menu selection
#define       	MENU2  	120		// Line 2 of display menu selection
#define       	MENU3  	180		// Line 3 of display menu selection
#define			MENUROW	280		// The way left location of names

extern const uint16_t letterRedirect[];
extern const uint8_t letterBitmaps[];
extern const uint8_t selectBoxHor[];

#endif
