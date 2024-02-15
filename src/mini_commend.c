#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "gma.h"
#include "gxutil.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "mot_ape.h"
#include "sound.h"
#include "sprite.h"
#include "thread.h"
#include "window.h"

static void mini_commend_finish(void);
static void lbl_8009C5E0(struct Sprite *);
static void func_8009C5E4(s8 *arg0, s8 *arg1);
static void func_8009CAE0(s8 *arg0, s8 *arg1, s8 arg2);
static void mini_commend_free_data(void);
static void func_8009CD5C(void);
static void func_8009D3AC(struct Ape *, int);
static u32 func_8009D5F4(void);
static int func_8009D794(void);
static void func_8009DB40(Mtx arg0);

void submode_mini_commend_init_func(void)
{
    s8 spC[4];
    s8 sp8[4];

    event_start(0xF);
    event_start(0x10);
    event_start(0xD);
    event_start(0x12);
    camera_set_state(0x46);
    cameraInfo->eye.x = -0.5f;
    cameraInfo->eye.y = 1.3f;
    cameraInfo->eye.z = -2.5f;
    cameraInfo->lookAt.x = -0.5f;
    cameraInfo->lookAt.y = 0.9f;
    cameraInfo->lookAt.z = 0.0f;

    spC[0] = 0;
    sp8[0] = 0;
    spC[1] = 0x41;
    sp8[1] = 1;
    spC[2] = 0x42;
    sp8[2] = 2;
    spC[3] = 0x43;
    sp8[3] = 3;

    func_8009C5E4(spC, sp8);
    modeCtrl.unk10 = 0;
    submodeFinishFunc = mini_commend_finish;
    gameSubmodeRequest = SMD_MINI_COMMEND_MAIN;
}

void submode_mini_commend_main_func(void)
{
    s8 sp1C[4];
    s8 sp18[4];
    s8 sp10[5];
    s8 sp8[5];
    s32 var_r30;

    if (debugFlags & 0xA)
        return;
    var_r30 = modeCtrl.unk10;
    if (controllerInfo[0].pressed.button & PAD_BUTTON_RIGHT)
    {
        var_r30++;
        if (var_r30 >= 11)
            var_r30 = 0;
    }
    if (controllerInfo[0].pressed.button & PAD_BUTTON_LEFT)
    {
        var_r30--;
        if (var_r30 < 0)
            var_r30 = 10;
    }
    if (var_r30 != modeCtrl.unk10)
    {
        switch (var_r30)
        {
        case 0:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 0;
            sp1C[1] = 1;
            sp18[1] = 1;
            sp1C[2] = 2;
            sp18[2] = 2;
            sp1C[3] = 3;
            sp18[3] = 3;
            func_8009C5E4(sp1C, sp18);
            break;
        case 1:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 3;
            sp1C[1] = 1;
            sp18[1] = 0;
            sp1C[2] = 2;
            sp18[2] = 1;
            sp1C[3] = 3;
            sp18[3] = 2;
            func_8009C5E4(sp1C, sp18);
            break;
        case 2:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 2;
            sp1C[1] = 1;
            sp18[1] = 3;
            sp1C[2] = 2;
            sp18[2] = 0;
            sp1C[3] = 3;
            sp18[3] = 1;
            func_8009C5E4(sp1C, sp18);
            break;
        case 3:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 1;
            sp1C[1] = 1;
            sp18[1] = 2;
            sp1C[2] = 2;
            sp18[2] = 3;
            sp1C[3] = 3;
            sp18[3] = 0;
            func_8009C5E4(sp1C, sp18);
            break;
        case 4:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 3;
            sp1C[1] = 0x41;
            sp18[1] = 0;
            sp1C[2] = 0x42;
            sp18[2] = 1;
            sp1C[3] = 0x43;
            sp18[3] = 2;
            func_8009C5E4(sp1C, sp18);
            break;
        case 5:
            mini_commend_free_data();
            sp1C[0] = 0x40;
            sp18[0] = 0;
            sp1C[1] = 1;
            sp18[1] = 1;
            sp1C[2] = 2;
            sp18[2] = 2;
            sp1C[3] = 3;
            sp18[3] = 3;
            func_8009C5E4(sp1C, sp18);
            break;
        case 6:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 0;
            sp1C[1] = 0x41;
            sp18[1] = 1;
            sp1C[2] = 0x42;
            sp18[2] = 2;
            sp1C[3] = 0x43;
            sp18[3] = 3;
            func_8009C5E4(sp1C, sp18);
            break;
        case 7:
            mini_commend_free_data();
            sp18[0] = 0;
            sp1C[0] = 0;
            sp1C[1] = 1;
            sp18[1] = 0;
            sp1C[2] = 2;
            sp18[2] = 0;
            sp1C[3] = 3;
            sp18[3] = 0;
            func_8009C5E4(sp1C, sp18);
            break;
        case 8:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 3;
            sp1C[1] = 1;
            sp18[1] = 3;
            sp1C[2] = 2;
            sp18[2] = 3;
            sp1C[3] = 3;
            sp18[3] = 3;
            func_8009C5E4(sp1C, sp18);
            break;
        case 9:
            mini_commend_free_data();
            sp1C[0] = 0;
            sp18[0] = 2;
            sp1C[1] = -1;
            sp1C[2] = -1;
            sp1C[3] = -1;
            func_8009C5E4(sp1C, sp18);
            break;
        case 10:
            mini_commend_free_data();
            sp10[0] = 0x40;
            sp8[0] = 3;
            sp10[1] = 0x41;
            sp8[1] = 3;
            sp10[2] = 0x42;
            sp8[2] = 3;
            sp10[3] = 0x43;
            sp8[3] = 3;
            sp10[4] = 2;
            sp8[4] = 0;
            func_8009CAE0(sp10, sp8, 0);
            break;
        }
        camera_set_state(0x46);
        modeCtrl.unk10 = var_r30;
    }
    mathutil_mtxA_from_translate_xyz(lbl_802F1ECC, lbl_802F1EC8, lbl_802F1EC4);
    mathutil_mtxA_rotate_y(lbl_802F1ED2);
    mathutil_mtxA_rotate_x(lbl_802F1ED4);
    mathutil_mtxA_rotate_z(lbl_802F1ED0);
    func_8009DB40(mathutilData->mtxA);
}

