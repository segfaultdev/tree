#define STBI_NO_STDIO
#define STBI_ONLY_PNG

#include <stb_image.h>
#include <default.h>
#include <pthread.h>
#include <perlin.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <tree.h>

#define GetMouseX() GetTouchPosition(0).x
#define GetMouseY() GetTouchPosition(0).y

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

uint32_t *old_tiles = NULL;
uint32_t *old_water = NULL;

uint32_t *tree_tiles = NULL;
uint32_t *tree_water = NULL;

uint32_t *empty_map = NULL;
uint32_t *polen_map = NULL;

int deep[65536];

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

int tick_speed = 2;

int screen_width = 0;
int screen_height = 0;

int total_width = 0;
int total_height = 0;

int view_width = 0;
int view_height = 0;

int active = 1;

int msleep(size_t ms) {
  struct timespec ts;
  int res;
  
  if (ms < 0) {
    errno = EINVAL;
    return -1;
  }
  
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  
  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);
  
  return res;
}

void set_tile(int x, int y, int tile) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_tiles[y + x * HEIGHT] = tile;
}

int get_tile(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return tile_air;
  return old_tiles[y + x * HEIGHT];
}

int get_tile_new(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return tile_air;
  return tree_tiles[y + x * HEIGHT];
}

void set_water(int x, int y, int water) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  tree_water[y + x * HEIGHT] = water;
}

int get_water(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return old_water[y + x * HEIGHT];
}

int get_water_new(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 0;
  return tree_water[y + x * HEIGHT];
}

void set_empty(int x, int y, int value) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  empty_map[x + y * WIDTH] = value;
}

void set_polen(int x, int y, int value) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  polen_map[x + y * WIDTH] = value;
}

int get_empty(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 1048576;
  return empty_map[x + y * WIDTH];
}

int get_polen(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return 1048576;
  return polen_map[x + y * WIDTH];
}

void del_tile(int x, int y) {
  if (WORLD_WRAP) {
    while (x < 0) x += WIDTH;
    while (y < 0) y += HEIGHT;
    
    x = x % WIDTH;
    y = y % HEIGHT;
  }
  
  if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) return;
  
  int valid = 0;
  
  for (int dy = -3; dy <= 3; dy++) {
    for (int dx = -3; dx <= 3; dx++) {
      if (get_tile_new(x + dx, y + dy) == tile_types[tree_tiles[x + y * WIDTH]].remove) {
        valid = 1;
        break;
      }
    }
    
    if (valid) break;
  }
  
  if (valid || tile_types[get_tile_new(x, y)].type < tile_type_ai_water) {
    set_tile(x, y, tile_types[get_tile_new(x, y)].remove);
  } else {
    set_tile(x, y, tile_air);
  }
}

void center_view(void) {
  off_x = (WIDTH * zoom - view_width) / 2;
  off_y = (HEIGHT * zoom - view_height) / 2;
}

#ifdef PLATFORM_WEB
#include <emscripten.h>

static void web_load_file(void) {
  EM_ASM(
    let file_selector = document.getElementById("file-input");
    file_selector.style.display = "block";
  );
  
  active = 0;
}

static void web_save_file(void) {
  EM_ASM({
    let date = new Date();
    let filename = "tree-";
    
    if (date.getDate() < 10) filename += "0";
    filename += date.getDate();
    
    if (date.getMonth() < 10) filename += "0";
    filename += date.getMonth();
    
    filename += date.getFullYear();
    filename += "-";
    
    if (date.getHours() < 10) filename += "0";
    filename += date.getHours();
    
    if (date.getMinutes() < 10) filename += "0";
    filename += date.getMinutes();
    
    if (date.getSeconds() < 10) filename += "0";
    filename += date.getSeconds();
    
    filename += ".png";
    console.log(filename);
    
    let buffer = new Uint8Array(Module.HEAPU8.buffer, $0, $1);
    
    let canvas = document.createElement("canvas");
    let ctx = canvas.getContext("2d");
    
    canvas.width = $2;
    canvas.height = $3;
    
    let image_data = ctx.createImageData($2, $3);
    image_data.data.set(buffer);
    
    for (let i = 3; i < $1; i += 4) {
      image_data.data[i] = 255;
    }
    
    ctx.putImageData(image_data, 0, 0);
    
    canvas.toBlob(function(blob) {
      saveAs(blob, filename);
    }, "image/png", 1.0);
  }, tree_tiles, WIDTH * HEIGHT * 4, HEIGHT, WIDTH);
}

EMSCRIPTEN_KEEPALIVE int web_file_loaded(uint8_t *buffer, size_t size) {
  active = 1;
  
  EM_ASM(
    let file_selector = document.getElementById("file-input");
    file_selector.style.display = "none";
  );
  
  memset(tree_tiles, 0, WIDTH * HEIGHT * 4);
  memset(tree_water, 0, WIDTH * HEIGHT * 4);
  
  memset(old_tiles, 0, WIDTH * HEIGHT * 4);
  memset(old_water, 0, WIDTH * HEIGHT * 4);
  
  center_view();
  
  int width, height, bpp;
  uint8_t *data = stbi_load_from_memory(buffer, size, &width, &height, &bpp, 4);
  
  for (int i = 3; i < WIDTH * HEIGHT * 4; i += 4) {
    data[i] = 0;
  }
  
  int size_1 = WIDTH * HEIGHT * 4;
  int size_2 = width * height * 4;
  
  memcpy(tree_tiles, data, MIN(size_1, size_2));
  
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int tile = get_tile(j, i);
      
      empty_map[j + i * WIDTH] = 1048576 * (tile != tile_flower_pink && tile != tile_flower_blue && tile != tile_flower_yellow);
      polen_map[j + i * WIDTH] = 1048576 * (tile != tile_hive);
    }
  }
  
  stbi_image_free(data);
  free(buffer);
  
  return 1;
}
#endif

