#include <default.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

int *old_tiles = NULL;
int *old_water = NULL;

int *tree_tiles = NULL;
int *tree_water = NULL;

int selection = tile_dirt;
int brush_size = 5;

int off_x = 0;
int off_y = 0;

int start_x = 0;
int start_y = 0;

int old_x = 0;
int old_y = 0;

int zoom = SCALE;

void set_tile(int x, int y, int tile) {
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_tiles[x + y * WIDTH] = tile;
}

int get_tile(int x, int y) {
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return tile_air;
  return old_tiles[x + y * WIDTH];
}

int get_tile_new(int x, int y) {
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return tile_air;
  return tree_tiles[x + y * WIDTH];
}

void set_water(int x, int y, int water) {
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_water[x + y * WIDTH] = water;
}

int get_water(int x, int y) {
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return old_water[x + y * WIDTH];
}

int get_water_new(int x, int y) {
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return tree_water[x + y * WIDTH];
}

void swap(int x1, int y1, int x2, int y2) {
  if (x1 < 0 || y1 < 0 || x1 >= WIDTH || y1 >= HEIGHT) return;
  if (x2 < 0 || y2 < 0 || x2 >= WIDTH || y2 >= HEIGHT) return;
  
  int tile = get_tile_new(x1, y1);
  int water = get_water_new(x1, y1);
  
  set_tile(x1, y1, get_tile_new(x2, y2));
  set_water(x1, y1, get_water_new(x2, y2));
  
  set_tile(x2, y2, tile);
  set_water(x2, y2, water);
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
        if (i < HEIGHT - 1 && get_tile(j, i + 1) == tile_air) {
          swap(j, i, j, i + 1);
        } else if (i < HEIGHT - 1 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
          if (rand() % 2) {
            swap(j, i, j - 1, i + 1);
          } else {
            swap(j, i, j + 1, i + 1);
          }
        } else if (i < HEIGHT - 1 && get_tile(j - 1, i + 1) == tile_air) {
          swap(j, i, j - 1, i + 1);
        } else if (i < HEIGHT - 1 && get_tile(j + 1, i + 1) == tile_air) {
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
        if (i < HEIGHT - 1 && get_tile(j, i + 1) == tile_air || get_tile(j, i + 1) == tile_water) {
          swap(j, i, j, i + 1);
        } else if (rand() % 256 < get_water(j, i)) {
          if (i < HEIGHT - 1 && (get_tile(j - 1, i + 1) == tile_air || get_tile(j - 1, i + 1) == tile_water) && (get_tile(j + 1, i + 1) == tile_air || get_tile(j + 1, i + 1) == tile_water)) {
            if (rand() % 2) {
              swap(j, i, j - 1, i + 1);
            } else {
              swap(j, i, j + 1, i + 1);
            }
          } else if (i < HEIGHT - 1 && get_tile(j - 1, i + 1) == tile_air || get_tile(j - 1, i + 1) == tile_water) {
            swap(j, i, j - 1, i + 1);
          } else if (i < HEIGHT - 1 && get_tile(j + 1, i + 1) == tile_air || get_tile(j + 1, i + 1) == tile_water) {
            swap(j, i, j + 1, i + 1);
          }
        } else {
          if (rand() % 2048 < get_water(j, i) && get_tile(j, i - 1) == tile_air) {
            int type = rand() % 100;
            if (type >= 99) {
              set_tile(j, i - 1, tile_apple_tree);
            } else if (type >= 98) {
              set_tile(j, i - 1, tile_orange_tree);
            } else if (type >= 88) {
              set_tile(j, i - 1, tile_mushroom);
            } else if (type >= 50) {
              set_tile(j, i - 1, tile_berry_bush);
            } else {
              set_tile(j, i - 1, tile_grass);
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
        int t = 6;
        
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
        } else if (!valid && get_water(j, i) > 3 && rand() % 2048 < (47 - get_water(j, i))) {
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
        
        if (!valid || !(get_water(j, i) > 0 && get_water(j, i) <= 39) || get_tile(j, i - 1) == tile_apple_leaves) {
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
        int t = 6;
        
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
        
        if (valid && rand() % 1024 < (get_water(j, i) - 6)) {
          int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
          
          if (get_tile(j + x, i - 1) == tile_air || get_tile(j + x, i - 1) == tile_water || get_tile(j + x, i - 1) == tile_grass || get_tile(j + x, i - 1) == tile_orange_leaves) {
            set_tile(j + x, i - 1, tile_orange_tree);
          }
        } else if (!valid && get_water(j, i) > 3 && rand() % 2048 < (47 - get_water(j, i))) {
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
        
        if (!valid || !(get_water(j, i) > 0 && get_water(j, i) <= 39) || get_tile(j, i - 1) == tile_orange_leaves) {
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
        if (i < HEIGHT - 1 && get_tile(j, i + 1) == tile_air || get_tile(j, i + 1) == tile_water) {
          swap(j, i, j, i + 1);
        } else if (i < HEIGHT - 1 && (get_tile(j - 1, i + 1) == tile_air || get_tile(j - 1, i + 1) == tile_water) && (get_tile(j + 1, i + 1) == tile_air || get_tile(j + 1, i + 1) == tile_water)) {
          if (rand() % 2) {
            swap(j, i, j - 1, i + 1);
          } else {
            swap(j, i, j + 1, i + 1);
          }
        } else if (i < HEIGHT - 1 && get_tile(j - 1, i + 1) == tile_air || get_tile(j - 1, i + 1) == tile_water) {
          swap(j, i, j - 1, i + 1);
        } else if (i < HEIGHT - 1 && get_tile(j + 1, i + 1) == tile_air || get_tile(j + 1, i + 1) == tile_water) {
          swap(j, i, j + 1, i + 1);
        } else {
          if (rand() % 1024 < (get_water(j, i) - 24) && get_tile(j, i - 1) == tile_air) {
            int valid = 1;
            
            for (int dy = -12; dy <= 12; dy++) {
              for (int dx = -16; dx <= 16; dx++) {
                if (get_tile(j + dx, i + dy - 1) == tile_palm_tree || (!(dx < -2 || dy < -2 || dx > 2 || dy > 2) && get_tile(j + dx, i + dy) == tile_water)) {
                  valid = 0;
                  break;
                }
              }
              
              if (!valid) break;
            }
            
            if (valid) {
              set_tile(j, i - 1, tile_palm_tree);
            }
          }
        }
      } else if (get_tile(j, i) == tile_stone) {
        if (rand() % 65536 < (get_water(j, i) - 40)) {
          set_tile(j, i, tile_sand);
        } else if (rand() % 2048 < get_water(j, i)) {
          if (get_tile(j, i + 1) == tile_air) {
            set_tile(j, i + 1, tile_vines);
          }
        }
      } else if (get_tile(j, i) == tile_berry_bush) {
        if ((rand() % 64 == 0 && (get_water(j, i) < 24 || get_water(j, i) >= 32)) || (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_berry_bush &&
             get_tile(j, i - 1) != tile_red_berry && get_tile(j, i - 1) != tile_blue_berry)) {
          set_tile(j, i, tile_air);
        } else if (rand() % 256 < get_water(j, i) - 24 && get_water(j, i) < 32) {
          int x, y;
          
          switch (rand() % 8) {
            case 0:
              x = -1, y = 0;
              break;
            case 1:
              x = 1, y = 0;
              break;
            case 2:
              x = 0, y = -1;
              break;
            case 3:
              x = -1, y = -1;
              break;
            case 4:
              x = 1, y = -1;
              break;
            case 5:
              x = 0, y = 1;
              break;
            case 6:
              x = -1, y = 1;
              break;
            case 7:
              x = 1, y = 1;
              break;
          }
          
          if ((get_tile(j + x, i + y) == tile_air || get_tile(j + x, i + y) == tile_grass || get_tile(j + x, i + y) == tile_flower_pink ||
               get_tile(j + x, i + y) == tile_flower_blue || get_tile(j + x, i + y) == tile_flower_yellow) && (get_tile(j + x, i + y + 1) == tile_dirt ||
               get_tile(j + x, i + y + 1) == tile_berry_bush || get_tile(j + x, i + y + 1) == tile_red_berry || get_tile(j + x, i + y + 1) == tile_blue_berry ||
               get_tile(j + x, i + y + 1) == tile_grass)) {
            set_tile(j + x, i + y, tile_berry_bush);
          }
        } else if (rand() % 512 < get_water(j, i) - 24) {
          int valid = 1;
          int color = rand() % 2;
          
          for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
              if (get_tile(j + dx, i + dy) == tile_red_berry || get_tile(j + dx, i + dy) == tile_blue_berry) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid) {
            for (int dy = -5; dy <= 5; dy++) {
              for (int dx = -5; dx <= 5; dx++) {
                if (get_tile(j + dx, i + dy) == tile_red_berry) {
                  color = 0;
                } else if (get_tile(j + dx, i + dy) == tile_blue_berry) {
                  color = 1;
                }
              }
            }
            
            set_tile(j, i, color ? tile_blue_berry : tile_red_berry);
          }
        }
      } else if (get_tile(j, i) == tile_red_berry || get_tile(j, i) == tile_blue_berry) {
        int valid = 0;
          
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_berry_bush) {
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
            if (get_water(j, i) < 36 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
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
        } else if (get_water(j, i) >= 40) {
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
        } else if (!valid && get_water(j, i) < 16 && rand() % 2048 < (47 - get_water(j, i))) {
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
        
        if (!valid || !(get_water(j, i) > 0 && get_water(j, i) < 16) || get_tile(j, i - 1) == tile_palm_leaves) {
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
      }
    }
  }
}

void tick_water(void) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (get_tile_new(j, i) == tile_water) {
        set_water(j, i, 47);
      } else if (get_tile_new(j, i) == tile_dirt || get_tile_new(j, i) == tile_grass || get_tile_new(j, i) == tile_apple_tree || get_tile_new(j, i) == tile_apple_leaves ||
                 get_tile_new(j, i) == tile_orange_tree || get_tile_new(j, i) == tile_orange_leaves || get_tile_new(j, i) == tile_sand || get_tile_new(j, i) == tile_stone ||
                 get_tile_new(j, i) == tile_berry_bush || get_tile_new(j, i) == tile_red_berry || get_tile_new(j, i) == tile_blue_berry || get_tile_new(j, i) == tile_vines ||
                 get_tile_new(j, i) == tile_mushroom || get_tile_new(j, i) == tile_red_mushroom || get_tile_new(j, i) == tile_brown_mushroom || get_tile_new(j, i) == tile_palm_tree ||
                 get_tile_new(j, i) == tile_palm_leaves) {
        int water = get_water(j, i);
        
        if (water < 0)  set_water(j, i, 0),  water = 0;
        if (water > 47) set_water(j, i, 47), water = 47;
        
        if (water > 0 && get_water(j - 1, i) <= water && get_water(j + 1, i) <= water && get_water(j, i - 1) <= water && get_water(j, i + 1) <= water &&
            get_water(j - 1, i - 1) <= water && get_water(j + 1, i - 1) <= water && get_water(j - 1, i + 1) <= water && get_water(j + 1, i + 1) <= water) {
          set_water(j, i, water - 1);
        }
        
        if (water < 47 && (get_water(j - 1, i) > water + 1 || get_water(j + 1, i) > water + 1 || get_water(j, i - 1) > water + 1 || get_water(j, i + 1) > water + 1)) {
          set_water(j, i, water + 1);
        }
        
        if (water < 47 && (get_water(j - 1, i - 1) > water + 2 || get_water(j + 1, i - 1) > water + 2 || get_water(j - 1, i + 1) > water + 2 || get_water(j + 1, i + 1) > water + 2)) {
          set_water(j, i, water + 1);
        }
      } else {
        set_water(j, i, 0);
      }
    }
  }
}

int main(int argc, const char **argv) {
  InitWindow(WIDTH * SCALE + 192, HEIGHT * SCALE, "tree");
  SetTargetFPS(30);
  
  old_tiles = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  old_water = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  
  tree_tiles = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  tree_water = calloc(WIDTH * HEIGHT * sizeof(int), 1);
  
  Texture2D background;
  
  if (BACKGROUND_IMAGE) {
    background = LoadTexture(BACKGROUND_IMAGE);
  }
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    
    if (BACKGROUND_IMAGE) {
      DrawTextureEx(background, (Vector2){0, 0}, 0.0f, (float)(HEIGHT * SCALE) / background.height, WHITE);
    } else {
      ClearBackground((Color){31, 31, 31, 255});
    }
    
    int tile_x = off_x / zoom;
    int tile_y = off_y / zoom;
    
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        // if (j + tile_x < 0 || j + tile_x >= WIDTH || i + tile_y < 0 || i + tile_y >= HEIGHT) {
        //   continue;
        //}
        
        int tile = get_tile(j, i);
        Color color = get_color(tile);
        
        if (WET_BLOCKS && get_water(j, i) >= 24 && (tile == tile_sand || tile == tile_dirt || tile == tile_stone)) {
          color.r *= 0.8f;
          color.g *= 0.8f;
          color.b *= 0.8f;
        }
        
        DrawRectangle((j - tile_x) * zoom, (i - tile_y) * zoom, zoom, zoom, color);
      }
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
      start_x = GetMouseX();
      start_y = GetMouseY();
      
      old_x = off_x;
      old_y = off_y;
    } else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
      off_x = old_x - (GetMouseX() - start_x);
      off_y = old_y - (GetMouseY() - start_y);
    }
    
    if (IsKeyPressed(KEY_S) && zoom < 16) {
      zoom++;
    } else if (IsKeyReleased(KEY_A) && zoom > 1) {
      zoom--;
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
    
    DrawRectangle(WIDTH * SCALE, 0, 192, HEIGHT * SCALE, BLACK);
    int sel_y = 1;
    
    for (int i = 1; i < tile_count; i++) {
      if (!tile_visible[i]) continue;
      
      if (selection == i) {
        DrawRectangle(WIDTH * SCALE + 8, sel_y * 52 - 44, 176, 48, WHITE);
      }
      
      Color color = get_color(i);
      int light = color.r * 0.2 + color.g * 0.7 + color.b * 0.1;
      
      color.a = 255;
      
      DrawRectangle(WIDTH * SCALE + 12, sel_y * 52 - 40, 168, 40, color);
      DrawText(tile_names[i], WIDTH * SCALE + 16, sel_y * 52 - 36, 20, light > 127 ? BLACK : WHITE);
      
      if (GetMouseX() >= WIDTH * SCALE + 12 && GetMouseY() >= sel_y * 52 - 40 && GetMouseX() < WIDTH * SCALE + 116 && GetMouseY() < sel_y * 52) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
          selection = i;
        }
      }
      
      sel_y++;
    }
    
    if (GetMouseX() >= 0 && GetMouseY() >= 0 && GetMouseX() >= -(tile_x * zoom) && GetMouseY() >= -(tile_y * zoom) && GetMouseX() < WIDTH * SCALE &&
        GetMouseY() < HEIGHT * SCALE && GetMouseX() < WIDTH * SCALE - (tile_x * zoom) && GetMouseY() < HEIGHT * SCALE - (tile_y * zoom)) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        set_circle(x, y, selection);
      } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        set_circle(x, y, tile_air);
      }
    }
    
    if (GetMouseWheelMove() > 0) {
      brush_size += GetMouseWheelMove();
      if (brush_size > 40) brush_size = 40;
    } else if (GetMouseWheelMove() < 0) {
      brush_size += GetMouseWheelMove();
      if (brush_size < 1) brush_size = 1;
    }
    
    for (int i = 0; i < 2; i++) {
      tick_tiles();
      tick_water();
      
      memcpy(old_tiles, tree_tiles, WIDTH * HEIGHT * sizeof(int));
      memcpy(old_water, tree_water, WIDTH * HEIGHT * sizeof(int));
    }
    
    DrawFPS(8, 8);
    EndDrawing();
  }
  
  CloseWindow();
  return 0;
}
