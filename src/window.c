#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "bitmap.h"
#include "gxcache.h"
#include "input.h"
#include "pool.h"

u8 lbl_80201928[0x7AC];  FORCE_BSS_ORDER(lbl_80201928)  // 0, size = 0x7A9
u8 lbl_802020D4[0x7AC];  FORCE_BSS_ORDER(lbl_802020D4)  // 0x7AC, size = 0x7A9
u8 lbl_80202880[0x7AC];  FORCE_BSS_ORDER(lbl_80202880)  // 0xF58, size = 0x7A9
u8 lbl_80201928_1704[0x7AC]; FORCE_BSS_ORDER(lbl_80201928_1704)  // 0x1704, size = 0x7A9
u8 lbl_802037D8[0x7AC];  FORCE_BSS_ORDER(lbl_802037D8)  // 0x1EB0, size = 0x7A9
u8 lbl_80203F84[0x7AC];  FORCE_BSS_ORDER(lbl_80203F84)  // 0x265C, size = 0x7A9
u8 lbl_80203F84_2[0x7AC];  FORCE_BSS_ORDER(lbl_80203F84_2)  // 0x2E08
u8 lbl_80203F84_3[0x7AC];  FORCE_BSS_ORDER(lbl_80203F84_3)  // 0x35B4

struct Struct8002EF0C_child
{
    u32 unk0;
    s32 unk4;
    s32 unk8;
    char *unkC;
    void *unk10;
    u32 unk14;
};  // size = 0x18

struct Struct8002EF0C
{
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    struct Struct8002EF0C_child *unk10;
    u32 unk14;
    u32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
};

struct Struct8002EF0C *lbl_80205688[16];  // 0x3D60
FORCE_BSS_ORDER(lbl_80205688)

struct
{
    //void *unk0[0x20/4];
    struct Struct8002EF0C unk60[1];  // 0x3DC0
    u8 filler6C[0x300-0x6C];
} lbl_80205688_2;  // 0x3DA0
FORCE_BSS_ORDER(lbl_80205688_2)

extern int lbl_802F1E08;  // s32 vs int actually matters here
extern u32 lbl_802F1EA8;

void func_80030030(int arg0, int arg1, int arg2, int arg3);

void func_8002DC54(void)
{
    struct Struct8002EF0C **var_r3;
    int var_r4;

    if (lbl_802F1EA8 == 0)
    {
        var_r3 = &lbl_80205688[15];
        for (var_r4 = 15; var_r4 >= 0; var_r4--, var_r3--)
        {
            if (*var_r3 != 0)
                break;
        }
        if (var_r4 >= 0)
        {
            lbl_802F1E08 = var_r4;
            lbl_802F1EA8 = 1;
        }
    }
    else
    {
        lbl_802F1E08 --;
        if (lbl_802F1E08 < 0 || lbl_80205688[lbl_802F1E08] == NULL)
        {
            lbl_802F1E08 = 0;
            lbl_802F1EA8 = 0;
        }
    }
}

static void func_8002DD5C_inline(void)
{
    struct Struct8002EF0C **r6 = &lbl_80205688[lbl_802F1E08];
    void *r4 = *r6;
    int i;

    for (i = lbl_802F1E08; i > 0; i--, r6--)
        *r6 = *(r6 - 1);
    lbl_80205688[0] = r4;
    lbl_802F1E08 = 0;
}

void func_8002DD5C(void)
{
    if (lbl_802F1E08 == 0)
    {
        struct Struct8002EF0C **r3 = lbl_80205688;
        struct Struct8002EF0C *r4 = *r3;
        int i;

        for (i = 0; i < 15 && *(r3 + 1) != 0; i++, r3++)
            *r3 = *(r3 + 1);
        *r3 = r4;
        lbl_802F1E08 = i;
    }
    else
    {
        func_8002DD5C_inline();
    }
}

void func_8002DE38(void)
{
    int r6;
    int i;  // r7

    for (i = 0; i < 16; i++)
    {
        r6 = ((lbl_802F1E08 - i) + 16) % 16;
        if (lbl_80205688[r6] != 0)
        {
            lbl_802F1E08 = r6;
            lbl_802F1EA8 = 1;
            break;
        }
    }
    if (i >= 16)
    {
        lbl_802F1E08 = 0;
        lbl_802F1EA8 = 0;
    }
}