void world_gen(void) {
  for (int i = 0; i < WIDTH; i++) {
    int height = (int)((noise_1(i / 59.7f) * 0.5f + noise_1(i / 61.3f) * 0.25f + noise_1(i / 55.1f) * 0.25f) * 192);
    int dirt_height = height - ((int)((noise_1(i / 49.7f) * 0.5f + noise_1(i / 51.3f) * 0.25f + noise_1(i / 45.1f) * 0.25f) * 192) + 64);
    
    for (int j = 0; j < HEIGHT; j++) {
      int elevation = (HEIGHT - j) - 0.4f * HEIGHT;
      
      if (elevation < height) {
        if (elevation < dirt_height) {
          if (noise_2(i / 57.0f, j / 53.0f) >= 0.8f) {
            set_tile(i, j, tile_dirt);
          } else {
            set_tile(i, j, tile_stone);
          }
        } else {
          if (noise_2(i / 47.0f, j / 43.0f) >= 0.7f) {
            set_tile(i, j, tile_stone);
          } else if (noise_2(i / 22.0f, j / 18.0f) >= lerp(0.85f, 0.95f, (float)(elevation - (height - 48)) / 48.0f)) {
            set_tile(i, j, tile_clay);
          } else if (noise_2(i / 32.0f, j / 28.0f) >= lerp(1.0f, 0.6f, (float)(elevation - (height - 64)) / 64.0f)) {
            set_tile(i, j, tile_sand);
          } else {
            set_tile(i, j, tile_dirt);
          }
        }
      } else if (elevation < 112) {
        set_tile(i, j, tile_water);
      }
      
      if (get_tile_new(i, j) != tile_stone && get_tile_new(i, j) != tile_clay) continue;
      
      if (noise_2(i / 15.9f, j / 16.1f) >= lerp(0.7f, 0.9f, (float)(elevation) / 160.0f)) {
        set_tile(i, j, tile_air);
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
  
  set_tile(x1, y1, get_tile_new(x2, y2));
  set_water(x1, y1, get_water_new(x2, y2));
  
  set_tile(x2, y2, tile);
  set_water(x2, y2, water);
}

void clear_empty(void) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      empty_map[j + i * WIDTH] = 1048576;
    }
  }
}

void clear_polen(void) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      polen_map[j + i * WIDTH] = 1048576;
    }
  }
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
  Color color = tile_types[tile].color_1;
  
  if (tile == tile_air) {
    color.a = BACKGROUND_ALPHA;
  } else {
    color.a = ALPHA;
  }
  
  return color;
}