static void mini_commend_finish(void)
{
    mini_commend_free_data();
}

void func_8009BEF8(void)
{
    window_set_cursor_pos(1, 1);
    u_debug_print("MINI COMMEND");
    window_set_cursor_pos(3, 3);
    window_printf_2("\x1C [ %d / %d ]", modeCtrl.unk10, 11);
    func_8009CD5C();
    if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
        effect_draw();
}

static s8 lbl_801D3F5C[] =
{
    0x30, 0x1E, 0x21, 0x20,
    0x1C, 0x25, 0x2C, 0x2B,
    0x23, 0x29, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10,
    0x10, 0x0C, 0x1B, 0x0D,
    0x0E, 0x0F, 0x24, 0x11,
    0x26, 0x12, 0x27, 0x28,
    0x13, 0x1D, 0x14, 0x15,
    0x1F, 0x2A, 0x16, 0x17,
    0x18, 0x19, 0x2D, 0x1A,
    0x2E, 0x22, 0x2F, 0x00,
};

struct Struct801D404C_child
{
    s32 unk0;
    u32 unk4;
};

struct Struct801D404C
{
    char *unk0;
    float unk4;
    float unk8;
    u32 unkC;
    struct Struct801D404C_child *unk10;
    s32 unk14;
};

static struct Struct801D404C_child lbl_802F1368[] =
{
    { -1, 0x1E9 },
};

static struct Struct801D404C_child lbl_802F1370[] =
{
    {  0, 0x1E9 },
};

static struct Struct801D404C_child lbl_802F1378[] =
{
    {  0, 0x1F0 },
};

static struct Struct801D404C_child lbl_802F1380[] =
{
    {  0, 0x1DB },
};

static struct Struct801D404C_child lbl_801D3F88[] =
{
    { 0, 0x1E9 },
    { 0x82, 0x1F1 },
};

static struct Struct801D404C_child lbl_801D3F98[] =
{
    { 0, 0x1E9 },
    { 0x82, 0x1EA },
    { 0xBE, 0x1EF },
};

static struct Struct801D404C_child lbl_801D3FB0[] =
{
    { 0, 0x1E9 },
    { 0x82, 0x1EB },
    { 0xBE, 0x1EF },
};

static struct Struct801D404C_child lbl_801D3FC8[] =
{
    { 0, 0x1E9 },
    { 0x82, 0x1EC },
    { 0xBE, 0x1EF },
};

static struct Struct801D404C_child lbl_801D3FE0[] =
{
    { 0, 0x1E9 },
    { 0x82, 0x1ED },
    { 0xBE, 0x1EF },
};

static struct Struct801D404C_child lbl_801D3FF8[] =
{
    { 0, 0x1E9 },
    { 0x82, 0x1EE },
};

static struct Struct801D404C lbl_801D404C[] =
{
    { "CONGRATULATIONS:", 1.65, 0.25, 0, lbl_802F1370, 1 },
    { "CONGRATULATIONS:", 1.90, 0.22, 0, lbl_802F1368, 1 },
    { "YOU LOSE",         1.55, 0.32, 0, lbl_802F1378, 1 },
    { "YOU RETIRE",       1.05, 0.32, 0, lbl_802F1380, 1 },
    { "COM WINS",         1.43, 0.32, 1, lbl_802F1370, 1 },
    { "1P WINS",          1.43, 0.32, 1, lbl_801D3F98, 3 },
    { "2P WINS",          1.43, 0.32, 1, lbl_801D3FB0, 3 },
    { "3P WINS",          1.43, 0.32, 1, lbl_801D3FC8, 3 },
    { "4P WINS",          1.43, 0.32, 1, lbl_801D3FE0, 3 },
    { "DRAW GAME",        1.23, 0.32, 1, lbl_802F1368, 1 },
    { "RANK IN",          1.43, 0.32, 1, lbl_801D3FF8, 2 },
    { "",                 0.00, 0.00, 0, lbl_802F1368, 1 },
    { "YOU WIN",          1.43, 0.32, 1, lbl_801D3F88, 2 },
};

u32 mini_commend_unused[] =
{
    0x00000D00,
    0,
    0,
    0x3F800000,
    0x3F000000,
    0x3F800000,
    0,
    0,
    0x3F000000,
    0x3F800000,
    0,
    0x3F800000,
    0xFFFFFFFF,
    0x00000005,
    0xFFFFFFFF,
    0,
    0,
    0,
    0,
    0,
    0x00000D00,
    0x44200000,
    0,
    0x3F800000,
    0x3F000000,
    0x3F800000,
    0x3F000000,
    0,
    0x3F800000,
    0x3F800000,
    0,
    0x3F800000,
    0xFFFFFFFF,
    0x00000007,
    0xFFFFFFFF,
    0,
    0,
    0,
    0,
    0,
};

