#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ppcintrinsic.h>

#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "input.h"
#include "mode.h"
#include "pool.h"
#include "vibration.h"

struct ControllerInfo controllerInfo[4];
struct ControllerInfo lbl_801F3C60[4];
struct AnalogInput analogInputs[4];
s32 controllerRepeatCounts[4];
u16 g_currPlayerButtons[6];
u16 g_currPlayerAnalogButtons[6];

FORCE_BSS_ORDER(controllerInfo);
FORCE_BSS_ORDER(lbl_801F3C60);
FORCE_BSS_ORDER(analogInputs);
FORCE_BSS_ORDER(controllerRepeatCounts);
FORCE_BSS_ORDER(g_currPlayerButtons);
FORCE_BSS_ORDER(g_currPlayerAnalogButtons);

static u8 resetCounter;
static u8 lbl_802F1CD1;
static u8 initialized;
static u32 lbl_802F1CD4;
static u32 lbl_802F1CD8;

s8 padLimit[32][2] =
{
    { 0x54, 0x00 },
    { 0x3B, 0x3B },
    { 0x00, 0x54 },
    { 0xC5, 0x3B },
    { 0xAC, 0x00 },
    { 0xC5, 0xC5 },
    { 0x00, 0xAC },
    { 0x3B, 0xC5 },
    { 0x54, 0x00 },
    { 0x3B, 0x3B },
    { 0x00, 0x54 },
    { 0xC5, 0x3B },
    { 0xAC, 0x00 },
    { 0xC5, 0xC5 },
    { 0x00, 0xAC },
    { 0x3B, 0xC5 },
    { 0x54, 0x00 },
    { 0x3B, 0x3B },
    { 0x00, 0x54 },
    { 0xC5, 0x3B },
    { 0xAC, 0x00 },
    { 0xC5, 0xC5 },
    { 0x00, 0xAC },
    { 0x3B, 0xC5 },
    { 0x54, 0x00 },
    { 0x3B, 0x3B },
    { 0x00, 0x54 },
    { 0xC5, 0x3B },
    { 0xAC, 0x00 },
    { 0xC5, 0xC5 },
    { 0x00, 0xAC },
    { 0x3B, 0xC5 },
};

const s8 lbl_80110320[100] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03,
	0x03, 0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07,
	0x07, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B, 0x0C, 0x0C, 0x0D,
	0x0E, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x13, 0x14, 0x15,
	0x16, 0x16, 0x17, 0x18, 0x19, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1D, 0x1E,
	0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
	0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3C,
};

const s8 lbl_80110320_64[8][2] =
{
	{ 0x69, 0x00 },
    { 0x69, 0x69 },
    { 0x00, 0x69 },
    { 0x97, 0x69 },
    { 0x97, 0x00 },
    { 0x97, 0x97 },
	{ 0x00, 0x97 },
    { 0x69, 0x97 },
};

const s8 lbl_80110320_74[4][8][2] =
{
    {
        { 0x50, 0x00 },
        { 0x38, 0x38 },
        { 0x00, 0x50 },
        { 0xC8, 0x38 },
        { 0xB0, 0x00 },
        { 0xC8, 0xC8 },
        { 0x00, 0xB0 },
        { 0x38, 0xC8 },
    },
    {
        { 0x50, 0x00 },
        { 0x38, 0x38 },
        { 0x00, 0x50 },
        { 0xC8, 0x38 },
        { 0xB0, 0x00 },
        { 0xC8, 0xC8 },
        { 0x00, 0xB0 },
        { 0x38, 0xC8 },
    },
    {
        { 0x50, 0x00 },
        { 0x38, 0x38 },
        { 0x00, 0x50 },
        { 0xC8, 0x38 },
        { 0xB0, 0x00 },
        { 0xC8, 0xC8 },
        { 0x00, 0xB0 },
        { 0x38, 0xC8 },
    },
    {
        { 0x50, 0x00 },
        { 0x38, 0x38 },
        { 0x00, 0x50 },
        { 0xC8, 0x38 },
        { 0xB0, 0x00 },
        { 0xC8, 0xC8 },
        { 0x00, 0xB0 },
        { 0x38, 0xC8 },
    },
};

void input_init(void)
{
    if (initialized == 0)
    {
        PADInit();
        PADSetAnalogMode(3);
        initialized++;
    }
    memset(controllerInfo, 0, sizeof(controllerInfo));
    resetCounter = 0;
    lbl_802F1CD1 = 0;
}

static inline int clamp(int val, int min, int max)
{
    if (val < min)
        return min;
    else if (val > max)
        val = max;
    return val;
}

#ifdef NONMATCHING
// https://decomp.me/scratch/EAwn5
static inline void test3(struct ControllerInfo *r6, PADStatus *b)
{
    r6->held = *b;
    r6->pressed.button = b->button & ~r6->prevHeld.button;
    r6->released.button = r6->prevHeld.button & ~b->button;
}

