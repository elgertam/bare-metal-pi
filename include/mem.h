#pragma once

#include "common.h"

void* memzero(void* src, u64 n);
void* memcpy(void* dest, void *src, u64 n);

#define GPU_CACHED_BASE       0x40000000
#define GPU_UNCACHED_BASE     0xC0000000
#define GPU_MEM_BASE          GPU_UNCACHED_BASE

#define BUS_ADDRESS(addr)     (((addr) & ~GPU_UNCACHED_BASE) | GPU_MEM_BASE)