void tick_tiles(void) {
  for (int i = 0; i < WIDTH; i++) {
    deep[i] = 0;
  }
  
  int x_start = off_x / zoom;
  if (x_start < 0) x_start = 0;
  
  int x_end = (off_x + view_width) / zoom;
  if (x_end > WIDTH) x_end = WIDTH;
  
  int x_length = x_end - x_start;
  
  for (int _j = 0; _j < x_length; _j++) {
    int j = x_start + 2 * (_j % (x_length / 2)) + (_j < (x_length / 2));
    
    for (int i = 0; i < HEIGHT; i++) {
      int tile = get_tile(j, i);
      
      if (tile < 0 || tile >= tile_count) {
        continue;
      }
      
      if ((tile_types[tile].type == tile_type_solid || tile_types[tile].type == tile_type_powder) && !tile_types[tile].weak) {
        deep[j] += 2;
      } else if (tile_types[tile].type == tile_type_liquid) {
        deep[j]++;
      }
      
      int need_dist = tile_types[tile].need_dist;
      
      int gas_x = (rand() % 3) - 1;
      
      int need_amnt = 0;
      int moved = 0;
      
      for (int dy = -need_dist; dy <= need_dist; dy++) {
        for (int dx = -need_dist; dx <= need_dist; dx++) {
          if (tile_types[tile].need_root == 1 && dy != 1) continue;
          if (tile_types[tile].need_root == 2 && dy != -1) continue;
          if (dx == 0 && dy == 0) continue;
          
          if (get_tile(j + dx, i + dy) == tile_types[tile].need_tile_1 || get_tile(j + dx, i + dy) == tile_types[tile].need_tile_2) {
            need_amnt++;
          }
        }
      }
      
      if (need_amnt < tile_types[tile].need_amnt) {
        del_tile(j, i);
        continue;
      }
      
      const int cond_up   = (i > 0 || WORLD_WRAP);
      const int cond_down = (i < HEIGHT - 1 || WORLD_WRAP);
      
      switch (tile_types[tile].type) {
        case tile_type_solid:
          if (rand() % 64 == 0 && get_water(j, i) < tile_types[tile].spread && rand() % 2 == 0) {
            del_tile(j, i);
          }
          
          break;
          
        case tile_type_powder:
          if (cond_down && tile_types[get_tile(j, i + 1)].sink) {
            swap(j, i, j, i + 1);
            moved = 1;
          } else if (rand() % 320 < get_water(j, i) - tile_types[tile].spread || tile_types[tile].spread < 0) {
            if (cond_down && tile_types[get_tile(j - 1, i + 1)].sink && tile_types[get_tile(j + 1, i + 1)].sink) {
              if (rand() % 2) {
                swap(j, i, j - 1, i + 1);
              } else {
                swap(j, i, j + 1, i + 1);
              }
              
              moved = 1;
            } else if (cond_down && tile_types[get_tile(j - 1, i + 1)].sink) {
              swap(j, i, j - 1, i + 1);
              moved = 1;
            } else if (cond_down && tile_types[get_tile(j + 1, i + 1)].sink) {
              swap(j, i, j + 1, i + 1);
              moved = 1;
            }
          }
          
          break;
          
        case tile_type_liquid:
          if (cond_down && tile_types[get_tile(j, i + 1)].very_weak) {
            swap(j, i, j, i + 1);
            moved = 1;
          } else if (rand() % 15 < tile_types[tile].spread) {
            if (cond_down && tile_types[get_tile(j - 1, i + 1)].very_weak && tile_types[get_tile(j + 1, i + 1)].very_weak) {
              if (rand() % 2) {
                swap(j, i, j - 1, i + 1);
              } else {
                swap(j, i, j + 1, i + 1);
              }
              
              moved = 1;
            } else if (cond_down && tile_types[get_tile(j - 1, i + 1)].very_weak) {
              swap(j, i, j - 1, i + 1);
              moved = 1;
            } else if (cond_down && tile_types[get_tile(j + 1, i + 1)].very_weak) {
              swap(j, i, j + 1, i + 1);
              moved = 1;
            } else {
              if (rand() % 2) {
                if (tile_types[get_tile(j - 1, i)].very_weak) {
                  swap(j, i, j - 1, i);
                  moved = 1;
                }
              } else {
                if (tile_types[get_tile(j + 1, i)].very_weak) {
                  swap(j, i, j + 1, i);
                  moved = 1;
                }
              }
            }
          }
          
          break;
          
        case tile_type_gas:
          if (rand() % 640 == 0) {
            del_tile(j, i);
            moved = 1;
          } else {
            if (cond_up && (get_tile(j + gas_x, i - 1) == tile_air || tile_types[get_tile(j + gas_x, i - 1)].type != tile_type_solid)) {
              swap(j, i, j + gas_x, i - 1);
              moved = 1;
            } else {
              if (rand() % 2) {
                if (get_tile(j - 1, i) == tile_air || tile_types[get_tile(j - 1, i)].type != tile_type_solid) {
                  swap(j, i, j - 1, i);
                  moved = 1;
                }
              } else {
                if (get_tile(j + 1, i) == tile_air || tile_types[get_tile(j + 1, i)].type != tile_type_solid) {
                  swap(j, i, j + 1, i);
                  moved = 1;
                }
              }
            }
          }
          
          break;
      }
      
      if (tile_types[tile].tree_type != -1) {
        int tree = tile_types[tile].tree_type;
        
        if (tile == tree_types[tree].tree_tile) {
          int valid = 1;
          
          for (int dx = -tree_types[tree].tree_dist; dx <= tree_types[tree].tree_dist; dx++) {
            for (int dy = -2; dy <= -1; dy++) {
              if (get_tile(j + dx, i + dy) == tree_types[tree].tree_tile) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (get_water(j, i) > 0 && get_water(j, i) <= tree_types[tree].leaf_grow) {
            for (int dx = -1; dx <= 1; dx++) {
              if (get_tile(j + dx, i - 1) == tile_air && get_tile(j + dx, i - 2) != tree_types[tree].leaf_tile && rand() % 12 == 0) {
                set_tile(j + dx, i - 1, tree_types[tree].leaf_tile);
              }
            }
          }
          
          if (valid && rand() % 1024 < (get_water(j, i) - tree_types[tree].upward_min)) {
            int x = (rand() % 4) ? 0 : ((rand() % 2) * 2 - 1);
            
            if (tile_types[get_tile(j + x, i - 1)].weak) {
              set_tile(j + x, i - 1, tree_types[tree].tree_tile);
            }
          } else if (!valid && get_water(j, i) <= tree_types[tree].branch_max && rand() % 1536 < (get_water(j, i) - tree_types[tree].branch_min)) {
            int x = (rand() % 2) * 2 - 1;
            
            
            
            if (tile_types[get_tile(j + x, i)].weak && tile_types[get_tile(j + x, i - 1)].weak && tile_types[get_tile(j + x, i + 1)].weak) {
              set_tile(j + x, i, tree_types[tree].tree_tile);
            }
          }
          
          if (tree_types[tree].fruit_tile_1 != -1) {
            int type_1 = 0;
            int type_2 = 0;
            
            for (int dx = -7; dx <= 7; dx++) {
              for (int dy = -7; dy <= 7; dy++) {
                if (get_tile(j + dx, i + dy) == tree_types[tree].fruit_tile_1) {
                  type_1++;
                }
                
                if (get_tile(j + dx, i + dy) == tree_types[tree].fruit_tile_2) {
                  type_2++;
                }
              }
            }
            
            int fruit_tile;
            
            if (type_1 == type_2) {
              fruit_tile = (rand() % 2) ? tree_types[tree].fruit_tile_1 : tree_types[tree].fruit_tile_2;
            } else if (type_1 > type_2) {
              fruit_tile = tree_types[tree].fruit_tile_1;
            } else {
              fruit_tile = tree_types[tree].fruit_tile_2;
            }
            
            valid = 1;
            
            for (int dx = -tree_types[tree].fruit_dist; dx <= tree_types[tree].fruit_dist; dx++) {
              for (int dy = -tree_types[tree].fruit_dist; dy <= tree_types[tree].fruit_dist; dy++) {
                if (get_tile(j + dx, i + dy) == fruit_tile) {
                  valid = 0;
                  break;
                }
              }
              
              if (!valid) break;
            }
            
            if (valid && get_water(j, i) > 0 && get_water(j, i) <= tree_types[tree].fruit_grow && get_tile(j, i + 1) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air) {
              set_tile(j, i + 1, fruit_tile);
            }
          }
        } else if (tile == tree_types[tree].leaf_tile && rand() % 64 == 0 && get_water(j, i) > tree_types[tree].leaf_grow) {
          del_tile(j, i);
        }
      }
      
      if (tile == tile_dirt) {
        if (rand() % 2048 < get_water(j, i) && get_water(j, i) > 0 && get_tile(j, i - 1) == tile_air) {
          set_tile(j, i - 1, tile_grass);
        } else if (rand() % 3072 < get_water(j, i) && get_tile(j, i - 1) == tile_water && get_tile(j, i - 2) == tile_water && get_tile(j, i - 3) == tile_water &&
                   get_tile(j, i - 4) == tile_water && get_tile(j, i - 5) == tile_water && get_tile(j, i - 6) == tile_water && get_tile(j, i - 7) == tile_water) {
          set_tile(j, i - 1, tile_algae);
        } else {
          int valid = 0;
          
          for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
              if (get_tile(j + dx, i + dy) == tile_soil) {
                valid = 1;
                break;
              }
            }
            
            if (valid) break;
          }
          
          if (valid && rand() % 12288 == 0 && get_water(j, i) >= 36) {
            set_tile(j, i, tile_soil);
          }
        }
      } else if (tile == tile_grass) {
        int valid = 0;
        
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (get_tile(j + dx, i + dy) == tile_air) {
              valid++;
            }
          }
        }
        
        valid = (valid >= 2);
        
        if (!valid && rand() % 8 == 0) {
          del_tile(j, i);
        } else if (valid && rand() % 48 < get_water(j, i) - 16) {
          int x = (rand() % 3) - 1;
          
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
          
          if (valid && tile_types[get_tile(j + x, i - 1)].weak) {
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
      } else if (tile == tile_fire) {
        if (rand() % 96 == 0) {
          del_tile(j, i);
          continue;
        }
        
        for (int dy = -7; dy <= 7; dy++) {
          for (int dx = -7; dx <= 7; dx++) {
            int ax = (dx < 0) ? -dx : dx;
            int ay = (dy < 0) ? -dy : dy;
            
            if ((!dx && !dy) || get_tile_new(j + dx, i + dy) == tile) {
              continue;
            }
            
            if (tile_types[get_tile(j + dx, i + dy)].type != tile_type_liquid) {
              if (ax > 1 || ay > 1) continue;
              
              if (dy < 0) {
                ay *= 6;
              } else {
                ax /= 2;
              }
            }
            
            int chance = 1 + (3 << ax) + (5 << ay);
            
            if (rand() % chance == 0) {
              set_tile(j + dx, i + dy, (rand() % 15 < tile_types[get_tile(j + dx, i + dy)].fire_amnt) ? tile_types[get_tile(j + dx, i + dy)].fire_tile : tile_fire);
            }
          }
        }
      } else if (tile == tile_ash) {
        if (rand() % 1024 == 0 && get_tile(j, i - 1) == tile_air) {
          set_tile(j, i - 1, tile_fire);
        } else if (rand() % 3072 == 0) {
          set_tile(j, i, tile_fire);
        }
      } else if (tile == tile_mushroom) {
        if (rand() % 256 < get_water(j, i)) {
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
          
          if (valid && tile_types[get_tile(j, i - 1)].weak) {
            if (get_water(j, i) < 48 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
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
      } else if (tile == tile_red_mushroom || tile == tile_brown_mushroom) {
        if ((rand() % 256 == 0 && get_water(j, i) < 8) || (get_tile(j, i + 1) != tile_air && get_tile(j, i + 1) != tile_mushroom && get_tile(j, i + 1) != tile_red_mushroom && get_tile(j, i + 1) != tile_brown_mushroom)) {
          set_tile(j, i, tile_air);
        } else if (get_water(j, i) >= 40 && get_water(j, i) < 44) {
          if (rand() % 8 == 0 && get_tile(j - 1, i) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j - 2, i) == tile_air) {
            set_tile(j - 1, i, tile);
          }
          
          if (rand() % 8 == 0 && get_tile(j + 1, i) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j + 2, i) == tile_air) {
            set_tile(j + 1, i, tile);
          }
          
          if (rand() % 32 == 0 && get_tile(j, i - 1) == tile_air) {
            set_tile(j, i - 1, tile);
          }
        }
      } else if (tile == tile_stone) {
        if (rand() % 16384 == 0 && get_water(j, i) >= 54) {
          set_tile(j, i, tile_sand);
        } else if (rand() % 2048 < get_water(j, i)) {
          if (get_tile(j, i + 1) == tile_air) {
            set_tile(j, i + 1, tile_vines);
          }
        } else if (rand() % 2048 < get_water(j, i) - 28 && get_water(j, i) > 0 && get_tile(j, i - 1) == tile_air && deep[j] >= 4) {
          set_tile(j, i - 1, tile_moss);
        }
      } else if (tile == tile_caveroom) {
        if (deep[j] < 4) {
          del_tile(j, i);
        } else if (rand() % 256 < get_water(j, i)) {
          int valid = 1;
          
          for (int dx = -5; dx <= 5; dx++) {
            for (int dy = -5; dy <= 5; dy++) {
              if (get_tile(j + dx, i + dy) == tile_red_caveroom || get_tile(j + dx, i + dy) == tile_brown_caveroom) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + dx, i + dy) == tile_caveroom) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid && tile_types[get_tile(j, i - 1)].weak) {
            if (get_water(j, i) < 48 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
              switch (rand() % 2) {
                case 0:
                  set_tile(j, i - 1, tile_red_caveroom);
                  break;
                case 1:
                  set_tile(j, i - 1, tile_brown_caveroom);
                  break;
              }
            } else {
              set_tile(j, i - 1, tile_caveroom);
            }
          }
        }
      } else if (tile == tile_red_caveroom || tile == tile_brown_caveroom) {
        if (deep[j] < 4 || (rand() % 256 == 0 && get_water(j, i) < 8) || (get_tile(j, i + 1) != tile_air && get_tile(j, i + 1) != tile_caveroom && get_tile(j, i + 1) != tile_red_caveroom && get_tile(j, i + 1) != tile_brown_caveroom)) {
          set_tile(j, i, tile_air);
        } else if (get_water(j, i) >= 40 && get_water(j, i) < 44) {
          if (rand() % 8 == 0 && get_tile(j - 1, i) == tile_air && get_tile(j - 1, i + 1) == tile_air && get_tile(j - 2, i) == tile_air) {
            set_tile(j - 1, i, tile);
          }
          
          if (rand() % 8 == 0 && get_tile(j + 1, i) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j + 2, i) == tile_air) {
            set_tile(j + 1, i, tile);
          }
          
          if (rand() % 32 == 0 && get_tile(j, i - 1) == tile_air) {
            set_tile(j, i - 1, tile);
          }
        }
      } else if (tile == tile_vines) {
        int water = (get_water(j, i) / 1.5f) - 12;
        
        if (get_tile(j - 1, i - 1) == tile_vines) {
          water -= 2;
        }
        
        if (get_tile(j + 1, i - 1) == tile_vines) {
          water -= 2;
        }
        
        if (get_tile(j - 1, i) == tile_vines) {
          water -= 10;
        }
        
        if (get_tile(j + 1, i) == tile_vines) {
          water -= 10;
        }
        
        if (rand() % 512 < water && get_tile(j, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_vines);
        }
      } else if (tile == tile_algae) {
        if (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_water && get_tile(j, i - 1) != tile_algae && get_tile(j, i - 1) != tile_algae_top) {
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
      } else if (tile == tile_cacti) {
        if (rand() % 1024 < get_water(j, i) && get_water(j, i) >= 20) {
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
          
          if (rand() % 3 == 0 && valid && get_water(j, i) < 48 && get_tile(j - 1, i + 1) == tile_air && get_tile(j + 1, i + 1) == tile_air && get_tile(j - 1, i + 2) == tile_air && get_tile(j + 1, i + 2) == tile_air) {
            set_tile(j - 1, i, tile_cacti);
            set_tile(j + 1, i, tile_cacti);
          } else if (get_tile(j, i - 1) == tile_air) {
            set_tile(j, i - 1, (rand() % 16 == 0 && get_water(j, i) < 40) ? tile_cacti_flower : tile_cacti);
          }
        }
      } else if (tile == tile_willow_leaves) {
        int water = get_water(j, i) - 8;
        
        if (get_tile(j - 1, i) == tile_vines) {
          water -= 16;
        }
        
        if (get_tile(j + 1, i) == tile_vines) {
          water -= 16;
        }
        
        int valid = 0;
        
        for (int dy = -8; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (get_tile(j + dx, i + dy + 1) == tile_willow_tree) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (valid && rand() % 512 < water && get_tile(j, i + 1) == tile_air) {
          set_tile(j, i + 1, tile_willow_leaves);
        }
      } else if (tile == tile_wheat) {
        if (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_wheat && get_tile(j, i - 1) != tile_wheat_top) {
          set_tile(j, i, tile_air);
        } else if (rand() % 1024 < (get_water(j, i) - 20)) {
          int x = (rand() % 2) ? ((rand() % 3) - 1) : 0;
          
          int valid = 1;
          
          for (int dx = -2; dx <= 2; dx++) {
            for (int dy = -2; dy <= 2; dy++) {
              if (get_tile(j + x + dx, i + dy) == tile_wheat_top) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + x + dx, i + dy) == tile_wheat) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid) {
            if (rand() % 3 == 0) {
              set_tile(j + x, i - 1, tile_wheat_top);
            } else {
              set_tile(j + x, i - 1, tile_wheat);
            }
          }
        }
      } else if (tile == tile_carrot) {
        if (get_tile(j, i - 1) != tile_air && get_tile(j, i - 1) != tile_carrot && get_tile(j, i - 1) != tile_carrot_leaves) {
          set_tile(j, i, tile_air);
        } else if (rand() % 1024 < (get_water(j, i) - 20)) {
          int valid = 1;
          
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              if (get_tile(j + dx, i + dy) == tile_carrot_leaves) {
                valid = 0;
                break;
              }
              
              if (dy < 0 && get_tile(j + dx, i + dy) == tile_carrot) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (valid) {
            if (rand() % 3 != 0) {
              set_tile(j, i - 1, tile_carrot_leaves);
            } else {
              set_tile(j, i - 1, tile_carrot);
            }
          }
        }
      } else if (tile == tile_snow) {
        int valid = 0;
        
        for (int dx = -3; dx <= 3; dx++) {
          for (int dy = -3; dy <= 3; dy++) {
            if (get_tile(j + dx, i + dy) == tile_ice) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          valid = 1;
          
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              if (get_tile(j + dx, i + dy) != tile_snow) {
                valid = 0;
                break;
              }
            }
            
            if (!valid) break;
          }
          
          if (rand() % 2048 == 0 && !valid) set_tile(j, i, tile_water);
        }
      } else if (tile == tile_ice) {
        int valid = 0;
        
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (!dx && !dy) continue;
            
            if (rand() % ((ABS(dx) > ABS(dy)) ? 800 : 6144) == 0 && get_tile(j + dx, i + dy) == tile_water) {
              set_tile(j + dx, i + dy, (rand() % 5) ? tile_ice : tile_snow);
            } else if (get_tile(j + dx, i + dy) == tile_steam) {
              set_tile(j + dx, i + dy, tile_water);
            }
          }
          
          if (valid) break;
        }
      } else if (tile == tile_hive) {
        int valid = 0;
        
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (!dx && !dy) continue;
            
            if (tile_types[get_tile(j + dx, i + dy)].tree_type >= 0) {
              valid = 1;
              break;
            }
          }
          
          if (valid) break;
        }
        
        if (!valid) {
          set_tile(j, i, tile_air);
        }
      } else if (tile == tile_empty_bee) {
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        int dist = 1048576;
        
        if (rand() % 3) {
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              int new_dist = get_empty(j + dx, i + dy);
              
              if (new_dist < dist) {
                x = dx;
                y = dy;
                
                dist = new_dist;
              }
            }
          }
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_flower_pink ||
                   get_tile(j + x, i + y) == tile_flower_blue || 
                   get_tile(j + x, i + y) == tile_flower_yellow) {
          set_tile(j + x, i + y, tile_polen_bee);
          set_tile(j, i, tile_air);
          
          clear_empty();
        }
      } else if (tile == tile_polen_bee) {
        int x = (rand() % 3) - 1;
        int y = (rand() % 3) - 1;
        
        int dist = 1048576;
        
        if (rand() % 3) {
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              int new_dist = get_polen(j + dx, i + dy);
              
              if (new_dist < dist) {
                x = dx;
                y = dy;
                
                dist = new_dist;
              }
            }
          }
        }
        
        if (x == 0 && y == 0) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
        }
        
        if (get_tile(j + x, i + y) == tile_air) {
          swap(j, i, j + x, i + y);
        } else if (get_tile(j + x, i + y) == tile_hive) {
          set_tile(j, i, tile_empty_bee);
          
          if (get_tile_new(j, i + 1) == tile_air) {
            set_tile(j, i + 1, tile_honey);
          } else if (get_tile_new(j, i - 1) == tile_air) {
            set_tile(j, i - 1, tile_honey);
          } else if (get_tile_new(j - 1, i) == tile_air) {
            set_tile(j - 1, i, tile_honey);
          } else if (get_tile_new(j + 1, i) == tile_air) {
            set_tile(j + 1, i, tile_honey);
          }
          
          clear_polen();
        }
      } else if (tile == tile_ant) {
        int x = 0;
        int y = 0;
        
        int valid = 0;
        
        int try_count = 20;
        int sub_tile = get_tile(j, i + 1);
        
        if (try_count && (tile_types[sub_tile].type == tile_type_gas || sub_tile == tile_air)) {
          swap(j, i, j, i + 1);
          try_count = 0;
        }
        
        sub_tile = get_tile(j, i - 1);
        
        if (try_count && (tile_types[sub_tile].type == tile_type_powder || tile_types[sub_tile].type == tile_type_liquid)) {
          swap(j, i, j, i - 1);
          try_count = 0;
        }
        
        for (int tries = 0; tries < try_count && !valid; tries++) {
          x = (rand() % 3) - 1;
          y = (rand() % 3) - 1;
          
          if (!(rand() % 8  == 0 && tries >= 10 && y >= 0 && get_tile(j + x, i + y) == tile_dirt) &&
              !(rand() % 64 == 0 && tries >= 10 && y == 0 && get_tile(j + x, i + y) == tile_clay) &&
              get_tile(j + x, i + y) != tile_air) {
            continue;
          }
          
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              if (!dx && !dy) continue;
              
              sub_tile = get_tile(j + x + dx, i + y + dy);
              
              if (sub_tile != tile_air && (tile_types[sub_tile].type == tile_type_solid || tile_types[sub_tile].type == tile_type_powder)) {
                valid = 1;
                break;
              }
            }
            
            if (valid) break;
          }
          
          if (valid) break;
        }
        
        if (valid) {
          if (get_tile(j + x, i + y) == tile_dirt || get_tile(j + x, i + y) == tile_clay) {
            for (int dx = -1; dx <= 1; dx++) {
              for (int dy = -1; dy <= 1; dy++) {
                sub_tile = get_tile(j + x + dx, i + y + dy);
                
                if (sub_tile == tile_dirt || sub_tile == tile_clay) {
                  if ((!dx && !dy) || (!(dx + x) && !(dy + y))) {
                    set_tile(j + x + dx, i + y + dy, tile_air);
                  } else if (dy < 0) {
                    set_tile(j + x + dx, i + y + dy, tile_clay);
                  }
                }
              }
            }
          }
          
          swap(j, i, j + x, i + y);
        }
      } else if (tile_types[tile].tree_type >= 0) {
        int tree_type = tile_types[tile].tree_type;
        
        if (tile != tree_types[tree_type].tree_tile &&
            tile != tree_types[tree_type].leaf_tile &&
            tile != tree_types[tree_type].fruit_tile_1 &&
            tile != tree_types[tree_type].fruit_tile_2) {
          if (get_tile(j, i + 1) == tile_types[tree_types[tree_type].tree_tile].need_tile_1 && rand() % 32 == 0) {
            set_tile(j, i, tree_types[tree_type].tree_tile);
          }
        }
      }
      
      if (tile_types[tile].type == tile_type_ai_water) {
        int x = (rand() % 3) - 1;
        int y = 0;
        
        if (rand() % 4 == 0) {
          y = (rand() % 3) - 1;
        }
        
        if (get_tile_new(j + x, i + y) == tile_water) {
          swap(j, i, j + x, i + y);
        }
      }
    }
  }
}

