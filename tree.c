#include <default.h>
#include <perlin.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int *old_tiles = NULL;
int *old_water = NULL;
int *old_plant = NULL;

int *tree_tiles = NULL;
int *tree_water = NULL;
int *tree_plant = NULL;

int selection = tile_dirt;
int brush_size = 5;

int off_x = 0;
int off_y = 0;

int start_x = 0;
int start_y = 0;

int old_x = 0;
int old_y = 0;

int zoom = SCALE;
int tick = 0;

int paused = 0;

void set_tile(int x, int y, int tile) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_tiles[x + y * WIDTH] = tile;
}

int get_tile(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return tile_air;
  return old_tiles[x + y * WIDTH];
}

int get_tile_new(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return tile_air;
  return tree_tiles[x + y * WIDTH];
}

void set_water(int x, int y, int water) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_water[x + y * WIDTH] = water;
}

int get_water(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return old_water[x + y * WIDTH];
}

int get_water_new(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return tree_water[x + y * WIDTH];
}

void set_plant(int x, int y, int plant) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_plant[x + y * WIDTH] = plant;
}

int get_plant(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return old_plant[x + y * WIDTH];
}

int get_plant_new(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return tree_plant[x + y * WIDTH];
}

void world_gen(void) {
  for (int i = 0; i < WIDTH; i++) {
    int height = (int)(noise_1(i / 59.7f) * 50 + noise_1(i / 61.3f) * 40 + noise_1(i / 55.1f) * 40);
    
    for (int j = 0; j < HEIGHT; j++) {
      if (j == HEIGHT - 1 && WORLD_WRAP) {
        set_tile(i, j, tile_stone);
        continue;
      }
      
      if (HEIGHT - j < height) {
        int depth = HEIGHT - j;
        
        if (noise_2(i / 47.0f, j / 43.0f) >= lerp(0.4f, 0.8f, depth / 50.0f)) {
          set_tile(i, j, tile_stone);
        } else if (noise_2(i / 55.0f, j / 51.0f) >= lerp(0.6f, 0.9f, (height - depth) / 50.0f)) {
          set_tile(i, j, tile_sand);
        } else {
          set_tile(i, j, tile_dirt);
        }
      } else if (HEIGHT - j < 65) {
        set_tile(i, j, tile_water);
      }
    }
  }
}

void swap(int x1, int y1, int x2, int y2) {
  if (!WORLD_WRAP) {
    if (x1 < 0 || y1 < 0 || x1 >= WIDTH || y1 >= HEIGHT) return;
    if (x2 < 0 || y2 < 0 || x2 >= WIDTH || y2 >= HEIGHT) return;
  }
  
  int tile = get_tile_new(x1, y1);
  int water = get_water_new(x1, y1);
  int plant = get_plant_new(x1, y1);
  
  set_tile(x1, y1, get_tile_new(x2, y2));
  set_water(x1, y1, get_water_new(x2, y2));
  set_plant(x1, y1, get_plant_new(x2, y2));
  
  set_tile(x2, y2, tile);
  set_water(x2, y2, water);
  set_plant(x2, y2, plant);
}

void set_circle(int x, int y, int tile) {
  for (int dx = -brush_size; dx <= brush_size; dx++) {
    for (int dy = -brush_size; dy <= brush_size; dy++) {
      if (dx * dx + dy * dy < brush_size * brush_size) {
        if (REPLACE_BLOCKS || get_tile_new(x + dx, y + dy) == tile_air || tile == tile_air) {
          set_tile(x + dx, y + dy, tile);
        }
      }
    }
  }
}

Color get_color(int tile) {
  return tile_colors[tile];
}

