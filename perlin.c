#include <perlin.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t noise_seed = 0;
uint32_t rand_seed = 1;

uint32_t fast_rand(void) {
  rand_seed ^= rand_seed >> 17;
  rand_seed *= 0xED5AD4BB;
  rand_seed ^= rand_seed >> 11;
  rand_seed *= 0xAC4C1B51;
  rand_seed ^= rand_seed >> 15;
  rand_seed *= 0x31848BAB;
  rand_seed ^= rand_seed >> 14;
  
  return rand_seed;
}

void fast_srand(uint32_t seed) {
  srand(seed);
  rand_seed = (seed + noise_seed + 1);
}

float lerp(float x, float y, float w) {
  if (w < 0.0f) return x;
  if (w > 1.0f) return y;
  
  return (y - x) * w + x;
}

float lerp_smooth(float x, float y, float w) {
  if (w < 0.0f) return x;
  if (w > 1.0f) return y;
  
  return (y - x) * ((w * (w * 6.0f - 15.0f) + 10.0f) * w * w * w) + x;
}

float grad(int x, int y) {
  fast_srand(x + 1048577 * y);
  return (fast_rand() % 65537) / 65537.0f;
}

float noise_1(float x) {
  return lerp(grad((int)(x + 0), 1024), grad((int)(x + 1), 1024), x - (int)(x));
}

float noise_2(float x, float y) {
  x -= (y / 1.6f);
  x += 1024;
  
  float dx = x - (int)(x);
  float dy = y - (int)(y);
  
  if (1 - dx < dy) {
    float s = ((dx - dy) + 1) / 2;
    float h = 2 - dx - dy;
    
    float t = lerp_smooth(grad((int)(x + 0), (int)(y + 1)), grad((int)(x + 1), (int)(y + 0)), s);
    return lerp_smooth(grad((int)(x + 1), (int)(y + 1)), t, h);
  } else {
    float s = ((dx - dy) + 1) / 2;
    float h = dx + dy;
    
    float t = lerp_smooth(grad((int)(x + 0), (int)(y + 1)), grad((int)(x + 1), (int)(y + 0)), s);
    return lerp_smooth(grad((int)(x + 0), (int)(y + 0)), t, h);
  }
}
