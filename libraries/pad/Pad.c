#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <dolphin/vi.h>

#include <string.h>

#include <ppcintrinsic.h>

#include "../os/__os.h"
#include "../si/__si.h"

static void PADResetCallback(s32 chan, u32 error, OSContext *context);
static void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
static void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
static void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
static BOOL OnReset(BOOL);

#ifdef __MWERKS__
u16 __OSWirelessPadFixMode : 0x800030E0;
#else
#define __OSWirelessPadFixMode (*(u16 *)0x800030E0)
#endif

static u32 ResettingChan = 0x20;
static u32 XPatchBits = 0xF0000000;
static u32 AnalogMode = 0x300;
static u32 Spec = 5;
static void (*MakeStatus_)(s32 chan, PADStatus *, u32[2]) = SPEC2_MakeStatus; // Spec
static u32 cmdReadOrigin = 0x41000000;
static u32 cmdCalibrate = 0x42000000;

static BOOL Initialized;
static u32 EnabledBits;
static u32 ResettingBits;
static u32 ProbingBits;
static u32 RecalibrateBits;
static u32 WaitingBits;
static u32 CheckingBits;
static int cmdTypeAndStatus;
u32 __PADFixBits;
u32 __PADSpec;

u32 PADType[4];      void *PADType_force_order(void) { return PADType; }
u32 Type[4];         void *Type_force_order(void) { return Type; }
PADStatus Origin[4];
u32 CmdProbeDevice[4];

s32 GetWirelessID(s32 chan)
{
    OSSramEx *sramEx = __OSLockSramEx();
    s32 id = sramEx->wirelessPadID[chan];
    __OSUnlockSramEx(0);
    return id;
}

void SetWirelessID(s32 chan, u16 id)
{
    OSSramEx *sramEx = __OSLockSramEx();

    if (sramEx->wirelessPadID[chan] != id)
    {
        sramEx->wirelessPadID[chan] = id;
        __OSUnlockSramEx(1);
    }
    else
        __OSUnlockSramEx(0);
}

static void PADEnable(s32 chan)
{
    u32 cmd;
    u32 chanBit;
    u32 data[3];

    chanBit = PAD_CHAN0_BIT >> chan;
    EnabledBits |= chanBit;
    SIGetResponse(chan, data + 1);
    cmd = (0x40 << 16) | AnalogMode;
    SISetCommand(chan, cmd);
    SIEnablePolling(EnabledBits);
}

// like PADEnable, but with stack differences.
static void PADEnable0(s32 chan)
{
    u32 cmd;
    u32 chanBit;
    u32 data[3];

    chanBit = PAD_CHAN0_BIT >> chan;
    EnabledBits |= chanBit;
    SIGetResponse(chan, data + 0);
    cmd = (0x40 << 16) | AnalogMode;
    SISetCommand(chan, cmd);
    SIEnablePolling(EnabledBits);
}

static void PADDisable(s32 chan)
{
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();

    chanBit = PAD_CHAN0_BIT >> chan;
    SIDisablePolling(chanBit);
    EnabledBits &= ~chanBit;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;
    ProbingBits &= ~chanBit;
    SetWirelessID(chan, 0);

    OSRestoreInterrupts(enabled);
}

s32 DoReset(void)
{
    s32 var_r3;
    PADStatus *temp_r6;

    var_r3 = 1;
    ResettingChan = __cntlzw(ResettingBits);
    if (((s32)ResettingChan >= 0) && ((s32)ResettingChan < 4))
    {
        memset(&Origin[ResettingChan], 0, 0xCU);
        Type[ResettingChan] = 0;
        PADType[ResettingChan] = 0;
        var_r3 = SITransfer(ResettingChan, &cmdTypeAndStatus, 1, &Type[ResettingChan], 3, PADResetCallback, 0);
        ResettingBits &= ~(PAD_CHAN0_BIT >> ResettingChan);
        if (var_r3 == 0)
        {
            ResettingBits = 0;
            ResettingChan = 0x20;
        }
    }
    return var_r3;
}

static void ProbeWireless(u32 chan)
{
    u32 sp18[3];
    s32 var_r4;
    u32 temp_r5;

    temp_r5 = PAD_CHAN0_BIT >> chan;
    EnabledBits |= temp_r5;
    ProbingBits |= temp_r5;
    SIGetResponse(chan, sp18 + 1);
    if (!(Type[chan] & 0x02000000))
        var_r4 = (chan << 0xE) | 0x4D0000 | (*(u16 *)0x800030E0U & 0x3FFF);
    else if ((u32)((Type[chan] & 0xC0000) + 0xFFFC0000) == 0U)
        var_r4 = 0x500000;
    else
        var_r4 = (Type[chan] & 0x70000) + 0x440000;
    SISetCommand(chan, var_r4);
    SIEnablePolling(EnabledBits);
}

