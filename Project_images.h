#ifndef __PROJECT_IMAGES_H__
#define __PROJECT_IMAGES_H__

#include <stdint.h>
#include <stdio.h>

#define       			KEYBOARD_HEIGHT 	320		// height of keyboard image in pixels
#define       			KEYBOARD_WIDTH  	239		// width of keyboard image in pixels
#define					KEYBOARD_WHITE_HEIGHT		44		// Height of white keys
#define					KEYBOARD_WHITE_WIDTH		120		// Width of white key
#define					KEYBOARD_BLACK_HEIGHT		32		// Height of black keys
#define					KEYBOARD_BLACK_WIDTH		120		// Width of black keys
#define					KEYBOARD_BLACK_CENTER		61		// Col location of black keys
#define					KEYBOARD_WHITE_CENTER		180		// Col location of white keys

// Correlating key notes with keyboardLocation index
#define					A_Loc		3
#define					B_Loc		1
#define					C_Loc		13
#define					D_Loc		11
#define					E_Loc		9
#define					F_Loc		7
#define					G_Loc		5
#define					As_Loc		0
#define					Gs_Loc		4
#define					Fs_Loc		6
#define					Ds_Loc		8
#define					Cs_Loc		12

extern const uint8_t keyboardBitmap[]; // Bitmap of the whole keyboard
extern const uint8_t keyboardBitmapWhite[]; // White key size
extern const uint8_t keyboardBitmapBlack[]; // Black key size
extern const uint8_t keyboardBitmapWhitetoPlay[]; // White key with red circle in the middle indicating to play it
extern const uint8_t keyboardBitmapBlacktoPlay[]; // Black key with red circle in the middle indicating to play it
extern const uint16_t keyboardLocation[]; // Key location

#endif
