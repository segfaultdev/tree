#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#include <raylib.h>

// width and height in tiles
#define WIDTH 192
#define HEIGHT 108

// pixels per tile
#define SCALE 8

// alpha, only applies to visible tiles, not to air
#define ALPHA 255

// background color alpha, set to 0 to see full image
#define BACKGROUND_ALPHA 255

// background image(or NULL if none)
// NOTE: change BACKGROUND_ALPHA to see image
#define BACKGROUND_IMAGE NULL
// #define BACKGROUND_IMAGE "path/to/background.png"

enum {
  tile_air,
  tile_dirt,
  tile_water,
  tile_grass,
  tile_flower_pink,
  tile_flower_blue,
  tile_flower_yellow,
  tile_stone,
  tile_sand,
  tile_iron,
  tile_apple_tree,
  tile_orange_tree,
  tile_palm_tree,
  tile_apple_leaves,
  tile_orange_leaves,
  tile_palm_leaves,
  tile_apple,
  tile_orange,
  tile_coconut,
  tile_berry_bush,
  tile_red_berry,
  tile_blue_berry,
  tile_vines,
  tile_mushroom,
  tile_red_mushroom,
  tile_brown_mushroom,
  
  tile_count
};

// names showed in the selection menu
const char *tile_names[] = {
  "Air",
  "Dirt",
  "Water",
  "Grass",
  "Pink Flower",
  "Blue Flower",
  "Yellow Flower",
  "Stone",
  "Sand",
  "Iron",
  "Apple Tree",
  "Orange Tree",
  "Palm Tree",
  "Apple Leaves",
  "Orange Leaves",
  "Palm Leaves",
  "Apple",
  "Orange",
  "Coconut",
  "Berry Bush",
  "Red Berry",
  "Blue Berry",
  "Vines",
  "Mushroom",
  "Red Mushroom",
  "Brown Mushroom",
};

const Color tile_colors[] = {
  (Color){51, 51, 51, BACKGROUND_ALPHA}, // Air <- Background Color
  (Color){127, 63, 0, ALPHA}, // Dirt
  (Color){63, 63, 255, ALPHA}, // Water
  (Color){63, 255, 63, ALPHA}, // Grass
  (Color){255, 127, 127, ALPHA}, // Pink Flower
  (Color){127, 127, 255, ALPHA}, // Blue Flower
  (Color){255, 255, 63, ALPHA}, // Yellow Flower
  (Color){127, 127, 127, ALPHA}, // Stone
  (Color){255, 255, 127, ALPHA}, // Sand
  (Color){191, 191, 191, ALPHA}, // Iron
  (Color){63, 31, 0, ALPHA}, // Apple Tree
  (Color){95, 47, 0, ALPHA}, // Orange Tree
  (Color){127, 95, 31, ALPHA}, // Palm Tree
  (Color){31, 127, 31, ALPHA}, // Apple Leaves
  (Color){47, 191, 47, ALPHA}, // Orange Leaves
  (Color){127, 255, 63, ALPHA}, // Palm Leaves
  (Color){255, 31, 31, ALPHA}, // Apple
  (Color){255, 127, 31, ALPHA}, // Orange
  (Color){191, 95, 31, ALPHA}, // Coconut
  (Color){39, 159, 39, ALPHA}, // Berry Bush
  (Color){127, 15, 15, ALPHA}, // Red Berry
  (Color){15, 15, 159, ALPHA}, // Blue Berry
  (Color){23, 95, 23, ALPHA}, // Vines
  (Color){255, 223, 223, ALPHA}, // Mushroom
  (Color){255, 95, 95, ALPHA}, // Red Mushroom
  (Color){223, 159, 127, ALPHA}, // Brown Mushroom
};

// set to 1 to be visible in the selection menu
const int tile_visible[] = {
  0, // Air
  1, // Dirt
  1, // Water
  0, // Grass
  0, // Pink Flower
  0, // Blue Flower
  0, // Yellow Flower
  1, // Stone
  1, // Sand
  1, // Iron
  1, // Apple Tree
  1, // Orange Tree
  1, // Palm Tree
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  1, // Berry Bush
  0, // Red Berry
  0, // Blue Berry
  0, // Vines
  1, // Mushroom
  0, // Red Mushroom
  0, // Brown Mushroom
};

#endif
