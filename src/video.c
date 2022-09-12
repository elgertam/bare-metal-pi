#include "mailbox.h"
#include "printf.h"
#include "timer.h"
#include "video.h"

typedef struct {
  mailbox_tag tag;
  u32 x_res;
  u32 y_res;
} mailbox_fb_size;

typedef struct {
  mailbox_tag tag;
  u32 bpp;
} mailbox_fb_depth;

typedef struct {
  mailbox_tag tag;
  u32 pitch;
} mailbox_fb_pitch;

typedef struct {
  mailbox_tag tag;
  u32 base;
  u32 screen_size;
} mailbox_fb_buffer;

typedef struct {
  mailbox_fb_size res;
  mailbox_fb_size vres;
  mailbox_fb_depth depth;
  mailbox_fb_buffer buf;
  mailbox_fb_pitch pitch;
} mailbox_fb_request;

static mailbox_fb_request fb_req;


void video_set_resolution(u32 x_res, u32 y_res, u32 bpp) {
  // mailbox_fb_request req;

  fb_req.res.tag.id = RFT_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
  fb_req.res.tag.buffer_size = 8;
  fb_req.res.tag.value_length = 8;
  fb_req.res.x_res = x_res;
  fb_req.res.y_res = y_res;

  fb_req.vres.tag.id = RFT_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
  fb_req.vres.tag.buffer_size = 8;
  fb_req.vres.tag.value_length = 8;
  fb_req.vres.x_res = x_res;
  fb_req.vres.y_res = y_res;

  fb_req.depth.tag.id = RFT_FRAMEBUFFER_SET_DEPTH;
  fb_req.depth.tag.buffer_size = 4;
  fb_req.depth.tag.value_length = 4;
  fb_req.depth.bpp = bpp;

  fb_req.buf.tag.id = RFT_FRAMEBUFFER_ALLOCATE;
  fb_req.buf.tag.buffer_size = 8;
  fb_req.buf.tag.value_length = 4;
  fb_req.buf.base = 16;
  fb_req.buf.screen_size = 0; //overwritten by response

  fb_req.pitch.tag.id = RFT_FRAMEBUFFER_GET_PITCH;
  fb_req.pitch.tag.buffer_size = 4;
  fb_req.pitch.tag.value_length = 4;
  fb_req.pitch.pitch = 0;

  mailbox_process((mailbox_tag*)&fb_req, sizeof(fb_req));

  printf("Allocated buffer:\t%X - %d\n", fb_req.buf.base, fb_req.buf.screen_size);

  timer_sleep(2000); // this is a long delay - can it be shorter and/or set via interrupt?

  // draw the resolution using bitmapped font on screen
  char res[64];
  sprintf(res, "Resolution: %d x %d\n", x_res, y_res);
  video_draw_string(res, 20, 20);

  //draw a blue box w/ a red box inside it
  u32 square_margin = 100;

  for (int y = 0; y < y_res; y++) {
    if (y == 40) {
      //redraw the "Resolution" string since it's been overwritten by the squares...
      video_draw_string(res, 20, 20);
    }

    for (int x = 0; x < x_res; x++) {
      //bool for should be draw the red square instead of the blue margin
      bool draw_square = (y > square_margin && y_res - y > square_margin) &&
        (x > square_margin && x_res - x > square_margin);

      video_draw_pixel(draw_square ? 0xAA0000FF : 0x0055BBFF, x, y);
    }
  }

  printf("DONE DRAWING\n");

  timer_sleep(2000);
}
void video_draw_pixel(u32 color, u32 x, u32 y) {
  reg8 * frame_buffer = (reg8 *)((fb_req.buf.base | 0x40000000) & ~0xC0000000);

  u32 pixel_offset = (x * (32 >> 3)) + (y * fb_req.pitch.pitch);

  if (fb_req.depth.bpp == 32) {
    u8 r = (color & RED) >> RED_SHIFT;
    u8 g = (color & GREEN) >> GREEN_SHIFT;
    u8 b = (color & BLUE) >> BLUE_SHIFT;
    u8 a = (color & ALPHA) >> ALPHA_SHIFT;

    frame_buffer[pixel_offset++] = b;
    frame_buffer[pixel_offset++] = g;
    frame_buffer[pixel_offset++] = r;
    frame_buffer[pixel_offset++] = a;
  } else {
    frame_buffer[pixel_offset++] = (color >> 8) & 0xff;
    frame_buffer[pixel_offset++] = color & 0xff;
  }
}

#define TEXT_COLOR 0xffffffff
#define BACK_COLOR 0x0055bbff

void video_draw_char(char c, u32 x_pos, u32 y_pos) {
  for (int y = 0; y < font_get_height(); y++){
    for (int x = 0; x < font_get_width(); x++){
      bool yes = font_get_pixel(c, x, y);
      video_draw_pixel(yes ? TEXT_COLOR : BACK_COLOR, x_pos + x, y_pos + y);
    }
  }
}

void video_draw_string(char * s, u32 x_pos, u32 y_pos) {
  int i = 0;
  int x_offset = 0;
  while (s[i] != '\0' && i < 64) {
    x_offset = i * font_get_width();
    video_draw_char(s[i], x_pos + x_offset, y_pos); // multi-line is much harder; use Dan's Nile
    i++;
  }
}
