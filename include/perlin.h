#ifndef __PERLIN_H__
#define __PERLIN_H__

// seg's own perlin like thing
// note: IT'S NOT ACTUAL PERLIN PLEASE DON'T KILL ME ):

#include <stdint.h>

extern uint32_t noise_seed;

uint32_t fast_rand(void);
void fast_srand(uint32_t seed);

float lerp(float x, float y, float w);

float noise_1(float x);
float noise_2(float x, float y);

#endif
