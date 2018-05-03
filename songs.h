#ifndef __SONGS_H__
#define __SONGS_H__

#include <stdint.h>
#include <stdio.h>

// Define key type
typedef enum {
	Sil,
	Cont,
	End,
	An,
	As,
	Bn,
	Cn,
	Cs,
	Dn,
	Ds,
	En,
	Fn,
	Fs,
	Gn,
	Gs,
} key_t;

// Keys array for Mary had a Little Lamb
extern const key_t maryHadALittleLamb[];

// Keys array for Pokemon Theme
extern const key_t pokemonTheme[];

#endif
