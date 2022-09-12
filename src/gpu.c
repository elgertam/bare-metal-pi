#include "common.h"
#include "gpu.h"
#include "timer.h"

i16 gpu_init(void){
    reg32* pm;
    reg32* asb_v3d_m_ctrl;
    reg32* asb_v3d_s_ctrl;
    u32 count;

    // MMIO::write(MMIO::PM_GRAFX, MMIO::read(MMIO::PM_GRAFX) | PM_PASSWORD | PM_V3DRSTN);
    pm = (reg32 *)PM_GRAFX;
    *pm = (*pm) | PM_PASSWORD | PM_V3DRSTN;

    asb_v3d_m_ctrl = (reg32 *)ASB_V3D_M_CTRL;
    *asb_v3d_m_ctrl = PM_PASSWORD | ((*asb_v3d_m_ctrl) & ~ASB_REQ_STOP);
    // printf("waiting for acknowledgement\n");

    // wait for acknowledgement
    count = 0;
    while ((*asb_v3d_m_ctrl) & ASB_ACK) {
        timer_sleep(100);
        count ++;
        if (count > 100) {
            // printf("Value ASB_V3D_M_CTRL(0x%08x) 0x%08x\n", (u32)asb_v3d_m_ctrl, (*asb_v3d_m_ctrl));
            // printf("Failed to acknowledge master controller! Exiting...\n");
            return AXI_M_INIT_FAIL;
        }
    };
    // printf("master AXI bridge ACK recv'd\n");

    // enable slave AXI bridges
    // printf("enable slave AXI bridges\n");
    asb_v3d_s_ctrl = (reg32 *)ASB_V3D_S_CTRL;
    *asb_v3d_s_ctrl = PM_PASSWORD | ((*asb_v3d_s_ctrl) & ~ASB_REQ_STOP);

    // wait for acknowledgement
    // printf("waiting for acknowledgement\n");
    count = 0;
    while ((*asb_v3d_s_ctrl) & ASB_ACK) {
        timer_sleep(100);
        count ++;
        if (count > 100) {
            // printf("Failed to acknowledge slave controller! Exiting...\n");
            return AXI_S_INIT_FAIL;
        }
    };
    // printf("slave AXI bridge ACK recv'd\n");
    return INIT_SUCCESS;
}