void func_8002E06C(struct Struct8002EF0C *arg0)
{

    struct Struct8002EF0C *var_r4;
    //int var_r6;
    int i;
    #define var_r6 i
    void **r6;
    struct Struct8002EF0C *var_r7;

    lbl_802F1EA8 = 1;
    if (lbl_80205688[15] == NULL)
    {
        //var_r4 = lbl_80205688.unk40;
        for (var_r6 = 0; var_r6 < 16; var_r6++)
        {
            if (lbl_80205688_2.unk60[var_r6].unk20 == 0)
                break;
        }
        if (var_r6 < 16)
        {
            var_r7 = &lbl_80205688_2.unk60[var_r6];
            //var_r7 = var_r7;
            //someinline(var_r7, arg0);
            //*var_r7 = *arg0;
            var_r7->unk0 = arg0->unk0;
            var_r7->unk4 = arg0->unk4;
            var_r7->unk8 = arg0->unk8;
            var_r7->unkC = arg0->unkC;
            var_r7->unk10 = arg0->unk10;
            var_r7->unk14 = arg0->unk14;
            var_r7->unk18 = arg0->unk18;
            var_r7->unk1C = arg0->unk1C;
            var_r7->unk20 = 1;
            var_r7->unk24 = arg0->unk24;
            var_r7->unk28 = arg0->unk28;
            r6 = lbl_80205688;
            for (i = 0; i < 16; i++, r6++)
            {
                void *old = *r6;
                *r6 = var_r7;
                var_r7 = old;
                if (old == NULL)
                    break;
            }
            lbl_802F1E08 = 0;
        }
    }
    #undef var_r6
}

extern u32 lbl_802F1E1C;
extern u32 lbl_802F1E20;
extern u32 lbl_802F1E24;
extern u32 lbl_802F1E28;

void draw_window_frame(struct Struct8002EF0C *arg0);
void func_8002FCAC(int arg0, int arg1);

static void set_some_params(int a, int b, int c, int d)
{
    lbl_802F1E1C = a;
    lbl_802F1E20 = b;
    lbl_802F1E24 = c;
    lbl_802F1E28 = d;
}

u8 datafiller[0x39B4] = {0};