static void PADProbeCallback(s32 chan, u32 error, OSContext *context)
{
    if (!(error & (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_NO_RESPONSE | SI_ERROR_COLLISION)))
    {
        if (!(Type[chan] & 0x80000) && !(Type[chan] & 0x40000))
        {
            PADEnable0(ResettingChan);
            WaitingBits |= PAD_CHAN0_BIT >> ResettingChan;
        }
        else
            ProbeWireless(ResettingChan);
    }
    DoReset();
}

static void UpdateOrigin(s32 chan)
{
    PADStatus *origin;
    u32 chanBit = PAD_CHAN0_BIT >> chan;

    origin = &Origin[chan];
    switch (AnalogMode & 0x00000700u)
    {
    case 0x00000000u:
    case 0x00000500u:
    case 0x00000600u:
    case 0x00000700u:
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000100u:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000200u:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        break;
    case 0x00000300u: break;
    case 0x00000400u: break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (XPatchBits & chanBit)
    {
        if (64 < origin->stickX && (Type[chan] & 0xffff0000) == SI_GC_CONTROLLER)
            origin->stickX = 0;
    }
}

static void PADOriginCallback(s32 chan, u32 error, OSContext *context)
{
    if (!(error & (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_NO_RESPONSE | SI_ERROR_COLLISION)))
    {
        UpdateOrigin(ResettingChan);
        PADEnable(ResettingChan);
    }
    DoReset();
}

static void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context)
{
    if (!(EnabledBits & (PAD_CHAN0_BIT >> chan)))
        return;
    if (!(error & (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_NO_RESPONSE | SI_ERROR_COLLISION)))
        UpdateOrigin(chan);
}

void PADFixCallback(s32 arg0, u32 arg1, struct OSContext *arg2)
{
    s32 temp_r29;
    u32 temp_r30_2;
    u32 r4;

    if (!(arg1 & 0xF))
    {
        temp_r29 = Type[ResettingChan];
        temp_r30_2 = GetWirelessID(ResettingChan); // inlined
        r4 = temp_r30_2 << 8;
        if (!(temp_r29 & 0x100000) || ((u32)(r4 & 0xCFFF00) != (u32)(temp_r29 & 0xCFFF00)))
            DoReset();
        else
        {
            if ((temp_r29 & 0x40000000) && !(temp_r29 & 0x80000) && !(temp_r29 & 0x40000))
            {
                SITransfer((s32)ResettingChan, &cmdReadOrigin, 1U, &Origin[ResettingChan], 0xAU, PADOriginCallback, 0);
                return;
            }
            SITransfer((s32)ResettingChan, &CmdProbeDevice[ResettingChan], 3U, &Origin[ResettingChan], 8U, PADProbeCallback, 0);
        }
    }
    else
        DoReset();
}

u32 bss_60[4];