void input_main(void)
{
    int r0;
    int i;  // r29
    int r3;

    PADStatus sp10[4];
    struct CoordsS8 spC;

    func_80025158(sp10);
    handle_reset_input(sp10);

    /*
    for (r6 = &lbl_801F3C60[0], i = 0; i < 4; i++, r6++)
    {
        r6->unkC.button = r6->unk0.button;
        r6->unk0 = sp10[i];
        r6->unk18.button = sp10[i].button & ~r6->unkC.button;
        r6->unk24.button = r6->unkC.button & ~sp10[i].button;
    }
    */

    for (i = 0; i < 4; i++)
    {
        lbl_801F3C60[i].prevHeld.button = lbl_801F3C60[i].held.button;
        test3(&lbl_801F3C60[i], &sp10[i]);
    }

    /*
    for (i = 0; i < 4; i++)
    {
        lbl_801F3C60[i].prevHeld.button = lbl_801F3C60[i].held.button;
        lbl_801F3C60[i].held = sp10[i];
        lbl_801F3C60[i].pressed.button = sp10[i].button & ~lbl_801F3C60[i].prevHeld.button;
        lbl_801F3C60[i].released.button = lbl_801F3C60[i].prevHeld.button & ~sp10[i].button;
    }
    */
    // r31 = sp10[i]
    for (i = 0; i < 4; i++)
    {
        controllerInfo[i].prevHeld.button = controllerInfo[i].held.button;

        sp10[i].triggerLeft = clamp(sp10[i].triggerLeft - 16, 0, 0xB4) * 0.77777779102325439f;
        sp10[i].triggerRight = clamp(sp10[i].triggerRight - 16, 0, 0xB4) * 0.77777779102325439f;

        spC.x = sp10[i].stickX;
        spC.y = sp10[i].stickY;
        func_80025B1C(&spC, padLimit[i]);
        sp10[i].stickX = spC.x;
        sp10[i].stickY = spC.y;

        // TODO: separate inline function: calc_input_square
        for (r3 = 0; r3 < 8; r3++)
        {
            float f5;
            float f2;
            float f6;
            float f3;

            float f4;

            int r7;
            int r8;
            int r6_;
            int r5;

            if (r3 == 7)
                r0 = 0;
            else
                r0 = r3 + 1;

            f5 = lbl_80110320_74[i][r3][0];
            f2 = lbl_80110320_74[i][r3][1];
            f6 = lbl_80110320_74[i][r0][0];
            f3 = lbl_80110320_74[i][r0][1];

            f4 = (float)lbl_80110320_74[i][r3][0] * (float)lbl_80110320_74[i][r0][1]
               - (float)lbl_80110320_74[i][r0][0] * (float)lbl_80110320_74[i][r3][1];

            if (fabs(f4) < FLT_EPSILON)
                continue;

            f4 = 1.0f / f4;

            f3 = f4 * (f3 * sp10[i].substickX - f6 * sp10[i].substickY);
            f2 = f4 * (-f2 * sp10[i].substickX + f5 * sp10[i].substickY);

            if (f3 < 0.0f || f2 < 0.0f)
                continue;

            r8 = (int)(f3 * lbl_80110320_64[r3][0] + f2 * lbl_80110320_64[r0][0]);
            r7 = r8 < 0;
            r6_ = clamp(abs(r8), 0, 99);
            //lbl_80024FF4
            r5 = r7 ? -1 : 1;
            //lbl_80025008
            sp10[i].substickX = r5 * lbl_80110320[r6_];

            r8 = (int)(f3 * lbl_80110320_64[r3][1] + f2 * lbl_80110320_64[r0][1]);
            r7 = r8 < 0;
            r6_ = clamp(abs(r8), 0, 99);
            r5 = r7 ? -1 : 1;
            sp10[i].substickY = r5 * lbl_80110320[r6_];
            break;
        }
        //lbl_800250D0
        /*
        controllerInfo[i].unk0 = sp10[i];
        controllerInfo[i].unk18.button = sp10[i].button & ~controllerInfo[i].unkC.button;
        controllerInfo[i].unk24.button = controllerInfo[i].unkC.button & ~sp10[i].button;
        */
        test3(&controllerInfo[i], &sp10[i]);
    }
    handle_analog_inputs();
    get_key_repeats();
    func_80025640();
}
#else
float  force_lbl_802F30A8(void) { return 0.77777779102325439f; }
double force_lbl_802F30B0(void) { return FLT_EPSILON; }
float  force_lbl_802F30B8(void) { return 1.0f; }
float  force_lbl_802F30BC(void) { return 0.0f; }
double force_lbl_802F30C0(void) { return 4503601774854144.0; }
asm void input_main(void)
{
    nofralloc
#include "../asm/nonmatchings/input_main.s"
}
#pragma peephole on
#endif

