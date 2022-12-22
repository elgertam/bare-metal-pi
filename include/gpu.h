#pragma once

#include "peripherals/pm.h"
#include "peripherals/v3d.h"

#define INIT_SUCCESS 0
#define AXI_M_INIT_FAIL -1
#define AXI_S_INIT_FAIL -2

i16 gpu_init(void);

void run_gpu(u64 * code, u32 * uniform);

u64 * get_gpu_code();