#ifdef NONMATCHING
static void PADResetCallback(s32 chan, u32 error, OSContext *context)
{
    u32 sp38;
    u32 sp1C;
    s32 temp_r6;
    u32 temp_r7;
    u32 var_r29;
    u32 temp_r28;
    u32 r27;
    u32 r26;
    u32 new_var;
    if ((error & 0xF) != 0)
    {
        Type[ResettingChan] = 0;
    }
    temp_r28 = Type[ResettingChan];
    temp_r7 = PAD_CHAN0_BIT >> ResettingChan;
    temp_r6 = ~temp_r7;
    PADType[ResettingChan] = temp_r28;
    new_var = temp_r28;
    r27 = RecalibrateBits & temp_r7;
    RecalibrateBits &= temp_r6;
    r26 = __PADFixBits & temp_r7;
    __PADFixBits &= temp_r6;
    if (((error & 0xF) != 0) || (((u32)((temp_r28 & 0x18000000) + 0xF8000000)) != 0U))
    {
        SetWirelessID(ResettingChan, 0);
        DoReset();
    }
    else if (((u32)Spec) < 2U)
    {
        PADEnable(ResettingChan);
        DoReset();
    }
    else if ((!(temp_r28 & 0x80000000)) || (new_var & 0x04000000))
    {
        SetWirelessID(ResettingChan, 0);
        if (!(temp_r28 & 0x01000000))
        {
            DoReset();
        }
        else
        {
            if (r27)
            {
                SITransfer((s32)ResettingChan, &cmdCalibrate, 3U, &Origin[ResettingChan], 0xAU, PADOriginCallback, 0);
                return;
            }
            SITransfer((s32)ResettingChan, &cmdReadOrigin, 1U, &Origin[ResettingChan], 0xAU, PADOriginCallback, 0);
        }
    }
    else
    {
        r27 = GetWirelessID(ResettingChan);
        var_r29 = r27 << 8;
        if ((((s32)r26) != 0) && (var_r29 & 0x100000))
        {
            bss_60[ResettingChan] = (s32)((var_r29 & 0xCFFF00) | 0x4E100000);
            SITransfer((s32)ResettingChan, &bss_60[ResettingChan], 3U, &Type[ResettingChan], 3U, PADFixCallback, 0);
            return;
        }
        if (new_var & 0x100000)
        {
            if (((u32)(var_r29 & 0xCFFF00)) != (temp_r28 & 0xCFFF00))
            {
                if (!(var_r29 & 0x100000))
                {
                    var_r29 = new_var & 0xCFFF00;
                    var_r29 |= 0x100000;
                    SetWirelessID(ResettingChan, var_r29 >> 8U);
                }
                bss_60[ResettingChan] = (s32)(var_r29 | 0x4E000000);
                SITransfer((s32)ResettingChan, &bss_60[ResettingChan], 3U, &Type[ResettingChan], 3U, PADFixCallback, 0);
                return;
            }
            if (((temp_r28 & 0x40000000) && (!(temp_r28 & 0x80000))) && (!(new_var & 0x40000)))
            {
                SITransfer((s32)ResettingChan, &cmdReadOrigin, 1U, &Origin[ResettingChan], 0xAU, PADOriginCallback, 0);
                return;
            }
            SITransfer((s32)ResettingChan, &CmdProbeDevice[ResettingChan], 3U, &Origin[ResettingChan], 8U, PADProbeCallback, 0);
            return;
        }
        if (new_var & 0x40000000)
        {
            r27 = new_var & 0xCFFF00;
            r27 |= 0x100000;
            SetWirelessID(ResettingChan, r27 >> 8);
            bss_60[ResettingChan] = (s32)(r27 | 0x4E000000);
            SITransfer((s32)ResettingChan, &bss_60[ResettingChan], 3U, &Type[ResettingChan], 3U, PADFixCallback, 0);
            return;
        }
        SetWirelessID(ResettingChan, 0);
        ProbeWireless(ResettingChan);
        DoReset();
    }
}
#else
asm void PADResetCallback(s32 chan, u32 error, OSContext *context){nofralloc
#include "../asm/nonmatchings/PADResetCallback.s"
}
#pragma peephole on
#endif

BOOL PADReset(u32 mask)
{
    BOOL intrEnabled;
    BOOL ret;
    u32 unused;

    ret = FALSE;
    intrEnabled = OSDisableInterrupts();
    mask &= ~(ProbingBits | WaitingBits | CheckingBits);
    ResettingBits |= mask;
    EnabledBits &= ~mask;
    WaitingBits &= ~mask;
    if (Spec == 4)
        RecalibrateBits |= mask;
    SIDisablePolling(ResettingBits);
    if ((s32)ResettingChan == 32)
        ret = DoReset();
    OSRestoreInterrupts(intrEnabled);
    return ret;
}

BOOL PADRecalibrate(u32 mask)
{
    BOOL intrEnabled;
    BOOL ret;
    u32 unused;

    ret = FALSE;
    intrEnabled = OSDisableInterrupts();
    mask &= ~(CheckingBits | (ProbingBits | WaitingBits));
    ResettingBits |= mask;
    EnabledBits &= ~mask;
    RecalibrateBits |= mask;
    SIDisablePolling(ResettingBits);
    if ((s32)ResettingChan == 32)
        ret = DoReset();
    OSRestoreInterrupts(intrEnabled);
    return ret;
}

static OSResetFunctionInfo ResetFunctionInfo = {
    OnReset,
    127,
    NULL,
    NULL,
};

BOOL PADInit()
{
    s32 chan;

    if (!Initialized)
    {
        if (__PADSpec)
            PADSetSpec(__PADSpec);

        if (__PADFixBits == 0xFFFFFFFF)
        {
            OSTime time = OSGetTime();
            __OSWirelessPadFixMode
                = (u16)((((time)&0xffff) + ((time >> 16) & 0xffff) + ((time >> 32) & 0xffff) + ((time >> 48) & 0xffff))
                        & 0x3fffu);
        }

        for (chan = 0; chan < SI_MAX_CHAN; ++chan)
            CmdProbeDevice[chan] = (0x4D << 24) | (chan << 22) | ((__OSWirelessPadFixMode & 0x3fffu) << 8);
        Initialized = TRUE;

        PADSetSamplingRate(0);
        OSRegisterResetFunction(&ResetFunctionInfo);
    }
    return PADReset(PAD_CHAN0_BIT | PAD_CHAN1_BIT | PAD_CHAN2_BIT | PAD_CHAN3_BIT);
}

