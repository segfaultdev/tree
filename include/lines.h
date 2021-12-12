#ifndef __LINES_H__
#define __LINES_H__

#include <stdint.h>
#include <stdio.h>

#define LINES_COUNT 256
#define LINES_STACK 32

#define LINES_WIDTH  128
#define LINES_HEIGHT 128

#define WIDTH  640
#define HEIGHT 400

#define SCALE 2

typedef struct line_t line_t;
typedef struct tile_t tile_t;
typedef struct sprt_t sprt_t;

struct line_t {
  int points[LINES_COUNT];
  int count;
  
  int quad_1, quad_2, quad_3, quad_4;
};

struct tile_t {
  int type;
  
  void (*exec)(tile_t *tile);
};

struct sprt_t {
  int x, y;
  int type;
};

enum {
  tile_air,
  tile_wall,
  tile_sign,
  
  tile_count
};

extern line_t *lines;
extern int line_count;
extern int line_total;

extern int line_stack[];
extern int line_offset;

extern int line_ticks;

extern FILE *cfg_file;

extern tile_t tiles[];

extern const char *arg0;

void draw_line(int x1, int y1, int x2, int y2);
void draw_rect(int x, int y, int width, int height);
void draw_curv(int x, int y, float scale, int start, int end);
void draw_char(int x, int y, float scale, char chr);
void draw_text(int x, int y, float scale, const char *str);
void draw_tile(int x, int y, float scale, int tile, int left, int right, int top, int down);
void draw_sprt(int x, int y, float scale, sprt_t *sprite);
void draw_zone(int x, int y, float scale, int off_x, int off_y, int width, int height);

void line_wait(int ticks);

void line_save(void);
void line_exit(void);

void line_push(void);
void line_pop(void);

void line_diag(const char *name, const char *text, int skip);

int  line_get(int x, int y);
void line_set(int x, int y, int tile);

void l_menu(void);            // main menu
void l_argh(const char *str); // error screen
void l_edit(void);            // editor
void l_play(void);            // game

#endif