struct
{
    u32 unk0;
    struct Ape *apePtrs[4];
    int apeThreads[4];
    s8 unk24;
    s8 unk25[4];
    s8 unk29[4];
    s8 unk2D[4];
    struct TPL *tpl;
    struct GMA *gma;
    Mtx unk3C;
    struct Ape *unk6C;
    s32 unk70;
    u8 unk74;
    s8 unk75;
    s32 unk78;
    u8 unused[0x10];
} miniCommendInfo;

static const s8 lbl_802F59D4[] = { 84, 85, 86, 87 };
static const s8 lbl_802F59D8[] = { 0, 1, 2, 9, 10 };

static void func_8009BF74(int arg0)
{
    Mtx sp24;
    Vec sp18;
    Vec spC;
    s8 length;
    char *c;
    struct Struct801D404C *temp_r27;
    int i;

    if (miniCommendInfo.unk24 > 0)
    {
        func_8009D98C(&spC);
        temp_r27 = &lbl_801D404C[arg0];
        mathutil_mtxA_from_identity();
        mathutil_mtxA_scale_s(2.0f);
        mathutil_mtxA_to_mtx(sp24);
        if (arg0 == 1)
        {
            spC.y += temp_r27->unk4;
            mathutil_mtxA_from_mtxB_translate(&spC);
            mathutil_mtxA_scale_s(temp_r27->unk8);
            mathutil_mtxA_mult_left(sp24);
            mathutil_mtxA_to_mtx(sp24);
        }
        else
        {
            spC.y += temp_r27->unk4;
            mathutil_mtxA_from_mtxB_translate(&spC);
            mathutil_mtxA_scale_s(temp_r27->unk8);
            mathutil_mtxA_mult_left(sp24);
            mathutil_mtxA_to_mtx(sp24);
        }
        c = temp_r27->unk0;
        length = strlen(c);
        for (i = 0; i < length; i++, c++)
        {
            sp18.x = 1.14 * (0.5 * (length - 1)) - (1.14 * i);
            sp18.y = 0.0f;
            sp18.z = 0.0f;
            mathutil_mtxA_from_mtx(sp24);
            mathutil_mtxA_translate(&sp18);
            mathutil_mtxA_rotate_y(-0x8000);
            GXLoadPosMtxImm(mathutilData->mtxA, 0U);
            mathutil_mtxA_rotate_y(globalAnimTimer << 7);
            mathutil_mtxA_rotate_z(globalAnimTimer << 9);
            GXLoadNrmMtxImm(mathutilData->mtxA, 0U);
            if (*c != ' ')
                avdisp_draw_model_culled_sort_translucent(miniCommendInfo.gma->modelEntries[lbl_801D3F5C[*c - '0']].model);
        }
    }
}

static void func_8009C178(s8 *arg0, s8 *arg1, s8 *arg2, int arg3)
{
    int var_r8 = 0;
    int var_r9 = 0;
    int var_r10 = 0;
    int var_r11 = -1;
    int i;
    int var_r0;

    for (i = 0; i < arg3; i++)
    {
        int temp_r7 = arg2[i];

        if (!(arg0[temp_r7] & 0x40))
        {
            var_r8++;
            if (var_r11 < 0)
                var_r11 = temp_r7;
            if (arg1[temp_r7] == 0)
                var_r9++;
        }
        if (var_r10 < arg1[temp_r7])
            var_r10 = arg1[temp_r7];
    }

    if (miniCommendInfo.unk0 & 0x20)
        var_r0 = 10;
    else if (var_r9 > 1)
        var_r0 = 0;
    else if (miniCommendInfo.unk0 & 4)
        var_r0 = 3;
    else if (var_r8 == 1)
    {
        if (var_r10 == arg1[var_r11] && var_r10 != 0)
            var_r0 = 2;
        else if (arg1[var_r11] == 0)
            var_r0 = 12;
        else
            var_r0 = 4;
    }
    else if (var_r8 > 1)
    {
        if (arg0[*arg2] & 0x40)
            var_r0 = 4;
        else
            var_r0 = *arg2 + 5;
    }
    else
        var_r0 = 0;
    miniCommendInfo.unk75 = var_r0;
}

