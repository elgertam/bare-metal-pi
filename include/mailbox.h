#pragma once

#include "common.h"

typedef struct {
    u32 id;
    u32 buffer_size;
    u32 value_length;
} mailbox_tag;

typedef struct {
    u32 size;
    void *tag;
} mailbox_command;

typedef struct {
    mailbox_tag tag;
    u32 id;
    u32 value;
} mailbox_generic;

typedef struct {
    mailbox_tag tag;
    u32 id;
    u32 state;
} mailbox_power;

typedef struct {
    mailbox_tag tag;
    u32 id;
    u32 rate;
} mailbox_clock;

typedef enum {
    CT_EMMC =       1,
    CT_UART =       2,
    CT_ARM =        3,
    CT_CORE =       4,
    CT_V3D =        5,
    CT_H264 =       6,
    CT_ISP =        7,
    CT_SDRAM =      8,
    CT_PIXEL =      9,
    CT_PWM =        10,
    CT_HEVC =       11,
    CT_EMMC2 =      12,
    CT_M2MC =       13,
    CT_PIXEL_BVB =  14,
    CT_COUNT =      15,
} clock_type;

typedef enum {
    RPD_I2C0 =          0,
    RPD_I2C1 =          1,
    RPD_I2C2 =          2,
    RPD_VIDEO_SCALER =  3,
    RPD_VPU1 =          4,
    RPD_HDMI =          5,
    RPD_USB =           6,
    RPD_VEC =           7,
    RPD_JPEG =          8,
    RPD_H264 =          9,
    RPD_V3D =           10,
    RPD_ISP =           11,
    RPD_UNICAM0 =       12,
    RPD_UNICAM1 =       13,
    RPD_CCP2RX =        14,
    RPD_CSI2 =          15,
    RPD_CPI =           16,
    RPD_DSI0 =          17,
    RPD_DSI1 =          18,
    RPD_TRANSPOSER =    19,
    RPD_CCP2TX =        20,
    RPD_CDP =           21,
    RPD_ARM =           22,
    RPD_COUNT =         23,
} rpi_power_domain;

// #define RPI_POWER_DOMAIN_I2C0           0
// #define RPI_POWER_DOMAIN_I2C1           1
// #define RPI_POWER_DOMAIN_I2C2           2
// #define RPI_POWER_DOMAIN_VIDEO_SCALER   3
// #define RPI_POWER_DOMAIN_VPU1           4
// #define RPI_POWER_DOMAIN_HDMI           5
// #define RPI_POWER_DOMAIN_USB            6
// #define RPI_POWER_DOMAIN_VEC            7
// #define RPI_POWER_DOMAIN_JPEG           8
// #define RPI_POWER_DOMAIN_H264           9
// #define RPI_POWER_DOMAIN_V3D            10
// #define RPI_POWER_DOMAIN_ISP            11
// #define RPI_POWER_DOMAIN_UNICAM0        12
// #define RPI_POWER_DOMAIN_UNICAM1        13
// #define RPI_POWER_DOMAIN_CCP2RX         14
// #define RPI_POWER_DOMAIN_CSI2           15
// #define RPI_POWER_DOMAIN_CPI            16
// #define RPI_POWER_DOMAIN_DSI0           17
// #define RPI_POWER_DOMAIN_DSI1           18
// #define RPI_POWER_DOMAIN_TRANSPOSER     19
// #define RPI_POWER_DOMAIN_CCP2TX         20
// #define RPI_POWER_DOMAIN_CDP            21
// #define RPI_POWER_DOMAIN_ARM            22
//
// #define RPI_POWER_DOMAIN_COUNT          23

typedef enum {
    RFS_REQUEST =   0,
    RFS_SUCCESS =   0x80000000,
    RFS_ERROR =     0x80000001,
} rpi_firmware_property_status;

