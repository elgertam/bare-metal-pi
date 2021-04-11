#pragma once

#define __NUM_syscalls       4

#define SYS_WRITE_NUMBER    0
#define SYS_MALLOC_NUMBER   1
#define SYS_CLONE_NUMBER    2
#define SYS_EXIT_NUMBER     3

#ifndef __ASSEMBLER__

#include "common.h"

enum SysCalls {
    write = SYS_WRITE_NUMBER,
    malloc = SYS_MALLOC_NUMBER,
    clone = SYS_CLONE_NUMBER,
    exit = SYS_EXIT_NUMBER
};

void sys_write(char * buffer);
int sys_fork();

void call_sys_write(char * buffer);
i32 call_sys_clone(u64 fn, u64 arg, u64 stack);
u64  call_sys_malloc();
void call_sys_exit();

#endif
