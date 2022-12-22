#pragma once

#include "common.h"
#include "peripherals/base.h"

#define V3D_OFFSET            0xc00000
#define V3D_BASE              (PBASE + V3D_OFFSET)

#define REGS_V3D_HUB          ((reg32 *)(V3D_BASE))

typedef struct {
  reg32 axi_cfg;
  reg32 uif_cfg;
  reg32 ident[4];
} v3d_hub_ident_regs;

#define VHI_NHOSTS_SHIFT      12
#define VHI_NHOSTS_MASK       (0xf << VHI_NHOSTS_SHIFT)
#define VHI_NCORES_SHIFT      8
#define VHI_NCORES_MASK       (0xf << VHI_NCORES_SHIFT)

#define V3D_HUB_OFFSET        0x0
#define REGS_V3D_HUB_IDENT    ((v3d_hub_ident_regs*)(V3D_BASE))

typedef struct {
  reg32 sts;
  reg32 set;
  reg32 clr;
  reg32 msk_sts;
  reg32 msk_set;
  reg32 msk_clr;
} v3d_hub_int_regs;

#define V3D_HUB_INT_OFFSET    0x50
#define REGS_V3D_HUB_INT      ((v3d_hub_int_regs*)(V3D_BASE + V3D_HUB_INT_OFFSET))

typedef struct {
  reg32 cs;
  reg32 su;
  reg32 icfg;
  reg32 iia;                  /* Input Image Address */
  reg32 ica;                  /* Input Chroma Address */
  reg32 iis;                  /* Input Image Stride */
  reg32 iua;                  /* Input Image U-Plane Address */
  reg32 ioa;                  /* Output Image Address */
  reg32 ios;                  /* Image Output Size */
  reg32 coef[4];              /* TFU YUV Coefficients */
  reg32 crc;
} v3d_tfu_regs;

#define V3D_TFU_OFFSET        0x400
#define REGS_V3D_TFU          ((v3d_tfu_regs*)(V3D_BASE + V3D_TFU_OFFSET))

#define V3D_MMUC_OFFSET       0x1000
#define REGS_V3D_MMUC_CONTROL ((reg32*)(V3D_BASE + V3D_MMUC_OFFSET))

typedef struct {
  reg32 ctl;
  reg32 pt_pa_base;
  reg32 hit;
  reg32 misses;
  reg32 stalls;
  reg32 addr_cap;
  reg32 shoot_down;
  reg32 bypass_start;
  reg32 bypass_end;
  reg32 vio_id;               /* AXI ID of the access that faulted */
  reg32 illegal_addr;         /* Address for illegal PTEs to return */
  reg32 vio_addr;
  reg32 debugt_info;
} v3d_mmu_regs;

#define V3D_MMU_OFFSET        0x1200
#define REGS_V3D_MMU          ((v3d_mmu_regs*)(V3D_BASE + V3D_MMU_OFFSET))

#define V3D_CORE_OFFSET       0x4000
#define V3D_CORE_BASE(n)      (V3D_BASE + (n+1)*V3D_CORE_OFFSET)
#define REGS_V3D_CORE(n)      ((reg32 *)V3D_CORE_BASE(n))

typedef struct {
  reg32 ident[3];   // 0  4   8
  reg32 res0[3];    // c  10  14
  reg32 misccfg;    // 18
  reg32 res1;       // 1c
  reg32 l2cactl;    // 20
  reg32 slcactl;    // 24
  reg32 res2[2];    // 28 2c
  reg32 l2tcactl;   // 30
  reg32 l2tflsta;   // 34
  reg32 l2tflend;   // 38
} v3d_core_ctl_regs;

// #define V3D_CORE_CTL_OFFSET   0x0
// #define REGS_V3D_CORE_CTL(n)  ((v3d_core_ctl_regs*)(V3D_CORE_BASE(n) + V3D_CORE_CTL_OFFSET))

#define REGS_V3D_CORE_CTL(n)  ((v3d_core_ctl_regs*)V3D_CORE_BASE(n))

