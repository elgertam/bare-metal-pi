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
#include "gpu.h"
#include "video.h"

extern unsigned char * memset_code;

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

static u64 gpu_code [] __attribute__((aligned(4))) =  {
    0x3d807180bb800000,
    0x3de0318bb683f006,
    0x3c00318cb6836040,
    0x3de021813883e044,
    0x3de0318bb683f00b,
    0x3c00318cb6836040,
    0x3c203180bb800000,
    0x3c203180bb800000,
    0x3c003180bb800000,
    0x3c003180bb800000,
    0x3c203180bb800000,
    0x3c003180bb800000,
    0x3c003180bb800000
};

// static u32 arr[0x4000] = {};
// u32 arr_length = 0x4000;

static u32 uniform [3] __attribute__((aligned(4))) = {
    0,
    ~0,
    ~0
};

void gpu_info(){
    i16 res = 0;
    u32 ncores;

    PROBE(REGS_V3D_HUB_IDENT->ident[0]);

    printf("Initialize GPU\n");
    res = gpu_init();
    if (res != 0){
        printf("GPU initialization failed %d\n", res);
        return;
    }
    printf("Initialize GPU complete\n");

    PROBE(REGS_V3D_HUB_IDENT->axi_cfg);
    PROBE(REGS_V3D_HUB_IDENT->uif_cfg);
    PROBE(REGS_V3D_HUB_IDENT->ident[0]);
    PROBE(REGS_V3D_HUB_IDENT->ident[1]);
    PROBE(REGS_V3D_HUB_IDENT->ident[2]);
    PROBE(REGS_V3D_HUB_IDENT->ident[3]);

    ncores = (REGS_V3D_HUB_IDENT->ident[1] & VHI_NCORES_MASK) >> VHI_NCORES_SHIFT;
    printf("number of GPU cores = %d\n", ncores);

    PROBE(REGS_V3D_CORE_CTL(0)->ident[0]);
    PROBE(REGS_V3D_CORE_CTL(0)->ident[1]);
    PROBE(REGS_V3D_CORE_CTL(0)->ident[2]);

    PROBE(REGS_V3D_CORE_CTL(1)->ident[0]);
    PROBE(REGS_V3D_CORE_CTL(1)->ident[1]);
    PROBE(REGS_V3D_CORE_CTL(1)->ident[2]);


    uniform[0] = (u32)(&uniform[1]);

    // arr[2] = arr_length;

    PROBE(gpu_code);
    PROBE(uniform);

    PROBE(uniform[1]);
    PROBE(uniform[2]);

    printf("Running GPU kernel\n");

    run_gpu(gpu_code, uniform);

    printf("Finished GPU kernel\n");

    PROBE(uniform[1]);
    PROBE(uniform[2]);
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
    video_set_dma(true);
    video_init();
    video_set_resolution(1280, 800, 32);
    // video_set_resolution(1600, 900, 32);
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