void func_80025158(PADStatus *pads)
{
    u32 i;

    PADRead(pads);

    for (i = 0; i < 4; i++)
    {
        if (pads[i].err == -1)
            lbl_802F1CD4 |= 0x80000000 >> i;
    }

    if (lbl_802F1CD8 != 0)
    {
        u32 r5 = lbl_802F1CD8;

        while (r5 != 0)
        {
            if (pads[i].err != 0 && pads[i].err != -3)
                lbl_802F1CD8 &= ~(0x80000000 >> i);
            r5 &= ~(0x80000000 >> i);
            i = __cntlzw(r5);  // huh?
        }

        if (lbl_802F1CD8 != 0)
        {
            if (PADRecalibrate(lbl_802F1CD8))
                lbl_802F1CD4 = 0;
            else
                lbl_802F1CD4 |= lbl_802F1CD8;
            lbl_802F1CD8 = 0;
        }
    }

    if (lbl_802F1CD4 != 0)
    {
        if (PADReset(lbl_802F1CD4))
            lbl_802F1CD8 = lbl_802F1CD4;
        lbl_802F1CD4 = 0;
    }
}

// Updates the state of analog inputs for each controller.
// This function detects when sticks or triggers have moved enough to be considered a button press.
// The threshold for an input to be detected as pressed is much greater than the threshold for being
// released. This makes it less sensitive to rapid fluctuations.
void handle_analog_inputs(void)
{
    int i;
    struct AnalogInput *analog = &analogInputs[0];

    for (i = 0; i < 4; i++)
    {
        int x;
        int y;
        int threshold;
        struct ControllerInfo *cont = &controllerInfo[i];

        analog->prevHeld = analog->held;
        analog->held = 0;

        x = cont->held.stickX;
        y = cont->held.stickY;

        threshold = (analog->prevHeld & ANALOG_STICK_LEFT) ? -15 : -52;
        if (x < threshold)
            analog->held |= ANALOG_STICK_LEFT;

        threshold = (analog->prevHeld & ANALOG_STICK_RIGHT) ? 15 : 52;
        if (x > threshold)
            analog->held |= ANALOG_STICK_RIGHT;

        threshold = (analog->prevHeld & ANALOG_STICK_DOWN) ? -15 : -52;
        if (y < threshold)
            analog->held |= ANALOG_STICK_DOWN;

        threshold = (analog->prevHeld & ANALOG_STICK_UP) ? 15 : 52;
        if (y > threshold)
            analog->held |= ANALOG_STICK_UP;

        x = cont->held.substickX;
        y = cont->held.substickY;

        threshold = (analog->prevHeld & ANALOG_CSTICK_LEFT) ? -15 : -52;
        if (x < threshold)
            analog->held |= ANALOG_CSTICK_LEFT;

        threshold = (analog->prevHeld & ANALOG_CSTICK_RIGHT) ? 15 : 52;
        if (x > threshold)
            analog->held |= ANALOG_CSTICK_RIGHT;

        threshold = (analog->prevHeld & ANALOG_CSTICK_DOWN) ? -15 : -52;
        if (y < threshold)
            analog->held |= ANALOG_CSTICK_DOWN;

        threshold = (analog->prevHeld & ANALOG_CSTICK_UP) ? 15 : 52;
        if (y > threshold)
            analog->held |= ANALOG_CSTICK_UP;

        x = cont->held.triggerLeft;
        y = cont->held.triggerRight;

        threshold = (analog->prevHeld & ANALOG_TRIGGER_LEFT) ? 40 : 80;
        if (x > threshold)
            analog->held |= ANALOG_TRIGGER_LEFT;

        threshold = (analog->prevHeld & ANALOG_TRIGGER_RIGHT) ? 40 : 80;
        if (y > threshold)
            analog->held |= ANALOG_TRIGGER_RIGHT;

        analog->pressed = analog->held & ~analog->prevHeld;
        // BUG: should be analog->released = ~analog->held & analog->prevHeld
        analog->released = analog->held & ~analog->prevHeld;

        analog++;
    }
}

void get_key_repeats(void)
{
    struct ControllerInfo *cont = &controllerInfo[0];
    struct AnalogInput *analog = &analogInputs[0];
    int i;

    for (i = 0; i < 4; i++, cont++, analog++)
    {
        if (cont->held.button == cont->prevHeld.button
         && analog->held == analog->prevHeld)
            controllerRepeatCounts[i]++;
        else
            controllerRepeatCounts[i] = 0;

        // After being held for 20 frames, repeat the input every 4th frame
        if (controllerRepeatCounts[i] >= 20 && (controllerRepeatCounts[i] & 3) == 0)
        {
            cont->repeat.button = cont->held.button;
            analog->repeat = analog->held;
        }
        else
        {
            cont->repeat.button = cont->pressed.button;
            analog->repeat = analog->pressed;
        }
    }
}

