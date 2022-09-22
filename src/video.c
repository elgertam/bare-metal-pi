#include "mm.h"
#include "mem.h"
#include "dma.h"
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
  u32 pitch; } mailbox_fb_pitch;
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

static dma_channel *dma;
static u8 *vid_buffer;

static u32 *bg32_buffer;
static u32 *bg8_buffer;

#define TEXT_COLOR 0xffffffff
#define BACK_COLOR 0xff0055bb

#define MB (1024 * 1024)

#define BG32_MEM_LOCATION   (LOW_MEMORY + (10*MB))
#define BG8_MEM_LOCATION    (BG32_MEM_LOCATION + (10*MB))
#define VB_MEM_LOCATION     (BG8_MEM_LOCATION + (4*MB))

void video_init() {
  dma = dma_open_channel(CT_NORMAL);
  vid_buffer = (u8 *)VB_MEM_LOCATION;

  printf("DMA CHANNEL:\t%d\n", dma->channel);
  printf("VID BUFF:\t0x%X\n", vid_buffer);

  bg32_buffer = (u32 *)BG32_MEM_LOCATION;
  bg8_buffer = (u32 *)BG8_MEM_LOCATION;

  int i;

  for (i = 0; i < (10*MB) / 4; i++) {
    bg32_buffer[i] = BACK_COLOR;
  }

  for (i = 0; i < (4*MB) / 4; i++) {
    bg8_buffer[i] = 0x01010101;
  }
}

static bool use_dma = false;

#define BUS_ADDR(x)   (((u64)x | GPU_CACHED_BASE) & ~GPU_UNCACHED_BASE)

#define FRAMEBUFFER   ((reg8 *)BUS_ADDR(fb_req.buf.base))
#define DMABUFFER     ((reg8 *)vid_buffer)
#define DRAW_BUFFER   (use_dma ? DMABUFFER : FRAMEBUFFER)

void video_set_dma(bool b) {
  use_dma = b;
}

void do_dma(void * dest, void * src, u32 total) {
  u32 start = 0;
  while (total > 0) {
    int num_bytes = total;

    if (num_bytes > 0xffffff) {
      num_bytes = 0xffffff;
    }

    dma_setup_mem_copy(dma, dest + start, src + start, num_bytes, 2);

    dma_start(dma);

    dma_wait(dma);

    start += num_bytes;
    total -= num_bytes;
  }
}

void video_dma() {
  do_dma((void *)FRAMEBUFFER, (void *)DMABUFFER, fb_req.buf.screen_size);
}

typedef struct {
  mailbox_tag tag;
  u32 offset;
  u32 num_entries;
  u32 entries[8];
} mailbox_set_palette;

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

  mailbox_set_palette palette;
  palette.tag.id = RFT_FRAMEBUFFER_SET_PALETTE;
  palette.tag.buffer_size = 40;
  palette.tag.value_length = 0;
  palette.offset = 0;
  palette.num_entries = 8;
  palette.entries[0] = 0;
  palette.entries[1] = 0xffbb5500;
  palette.entries[2] = 0xffffffff;
  palette.entries[3] = 0xff00ff00;
  palette.entries[5] = 0xff0000ff;
  palette.entries[6] = 0x55555555;
  palette.entries[7] = 0xcccccccc;

  mailbox_process((mailbox_tag*)&fb_req, sizeof(fb_req));

  printf("Allocated buffer:\t%X - %d\n", fb_req.buf.base, fb_req.buf.screen_size);

  if (bpp == 8) {
    mailbox_process((mailbox_tag *)&palette, sizeof(palette));
  }

  timer_sleep(2000); // this is a long delay - can it be shorter and/or set via interrupt?

  // draw the resolution using bitmapped font on screen
  char res[64];

  if (fb_req.depth.bpp == 32) {
    if (!use_dma){
      u32 * buf = (u32*)FRAMEBUFFER;
      for (int i = 0; i < fb_req.buf.screen_size / 4; i++) {
        buf[i] = bg32_buffer[i];
      }
    } else {
      do_dma((void *)BUS_ADDR(vid_buffer), (void *)bg32_buffer, fb_req.buf.screen_size);
    }
  } else if (fb_req.depth.bpp == 8) {
    if (!use_dma){
      u32 * buf = (u32*)FRAMEBUFFER;
      for (int i = 0; i < fb_req.buf.screen_size / 4; i++) {
        buf[i] = bg8_buffer[i];
      }
    } else {
      do_dma((void *)BUS_ADDR(vid_buffer), (void *)bg8_buffer, fb_req.buf.screen_size);
    }
  } else {
    printf("Cannot handle %d bits per pixel.\n", fb_req.depth.bpp);
  }

  if (use_dma){
    video_dma();
  }

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

      // video_draw_pixel(draw_square ? 0xAA0000FF : 0x0055BBFF, x, y);
      video_draw_pixel(draw_square ? 0xFFAA0000 : 0xFF0055BB, x, y);
    }
  }

  if (use_dma){
    video_dma();
  }

  printf("DONE DRAWING\n");

  timer_sleep(2000);
}

void video_draw_pixel(u32 color, u32 x, u32 y) {
  // reg8 * frame_buffer = (reg8 *)((fb_req.buf.base | 0x40000000) & ~0xC0000000);

  u8 bpp = fb_req.depth.bpp;
  u8 span = bpp >> 3;
  u32 pixel_offset = (x * span) + (y * fb_req.pitch.pitch);

  if (bpp == 32) {
    reg32 * buf = (reg32 *)DRAW_BUFFER;
    buf[pixel_offset / span] = (color & ((u32)(-1)));
  } else if (bpp == 16) {
    reg16 * buf = (reg16 *)DRAW_BUFFER;
    buf[pixel_offset / span] = (color & ((u16)(-1)));
  } else {
    reg8 * buf = (reg8 *)DRAW_BUFFER;
    buf[pixel_offset / span] = (color & ((u8)(-1)));
  }
}

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
