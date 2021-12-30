#ifndef __TREE_H__
#define __TREE_H__

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
  tile_apple_leaves,
  tile_orange_leaves,
  tile_palm_leaves,
  tile_pine_leaves,
  tile_beech_leaves,
  tile_willow_leaves,
  tile_lemon_leaves,
  tile_birch_leaves,
  tile_ebony_leaves,
  tile_apple,
  tile_orange,
  tile_coconut,
  tile_cone,
  tile_lemon,
  tile_persimmon,
  
  tile_count,
  
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
  tile_cacti,
  tile_cacti_flower,
  tile_steam,
  
  tile_count_2
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
  tile_type_gas     // fire, steam, will destroy itself after some random time
};

struct tile_t {
  char name_en[28]; // tile name in english
  
  Color color_1, color_2;
  int color_mode; // check tile_color_* in enum above
  
  int type; // tile type, check tile_type_* in enum above
  
  int show; // show in the selection menu
  int flow; // only wet tiles of the same type
  int sink; // float above powders, making those sink
  int soil; // spread fertile levels
  
  int fire_tile; // tile it is replaced with when burnt(set to same tile to not burn)
  int fire_amnt; // chance it will be replaced with that tile and not fire(0 being all fire, 15 being all that tile)
  
  int spread; // tile_type_powder: if water level GREATER(not greater or equal) than this, will not only fall(like dry dirt) but also spread(like sand or wet dirt), set to -1 to act like sand
              // tile_type_liquid: viscosity, from 0 to 15, being 15 the most liquid-ey
              // tile_type_solid:  minimum water level needed to not disappear
  int remove; // tile it will be replaced with when destroying it(should be air for most blocks, water for algae and steam, etc.)
  
  int need_tile_1; // tile it needs to have close to not disappear
  int need_tile_2; // tile it needs to have close to not disappear(either this one or need_tile_1)
  int need_dist; // distance that tile has to be within to survive
  int need_amnt; // amount of tiles needed to survive(or 0 if it doesn't need any tiles near)
  int need_root; // only check tiles below(includes diagonals), but not the ones on the sides or above
  
  int tree_type; // tree type index, -1 if not part of a tree
};

struct tree_t {
  int tree_tile; // tree/trunk tile
  int tree_dist; // minimum distance between trees
  
  int leaf_tile; // leaf tile
  int leaf_grow; // maximum water level for leaves to grow(minimum is always 1)
  
  int fruit_tile; // fruit tile, or -1 if fruitless
  int fruit_dist; // minimum distance between fruits
  int fruit_grow; // maximum water level for fruits to grow(minimum is always 1)
  
  int upward_min; // minimum water for the tree to grow upwards
  int branch_min; // minimum water for branches to grow
  int branch_max; // maximum water for branches to grow
};