void func_8002EA40(struct Struct8002EF0C *arg0, u32 arg1)
{
    struct Struct8002EF0C_child *var_r28;
    int var_r27;
    int var_r26;
    int var_r25;
    char *var_r5_2;
    void *temp_r5;
    int var_r5;

    if (arg1 != 0)
        func_8002FCAC(arg0->unk24, arg0->unk28);
    else
        func_8002FCAC(0, 0);
    func_80030030(arg0->unk0, arg0->unk4, arg0->unk8, arg0->unkC);
    set_some_params(arg0->unk0, arg0->unk4, arg0->unk8, arg0->unkC);
    if (arg1 != 0)
        u_debug_set_text_color(2);
    else
        u_debug_set_text_color(0);
    draw_window_frame(arg0);
    u_debug_set_text_color(0);
    set_some_params(arg0->unk0 + 1, arg0->unk4 + 1, arg0->unk8 - 2, arg0->unkC - 2);
    var_r26 = 0;
    var_r25 = 0;
    var_r27 = 0;
    var_r28 = arg0->unk10;
    while (var_r28->unk0 != 0x1F)
    {
        if (var_r28->unk4 > 0)
            var_r26 = arg0->unk0 + var_r28->unk4;
        else
            var_r26 -= var_r28->unk4;
        if (var_r28->unk8 > 0)
            var_r25 = arg0->unk4 + var_r28->unk8;
        else
            var_r25 -= var_r28->unk8;
        u_debug_set_cursor_pos(var_r26, var_r25);
        u_debug_set_text_color(0);
        switch (var_r28->unk0 & 0xFF)
        {
        case 13:
        case 26:
            if (var_r27 == 0 && arg1 != 0)
                u_debug_set_text_color(2);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (arg1 != 0)
                u_debug_set_text_color(2);
            break;
        }
        if (var_r28->unk0 & 0x200)
            var_r5 = 0;
        else
            var_r5 = arg0->unk18;
        temp_r5 = (u8 *)var_r28->unk10 + (arg0->unk14 * var_r5);
        switch (var_r28->unk0 & 0xFF)
        {
        case 0:
        case 9:
        case 11:
        case 13:
            window_printf(2, var_r28->unkC);
            break;
        case 1:
        case 14:
            window_printf(2, var_r28->unkC, *(int *)temp_r5);
            break;
        case 2:
        case 15:
            window_printf(2, var_r28->unkC, *(int *)temp_r5);
            break;
        case 10:
            window_printf(2, var_r28->unkC, arg0->unk18);
            break;
        case 3:
        case 16:
            window_printf(2, var_r28->unkC, *(u16 *)temp_r5);
            break;
        case 4:
        case 17:
            window_printf(2, var_r28->unkC, *(s16 *)temp_r5);
            break;
        case 5:
        case 18:
            window_printf(2, var_r28->unkC, *(u8 *)temp_r5);
            break;
        case 6:
        case 19:
            window_printf(2, var_r28->unkC, *(s8 *)temp_r5);
            break;
        case 7:
        case 20:
            window_printf(2, var_r28->unkC, *(float *)temp_r5);
            break;
        case 21:
            window_printf(2, var_r28->unkC, (char *)temp_r5);
            break;
        case 22:
            window_printf(2, var_r28->unkC, *(char **)temp_r5);
            break;
        case 8:
        case 23:
            if (*(u32 *)temp_r5 & (u32)var_r28->unk14)
                var_r5_2 = " ON";
            else
                var_r5_2 = "OFF";
            window_printf(2, var_r28->unkC, var_r5_2);
            break;
        case 24:
            if (*(u16 *)temp_r5 & (u16)var_r28->unk14)
                var_r5_2 = " ON";
            else
                var_r5_2 = "OFF";
            window_printf(2, var_r28->unkC, var_r5_2);
            break;
        case 26:
            window_printf(2, var_r28->unkC, arg0->unk18);
            break;
        case 25:
            window_printf(2, var_r28->unkC, ((u32 *)var_r28->unk14)[*(u8 *)temp_r5]);
            break;
        case 29:
            window_printf(2, "                 free       size\n");
            window_printf(2, "      MainHeap %08X / %08X\n", OSCheckHeap(mainHeap), mainHeapSize);
            window_printf(2, "       SubHeap %08X / %08X\n", OSCheckHeap(subHeap), subHeapSize);
            window_printf(2, "     StageHeap %08X / %08X\n", OSCheckHeap(stageHeap), stageHeapSize);
            window_printf(2, "BackgroundHeap %08X / %08X\n", OSCheckHeap(backgroundHeap), backgroundHeapSize);
            window_printf(2, "     CharaHeap %08X / %08X\n", OSCheckHeap(charaHeap), charaHeapSize);
            break;
        case 30:
            window_printf(2, var_r28->unkC, lbl_801B7948[((s8 *)var_r28->unk10)[arg0->unk18]]);
            break;
        }
        if (arg0->unk1C == var_r27 && arg1 != 0)
        {
            u_debug_set_cursor_pos(var_r26 - 1, var_r25);
            u_debug_set_text_color(1);
            func_8002FD68(2, "\x1C");
        }
        var_r28++;
        var_r27++;
    }
    func_8002FCAC(0, 0);
}

void draw_window_frame(struct Struct8002EF0C *arg0)
{
    int i;

    set_some_params(arg0->unk0, arg0->unk4, arg0->unk8, arg0->unkC);
    u_debug_set_cursor_pos(arg0->unk0, arg0->unk4);
    func_8002FD68(2, "\x18");
    u_debug_set_cursor_pos(arg0->unk0 + arg0->unk8 - 1, arg0->unk4);
    func_8002FD68(2, "\x19");
    u_debug_set_cursor_pos(arg0->unk0, arg0->unk4 + arg0->unkC - 1);
    func_8002FD68(2, "\x1A");
    u_debug_set_cursor_pos(arg0->unk0 + arg0->unk8 - 1, arg0->unk4 + arg0->unkC - 1);
    func_8002FD68(2, "\x1B");

    for (i = 1; i < arg0->unk8 - 1; i++)
    {
        u_debug_set_cursor_pos(arg0->unk0 + i, arg0->unk4);
        func_8002FD68(2, "\x16");
    }

    for (i = 1; i < arg0->unkC - 1; i++)
    {
        u_debug_set_cursor_pos(arg0->unk0, arg0->unk4 + i);
        func_8002FD68(2, "\x17");
    }

    for (i = 1; i < arg0->unk8 - 1; i++)
    {
        u_debug_set_cursor_pos(arg0->unk0 + i, arg0->unk4 + arg0->unkC - 1);
        func_8002FD68(2, "\x16");
    }

    for (i = 1; i < arg0->unkC - 1; i++)
    {
        u_debug_set_cursor_pos(arg0->unk0 + arg0->unk8 - 1, arg0->unk4 + i);
        func_8002FD68(2, "\x17");
    }
}

