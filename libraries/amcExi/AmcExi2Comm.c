#include <stdio.h>
#include <dolphin/os.h>
#include <dolphin/amc/AmcExi2Comm.h> // from AMC DDH SDK

#ifdef __MWERKS__
extern u32 OS_PI_INTR_CAUSE[] : 0xCC003000;
#else
#define OS_PI_INTR_CAUSE ((u32 *)0xCC003000)
#endif

static u8 ucEXI2InputPending;
static u8* pucEXI2InputPending = &ucEXI2InputPending;
static s32 fExi2Selected;
static void (*TRK_Callback)(s32 chan, OSContext* context);

// extern functions for AmcExi.s
s32 AmcEXISelect();
s32 AmcEXIDeselect();
s32 AmcEXIImm(u32*, s32, s32, s32);
s32 AmcEXISync();
void AmcEXISetExiCallback(void*);
void AmcEXIEnableInterrupts(void);
void AmcEXIInit(void);

static void EXI2_SetTRKCallback(EXICallback monitorCallback) {
    BOOL intrEnable = OSDisableInterrupts();
    TRK_Callback = monitorCallback;
    OSRestoreInterrupts(intrEnable);
}

void EXI2_CallBack(u32 unused, OSContext *arg1) {
    *pucEXI2InputPending = 1;
    if (TRK_Callback) {
        TRK_Callback(0, arg1);
    }
}

static s32 EXI2_Select(s32 arg0, s32 unused) {
    s32 var_r31 = 0;

    while (var_r31 == 0) {
        var_r31 = AmcEXISelect(arg0);
    }
    return var_r31;
}

static void EXI2_ToWriteMode(void) {
    u32 sp8 = 0x80000000;

    AmcEXIImm(&sp8, 2, 1, 0);
    AmcEXISync();
}

static void EXI2_ToReadMode(u16 arg0) {
    u32 spC = arg0 << 0x10;

    AmcEXIImm(&spC, 2, 1, 0);
    AmcEXISync();
}

s32 EXI2_StartRead(void) {
    u32 sp8;
    s32 var_r31;

    if ((s32) fExi2Selected != 0) {
        var_r31 = 1;
    } else {
        if ((var_r31 = EXI2_Select(5, 0)) == 0) {
            printf("Can't select EXI2 port!\n");
            return 0;
        } else {
            fExi2Selected = 1;
            EXI2_ToReadMode(0);
            AmcEXIImm(&sp8, 2, 0, 0);
            AmcEXISync();
        }
    }

    return var_r31;
}

static void EXI2_FinishRead(void) {
    AmcEXIDeselect();
    fExi2Selected = 0;
    *pucEXI2InputPending = 0;
}

static s32 EXI2_SendCmd(u8 arg0, u32 arg1, s32 arg2, u32 arg3) {
    s32 temp_r31;
    u32 sp20;
    u32 sp1C;
    u32 sp18;

    sp20 = (arg0 << 24) | (arg1 & 0xFFFFFF);
    sp1C = arg2;
    temp_r31 = EXI2_Select(5, 0);

    EXI2_ToWriteMode();
    AmcEXIImm(&sp20, 4, 1, 0);
    AmcEXISync();
    AmcEXIImm(&sp1C, 4, 1, 0);
    AmcEXISync();
    // what is this.
    if (arg3 != 0U) {
        sp18 = arg3;
    }
    AmcEXIDeselect();
    return temp_r31;
}


void EXI2_Init( volatile unsigned char **inputPendingPtrRef, EXICallback monitorCallback ) {
    *inputPendingPtrRef = pucEXI2InputPending;
    EXI2_SetTRKCallback(monitorCallback);
    AmcEXIInit();
    EXI2_SendCmd(1, 3, 0, 0);
}

void EXI2_EnableInterrupts(void) {
    AmcEXISetExiCallback(&EXI2_CallBack);
    AmcEXIEnableInterrupts();
}

#ifndef NONMATCHING
static inline s32 EXI2_StartRead_hack(void) {
    u32 sp8[2];
    s32 var_r31;

    if ((s32) fExi2Selected != 0) {
        var_r31 = 1;
    } else {
        if ((var_r31 = EXI2_Select(5, 0)) == 0) {
            printf("Can't select EXI2 port!\n");
            return 0;
        } else {
            fExi2Selected = 1;
            EXI2_ToReadMode(0);
            AmcEXIImm(&sp8[1], 2, 0, 0);
            AmcEXISync();
        }
    }

    return var_r31;
}
#endif