static void func_8009C2A4(s8 *status, struct Sprite *sprite)
{
    struct Effect effect;
    Vec sp34;
    Vec sp28;
    Vec vec1;
    Vec vec2;
    float var_f27;
    int var_r29;
    int var_r28;
    int var_r28_2;
    u16 temp_r28;
    struct Struct801D404C_child *var_r27;
    int var_r27_2;

    if (func_8009D794() != 0)
    {
        var_r27 = lbl_801D404C[miniCommendInfo.unk75].unk10;
        var_r28 = 0;
        for (var_r28 = 0; var_r28 < lbl_801D404C[miniCommendInfo.unk75].unk14; var_r28++, var_r27++)
        {
            if (var_r27->unk0 == miniCommendInfo.unk78)
                u_play_sound_0(var_r27->unk4);
        }
        if (miniCommendInfo.unk78 < 0x258)
            miniCommendInfo.unk78++;
    }
    if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING && !(miniCommendInfo.unk0 & 0x40))
    {
        if (miniCommendInfo.unk6C != NULL)
        {
            mathutil_mtxA_from_translate_xyz(miniCommendInfo.unk6C->unk30.x, 3.8 + lbl_802F1EC8, miniCommendInfo.unk6C->unk30.z);
            var_f27 = 0.4f;
        }
        else
        {
            var_r27_2 = 0;
            for (var_r28_2 = 0; var_r28_2 < miniCommendInfo.unk24; var_r28_2++)
            {
                if (miniCommendInfo.unk25[var_r28_2] == 0)
                {
                    func_8009D7FC(var_r28_2, &sp28);
                    if (var_r27_2 <= 0)
                    {
                        vec1.x = sp28.x;
                        vec1.z = sp28.z;
                        vec2.x = vec1.x;
                        vec2.z = vec1.z;
                    }
                    else
                    {
                        if (sp28.x > vec1.x)
                            vec1.x = sp28.x;
                        if (sp28.z > vec1.z)
                            vec1.z = sp28.z;
                        if (sp28.x < vec2.x)
                            vec2.x = sp28.x;
                        if (sp28.z < vec2.z)
                            vec2.z = sp28.z;
                    }
                    var_r27_2++;
                }
            }
            if (var_r27_2 <= 0)
                return;
            sp28.x = 0.5 * (vec1.x + vec2.x);
            sp28.y = 0.5 * (vec1.z + vec2.z);
            mathutil_mtxA_from_translate_xyz(sp28.x, 3.8 + lbl_802F1EC8, sp28.z);
            var_f27 = 0.4 + (vec1.x - vec2.x);
        }
        temp_r28 = 1 << currentBall->playerId;
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_COMMENDFRAG;
        effect.vel.x = 0.0f;
        effect.vel.y = 0.0f;
        effect.vel.z = 0.0f;
        var_r29 = 2;
        if (miniCommendInfo.unk0 & 0x80)
            var_r29 = 1;
        sp34.x = 0.0f;
        sp34.y = 0.0f;
        while (var_r29 > 0)
        {
            sp34.z = 0.5 * (var_f27 * (1.0 + RAND_FLOAT()));
            mathutil_mtxA_rotate_y(rand() & 0x7FFF);
            mathutil_mtxA_rotate_x(rand() & 0x7FFF);
            mathutil_mtxA_tf_point(&sp34, &effect.pos);
            effect.rotX = rand() & 0x7FFF;
            effect.rotY = rand() & 0x7FFF;
            effect.rotZ = rand() & 0x7FFF;
            if (var_r29 & 1)
                effect.cameras = temp_r28;
            else
                effect.cameras = 0;
            spawn_effect(&effect);
            var_r29--;
        }
    }
}

static void create_some_sprite(void)
{
    struct Sprite *temp_r3_5 = create_sprite();
    if (temp_r3_5 != NULL)
    {
        temp_r3_5->tag = 0x62;
        temp_r3_5->mainFunc = func_8009C2A4;
        temp_r3_5->drawFunc = lbl_8009C5E0;
        strcpy(temp_r3_5->text, "commend disp");
    }
}

static void lbl_8009C5E0(struct Sprite *sprite) {}

static void func_8009C5E4(s8 *arg0, s8 *arg1)
{
    s8 sp1C[4];
    int var_r22;
    int temp_r20;
    int var_ctr;
    int var_r4_2;
    int var_r9;
    int var_ctr_2;
    int var_ctr_4;
    struct Ape *temp_r3_4;
    s8 temp_r3_3;
    s8 temp_r8;
    s8 temp_r7;
    int i;

    if (miniCommendInfo.unk0 & 1)
        OSPanic("mini_commend.c", 726, "multiple CommendInit without CommendDest\n");
    miniCommendInfo.unk0 = 0;
    miniCommendInfo.unk75 = 0;
    miniCommendInfo.unk78 = 0;
    miniCommendInfo.unk6C = NULL;
    miniCommendInfo.unk70 = -1;
    for (i = 0; i < 4; i++)
    {
        miniCommendInfo.apePtrs[i] = NULL;
        miniCommendInfo.apeThreads[i] = -1;
    }
    DVDChangeDir("init");
    miniCommendInfo.tpl = load_tpl("commend.tpl");
    if (miniCommendInfo.tpl != NULL)
        miniCommendInfo.gma = load_gma("commend.gma", miniCommendInfo.tpl);
    DVDChangeDir("..");

    var_r22 = 0;
    for (i = 0; i < 4; i++)
    {
        if (arg0[i] >= 0)
        {
            sp1C[var_r22] = i;
            var_r22++;
        }
    }

    for (var_r9 = 0; var_r9 < var_r22 - 1; var_r9++)
    {
        for (var_ctr = var_r9 + 1; var_ctr < var_r22; var_ctr++)
        {
            if (arg1[sp1C[var_r9]] > arg1[sp1C[var_ctr]])
            {
                temp_r7 = sp1C[var_r9];
                temp_r8 = sp1C[var_ctr];
                sp1C[var_r9] = temp_r8;
                sp1C[var_ctr] = temp_r7;
            }
        }
    }
    if (var_r22 <= 1)
        miniCommendInfo.unk0 |= 0x20;
    if (var_r22 > 1)
    {
        var_r4_2 = 1;
        for (var_ctr_2 = 0; var_ctr_2 < var_r22; var_ctr_2++)
        {
            if (arg1[sp1C[var_ctr_2]] != 0)
            {
                var_r4_2 = 0;
                break;
            }
        }
        if (var_r4_2 != 0)
            miniCommendInfo.unk0 |= 2;
    }

    if (!(miniCommendInfo.unk0 & 2) && (var_r22 > 1))
    {
        var_r4_2 = 1;
        for (var_ctr_2 = 0; var_ctr_2 < var_r22; var_ctr_2++)
        {
            if (arg1[sp1C[var_ctr_2]] != 3)
            {
                var_r4_2 = 0;
                break;
            }
        }
        if (var_r4_2 != 0)
            miniCommendInfo.unk0 |= 4;
    }
    if (var_r22 == 2)
    {
        if (arg1[sp1C[0]] < arg1[sp1C[1]])
            arg1[sp1C[1]] = 3;
    }
    func_8009C178(arg0, arg1, sp1C, var_r22);
    if (var_r22 > 2 && !(miniCommendInfo.unk0 & 6))
    {
        u8 temp = sp1C[0];
        sp1C[0] = sp1C[1];
        sp1C[1] = temp;
    }

    miniCommendInfo.unk24 = var_r22;

    for (var_ctr_4 = 0; var_ctr_4 < var_r22; var_ctr_4++)
    {
        miniCommendInfo.unk25[var_ctr_4] = CLAMP(arg1[sp1C[var_ctr_4]], 0, 4);
        miniCommendInfo.unk29[var_ctr_4] = sp1C[var_ctr_4];
        miniCommendInfo.unk2D[var_ctr_4] = (arg0[sp1C[var_ctr_4]] & 0x40) != 0;
        arg0[sp1C[var_ctr_4]] &= 0xFFFFFFBF;
    }

    for (var_r22 = 0; var_r22 < miniCommendInfo.unk24; var_r22++)
    {
        temp_r3_3 = arg0[sp1C[var_r22]];
        if (temp_r3_3 >= 0)
        {
            temp_r20 = miniCommendInfo.unk25[var_r22];
            temp_r3_4 = u_make_ape(temp_r3_3);
            miniCommendInfo.apeThreads[var_r22] = thread_create(func_8009D3AC, temp_r3_4, 5);
            temp_r3_4->unk74 = temp_r20;
            if (miniCommendInfo.unk0 & 0x20)
                temp_r3_4->unk74 = 0;
            temp_r3_4->unk74 |= sp1C[var_r22] << 8;
            temp_r3_4->unk74 |= var_r22 * 0x10000;
            temp_r3_4->colorId = sp1C[var_r22];
            mathutil_mtxA_from_quat(&temp_r3_4->unk60);
            mathutil_mtxA_rotate_y(-0x4000);
            mathutil_mtxA_to_quat(&temp_r3_4->unk60);
            func_8009D7FC(var_r22, &temp_r3_4->unk30);
            func_8008BA2C(temp_r3_4, 10, (u8)temp_r3_4->unk74);
            miniCommendInfo.apePtrs[var_r22] = temp_r3_4;
        }
    }
    mathutil_mtxA_from_identity();
    mathutil_mtxA_to_mtx(miniCommendInfo.unk3C);
    create_some_sprite();
    u_play_music(0x40U, 0);
    miniCommendInfo.unk0 |= 1;
}