void window_init(void)
{
    s32 i;

    lbl_802F1E08 = 0;
    lbl_802F1EA8 = 0U;
    for (i = 0; i < 16; i++)
        lbl_80205688_2.unk60[i].unk20 = 0;
    memset(lbl_80201928, 0x20, 0x7A9);
    memset(lbl_802020D4, 0x20, 0x7A9);
    memset(lbl_80202880, 0x20, 0x7A9);
    memset(lbl_80201928_1704, 0, 0x7A9);
    memset(lbl_802037D8, 0, 0x7A9);
    memset(lbl_80203F84, 0, 0x7A9);
}

void window_main(void)
{

}

extern s32 lbl_802F1E10;
extern s32 lbl_802F1E0C;
extern s32 lbl_802F1E14;
extern s8 lbl_802F1E18;
extern s8 lbl_802F1E19;
extern u8 lbl_802F1E2C;

void u_debug_set_cursor_pos(int arg0, int arg1)
{
    lbl_802F1E10 = arg0;
    lbl_802F1E0C = arg0;
    lbl_802F1E14 = arg1;
}

void func_8002FC90(int arg0, int arg1)
{
    lbl_802F1E10 += arg0;
    lbl_802F1E14 += arg1;
}

void func_8002FCAC(int arg0, int arg1)
{
    lbl_802F1E18 = arg0;
    lbl_802F1E19 = arg1;
}

int window_printf(int arg0, char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    func_8002FD68(arg0, buf);
    va_end(args);
    return 0;
}

void u_debug_set_text_color(int arg0)
{
    lbl_802F1E2C = arg0;
}

void func_8002FD68(int arg0, const char *arg1)
{
    u8 *var_r5;
    u8 *var_r6;
    u8 *var_r7;
    u8 *var_r8;
    int var_r9;
    int var_r10;
    int var_r11;
    int var_r12;

    switch (arg0)
    {
    case 0:
        var_r9 = 0;
        var_r10 = 0;
        var_r11 = 0x34;
        var_r5 = lbl_80201928 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r12 = 0x24;
        var_r6 = lbl_80201928_1704 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r7 = NULL;
        var_r8 = NULL;
        break;
    case 1:
        var_r9 = 0;
        var_r10 = 0;
        var_r11 = 0x34;
        var_r5 = lbl_802020D4 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r12 = 0x24;
        var_r6 = lbl_802037D8 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r7 = NULL;
        var_r8 = NULL;
        break;
    default:
        var_r9 = lbl_802F1E1C;
        var_r10 = lbl_802F1E20;
        var_r11 = var_r9 + lbl_802F1E24 - 1;
        var_r5 = lbl_80202880  + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r12 = var_r10 + lbl_802F1E28 - 1;
        var_r6 = lbl_80203F84 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r7 = lbl_80203F84_2 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        var_r8 = lbl_80203F84_3 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
        break;
    }

    while (*arg1 != 0)
    {
        if (lbl_802F1E10 >= 0x35)
        {
            lbl_802F1E10 = 0;
            lbl_802F1E14++;
        }
        if (*arg1 == '\n')
        {
            if (lbl_802F1E10 >= lbl_802F1E0C)
                lbl_802F1E14++;
            lbl_802F1E10 = lbl_802F1E0C - 1;
            switch (arg0)
            {
            case 0:
                var_r5 = lbl_80201928 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                var_r6 = lbl_80201928_1704 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                break;
            case 1:
                var_r5 = lbl_802020D4 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                var_r6 = lbl_802037D8 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                break;
            default:
                var_r5 = lbl_80202880 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                var_r6 = lbl_80203F84 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                var_r7 = lbl_80203F84_2 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                var_r8 = lbl_80203F84_3 + (lbl_802F1E14 * 0x35) + lbl_802F1E10;
                break;
            }
        }
        else if (lbl_802F1E10 >= var_r9 && lbl_802F1E10 <= var_r11
         && lbl_802F1E14 >= var_r10 && lbl_802F1E14 <= var_r12)
        {
            *var_r5 = *arg1;
            *var_r6 = lbl_802F1E2C;
            if (var_r7 != NULL)
                *var_r7 = lbl_802F1E18;
            if (var_r8 != NULL)
                *var_r8 = lbl_802F1E19;
        }
        if (var_r7 != NULL)
            var_r7++;
        if (var_r8 != NULL)
            var_r8++;
        var_r5++;
        arg1++;
        var_r6++;
        lbl_802F1E10++;
    }
}

