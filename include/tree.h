#ifndef __TREE_H__
#define __TREE_H__

// seg's wonderful todo list:

// tree 0.09: done!

// tree 0.10: done!

// tree 0.11 - the ALIVE update: done!

// tree 0.12:
// - add a USTAR file parser, and allow for dynamic mod loading(cos changing this file to add new blocks is boring)
// - write some documentation for making mods

typedef struct tile_t tile_t;
typedef struct tree_t tree_t;

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
  tile_beech_tree,
  tile_willow_tree,
  tile_lemon_tree,
  tile_birch_tree,
  tile_ebony_tree,
  tile_cherry_tree,
  tile_apple_seed,
  tile_orange_seed,
  tile_palm_seed,
  tile_pine_seed,
  tile_beech_seed,
  tile_willow_seed,
  tile_lemon_seed,
  tile_birch_seed,
  tile_ebony_seed,
  tile_cherry_seed,
  tile_apple_leaves,
  tile_orange_leaves,
  tile_palm_leaves,
  tile_pine_leaves,
  tile_beech_leaves,
  tile_willow_leaves,
  tile_lemon_leaves,
  tile_birch_leaves,
  tile_ebony_leaves,
  tile_cherry_leaves,
  tile_apple,
  tile_orange,
  tile_coconut,
  tile_cone,
  tile_lemon,
  tile_persimmon,
  tile_cherry,
  tile_berry_bush,
  tile_bush_leaves,
  tile_red_berry,
  tile_blue_berry,
  tile_cacti,
  tile_cacti_flower,
  tile_algae,
  tile_algae_top,
  tile_vines,
  tile_mushroom,
  tile_red_mushroom,
  tile_brown_mushroom,
  tile_fire,
  tile_ash,
  tile_steam,
  tile_soil,
  tile_wheat,
  tile_wheat_top,
  tile_carrot,
  tile_carrot_leaves,
  tile_fish,
  tile_honey,
  tile_snow,
  tile_ice,
  tile_burn,
  tile_hive,
  tile_empty_bee,
  tile_polen_bee,
  tile_ant,
  tile_clay,
  tile_orange_fish,
  tile_white_fish,
  tile_pink_fish,
  tile_moss,
  tile_caveroom,
  tile_red_caveroom,
  tile_brown_caveroom,
  
  tile_count
};

enum {
  tile_color_none, // only use color_1
  tile_color_ceil, // use color_2 on those tiles which have air on top(for example, water)
  tile_color_dots, // use mostly color_1, but adding some dots of color_2(for example, birch trees)
  tile_color_wet   // use color_2 when the water level is over 20
};

enum {
  tile_type_solid,  // not only solids, but also air, cos we just don't apply any logic to these blocks
  tile_type_powder, // sand, dirt
  tile_type_liquid, // water
  tile_type_gas,    // fire, steam, will destroy itself after some random time
  
  tile_type_ai_water, // entity with water ai(fish)
  tile_type_ai_fly, // bees(does not apply the ai)
  tile_type_ai_land, // ants
};

struct tile_t {
  char name_en[28]; // tile name in english
  
  Color color_1, color_2;
  int color_mode; // check tile_color_* in enum above
  
  int type; // tile type, check tile_type_* in enum above
  
  int show; // show in the selection menu(if 2 will only show in god mode)
  int flow; // only wet tiles of the same type
  int sink; // float above powders, making those sink
  int soil; // spread fertile levels
  
  int fire_tile; // tile it is replaced with when burnt(set to same tile to not burn)
  int fire_amnt; // chance it will be replaced with that tile and not fire(0 being all fire, 15 being all that tile)
  
  int spread; // tile_type_powder: if water level GREATER(not greater or equal) than this, will not only fall(like dry dirt) but also spread(like sand or wet dirt), set to -1 to act like sand
              // tile_type_liquid: viscosity, from 0 to 15, being 15 the most liquid-ey
              // tile_type_solid:  minimum water level needed to not disappear
  int remove; // tile it will be replaced with when destroying it(should be air for most blocks, water for algae and steam, etc.)
  
