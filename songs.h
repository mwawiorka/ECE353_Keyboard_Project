#ifndef __SONGS_H__
#define __SONGS_H__

#include <stdint.h>
#include <stdio.h>

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

extern const key_t maryHadALittleLamb[];

extern const key_t freedomSong[];

#endif