void func_8002FFEC(void)
{
    memset(lbl_802020D4, 0x20, 0x7A9);
    memset(lbl_802037D8, 0, 0x7A9);
}

void func_80030030(int arg0, int arg1, int arg2, int arg3)
{
    u8 *var_r8;
    u8 *var_r9;
    int i;
    int j;

    for (i = arg1; i < arg1 + arg3; i++)
    {
        var_r8 = lbl_80202880 + i * 0x35 + arg0;
        var_r9 = lbl_80203F84 + i * 0x35 + arg0;

        for (j = arg0; j < arg0 + arg2; j++)
        {
            *var_r8++ = 0x20;
            *var_r9++ = 0;
        }
    }
}

int func_80030114(const char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    func_8002FD68(1, buf);
    va_end(args);
    return 0;
}

int u_debug_printf(const char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    func_8002FD68(0, buf);
    va_end(args);
    return 0;
}

void u_debug_print(char *arg0)
{
    func_8002FD68(0, arg0);
}

int func_8003026C(int unused, char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    if (!(debugFlags & 4))
        return 0;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    puts(buf);
    va_end(args);
    return 0;
}

extern u32 lbl_801B75F8[];

void func_80030310(int arg0, int arg1, s8 arg2, u8 arg3)
{
    if (arg2 < 0x80)
    {
        float temp_f31 = (12.0 * (float)(arg2 & 0xF)) / 192.0;
        float temp_f29 = (12.0 * (float)((arg2 >> 4) & 0xF)) / 96.0;
        float temp_f28 = arg0;
        float temp_f30 = arg1;
        u32 temp;
        GXColor sp20;
        GXColor sp1C = {0};

        temp_f30 *= 1.0714285714285714;
        temp = lbl_801B75F8[arg3 & 0xFF];
        sp20.r = (temp >> 24) /*& 0xFF*/;
        sp20.g = (temp >> 16) & 0xFF;
        sp20.b = (temp >> 8) /*& 0xFF*/;
        sp20.a = temp & 0xFF;

        GXSetTevColor(GX_TEVREG0, sp20);
        GXSetTevColor(GX_TEVREG1, sp1C);
        GXBegin(GX_QUADS, GX_VTXFMT7, 4U);
        GXPosition3f32(temp_f28, temp_f30, -0.0078125f);
        GXTexCoord2f32(temp_f31, 0.010416666666666666 + temp_f29);
        GXPosition3f32(12.0f + temp_f28, temp_f30, -0.0078125f);
        GXTexCoord2f32(0.0625 + temp_f31, 0.010416666666666666 + temp_f29);
        GXPosition3f32(12.0f + temp_f28, 12.0f + temp_f30, -0.0078125f);
        GXTexCoord2f32(0.0625 + temp_f31, 0.125 + temp_f29);
        GXPosition3f32(temp_f28, 12.0f + temp_f30, -0.0078125f);
        GXTexCoord2f32(temp_f31, 0.125 + temp_f29);
        GXEnd();
    }
}

void func_800304E0(float x1, float y1, float x2, float y2)
{
    GXColor tevColor0 = {0, 0, 0, 96};
    GXColor tevColor1 = {0, 0, 0, 0};
    u8 unused[4];

    GXLoadTexObj_cached(&u_unkBitmapTPL->texObjs[75], 0);
    y1 *= 1.0714285714285714;
    y2 *= 1.0714285714285714;
    GXSetTevColor(GX_TEVREG0, tevColor0);
    GXSetTevColor(GX_TEVREG1, tevColor1);
    GXBegin(GX_QUADS, GX_VTXFMT7, 4U);
    GXPosition3f32(x1, y1, -0.00859375f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x2, y1, -0.00859375f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x2, y2, -0.00859375f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x1, y2, -0.00859375f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXEnd();
}
