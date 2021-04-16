#include "common.h"
#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "sched.h"
#include "fork.h"
#include "mini_uart.h"
#include "sys.h"
#include "debug.h"

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

void user_process1(char *array)
{
    char buf[2] = {0};
    while (1){
        for (int i = 0; i < 5; i++){
            buf[0] = array[i];
            call_sys_write(buf);
            delay(100000);
        }
    }
}

void user_process(){
    char buf[31] = {0};
    tfp_sprintf(buf, "User process started\n\r");
    call_sys_write(buf);
    unsigned long stack = call_sys_malloc();
    if (stack < 0) {
        printf("Error while allocating stack for process 1\n\r");
        return;
    }
    int err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"12345", stack);
    if (err < 0){
        printf("Error while cloning process 1\n\r");
        return;
    }
    stack = call_sys_malloc();
    if (stack < 0) {
        printf("Error while allocating stack for process 1\n\r");
        return;
    }
    err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"abcd", stack);
    if (err < 0){
        printf("Error while cloning process 2\n\r");
        return;
    }
    call_sys_exit();
}

void kernel_process(){
    printf("Kernel process started.\r\n");
    i32 err = move_to_user_mode((u64)&user_process);
    if (err < 0){
        printf("Error while moving process to user mode\n\r");
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
    printf("\tBoard: Raspberry Pi 3\n");
#endif

#if RPI_VERSION == 4
    printf("\tBoard: Raspberry Pi 4\n");
#endif
    printf("\tClock Frequency: %d MHz\n", get_freq());
    printf("\tException Level: %d\n", get_el());

//  ------------------ Start OS -------------------

    i32 result;

    result = copy_process(PF_KTHREAD, (u64)&kernel_process, 0, 0);
    if (result < 0) {
        printf("error while starting kernel process");
        return;
    }

    while(1) {
        schedule();
    }
}