void tick_tiles(void) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (get_tile(j, i) == tile_water) {
        if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j, i + 1) == tile_air) {
          swap(j, i, j, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          if (rand() % 2) {
            swap(j, i, j - 1, i + 1);
          } else {
            swap(j, i, j + 1, i + 1);
          }
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j - 1, i + 1) == tile_air) {
          swap(j, i, j - 1, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j + 1, i + 1) == tile_air) {
          swap(j, i, j + 1, i + 1);
        } else {
          if (rand() % 2) {
            if (get_tile(j - 1, i) == tile_air) {
              swap(j, i, j - 1, i);
            }
          } else {
            if (get_tile(j + 1, i) == tile_air) {
              swap(j, i, j + 1, i);
            }
          }
        }
      } else if (get_tile(j, i) == tile_dirt) {
        if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j, i + 1) == tile_air || get_tile(j, i + 1) == tile_water) {
          swap(j, i, j, i + 1);
        } else if (rand() % 256 < get_water(j, i)) {
          if ((i < HEIGHT - 1 || WORLD_WRAP) && (get_tile(j - 1, i + 1) == tile_air || get_tile(j - 1, i + 1) == tile_water) && (get_tile(j + 1, i + 1) == tile_air || get_tile(j + 1, i + 1) == tile_water)) {
            if (rand() % 2) {
              swap(j, i, j - 1, i + 1);
            } else {
              swap(j, i, j + 1, i + 1);
            }
          } else if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j - 1, i + 1) == tile_air || get_tile(j - 1, i + 1) == tile_water) {
            swap(j, i, j - 1, i + 1);
          } else if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j + 1, i + 1) == tile_air || get_tile(j + 1, i + 1) == tile_water) {
            swap(j, i, j + 1, i + 1);
          }
        } else {
          if (rand() % 2048 < (get_water(j, i) + get_plant(j, i)) && get_water(j, i) > 0 && get_tile(j, i - 1) == tile_air) {
            set_tile(j, i - 1, tile_grass);
          } else if (rand() % 3072 < (get_water(j, i) + get_plant(j, i)) && get_tile(j, i - 1) == tile_water && get_tile(j, i - 2) == tile_water && get_tile(j, i - 3) == tile_water &&
                     get_tile(j, i - 4) == tile_water && get_tile(j, i - 5) == tile_water && get_tile(j, i - 6) == tile_water && get_tile(j, i - 7) == tile_water) {
            set_tile(j, i - 1, tile_algae);
          } else if (rand() % 1024 < get_plant(j, i) && get_water(j, i) >= 12 && get_tile(j, i - 1) == tile_grass) {
            int valid = 1;
            
            for (int dy = -8; dy <= 8; dy++) {
              for (int dx = -12; dx <= 12; dx++) {
                if (get_tile(j + dx, i + dy - 1) == tile_apple_tree || get_tile(j + dx, i + dy - 1) == tile_orange_tree || get_tile(j + dx, i + dy - 1) == tile_lemon_tree ||
                    get_tile(j + dx, i + dy - 1) == tile_pine_tree || get_tile(j + dx, i + dy - 1) == tile_beech_tree || get_tile(j + dx, i + dy - 1) == tile_birch_tree ||
                    get_tile(j + dx, i + dy - 1) == tile_willow_tree || get_tile(j + dx, i + dy - 1) == tile_ebony_tree) {
                  valid = 0;
                  break;
                } else if (dx >= -8 && dx <= 8 && (get_tile(j + dx, i + dy - 1) == tile_mushroom || get_tile(j + dx, i + dy - 1) == tile_berry_bush)) {
                  valid = 0;
                  break;
                }
              }
              
              if (!valid) break;
            }
            
            if (valid) {
              switch (rand() % 24) {
                case 0: case 1: case 2: case 3:
                  set_tile(j, i - 1, tile_apple_tree);
                  break;
                case 4: case 5: case 6:
                  set_tile(j, i - 1, tile_orange_tree);
                  break;
                case 7: case 8: case 9:
                  set_tile(j, i - 1, tile_lemon_tree);
                  break;
                case 10: case 11:
                  set_tile(j, i - 1, tile_pine_tree);
                  break;
                case 12: case 13:
                  set_tile(j, i - 1, tile_beech_tree);
                  break;
                case 14: case 15:
                  set_tile(j, i - 1, tile_birch_tree);
                  break;
                case 16:
                  set_tile(j, i - 1, tile_willow_tree);
                  break;
                case 17:
                  set_tile(j, i - 1, tile_ebony_tree);
                  break;
                case 18: case 19: case 20:
                  set_tile(j, i - 1, tile_mushroom);
                  break;
                case 21: case 22: case 23:
                  set_tile(j, i - 1, tile_berry_bush);
                  break;
              }
            }
          }
        }
      } else if (get_tile(j, i) == tile_grass) {
        if ((rand() % 64 == 0 && get_water(j, i) == 0) || (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_grass &&
                                                           get_tile(j, i - 1) != tile_flower_pink && get_tile(j, i - 1) != tile_flower_blue && get_tile(j, i - 1) != tile_flower_yellow)) {
          set_tile(j, i, tile_air);
        } else if ((get_tile(j - 1, i + 1) != tile_dirt && get_tile(j - 1, i + 1) != tile_grass) &&
            (get_tile(j + 1, i + 1) != tile_dirt && get_tile(j + 1, i + 1) != tile_grass) && 
            (get_tile(j, i + 1) != tile_dirt && get_tile(j, i + 1) != tile_grass)) {
          set_tile(j, i, tile_air);
        } else if (rand() % 1024 < get_water(j, i)) {
          int x = (rand() % 3) - 1;
          
          int valid = 1;
          
          for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
              if (get_tile(j + x + dx, i + dy) == tile_flower_pink || get_tile(j + x + dx, i + dy) == tile_flower_blue || get_tile(j + x + dx, i + dy) == tile_flower_yellow) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + x + dx, i + dy) == tile_grass) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid && (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water)) {
            if (rand() % 4 == 0) {
              switch (rand() % 3) {
                case 0:
                  set_tile(j + x, i - 1, tile_flower_pink);
                  break;
                case 1:
                  set_tile(j + x, i - 1, tile_flower_blue);
                  break;
                case 2:
                  set_tile(j + x, i - 1, tile_flower_yellow);
                  break;
              }
            } else {
              set_tile(j + x, i - 1, tile_grass);
            }
          }
        }
      } else if (get_tile(j, i) == tile_flower_pink || get_tile(j, i) == tile_flower_blue || get_tile(j, i) == tile_flower_yellow) {
        if (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_grass &&
            get_tile(j, i - 1) != tile_flower_pink && get_tile(j, i - 1) != tile_flower_blue && get_tile(j, i - 1) != tile_flower_yellow) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_grass) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_apple_tree) {
        int valid = 0;
        int t = 4;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_apple_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_apple_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_apple_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_apple_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 6)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_apple_leaves) {
            set_tile(j + x, i - 1, tile_apple_tree);
          }
        } else if (!valid && get_water(j, i) < 32 && get_water(j, i) > 3 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_apple_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_apple_leaves)) {
            set_tile(j + x, i, tile_apple_tree);
          }
        }
        
        valid = 1;
        
        for (int dx = -6; dx <= 6; dx++) {
          for (int dy = -6; dy <= 6; dy++) {
            if (get_tile(j + dx, i + dy) == tile_apple) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (valid && get_water(j, i) > 0 && get_water(j, i) <= 15 && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_apple);
        }
      } else if (get_tile(j, i) == tile_apple_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_apple_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_apple_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_apple) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_apple_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_orange_tree) {
        int valid = 0;
        int t = 4;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_orange_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_orange_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_orange_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_orange_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 10)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_orange_leaves) {
            set_tile(j + x, i - 1, tile_orange_tree);
          }
        } else if (!valid && get_water(j, i) < 36 && get_water(j, i) > 3 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_orange_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_orange_leaves) &&
              (get_tile(j + x, i - 2) == tile_air || get_tile(j + x, i - 2) == tile_water || get_tile(j + x, i - 2) == tile_grass || get_tile(j + x, i - 2) == tile_orange_leaves) &&
              (get_tile(j + x, i + 2) == tile_air || get_tile(j + x, i + 2) == tile_water || get_tile(j + x, i + 2) == tile_grass || get_tile(j + x, i + 2) == tile_orange_leaves)) {
            set_tile(j + x, i, tile_orange_tree);
          }
        }
        
        valid = 1;
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -3; dy <= 3; dy++) {
            if (get_tile(j + dx, i + dy) == tile_orange) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (valid && get_water(j, i) > 0 && get_water(j, i) <= 23 && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_orange);
        }
      } else if (get_tile(j, i) == tile_orange_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_orange_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_orange_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_orange) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_orange_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_sand) {
        if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j, i + 1)]) {
          swap(j, i, j, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j - 1, i + 1)] && tile_light[get_tile(j + 1, i + 1)]) {
          if (rand() % 2) {
            swap(j, i, j - 1, i + 1);
          } else {
            swap(j, i, j + 1, i + 1);
          }
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j - 1, i + 1)]) {
          swap(j, i, j - 1, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j + 1, i + 1)]) {
          swap(j, i, j + 1, i + 1);
        } else {
          if (rand() % 1024 < get_plant(j, i) && get_water(j, i) >= 20 && get_tile(j, i - 1) == tile_air) {
            int valid = 1;
            
            for (int dy = -12; dy <= 12; dy++) {
              for (int dx = -15; dx <= 15; dx++) {
                if (get_tile(j + dx, i + dy - 1) == tile_palm_tree || (!(dx < -2 || dy < -2 || dx > 2 || dy > 2) && get_tile(j + dx, i + dy) == tile_water) ||
                    (!(dx < -6 || dy < -6 || dx > 6 || dy > 6) && get_tile(j + dx, i + dy) == tile_cacti)) {
                  valid = 0;
                  break;
                }
              }
              
              if (!valid) break;
            }
            
            if (valid) {
              if (rand() % 3 == 0) {
                set_tile(j, i - 1, tile_palm_tree);
              } else {
                set_tile(j, i - 1, tile_cacti);
              }
            }
          }
        }
      } else if (get_tile(j, i) == tile_stone) {
        if (rand() % 16384 == 0 && (get_tile(j - 1, i) == tile_water || get_tile(j + 1, i) == tile_water || get_tile(j, i - 1) == tile_water || get_tile(j, i + 1) == tile_water)) {
          set_tile(j, i, tile_sand);
        } else if (rand() % 2048 < get_water(j, i)) {
          if (get_tile(j, i + 1) == tile_air) {
            set_tile(j, i + 1, tile_vines);
          }
        }
      } else if (get_tile(j, i) == tile_berry_bush) {
        if ((rand() % 256 == 0 && get_water(j, i) == 0) || (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_berry_bush &&
                                                            get_tile(j, i - 1) != tile_bush_leaves && get_tile(j, i - 1) != tile_red_berry && get_tile(j, i - 1) != tile_blue_berry &&
                                                            get_tile(j, i - 1) != tile_grass)) {
          set_tile(j, i, tile_air);
        } else if ((get_tile(j - 1, i + 1) != tile_dirt && get_tile(j - 1, i + 1) != tile_berry_bush) &&
            (get_tile(j + 1, i + 1) != tile_dirt && get_tile(j + 1, i + 1) != tile_berry_bush) && 
            (get_tile(j, i + 1) != tile_dirt && get_tile(j, i + 1) != tile_berry_bush)) {
          set_tile(j, i, tile_air);
        } else if (rand() % 256 < get_water(j, i)) {
          int valid = 1;
          
          for (int dx = -5; dx <= 5; dx++) {
            for (int dy = -5; dy <= 5; dy++) {
              if (get_tile(j + dx, i + dy) == tile_bush_leaves) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + dx, i + dy) == tile_berry_bush) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid && (get_tile(j, i - 1) == tile_air || get_tile(j, i - 1) == tile_grass || get_tile(j, i - 1) == tile_flower_pink || get_tile(j, i - 1) == tile_flower_blue || get_tile(j, i - 1) == tile_flower_yellow)) {
            if (get_water(j, i) < 48 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
              set_tile(j, i - 1, tile_bush_leaves);
            } else {
              set_tile(j, i - 1, tile_berry_bush);
            }
          }
        }
      } else if (get_tile(j, i) == tile_bush_leaves) {
        if (rand() % 128 == 0 && (get_water(j, i) < 16 || (get_tile(j, i + 1) != tile_air && get_tile(j, i + 1) != tile_red_berry && get_tile(j, i + 1) != tile_blue_berry &&
            get_tile(j, i + 1) != tile_berry_bush && get_tile(j, i + 1) != tile_bush_leaves))) {
          set_tile(j, i, tile_air);
        } else if (get_water(j, i) >= 42 && get_water(j, i) < 48) {
          int berry = tile_red_berry;
          
          if (rand() % 6 == 0 && get_tile(j - 1, i) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j - 2, i) == tile_air) {
            set_tile(j - 1, i, tile_bush_leaves);
          }
          
          if (rand() % 6 == 0 && get_tile(j + 1, i) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j + 2, i) == tile_air) {
            set_tile(j + 1, i, tile_bush_leaves);
          }
          
          if (rand() % 24 == 0 && get_tile(j, i - 1) == tile_air) {
            set_tile(j, i - 1, tile_bush_leaves);
          }
        }
      } else if (get_tile(j, i) == tile_red_berry || get_tile(j, i) == tile_blue_berry) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_bush_leaves) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_vines) {
        int water = (get_water(j, i) / 1.5f) - 12;
        
        if (get_tile(j - 1, i) == tile_vines) {
          water -= 8;
        }
        
        if (get_tile(j + 1, i) == tile_vines) {
          water -= 8;
        }
        
        if (rand() % 128 == 0 && (water == 0 || get_tile(j, i - 1) == tile_air || get_tile(j, i - 1) == tile_water)) {
          set_tile(j, i, tile_air);
        } else if (rand() % 512 < water) {
          if (get_tile(j, i + 1) == tile_air) {
            set_tile(j, i + 1, tile_vines);
          }
        }
      } else if (get_tile(j, i) == tile_mushroom) {
        if ((rand() % 256 == 0 && get_water(j, i) == 0) || (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_mushroom &&
                                                            get_tile(j, i - 1) != tile_red_mushroom && get_tile(j, i - 1) != tile_brown_mushroom && get_tile(j, i - 1) != tile_grass)) {
          set_tile(j, i, tile_air);
        } else if ((get_tile(j - 1, i + 1) != tile_dirt && get_tile(j - 1, i + 1) != tile_mushroom) &&
            (get_tile(j + 1, i + 1) != tile_dirt && get_tile(j + 1, i + 1) != tile_mushroom) && 
            (get_tile(j, i + 1) != tile_dirt && get_tile(j, i + 1) != tile_mushroom)) {
          set_tile(j, i, tile_air);
        } else if (rand() % 256 < get_water(j, i)) {
          int valid = 1;
          
          for (int dx = -5; dx <= 5; dx++) {
            for (int dy = -5; dy <= 5; dy++) {
              if (get_tile(j + dx, i + dy) == tile_red_mushroom || get_tile(j + dx, i + dy) == tile_brown_mushroom) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + dx, i + dy) == tile_mushroom) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid && (get_tile(j, i - 1) == tile_air || get_tile(j, i - 1) == tile_grass || get_tile(j, i - 1) == tile_flower_pink || get_tile(j, i - 1) == tile_flower_blue || get_tile(j, i - 1) == tile_flower_yellow)) {
            if (get_water(j, i) < 44 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
              switch (rand() % 2) {
                case 0:
                  set_tile(j, i - 1, tile_red_mushroom);
                  break;
                case 1:
                  set_tile(j, i - 1, tile_brown_mushroom);
                  break;
              }
            } else {
              set_tile(j, i - 1, tile_mushroom);
            }
          }
        }
      } else if (get_tile(j, i) == tile_red_mushroom || get_tile(j, i) == tile_brown_mushroom) {
        if ((rand() % 256 == 0 && get_water(j, i) < 8) || (get_tile(j, i + 1) != tile_air && get_tile(j, i + 1) != tile_mushroom && get_tile(j, i + 1) != tile_red_mushroom && get_tile(j, i + 1) != tile_brown_mushroom)) {
          set_tile(j, i, tile_air);
        } else if (get_water(j, i) >= 40 && get_water(j, i) < 44) {
          if (rand() % 8 == 0 && get_tile(j - 1, i) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j - 2, i) == tile_air) {
            set_tile(j - 1, i, get_tile(j, i));
          }
          
          if (rand() % 8 == 0 && get_tile(j + 1, i) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j + 2, i) == tile_air) {
            set_tile(j + 1, i, get_tile(j, i));
          }
          
          if (rand() % 32 == 0 && get_tile(j, i - 1) == tile_air) {
            set_tile(j, i - 1, get_tile(j, i));
          }
        }
      } else if (get_tile(j, i) == tile_palm_tree) {
        int valid = 0;
        int t = 12;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_sand || get_tile(j + dx, i + dy) == tile_palm_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_palm_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) >= 4 && get_water(j, i) < 16) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_palm_leaves && rand() % 8 == 0) {
              set_tile(j + dx, i - 1, tile_palm_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 6)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_palm_leaves) {
            set_tile(j + x, i - 1, tile_palm_tree);
          }
        } else if (!valid && get_water(j, i) < 16 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_palm_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_palm_leaves) &&
              (get_tile(j + x, i - 2) == tile_air || get_tile(j + x, i - 2) == tile_water || get_tile(j + x, i - 2) == tile_palm_leaves) &&
              (get_tile(j + x, i + 2) == tile_air || get_tile(j + x, i + 2) == tile_water || get_tile(j + x, i + 2) == tile_palm_leaves)) {
            set_tile(j + x, i, tile_palm_tree);
          }
        }
        
        valid = 1;
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -3; dy <= 3; dy++) {
            if (get_tile(j + dx, i + dy) == tile_coconut) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (valid && get_water(j, i) > 0 && get_water(j, i) <= 23 && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_coconut);
        }
      } else if (get_tile(j, i) == tile_palm_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_palm_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || !(get_water(j, i) > 0 && get_water(j, i) < 16) || get_tile(j, i - 1) == tile_palm_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_coconut) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_palm_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_fire) {
        if (rand() % 64 == 0 || get_tile(j - 1, i) == tile_water || get_tile(j + 1, i) == tile_water || get_tile(j, i - 1) == tile_water || get_tile(j, i + 1) == tile_water) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        if (rand() % 4 == 0 && tile_flamable[get_tile(j - 1, i)]) {
          set_tile(j - 1, i, (rand() % 3 == 0 && tile_wooden[get_tile(j - 1, i)]) ? tile_ash : tile_fire);
        }
        
        if (rand() % 4 == 0 && tile_flamable[get_tile(j + 1, i)]) {
          set_tile(j + 1, i, (rand() % 3 == 0 && tile_wooden[get_tile(j + 1, i)]) ? tile_ash : tile_fire);
        }
        
        if (rand() % 128 == 0 && tile_flamable[get_tile(j, i - 1)]) {
          set_tile(j, i - 1, (rand() % 3 == 0 && tile_wooden[get_tile(j, i - 1)]) ? tile_ash : tile_fire);
        }
        
        if (rand() % 2 == 0 && tile_flamable[get_tile(j, i + 1)]) {
          set_tile(j, i + 1, (rand() % 3 == 0 && tile_wooden[get_tile(j, i + 1)]) ? tile_ash : tile_fire);
        }
        
        if ((i > 0 || WORLD_WRAP) && get_tile(j, i - 1) == tile_air) {
          swap(j, i, j, i - 1);
        } else if ((i > 0 || WORLD_WRAP) && get_tile(j - 1, i - 1) == tile_air && get_tile(j + 1, i - 1) == tile_air) {
          if (rand() % 2) {
            swap(j, i, j - 1, i - 1);
          } else {
            swap(j, i, j + 1, i - 1);
          }
        } else if ((i > 0 || WORLD_WRAP) && get_tile(j - 1, i - 1) == tile_air) {
          swap(j, i, j - 1, i - 1);
        } else if ((i > 0 || WORLD_WRAP) && get_tile(j + 1, i - 1) == tile_air) {
          swap(j, i, j + 1, i - 1);
        } else {
          if (rand() % 2) {
            if (get_tile(j - 1, i) == tile_air) {
              swap(j, i, j - 1, i);
            }
          } else {
            if (get_tile(j + 1, i) == tile_air) {
              swap(j, i, j + 1, i);
            }
          }
        }
      } else if (get_tile(j, i) == tile_fertilizer) {
        if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j, i + 1)]) {
          swap(j, i, j, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j - 1, i + 1)] && tile_light[get_tile(j + 1, i + 1)]) {
          if (rand() % 2) {
            swap(j, i, j - 1, i + 1);
          } else {
            swap(j, i, j + 1, i + 1);
          }
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j - 1, i + 1)]) {
          swap(j, i, j - 1, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j + 1, i + 1)]) {
          swap(j, i, j + 1, i + 1);
        }
      } else if (get_tile(j, i) == tile_algae) {
        if (get_water(j, i) < 48 && get_tile(j, i - 1) != tile_water && get_tile(j - 1, i) != tile_water && get_tile(j + 1, i) != tile_water) {
          set_tile(j, i, tile_air);
        } else if ((get_tile(j - 1, i + 1) != tile_dirt && get_tile(j - 1, i + 1) != tile_algae) &&
            (get_tile(j + 1, i + 1) != tile_dirt && get_tile(j + 1, i + 1) != tile_algae) && 
            (get_tile(j, i + 1) != tile_dirt && get_tile(j, i + 1) != tile_algae)) {
          set_tile(j, i, tile_air);
        } else if (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_algae && get_tile(j, i - 1) != tile_algae_top) {
          set_tile(j, i, tile_air);
        } else if (rand() % 1024 < (get_water(j, i) - 48)) {
          int x = (rand() % 3) - 1;
          
          int valid = 1;
          
          for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
              if (get_tile(j + x + dx, i + dy) == tile_algae_top) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + x + dx, i + dy) == tile_algae) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid && get_tile(j + x, i - 1) == tile_water && get_tile(j + x, i - 2) == tile_water && get_tile(j + x, i - 3) == tile_water && get_tile(j + x, i - 4) == tile_water) {
            if (rand() % 4 == 0) {
              set_tile(j + x, i - 1, tile_algae_top);
            } else {
              set_tile(j + x, i - 1, tile_algae);
            }
          }
        }
      } else if (get_tile(j, i) == tile_algae_top) {
        if (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_algae) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_algae) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_ash) {
        if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j, i + 1)]) {
          swap(j, i, j, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j - 1, i + 1)] && tile_light[get_tile(j + 1, i + 1)]) {
          if (rand() % 2) {
            swap(j, i, j - 1, i + 1);
          } else {
            swap(j, i, j + 1, i + 1);
          }
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j - 1, i + 1)]) {
          swap(j, i, j - 1, i + 1);
        } else if ((i < HEIGHT - 1 || WORLD_WRAP) && tile_light[get_tile(j + 1, i + 1)]) {
          swap(j, i, j + 1, i + 1);
        } else if (rand() % 2048 == 0 && get_tile(j, i - 1) == tile_air) {
          set_tile(j, i - 1, tile_fire);
        } else if (rand() % 8192 == 0) {
          set_tile(j, i, tile_fire);
        }
      } else if (get_tile(j, i) == tile_pine_tree) {
        int valid = 0;
        int t = 3;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_pine_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_pine_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_pine_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_pine_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 2)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_pine_leaves) {
            set_tile(j + x, i - 1, tile_pine_tree);
          }
        } else if (!valid && get_water(j, i) < 28 && get_water(j, i) > 0 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_pine_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_pine_leaves)) {
            set_tile(j + x, i, tile_pine_tree);
          }
        }
        
        valid = 1;
        
        for (int dx = -8; dx <= 8; dx++) {
          for (int dy = -8; dy <= 8; dy++) {
            if (get_tile(j + dx, i + dy) == tile_cone) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (valid && get_water(j, i) > 0 && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_cone);
        }
      } else if (get_tile(j, i) == tile_pine_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_pine_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_pine_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_cone) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_pine_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_insect) {
        int valid = 0;
        
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        if (rand() % 4 == 0) {
          for (int dx = -12; dx <= 12; dx++) {
            for (int dy = -12; dy <= 12; dy++) {
              if (get_tile(j + dx, i + dy) == tile_apple || get_tile(j + dx, i + dy) == tile_orange || get_tile(j + dx, i + dy) == tile_coconut || get_tile(j + dx, i + dy) == tile_cone) {
                valid = 1;
                
                x = dx;
                y = dy;
                
                break;
              }
            }
            
            if (valid) break;
          }
        }
        
        while (x > 1 || x < -1 || y > 1 || y < -1) {
          if (x < 0) x++;
          else x--;
          
          if (y < 0) y++;
          else y--;
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_apple) {
          set_tile(j, i, tile_insect_apple);
          set_tile(j + x, i + y, tile_air);
        } else if (get_tile(j + x, i + y) == tile_orange) {
          set_tile(j, i, tile_insect_orange);
          set_tile(j + x, i + y, tile_air);
        } else if (get_tile(j + x, i + y) == tile_coconut) {
          set_tile(j, i, tile_insect_coconut);
          set_tile(j + x, i + y, tile_air);
        } else if (get_tile(j + x, i + y) == tile_cone) {
          set_tile(j, i, tile_insect_cone);
          set_tile(j + x, i + y, tile_air);
        }
      } else if (get_tile(j, i) == tile_insect_apple) {
        int valid = 0;
        
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        if (rand() % 4 == 0) {
          for (int dx = -12; dx <= 12; dx++) {
            for (int dy = -12; dy <= 12; dy++) {
              if (get_tile(j + dx, i + dy) == tile_grass && get_tile(j + dx, i + dy + 1) == tile_dirt) {
                valid = 1;
                
                x = dx;
                y = dy;
                
                break;
              }
            }
            
            if (valid) break;
          }
        }
        
        while (x > 1 || x < -1 || y > 1 || y < -1) {
          if (x < 0) x++;
          else x--;
          
          if (y < 0) y++;
          else y--;
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_grass && get_tile(j + x, i + y + 1) == tile_dirt) {
          set_tile(j + x, i + y, tile_apple_tree);
          set_tile(j, i, tile_insect);
        }
      } else if (get_tile(j, i) == tile_insect_orange) {
        int valid = 0;
        
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        if (rand() % 4 == 0) {
          for (int dx = -12; dx <= 12; dx++) {
            for (int dy = -12; dy <= 12; dy++) {
              if (get_tile(j + dx, i + dy) == tile_grass && get_tile(j + dx, i + dy + 1) == tile_dirt) {
                valid = 1;
                
                x = dx;
                y = dy;
                
                break;
              }
            }
            
            if (valid) break;
          }
        }
        
        while (x > 1 || x < -1 || y > 1 || y < -1) {
          if (x < 0) x++;
          else x--;
          
          if (y < 0) y++;
          else y--;
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_grass && get_tile(j + x, i + y + 1) == tile_dirt) {
          set_tile(j + x, i + y, tile_orange_tree);
          set_tile(j, i, tile_insect);
        }
      } else if (get_tile(j, i) == tile_insect_coconut) {
        int valid = 0;
        
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        if (rand() % 4 == 0) {
          for (int dx = -12; dx <= 12; dx++) {
            for (int dy = -12; dy <= 12; dy++) {
              if (get_tile(j + dx, i + dy) == tile_air && get_tile(j + dx, i + dy + 1) == tile_sand && get_water(j + dx, i + dy + 1) > 4) {
                valid = 1;
                
                x = dx;
                y = dy;
                
                break;
              }
            }
            
            if (valid) break;
          }
        }
        
        while (x > 1 || x < -1 || y > 1 || y < -1) {
          if (x < 0) x++;
          else x--;
          
          if (y < 0) y++;
          else y--;
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_air && get_tile(j + x, i + y + 1) == tile_sand && get_water(j + x, i + y + 1) > 4) {
          set_tile(j + x, i + y, tile_palm_tree);
          set_tile(j, i, tile_insect);
        }
      } else if (get_tile(j, i) == tile_insect_cone) {
        int valid = 0;
        
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        if (rand() % 4 == 0) {
          for (int dx = -12; dx <= 12; dx++) {
            for (int dy = -12; dy <= 12; dy++) {
              if (get_tile(j + dx, i + dy) == tile_grass && get_tile(j + dx, i + dy + 1) == tile_dirt) {
                valid = 1;
                
                x = dx;
                y = dy;
                
                break;
              }
            }
            
            if (valid) break;
          }
        }
        
        while (x > 1 || x < -1 || y > 1 || y < -1) {
          if (x < 0) x++;
          else x--;
          
          if (y < 0) y++;
          else y--;
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_grass && get_tile(j + x, i + y + 1) == tile_dirt) {
          set_tile(j + x, i + y, tile_pine_tree);
          set_tile(j, i, tile_insect);
        }
      } else if (get_tile(j, i) == tile_fish) {
        if ((i < HEIGHT - 1 || WORLD_WRAP) && get_tile(j, i + 1) == tile_air) {
          swap(j, i, j, i + 1);
        } else if (get_tile(j - 1, i) == tile_water || get_tile(j + 1, i) == tile_water || get_tile(j, i - 1) == tile_water || get_tile(j, i + 1) == tile_water || get_water(j, i) >= 52) {
          int x = (rand() % 3) - 1;
          int y = 0;
          
          if (rand() % 4 == 0) {
            y = (rand() % 3) - 1;
          }
          
          if (get_tile(j + x, i + y) == tile_water) {
            swap(j, i, j + x, i + y);
          }
        } else {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_cacti) {
        if (rand() % 64 == 0 && (get_water(j, i) == 0 || (get_tile(j - 3, i + 1) != tile_cacti && get_tile(j - 2, i + 1) != tile_cacti && get_tile(j - 1, i + 1) != tile_cacti &&
            get_tile(j, i + 1) != tile_sand && get_tile(j, i + 1) != tile_cacti && get_tile(j + 1, i + 1) != tile_cacti && get_tile(j + 2, i + 1) != tile_cacti && get_tile(j + 3, i + 1) != tile_cacti))) {
          set_tile(j, i, tile_air);
        } else if (rand() % 1024 < get_water(j, i) + get_plant(j, i) && get_water(j, i) >= 20) {
          if (get_tile(j - 1, i) == tile_cacti && get_tile(j + 1, i) == tile_cacti) continue;
          if (get_tile(j - 1, i) != tile_cacti && get_tile(j - 2, i) == tile_cacti) continue;
          if (get_tile(j + 1, i) != tile_cacti && get_tile(j + 2, i) == tile_cacti) continue;
          if (get_tile(j, i - 1) == tile_cacti) continue;
          
          int valid = 1;
          
          for (int dy = -3; dy <= 0; dy++) {
            for (int dx = -3; dx <= 3; dx++) {
              if (dx == 0) continue;
              
              if (get_tile(j + dx, i + dy) == tile_cacti) {
                valid = 0;
              }
            }
          }
          
          if (valid) {
            if (rand() % 6 == 0 && get_water(j, i) < 32 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
              set_tile(j - 1, i, tile_cacti);
              set_tile(j + 1, i, tile_cacti);
            } else if (get_tile(j, i - 1) == tile_air) {
              set_tile(j, i - 1, (rand() % 8 == 0 && get_water(j, i) < 40) ? tile_cacti_flower : tile_cacti);
            }
          } else {
            if (rand() % 4 != 0 && get_water(j, i) < 32 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
              set_tile(j - 1, i, tile_cacti);
              set_tile(j + 1, i, tile_cacti);
            } else if (get_tile(j, i - 1) == tile_air) {
              set_tile(j, i - 1, (rand() % 8 == 0 && get_water(j, i) < 40) ? tile_cacti_flower : tile_cacti);
            }
          }
        }
      } else if (get_tile(j, i) == tile_cacti_flower) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_cacti) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_lemon_tree) {
        int valid = 0;
        int t = 5;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_lemon_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -2; dx <= 2; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_lemon_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_lemon_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_lemon_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 2)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_lemon_leaves) {
            set_tile(j + x, i - 1, tile_lemon_tree);
          }
        } else if (!valid && get_water(j, i) < 36 && get_water(j, i) > 0 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_lemon_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_lemon_leaves)) {
            set_tile(j + x, i, tile_lemon_tree);
          }
        }
        
        valid = 1;
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -3; dy <= 3; dy++) {
            if (get_tile(j + dx, i + dy) == tile_lemon) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (valid && get_water(j, i) > 0 && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_lemon);
        }
      } else if (get_tile(j, i) == tile_lemon_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_lemon_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_lemon_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_lemon) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_lemon_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_ebony_tree) {
        int valid = 0;
        int t = 4;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_ebony_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -2; dx <= 2; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_ebony_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_ebony_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_ebony_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 2)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_ebony_leaves) {
            set_tile(j + x, i - 1, tile_ebony_tree);
          }
        } else if (!valid && get_water(j, i) < 32 && get_water(j, i) > 0 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_ebony_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_ebony_leaves)) {
            set_tile(j + x, i, tile_ebony_tree);
          }
        }
        
        valid = 1;
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -3; dy <= 3; dy++) {
            if (get_tile(j + dx, i + dy) == tile_persimmon) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (valid && get_water(j, i) > 0 && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_persimmon);
        }
      } else if (get_tile(j, i) == tile_ebony_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_ebony_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_ebony_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_persimmon) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_ebony_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_birch_tree) {
        int valid = 0;
        int t = 4;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_birch_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_birch_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_birch_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_birch_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 2)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_birch_leaves) {
            set_tile(j + x, i - 1, tile_birch_tree);
          }
        } else if (!valid && get_water(j, i) < 28 && get_water(j, i) > 0 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_birch_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_birch_leaves)) {
            set_tile(j + x, i, tile_birch_tree);
          }
        }
      } else if (get_tile(j, i) == tile_birch_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_birch_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_birch_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_beech_tree) {
        int valid = 0;
        int t = 4;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_beech_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_beech_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 31) {
          for (int dx = -2; dx <= 2; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_beech_leaves && rand() % 12 == 0) {
              if (get_tile(j + dx - 1, i - 1) != tile_air || get_tile(j + dx + 1, i - 1) != tile_air || dx == 0) {
                set_tile(j + dx, i - 1, tile_beech_leaves);
              }
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 2)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_beech_leaves) {
            set_tile(j + x, i - 1, tile_beech_tree);
          }
        } else if (!valid && get_water(j, i) < 28 && get_water(j, i) > 0 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_beech_leaves) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_beech_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_beech_leaves)) {
            set_tile(j + x, i, tile_beech_tree);
          }
        }
      } else if (get_tile(j, i) == tile_beech_leaves) {
        int valid = 0;
          
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_beech_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0 || get_tile(j, i - 1) == tile_beech_leaves)) {
          set_tile(j, i, tile_air);
        }
      } else if (get_tile(j, i) == tile_willow_tree) {
        int valid = 0;
        int t = 4;
        
        for (int dx = -t; dx <= t; dx++) {
          for (int dy = 1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_dirt || get_tile(j + dx, i + dy) == tile_willow_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
          continue;
        }
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -2; dy <= -1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_willow_tree) {
              valid = 0;
              break;
            }
          }
          
          if (!valid) break;
        }
        
        if (get_water(j, i) > 0 && get_water(j, i) <= 39) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tile_willow_leaves && rand() % 12 == 0) {
              set_tile(j + dx, i - 1, tile_willow_leaves);
            }
          }
        }
        
        if (valid && rand() % 1024 < (get_water(j, i) - 2)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_willow_leaves) {
            set_tile(j + x, i - 1, tile_willow_tree);
          }
        } else if (!valid && get_water(j, i) < 30 && get_water(j, i) > 0 && rand() % 2048 < (55 - get_water(j, i))) {
          int x = (rand() % 2) * 2 - 1;
          
          if ((get_tile(j + x, i) == tile_air || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_grass || get_tile(j + x, i) == tile_water || get_tile(j + x, i) == tile_willow_leaves) &&
              (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_willow_leaves) &&
              (get_tile(j + x, i + 1) == tile_air || get_tile(j + x, i + 1) == tile_water || get_tile(j + x, i + 1) == tile_grass || get_tile(j + x, i + 1) == tile_willow_leaves)) {
            set_tile(j + x, i, tile_willow_tree);
          }
        }
      } else if (get_tile(j, i) == tile_willow_leaves) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 4; dy++) {
            if (get_tile(j + dx, i + dy) == tile_willow_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (rand() % 16 == 0 && (!valid || get_water(j, i) == 0)) {
          set_tile(j, i, tile_air);
        } else if (rand() % 32 == 0 && (get_tile(j, i + 1) == tile_air && get_water(j, i) >= 16)) {
          set_tile(j, i + 1, tile_willow_leaves);
        }
      }
    }
  }
}

