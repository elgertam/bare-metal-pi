#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"
#include "peripherals/aux.h"
#include "mini_uart.h"

const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1t",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32",

    "SYNC_ERROR",
    "SYSCALL_ERROR",

    "DEBUG_STATEMENT"
};

void enable_interrupt_controller() {
    u64 handled_interrupts = AUX_IRQ | SYS_TIMER_IRQ_1 | SYS_TIMER_IRQ_3;

    #if RPI_VERSION == 3
        REGS_IRQ->irq0_enable_1 = handled_interrupts;
    #endif

    #if RPI_VERSION == 4
        REGS_IRQ->irq0_enable_0 = handled_interrupts;
    #endif
}

void show_invalid_entry_message (u32 type, u64 esr, u64 address, u64 spsr, u64 el){
    printf("ERROR CAUGHT: %s - %d, ESR: 0x%X, Address: 0x%X, SPSR: 0x%x, EL: %d\n",
           entry_error_messages[type], type, esr, address, spsr, el);
}

void handle_irq(){
    u32 irq;
    #if RPI_VERSION == 3
        irq = REGS_IRQ->irq0_pending_1;
    #endif

    #if RPI_VERSION == 4
        irq = REGS_IRQ->irq0_pending_0;
    #endif

    while (irq) {
        // we're going to use cont here because continue kind of sucks
        u8 cont = 0;
        if (irq & AUX_IRQ) {
            irq &= ~AUX_IRQ;

            while ((REGS_AUX->mu_iir & 0x4) == 0x4){
                printf("UART recv: ");
                uart_send(uart_recv());
                printf("\n");
            }
            cont = 1 << 0;
        }
        if (irq & SYS_TIMER_IRQ_1) {
            irq &= ~SYS_TIMER_IRQ_1;

            handle_timer_1();
            cont = 1 << 1;
        }
        if (irq & SYS_TIMER_IRQ_3) {
            irq &= ~SYS_TIMER_IRQ_3;

            handle_timer_3();
            cont = 1 << 2;
        }
        if (!cont){
            // default:
            printf("Unknown pending IRQ: 0x%x\n", irq);
        }
    }
}