// make_act_input?
void func_80025640(void)
{
    int i;
    int j;
    s8 *status;

    for (i = 0; i < 5; i++)
    {
        g_currPlayerButtons[i] = 0;
        g_currPlayerAnalogButtons[i] = 0;
    }

    // I could only get this function to match with nested loops, which suggests that controllerInfo
    // and analogInputs should be 2D arrays. However, I find the rest of the code more readable when
    // they are arrays of structs with properly labeled members.

    switch (gameMode)
    {
    case MD_ADV:
    case MD_TEST:
    case MD_OPTION:
        for (i = 0; i < 4; i++)
        {
            if (controllerInfo[i].held.err == -1)
                continue;
            for (j = 0; j < 5; j++)
            {
                g_currPlayerButtons[j] |= ((PADStatus *)&controllerInfo[i])[j].button;
                g_currPlayerAnalogButtons[j] |= ((u16 *)&analogInputs[i])[j];
            }
        }
        break;
    default:
        status = g_poolInfo.playerPool.statusList;
        if (status == NULL)
            break;
        for (i = 0; i < 4; i++, status++)
        {
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MINI_BILLIARDS:
                if (i > 1 || lbl_802F0310[i] == 0)
                    continue;
                break;
            default:
                if (*status != STAT_NORMAL)
                    continue;
            }

            for (j = 0; j < 5; j++)
            {
                g_currPlayerButtons[j] |= ((PADStatus *)&controllerInfo[playerControllerIDs[i]])[j].button;
                g_currPlayerAnalogButtons[j] |= ((u16 *)&analogInputs[playerControllerIDs[i]])[j];
            }
        }
        break;
    }
}

void handle_reset_input(PADStatus *pads)
{
    int resetCode;
    int i;

    if (lbl_802F1CD1 == 1 && !OSGetResetButtonState())
    {
        resetCounter = 0xFF;
        resetCode = 2;
    }
    else
        resetCode = 1;

    if (OSGetResetButtonState())
        lbl_802F1CD1 = 1;

    if (resetCounter > 30.0)
    {
        ev_vibration_dest();
        if (memcard_is_write_in_progress() != 0)
            return;
        func_800A4CEC();
        gpwait_main();
        OSResetSystem(OS_RESET_HOTRESET, resetCode, FALSE);
        OSPanic("input.c", 472, "NANDEYANEN!?\n");
    }

    for (i = 0; i < 4; i++)
    {
        if ((pads[i].button & (PAD_BUTTON_START|PAD_BUTTON_X|PAD_BUTTON_B)) == (PAD_BUTTON_START|PAD_BUTTON_X|PAD_BUTTON_B))
        {
            resetCounter++;
            break;
        }
    }
    if (i == 4)
        resetCounter = 0;
}

void func_80025B1C(struct CoordsS8 *a, s8 *b)
{
    int r0;
    int i;

    for (i = 0; i < 8; i++)
    {
        float f9, f4, f10, f5;
        float f8;
        float f5_, f3;
        int r8;
        int foo;
        int bar;
        int sign;

        if (i == 7)
            r0 = 0;
        else
            r0 = i + 1;

        f9  = b[i*2+0];
        f4  = b[i*2+1];
        f10 = b[r0*2+0];
        f5  = b[r0*2+1];

        f8 = (float)b[i*2+0] * (float)b[r0*2+1]
           - (float)b[r0*2+0] * (float)b[i*2+1];

        if (fabs(f8) < FLT_EPSILON)
            continue;

        f8 = 1.0f / f8;

        f5_ = (f5 * a->x - f10 * a->y) * f8;
        f3 = (-f4 * a->x + f9 * a->y) * f8;
        if (f5_ < 0.0f || f3 < 0.0f)
            continue;

        r8 = f5_ * lbl_80110320_64[i][0] + f3 * lbl_80110320_64[r0][0];
        foo = r8 < 0;
        bar = clamp(abs(r8), 0, 99);
        sign = foo ? -1 : 1;
        a->x = sign * lbl_80110320[bar];

        r8 = f5_ * lbl_80110320_64[i][1] + f3 * lbl_80110320_64[r0][1];
        foo = r8 < 0;
        bar = clamp(abs(r8), 0, 99);
        sign = foo ? -1 : 1;
        a->y = sign * lbl_80110320[bar];

        break;
    }
}

void save_input_data(struct MemcardContents *dest)
{
    memcpy(dest->gameData.padLimit, padLimit, sizeof(padLimit));
}

void load_input_data(const struct MemcardContents *src)
{
    memcpy(padLimit, src->gameData.padLimit, sizeof(padLimit));
}