static void PADReceiveCheckCallback(s32 chan, u32 type, OSContext *arg2)
{
    u32 chanBit;
    u32 temp_r6;

    temp_r6 = Type[chan];
    chanBit = PAD_CHAN0_BIT >> chan;

    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;

    if (!(EnabledBits & chanBit))
        return;

    if (!(type & 0xF) && (temp_r6 & 0x80000000) && (temp_r6 & 0x02000000) && (temp_r6 & 0x40000000)
        && !(temp_r6 & 0x04000000))
        SITransfer(chan, &cmdReadOrigin, 1, &Origin[chan], 10, PADOriginUpdateCallback, 0);
    else
        PADDisable(chan);
}

u32 PADRead(PADStatus *status)
{
    BOOL enabled;
    s32 chan;
    u32 chanBit;
    int chanShift;
    u32 motor;
    u32 cmd;
    u32 data[2];

    motor = 0;
    for (chan = 0; chan < SI_MAX_CHAN; chan++, status++)
    {
        chanBit = PAD_CHAN0_BIT >> chan;
        chanShift = 8 * (SI_MAX_CHAN - 1 - chan);

        if ((ResettingBits & chanBit) || (s32)ResettingChan == chan)
        {
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (!(EnabledBits & chanBit))
        {
            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        cmd = SIGetStatus();
        if (cmd & (8 << chanShift))
        {
            if (WaitingBits & chanBit)
            {
                status->err = PAD_ERR_NONE;
                memset(status, 0, offsetof(PADStatus, err));
                if (CheckingBits & chanBit)
                    continue;
                enabled = OSDisableInterrupts();
                if (SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, PADReceiveCheckCallback, 0))
                    CheckingBits |= chanBit;
                OSRestoreInterrupts(enabled);
                continue;
            }
            PADDisable(chan);
            status->err = PAD_ERR_NO_CONTROLLER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (!(ProbingBits & chanBit) && !(Type[chan] & 0x20000000))
            motor |= chanBit;

        if (!(cmd & (0x20 << chanShift)))
        {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        SIGetResponse(chan, data);
        if (data[0] & 0x80000000)
        {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (ProbingBits & chanBit)
        {
            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        MakeStatus_(chan, status, data);

        // Check and clear PAD_ORIGIN bit
        if (status->button & 0x2000)
        {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));

            // Get origin. It is okay if the following transfer fails
            // since the PAD_ORIGIN bit remains until the read origin
            // command complete.
            SITransfer(chan, &cmdReadOrigin, 1, &Origin[chan], 10, PADOriginUpdateCallback, 0);
            continue;
        }

        status->err = PAD_ERR_NONE;

        // Clear PAD_INTERFERE bit
        status->button &= ~0x0080;
    }

    return motor;
}

static u8 data10[] = {
    0xF7, 0x02,
    0x0E, 0x13,
    0x1D, 0x09,
    0x25, 0x07,
    0x34, 0x05,
    0x41, 0x04,
    0x57, 0x03,
    0x57, 0x03,
    0x57, 0x03,
    0x83, 0x02,
    0x83, 0x02,
    0x83, 0x02,
};

static u8 data28[] = {
    0x94, 0x03,
    0x0D, 0x18,
    0x1A, 0x0C,
    0x27, 0x08,
    0x34, 0x06,
    0x3E, 0x05,
    0x4E, 0x04,
    0x68, 0x03,
    0x68, 0x03,
    0x68, 0x03,
    0x68, 0x03,
    0x9C, 0x02,
};

void PADSetSamplingRate(u32 msec)
{
    u8 *ptr;

    if (msec > 11)
        msec = 11;
    switch (VIGetTvFormat())
    {
    case VI_NTSC:
    case VI_MPAL:
        ptr = data10;
        break;
    case VI_PAL:
        ptr = data28;
        break;
    default:
        OSPanic("Pad.c", 0x510, "PADSetSamplingRate: unknown TV format");
        break;
    }
    SISetXY(ptr[msec * 2 + 0], ptr[msec * 2 + 1]);
    SIEnablePolling(EnabledBits);
}

void PADControlAllMotors(const u32 *commandArray)
{
    BOOL enabled;
    int chan;
    u32 command;
    BOOL commit;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    commit = FALSE;

    for (chan = 0; chan < SI_MAX_CHAN; chan++, commandArray++)
    {
        chanBit = PAD_CHAN0_BIT >> chan;
        if ((EnabledBits & chanBit) && !(ProbingBits & chanBit) && !(Type[chan] & 0x20000000))
        {
            command = *commandArray;
            if (Spec < PAD_SPEC_2 && command == PAD_MOTOR_STOP_HARD)
                command = PAD_MOTOR_STOP;
            SISetCommand(chan, (0x40 << 16) | AnalogMode | (command & (0x00000001 | 0x00000002)));
            commit = TRUE;
        }
    }
    if (commit)
        SITransferCommands();
    OSRestoreInterrupts(enabled);
}

void PADControlMotor(s32 chan, u32 command)
{
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = PAD_CHAN0_BIT >> chan;
    if ((EnabledBits & chanBit) && !(ProbingBits & chanBit) && !(Type[chan] & 0x20000000))
    {
        if (Spec < PAD_SPEC_2 && command == PAD_MOTOR_STOP_HARD)
            command = PAD_MOTOR_STOP;
        SISetCommand(chan, (0x40 << 16) | AnalogMode | (command & (0x00000001 | 0x00000002)));
        SITransferCommands();
    }
    OSRestoreInterrupts(enabled);
}

void PADSetSpec(u32 spec)
{
    __PADSpec = 0;
    switch (spec)
    {
    case PAD_SPEC_0:
        MakeStatus_ = SPEC0_MakeStatus;
        break;
    case PAD_SPEC_1:
        MakeStatus_ = SPEC1_MakeStatus;
        break;
    case PAD_SPEC_2:
    case PAD_SPEC_3:
    case PAD_SPEC_4:
    case PAD_SPEC_5:
        MakeStatus_ = SPEC2_MakeStatus;
        break;
    }
    Spec = spec;
}

static void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2])
{
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0008) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0001) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_START : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2])
{

    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0080) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0200) ? PAD_BUTTON_START : 0;

    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);

    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];

    status->analogA = 0;
    status->analogB = 0;

    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static s8 ClampS8(s8 var, s8 org)
{
    if (0 < org)
    {
        s8 min = (s8)(-128 + org);
        if (var < min)
            var = min;
    }
    else if (org < 0)
    {
        s8 max = (s8)(127 + org);
        if (max < var)
            var = max;
    }
    return var -= org;
}