void tick_water(void) {
  int x_start = off_x / zoom - 56;
  if (x_start < 0) x_start = 0;
  
  for (int j = x_start; j < WIDTH; j++) {
    if (j >= (off_x + view_width) / zoom + 56) {
      break;
    }
    
    for (int i = 0; i < HEIGHT; i++) {
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
            
            if (get_water(j + dx, i + dy) - (dx < 0 ? -dx : dx) - (dy < 0 ? -dy : dy) > water_max && (!tile_types[get_tile_new(j + dx, i + dy)].flow || get_tile_new(j + dx, i + dy) == get_tile_new(j, i))) {
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

void tick_dists(int dir) {
  int x_start = off_x / zoom - 112;
  if (x_start < 0) x_start = 0;
  
  int x_end = (off_x + view_width) / zoom + 112;
  if (x_end > WIDTH) x_end = WIDTH;
  
  int x_length = x_end - x_start;
  
  for (int _j = x_start; _j < x_end; _j++) {
    int j = _j;
    if (dir) j = ((x_end - 1) - (_j - x_start)) + x_start;
    
    for (int _i = 0; _i < HEIGHT; _i++) {
      int i = _i;
      if (dir) i = (HEIGHT - 1) - _i;
      
      if (get_tile_new(j, i) == tile_flower_pink || get_tile_new(j, i) == tile_flower_blue || get_tile_new(j, i) == tile_flower_yellow) {
        set_empty(j, i, 0);
      } else if (get_tile_new(j, i) != tile_air) {
        set_empty(j, i, 1048576);
      } else {
        int dist = get_empty(j, i);
        int min_dist = 1048576;
        
        if (dist < 0)       set_empty(j, i, 0),       dist = 0;
        if (dist > 1048576) set_empty(j, i, 1048576), dist = 1048576;
        
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (!dx && !dy) continue;
            if (dx && dy) continue;
            
            if (get_empty(j + dx, i + dy) + 1 < min_dist) {
              min_dist = get_empty(j + dx, i + dy) + 1;
            }
          }
        }
        
        if (dist < min_dist) {
          set_empty(j, i, /* dist + 1 */ min_dist);
        } else if (dist > min_dist) {
          set_empty(j, i, min_dist);
        }
      }
    }
  }
  
  for (int _j = x_start; _j < x_end; _j++) {
    int j = _j;
    if (dir) j = ((x_end - 1) - (_j - x_start)) + x_start;
    
    for (int _i = 0; _i < HEIGHT; _i++) {
      int i = _i;
      if (dir) i = (HEIGHT - 1) - _i;
      
      if (get_tile_new(j, i) == tile_hive) {
        set_polen(j, i, 0);
      } else if (get_tile_new(j, i) != tile_air) {
        set_polen(j, i, 1048576);
      } else {
        int dist = get_polen(j, i);
        int min_dist = 1048576;
        
        if (dist < 0)       set_polen(j, i, 0),       dist = 0;
        if (dist > 1048576) set_polen(j, i, 1048576), dist = 1048576;
        
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (!dx && !dy) continue;
            if (dx && dy) continue;
            
            if (get_polen(j + dx, i + dy) + 1 < min_dist) {
              min_dist = get_polen(j + dx, i + dy) + 1;
            }
          }
        }
        
        if (dist < min_dist) {
          set_polen(j, i, /* dist + 1 */ min_dist);
        } else if (dist > min_dist) {
          set_polen(j, i, min_dist);
        }
      }
    }
  }
}

