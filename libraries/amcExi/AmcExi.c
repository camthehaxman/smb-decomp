#include <string.h>
#include <dolphin.h>
#include <dolphin/amc/AmcExi2Comm.h>
#include <dolphin/hw_regs.h>

#include "__amcExi.h"

extern s32 __OSMaskInterrupts(s32);

struct Ecb {
    EXICallback exiCallback;
    EXICallback exiCallback2;
    char filler8[0x4];
    volatile u32 state;
    u32 unk10;
    u8 *unk14;
};

static struct Ecb Ecb;
static struct Ecb *exi = &Ecb;

void AmcEXIClearInterrupts(s32 arg0, s32 arg1);

#define STATE_BUSY 3
#define STATE_SELECTED 4
#define MAX_IMM 4
#define MAX_TYPE 3

static void CompleteTransfer(void) {
    register u8* var_r30;
    register u32 c;
    register s32 i;
    s32 temp_r28;

    if (exi->state & 3) {
        if (exi->state & 2) {
            temp_r28 = exi->unk10;
            if (temp_r28 != 0) {
                var_r30 = exi->unk14;
                c = OS_EXI[14];

                for(i = 0; i < temp_r28; i++) {
                    *var_r30++ = (c >> ((3 - i) * 8));
                }
            }
        }
        exi->state &= 0xFFFFFFFC;
    }
}

s32 AmcEXIImm(void *buf, s32 len, u32 type, EXICallback func) {
    int intrEnable;
    int var_r26;
    int i;
    int c = 2;

    ASSERTLINE(exi->state & STATE_SELECTED, 140);
    ASSERTLINE(0 < len && len <= MAX_IMM, 141);
    ASSERTLINE(type < MAX_TYPE, 142);
    
    intrEnable = OSDisableInterrupts();
    if ((exi->state & 3) || !(exi->state & STATE_SELECTED)) {
        OSRestoreInterrupts(intrEnable);
        return 0;
    }
    
    exi->exiCallback2 = func;
    if ((u32) exi->exiCallback2 != 0U) {
        AmcEXIClearInterrupts(0, 1);
        __OSUnmaskInterrupts(0x200000U >> (c * 3));
    }
    exi->state |= 2;
    if (type != 0) {
        var_r26 = 0;
        
        for(i = 0; i < len; i++) {
            var_r26 |= (int)((u8*)buf)[i] << ((3 - i) * 8);
        }
        OS_EXI[14] = var_r26;
    }
    else {
        
    }
    exi->unk14 = buf;
    exi->unk10 = (type != 1) ? len : 0;
    OS_EXI[13] = (type * 4) | 1 | ((len - 1) * 0x10);
    OSRestoreInterrupts(intrEnable);
    return 1;
}

#define EXI_0LENGTH_EXILENGTH_MASK 0x03FFFFE0

#define EXI_READ 0
#define EXI_WRITE 1

s32 AmcEXIDma(s32 buf, s32 len, u32 type, EXICallback func) {
    s32 intrEnable;
    u32 c = 2;

    ASSERTLINE(exi->state & STATE_SELECTED, 208);
    ASSERTLINE(OFFSET(buf, 32) == 0, 209);
    ASSERTLINE(0 < len && OFFSET(len, 32) == 0, 210);
    ASSERTLINE(((u32) len & ~EXI_0LENGTH_EXILENGTH_MASK) == 0, 212);
    ASSERTLINE(type == EXI_READ || type == EXI_WRITE, 214);  // TODO: check this

    intrEnable = OSDisableInterrupts();
    if ((exi->state & 3) || !(exi->state & 4)) {
        OSRestoreInterrupts(intrEnable);
        return 0;
    }
    exi->exiCallback2 = func;
    if (exi->exiCallback2 != NULL) {
        AmcEXIClearInterrupts(0, 1);
        __OSUnmaskInterrupts(0x200000U >> (c * 3));
    }
    exi->state |= 1;
    OS_EXI[11] = buf & 0x03FFFFE0;
    OS_EXI[12] = len;
    OS_EXI[13] = (type * 4) | 3;
    OSRestoreInterrupts(intrEnable);
    return 1;
}

