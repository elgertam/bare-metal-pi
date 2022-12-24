#include "common.h"
#include "debug.h"
#include "gpu.h"
#include "printf.h"
#include "timer.h"

i16 gpu_init(void){
    u32 count;

    // turn on & reset grafx power
    REGS_PM->grafx |= PM_PASSWORD | PM_V3DRSTN;

    // enable master AXI bridges
    REGS_ASB->v3d_m_ctrl = PM_PASSWORD | (REGS_ASB->v3d_m_ctrl & ~ASB_REQ_STOP);

    // wait for acknowledgement
    count = 0;
    while (REGS_ASB->v3d_m_ctrl & ASB_ACK) {
        timer_sleep(100);
        count ++;
        if (count > 100) {
            return AXI_M_INIT_FAIL;
        }
    };

    // enable slave AXI bridges
    REGS_ASB->v3d_s_ctrl = PM_PASSWORD | (REGS_ASB->v3d_s_ctrl & ~ASB_REQ_STOP);

    // wait for acknowledgement
    count = 0;
    while (REGS_ASB->v3d_s_ctrl & ASB_ACK) {
        timer_sleep(100);
        count ++;
        if (count > 100) {
            return AXI_S_INIT_FAIL;
        }
    };

    return INIT_SUCCESS;
}

#define CSD_STATUS(n) ((n >> 4) & ((1 << 8) - 1u))

void run_gpu(u64 * code, u32 * uniform) {

    // PROBE(code);
    // PROBE(uniform);

    u32 V3D_L2TCACTL_TMUWCF = 1 << 8;
    u32 V3D_L2TCACTL_L2TFLS = 1 << 0;
    u32 V3D_L2TCACTL_FLM_CLEAN = (2 & ((1 << 2) - 1)) << 1;
    u32 V3D_L2TCACTL_FLM_FLUSH = (0 & ((1 << 2) - 1)) << 1;

    v3d_core_ctl_regs * v3d_core0_ctl = REGS_V3D_CORE_CTL(0);

    printf("Resetting L2 cache\n");

    // PROBE(v3d_core0_ctl->l2tflsta);

    v3d_core0_ctl->l2tflsta = 0;
    // PROBE(v3d_core0_ctl->l2tflsta);

    // PROBE(v3d_core0_ctl->l2tflend);

    v3d_core0_ctl->l2tflend = ~0;
    // PROBE(v3d_core0_ctl->l2tflend);

    // PROBE(v3d_core0_ctl->l2tcactl);

    v3d_core0_ctl->l2tcactl = V3D_L2TCACTL_L2TFLS | V3D_L2TCACTL_FLM_CLEAN;
    // PROBE(v3d_core0_ctl->l2tcactl);

    v3d_core0_ctl->l2tcactl = V3D_L2TCACTL_TMUWCF;
    // PROBE(v3d_core0_ctl->l2tcactl);

    while (v3d_core0_ctl->l2tcactl & V3D_L2TCACTL_TMUWCF)
        ;
    // PROBE(v3d_core0_ctl->l2tcactl);

    v3d_core0_ctl->l2tcactl = V3D_L2TCACTL_L2TFLS | V3D_L2TCACTL_FLM_FLUSH;
    // PROBE(v3d_core0_ctl->l2tcactl);

    while (v3d_core0_ctl->l2tcactl & V3D_L2TCACTL_L2TFLS)
        ;
    // PROBE(v3d_core0_ctl->l2tcactl);

    v3d_core_csd_regs * v3d_core0_csd = REGS_V3D_CORE_CSD(0);

    u32 num_wg_x = 16;
    u32 num_wg_y = 1;
    u32 num_wg_z = 1;
    u32 wg_size = num_wg_x * num_wg_y * num_wg_z;
    u32 wg_per_sg = 16;

    u32 num_batch = 1;

    PROBE(v3d_core0_csd->status);
    PROBE(v3d_core0_csd->queued_cfg[0]);
    PROBE(v3d_core0_csd->queued_cfg[1]);
    PROBE(v3d_core0_csd->queued_cfg[2]);
    PROBE(v3d_core0_csd->queued_cfg[3]);
    PROBE(v3d_core0_csd->queued_cfg[4]);
    PROBE(v3d_core0_csd->queued_cfg[5]);
    PROBE(v3d_core0_csd->queued_cfg[6]);

    v3d_core0_csd->queued_cfg[1] = num_wg_y << 16;
    v3d_core0_csd->queued_cfg[2] = num_wg_z << 16;
    v3d_core0_csd->queued_cfg[3] = ((((wg_per_sg * wg_size + 16u - 1u) / 16u) - 1u) << 12) |
        (wg_per_sg << 8) || (wg_size);

    v3d_core0_csd->queued_cfg[4] = num_batch - 1;
    v3d_core0_csd->queued_cfg[5] = (u32)code;
    v3d_core0_csd->queued_cfg[6] = (u32)uniform;

    // PROBE(v3d_core0_csd->status);
    // PROBE(v3d_core0_csd->queued_cfg[0]);
    // PROBE(v3d_core0_csd->queued_cfg[1]);
    // PROBE(v3d_core0_csd->queued_cfg[2]);
    // PROBE(v3d_core0_csd->queued_cfg[3]);
    // PROBE(v3d_core0_csd->queued_cfg[4]);
    // PROBE(v3d_core0_csd->queued_cfg[5]);
    // PROBE(v3d_core0_csd->queued_cfg[6]);

    printf("Launching job\n");

    u32 count = CSD_STATUS(v3d_core0_csd->status);

    v3d_core0_csd->queued_cfg[0] = num_wg_x << 16;

    PROBE(v3d_core0_csd->status);

    PROBE(v3d_core0_csd->queued_cfg[0]);
    PROBE(v3d_core0_csd->queued_cfg[1]);
    PROBE(v3d_core0_csd->queued_cfg[2]);
    PROBE(v3d_core0_csd->queued_cfg[3]);
    PROBE(v3d_core0_csd->queued_cfg[4]);
    PROBE(v3d_core0_csd->queued_cfg[5]);
    PROBE(v3d_core0_csd->queued_cfg[6]);

    // printf("Sleeping 2s\n");
    // timer_sleep(2000);

    u32 icount = 0;
    while (CSD_STATUS(v3d_core0_csd->status) <= count && icount < 10) {
        if (icount % 250 == 0){
            printf("Sleeping 1s\n");
            timer_sleep(1000);
            PROBE(v3d_core0_csd->status);
            printf("Completed: %d\n", CSD_STATUS(v3d_core0_csd->status));
        }
        icount++;
    }

    PROBE(v3d_core0_csd->status);
    printf("Completed: %d\n", CSD_STATUS(v3d_core0_csd->status));

    v3d_core0_ctl->l2tcactl = V3D_L2TCACTL_TMUWCF;

    while (v3d_core0_ctl->l2tcactl & V3D_L2TCACTL_TMUWCF)
        ;

    v3d_core0_ctl->l2tcactl = V3D_L2TCACTL_L2TFLS | V3D_L2TCACTL_FLM_FLUSH;

    while (v3d_core0_ctl->l2tcactl & V3D_L2TCACTL_L2TFLS)
        ;
}