int tps = 0;

void *tick_loop(void *data) {
  int dir = 0;
  
  for (;;) {
    double time_start = GetTime();
    
    int copy_start = off_x / zoom;
    if (copy_start < 0) copy_start = 0;
    
    int copy_end = (off_x + view_width) / zoom;
    if (copy_end > WIDTH) copy_end = WIDTH;
    
    int copy_length = copy_end - copy_start;
    
    if (copy_start < WIDTH && copy_end > 0 && copy_length > 0 && tick_speed) {
      tick_water();
      
      tick_dists(dir);
      dir = 1 - dir;
      
      for (int i = 0; i < tick_speed; i++) {
        tick_tiles();
        
        memcpy(old_tiles + copy_start * HEIGHT, tree_tiles + copy_start * HEIGHT, copy_length * HEIGHT * 4);
        memcpy(old_water + copy_start * HEIGHT, tree_water + copy_start * HEIGHT, copy_length * HEIGHT * 4);
      }
    } else if (copy_start < WIDTH && copy_end > 0 && copy_length > 0) {
      memcpy(old_tiles + copy_start * HEIGHT, tree_tiles + copy_start * HEIGHT, copy_length * HEIGHT * 4);
    }
    
    double time_end = GetTime();
    
    if (time_end - time_start < 0.033) {
      msleep(1000 * (0.033 - (time_end - time_start)));
      time_end = GetTime();
    }
    
    tps = tick_speed / (time_end - time_start);
  }
}