void AmcEXISync(void) {
    while (exi->state & 4) {
        if(!(OS_EXI[13] & 1)) {
            s32 intrEnable = OSDisableInterrupts();
            CompleteTransfer();
            OSRestoreInterrupts(intrEnable);
            break;
        }
    }

    ASSERTLINE(!(exi->state & STATE_BUSY), 270);
}

void AmcEXIClearInterrupts(s32 arg0, s32 arg1) {
    u32 exi_tmp;
    u32 cause_tmp;
    
    if (arg0 != 0) {
        cause_tmp = OS_PI_INTR_CAUSE[0];
        cause_tmp &= 0x1000;
        cause_tmp |= 0x1000;
        OS_PI_INTR_CAUSE[0] = cause_tmp;
    }
    if (arg1 != 0) {
        exi_tmp = OS_EXI[10];
        exi_tmp &= 0x7F5;
        exi_tmp |= 8;
        OS_EXI[10] = exi_tmp;
    }
}

void *AmcEXISetExiCallback(EXICallback func) {
    EXICallback exiCallback;
    s32 intrEnable;

    intrEnable = OSDisableInterrupts();
    if (func != 0) {
        __OSUnmaskInterrupts(0x40);
    } else {
        __OSMaskInterrupts(0x40);
    }
    exiCallback = exi->exiCallback;
    exi->exiCallback = func;
    OSRestoreInterrupts(intrEnable);
    return exiCallback;
}

#define MAX_FREQ 6

s32 AmcEXISelect(u32 freq) {
    s32 intrEnable;
    int c;

    ASSERTLINE(freq < MAX_FREQ, 387);
    ASSERTLINE(!(exi->state & STATE_SELECTED), 388);

    intrEnable = OSDisableInterrupts();
    if (exi->state & 4) {
        OSRestoreInterrupts(intrEnable);
        return 0;
    }
    exi->state |= 4;
    c = OS_EXI[10];
    c &= 0x405;
    c |= ((freq * 0x10) | 0x80);
    OS_EXI[10] = c;
    OSRestoreInterrupts(intrEnable);
    return 1;
}

s32 AmcEXIDeselect(void) {
    s32 intrEnable = OSDisableInterrupts();
    register int c;

    if (!(exi->state & 4)) {
        OSRestoreInterrupts(intrEnable);
        return 0;
    }
    exi->state &= 0xFFFFFFFB;
    c = OS_EXI[10];
    c &= 0x405;
    OS_EXI[10] = c;
    OSRestoreInterrupts(intrEnable);
    return 1;
}

void AmcTCIntrruptHandler(__OSInterrupt intr, OSContext *ctx) {
    s32 chan = ((intr - 10) / 3);
    EXICallback exiCallback2;

    ASSERTLINE(chan == 2, 458);

    __OSMaskInterrupts(0x80000000U >> intr);
    AmcEXIClearInterrupts(0, 1);
    exiCallback2 = exi->exiCallback2;
    if (exiCallback2 != NULL) {
        exi->exiCallback2 = 0;
        CompleteTransfer();
        exiCallback2(chan, ctx);
    }
}

void AmcDebugIntHandler(__OSInterrupt intr, OSContext *ctx) {
    EXICallback exiCallback;

    AmcEXIClearInterrupts(1, 0);
    exiCallback = exi->exiCallback;
    if (exiCallback != NULL) {
        exiCallback(2, ctx);
    }
}

void AmcEXIEnableInterrupts(void) {
    __OSMaskInterrupts(0x8000);
    __OSSetInterruptHandler(__OS_INTERRUPT_PI_DEBUG, AmcDebugIntHandler);
    __OSUnmaskInterrupts(0x40);
}

void AmcEXIInit(void) {
    memset(&Ecb, 0, sizeof(Ecb));
    OS_EXI[10] = 0;
    OS_SI[15] = 0;
}