static const float lbl_80171B60[] = { 1.0f, 0.65f, 0.4f, 0.0f };

static void func_8009CAE0(s8 *arg0, s8 *arg1, s8 arg2)
{
    struct Ape *ape;
    int i;

    func_8009C5E4(arg0, arg1);
    if (arg0[4] >= 0)
    {
        ape = u_make_ape(arg0[4]);
        miniCommendInfo.unk70 = thread_create(&func_8009D3AC, ape, 5);
        ape->unk74 = 0;
        ape->unk74 |= arg2 << 8;
        ape->colorId = arg2;
        mathutil_mtxA_from_quat(&ape->unk60);
        mathutil_mtxA_rotate_y(-0x4000);
        mathutil_mtxA_to_quat(&ape->unk60);
        ape->unk30.x = -0.5f;
        ape->unk30.y = lbl_80171B60[0];
        ape->unk30.z = 1.0f;
        func_8008BA2C(ape, 10, (u8)ape->unk74);
        miniCommendInfo.unk6C = ape;
        miniCommendInfo.unk74 = arg2;
        miniCommendInfo.unk0 |= 8;
        miniCommendInfo.unk75 = arg2 + 5;
        for (i = 0; i < 4; i++)
        {
            if (miniCommendInfo.apePtrs[i] != NULL)
                miniCommendInfo.apePtrs[i]->colorId = 1;
        }
    }
}

static void mini_commend_free_data(void)
{
    int i;

    if (miniCommendInfo.unk0 & 1)
    {
        destroy_sprite_with_tag(0x62);
        if (miniCommendInfo.tpl != NULL || miniCommendInfo.gma != NULL)
        {
            VISetNextFrameBuffer(gfxBufferInfo->currFrameBuf);
            VIWaitForRetrace();
        }
        if (miniCommendInfo.tpl != NULL)
        {
            free_tpl(miniCommendInfo.tpl);
            miniCommendInfo.tpl = NULL;
        }
        if (miniCommendInfo.gma != NULL)
        {
            free_gma(miniCommendInfo.gma);
            miniCommendInfo.gma = NULL;
        }
        if (miniCommendInfo.unk6C != NULL)
        {
            new_ape_close(miniCommendInfo.unk6C);
            miniCommendInfo.unk6C = NULL;
        }
        if (miniCommendInfo.unk70 >= 0)
        {
            thread_kill(miniCommendInfo.unk70);
            miniCommendInfo.unk70 = -1;
        }
        for (i = 3; i >= 0; i--)
        {
            if (miniCommendInfo.apePtrs[i] != NULL)
            {
                new_ape_close(miniCommendInfo.apePtrs[i]);
                miniCommendInfo.apePtrs[i] = NULL;
            }
            if (miniCommendInfo.apeThreads[i] >= 0)
            {
                thread_kill(miniCommendInfo.apeThreads[i]);
                miniCommendInfo.apeThreads[i] = -1;
            }
        }
        miniCommendInfo.unk0 &= 0xFFFFFFFE;
    }
}

