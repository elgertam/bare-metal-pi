#pragma once

#include "common.h"

extern void delay(u64 ticks);
extern void put32(u64 address, u32 value);
extern u32 get32(u64 address);
extern u32 get_el(void);
