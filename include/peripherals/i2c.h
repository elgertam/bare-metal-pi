#pragma once
#include "peripherals/base.h"
#include "common.h"

struct I2CRegs {
    reg32 control;
    reg32 status;
    reg32 data_length;
    reg32 slave_address;
    reg32 fifo;
    reg32 div;
    reg32 delay;
    reg32 clock_stretch;
};

#define REGS_I2C ((struct I2CRegs *)(PBASE + 0x00804000))

typedef enum {
    C_I2CEN =   (1 << 15),
    C_INTR =    (1 << 10),
    C_INTT =    (1 << 9),
    C_INTD =    (1 << 8),
    C_ST =      (1 << 7),
    C_CLEAR =   (1 << 5),
    C_READ =    (1 << 0),
} control_reg;

typedef enum {
    S_CLKT =    (1 << 9),
    S_ERR =     (1 << 8),
    S_RXF =     (1 << 7),
    S_TXE =     (1 << 6),
    S_RXD =     (1 << 5),
    S_TXD =     (1 << 4),
    S_RXR =     (1 << 3),
    S_TXW =     (1 << 2),
    S_DONE =    (1 << 1),
    S_TA =      (1 << 0),
} status_reg;

// // Control register
// #define C_I2CEN (1 << 15)
// #define C_INTR  (1 << 10)
// #define C_INTT  (1 << 9)
// #define C_INTD  (1 << 8)
// #define C_ST    (1 << 7)
// #define C_CLEAR (1 << 5)
// #define C_READ  (1 << 0)

// // Status register
// #define S_CLKT  (1 << 9)
// #define S_ERR   (1 << 8)
// #define S_RXF   (1 << 7)
// #define S_TXE   (1 << 6)
// #define S_RXD   (1 << 5)
// #define S_TXD   (1 << 4)
// #define S_RXR   (1 << 3)
// #define S_TXW   (1 << 2)
// #define S_DONE  (1 << 1)
// #define S_TA    (1 << 0)
