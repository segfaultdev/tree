#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#include <raylib.h>

// nice values:
// - 160x100,  scale 9
// - 192x108,  scale 8
// - 240x150,  scale 6
// - 256x160,  scale 5 <- default
// - 320x200,  scale 4
// - 400x225,  scale 4
// - 480x300,  scale 3
// - 640x440,  scale 2
// - 800x450,  scale 2
// - 1280x880, scale 1 <- cayden pweease try this

// width and height in tiles(has to be editable in runtime by -w or --world flag, where you specify the screen size)
static int WIDTH = 3200;
static int HEIGHT = 240;

// pixels per tile(default zoom, making it smaller will make the window smaller, but won't change your min and max zoom)
#define SCALE 8

// if 1, will replace existing non-air blocks with the brush
#define REPLACE_BLOCKS 1

// if 1, will darken soils when wet, add foam to water, add dots to birch, etc...
#define TILE_EFFECTS 1

// if 1, deepness will darken blocks(affects performance hugely!)
#define DEEP_DARKEN 0

// if 1, will show a colorful bar representing how much time each part of the game takes(affects performance slightly)
#define DEBUG_MODE 0

// if 1, a world will be generated using perlin-like noise on startup(pass "-s NUMBER" to set the seed, will use current time by default)
#define WORLD_GEN 1

// if 1, tile coordinates will wrap around(affects performance a lot, not recommended)
#define WORLD_WRAP 0

// alpha, only applies to visible tiles, not to air
#define ALPHA 255

// background color alpha, set to 0 to see full image
#define BACKGROUND_ALPHA 255

// background image(or NULL if none)
// NOTE: change BACKGROUND_ALPHA to see image
// NOTE: does not work on some pcs, i think it's actually raylib's fault sooooooo
#define BACKGROUND_IMAGE NULL
// #define BACKGROUND_IMAGE "path/to/background.png"