typedef enum {
    RFT_PROPERTY_END =                              0,
    RFT_GET_FIRMWARE_REVISION =                     0x00000001,

    RFT_SET_CURSOR_INFO =                           0x00008010,
    RFT_SET_CURSOR_STATE =                          0x00008011,

    RFT_GET_BOARD_MODEL =                           0x00010001,
    RFT_GET_BOARD_REVISION =                        0x00010002,
    RFT_GET_BOARD_MAC_ADDRESS =                     0x00010003,
    RFT_GET_BOARD_SERIAL =                          0x00010004,
    RFT_GET_ARM_MEMORY =                            0x00010005,
    RFT_GET_VC_MEMORY =                             0x00010006,
    RFT_GET_CLOCKS =                                0x00010007,
    RFT_GET_POWER_STATE =                           0x00020001,
    RFT_GET_TIMING =                                0x00020002,
    RFT_SET_POWER_STATE =                           0x00028001,
    RFT_GET_CLOCK_STATE =                           0x00030001,
    RFT_GET_CLOCK_RATE =                            0x00030002,
    RFT_GET_VOLTAGE =                               0x00030003,
    RFT_GET_MAX_CLOCK_RATE =                        0x00030004,
    RFT_GET_MAX_VOLTAGE =                           0x00030005,
    RFT_GET_TEMPERATURE =                           0x00030006,
    RFT_GET_MIN_CLOCK_RATE =                        0x00030007,
    RFT_GET_MIN_VOLTAGE =                           0x00030008,
    RFT_GET_TURBO =                                 0x00030009,
    RFT_GET_MAX_TEMPERATURE =                       0x0003000a,
    RFT_ALLOCATE_MEMORY =                           0x0003000c,
    RFT_LOCK_MEMORY =                               0x0003000d,
    RFT_UNLOCK_MEMORY =                             0x0003000e,
    RFT_RELEASE_MEMORY =                            0x0003000f,
    RFT_EXECUTE_CODE =                              0x00030010,
    RFT_EXECUTE_QPU =                               0x00030011,
    RFT_SET_ENABLE_QPU =                            0x00030012,
    RFT_GET_DISPMANX_RESOURCE_MEM_HANDLE =          0x00030014,
    RFT_GET_EDID_BLOCK =                            0x00030020,
    RFT_GET_DOMAIN_STATE =                          0x00030030,
    RFT_SET_CLOCK_STATE =                           0x00038001,
    RFT_SET_CLOCK_RATE =                            0x00038002,
    RFT_SET_VOLTAGE =                               0x00038003,
    RFT_SET_TURBO =                                 0x00038009,
    RFT_SET_DOMAIN_STATE =                          0x00038030,

    /* Dispmanx TAGS */
    RFT_FRAMEBUFFER_ALLOCATE =                      0x00040001,
    RFT_FRAMEBUFFER_BLANK =                         0x00040002,
    RFT_FRAMEBUFFER_GET_PHYSICAL_WIDTH_HEIGHT =     0x00040003,
    RFT_FRAMEBUFFER_GET_VIRTUAL_WIDTH_HEIGHT =      0x00040004,
    RFT_FRAMEBUFFER_GET_DEPTH =                     0x00040005,
    RFT_FRAMEBUFFER_GET_PIXEL_ORDER =               0x00040006,
    RFT_FRAMEBUFFER_GET_ALPHA_MODE =                0x00040007,
    RFT_FRAMEBUFFER_GET_PITCH =                     0x00040008,
    RFT_FRAMEBUFFER_GET_VIRTUAL_OFFSET =            0x00040009,
    RFT_FRAMEBUFFER_GET_OVERSCAN =                  0x0004000a,
    RFT_FRAMEBUFFER_GET_PALETTE =                   0x0004000b,
    RFT_FRAMEBUFFER_RELEASE =                       0x00048001,
    RFT_FRAMEBUFFER_TEST_PHYSICAL_WIDTH_HEIGHT =    0x00044003,
    RFT_FRAMEBUFFER_TEST_VIRTUAL_WIDTH_HEIGHT =     0x00044004,
    RFT_FRAMEBUFFER_TEST_DEPTH =                    0x00044005,
    RFT_FRAMEBUFFER_TEST_PIXEL_ORDER =              0x00044006,
    RFT_FRAMEBUFFER_TEST_ALPHA_MODE =               0x00044007,
    RFT_FRAMEBUFFER_TEST_VIRTUAL_OFFSET =           0x00044009,
    RFT_FRAMEBUFFER_TEST_OVERSCAN =                 0x0004400a,
    RFT_FRAMEBUFFER_TEST_PALETTE =                  0x0004400b,
    RFT_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT =     0x00048003,
    RFT_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT =      0x00048004,
    RFT_FRAMEBUFFER_SET_DEPTH =                     0x00048005,
    RFT_FRAMEBUFFER_SET_PIXEL_ORDER =               0x00048006,
    RFT_FRAMEBUFFER_SET_ALPHA_MODE =                0x00048007,
    RFT_FRAMEBUFFER_SET_VIRTUAL_OFFSET =            0x00048009,
    RFT_FRAMEBUFFER_SET_OVERSCAN =                  0x0004800a,
    RFT_FRAMEBUFFER_SET_PALETTE =                   0x0004800b,

    RFT_GET_COMMAND_LINE =                          0x00050001,
    RFT_GET_DMA_CHANNELS =                          0x00060001,
} rpi_firmware_property_tag;

u32 mailbox_clock_rate(clock_type ct);

bool mailbox_generic_command(u32 tag_id, u32 id, u32 *value);
bool mailbox_power_check(u32 type);
bool mailbox_process(mailbox_tag *tag, u32 tag_size);
