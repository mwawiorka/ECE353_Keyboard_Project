#ifndef __PROJECT_IMAGES_H__
#define __PROJECT_IMAGES_H__

#include <stdint.h>
#include <stdio.h>

#define       	KEYBOARD_HEIGHT 	320		// height of keyboard image in pixels
#define       	KEYBOARD_WIDTH  	239		// width of keyboard image in pixels
#define					KEYBOARD_WHITE_HEIGHT		44
#define					KEYBOARD_WHITE_WIDTH		120
#define					KEYBOARD_BLACK_HEIGHT		32
#define					KEYBOARD_BLACK_WIDTH		120
#define					KEYBOARD_BLACK_CENTER		61
#define					KEYBOARD_WHITE_CENTER		180

extern const uint8_t keyboardBitmap[];
extern const uint8_t keyboardBitmapWhite[];
extern const uint8_t keyboardBitmapBlack[];
extern const uint16_t keyboardLocation[];

#endif