void tick_water(void) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (get_tile_new(j, i) == tile_water) {
        set_water(j, i, 55);
      } else if (get_tile_new(j, i) == tile_air) {
        set_water(j, i, 0);
      } else {
        int water = get_water(j, i);
        
        if (water < 0)  set_water(j, i, 0),  water = 0;
        if (water > 55) set_water(j, i, 55), water = 55;
        
        int water_max = 0;
        
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (!dx && !dy) continue;
            
            if (get_water(j + dx, i + dy) - (dx < 0 ? -dx : dx) - (dy < 0 ? -dy : dy) > water_max && (!tile_limited[get_tile_new(j + dx, i + dy)] || get_tile_new(j + dx, i + dy) == get_tile_new(j, i))) {
              water_max = get_water(j + dx, i + dy) - (dx < 0 ? -dx : dx) - (dy < 0 ? -dy : dy);
            }
          }
        }
        
        if (water > water_max) {
          set_water(j, i, water - 1);
        } else if (water < water_max) {
          set_water(j, i, water + 1);
        }
      }
    }
  }
}

void tick_plant(void) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (get_tile_new(j, i) == tile_fertilizer) {
        set_plant(j, i, 39);
      } else if (get_tile_new(j, i) == tile_dirt || get_tile_new(j, i) == tile_sand || get_tile_new(j, i) == tile_grass) {
        int plant = get_plant(j, i);
        
        if (plant < 0)  set_plant(j, i, 0),  plant = 0;
        if (plant > 39) set_plant(j, i, 39), plant = 39;
        
        if (plant > 0 && get_plant(j - 1, i) <= plant && get_plant(j + 1, i) <= plant && get_plant(j, i - 1) <= plant && get_plant(j, i + 1) <= plant &&
            get_plant(j - 1, i - 1) <= plant && get_plant(j + 1, i - 1) <= plant && get_plant(j - 1, i + 1) <= plant && get_plant(j + 1, i + 1) <= plant) {
          set_plant(j, i, plant - 1);
        }
        
        if (plant < 39 && (get_plant(j - 1, i) > plant + 1 || get_plant(j + 1, i) > plant + 1 || get_plant(j, i - 1) > plant + 1 || get_plant(j, i + 1) > plant + 1)) {
          set_plant(j, i, plant + 1);
        }
        
        if (plant < 39 && (get_plant(j - 1, i - 1) > plant + 2 || get_plant(j + 1, i - 1) > plant + 2 || get_plant(j - 1, i + 1) > plant + 2 || get_plant(j + 1, i + 1) > plant + 2)) {
          set_plant(j, i, plant + 1);
        }
      } else {
        set_plant(j, i, 0);
      }
    }
  }
}