int EXI2_Poll(void) {
    u32 sp18;

    if (!(OS_PI_INTR_CAUSE[0] & 0x1000) && (*pucEXI2InputPending == 0)) {
        sp18 = 0;
    } else {
#ifdef NONMATCHING
        EXI2_StartRead();
#else
        // should be calling EXI2_StartRead here, but I can't get the stack allocation to match in the inline call
        EXI2_StartRead_hack();
#endif
        AmcEXIImm(&sp18, 4, 0, 0);
        AmcEXISync();
        if (*pucEXI2InputPending == 0) {
            OS_PI_INTR_CAUSE[0] &= 0x1000;
            *pucEXI2InputPending = 1;
        }
    }
    return sp18;
}

AmcExiError	EXI2_ReadN( void *bytes, unsigned long length) {
    u8 *arg0 = (u8*)bytes;
    register u32 spC = length;
    register s32 res;
    register int j;
    register int i;
    s32 size;
    register int max;
    u32 sp10;

    if ((res = EXI2_StartRead()) == 0) {
        return 1;
    }

    size = (spC / 4) + ((spC % 4) ? 1 : 0);

    for(i = 0; i < spC; i += 4) {
        AmcEXIImm(&sp10, ((u32)(i / 4) < (size - 1)) ? 4 : ((spC & 3) + (spC & 1)) == 2U ? 2 : 4, 0, 0);
        AmcEXISync();

        if ((spC - i) >= 4) {
            max = 4;
        } else {
            max = (((s32)spC) % 4);
        }
        for(j = 0; j < max; j++) {
            arg0[i + j] = (sp10 >> ((3 - j) * 8));
        }
    }

    EXI2_FinishRead();
    return 0;
}

AmcExiError	EXI2_WriteN( const void *bytes, unsigned long length) {
    u32 sp14;
    u32 sp10;
    register s32 res;
    register u32 i;
    s32 unused_stack; // comment this out to match the debug version. the real match is
                      // probably something else, but not worth figuring out right now
    u32* arg0 = (u32*)bytes;
    u32 temp_r29;

    sp14 = length;
    if ((res = EXI2_Select(5, 0)) == 0) {
        return 1;
    }

    EXI2_ToWriteMode();
    AmcEXIImm(&sp14, 4, 1, 0);
    AmcEXISync();

    temp_r29 = ((sp14 >> 2U) + ((sp14 & 3) ? 1 : 0));

    for(i = 0; i < temp_r29; i++) {
        AmcEXIImm(arg0++, (i < (temp_r29 - 1)) ? 4 : ((u32) ((sp14 & 3) + (sp14 & 1)) == 2U ? 2 : 4), 1, 0);
        AmcEXISync();
    }
    AmcEXIDeselect();
    if ((res = EXI2_Select(5, 0)) == 0) {
        return 1;
    }
    EXI2_ToReadMode(1);
    AmcEXIImm(&sp10, 2, 0, 0);
    AmcEXISync();
    do {
        AmcEXIImm(&sp10, 2, 0, 0);
        AmcEXISync();
    } while (!((sp10 >> 0x10U) & 1));
    AmcEXIDeselect();
    return 0;
}

void EXI2_Reserve(void) {

}

void EXI2_Unreserve(void) {

}

AmcExiError EXI2_GetStatusReg( u16* pu16StatusReg ) {
    u32 spC;
    s32 result = 0;
    s32 unused_stack; // comment this out to match the debug version. the real match is
                      // probably something else, but not worth figuring out right now

    if (!(result = EXI2_Select(5, 0))) {
        return AMC_EXI_UNSELECTED;
    }

    EXI2_ToReadMode(1);
    AmcEXIImm(&spC, 2, 0, 0);
    AmcEXISync();
    AmcEXIImm(&spC, 2, 0, 0);
    AmcEXISync();

    *pu16StatusReg = (spC >> 16);
    AmcEXIDeselect();
    return AMC_EXI_NO_ERROR;
}
