#pragma once

#include "common.h"

typedef struct {
  u32 pos_x;
  u32 pos_y;
} point;

typedef enum {
  RED_SHIFT = 24,
  GREEN_SHIFT = 16,
  BLUE_SHIFT = 8,
  ALPHA_SHIFT = 0
} color_shift;

typedef enum {
  RED = 0xff << RED_SHIFT,
  GREEN = 0xff << GREEN_SHIFT,
  BLUE = 0xff << BLUE_SHIFT,
  ALPHA = 0xff << ALPHA_SHIFT
} color_channel;

void video_init();

void video_set_resolution(u32 x_res, u32 y_res, u32 bpp);
void video_draw_pixel(u32 color, u32 x_pos, u32 y_pos);
void video_draw_char(char c, u32 x_pos, u32 y_pos);
void video_draw_string(char * s, u32 x_pos, u32 y_pos);

void video_dma();
void video_set_dma(bool b);

u32 font_get_height();
u32 font_get_width();
bool font_get_pixel(char c, u32 x_offset, u32 y_offset);

// bool font_get_pixel(char c, point pos);
//void video_draw_char(char c, point pos);
//void video_draw_string(char * s, point pos);