static void func_8009CD5C(void)
{
    int i;
    int var_r4;
    struct Ape *temp_r27;
    struct Ape *var_r25_2;
    EnvMapFunc temp_r26;
    struct Color3f *temp_r3_2;
    Mtx sp8C;
    Vec sp80;
    Mtx sp50;
    Vec sp44;
    Mtx sp14;
    Vec sp8;

    if (miniCommendInfo.unk0 & 1)
    {
        mathutil_mtx_copy(mathutilData->mtxB, sp8C);
        mathutil_mtxA_from_mtxB();
        mathutil_mtxA_mult_right(miniCommendInfo.unk3C);
        mathutil_mtxA_to_mtx(mathutilData->mtxB);
        temp_r26 = backgroundInfo.stageEnvMapFunc;
        if (backgroundInfo.stageEnvMapFunc != NULL)
            u_avdisp_set_some_func_1(temp_r26);
        mathutil_mtxA_from_mtxB();
        mathutil_mtxA_rotate_y(-0x8000);
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_translucent(miniCommendInfo.gma->modelEntries[8].model);
        for (i = 0; i < miniCommendInfo.unk24; i++)
        {
            func_8009D7FC(i, &sp80);
            mathutil_mtxA_from_mtxB_translate(&sp80);
            mathutil_mtxA_rotate_y(-0x8000);
            u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
            avdisp_draw_model_culled_sort_translucent(miniCommendInfo.gma->modelEntries[3].model);
            if (miniCommendInfo.unk25[i] < 5U)
            {
                var_r4 = lbl_802F59D8[miniCommendInfo.unk25[i]];
                if (miniCommendInfo.unk24 == 2 && miniCommendInfo.unk25[i] == 0 && !(miniCommendInfo.unk0 & 2))
                    var_r4 = 11;
                avdisp_draw_model_culled_sort_translucent(miniCommendInfo.gma->modelEntries[var_r4].model);
            }
        }
        if (miniCommendInfo.unk6C != NULL)
        {
            mathutil_mtxA_from_mtxB_translate(&miniCommendInfo.unk6C->unk30);
            mathutil_mtxA_rotate_y(-0x8000);
            u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
            avdisp_draw_model_culled_sort_translucent(miniCommendInfo.gma->modelEntries[3].model);
            avdisp_draw_model_culled_sort_translucent(miniCommendInfo.gma->modelEntries[11].model);
        }
        if (temp_r26 != NULL)
            u_avdisp_set_some_func_1(NULL);
        avdisp_set_post_mult_color(0.3f, 0.3f, 0.3f, 0.3f);
        avdisp_set_z_mode(1U, GX_LEQUAL, 0U);
        for (i = 0; i < miniCommendInfo.unk24; i++)
        {
            temp_r27 = miniCommendInfo.apePtrs[i];
            if (temp_r27 != NULL)
            {
                sp44.x = temp_r27->unk0->joints[0].transformMtx[0][3];
                sp44.y = 0.0f;
                sp44.z = temp_r27->unk0->joints[0].transformMtx[2][3];
                mathutil_mtxA_from_quat(&temp_r27->unk60);
                mathutil_mtxA_to_mtx(sp50);
                mathutil_mtxA_from_translate(&temp_r27->unk30);
                mathutil_mtxA_scale_xyz(temp_r27->modelScale, temp_r27->modelScale, temp_r27->modelScale);
                mathutil_mtxA_translate(&temp_r27->unk3C);
                mathutil_mtxA_mult_right(sp50);
                mathutil_mtxA_tf_point(&sp44, &sp44);
                mathutil_mtxA_from_identity();
                mathutil_mtxA_scale_s(0.95f);
                mathutil_mtxA_to_mtx(sp50);
                mathutil_mtxA_from_mtxB_translate(&sp44);
                mathutil_mtxA_mult_left(sp50);
                mathutil_mtxA_rotate_x(0x4000);
                switch (temp_r27->charaId)
                {
                case CHARACTER_BABY:
                    mathutil_mtxA_scale_s(0.2f);
                    break;
                default:
                    mathutil_mtxA_scale_s(0.25f);
                    break;
                }
                u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
                avdisp_set_alpha(0.25f);
                avdisp_draw_model_culled_sort_translucent(commonGma->modelEntries[78].model);
            }
        }
        avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1U, GX_LEQUAL, 1U);
        for (i = 0; i < miniCommendInfo.unk24 + 1; i++)
        {
            if (i == miniCommendInfo.unk24)
                var_r25_2 = miniCommendInfo.unk6C;
            else
                var_r25_2 = miniCommendInfo.apePtrs[i];
            if (var_r25_2 != NULL && (u8)var_r25_2->unk74 == 0)
            {
                mathutil_mtxA_from_quat(&var_r25_2->unk60);
                mathutil_mtxA_to_mtx(sp14);
                mathutil_mtxA_from_mtxB_translate(&var_r25_2->unk30);
                mathutil_mtxA_scale_xyz(var_r25_2->modelScale, var_r25_2->modelScale, var_r25_2->modelScale);
                mathutil_mtxA_translate(&var_r25_2->unk3C);
                mathutil_mtxA_mult_right(sp14);
                switch (var_r25_2->charaId)
                {
                case 0:
                    mathutil_mtxA_mult_right(var_r25_2->unk0->joints[0xA].transformMtx);
                    mathutil_mtxA_translate_xyz(0.04f, -0.17f, 0.03f);
                    mathutil_mtxA_rotate_z(0x1111);
                    mathutil_mtxA_rotate_y(0x416C);
                    mathutil_mtxA_rotate_x(0x127D);
                    break;
                case 1:
                    mathutil_mtxA_mult_right(var_r25_2->unk0->joints[0xA].transformMtx);
                    mathutil_mtxA_translate_xyz(0.03f, -0.15f, 0.03f);
                    mathutil_mtxA_rotate_y(0x4000);
                    break;
                case 2:
                    mathutil_mtxA_from_mtxB_translate(&var_r25_2->unk30);
                    break;
                case 3:
                    mathutil_mtxA_mult_right(var_r25_2->unk0->joints[0xF].transformMtx);
                    mathutil_mtxA_translate_xyz(0.07f, -0.16f, -0.03f);
                    mathutil_mtxA_rotate_z(-0x6D);
                    mathutil_mtxA_rotate_y(0x638E);
                    mathutil_mtxA_rotate_x(0x753);
                    break;
                }
                mathutil_mtxA_scale_xyz(0.1f, 0.1f, 0.1f);
                u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
                avdisp_draw_model_unculled_sort_translucent(miniCommendInfo.gma->modelEntries[7].model);
            }
        }
        for (i = 0; i < miniCommendInfo.unk24; i++)
            mot_ape_thread(miniCommendInfo.apePtrs[i], 0);
        if (miniCommendInfo.unk6C != NULL)
            mot_ape_thread(miniCommendInfo.unk6C, 0);
        if (miniCommendInfo.unk24 >= 2 && !(miniCommendInfo.unk0 & 8))
        {
            for (i = 0; i < miniCommendInfo.unk24; i++)
            {
                func_8009D7FC(i, &sp8);
                mathutil_mtxA_from_mtxB_translate(&sp8);
                mathutil_mtxA_translate_xyz(0.3f, -0.125f, -0.4f);
                mathutil_mtxA_rotate_y(-0x8000);
                mathutil_mtxA_scale_s(0.25f);
                u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
                if (miniCommendInfo.unk2D[i] != 0)
                {
                    temp_r3_2 = &lbl_801B7CF8[miniCommendInfo.unk29[i]];
                    avdisp_set_post_mult_color(temp_r3_2->r, temp_r3_2->g, temp_r3_2->b, 1.0f);
                    avdisp_draw_model_culled_sort_translucent(commonGma->modelEntries[88].model);
                    avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
                }
                else
                {
                    avdisp_draw_model_culled_sort_translucent(commonGma->modelEntries[lbl_802F59D4[miniCommendInfo.unk29[i]]].model);
                }
            }
        }
        if (func_8009D794() != 0)
        {
            func_8009BF74(miniCommendInfo.unk75);
            if (lbl_801D404C[miniCommendInfo.unk75].unkC & 1)
                func_8009BF74(1);
        }
        mathutil_mtx_copy(sp8C, mathutilData->mtxB);
    }
}

