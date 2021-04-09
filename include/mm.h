#pragma once

#include "peripherals/base.h"

#define PAGE_SHIFT      12
#define TABLE_SHIFT     9
#define SECTION_SHIFT   (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE       (1 << PAGE_SHIFT)
#define SECTION_SIZE    (1 << SECTION_SHIFT)

#define LOW_MEMORY      (2 * SECTION_SIZE)

#if RPI_VERSION == 4
#define HIGH_MEMORY     0x20000000
#else
#define HIGH_MEMORY     PBASE
#endif

#define PAGING_MEMORY   (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES    (PAGING_MEMORY/PAGE_SIZE)

#ifndef __ASSEMBLER__

#include "common.h"

enum PageStatus {
    FREE = 0,
    ALLOCATED = 1
};

u64 get_free_page();
void memzero(u64 src, u64 n);
void free_page(u64 p);

#endif