  int need_tile_1; // tile it needs to have close to not disappear <- store dirt type here for trees
  int need_tile_2; // tile it needs to have close to not disappear(either this one or need_tile_1)
  int need_dist; // distance that tile has to be within to survive
  int need_amnt; // amount of tiles needed to survive(or 0 if it doesn't need any tiles near)
  int need_root; // only check tiles below(includes diagonals), but not the ones on the sides or above(only checks tiles above if 2)
  
  int tree_type; // tree type index, -1 if not part of a tree
  int grow_type; // grow type index, -1 if not soil, affects only tiles that can spread fertile levels
  
  int weak; // can be broken by other growing tiles
  int very_weak; // can be broken by anything
};

struct tree_t {
  int tree_tile; // tree/trunk tile
  int tree_dist; // minimum distance between trees
  
  int leaf_tile; // leaf tile
  int leaf_grow; // maximum water level for leaves to grow(minimum is always 1)
  
  // the most common fruit tile in a radius of 7 tiles will be chosen, or randomly if there is the same amount of each type
  
  int fruit_tile_1; // first fruit tile, or -1 if fruitless
  int fruit_tile_2; // second fruit tile, or -1 if fruitless
  int fruit_dist; // minimum distance between fruits
  int fruit_grow; // maximum water level for fruits to grow(minimum is always 1)
  
  int upward_min; // minimum water for the tree to grow upwards
  int branch_min; // minimum water for branches to grow
  int branch_max; // maximum water for branches to grow
};