typedef struct {
  reg32 sts;
  reg32 set;
  reg32 clr;
  reg32 msk_sts;
  reg32 msk_set;
  reg32 msk_clr;
} v3d_core_int_regs;

#define V3D_CORE_INT_OFFSET   0x50
#define REGS_V3D_CORE_INT(n)  ((v3d_core_int_regs*)(V3D_CORE_BASE(n) + V3D_CORE_INT_OFFSET))

typedef struct {
  reg32 ctcs[2];
  reg32 ctea[2];
  reg32 ctca[2];
  reg32 ctra[2];
  reg32 ctlc[2];
  reg32 ctpc[2];
  reg32 pcs;
  reg32 bfc;
  reg32 rfc;
  reg32 tfbc;
  reg32 tfit;
  reg32 ct1cfg;
  reg32 ct1tilect;
  reg32 ct1tskip;
  reg32 ct1ptct;
  reg32 ctsync[2];
  reg32 ct0qts;
  reg32 ctqba[2];
  reg32 ctqea[2];
  reg32 ct0qma;
  reg32 ct0qms;
  reg32 ct1qcfg;
} v3d_core_cle_regs;

#define V3D_CORE_CLE_OFFSET   0x100
#define REGS_V3D_CORE_CLE(n)  ((v3d_core_cle_regs*)(V3D_CORE_BASE(n) + V3D_CORE_CLE_OFFSET))

typedef struct {
  reg32 bpca;
  reg32 bpcs;
  reg32 bpoa;
  reg32 bpos;
  reg32 bxcf;
} v3d_core_ptb_regs;

#define V3D_CORE_PTB_OFFSET   0x300
#define REGS_V3D_CORE_PTB(n)  ((v3d_core_ptb_regs*)(V3D_CORE_BASE(n) + V3D_CORE_PTB_OFFSET))

typedef struct {
  reg32 en;
  reg32 clr;
  reg32 overflow;
  reg32 res;
  reg32 src_0_3;
  reg32 src_4_7;
  reg32 src_8_11;
  reg32 src_12_15;
  reg32 src_16_19;
  reg32 src_20_23;
  reg32 src_24_27;
  reg32 src_28_31;
  reg32 pctr[32];
} v3d_core_pctr_regs;

#define V3D_CORE_PCTR_OFFSET  0x650
#define REGS_V3D_CORE_PCTR(n) ((v3d_core_pctr_regs*)(V3D_CORE_BASE(n) + V3D_CORE_PCTR_OFFSET))

typedef struct {
  reg32 status;
  reg32 cfg;
  reg32 vio_addr;
  reg32 vio_type;
  reg32 table_addr;
  reg32 clear_load;
  reg32 preserve_load;
  reg32 valid_lines;
} v3d_core_gmp_regs;

#define V3D_CORE_GMP_OFFSET   0x800
#define REGS_V3D_CORE_GMP(n)  ((v3d_core_gmp_regs*)(V3D_CORE_BASE(n) + V3D_CORE_GMP_OFFSET))

typedef struct {
  reg32 status;           // 00
  reg32 queued_cfg[7];    // 04 08 0c 10 14 18 1c
  reg32 current_cfg[7];   // 20 24 28 2c 30 34 38
  reg32 current_id[2];    // 3c 40
} v3d_core_csd_regs;

#define V3D_CORE_CSD_OFFSET   0x900
#define REGS_V3D_CORE_CSD(n)  ((v3d_core_csd_regs*)(V3D_CORE_BASE(n) + V3D_CORE_CSD_OFFSET))

typedef struct {
  reg32 dbge;
  reg32 fdbgo;
  reg32 fdbgb;
  reg32 fdbgr;
  reg32 fdbgs;
  reg32 res0[3];
  reg32 stat;
} v3d_core_err_regs;

#define V3D_CORE_ERR_OFFSET   0xf00
#define REGS_V3D_CORE_ERR(n)  ((v3d_core_err_regs*)(V3D_CORE_BASE(n) + V3D_CORE_ERR_OFFSET))