int main(int argc, const char **argv) {
  InitWindow(WIDTH * SCALE + 192, HEIGHT * SCALE, "tree");
  SetTargetFPS(30);
  
  old_tiles = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  old_water = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  old_plant = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  
  tree_tiles = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  tree_water = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  tree_plant = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  
  Texture2D background;
  
  if (BACKGROUND_IMAGE) {
    background = LoadTexture(BACKGROUND_IMAGE);
  }
  
  srand(time(0));
  noise_seed = rand();
  
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--seed")) {
      noise_seed = strtol(argv[++i], NULL, 0);
    }
  }
  
  if (WORLD_GEN) world_gen();
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    
    double time_0 = GetTime();
    
    int tile_x = off_x / zoom;
    int tile_y = off_y / zoom;
    
    if (BACKGROUND_IMAGE) {
      DrawTextureEx(background, (Vector2){0, 0}, 0.0f, (float)(HEIGHT * SCALE) / background.height, WHITE);
    } else {
      ClearBackground((Color){31, 31, 31, 255});
      DrawRectangle(-tile_x * zoom, -tile_y * zoom, zoom * WIDTH, zoom * HEIGHT, tile_colors[tile_air]);
    }
    
    double time_1 = GetTime();
    int draw_count = 0;
    
    for (int i = 0; i < HEIGHT; i++) {
      Color prev = BLACK;
      
      int length = 0;
      int start = 0;
      
      for (int j = 0; j < WIDTH; j++) {
        int tile = get_tile(j, i);
        
        if ((!BACKGROUND_IMAGE && tile == tile_air) || !BACKGROUND_ALPHA) {
          if (length) {
            DrawRectangle((start - tile_x) * zoom, (i - tile_y) * zoom, length * zoom, zoom, prev);
            draw_count++;
          }
          
          prev = BLACK;
          length = 0;
          start = j;
          
          continue;
        }
        
        Color color = get_color(tile);
        
        if (tile == tile_birch_tree && (((j ^ ((j * 0xED5AD4BB) << 2)) ^ ((i * 0x31848BAB) ^ (j >> 4))) >> 1) % 4 == 0) {
          color = (Color){15, 15, 15, ALPHA};
        }
        
        if (WET_BLOCKS && get_water(j, i) >= 20 && (tile == tile_sand || tile == tile_dirt || tile == tile_stone)) {
          color.r *= 0.9f;
          color.g *= 0.9f;
          color.b *= 0.9f;
        }
        
        if (WATER_FOAM && tile == tile_water && get_tile(j, i - 1) == tile_air && get_tile(j, i + 1) == tile_water) {
          color.r = (color.r + 255 + 255) / 3;
          color.g = (color.g + 255 + 255) / 3;
          color.b = (color.b + 255 + 255) / 3;
        }
        
        if (prev.r != color.r || prev.g != color.g || prev.b != color.b) {
          if (length) {
            DrawRectangle((start - tile_x) * zoom, (i - tile_y) * zoom, length * zoom, zoom, prev);
            draw_count++;
          }
          
          prev = color;
          length = 0;
          start = j;
        }
        
        length++;
      }
      
      if (length) {
        DrawRectangle((start - tile_x) * zoom, (i - tile_y) * zoom, length * zoom, zoom, prev);
        draw_count++;
      }
    }
    
    double time_2 = GetTime();
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) || IsKeyPressed(KEY_D)) {
      start_x = GetMouseX();
      start_y = GetMouseY();
      
      old_x = off_x;
      old_y = off_y;
    } else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsKeyDown(KEY_D)) {
      off_x = old_x - (GetMouseX() - start_x);
      off_y = old_y - (GetMouseY() - start_y);
    }
    
    if (IsKeyPressed(KEY_F)) {
      paused = 1 - paused;
    }
    
    int x = GetMouseX() / zoom;
    int y = GetMouseY() / zoom;
    
    for (int i = -40; i <= 40; i++) {
      for (int j = -40; j <= 40; j++) {
        if (i * i + j * j >= brush_size * brush_size) continue;
        DrawRectangle((j + x) * zoom, (i + y) * zoom, zoom, zoom, get_color(selection));
      }
    }
    
    x += tile_x;
    y += tile_y;
    
    if (IsKeyPressed(KEY_S) && zoom < 16) {
      zoom++;
    } else if (IsKeyReleased(KEY_A) && zoom > 1) {
      zoom--;
    }
    
    DrawRectangle(WIDTH * SCALE, 0, 192, HEIGHT * SCALE, BLACK);
    int sel_y = 0;
    
    for (int i = 1; i < tile_count; i++) {
      if (!tile_visible[i]) continue;
      
      if (selection == i) {
        DrawRectangle(WIDTH * SCALE + 10, sel_y * 34 + 10, 172, 36, WHITE);
      }
      
      Color color = get_color(i);
      int light = color.r * 0.2 + color.g * 0.7 + color.b * 0.1;
      
      color.a = 255;
      
      DrawRectangle(WIDTH * SCALE + 12, sel_y * 34 + 12, 168, 32, color);
      DrawText(tile_names[i], WIDTH * SCALE + 16, sel_y * 34 + 16, 20, light > 127 ? BLACK : WHITE);
      
      if (GetMouseX() >= WIDTH * SCALE + 12 && GetMouseY() >= sel_y * 34 + 12 && GetMouseX() < WIDTH * SCALE + 116 && GetMouseY() < sel_y * 34 + 34) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
          selection = i;
        }
      }
      
      sel_y++;
    }
    
    char buffer[256];
    
    if (GetMouseX() >= 0 && GetMouseY() >= 0 && GetMouseX() < WIDTH * SCALE && GetMouseY() < HEIGHT * SCALE) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        set_circle(x, y, selection);
      } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        set_circle(x, y, tile_air);
      }
      
      if (get_tile_new(x, y) != tile_air) {
        sprintf(buffer, "Tile: %s(moisture: %d/55, fertile: %d/39)", tile_names[get_tile_new(x, y)], get_water_new(x, y), get_plant_new(x, y));
        DrawText(buffer, 8, 32, 20, WHITE);
      } else {
        DrawText("Hover a tile to see information about it", 8, 32, 20, WHITE);
      }
    } else {
      DrawText("Hover a tile to see information about it", 8, 32, 20, WHITE);
    }
    
    sprintf(buffer, "Draw: %.1f%%", (100.0f * draw_count) / (WIDTH * HEIGHT));
    DrawText(buffer, 8, 56, 20, WHITE);
    
    sprintf(buffer, "Seed: %d", noise_seed);
    DrawText(buffer, 8, 80, 20, WHITE);
    
    if (GetMouseWheelMove() > 0) {
      brush_size += GetMouseWheelMove();
      if (brush_size > 40) brush_size = 40;
    } else if (GetMouseWheelMove() < 0) {
      brush_size += GetMouseWheelMove();
      if (brush_size < 1) brush_size = 1;
    }
    
    double time_3 = GetTime();
    double time_4 = time_3;
    
    if (!paused) {
      tick_water();
      tick_plant();
      
      time_4 = GetTime();
      
      for (int i = 0; i < 2; i++) {
        tick_tiles();
        
        memcpy(old_tiles, tree_tiles, WIDTH * HEIGHT * sizeof(int));
        memcpy(old_water, tree_water, WIDTH * HEIGHT * sizeof(int));
        memcpy(old_plant, tree_plant, WIDTH * HEIGHT * sizeof(int));
      }
    } else {
      memcpy(old_tiles, tree_tiles, WIDTH * HEIGHT * sizeof(int));
    }
    
    double time_5 = GetTime();
    
