#include "common.h"
#include "mailbox.h"
#include "peripherals/base.h"
#include "printf.h"
#include "mem.h"

typedef struct {
    reg32 read;     // Begin Mailbox 0
    reg32 res0[3];
    reg32 peek;
    reg32 sender;
    reg32 status;
    reg32 config;   // End Mailbox 0

    reg32 write;    // Begin Mailbox 1
    reg32 res1[7];  // End Mailbox 1
} mailbox_regs;

mailbox_regs *MBX() {
    return (mailbox_regs *)(PBASE + 0xB880);
}

typedef struct {
    u32 size;
    u32 code;
    u8 tags[0];
} property_buffer;

static u32 property_data[8192] __attribute__((aligned(16)));

typedef enum {
    MAIL_EMPTY = 0x40000000,
    MAIL_FULL=   0x80000000
} mail_status;

typedef enum {
    MAIL_POWER =    0,
    MAIL_FB =       1,
    MAIL_VUART =    2,
    MAIL_VCHIQ =    3,
    MAIL_LEDS =     4,
    MAIL_BUTTONS =  5,
    MAIL_TOUCH =    6,
    MAIL_COUNT =    7,
    MAIL_TAGS =     8,
    MAIL_TAGS_IN =  9,
} mail_channel;

static void mailbox_write(u8 channel, u32 data) {
    while (MBX()->status & MAIL_FULL) ;

    MBX()->write = (data & 0xfffffff0) | (channel & 0xf);
}

static u32 mailbox_read(u8 channel) {
    while (true) {
        while(MBX()->status & MAIL_EMPTY) ;

        u32 data = MBX()->read;

        u8 read_channel = (u8)(data & 0xf);

        if (read_channel == channel) {
            return data & 0xfffffff0;
        }
    }
}

bool mailbox_process(mailbox_tag *tag, u32 tag_size) {
    const u32 buffer_size = tag_size + sizeof(mailbox_tag);

    memcpy(property_data + 2, tag, tag_size);

    property_buffer *buff = (property_buffer *)property_data;
    buff->size = buffer_size;
    buff->code = RFS_REQUEST;

    property_data[(buffer_size / sizeof(u32)) - 1] = RFT_PROPERTY_END;

    mailbox_write(MAIL_TAGS, SAFE_TRUNCATE(property_data));

    mailbox_read(MAIL_TAGS);

    memcpy(tag, property_data+2, tag_size);

    return true;
}

bool mailbox_generic_command(u32 tag_id, u32 id, u32 * value) {
    mailbox_generic mbx;
    mbx.tag.id = tag_id;
    mbx.tag.value_length = 0;
    mbx.tag.buffer_size = sizeof(mailbox_generic) - sizeof(mailbox_tag);
    mbx.id = id;
    mbx.value = *value;

    if (!mailbox_process((mailbox_tag *)&mbx, sizeof(mbx))) {
        printf("FAILED TO PROCESS %X\n", tag_id);
    }

    *value = mbx.value;

    return true;
}

u32 mailbox_clock_rate(clock_type ct) {
    mailbox_clock c;

    c.tag.id = RFT_GET_CLOCK_RATE;
    c.tag.value_length = 0;
    c.tag.buffer_size = sizeof(c) - sizeof(c.tag);
    c.id = ct;

    mailbox_process((mailbox_tag *)&c, sizeof(c));

    return c.rate;
}

bool mailbox_power_check(u32 type) {
    mailbox_power p;
    p.tag.id = RFT_GET_DOMAIN_STATE;
    p.tag.value_length = 0;
    p.tag.buffer_size = sizeof(p) - sizeof(p.tag);
    p.id = type;
    p.state = ~0;

    mailbox_process((mailbox_tag *)&p, sizeof(p));

    return p.state && p.state != ~0;
}