void set_zoom(int new_zoom) {
  float mid_x = (float)(off_x + view_width / 2.0f) / (float)(zoom);
  float mid_y = (float)(off_y + view_height / 2.0f) / (float)(zoom);
  
  zoom = new_zoom;
  
  off_x = mid_x * zoom - view_width / 2.0f;
  off_y = mid_y * zoom - view_height / 2.0f;
  
  start_x = GetMouseX();
  start_y = GetMouseY();
  
  old_x = off_x;
  old_y = off_y;
}

int main(int argc, const char **argv) {
  srand(time(0));
  noise_seed = rand();
  
  screen_width = 1536;
  screen_height = 940;
  
  int force_screen = 1;
  int phone_ui = 1;
  
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--seed")) {
      noise_seed = strtol(argv[++i], NULL, 0);
    } else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--world")) {
      screen_width = strtol(argv[++i], NULL, 0);
      screen_height = strtol(argv[++i], NULL, 0);
      
      force_screen = 1;
    } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--phone")) {
      phone_ui = 1;
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--computer")) {
      phone_ui = 0;
    }
  }
  
  int rect_width = 144;
  int rect_count = (screen_width - 20) / rect_width;
  
  if (phone_ui) {
    if (rect_count < 3) rect_count = 3;
    rect_width = (screen_width - 20) / rect_count;
  }
  
  int view_count = 0;
  
  for (int i = 0; i < tile_count; i++) {
    if (tile_types[i].show != 1) continue;
    view_count++;
  }
  
  total_width = 20 + rect_width;
  total_height = 20 + 30 * ((view_count + (rect_count - 1)) / rect_count);
  
  view_width = screen_width;
  view_height = screen_height;
  
  if (phone_ui) {
    if (force_screen) {
      view_height -= total_height;
    } else {
      screen_height += total_height;
    }
  } else {
    if (force_screen) {
      view_width -= total_width;
    } else {
      screen_width += total_width;
    }
  }
  
  InitWindow(screen_width, screen_height, "tree");
  SetTargetFPS(30);
  
  old_tiles = calloc(WIDTH * HEIGHT * 4, 1);
  old_water = calloc(WIDTH * HEIGHT * 4, 1);
  
  tree_tiles = calloc(WIDTH * HEIGHT * 4, 1);
  tree_water = calloc(WIDTH * HEIGHT * 4, 1);
  
  empty_map = malloc(WIDTH * HEIGHT * 4);
  polen_map = malloc(WIDTH * HEIGHT * 4);
  
  Texture2D background;
  
  if (BACKGROUND_IMAGE) {
    background = LoadTexture(BACKGROUND_IMAGE);
  }
  
  if (WORLD_GEN) world_gen();
  center_view();
  
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int tile = get_tile(j, i);
      
      empty_map[j + i * WIDTH] = 1048576 * (tile != tile_flower_pink && tile != tile_flower_blue && tile != tile_flower_yellow);
      polen_map[j + i * WIDTH] = 1048576 * (tile != tile_hive);
    }
  }
  
  int drag_mode = 0;
  
  pthread_t thread;
  
  if (pthread_create(&thread, NULL, tick_loop, NULL)) {
    exit(1);
  }
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    
    double time_0 = GetTime();
    
    int tile_x = off_x / zoom;
    int tile_y = off_y / zoom;
    
    if (BACKGROUND_IMAGE) {
      DrawTextureEx(background, (Vector2){0, 0}, 0.0f, (float)(HEIGHT * SCALE) / background.height, WHITE);
    } else {
      ClearBackground((Color){31, 31, 31, 255});
      
      Color color = tile_types[tile_air].color_1;
      color.a = 255;
      
      DrawRectangle(-tile_x * zoom, -tile_y * zoom, zoom * WIDTH, zoom * HEIGHT, color);
    }
    
    if (!active) {
      EndDrawing();
      continue;
    }
    
    double time_1 = GetTime();
    int draw_count = 0;
    
    for (int i = 0; i < WIDTH; i++) {
      deep[i] = 0;
    }
    
    int y_start = off_y / zoom;
    if (y_start < 0) y_start = 0;
    
    for (int i = y_start; i < HEIGHT; i++) {
      if (i >= (off_y + view_height) / zoom) {
        break;
      }
      
      Color prev = BLACK;
      
      int length = 0;
      int start = 0;
      
      int x_start = off_x / zoom;
      if (x_start < 0) x_start = 0;
      
      for (int j = x_start; j < WIDTH; j++) {
        if (j >= (off_x + view_width) / zoom) {
          break;
        }
        
        int tile = get_tile(j, i);
        
        if ((tile_types[tile].type == tile_type_solid || tile_types[tile].type == tile_type_powder) && !tile_types[tile].weak) {
          deep[j] += 2;
        } else if (tile_types[tile].type == tile_type_liquid) {
          deep[j]++;
        }
        
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
        
        #if (TILE_EFFECTS)
          if (tile_types[tile].color_mode == tile_color_dots && (((j ^ ((j * 0xED5AD4BB) << 2)) ^ ((i * 0x31848BAB) ^ (j >> 4))) >> 1) % 4 == 0) {
            int alpha = color.a;
            
            color = tile_types[tile].color_2;
            color.a = alpha;
          }
          
          if (get_water(j, i) >= 20 && tile_types[tile].color_mode == tile_color_wet) {
            int alpha = color.a;
            
            color = tile_types[tile].color_2;
            color.a = alpha;
          }
          
          if (tile_types[tile].color_mode == tile_color_ceil && get_tile(j, i - 1) == tile_air && get_tile(j, i + 1) == tile) {
            int alpha = color.a;
            
            color = tile_types[tile].color_2;
            color.a = alpha;
          }
        #endif
        
        #if (DEEP_DARKEN)
          float mult = 1.0f + 0.00625f * deep[j];
          
          color.r /= mult;
          color.g /= mult;
          color.b /= mult;
        #endif
        
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
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) || (drag_mode && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
      start_x = GetMouseX();
      start_y = GetMouseY();
      
      old_x = off_x;
      old_y = off_y;
    } else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || (drag_mode && IsMouseButtonDown(MOUSE_BUTTON_LEFT))) {
      off_x = old_x - (GetMouseX() - start_x);
      off_y = old_y - (GetMouseY() - start_y);
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
    
    if (IsKeyPressed(KEY_S) && zoom < 32) {
      set_zoom(zoom + 1);
    } else if (IsKeyReleased(KEY_A) && zoom > 1) {
      set_zoom(zoom - 1);
    }
    
    int sel_pos = 0;
    
    if (phone_ui) {
      DrawRectangle(0, view_height, view_width, total_height, BLACK);
    } else {
      DrawRectangle(view_width, 0, total_width, view_height, BLACK);
    }
    
    for (int i = 0; i < tile_count; i++) {
      if (!tile_types[i].show || tile_types[i].show != 1) continue;
      
      int rect_x, rect_y;
      
      if (phone_ui) {
        rect_x = rect_width * (sel_pos % rect_count) + 10;
        rect_y = (sel_pos / rect_count) * 30 + 10 + view_height;
      } else {
        rect_x = WIDTH * SCALE + 10;
        rect_y = sel_pos * 30 + 10;
      }
      
      if (selection == i) {
        DrawRectangle(rect_x, rect_y, rect_width, 32, WHITE);
      }
      
      Color color = get_color(i);
      int light = color.r * 0.2 + color.g * 0.7 + color.b * 0.1;
      
      color.a = 255;
      
      DrawRectangle(rect_x + 2, rect_y + 2, rect_width - 4, 28, color);
      DrawText(tile_types[i].name_en, rect_x + 6, rect_y + 6, 20, light > 127 ? BLACK : WHITE);
      
      if (GetMouseX() >= rect_x + 2 && GetMouseY() >= rect_y + 2 && GetMouseX() < rect_x + rect_width - 2 && GetMouseY() < rect_y + 34) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          selection = i;
        }
      }
      
      sel_pos++;
    }
    
    char buffer[256];
    
    if (GetMouseX() >= 0 && GetMouseY() >= 0 && GetMouseX() < view_width && GetMouseY() < view_height) {
      if ((GetMouseX() < view_width - 44 && (GetMouseY() < 80)) || GetMouseY() >= 122 || GetMouseX() < view_width - 130) {
        if (!(GetMouseX() >= view_width - 314 && GetMouseX() < view_width - 166 && GetMouseY() < 116)) {
          if (!drag_mode) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
              set_circle(x, y, selection);
            } else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
              set_circle(x, y, tile_air);
            }
          }
        }
      }
      
      if (get_tile_new(x, y) != tile_air) {
        sprintf(buffer, "Tile: %s", tile_types[get_tile_new(x, y)].name_en);
        DrawText(buffer, 8, 32, 20, WHITE);
      } else {
        DrawText("Tile: Hover a tile", 8, 32, 20, WHITE);
      }
    } else {
      DrawText("Tile: Hover a tile", 8, 32, 20, WHITE);
    }
    
    sprintf(buffer, "Seed: %d", noise_seed);
    DrawText(buffer, 8, 56, 20, WHITE);
    
    DrawRectangle(view_width - 134, 0, 134, 122, (Color){0, 0, 0, 95});
    
    DrawRectangle(view_width - 42, 6, 36, 36, WHITE);
    DrawRectangle(view_width - 40, 8, 32, 32, BLACK);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 40 && GetMouseY() >= 8 && GetMouseX() < view_width - 6 && GetMouseY() < 40) {
      brush_size += 2;
      if (brush_size > 40) brush_size = 40;
    }
    
    DrawText("+", view_width - 34, 6, 40, WHITE);
    
    DrawRectangle(view_width - 42, 46, 36, 36, WHITE);
    DrawRectangle(view_width - 40, 48, 32, 32, BLACK);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 40 && GetMouseY() >= 48 && GetMouseX() < view_width - 6 && GetMouseY() < 80) {
      brush_size -= 2;
      if (brush_size < 1) brush_size = 1;
    }
    
    DrawText("-", view_width - 32, 46, 40, WHITE);
    
    DrawRectangle(view_width - 64, 86, 58, 30, WHITE);
    DrawRectangle(view_width - 62, 88, 54, 26, BLACK);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 64 && GetMouseY() >= 88 && GetMouseX() < view_width - 6 && GetMouseY() < 114) {
      #ifdef PLATFORM_WEB
        web_load_file();
      #endif
    }
    
    DrawText("Load", view_width - 58, 92, 20, WHITE);
    
    DrawRectangle(view_width - 128, 86, 60, 30, WHITE);
    DrawRectangle(view_width - 126, 88, 56, 26, BLACK);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 128 && GetMouseY() >= 88 && GetMouseX() < view_width - 68 && GetMouseY() < 114) {
      #ifdef PLATFORM_WEB
        web_save_file();
      #endif
    }
    
    DrawText("Save", view_width - 122, 92, 20, WHITE);
    
    for (int i = -40; i <= 40; i++) {
      for (int j = -40; j <= 40; j++) {
        if (i * i + j * j >= brush_size * brush_size) continue;
        DrawRectangle((view_width - 88) + j, 44 + i, 1, 1, get_color(selection));
      }
    }
    
    DrawRectangle(view_width - 314, 0, 148, 116, (Color){0, 0, 0, 95});
    
    DrawRectangle(view_width - 308, 6, 46, 36, WHITE);
    DrawRectangle(view_width - 306, 8, 42, 32, BLACK);
    
    DrawText("<<", view_width - 300, 6, 40, WHITE);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 308 && GetMouseX() < view_width - 272 && GetMouseY() >= 6 && GetMouseY() < 52) {
      tick_speed--;
      
      if (tick_speed < 0) {
        tick_speed = 0;
      }
    }
    
    DrawRectangle(view_width - 218, 6, 46, 36, WHITE);
    DrawRectangle(view_width - 216, 8, 42, 32, BLACK);
    
    DrawText(">>", view_width - 210, 6, 40, WHITE);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 218 && GetMouseX() < view_width - 182 && GetMouseY() >= 6 && GetMouseY() < 42) {
      tick_speed++;
      
      if (tick_speed > 16) {
        tick_speed = 16;
      }
    }
    
    char speed_buffer[4];
    sprintf(speed_buffer, "x%d", tick_speed);
    
    DrawText(speed_buffer, view_width - 252, 14, 20, WHITE);
    
    DrawRectangle(view_width - 308, 46, 136, 30, WHITE);
    DrawRectangle(view_width - 306, 48, 132, 26, BLACK);
    
    if (drag_mode) {
      DrawText("Drag: Yes", view_width - 300, 52, 20, WHITE);
    } else {
      DrawText("Drag: No", view_width - 300, 52, 20, WHITE);
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 308 && GetMouseX() < view_width - 182 && GetMouseY() >= 46 && GetMouseY() < 76) {
      drag_mode = !drag_mode;
      
      if (drag_mode) {
        start_x = GetMouseX();
        start_y = GetMouseY();
        
        old_x = off_x;
        old_y = off_y;
      }
    }
    
    DrawRectangle(view_width - 308, 80, 40, 30, WHITE);
    DrawRectangle(view_width - 306, 82, 36, 26, BLACK);
    
    DrawText("Z+", view_width - 300, 86, 20, WHITE);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 308 && GetMouseX() < view_width - 268 && GetMouseY() >= 80 && GetMouseY() < 110) {
      if (zoom < 32) set_zoom(zoom + 1);
    }
    
    DrawRectangle(view_width - 264, 80, 40, 30, WHITE);
    DrawRectangle(view_width - 262, 82, 36, 26, BLACK);
    
    DrawText("Z-", view_width - 256, 86, 20, WHITE);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 264 && GetMouseX() < view_width - 224 && GetMouseY() >= 80 && GetMouseY() < 110) {
      if (zoom > 1) set_zoom(zoom - 1);
    }
    
    DrawRectangle(view_width - 220, 80, 48, 30, WHITE);
    DrawRectangle(view_width - 218, 82, 44, 26, BLACK);
    
    DrawText("ZO", view_width - 209, 86, 20, WHITE);
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() >= view_width - 220 && GetMouseX() < view_width - 182 && GetMouseY() >= 80 && GetMouseY() < 110) {
      set_zoom(SCALE);
    }
    
    int wheel_move = GetMouseWheelMove();
    
    #ifdef PLATFORM_WEB
      wheel_move = -wheel_move;
    #endif
    
    if (wheel_move > 0) {
      brush_size += wheel_move;
      if (brush_size > 40) brush_size = 40;
    } else if (wheel_move < 0) {
      brush_size += wheel_move;
      if (brush_size < 1) brush_size = 1;
    }
    
    double time_3 = GetTime();
    double time_4 = time_3;
    
    double time_5 = GetTime();
    
