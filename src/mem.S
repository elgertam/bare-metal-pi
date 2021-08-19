.globl memcpy
memcpy:
    mov     x9, x0
1:
    ldr     x3, [x1], #8
    str     x3, [x0], #8
    subs    x2, x2, #8
    b.gt    1b
    mov     x0, x9
    ret

.globl memzero
memzero:
    mov     x9, x0
1:
    str     xzr, [x0], #8
    subs    x1, x1, #8
    b.gt    1b
    mov     x0, x9
    ret