static u8 ClampU8(u8 var, u8 org)
{
    if (var < org)
        var = org;
    return var -= org;
}

#define PAD_ALL                                                                                                        \
    (PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT | PAD_BUTTON_DOWN | PAD_BUTTON_UP | PAD_TRIGGER_Z | PAD_TRIGGER_R              \
     | PAD_TRIGGER_L | PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y | PAD_BUTTON_MENU | 0x2000 | 0x0080)

static void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2])
{
    PADStatus *origin;

    status->button = (u16)((data[0] >> 16) & PAD_ALL);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);

    switch (AnalogMode & 0x00000700)
    {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0f) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0f) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    case 0x00000300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1] >> 0);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x00000400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    origin = &Origin[chan];
    status->stickX = ClampS8(status->stickX, origin->stickX);
    status->stickY = ClampS8(status->stickY, origin->stickY);
    status->substickX = ClampS8(status->substickX, origin->substickX);
    status->substickY = ClampS8(status->substickY, origin->substickY);
    status->triggerLeft = ClampU8(status->triggerLeft, origin->triggerLeft);
    status->triggerRight = ClampU8(status->triggerRight, origin->triggerRight);
}

BOOL PADSync(void)
{
    return ResettingBits == 0 && (s32)ResettingChan == 32 && !SIBusy();
}

void PADSetAnalogMode(u32 mode)
{
    BOOL enabled;
    u32 mask;

    enabled = OSDisableInterrupts();
    AnalogMode = mode << 8;
    mask = EnabledBits & ~ProbingBits;

    EnabledBits &= ~mask;
    WaitingBits &= ~mask;
    CheckingBits &= ~mask;

    SIDisablePolling(mask);
    OSRestoreInterrupts(enabled);
}

static BOOL OnReset(BOOL f)
{
    BOOL sync;
    static BOOL recalibrated = FALSE;

    if (!f)
    {
        sync = PADSync();
        if (!recalibrated && sync)
        {
            recalibrated = PADRecalibrate(PAD_CHAN0_BIT | PAD_CHAN1_BIT | PAD_CHAN2_BIT | PAD_CHAN3_BIT);
            return FALSE;
        }
        return sync;
    }
    else
        recalibrated = FALSE;

    return TRUE;
}
