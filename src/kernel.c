#include "common.h"
#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "sched.h"
#include "fork.h"
#include "mini_uart.h"

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
}

void process1(char *array){
    while (1) {
        for (int i = 0; i < 5; i++){
            uart_send(array[i]);
            delay(100000);
        }
    }
}

void process2(char *array){
    while (1) {
        for (int i = 0; i < 5; i++){
            uart_send(array[i]);
            delay(1000000);
        }
    }
}


void kernel_main() {

// -------------------- Initialize machine --------

    uart_init();
    init_printf(0, putc);
    printf("\nRaspberry Pi Bare Metal OS Initializing...\n");

    irq_init_vectors();
    timer_init();
    enable_interrupt_controller();
    irq_enable();

#if RPI_VERSION == 3
    printf("\t Board: Raspberry Pi 3\n");
#endif

#if RPI_VERSION == 4
    printf("\tBoard: Raspberry Pi 4\n");
#endif

    printf("\tException Level: %d\n", get_el());

//  ------------------ Start OS -------------------

    int result;

    result = copy_process((u64)&process1, (u64)"12345");
    if (result != 0) {
        printf("Error while starting process 1");
    }

    result = copy_process((u64)&process2, (u64)"abcde");
    if (result != 0) {
        printf("Error while starting process 2");
    }

    while(1) {
        schedule();
    }
}
