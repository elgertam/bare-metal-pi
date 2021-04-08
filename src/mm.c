#include "mm.h"

static enum PageStatus mem_map [ PAGING_PAGES ] = {FREE, };

u64 get_free_page() {
    for (int i = 0; i < PAGING_PAGES; i++) {
        if (mem_map[i] == FREE) {
            mem_map[i] = ALLOCATED;
            return LOW_MEMORY + (i * PAGE_SIZE);
        }
    }
    return 0; // all pages are allocated
}

void free_page(u64 p) {
    mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = FREE;  // remember, C uses integer division
}
