#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#include <raylib.h>

// nice values:
// - 160x100,  scale 9
// - 192x108,  scale 8
// - 240x150,  scale 6 <- default
// - 256x160,  scale 5
// - 320x200,  scale 4
// - 400x225,  scale 4
// - 480x300,  scale 3 <- twice as big as default, still fast enough for most pcs
// - 640x400,  scale 2
// - 800x450,  scale 2
// - 1280x800, scale 1 <- cayden don't use this

// width and height in tiles
#define WIDTH 240
#define HEIGHT 150

// pixels per tile(default zoom, making it smaller will make the window smaller, but won't change your min and max zoom)
#define SCALE 6

// if 1, will replace existing non-air blocks with the brush
#define REPLACE_BLOCKS 1

// if 1, will darken dirt, stone and sand when wet
#define WET_BLOCKS 1

// alpha, only applies to visible tiles, not to air
#define ALPHA 255

// background color alpha, set to 0 to see full image
#define BACKGROUND_ALPHA 255

// background image(or NULL if none)
// NOTE: change BACKGROUND_ALPHA to see image
// NOTE: does not work on some pcs, i think it's actually raylib's fault sooooooo
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
  tile_pine_tree,
  tile_apple_leaves,
  tile_orange_leaves,
  tile_palm_leaves,
  tile_pine_leaves,
  tile_apple,
  tile_orange,
  tile_coconut,
  tile_cone,
  tile_berry_bush,
  tile_bush_leaves,
  tile_red_berry,
  tile_blue_berry,
  tile_vines,
  tile_mushroom,
  tile_red_mushroom,
  tile_brown_mushroom,
  tile_fire,
  tile_fertilizer,
  tile_algae,
  tile_algae_top,
  tile_ash,
  tile_insect,
  tile_insect_apple,
  tile_insect_orange,
  tile_insect_coconut,
  tile_insect_cone,
  tile_fish,
  
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
  "Pine Tree",
  "Apple Leaves",
  "Orange Leaves",
  "Palm Leaves",
  "Pine Leaves",
  "Apple",
  "Orange",
  "Coconut",
  "Cone",
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
  "Fish"
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
  (Color){31, 127, 31, ALPHA}, // Apple Leaves
  (Color){47, 191, 47, ALPHA}, // Orange Leaves
  (Color){127, 255, 63, ALPHA}, // Palm Leaves
  (Color){55, 143, 31, ALPHA}, // Pine Leaves
  (Color){255, 31, 31, ALPHA}, // Apple
  (Color){255, 127, 31, ALPHA}, // Orange
  (Color){191, 95, 31, ALPHA}, // Coconut
  (Color){159, 91, 23, ALPHA}, // Cone
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
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Pine Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  0, // Cone
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
  1, // Apple Leaves
  1, // Orange Leaves
  1, // Palm Leaves
  1, // Pine Leaves
  1, // Apple
  1, // Orange
  1, // Coconut
  1, // Cone
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
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Pine Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  0, // Cone
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
  0, // Apple Leaves
  0, // Orange Leaves
  0, // Palm Leaves
  0, // Pine Leaves
  0, // Apple
  0, // Orange
  0, // Coconut
  0, // Cone
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
};

#endif
