#pragma once

#define S_FRAME_SIZE            256         // size of all saved registers

#define SYNC_INVALID_EL1T    0
#define IRQ_INVALID_EL1T     1
#define FIQ_INVALID_EL1T     2
#define ERROR_INVALID_EL1T   3

#define SYNC_INVALID_EL1H    4
#define IRQ_INVALID_EL1H     5
#define FIQ_INVALID_EL1H     6
#define ERROR_INVALID_EL1H   7

#define SYNC_INVALID_EL0_64  8
#define IRQ_INVALID_EL0_64   9
#define FIQ_INVALID_EL0_64   10
#define ERROR_INVALID_EL0_64 11

#define SYNC_INVALID_EL0_32  12
#define IRQ_INVALID_EL0_32   13
#define FIQ_INVALID_EL0_32   14
#define ERROR_INVALID_EL0_32 15

#ifndef __ASSEMBLER__

void ret_from_fork(void);

#endif
