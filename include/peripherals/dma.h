#pragma once

#include "common.h"
#include "peripherals/base.h"

typedef struct {
  reg32 transfer_info;
  reg32 src_addr;
  reg32 dest_addr;
  reg32 xfer_length;
  reg32 stride;
  reg32 next_block_addr;  // this is a linked list, the bottom 32 of the addr
  reg32 res0[2];
} dma_control_block;

typedef struct {
  reg32 transfer_info;
  reg32 src_addr;
  reg32 dest_addr;
  reg32 xfer_length;
  reg32 res0;
  reg32 next_block_addr;
  reg32 res1[2];
} dma_lite_control_block;

typedef struct {
  reg32 transfer_info;
  reg32 src_addr;
  reg32 src_info;
  reg32 dest_addr;
  reg32 dest_info;
  reg32 xfer_length;
  reg32 next_block_addr;
  reg32 res0;
} dma4_control_block;

typedef struct {
  reg32 control;
  reg32 control_block_addr;
  dma_control_block block;
  reg32 debug;
  reg32 res[55];
} dma_channel_regs;

typedef struct {
  reg32 control;
  reg32 control_block_addr;
  dma_lite_control_block block;
  reg32 debug;
  reg32 res[55];
} dma_lite_channel_regs;

typedef struct {
  reg32 control;
  reg32 control_block_addr;
  dma4_control_block block;
  reg32 debug;
  reg32 res[55];
} dma4_channel_regs;

typedef struct {
  dma_channel_regs dma_regs[7];
  dma_lite_channel_regs dma_lite_regs[4];
  dma4_channel_regs dma4_regs[4];
} dma_channel_regs_all;

#define REGS_DMA(channel) ((dma_channel_regs *)((u64)((PBASE + 0x7000) + (channel * 0x100))))

#define REGS_DMA_INT_STATUS *((reg32 *)(PBASE + 0x7fe0))
#define REGS_DMA_ENABLE *((reg32 *)(PBASE + 0x7ff0))

#define DMA_CS_PANIC_PRIORITY_SHIFT   20
#define DMA_DEFAULT_PANIC_PRIORITY    0xf
#define DMA_CS_PRIORITY_SHIFT         16
#define DMA_DEFAULT_PRIORITY          1


typedef enum {
  DMA_CS_RESET                           = (1 << 31),
  DMA_CS_ABORT                           = (1 << 30),
  DMA_CS_DISDEBUG                        = (1 << 29),
  DMA_CS_WAIT_FOR_OUTSTANDING_WRITES     = (1 << 28),
  DMA_CS_PANIC_PRIORITY                  = (0xf << 20),
  DMA_CS_PRIORITY                        = (0xf << 16),
  DMA_CS_ERROR                           = (1 << 8),
  DMA_CS_WAITING_FOR_OUTSTANDING_WRITES  = (1 << 6),
  DMA_CS_DREQ_STOPS_DMA                  = (1 << 5),
  DMA_CS_PAUSED                          = (1 << 4),
  DMA_CS_DREQ                            = (1 << 3),
  DMA_CS_INT                             = (1 << 2),
  DMA_CS_END                             = (1 << 1),
  DMA_CS_ACTIVE                          = (1 << 0)
} dma_cs_fields;

typedef enum {
  DMA_XFER_LEN_YLENGTH = (0x3fff << 16),
  DMA_XFER_LEN_XLENGTH = (0xffff << 0)
} dma_xfer_len_fields;

typedef enum {
  DMA_STRIDE_DEST = (0xffff << 16),
  DMA_STRIDE_SRC  = (0xffff << 0)
} dma_stride_fields;

typedef enum {
  DMA_TI_NO_WIDE_BURSTS = (1 << 26),
  DMA_TI_WAITS          = (0x1f << 21),
  DMA_TI_PERMAP         = (0x1f << 16),
  DMA_TI_BURST_LENGTH   = (0xf << 12),
  DMA_TI_SRC_IGNORE     = (1 << 11),
  DMA_TI_SRC_DREQ       = (1 << 10),
  DMA_TI_SRC_WIDTH      = (1 << 9),
  DMA_TI_SRC_INC        = (1 << 8),
  DMA_TI_DEST_IGNORE    = (1 << 7),
  DMA_TI_DEST_DREQ      = (1 << 6),
  DMA_TI_DEST_WIDTH     = (1 << 5),
  DMA_TI_DEST_INC       = (1 << 4),
  DMA_TI_WAIT_RESP      = (1 << 3),
  DMA_TI_TDMODE         = (1 << 1),
  DMA_TI_INTEN          = (1 << 0)
} dma_ti_fields;