/*
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
  "Pine Tree",
  "Beech Tree",
  "Willow Tree",
  "Lemon Tree",
  "Birch Tree",
  "Ebony Tree",
  "Apple Leaves",
  "Orange Leaves",
  "Palm Leaves",
  "Pine Leaves",
  "Beech Leaves",
  "Willow Leaves",
  "Lemon Leaves",
  "Birch Leaves",
  "Ebony Leaves",
  "Apple",
  "Orange",
  "Coconut",
  "Cone",
  "Lemon",
  "Persimmon",
  "Berry Bush",
  "Bush Leaves",
  "Red Berry",
  "Blue Berry",
  "Vines",
  "Mushroom",
  "Red Mushroom",
  "Brown Mushroom",
  "Fire",
  "Fertilizer",
  "Algae",
  "Algae Top",
  "Ash",
  "Insect",
  "Insect Carrying Apple",
  "Insect Carrying Orange",
  "Insect Carrying Coconut",
  "Insect Carrying Cone",
  "Fish",
  "Cacti",
  "Cacti Flower",
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
  (Color){103, 63, 0, ALPHA}, // Pine Tree
  (Color){95, 79, 7, ALPHA}, // Beech Tree
  (Color){127, 75, 47, ALPHA}, // Willow Tree
  (Color){95, 47, 0, ALPHA}, // Lemon Tree
  (Color){255, 223, 159, ALPHA}, // Birch Tree
  (Color){79, 31, 7, ALPHA}, // Ebony Tree
  (Color){31, 127, 31, ALPHA}, // Apple Leaves
  (Color){47, 191, 47, ALPHA}, // Orange Leaves
  (Color){127, 255, 63, ALPHA}, // Palm Leaves
  (Color){55, 143, 31, ALPHA}, // Pine Leaves
  (Color){49, 159, 59, ALPHA}, // Beech Leaves
  (Color){71, 127, 11, ALPHA}, // Willow Leaves
  (Color){47, 191, 47, ALPHA}, // Lemon Leaves
  (Color){131, 199, 31, ALPHA}, // Birch Leaves
  (Color){23, 95, 7, ALPHA}, // Ebony Leaves
  (Color){255, 31, 31, ALPHA}, // Apple
  (Color){255, 127, 31, ALPHA}, // Orange
  (Color){191, 95, 31, ALPHA}, // Coconut
  (Color){159, 91, 23, ALPHA}, // Cone
  (Color){255, 239, 47, ALPHA}, // Lemon
  (Color){240, 69, 13, ALPHA}, // Persimmon
  (Color){19, 79, 19, ALPHA}, // Berry Bush
  (Color){39, 159, 39, ALPHA}, // Bush Leaves
  (Color){127, 15, 15, ALPHA}, // Red Berry
  (Color){15, 15, 159, ALPHA}, // Blue Berry
  (Color){23, 95, 23, ALPHA}, // Vines
  (Color){255, 223, 223, ALPHA}, // Mushroom
  (Color){255, 95, 95, ALPHA}, // Red Mushroom
  (Color){223, 159, 127, ALPHA}, // Brown Mushroom
  (Color){255, 159, 31, ALPHA}, // Fire
  (Color){47, 23, 0, ALPHA}, // Fertilizer
  (Color){7, 95, 47, ALPHA}, // Algae
  (Color){23, 95, 63, ALPHA}, // Algae Top
  (Color){23, 23, 23, ALPHA}, // Ash
  (Color){7, 7, 7, ALPHA}, // Insect
  (Color){7, 7, 7, ALPHA}, // Insect Carrying Apple
  (Color){7, 7, 7, ALPHA}, // Insect Carrying Orange
  (Color){7, 7, 7, ALPHA}, // Insect Carrying Coconut
  (Color){7, 7, 7, ALPHA}, // Insect Carrying Cone
  (Color){143, 143, 191, ALPHA}, // Fish
  (Color){63, 127, 23, ALPHA}, // Cacti
  (Color){223, 15, 223, ALPHA}, // Cacti Flower
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
  1, // Pine Tree
  1, // Beech Tree
  1, // Willow Tree
  1, // Lemon Tree
  1, // Birch Tree
  1, // Ebony Tree
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Pine Leaves
  0, // Beech Leaves
  0, // Willow Leaves
  0, // Lemon Leaves
  0, // Birch Leaves
  0, // Ebony Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  0, // Cone
  0, // Lemon
  0, // Persimmon
  1, // Berry Bush
  0, // Bush Leaves
  0, // Red Berry
  0, // Blue Berry
  0, // Vines
  1, // Mushroom
  0, // Red Mushroom
  0, // Brown Mushroom
  1, // Fire
  1, // Fertilizer
  0, // Algae
  0, // Algae Top
  1, // Ash
  1, // Insect
  0, // Insect Carrying Apple
  0, // Insect Carrying Orange
  0, // Insect Carrying Coconut
  0, // Insect Carrying Cone
  1, // Fish
  1, // Cacti
  0, // Cacti Flower
};

// tiles with 1 will only be able to wet tiles of the same tile (SHOULD NOT BE CHANGED)
const int tile_limited[] = {
  0, // Air
  0, // Dirt
  0, // Water
  1, // Grass
  1, // Pink Flower
  1, // Blue Flower
  1, // Yellow Flower
  0, // Stone
  0, // Sand
  1, // Iron
  0, // Apple Tree
  0, // Orange Tree
  0, // Palm Tree
  0, // Pine Tree
  0, // Beech Tree
  0, // Willow Tree
  0, // Lemon Tree
  0, // Birch Tree
  0, // Ebony Tree
  1, // Apple Leaves
  1, // Orange Leaves
  1, // Palm Leaves
  1, // Pine Leaves
  1, // Beech Leaves
  1, // Willow Leaves
  1, // Lemon Leaves
  1, // Birch Leaves
  1, // Ebony Leaves
  1, // Apple
  1, // Orange
  1, // Coconut
  1, // Cone
  1, // Lemon
  1, // Persimmon
  0, // Berry Bush
  1, // Bush Leaves
  1, // Red Berry
  1, // Blue Berry
  1, // Vines
  0, // Mushroom
  1, // Red Mushroom
  1, // Brown Mushroom
  1, // Fire
  1, // Fertilizer
  1, // Algae
  1, // Algae Top
  1, // Ash
  1, // Insect
  1, // Insect Carrying Apple
  1, // Insect Carrying Orange
  1, // Insect Carrying Coconut
  1, // Insect Carrying Cone
  1, // Fish
  1, // Cacti
  1, // Cacti Flower
};

// tiles with 1 will float above powders, making those sink (SHOULD NOT BE CHANGED)
const int tile_light[] = {
  1, // Air
  0, // Dirt
  1, // Water
  0, // Grass
  0, // Pink Flower
  0, // Blue Flower
  0, // Yellow Flower
  0, // Stone
  0, // Sand
  0, // Iron
  0, // Apple Tree
  0, // Orange Tree
  0, // Palm Tree
  0, // Pine Tree
  0, // Beech Tree
  0, // Willow Tree
  0, // Lemon Tree
  0, // Birch Tree
  0, // Ebony Tree
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Pine Leaves
  0, // Beech Leaves
  0, // Willow Leaves
  0, // Lemon Leaves
  0, // Birch Leaves
  0, // Ebony Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  0, // Cone
  0, // Lemon
  0, // Persimmon
  0, // Berry Bush
  0, // Bush Leaves
  0, // Red Berry
  0, // Blue Berry
  0, // Vines
  0, // Mushroom
  0, // Red Mushroom
  0, // Brown Mushroom
  1, // Fire
  0, // Fertilizer
  0, // Algae
  0, // Algae Top
  0, // Ash
  1, // Insect
  1, // Insect Carrying Apple
  1, // Insect Carrying Orange
  1, // Insect Carrying Coconut
  1, // Insect Carrying Cone
  0, // Fish
  0, // Cacti
  0, // Cacti Flower
};

// tiles with 1 will burn (SHOULD NOT BE CHANGED)
const int tile_flamable[] = {
  0, // Air
  0, // Dirt
  0, // Water
  1, // Grass
  1, // Pink Flower
  1, // Blue Flower
  1, // Yellow Flower
  0, // Stone
  0, // Sand
  0, // Iron
  1, // Apple Tree
  1, // Orange Tree
  1, // Palm Tree
  1, // Pine Tree
  1, // Beech Tree
  1, // Willow Tree
  1, // Lemon Tree
  1, // Birch Tree
  1, // Ebony Tree
  1, // Apple Leaves
  1, // Orange Leaves
  1, // Palm Leaves
  1, // Pine Leaves
  1, // Beech Leaves
  1, // Willow Leaves
  1, // Lemon Leaves
  1, // Birch Leaves
  1, // Ebony Leaves
  1, // Apple
  1, // Orange
  1, // Coconut
  1, // Cone
  1, // Lemon
  1, // Persimmon
  1, // Berry Bush
  1, // Bush Leaves
  1, // Red Berry
  1, // Blue Berry
  1, // Vines
  1, // Mushroom
  1, // Red Mushroom
  1, // Brown Mushroom
  0, // Fire
  0, // Fertilizer
  1, // Algae
  1, // Algae Top
  0, // Ash
  1, // Insect
  1, // Insect Carrying Apple
  1, // Insect Carrying Orange
  1, // Insect Carrying Coconut
  1, // Insect Carrying Cone
  1, // Fish
  1, // Cacti
  1, // Cacti Flower
};

// tiles with 1 will generate ash instead of fire (SHOULD NOT BE CHANGED)
const int tile_wooden[] = {
  0, // Air
  0, // Dirt
  0, // Water
  0, // Grass
  0, // Pink Flower
  0, // Blue Flower
  0, // Yellow Flower
  0, // Stone
  0, // Sand
  0, // Iron
  1, // Apple Tree
  1, // Orange Tree
  1, // Palm Tree
  1, // Pine Tree
  1, // Beech Tree
  1, // Willow Tree
  1, // Lemon Tree
  1, // Birch Tree
  1, // Ebony Tree
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Pine Leaves
  0, // Beech Leaves
  0, // Willow Leaves
  0, // Lemon Leaves
  0, // Birch Leaves
  0, // Ebony Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  0, // Cone
  0, // Lemon
  0, // Persimmon
  1, // Berry Bush
  0, // Bush Leaves
  0, // Red Berry
  0, // Blue Berry
  0, // Vines
  0, // Mushroom
  0, // Red Mushroom
  0, // Brown Mushroom
  0, // Fire
  0, // Fertilizer
  0, // Algae
  0, // Algae Top
  0, // Ash
  0, // Insect
  0, // Insect Carrying Apple
  0, // Insect Carrying Orange
  0, // Insect Carrying Coconut
  0, // Insect Carrying Cone
  0, // Fish
  1, // Cacti
  0, // Cacti Flower
};
*/

#endif