#if (DEBUG_MODE == 1)
    int length_1 = (int)((WIDTH * SCALE) * ((time_1 - time_0) / (time_5 - time_0)) * 0.5f);
    int length_2 = (int)((WIDTH * SCALE) * ((time_2 - time_1) / (time_5 - time_0)) * 0.5f);
    int length_3 = (int)((WIDTH * SCALE) * ((time_3 - time_2) / (time_5 - time_0)) * 0.5f);
    int length_4 = (int)((WIDTH * SCALE) * ((time_4 - time_3) / (time_5 - time_0)) * 0.5f);
    int length_5 = (int)((WIDTH * SCALE) * ((time_5 - time_4) / (time_5 - time_0)) * 0.5f);
    
    int debug_x = 0;
    
    DrawRectangle((WIDTH * SCALE) / 2 + debug_x, 0, length_1, 40, RED);
    debug_x += length_1;
    
    DrawRectangle((WIDTH * SCALE) / 2 + debug_x, 0, length_2, 40, YELLOW);
    debug_x += length_2;
    
    DrawRectangle((WIDTH * SCALE) / 2 + debug_x, 0, length_3, 40, GREEN);
    debug_x += length_3;
    
    DrawRectangle((WIDTH * SCALE) / 2 + debug_x, 0, length_4, 40, BLUE);
    debug_x += length_4;
    
    DrawRectangle((WIDTH * SCALE) / 2 + debug_x, 0, length_5, 40, PURPLE);
    debug_x += length_5;
#endif
    
    DrawFPS(8, 8);
    EndDrawing();
    
    tick++;
  }
  
  CloseWindow();
  return 0;
}