#define SOME_MACRO(a, b) (((a) << 18) & 0x1FC0000) | (b) | (((a) << 25) & 0xFE000000)

static void func_8009D3AC(struct Ape *ape, int status)
{
    s32 var_r0;
    u32 temp_r0;
    u32 temp_r3_2;
    u8 temp_r4;
    int temp_r5;
    u32 r4;

    if (status == THREAD_STATUS_KILLED)
    {
        if (status != THREAD_STATUS_KILLED)  // WTF? never true
            thread_exit();
        return;
    }

    if (debugFlags & 0xA)
        return;

    temp_r4 = ape->unk74 >> 16;
    temp_r5 = ape->unk74 & 0xFF;
    if (temp_r4 < (u32)(miniCommendInfo.unk24 / 2)
     && (temp_r5 == 2 || (ape->charaId == 1 && temp_r5 == 3)))
        ape->unk0->unk0 |= 4;
    new_ape_stat_motion(ape, 0xA, temp_r5, 0, 0.0f);
    new_ape_calc(ape);
    temp_r3_2 = func_8009D5F4();
    temp_r0 = 1 << (u8)(ape->unk74 >> 8);
    if (temp_r3_2 == temp_r0 || ((temp_r3_2 & temp_r0) && (rand() & 0x7FFF) % 3 == 0))
    {
        switch (ape->unk0->unk32)
        {
        case 0x172:
            var_r0 = ape->unk0->unk38 == 0x60;
            break;
        case 0x173:
            var_r0 = ape->unk0->unk38 == 0x4D;
            break;
        case 0x14F:
            var_r0 = ape->unk0->unk38 == 0x1B;
            break;
        case 0x174:
            var_r0 = ape->unk0->unk38 == 0x58;
            break;
        default:
            var_r0 = ape->unk0->unk38 == 1;
            break;
        }
        if (var_r0)
        {
            lbl_802F1DFC = ape->charaId;
            u_somePlayerId = r4 = 0;
            switch (ape->unk74 & 0xFF)
            {
            case 0:
                u_play_sound_0(SOME_MACRO(r4, 0x1B));
                return;
            case 1:
                u_play_sound_0(SOME_MACRO(r4, 0x58));
                return;
            case 2:
                u_play_sound_0(SOME_MACRO(r4, 0x59));
                return;
            case 3:
                u_play_sound_0(SOME_MACRO(r4, 0x1C));
                break;
            }
        }
    }
}

int func_8009D5D8(void)
{
    return (miniCommendInfo.unk0 & 1) != 0;
}

static u32 func_8009D5F4(void)
{
    int i;
    u32 var_r3;

    for (i = 0; i < miniCommendInfo.unk24; i++)
    {
        if (cameraInfo->timerCurr < (i + 1) * 0x110)
        {
            s32 var_r0 = i;
            if (miniCommendInfo.unk24 > 2)
            {
                if (var_r0 == 0)
                    var_r0 = 1;
                else if (var_r0 == 1)
                    var_r0 = 0;
            }
            return 1 << miniCommendInfo.unk29[var_r0];
        }
    }

    var_r3 = 0;
    for (i = 0; i < miniCommendInfo.unk24; i++)
        var_r3 |= 1 << miniCommendInfo.unk29[i];

    return var_r3;
}

