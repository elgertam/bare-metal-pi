#pragma once

#include "common.h"
#include "peripherals/base.h"

typedef struct {
  reg32 res0[2];
  reg32 image;
  reg32 grafx;
  reg32 proc;
} pm_regs;

#define PM_V3DRSTN      (1 << 6)
#define PM_PASSWORD     0x5a000000

#define PM_OFFSET       0x100000
#define PM_BASE         (PBASE + PM_OFFSET)
#define REGS_PM         ((pm_regs*)PM_BASE)

typedef struct {
  reg32 res0[2];
  reg32 v3d_s_ctrl;
  reg32 v3d_m_ctrl;
  reg32 isp_s_ctrl;
  reg32 isp_m_ctrl;
  reg32 h264_s_ctrl;
  reg32 h265_m_ctrl;
  reg32 axi_brdg_id;
} asb_regs;

#define ASB_REQ_STOP    (1 << 0)
#define ASB_ACK         (1 << 1)
#define ASB_EMPTY       (1 << 2)
#define ASB_FULL        (1 << 3)

#define ASB_OFFSET      0xc11000
#define ASB_BASE        (PBASE + ASB_OFFSET)
#define REGS_ASB         ((asb_regs*)ASB_BASE)