static const tile_t tile_types[] = {
  {"Air"           , (Color){51 , 51 , 51 }, (Color){51 , 51 , 51 }, tile_color_none, tile_type_solid   , 1, 1, 1, 0, tile_air  , 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 1, 1},
  {"Dirt"          , (Color){127, 63 , 0  }, (Color){119, 59 , 0  }, tile_color_wet , tile_type_powder  , 1, 0, 0, 0, tile_clay , 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, 0 , 0, 0},
  {"Water"         , (Color){63 , 63 , 255}, (Color){191, 191, 255}, tile_color_ceil, tile_type_liquid  , 1, 0, 1, 0, tile_steam, 15, 15, tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 1, 0},
  {"Grass"         , (Color){63 , 255, 63 }, (Color){63 , 255, 63 }, tile_color_none, tile_type_solid   , 0, 1, 1, 0, tile_air  , 3 , 1 , tile_air  , tile_grass         , tile_dirt       , 1, 1, 1, -1, -1, 1, 1},
  {"Pink Flower"   , (Color){255, 127, 127}, (Color){255, 127, 127}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 3 , 0 , tile_soil , tile_grass         , tile_grass      , 1, 1, 0, -1, -1, 1, 0},
  {"Blue Flower"   , (Color){127, 127, 255}, (Color){127, 127, 255}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 3 , 0 , tile_soil , tile_grass         , tile_grass      , 1, 1, 0, -1, -1, 1, 0},
  {"Yellow Flower" , (Color){255, 255, 63 }, (Color){255, 255, 63 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 3 , 0 , tile_soil , tile_grass         , tile_grass      , 1, 1, 0, -1, -1, 1, 0},
  {"Stone"         , (Color){127, 127, 127}, (Color){119, 119, 119}, tile_color_wet , tile_type_solid   , 1, 0, 0, 0, tile_stone, 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Sand"          , (Color){255, 255, 127}, (Color){239, 239, 119}, tile_color_wet , tile_type_powder  , 1, 0, 0, 0, tile_sand , 15, -1, tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, 1 , 0, 0},
  {"Iron"          , (Color){191, 191, 191}, (Color){191, 191, 191}, tile_color_none, tile_type_solid   , 1, 1, 0, 0, tile_iron , 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Apple Tree"    , (Color){63 , 31 , 0  }, (Color){63 , 31 , 0  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_apple_tree , 6, 1, 1, 0 , -1, 0, 0},
  {"Orange Tree"   , (Color){95 , 47 , 0  }, (Color){95 , 47 , 0  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_orange_tree, 6, 1, 1, 1 , -1, 0, 0},
  {"Palm Tree"     , (Color){127, 95 , 31 }, (Color){127, 95 , 31 }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_sand          , tile_palm_tree  , 8, 1, 1, 2 , -1, 0, 0},
  {"Pine Tree"     , (Color){103, 63 , 0  }, (Color){103, 63 , 0  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_pine_tree  , 4, 1, 1, 3 , -1, 0, 0},
  {"Beech Tree"    , (Color){95 , 79 , 7  }, (Color){95 , 79 , 7  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_beech_tree , 4, 1, 1, 4 , -1, 0, 0},
  {"Willow Tree"   , (Color){127, 75 , 47 }, (Color){127, 75 , 47 }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_willow_tree, 4, 1, 1, 5 , -1, 0, 0},
  {"Lemon Tree"    , (Color){95 , 47 , 0  }, (Color){95 , 47 , 0  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_lemon_tree , 6, 1, 1, 6 , -1, 0, 0},
  {"Birch Tree"    , (Color){255, 223, 159}, (Color){15 , 15 , 15 }, tile_color_dots, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_birch_tree , 4, 1, 1, 7 , -1, 0, 0},
  {"Ebony Tree"    , (Color){79 , 31 , 7  }, (Color){79 , 31 , 7  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_ebony_tree , 4, 1, 1, 8 , -1, 0, 0},
  {"Cherry Tree"   , (Color){94 , 57 , 6  }, (Color){94 , 57 , 6  }, tile_color_none, tile_type_solid   , 0, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_dirt          , tile_cherry_tree, 6, 1, 1, 10, -1, 0, 0},
  {"Apple Seed"    , (Color){63 , 31 , 0  }, (Color){63 , 31 , 0  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 0 , -1, 1, 0},
  {"Orange Seed"   , (Color){95 , 47 , 0  }, (Color){95 , 47 , 0  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 1 , -1, 1, 0},
  {"Palm Seed"     , (Color){127, 95 , 31 }, (Color){127, 95 , 31 }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 2 , -1, 1, 0},
  {"Pine Seed"     , (Color){103, 63 , 0  }, (Color){103, 63 , 0  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 3 , -1, 1, 0},
  {"Beech Seed"    , (Color){95 , 79 , 7  }, (Color){95 , 79 , 7  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 4 , -1, 1, 0},
  {"Willow Seed"   , (Color){127, 75 , 47 }, (Color){127, 75 , 47 }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 5 , -1, 1, 0},
  {"Lemon Seed"    , (Color){95 , 47 , 0  }, (Color){95 , 47 , 0  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 6 , -1, 1, 0},
  {"Birch Seed"    , (Color){255, 223, 159}, (Color){15 , 15 , 15 }, tile_color_dots, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 7 , -1, 1, 0},
  {"Ebony Seed"    , (Color){79 , 31 , 7  }, (Color){79 , 31 , 7  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 8 , -1, 1, 0},
  {"Cherry Seed"   , (Color){94 , 57 , 6  }, (Color){94 , 57 , 6  }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, 10, -1, 1, 0},
  {"Apple Leaves"  , (Color){31 , 127, 31 }, (Color){31 , 127, 31 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_apple_tree    , tile_apple_tree , 1, 1, 0, 0 , -1, 1, 0},
  {"Orange Leaves" , (Color){47 , 191, 47 }, (Color){47 , 191, 47 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_orange_tree   , tile_orange_tree, 1, 1, 0, 1 , -1, 1, 0},
  {"Palm Leaves"   , (Color){127, 255, 63 }, (Color){127, 255, 63 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_palm_tree     , tile_palm_tree  , 1, 1, 0, 2 , -1, 1, 0},
  {"Pine Leaves"   , (Color){55 , 143, 31 }, (Color){55 , 143, 31 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_pine_tree     , tile_pine_tree  , 1, 1, 0, 3 , -1, 1, 0},
  {"Beech Leaves"  , (Color){49 , 159, 59 }, (Color){49 , 159, 59 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_beech_tree    , tile_beech_tree , 1, 1, 0, 4 , -1, 1, 0},
  {"Willow Leaves" , (Color){71 , 127, 11 }, (Color){71 , 127, 11 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_willow_tree   , tile_willow_tree, 8, 1, 0, 5 , -1, 1, 0},
  {"Lemon Leaves"  , (Color){47 , 191, 47 }, (Color){47 , 191, 47 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_lemon_tree    , tile_lemon_tree , 1, 1, 0, 6 , -1, 1, 0},
  {"Birch Leaves"  , (Color){131, 199, 31 }, (Color){131, 199, 31 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_birch_tree    , tile_birch_tree , 1, 1, 0, 7 , -1, 1, 0},
  {"Ebony Leaves"  , (Color){23 , 95 , 7  }, (Color){23 , 95 , 7  }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_ebony_tree    , tile_ebony_tree , 1, 1, 0, 8 , -1, 1, 0},
  {"Cherry Leaves" , (Color){242, 171, 180}, (Color){252, 212, 217}, tile_color_dots, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_cherry_tree   , tile_cherry_tree, 1, 1, 0, 10, -1, 1, 0},
  {"Apple"         , (Color){255, 31 , 31 }, (Color){255, 31 , 31 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_apple_tree    , tile_apple_tree , 1, 1, 0, 0 , -1, 1, 0},
  {"Orange"        , (Color){255, 127, 31 }, (Color){255, 127, 31 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_orange_tree   , tile_orange_tree, 1, 1, 0, 1 , -1, 1, 0},
  {"Coconut"       , (Color){191, 95 , 31 }, (Color){191, 95 , 31 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_palm_tree     , tile_palm_tree  , 1, 1, 0, 2 , -1, 1, 0},
  {"Cone"          , (Color){159, 91 , 23 }, (Color){159, 91 , 23 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_pine_tree     , tile_pine_tree  , 1, 1, 0, 3 , -1, 1, 0},
  {"Lemon"         , (Color){255, 239, 47 }, (Color){255, 239, 47 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_lemon_tree    , tile_lemon_tree , 1, 1, 0, 6 , -1, 1, 0},
  {"Persimmon"     , (Color){240, 69 , 13 }, (Color){240, 69 , 13 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_ebony_tree    , tile_ebony_tree , 1, 1, 0, 8 , -1, 1, 0},
  {"Cherry"        , (Color){203, 2  , 25 }, (Color){203, 2  , 25 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_cherry_tree   , tile_cherry_tree, 1, 1, 0, 10, -1, 1, 0},
  {"Berry Bush"    , (Color){19,  79 , 19 }, (Color){19,  79 , 19 }, tile_color_none, tile_type_solid   , 1, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_dirt          , tile_berry_bush , 3, 1, 1, 9 , -1, 0, 0},
  {"Bush Leaves"   , (Color){39 , 159, 39 }, (Color){39 , 159, 39 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_berry_bush    , tile_berry_bush , 1, 1, 0, 0 , -1, 1, 0},
  {"Red Berry"     , (Color){127, 15 , 15 }, (Color){127, 15 , 15 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_berry_bush    , tile_berry_bush , 1, 1, 0, 0 , -1, 1, 0},
  {"Blue Berry"    , (Color){15 , 15 , 159}, (Color){15 , 15 , 159}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_soil , tile_berry_bush    , tile_berry_bush , 1, 1, 0, 0 , -1, 1, 0},
  {"Cacti"         , (Color){63 , 127, 23 }, (Color){63 , 127, 23 }, tile_color_none, tile_type_solid   , 1, 1, 0, 0, tile_ash  , 3 , 1 , tile_air  , tile_sand          , tile_cacti      , 1, 1, 1, -1, -1, 0, 0},
  {"Cacti Flower"  , (Color){223, 15 , 223}, (Color){223, 15 , 223}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 0 , tile_soil , tile_cacti         , tile_cacti      , 1, 1, 1, -1, -1, 1, 0},
  {"Algae"         , (Color){7  , 95 , 47 }, (Color){7  , 95 , 47 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_steam, 15, 1 , tile_water, tile_algae         , tile_dirt       , 1, 1, 1, -1, -1, 0, 0},
  {"Algae Top"     , (Color){23 , 95 , 63 }, (Color){23 , 95 , 63 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_steam, 15, 1 , tile_water, tile_algae         , tile_algae      , 1, 1, 1, -1, -1, 0, 0},
  {"Vines"         , (Color){23 , 95 , 23 }, (Color){23 , 95 , 23 }, tile_color_none, tile_type_solid   , 0, 1, 1, 0, tile_air  , 0 , 1 , tile_air  , tile_vines         , tile_stone      , 1, 1, 2, -1, -1, 1, 1},
  {"Mushroom"      , (Color){255, 223, 223}, (Color){255, 223, 223}, tile_color_none, tile_type_solid   , 1, 0, 0, 0, tile_ash  , 3 , 1 , tile_air  , tile_dirt          , tile_mushroom   , 1, 1, 1, -1, -1, 0, 0},
  {"Red Mushroom"  , (Color){255, 95 , 95 }, (Color){255, 95 , 95 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_red_mushroom  , tile_mushroom   , 1, 1, 1, -1, -1, 0, 0},
  {"Brown Mushroom", (Color){223, 159, 127}, (Color){223, 159, 127}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_brown_mushroom, tile_mushroom   , 1, 1, 1, -1, -1, 0, 0},
  {"Fire"          , (Color){255, 159, 31 }, (Color){255, 159, 31 }, tile_color_none, tile_type_gas     , 1, 1, 1, 0, tile_air  , 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Ash"           , (Color){23 , 23 , 23 }, (Color){23 , 23 , 23 }, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_ash  , 15, -1, tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Steam"         , (Color){43 , 43 , 43 }, (Color){43 , 43 , 43 }, tile_color_none, tile_type_gas     , 2, 1, 1, 0, tile_steam, 15, 0 , tile_water, tile_air           , tile_air        , 0, 0, 0, -1, -1, 1, 0},
  {"Soil"          , (Color){47 , 23 , 0  }, (Color){47 , 23 , 0  }, tile_color_none, tile_type_powder  , 2, 0, 0, 0, tile_dirt , 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0},   
  {"Wheat"         , (Color){135, 206, 28 }, (Color){135, 206, 28 }, tile_color_none, tile_type_solid   , 1, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_wheat         , tile_soil       , 1, 1, 1, -1, -1, 0, 0},
  {"Wheat Top"     , (Color){243, 249, 112}, (Color){243, 249, 112}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 0 , tile_air  , tile_wheat         , tile_wheat      , 1, 1, 1, -1, -1, 0, 0},
  {"Carrot"        , (Color){247, 132, 24 }, (Color){247, 132, 24 }, tile_color_none, tile_type_solid   , 1, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_carrot        , tile_soil       , 1, 1, 1, -1, -1, 0, 0},
  {"Carrot Leaves" , (Color){58 , 247, 24 }, (Color){58 , 247, 24 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 0 , tile_air  , tile_carrot        , tile_carrot     , 1, 1, 1, -1, -1, 0, 0},
  {"Fish"          , (Color){143, 143, 191}, (Color){143, 143, 191}, tile_color_none, tile_type_ai_water, 1, 1, 0, 0, tile_steam, 3 , 1 , tile_water, tile_water         , tile_water      , 1, 1, 0, -1, -1, 0, 0},
  {"Honey"         , (Color){255, 95 , 11 }, (Color){255, 223, 111}, tile_color_ceil, tile_type_liquid  , 2, 0, 1, 0, tile_air  , 15, 1 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Snow"          , (Color){239, 239, 255}, (Color){239, 239, 255}, tile_color_none, tile_type_powder  , 1, 0, 0, 0, tile_water, 15, -1, tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, 1 , 0, 0},
  {"Ice"           , (Color){159, 159, 255}, (Color){191, 191, 255}, tile_color_dots, tile_type_solid   , 1, 0, 0, 0, tile_water, 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Burn"          , (Color){223, 63 , 31 }, (Color){223, 63 , 31 }, tile_color_none, tile_type_solid   , 1, 0, 0, 0, tile_fire , 0 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Bee Hive"      , (Color){255, 191, 31 }, (Color){255, 191, 31 }, tile_color_none, tile_type_solid   , 1, 1, 0, 0, tile_air  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Bee"           , (Color){255, 255, 15 }, (Color){255, 255, 15 }, tile_color_none, tile_type_ai_fly  , 1, 1, 0, 0, tile_air  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Bee With Polen", (Color){255, 255, 15 }, (Color){255, 255, 15 }, tile_color_none, tile_type_ai_fly  , 0, 1, 0, 0, tile_air  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Ant"           , (Color){7  , 0  , 0  }, (Color){7  , 0  , 0  }, tile_color_none, tile_type_ai_land , 1, 1, 1, 0, tile_air  , 3 , 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, -1, 0, 0},
  {"Clay"          , (Color){191, 127, 63 }, (Color){181, 121, 59 }, tile_color_wet , tile_type_solid   , 1, 0, 0, 0, tile_clay , 15, 0 , tile_air  , tile_air           , tile_air        , 0, 0, 0, -1, 0 , 0, 0},
  {"Orange Fish"   , (Color){255, 143, 63 }, (Color){255, 143, 63 }, tile_color_none, tile_type_ai_water, 1, 1, 0, 0, tile_steam, 3 , 1 , tile_water, tile_water         , tile_water      , 1, 1, 0, -1, -1, 0, 0},
  {"White Fish"    , (Color){203, 203, 203}, (Color){203, 203, 203}, tile_color_none, tile_type_ai_water, 1, 1, 0, 0, tile_steam, 3 , 1 , tile_water, tile_water         , tile_water      , 1, 1, 0, -1, -1, 0, 0},
  {"Pink Fish"     , (Color){255, 127, 127}, (Color){255, 127, 127}, tile_color_none, tile_type_ai_water, 1, 1, 0, 0, tile_steam, 3 , 1 , tile_water, tile_water         , tile_water      , 1, 1, 0, -1, -1, 0, 0},
  {"Moss"          , (Color){7  , 167, 51 }, (Color){7  , 167, 51 }, tile_color_none, tile_type_solid   , 0, 1, 1, 0, tile_air  , 3 , 1 , tile_air  , tile_stone         , tile_stone      , 1, 1, 1, -1, -1, 1, 1},
  {"Caveroom"      , (Color){223, 191, 223}, (Color){223, 191, 223}, tile_color_none, tile_type_solid   , 1, 0, 0, 0, tile_ash  , 3 , 1 , tile_air  , tile_stone         , tile_caveroom   , 1, 1, 1, -1, -1, 0, 0},
  {"Red Caveroom"  , (Color){223, 31 , 95 }, (Color){223, 31 , 95 }, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_red_caveroom  , tile_caveroom   , 1, 1, 1, -1, -1, 0, 0},
  {"Brown Caveroom", (Color){191, 127, 127}, (Color){191, 127, 127}, tile_color_none, tile_type_solid   , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air  , tile_brown_caveroom, tile_caveroom   , 1, 1, 1, -1, -1, 0, 0},
};

static const tree_t tree_types[] = {
  {tile_apple_tree , 4 , tile_apple_leaves , 43, tile_apple    , tile_apple     , 5, 15, 6 , 1 , 39} ,
  {tile_orange_tree, 3 , tile_orange_leaves, 43, tile_orange   , tile_orange    , 4, 23, 16, 4 , 39} ,
  {tile_palm_tree  , 3 , tile_palm_leaves  , 19, tile_coconut  , tile_coconut   , 6, 15, 12, 1 , 16} ,
  {tile_pine_tree  , 6 , tile_pine_leaves  , 43, tile_cone     , tile_cone      , 6, 15, 6 , 4 , 43} ,
  {tile_beech_tree , 3 , tile_beech_leaves , 43, -1            , -1             , 0, 0 , 6 , 4 , 43} ,
  {tile_willow_tree, 3 , tile_willow_leaves, 39, -1            , -1             , 0, 0 , 7 , 8 , 39} ,
  {tile_lemon_tree , 3 , tile_lemon_leaves , 43, tile_lemon    , tile_lemon     , 4, 23, 14, 2 , 39} ,
  {tile_birch_tree , 3 , tile_birch_leaves , 35, -1            , -1             , 6, 15, 6 , 4 , 23} ,
  {tile_ebony_tree , 3 , tile_ebony_leaves , 39, tile_persimmon, tile_persimmon , 6, 15, 6 , 4 , 39} ,
  {tile_berry_bush , 5 , tile_bush_leaves  , 55, tile_red_berry, tile_blue_berry, 3, 55, 32, 28, 55} ,
  {tile_cherry_tree, 4 , tile_cherry_leaves, 43, tile_cherry   , tile_cherry    , 5, 15, 6 , 1 , 39} ,
};

#endif
