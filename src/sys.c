#include "fork.h"
#include "printf.h"
#include "utils.h"
#include "sched.h"
#include "mm.h"

void sys_write(char * buffer) {
    printf(buffer);
}

i32 sys_clone(u64 stack) {
    return copy_process(0, 0, 0, stack);
}

u64 sys_malloc() {
    u64 addr = get_free_page();

    if (!addr) {
        return -1; // TODO: is this right? -1 would be 0xFFFF_FFFF_FFFF_FFFF
    }

    return addr;
}

void sys_exit() {
    exit_process();
}

void * const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit};
