#include "common.h"
#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "sched.h"
#include "fork.h"
#include "mini_uart.h"
#include "i2c.h"
#include "spi.h"
#include "mailbox.h"
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

#define PBASE           0xFE000000
#define PM_BASE         (PBASE + 0x100000)
#define PM_V3DRSTN      (1 << 6)
#define PM_PASSWORD     0x5a000000
#define PM_GRAFX        (PM_BASE + 0x10c)

#define ASB_V3D_M_CTRL  0x0c
#define ASB_V3D_S_CTRL  0x08

#define ASB_REQ_STOP    (1 << 0)
#define ASB_ACK         (1 << 1)
#define ASB_EMPTY       (1 << 2)
#define ASB_FULL        (1 << 3)

void gpu_init(void){
    reg32* pm;
    reg32* asb_v3d_m_ctrl;
    reg32* asb_v3d_s_ctrl;

    // MMIO::write(MMIO::PM_GRAFX, MMIO::read(MMIO::PM_GRAFX) | PM_PASSWORD | PM_V3DRSTN);
    pm = (reg32 *)PM_GRAFX;
    *pm = (*pm) | PM_PASSWORD | PM_V3DRSTN;

    // enable master AXI bridges
    asb_v3d_m_ctrl = (reg32 *)ASB_V3D_M_CTRL;
    *asb_v3d_m_ctrl = PM_PASSWORD | ((*asb_v3d_m_ctrl) & ~ASB_REQ_STOP);
    // wait for acknowledgement
    while ((*asb_v3d_m_ctrl) & ASB_ACK);

    // enable slave AXI bridges
    asb_v3d_s_ctrl = (reg32 *)ASB_V3D_S_CTRL;
    *asb_v3d_s_ctrl = PM_PASSWORD | ((*asb_v3d_s_ctrl) & ~ASB_REQ_STOP);

    // wait for acknowledgement
    while ((*asb_v3d_s_ctrl) & ASB_ACK);
}

void gpu_info(){
    u32 v3d_reg;
    v3d_reg = (u32)*((reg32 *) 0xf1200008);
    printf("Value V3D_HUB_IDENT0(0xf1200008) 0x%08x\n", v3d_reg);
    v3d_reg = (u32)*((reg32 *) 0xfec00008);
    printf("Value V3D_HUB_IDENT0(0xfec00008) 0x%08x\n", v3d_reg);
    v3d_reg = (u32)*((reg32 *) 0x7ec00008);
    printf("Value V3D_HUB_IDENT0(0x7ec00008) 0x%08x\n", v3d_reg);

    printf("Initialize GPU\n");
    gpu_init();
    printf("Initialize GPU complete\n");

    v3d_reg = (u32)*((reg32 *) 0xf1200008);
    printf("Value V3D_HUB_IDENT0(0xf1200008) 0x%08x\n", v3d_reg);
    v3d_reg = (u32)*((reg32 *) 0xfec00008);
    printf("Value V3D_HUB_IDENT0(0xfec00008) 0x%08x\n", v3d_reg);
    v3d_reg = (u32)*((reg32 *) 0x7ec00008);
    printf("Value V3D_HUB_IDENT0(0x7ec00008) 0x%08x\n", v3d_reg);
}

void mailbox() {
    printf("MAILBOX\n");

    printf("EMMC CLOCK %d\n", mailbox_clock_rate(CT_EMMC));
    printf("UART CLOCK %d\n", mailbox_clock_rate(CT_UART));
    printf("ARM CLOCK %d\n", mailbox_clock_rate(CT_ARM));
    printf("CORE CLOCK %d\n", mailbox_clock_rate(CT_CORE));
    printf("V3D CLOCK %d\n", mailbox_clock_rate(CT_V3D));

    printf("VPU POWER STATE: %d\n", mailbox_power_check(RPD_VPU1));
    printf("V3D POWER STATE: %d\n", mailbox_power_check(RPD_V3D));


    for (int i = RPD_I2C0; i < RPD_COUNT; i++) {
        u32 on = 1;
        mailbox_generic_command(RFT_SET_DOMAIN_STATE, i, &on);
    }

    u32 on = 1;
    mailbox_generic_command(RFT_SET_ENABLE_QPU, 1, &on);
    mailbox_generic_command(RFT_SET_ENABLE_QPU, 1, &on);

    printf("V3D POWER STATE: %d\n", mailbox_power_check(RPD_V3D));

    u32 max_temp = 0;
    mailbox_generic_command(RFT_GET_MAX_TEMPERATURE, 0, &max_temp);

    printf("MAX TEMP: %d\n", max_temp);

    for (int i = 0; i < 1; i++) {
        u32 cur_temp = 0;

        mailbox_generic_command(RFT_GET_TEMPERATURE, 0, &cur_temp);

        printf("CUR TEMP: %dC\n", cur_temp / 1000);
        timer_sleep(1000);
    }
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
    printf("\tException Level: %d\n", get_el());
    mailbox();
    gpu_info();

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
