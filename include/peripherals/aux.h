#pragma once

#include "common.h"
#include "peripherals/base.h"

struct AuxRegs {
    reg32 irq_status;   // AUX_IRQ
    reg32 enables;      // AUX_ENABLES
    reg32 reserved[14]; // 56-byte gap = 14 `reg32`s
    reg32 mu_io;        // AUX_MU_IO_REG
    reg32 mu_ier;       // AUX_MU_IER_REG
    reg32 mu_iir;       // AUX_MU_IIR_REG
    reg32 mu_lcr;       // AUX_MU_LCR_REG
    reg32 mu_mcr;       // AUX_MU_MCR_REG
    reg32 mu_lsr;       // AUX_MU_LSR_REG
    reg32 mu_msr;       // AUX_MU_MSR_REG
    reg32 mu_scratch;   // AUX_MU_SCRATCH
    reg32 mu_control;   // AUX_MU_CNTL_REG
    reg32 mu_status;    // AUX_MU_STAT_REG
    reg32 mu_baud_rate; // AUX_MU_BAUD_REG
};

#define REGS_AUX ((struct AuxRegs *)(PBASE + 0x00215000))