#if (DEBUG_MODE)
    int length_1 = (int)((WIDTH * SCALE) * ((time_1 - time_0) / (time_5 - time_0)) * 0.5f); // draw background
    int length_2 = (int)((WIDTH * SCALE) * ((time_2 - time_1) / (time_5 - time_0)) * 0.5f); // draw world
    int length_3 = (int)((WIDTH * SCALE) * ((time_3 - time_2) / (time_5 - time_0)) * 0.5f); // draw interface
    int length_4 = (int)((WIDTH * SCALE) * ((time_4 - time_3) / (time_5 - time_0)) * 0.5f); // update water and distances
    int length_5 = (int)((WIDTH * SCALE) * ((time_5 - time_4) / (time_5 - time_0)) * 0.5f); // update tiles
    
    int debug_x = 0;
    
    DrawRectangle(view_width / 2 + debug_x, 0, length_1, 40, RED);
    debug_x += length_1;
    
    DrawRectangle(view_width / 2 + debug_x, 0, length_2, 40, YELLOW);
    debug_x += length_2;
    
    DrawRectangle(view_width / 2 + debug_x, 0, length_3, 40, GREEN);
    debug_x += length_3;
    
    DrawRectangle(view_width / 2 + debug_x, 0, length_4, 40, BLUE);
    debug_x += length_4;
    
    DrawRectangle(view_width / 2 + debug_x, 0, length_5, 40, PURPLE);
    debug_x += length_5;
#endif
    
    char fps_tps_buffer[32];
    sprintf(fps_tps_buffer, "%d FPS, %d TPS", GetFPS(), tps);
    
    DrawText(fps_tps_buffer, 8, 8, 20, WHITE);
    
    EndDrawing();
    
    #ifndef PLATFORM_WEB
      if (IsKeyPressed(KEY_E)) TakeScreenshot("screen.png");
    #endif
    
    tick++;
  }
  
  CloseWindow();
  return 0;
}