static int func_8009D794(void)
{
    if (cameraInfo->timerCurr < miniCommendInfo.unk24 * 0x110)
        return 0;
    else
        return 1;
}

int func_8009D7CC(void)
{
    return (miniCommendInfo.unk0 & 0x26) == 0;
}

int func_8009D7E8(void)
{
    return miniCommendInfo.unk24;
}

void func_8009D7FC(int arg0, Vec *arg1)
{
    arg1->x = (0.5 * (miniCommendInfo.unk24 - 1)) - arg0 - 0.5;
    arg1->y = lbl_80171B60[miniCommendInfo.unk25[arg0]];
    if (miniCommendInfo.unk0 & 0x20)
        arg1->y = *lbl_80171B60;
    arg1->z = 0.0f;
}

#pragma force_active on
void func_8009D8A4(int arg0, Vec *arg1)
{
    int i;

    arg1->x = 0.0f;
    arg1->y = 0.0f;
    arg1->z = 0.0f;
    for (i = 0; i < miniCommendInfo.unk24; i++)
    {
        if (arg0 == miniCommendInfo.unk29[i])
        {
            func_8009D7FC(i, arg1);
            break;
        }
    }
}
#pragma force_active reset

void func_8009D98C(Vec *arg0)
{
    int i;
    Vec vec1;
    Vec vec2;
    Vec vec3;

    for (i = 0; i < miniCommendInfo.unk24; i++)
    {
        func_8009D7FC(i, &vec3);
        if (i == 0)
        {
            vec1.x = vec2.x = vec3.x;
            vec1.y = vec2.y = vec3.y;
            vec1.z = vec2.z = vec3.z;
            if (vec1.y >= 0.0)
                vec1.y = 0.0;
            continue;
        }
        vec1.x = MIN(vec1.x, vec3.x);
        vec1.y = MIN(vec1.y, vec3.y);
        vec1.z = MIN(vec1.z, vec3.z);
        vec2.x = MAX(vec2.x, vec3.x);
        vec2.y = MAX(vec2.y, vec3.y);
        vec2.z = MAX(vec2.z, vec3.z);
    }
    if (miniCommendInfo.unk6C != NULL)
    {
        vec1.y = MIN(vec1.y, miniCommendInfo.unk6C->unk30.y);
        vec2.y = MAX(vec2.y, miniCommendInfo.unk6C->unk30.y);
    }
    arg0->x = 0.5 * (vec1.x + vec2.x);
    arg0->y = 0.5 * (vec1.y + vec2.y);
    arg0->z = 0.5 * (vec1.z + vec2.z);
}

static void func_8009DB40(Mtx arg0)
{
    mathutil_mtx_copy(arg0, miniCommendInfo.unk3C);
}

void func_8009DB6C(Mtx arg0)
{
    mathutil_mtx_copy(miniCommendInfo.unk3C, arg0);
}

#pragma force_active on
void func_8009DB9C(int arg0)
{
    miniCommendInfo.unk75 = arg0;
}
#pragma force_active reset

int func_8009DBB0(Vec *arg0, struct Struct8009DBB0 *arg1, Vec *arg2)
{
    int i;
    Vec vec1;
    int count;

    arg1->unk0 = 0;
    arg2->x = 0.0f;
    arg2->y = 0.0f;
    arg2->z = 0.0f;
    arg1->unk4.x = arg0->x;
    arg1->unk4.z = arg0->z;
    arg1->unk10.x = 0.0f;
    arg1->unk10.y = 1.0f;
    arg1->unk10.z = 0.0f;
    if (-2.5 <= arg0->x && arg0->x <= 1.5
     && -0.5 <= arg0->z && arg0->z <= 0.5)
    {
        arg1->unk0 = 1;
        arg1->unk4.y = -0.05f;
    }

    count = miniCommendInfo.unk24;
    for (i = 0; i < count; i++)
    {
        func_8009D7FC(i, &vec1);
        if (!(arg0->x < vec1.x - 0.5)
         && !(arg0->z < vec1.z - 0.5)
         && !(arg0->x > vec1.x + 0.5)
         && !(arg0->z > vec1.z + 0.5))
        {
            arg1->unk0 = 1;
            arg1->unk4.y = vec1.y;
        }
    }
    if (miniCommendInfo.unk6C != NULL)
    {
        struct Ape *ape = miniCommendInfo.unk6C;
        if (arg0->x >= ape->unk30.x - 0.5
         && arg0->z >= ape->unk30.z - 0.5
         && arg0->x <= ape->unk30.x + 0.5
         && arg0->z <= ape->unk30.z + 0.5)
        {
            arg1->unk0 = 1;
            arg1->unk4.y = ape->unk30.y;
        }
    }
    return (arg1->unk0 & 1) != 0;
}

#pragma force_active on
void func_8009DDC4(int arg0)
{
    switch (arg0)
    {
    case 0:
        miniCommendInfo.unk0 |= 0x40;
        miniCommendInfo.unk0 &= 0xFFFFFF7F;
        return;
    case 1:
        miniCommendInfo.unk0 &= 0xFFFFFFBF;
        miniCommendInfo.unk0 &= 0xFFFFFF7F;
        return;
    case 2:
        miniCommendInfo.unk0 &= 0xFFFFFFBF;
        miniCommendInfo.unk0 |= 0x80;
        return;
    }
}
#pragma force_active reset
