#include "sys.h"

.globl call_sys_write
call_sys_write:
    mov     w8, #SYS_WRITE_NUMBER
    svc     #0
    ret

.globl call_sys_malloc
call_sys_malloc:
    mov     w8, #SYS_MALLOC_NUMBER
    svc     #0
    ret

.globl call_sys_exit
call_sys_exit:
    mov     w8, #SYS_EXIT_NUMBER
    svc     #0
    ret

//                         x0      x1       x2
// void call_sys_clone(u64 fn, u64 arg, u64 stack);
.globl call_sys_clone
call_sys_clone:
    mov     x10, x0
    mov     x11, x1
    mov     x12, x2

    mov     x0, x2
    mov     x8, #SYS_CLONE_NUMBER
    svc     0x0

    cmp     x0, #0
    beq     thread_start
    ret

thread_start:
    mov     x29, 0

    mov     x0, x11
    blr     x10

    mov     x8, #SYS_EXIT_NUMBER
    svc     0x0
