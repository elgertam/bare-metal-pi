#include "common.h"
#include "utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
}

void kernel_main() {
    uart_init();
    init_printf(0, putc);
    printf("\nRaspberry Pi Bare Metal OS Initializing...\n");

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

#if RPI_VERSION == 3
    printf("\t Board: Raspberry Pi 3\n");
#endif

#if RPI_VERSION == 4
    printf("\tBoard: Raspberry Pi 4\n");
#endif

    printf("\tException Level: %d\n", get_el());

    printf("\n\nDone\n");

    while(1) {
        // uart_send(uart_recv());
    }
}
