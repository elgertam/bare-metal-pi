#include "dma.h"
#include "mem.h"
#include "mm.h"
#include "timer.h"
#include "printf.h"

dma_channel channels[15];

static u16 channel_map = 0x1f35;

static i16 allocate_channel(u32 channel) {
  if (!(channel & ~0x0f)) {
    if (channel_map & (1 << channel)) {
      channel_map &= ~(1 << channel);
      return channel;
    }

    return CT_NONE;
  }

  u16 i = channel == CT_NORMAL ? 6 : 12;

  for (; i >= 0; i--){
    if (channel_map & (1 << i)) {
      channel_map &= ~(1 << i);
      return i;
    }
  }

  return CT_NONE;
}


dma_channel * dma_open_channel(u32 channel) {
  i16 _channel = allocate_channel(channel);

  if (_channel == CT_NONE) {
    printf("Invalid DMA channel! %d\n", _channel);
    return 0;
  }

  dma_channel * dma = (dma_channel *)&channels[_channel];
  dma->channel = _channel;

  // Blocks in memory must be 32-byte aligned
  dma->block = (dma_control_block *)((LOW_MEMORY + 31) & ~31);
  dma->block->res0[0] = 0;
  dma->block->res0[1] = 0;

  dma_channel_regs * dcr = REGS_DMA(dma->channel);

  REGS_DMA_ENABLE |= (1 << dma->channel);
  timer_sleep(3);
  dcr->control |= DMA_CS_RESET;

  while(dcr->control & DMA_CS_RESET) ;

  return dma;
}


void dma_close_channel(dma_channel *channel) {
  channel_map |= (1 << channel->channel);
}

void dma_setup_mem_copy(dma_channel *channel, void * dest, void * src, u32 length, u32 burst_length) {
  channel->block->transfer_info = (burst_length << 12)
                                | DMA_TI_SRC_WIDTH
                                | DMA_TI_SRC_INC
                                | DMA_TI_DEST_WIDTH
                                | DMA_TI_DEST_INC
                                ;

  channel->block->src_addr = SAFE_TRUNCATE(src);
  channel->block->dest_addr = SAFE_TRUNCATE(dest);
  channel->block->xfer_length = length;
  channel->block->stride = 0;
  channel->block->next_block_addr = 0;
}

void dma_start(dma_channel *channel) {
  dma_channel_regs * dcr = REGS_DMA(channel->channel);

  dcr->control_block_addr = BUS_ADDRESS(SAFE_TRUNCATE(channel->block));
  dcr->control = DMA_CS_WAIT_FOR_OUTSTANDING_WRITES
               | DMA_CS_PANIC_PRIORITY
               | (DMA_DEFAULT_PRIORITY << DMA_CS_PRIORITY_SHIFT)
               | DMA_CS_ACTIVE
               ;
}

bool dma_wait(dma_channel *channel) {
  dma_channel_regs * dcr = REGS_DMA(channel->channel);

  while(dcr->control & DMA_CS_ACTIVE) ;

  channel->status = REGS_DMA(channel->channel)->control & DMA_CS_ERROR ? false : true;

  return channel->status;
}
