#pragma once

#define PBASE           0xfe000000
#define PM_BASE         (PBASE + 0x100000)
#define PM_V3DRSTN      (1 << 6)
#define PM_PASSWORD     0x5a000000
#define PM_GRAFX        (PM_BASE + 0x10c)

#define ASB_BASE        (PBASE + 0xc11000)
#define ASB_V3D_M_CTRL  (ASB_BASE + 0x0c)
#define ASB_V3D_S_CTRL  (ASB_BASE + 0x08)

#define ASB_REQ_STOP    (1 << 0)
#define ASB_ACK         (1 << 1)
#define ASB_EMPTY       (1 << 2)
#define ASB_FULL        (1 << 3)