static const tile_t tile_types[] = {
  {"Air"                        , (Color){51 , 51 , 51 }, (Color){51 , 51 , 51 }, tile_color_none, tile_type_solid , 1, 1, 1, 0, tile_air  , 15, 0 , tile_air, tile_air        , tile_air        , 0, 0, 0, -1},
  {"Dirt"                       , (Color){127, 63 , 0  }, (Color){114, 56 , 0  }, tile_color_wet , tile_type_powder, 1, 0, 0, 1, tile_dirt , 15, 0 , tile_air, tile_air        , tile_air        , 0, 0, 0, -1},
  {"Water"                      , (Color){63 , 63 , 255}, (Color){191, 191, 255}, tile_color_ceil, tile_type_liquid, 1, 0, 1, 0, tile_steam, 15, 15, tile_air, tile_air        , tile_air        , 0, 0, 0, -1},
  {"Grass"                      , (Color){63 , 255, 63 }, (Color){63 , 255, 63 }, tile_color_none, tile_type_solid , 0, 1, 0, 1, tile_air  , 0 , 1 , tile_air, tile_grass      , tile_air        , 1, 3, 0, -1},
  {"Pink Flower"                , (Color){255, 127, 127}, (Color){255, 127, 127}, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 0 , tile_air, tile_grass      , tile_grass      , 1, 1, 0, -1},
  {"Blue Flower"                , (Color){127, 127, 255}, (Color){127, 127, 255}, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 0 , tile_air, tile_grass      , tile_grass      , 1, 1, 0, -1},
  {"Yellow Flower"              , (Color){255, 255, 63 }, (Color){255, 255, 63 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 0 , tile_air, tile_grass      , tile_grass      , 1, 1, 0, -1},
  {"Stone"                      , (Color){127, 127, 127}, (Color){114, 114, 114}, tile_color_wet , tile_type_solid , 1, 0, 0, 0, tile_stone, 15, 0 , tile_air, tile_air        , tile_air        , 0, 0, 0, -1},
  {"Sand"                       , (Color){255, 255, 127}, (Color){229, 229, 114}, tile_color_wet , tile_type_powder, 1, 0, 0, 1, tile_sand , 15, -1, tile_air, tile_air        , tile_air        , 0, 0, 0, -1},
  {"Iron"                       , (Color){191, 191, 191}, (Color){191, 191, 191}, tile_color_none, tile_type_solid , 1, 1, 0, 0, tile_iron , 15, 0 , tile_air, tile_air        , tile_air        , 0, 0, 0, -1},
  {"Apple Tree"                 , (Color){63 , 31 , 0  }, (Color){63 , 31 , 0  }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_apple_tree , 6, 1, 1, 0 },
  {"Orange Tree"                , (Color){95 , 47 , 0  }, (Color){95 , 47 , 0  }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_orange_tree, 6, 1, 1, 1 },
  {"Palm Tree"                  , (Color){127, 95 , 31 }, (Color){127, 95 , 31 }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_sand       , tile_palm_tree  , 8, 1, 1, 2 },
  {"Pine Tree"                  , (Color){103, 63 , 0  }, (Color){103, 63 , 0  }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_pine_tree  , 4, 1, 1, 3 },
  {"Beech Tree"                 , (Color){95 , 79 , 7  }, (Color){95 , 79 , 7  }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_beech_tree , 4, 1, 1, 4 },
  {"Willow Tree"                , (Color){127, 75 , 47 }, (Color){127, 75 , 47 }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_willow_tree, 4, 1, 1, 5 },
  {"Lemon Tree"                 , (Color){95 , 47 , 0  }, (Color){95 , 47 , 0  }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_lemon_tree , 6, 1, 1, 6 },
  {"Birch Tree"                 , (Color){255, 223, 159}, (Color){15 , 15 , 15 }, tile_color_dots, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_birch_tree , 4, 1, 1, 7 },
  {"Ebony Tree"                 , (Color){79 , 31 , 7  }, (Color){79 , 31 , 7  }, tile_color_none, tile_type_solid , 1, 0, 0, 0, tile_ash  , 3 , 0 , tile_air, tile_dirt       , tile_ebony_tree , 4, 1, 1, 8 },
  {"Apple Leaves"               , (Color){31 , 127, 31 }, (Color){31 , 127, 31 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_apple_tree , tile_apple_tree , 1, 1, 0, 0 },
  {"Orange Leaves"              , (Color){47 , 191, 47 }, (Color){47 , 191, 47 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_orange_tree, tile_orange_tree, 1, 1, 0, 1 },
  {"Palm Leaves"                , (Color){127, 255, 63 }, (Color){127, 255, 63 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_palm_tree  , tile_palm_tree  , 1, 1, 0, 2 },
  {"Pine Leaves"                , (Color){55 , 143, 31 }, (Color){55 , 143, 31 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_pine_tree  , tile_pine_tree  , 1, 1, 0, 3 },
  {"Beech Leaves"               , (Color){49 , 159, 59 }, (Color){49 , 159, 59 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_beech_tree , tile_beech_tree , 1, 1, 0, 4 },
  {"Willow Leaves"              , (Color){71 , 127, 11 }, (Color){71 , 127, 11 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_willow_tree, tile_willow_tree, 1, 1, 0, 5 },
  {"Lemon Leaves"               , (Color){47 , 191, 47 }, (Color){47 , 191, 47 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_lemon_tree , tile_lemon_tree , 1, 1, 0, 6 },
  {"Birch Leaves"               , (Color){131, 199, 31 }, (Color){131, 199, 31 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_birch_tree , tile_birch_tree , 1, 1, 0, 7 },
  {"Ebony Leaves"               , (Color){23 , 95 , 7  }, (Color){23 , 95 , 7  }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_ebony_tree , tile_ebony_tree , 1, 1, 0, 8 },
  {"Apple"                      , (Color){255, 31 , 31 }, (Color){255, 31 , 31 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_apple_tree , tile_apple_tree , 1, 1, 0, 0 },
  {"Orange"                     , (Color){255, 127, 31 }, (Color){255, 127, 31 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_orange_tree, tile_orange_tree, 1, 1, 0, 1 },
  {"Coconut"                    , (Color){191, 95 , 31 }, (Color){191, 95 , 31 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_palm_tree  , tile_palm_tree  , 1, 1, 0, 2 },
  {"Cone"                       , (Color){159, 91 , 23 }, (Color){159, 91 , 23 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_pine_tree  , tile_pine_tree  , 1, 1, 0, 3 },
  {"Lemon"                      , (Color){255, 239, 47 }, (Color){255, 239, 47 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_lemon_tree , tile_lemon_tree , 1, 1, 0, 6 },
  {"Persimmon"                  , (Color){240, 69 , 13 }, (Color){240, 69 , 13 }, tile_color_none, tile_type_solid , 0, 1, 0, 0, tile_air  , 0 , 1 , tile_air, tile_ebony_tree , tile_ebony_tree , 1, 1, 0, 8 },
};

static const tree_t tree_types[] = {
  {tile_apple_tree , 4 , tile_apple_leaves , 43, tile_apple    , 5, 15, 6 , 1 , 39}, // DONE
  {tile_orange_tree, 3 , tile_orange_leaves, 43, tile_orange   , 4, 23, 16, 4 , 39}, // DONE
  {tile_palm_tree  , 3 , tile_palm_leaves  , 19, tile_coconut  , 6, 15, 12, 1 , 16}, // DONE
  {tile_pine_tree  , 6 , tile_pine_leaves  , 43, tile_cone     , 6, 15, 6 , 4 , 43}, // DONE
  {tile_beech_tree , 3 , tile_beech_leaves , 43, -1            , 0, 0 , 6 , 4 , 43},
  {tile_willow_tree, 3 , tile_willow_leaves, 39, -1            , 0, 0 , 6 , 4 , 39},
  {tile_lemon_tree , 3 , tile_lemon_leaves , 43, tile_lemon    , 4, 23, 14, 2 , 39}, // DONE
  {tile_birch_tree , 3 , tile_birch_leaves , 35, -1            , 6, 15, 6 , 4 , 23}, // DONE
  {tile_ebony_tree , 3 , tile_ebony_leaves , 39, tile_persimmon, 6, 15, 6 , 4 , 39},
};

static const tile_dirt_count = 8;

// TODO: use
static const int tile_dirt_grow[] = {
  tile_apple_tree,
  tile_orange_tree,
  tile_pine_tree,
  tile_beech_tree,
  tile_willow_tree,
  tile_lemon_tree,
  tile_birch_tree,
  tile_ebony_tree
};

#endif
