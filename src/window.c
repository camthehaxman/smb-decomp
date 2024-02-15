/**
 * window.c - Implements a GUI windowing system used for debug menus
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "bitmap.h"
#include "camera.h"
#include "event.h"
#include "gxcache.h"
#include "input.h"
#include "light.h"
#include "mode.h"
#include "pool.h"
#include "sound.h"
#include "sprite.h"

extern struct Light lbl_801F3A08;

extern u32 lbl_802F1E48;
extern u32 lbl_802F1E4C;
extern s8 lbl_802F1E50;
extern s8 lbl_802F1E51;

extern s8 lbl_802F1C6C[8];

extern struct WindowDesc lbl_801B7474;

extern u32 lbl_802F1E30;
extern u32 lbl_802F1E34;
extern u32 lbl_802F1E38;
extern u32 lbl_802F1E3C;

extern s32 lbl_802F1E54;
extern s32 lbl_802F1E58;

extern struct Color3f lbl_801F39FC;

u8 lbl_80201928[0x7AC];  FORCE_BSS_ORDER(lbl_80201928)  // 0, size = 0x7A9
u8 lbl_802020D4[0x7AC];  FORCE_BSS_ORDER(lbl_802020D4)  // 0x7AC, size = 0x7A9
u8 lbl_80202880[0x7AC];  FORCE_BSS_ORDER(lbl_80202880)  // 0xF58, size = 0x7A9
u8 lbl_80201928_1704[0x7AC]; FORCE_BSS_ORDER(lbl_80201928_1704)  // 0x1704, size = 0x7A9
u8 lbl_802037D8[0x7AC];  FORCE_BSS_ORDER(lbl_802037D8)  // 0x1EB0, size = 0x7A9
u8 lbl_80203F84[0x7AC];  FORCE_BSS_ORDER(lbl_80203F84)  // 0x265C, size = 0x7A9
u8 lbl_80203F84_2[0x7AC];  FORCE_BSS_ORDER(lbl_80203F84_2)  // 0x2E08
u8 lbl_80203F84_3[0x7AC];  FORCE_BSS_ORDER(lbl_80203F84_3)  // 0x35B4

struct RangeFloat
{
    u32 flags;
    float min, max;
    float delta;
};

struct RangeInt
{
    u32 flags;
    s32 min, max;
    s32 delta;
};

// specifies the parameter to the format string
enum WindowItemType
{
    ITEM_BIT     =  8,  // %s string "ON" if (*pValue & param) is nonzero, or "OFF" if it is zero
    ITEM_NONE    = 13,  // no format parameters
    ITEM_FLOAT32 = 20,  // %f float specified by *pValue
    ITEM_STRING  = 22,  // %s string specified by *pValue
    ITEM_END     = 31,  // end of item list
};

struct WindowItem
{
    u32 type;  // enum WindowItemType
    s32 unk4;
    s32 unk8;
    char *format;
    void *pValue;
    void *param;  // usage of this depends on "type"
};  // size = 0x18

struct WindowDesc
{
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    struct WindowItem *items;
    u32 structSize;
    u32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
};

static char *lbl_802F0828[] = { "OFF", "ON" };

// .data

struct RangeInt   lbl_801B3B18 = { 3, 0, 100, 1 };
struct RangeFloat posScaleRange = { 3, -10.0f, 10.0f, 0.1f };
struct RangeInt   angleRange = { 3, 0, 0, 128 };
struct RangeInt   lbl_801B3B78 = { 0, 0, 1, 1 };

struct WindowItem lbl_801B3D98[] =
{
    {ITEM_NONE, 1,  1, "Dip Switch",           NULL,         NULL},
    {ITEM_BIT,  2,  3, "                  %s", &dipSwitches, (void *)DIP_DEBUG},
    {ITEM_BIT,  2,  4, "                  %s", &dipSwitches, (void *)DIP_DISP},
    {ITEM_BIT,  2,  5, "                  %s", &dipSwitches, (void *)DIP_STCOLI},
    {ITEM_BIT,  2,  6, "                  %s", &dipSwitches, (void *)DIP_TRIANGLE},
    {ITEM_BIT,  2,  8, "                  %s", &dipSwitches, (void *)DIP_BALL_TGT},
    {ITEM_BIT,  2,  9, "                  %s", &dipSwitches, (void *)DIP_JOINT_OFF},
    {ITEM_BIT,  2, 10, "                  %s", &dipSwitches, (void *)DIP_APE_COLOR},
    {ITEM_BIT,  2, 11, "                  %s", &dipSwitches, (void *)DIP_TIME_STOP},
    {ITEM_BIT,  2, 13, "                  %s", &dipSwitches, (void *)DIP_KARAKUCHI},
    {ITEM_BIT,  2, 14, "                  %s", &dipSwitches, (void *)DIP_NAMEENTRY},
    {ITEM_BIT,  2, 15, "                  %s", &dipSwitches, (void *)DIP_SHADOW_TEST},
    {ITEM_BIT,  2, 16, "                  %s", &dipSwitches, (void *)DIP_FALL_DISP},
    {ITEM_BIT,  2, 18, "                  %s", &dipSwitches, (void *)DIP_TEST_CAM},
    {ITEM_BIT,  2, 19, "                  %s", &dipSwitches, (void *)DIP_NO_INTR},
    {ITEM_BIT,  2, 20, "                  %s", &dipSwitches, (void *)DIP_CAPTURE},
    {ITEM_BIT,  2, 21, "                  %s", &dipSwitches, (void *)DIP_OLD_BALL},
    {ITEM_BIT,  2, 23, "                  %s", &dipSwitches, (void *)DIP_NO_KARI},
    {ITEM_BIT,  2, 24, "                  %s", &dipSwitches, (void *)DIP_PLAY_STG_ALL},
    {ITEM_BIT,  2, 25, "                  %s", &dipSwitches, (void *)DIP_PLAY_PNT_X10},
    {ITEM_BIT,  2, 26, "                  %s", &dipSwitches, (void *)DIP_SARU_0},
    {ITEM_BIT,  2, 28, "                  %s", &dipSwitches, (void *)DIP_SWITCH20},
    {ITEM_BIT,  2, 29, "                  %s", &dipSwitches, (void *)DIP_SWITCH21},
    {ITEM_BIT,  2, 30, "                  %s", &dipSwitches, (void *)DIP_SWITCH22},
    {ITEM_BIT,  2, 31, "                  %s", &dipSwitches, (void *)DIP_SWITCH23},
    {ITEM_BIT,  2, 33, "                  %s", &dipSwitches, (void *)DIP_SWITCH24},
    {ITEM_BIT,  2, 34, "                  %s", &dipSwitches, (void *)DIP_SWITCH25},
    {ITEM_BIT,  2, 35, "                  %s", &dipSwitches, (void *)DIP_SWITCH26},
    {ITEM_BIT,  2, 36, "                  %s", &dipSwitches, (void *)DIP_SWITCH27},
    {ITEM_BIT,  2, 38, "                  %s", &dipSwitches, (void *)DIP_SWITCH28},
    {ITEM_BIT,  2, 39, "                  %s", &dipSwitches, (void *)DIP_SWITCH29},
    {ITEM_BIT,  2, 40, "                  %s", &dipSwitches, (void *)DIP_SWITCH30},
    {ITEM_BIT,  2, 41, "                  %s", &dipSwitches, (void *)DIP_SWITCH31},
    {ITEM_NONE, 3,  3, "DIP_DEBUG",            NULL,         NULL},
    {ITEM_NONE, 3,  4, "DIP_DISP",             NULL,         NULL},
    {ITEM_NONE, 3,  5, "DIP_STCOLI",           NULL,         NULL},
    {ITEM_NONE, 3,  6, "DIP_TRIANGLE",         NULL,         NULL},
    {ITEM_NONE, 3,  8, "DIP_BALL_TGT",         NULL,         NULL},
    {ITEM_NONE, 3,  9, "DIP_JOINT_OFF",        NULL,         NULL},
    {ITEM_NONE, 3, 10, "DIP_APE_COLOR",        NULL,         NULL},
    {ITEM_NONE, 3, 11, "DIP_TIME_STOP",        NULL,         NULL},
    {ITEM_NONE, 3, 13, "DIP_KARAKUCHI",        NULL,         NULL},
    {ITEM_NONE, 3, 14, "DIP_NAMEENTRY",        NULL,         NULL},
    {ITEM_NONE, 3, 15, "DIP_SHADOW_TEST",      NULL,         NULL},
    {ITEM_NONE, 3, 16, "DIP_FALL_DISP",        NULL,         NULL},
    {ITEM_NONE, 3, 18, "DIP_TEST_CAM",         NULL,         NULL},
    {ITEM_NONE, 3, 19, "DIP_NO_INTR",          NULL,         NULL},
    {ITEM_NONE, 3, 20, "DIP_CAPTURE",          NULL,         NULL},
    {ITEM_NONE, 3, 21, "DIP_OLD_BALL",         NULL,         NULL},
    {ITEM_NONE, 3, 23, "DIP_NO_KARI",          NULL,         NULL},
    {ITEM_NONE, 3, 24, "DIP_PLAY_STG_ALL",     NULL,         NULL},
    {ITEM_NONE, 3, 25, "DIP_PLAY_PNT_x10",     NULL,         NULL},
    {ITEM_NONE, 3, 26, "DIP_SARU_0",           NULL,         NULL},
    {ITEM_NONE, 3, 28, "DIP_SWITCH20",         NULL,         NULL},
    {ITEM_NONE, 3, 29, "DIP_SWITCH21",         NULL,         NULL},
    {ITEM_NONE, 3, 30, "DIP_SWITCH22",         NULL,         NULL},
    {ITEM_NONE, 3, 31, "DIP_SWITCH23",         NULL,         NULL},
    {ITEM_NONE, 3, 33, "DIP_SWITCH24",         NULL,         NULL},
    {ITEM_NONE, 3, 34, "DIP_SWITCH25",         NULL,         NULL},
    {ITEM_NONE, 3, 35, "DIP_SWITCH26",         NULL,         NULL},
    {ITEM_NONE, 3, 36, "DIP_SWITCH27",         NULL,         NULL},
    {ITEM_NONE, 3, 38, "DIP_SWITCH28",         NULL,         NULL},
    {ITEM_NONE, 3, 39, "DIP_SWITCH29",         NULL,         NULL},
    {ITEM_NONE, 3, 40, "DIP_SWITCH30",         NULL,         NULL},
    {ITEM_NONE, 3, 41, "DIP_SWITCH31",         NULL,         NULL},
    {ITEM_END, 0,  0, NULL,                   NULL,         NULL},
};

struct WindowDesc dipSwitchWindow = { 9, 0, 25, 29, lbl_801B3D98, 0, 0, 0, 0, 0, 0 };

struct WindowItem lbl_801B43F4[] =
{
    {ITEM_NONE, 1, 1, "Game", NULL, NULL},
    {ITEM_END, 0, 0, NULL,   NULL, NULL},
};

struct WindowDesc lbl_801B4424 = { 0, 0, 10, 10, lbl_801B43F4, 0, 0, 0, 0, 0, 0 };

struct WindowItem lbl_801B4480[] =
{
    {ITEM_NONE, 1, 1, "Mode",          NULL,              NULL},
    {17, 3, 3, "main_mode: %d", &gameMode,         NULL},
    {ITEM_STRING, 3, 4, "  %-14s",       &modeStringPtr,    NULL},
    {17, 3, 5, " sub_mode: %d", &gameSubmode,      NULL},
    {ITEM_STRING, 3, 6, "  %-14s",       &submodeStringPtr, NULL},
    {15, 3, 8, "mode.cntr: %d", &modeCtrl,         NULL},
    {ITEM_END, 0, 0, NULL,            NULL,              NULL},
};

struct WindowDesc modeWindow = { 33, 0, 20, 11, lbl_801B4480, 0, 0, 0, 0, 0, 0 };

struct WindowItem lbl_801B4554[] =
{
    {ITEM_NONE,  1,  1, "Event", NULL,                NULL},
    {ITEM_STRING,  2,  3, "%10s",  &eventInfo[0].name,  NULL},
    {ITEM_STRING,  2,  4, "%10s",  &eventInfo[1].name,  NULL},
    {ITEM_STRING,  2,  5, "%10s",  &eventInfo[2].name,  NULL},
    {ITEM_STRING,  2,  6, "%10s",  &eventInfo[3].name,  NULL},
    {ITEM_STRING,  2,  7, "%10s",  &eventInfo[4].name,  NULL},
    {ITEM_STRING,  2,  8, "%10s",  &eventInfo[5].name,  NULL},
    {ITEM_STRING,  2,  9, "%10s",  &eventInfo[6].name,  NULL},
    {ITEM_STRING,  2, 10, "%10s",  &eventInfo[7].name,  NULL},
    {ITEM_STRING,  2, 11, "%10s",  &eventInfo[8].name,  NULL},
    {ITEM_STRING,  2, 12, "%10s",  &eventInfo[9].name,  NULL},
    {ITEM_STRING,  2, 13, "%10s",  &eventInfo[10].name, NULL},
    {ITEM_STRING,  2, 14, "%10s",  &eventInfo[11].name, NULL},
    {ITEM_STRING,  2, 15, "%10s",  &eventInfo[12].name, NULL},
    {ITEM_STRING,  2, 16, "%10s",  &eventInfo[13].name, NULL},
    {ITEM_STRING,  2, 17, "%10s",  &eventInfo[14].name, NULL},
    {ITEM_STRING,  2, 18, "%10s",  &eventInfo[15].name, NULL},
    {ITEM_STRING,  2, 19, "%10s",  &eventInfo[16].name, NULL},
    {ITEM_STRING,  2, 20, "%10s",  &eventInfo[17].name, NULL},
    {ITEM_STRING,  2, 21, "%10s",  &eventInfo[18].name, NULL},
    {ITEM_STRING,  2, 22, "%10s",  &eventInfo[19].name, NULL},
    {ITEM_STRING,  2, 23, "%10s",  &eventInfo[20].name, NULL},
    {25, 13,  3, "%s",    &eventInfo[0],       &u_stateNames},
    {25, 13,  4, "%s",    &eventInfo[1],       &u_stateNames},
    {25, 13,  5, "%s",    &eventInfo[2],       &u_stateNames},
    {25, 13,  6, "%s",    &eventInfo[3],       &u_stateNames},
    {25, 13,  7, "%s",    &eventInfo[4],       &u_stateNames},
    {25, 13,  8, "%s",    &eventInfo[5],       &u_stateNames},
    {25, 13,  9, "%s",    &eventInfo[6],       &u_stateNames},
    {25, 13, 10, "%s",    &eventInfo[7],       &u_stateNames},
    {25, 13, 11, "%s",    &eventInfo[8],       &u_stateNames},
    {25, 13, 12, "%s",    &eventInfo[9],       &u_stateNames},
    {25, 13, 13, "%s",    &eventInfo[10],      &u_stateNames},
    {25, 13, 14, "%s",    &eventInfo[11],      &u_stateNames},
    {25, 13, 15, "%s",    &eventInfo[12],      &u_stateNames},
    {25, 13, 16, "%s",    &eventInfo[13],      &u_stateNames},
    {25, 13, 17, "%s",    &eventInfo[14],      &u_stateNames},
    {25, 13, 18, "%s",    &eventInfo[15],      &u_stateNames},
    {25, 13, 19, "%s",    &eventInfo[16],      &u_stateNames},
    {25, 13, 20, "%s",    &eventInfo[17],      &u_stateNames},
    {25, 13, 21, "%s",    &eventInfo[18],      &u_stateNames},
    {25, 13, 22, "%s",    &eventInfo[19],      &u_stateNames},
    {25, 13, 23, "%s",    &eventInfo[20],      &u_stateNames},
    {ITEM_END,  0,  0, NULL,    NULL,                NULL},
};

struct WindowDesc eventWindow = {0, 11, 27, 26, lbl_801B4554, 0, 0, 0, 0, 0, 0};

struct RangeInt lbl_801B49A0 = {0, 0, 3, 1};

struct WindowItem lbl_801B4A38[] =
{
    {ITEM_NONE, 1,  1, "Camera",         NULL,                    NULL},
    {10, 2,  3, "id:       [%d]", NULL,                    &lbl_801B49A0},
    {ITEM_FLOAT32, 2, -1, "camx:%8.3f",     &cameraInfo[0].eye.x,    NULL},
    {ITEM_FLOAT32, 2, -1, "camy:%8.3f",     &cameraInfo[0].eye.y,    NULL},
    {ITEM_FLOAT32, 2, -1, "camz:%8.3f",     &cameraInfo[0].eye.z,    NULL},
    {ITEM_FLOAT32, 2, -1, "intx:%8.3f",     &cameraInfo[0].lookAt.x, NULL},
    {ITEM_FLOAT32, 2, -1, "inty:%8.3f",     &cameraInfo[0].lookAt.y, NULL},
    {ITEM_FLOAT32, 2, -1, "intz:%8.3f",     &cameraInfo[0].lookAt.z, NULL},
    {16, 2, -1, "angx:    %04hX", &cameraInfo[0].rotX,     NULL},
    {16, 2, -1, "angy:    %04hX", &cameraInfo[0].rotY,     NULL},
    {16, 2, -1, "angz:    %04hX", &cameraInfo[0].rotZ,     NULL},
    {ITEM_END, 0,  0, NULL,             NULL,                    NULL},
};

struct WindowDesc cameraWindow = {36, 22, 17, 15, lbl_801B4A38, sizeof(struct Camera), 0, 0, 0, 0, 0};

struct WindowItem lbl_801B4B84[] =
{
    {ITEM_NONE, 1, 1, "Effect", NULL, NULL},
    {ITEM_END, 0, 0, NULL,     NULL, NULL},
};

struct WindowDesc effectWindow = {0, 0, 10, 10, lbl_801B4B84, 0, 0, 0, 0, 0, 0};

struct RangeFloat lbl_801B4BE0 = { 3, 0.0f, 640.0f, 1.0f };
struct RangeInt   lbl_801B4BF0 = { 0, 0, 63, 1 };
struct RangeInt   lbl_801B4C00 = { 0, 0, 178, 1 };
struct RangeInt   lbl_801B4C10 = { 0, 0, 8, 1 };
struct RangeInt   lbl_801B4C20 = { 0, 0, 255, 1 };
struct RangeFloat lbl_801B4C30 = { 0, 0.0f, 1.0f, 0.01f };

struct WindowItem lbl_801B4D94[] =
{
    {ITEM_NONE, 1,  1, "Sprite",            NULL,                     NULL},
    {10, 3,  3, "Sprite ID : %3d",   NULL,                     &lbl_801B4BF0},
    {30, 3, -1, "%15s",              &g_poolInfo.spriteBuf,    NULL},
    {21, 3, -2, "%s",                &spriteWork[0].text,      NULL},
    { 7, 3, -1, "locate_dx : %4.1f", &spriteWork[0].x,         &lbl_801B4BE0},
    { 7, 3, -1, "locate_dy : %4.1f", &spriteWork[0].y,         &lbl_801B4BE0},
    { 7, 3, -1, "    depth : %4.1f", &spriteWork[0].unk4C,     &posScaleRange},
    { 5, 3, -1, "     font : %3d",   &spriteWork[0].fontId,    &lbl_801B4C00},
    {25, 3, -1, "  %s",              &spriteWork[0].fontId,    &fontStrArray},
    { 6, 3, -1, "    align : %3d",   &spriteWork[0].textAlign, &lbl_801B4C10},
    { 5, 3, -1, "      red :  %02X", &spriteWork[0].mulR,      &lbl_801B4C20},
    { 5, 3, -1, "    green :  %02X", &spriteWork[0].mulG,      &lbl_801B4C20},
    { 5, 3, -1, "     bule :  %02X", &spriteWork[0].mulB,      &lbl_801B4C20},
    { 5, 3, -1, "ofs   red :  %02X", &spriteWork[0].addR,      &lbl_801B4C20},
    { 5, 3, -1, "ofs green :  %02X", &spriteWork[0].addG,      &lbl_801B4C20},
    { 5, 3, -1, "ofs  bule :  %02X", &spriteWork[0].addB,      &lbl_801B4C20},
    { 4, 3, -1, "     zang : %04hX", &spriteWork[0].rotation,  &angleRange},
    { 7, 3, -1, "     zm_x : %4.1f", &spriteWork[0].scaleX,    &posScaleRange},
    { 7, 3, -1, "     zm_y : %4.1f", &spriteWork[0].scaleY,    &posScaleRange},
    { 7, 3, -1, "    trnsl : %4.2f", &spriteWork[0].opacity,   &lbl_801B4C30},
    {17, 3, -2, "    timer : %5d",   &spriteWork[0].counter,   NULL},
    {15, 3, -1, "    para1 : %5d",   &spriteWork[0].userVar,   NULL},
    {ITEM_END, 0,  0, NULL,                NULL,                     NULL},
};

struct WindowDesc spriteWindow = {0, 0, 24, 30, lbl_801B4D94, sizeof(struct Sprite), 0, 0, 0, 0, 0};

char *lbl_801B503C[] =
{
    "GX_SP_OFF",
    "GX_SP_FLAT",
    "GX_SP_COS",
    "GX_SP_COS2",
    "GX_SP_SHARP",
    "GX_SP_RING1",
    "GX_SP_RING2",
};

struct RangeInt   lbl_801B5058 = { 0, 0, 31, 1 };
struct RangeInt   lbl_801B5068 = { 0, 0, 7, 1 };
struct RangeInt   lbl_801B5078 = { 0, 0, 8, 1 };
struct RangeFloat lbl_801B5088 = { 0, 0.0f, 1.0f, 0.01f };
struct RangeFloat lbl_801B5098 = { 2, 0.1f, 1.0f, 0.1f };
struct RangeFloat lbl_801B50A8 = { 3, 0.0f, 1.0f, 0.1f };
struct RangeFloat lbl_801B50B8 = { 3, 0.0f, 1.0f, 0.01f };
struct RangeFloat lbl_801B50C8 = { 3, 0.0f, 1.0f, 0.01f };
struct RangeInt   lbl_801B50D8 = { 0, 0, 6, 1 };
struct RangeFloat lbl_801B50E8 = { 0, 0.0f, 90.0f, 0.1f };
struct RangeInt   lbl_801B50F8 = { 0, 1, 199, 1 };

struct WindowItem lbl_801B5170[] =
{
    {ITEM_NONE,  1,  1, "Light Param",  NULL,                     NULL},
    { 2,  2,  3, "data ID : %d", &u_lightToPrint,          &lbl_801B5058},
    { 6,  2, -2, "stat : ",      &lbl_801F3A08,            &lbl_801B3B78},
    {25,  9,  0, "%s",           &lbl_801F3A08,            &lbl_802F0828},
    { 6,  2, -1, "name : ",      &lbl_801F3A08.u_id,       &lbl_801B5068},
    {25,  9,  0, "%s",           &lbl_801F3A08.u_id,       &s_lightIdNames},
    { 4,  2, -1, "sub  : %d",    &lbl_801F3A08.u_inst,     &lbl_801B3B18},
    { 6,  2, -1, "type : ",      &lbl_801F3A08.type,       &lbl_801B5078},
    {25,  9,  0, "%s",           &lbl_801F3A08.type,       &lightTypeNames},
    {ITEM_NONE,  2, -2, "RGB",          NULL,                     NULL},
    { 7,  3, -1, "%4.2f",        &lbl_801F3A08.red,        NULL},
    { 7,  8,  0, "%4.2f",        &lbl_801F3A08.green,      NULL},
    { 7, 13,  0, "%4.2f",        &lbl_801F3A08.blue,       NULL},
    { 7,  2, -2, "px:%f",        &lbl_801F3A08.pos.x,      &posScaleRange},
    { 7,  2, -1, "py:%f",        &lbl_801F3A08.pos.y,      &posScaleRange},
    { 7,  2, -1, "pz:%f",        &lbl_801F3A08.pos.z,      &posScaleRange},
    { 4,  2, -2, "xa:%04X",      &lbl_801F3A08.rotX,       &angleRange},
    { 4,  2, -1, "ya:%04X",      &lbl_801F3A08.rotY,       &angleRange},
    { 7,  2, -2, "leng:%5.1f",   &lbl_801F3A08.refDist,    &lbl_801B5098},
    { 7,  2, -1, "att0:%5.2f",   &lbl_801F3A08.k0,         &lbl_801B50A8},
    { 7,  2, -1, "att1:%5.2f",   &lbl_801F3A08.k1,         &lbl_801B50B8},
    { 7,  2, -1, "att2:%5.2f",   &lbl_801F3A08.k2,         &lbl_801B50C8},
    { 6,  2, -2, "cone  :",      &lbl_802F1E50,            &lbl_801B50D8},
    {25,  9,  0, "%s",           &lbl_802F1E50,            &lbl_801B503C},
    { 7,  2, -1, "cutoff:%4.1f", &lbl_801F3A08.spotCutoff, &lbl_801B50E8},
    { 6,  2, -2, "DISP:",        &lbl_802F1C8C,            &lbl_801B3B78},
    {25,  7,  0, "%s",           &lbl_802F1C8C,            &lbl_802F0828},
    { 6,  2, -1, "\"SAVE\"",     &u_printLight,            &lbl_801B3B78},
    { 6,  2, -1, "\"COPY\"",     &lbl_802F1C84,            &lbl_801B3B78},
    { 2,  9,  0, "%2d / ",       &lbl_802F1C80,            &lbl_801B5058},
    { 2, 14,  0, "st%03d",       &lightingStageId,         &lbl_801B50F8},
    {ITEM_END,  0,  0, NULL,           NULL,                     },
};

struct WindowDesc lightParamWindow = {0, 0, 21, 33, lbl_801B5170, 0, 0, 0, 0, 0, 0};

struct RangeInt lbl_801B549C = { 0, 0, 21, 1 };
struct RangeInt lbl_801B54AC = { 0, -1, 31, 1 };

struct WindowItem lbl_801B5538[] =
{
    {ITEM_NONE,  1,  1, "Light Group",   NULL, 0},
    { 6,  2,  3, "group ID : %d", &lbl_802F1C75,               &lbl_801B549C},
    {25,  3, -1, "%s",            &lbl_802F1C75,               &s_lightGroupNames},
    { 6,  3, -2, "edit : ",       &s_u_lightGroupsInitialized, &lbl_801B3B78},
    {25, 10,  0, "%s",            &s_u_lightGroupsInitialized, &lbl_802F0828},
    { 6,  3, -2, "%2d",           &lbl_802F1C6C[0],            &lbl_801B54AC},
    { 6, -3,  0, "%2d",           &lbl_802F1C6C[1],            &lbl_801B54AC},
    { 6, -3,  0, "%2d",           &lbl_802F1C6C[2],            &lbl_801B54AC},
    { 6, -3,  0, "%2d",           &lbl_802F1C6C[3],            &lbl_801B54AC},
    { 6,  3, -1, "%2d",           &lbl_802F1C6C[4],            &lbl_801B54AC},
    { 6, -3,  0, "%2d",           &lbl_802F1C6C[5],            &lbl_801B54AC},
    { 6, -3,  0, "%2d",           &lbl_802F1C6C[6],            &lbl_801B54AC},
    { 6, -3,  0, "%2d",           &lbl_802F1C6C[7],            &lbl_801B54AC},
    { 7,  3, -2, "amb r: %4.2f",  &lbl_801F39FC.r,             &lbl_801B5088},
    { 7,  0, -1, "amb g: %4.2f",  &lbl_801F39FC.g,             &lbl_801B5088},
    { 7,  0, -1, "amb b: %4.2f",  &lbl_801F39FC.b,             &lbl_801B5088},
    {15,  2, -2, "CalcLgtGrp:%d", &lbl_802F1C68,               NULL},
    {15,  2, -1, "    RegSet:%d", &lbl_802F1C64,               NULL},
    {15,  2, -1, "SetLgtGrp :%d", &lbl_802F1C60,               NULL},
    {15,  2, -1, "    RegSet:%d", &s_numLightObjsLoaded,       NULL},
    {ITEM_END,  0,  0, 0,               NULL,                        NULL},
};

struct WindowDesc lightGroupWindow = { 36, 0, 17, 16, lbl_801B5538, 0, 0, 0, 0, 0, 0 };

char *lbl_801B57B8[] =
{
    "GX_FOG_NONE",
    "no define",
    "GX_FOG_LIN",
    "no define",
    "GX_FOG_EXP",
    "GX_FOG_EXP2",
    "GX_FOG_REVEXP",
    "GX_FOG_REVEXP2",
};

struct RangeInt   lbl_801B57D8 = { 0, 0, 7, 1 };
struct RangeFloat lbl_801B57E8 = { 3, -10.0f, 10.0f, 1.0f };
struct RangeInt   lbl_801B57F8 = { 0, 0, 255, 1 };

struct WindowItem fogWindowItems[] =
{
    {ITEM_NONE,  1,  1, "Fog",           NULL,                NULL},
    { 6,  2, -2, "sw : ",         &fogInfo.enabled, &lbl_801B3B78},
    {25,  7,  0, "%s",            &fogInfo.enabled, &lbl_802F0828},
    { 6,  2, -2, "type : ",       &fogInfo.unk0,    &lbl_801B57D8},
    {25,  9,  0, "%s",            &fogInfo.unk0,    &lbl_801B57B8},
    { 7,  2, -2, "start : %5.1f", &fogInfo.unk4,    &lbl_801B57E8},
    { 7,  2, -1, "end   : %5.1f", &fogInfo.unk8,    &lbl_801B57E8},
    { 5,  2, -2, "color r :%3d",  &fogInfo.r,       &lbl_801B57F8},
    { 5,  2, -1, "color g :%3d",  &fogInfo.g,       &lbl_801B57F8},
    { 5,  2, -1, "color b :%3d",  &fogInfo.b,       &lbl_801B57F8},
    {ITEM_END,  0,  0, NULL,            NULL,             NULL},
};

struct WindowDesc fogWindow = { 0, 0, 28, 16, fogWindowItems, 0, 0, 0, 0, 0, 0 };

struct RangeInt lbl_801B598C = { 0, 0, 100, 5 };
struct RangeInt lbl_801B599C = { 0, 0, 16, 1 };
struct RangeInt lbl_801B59AC = { 0, 0, 1061, 1 };
struct RangeInt lbl_801B59BC = { 0, -128, 127, 1 };
struct RangeInt lbl_801B59CC = { 3, 0, 100, 1 };

struct WindowDesc *lbl_80205688[16];  // 0x3D60
FORCE_BSS_ORDER(lbl_80205688)

struct WindowDesc bss_3DA0[16];
FORCE_BSS_ORDER(bss_3DA0)

extern int lbl_802F1E08;  // s32 vs int actually matters here
extern u32 lbl_802F1EA8;

void func_80030030(int arg0, int arg1, int arg2, int arg3);
void draw_some_window_quad_1(int arg0, int arg1, s8 arg2, u8 arg3);
void draw_some_window_quad_2(float x1, float y1, float x2, float y2);

void func_8002DC54(void)
{
    struct WindowDesc **var_r3;
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
    struct WindowDesc **r6 = &lbl_80205688[lbl_802F1E08];
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
        struct WindowDesc **r3 = lbl_80205688;
        struct WindowDesc *r4 = *r3;
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

#pragma dont_inline on
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
#pragma dont_inline reset

void func_8002E06C(struct WindowDesc *arg0)
{

    struct WindowDesc *var_r4;
    //int var_r6;
    int i;
    #define var_r6 i
    void **r6;
    struct WindowDesc *var_r7;

    lbl_802F1EA8 = 1;
    if (lbl_80205688[15] == NULL)
    {
        //var_r4 = lbl_80205688.unk40;
        for (var_r6 = 0; var_r6 < 16; var_r6++)
        {
            if (bss_3DA0[var_r6].unk20 == 0)
                break;
        }
        if (var_r6 < 16)
        {
            var_r7 = &bss_3DA0[var_r6];
            //var_r7 = var_r7;
            //someinline(var_r7, arg0);
            //*var_r7 = *arg0;
            var_r7->unk0 = arg0->unk0;
            var_r7->unk4 = arg0->unk4;
            var_r7->unk8 = arg0->unk8;
            var_r7->unkC = arg0->unkC;
            var_r7->items = arg0->items;
            var_r7->structSize = arg0->structSize;
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
extern s32 lbl_802F1E40;
extern s32 lbl_802F1E44;

void process_window(struct WindowDesc *window)
{
    void *temp_r4_6;
    f32 somefloatval;
    f32 minFloat;
    f32 maxFloat;
    f32 deltaFloat;
    s32 var_r8;
    s32 deltaInt;
    s32 someintval;
    s32 rangeFlags;
    s32 minInt;
    s32 maxInt;
    s32 temp_r6_3;
    s32 var_r7;
    s32 temp_r3_4;
    s32 temp_r4;
    s32 temp_r5;
    s32 temp_r5_2;
    s32 temp_r5_4;
    s32 var_r6;
    s32 var_r6_2;
    int var_r6_3;
    struct WindowItem *item;
    struct RangeInt *someParam;
    struct RangeFloat *someParamF;
    struct WindowDesc *subWindow;  // idk?
    u32 *pValue;
    s32 *new_var;

    if ((analogButtonInfo[0][4] & 0x80) || ((analogButtonInfo[0][0] & 0x80) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->unkC--;
        window->unkC = MAX(window->unkC, 3);
    }
    else if ((analogButtonInfo[0][4] & 0x40) || ((analogButtonInfo[0][0] & 0x40) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->unkC++;
        window->unkC = MIN(window->unkC, 0x25 - window->unk4);
    }
    if ((analogButtonInfo[0][4] & 0x10) || ((analogButtonInfo[0][0] & 0x10) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->unk8--;
        window->unk8 = MAX(window->unk8, 3);
    }
    else if ((analogButtonInfo[0][4] & 0x20) || ((analogButtonInfo[0][0] & 0x20) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->unk8++;
        window->unk8 = MIN(window->unk8, 0x35 - window->unk0);
    }
    if ((analogButtonInfo[0][4] & 8) || ((analogButtonInfo[0][0] & 8) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r5 = window->unk4 - 1;
        var_r6 = MAX(temp_r5, 0);
        window->unk28 += (window->unk4 - var_r6) * 0xC;
        window->unk4 = var_r6;
    }
    else if ((analogButtonInfo[0][4] & 4) || ((analogButtonInfo[0][0] & 4) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r5_2 = window->unk4;
        var_r6_2 = temp_r5_2 + 1;
        temp_r3_4 = 0x25 - window->unkC;
        var_r6_2 = MIN(var_r6_2, temp_r3_4);
        window->unk28 += (temp_r5_2 - var_r6_2) * 0xC;
        window->unk4 = var_r6_2;
    }
    if ((analogButtonInfo[0][4] & 1) || ((analogButtonInfo[0][0] & 1) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r4 = window->unk0 - 1;
        var_r6 = MAX(temp_r4, 0);
        window->unk24 += (window->unk0 - var_r6) * 0xC;
        window->unk0 = var_r6;
    }
    else if ((analogButtonInfo[0][4] & 2) || ((analogButtonInfo[0][0] & 2) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r5_2 = window->unk0;
        var_r6_2 = temp_r5_2 + 1;
        temp_r3_4 = 0x35 - window->unk8;
        var_r6_2 = MIN(var_r6_2, temp_r3_4);
        window->unk24 += (temp_r5_2 - var_r6_2) * 0xC;
        window->unk0 = var_r6_2;
    }
    if (lbl_802F1E40 != 0)
    {
        temp_r5_4 = window->unk1C;
        var_r6_3 = temp_r5_4;
        do
        {
            if (--var_r6_3 < 0)
            {
                item = window->items;
                while ((u8)item->type != ITEM_END)
                {
                    var_r6_3++;
                    item++;
                }
            }
            if (var_r6_3 == 0 || var_r6_3 == temp_r5_4)
                break;
        } while ((u8)window->items[var_r6_3].type > 12);
        window->unk1C = var_r6_3;
    }
    else if (lbl_802F1E44 != 0)
    {
        var_r7 = temp_r6_3 = window->unk1C;
        do
        {
            var_r7++;
            if ((u8)window->items[var_r7].type == ITEM_END)
            {
                var_r7 = 0;
                break;
            }
            if (var_r7 == temp_r6_3)
                break;
        } while ((u8)window->items[var_r7].type > 12);
        window->unk1C = var_r7;
    }
    item = &window->items[window->unk1C];
    switch ((s32)item->type)
    {
    case 0:
        if (controllerInfo[0].unk0[4].button & PAD_BUTTON_A)
        {
            subWindow = item->pValue;
            if (subWindow != NULL)
            {
                lbl_802F1EA8 = 1U;
                lbl_80205688[lbl_802F1E08]->unk0 = subWindow->unk0;
                lbl_80205688[lbl_802F1E08]->unk4 = subWindow->unk4;
                lbl_80205688[lbl_802F1E08]->unk8 = subWindow->unk8;
                lbl_80205688[lbl_802F1E08]->unkC = subWindow->unkC;
                lbl_80205688[lbl_802F1E08]->items = subWindow->items;
                lbl_80205688[lbl_802F1E08]->structSize = subWindow->structSize;
                lbl_80205688[lbl_802F1E08]->unk18 = subWindow->unk18;
                lbl_80205688[lbl_802F1E08]->unk1C = subWindow->unk1C;
            }
        }
        break;
    case 11:
        if ((controllerInfo[0].unk0[4].button & PAD_BUTTON_A) && item->pValue != NULL)
            func_8002E06C(item->pValue);
        break;
    default:
        if ((controllerInfo[0].unk0[4].button & PAD_BUTTON_A)
         || (controllerInfo[0].unk0[4].button & PAD_BUTTON_B)
         || ((analogButtonInfo[0][0] & 0x200) && ((controllerInfo[0].unk0[0].button & PAD_BUTTON_A) || (controllerInfo[0].unk0[0].button & PAD_BUTTON_B))))
        {
            if (item->type & 0x200)
                var_r8 = 0;
            else
                var_r8 = window->unk18;
            pValue = item->pValue;
            temp_r4_6 = (void *)((u8 *)pValue + (window->structSize * var_r8));
            switch ((u8)item->type)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 10:
                switch ((u8)item->type)
                {
                case 10:
                    someintval = window->unk18;
                    break;
                case 1:
                    someintval = *(u32 *)temp_r4_6;
                    break;
                case 2:
                    someintval = *(u32 *)temp_r4_6;
                    break;
                case 3:
                    someintval = *(u16 *)temp_r4_6;
                    break;
                case 4:
                    someintval = *(s16 *)temp_r4_6;
                    break;
                case 5:
                    someintval = *(u8 *)temp_r4_6;
                    break;
                default:
                    someintval = *(s8 *)temp_r4_6;
                    break;
                }
                someParam = item->param;
                rangeFlags = someParam->flags;
                minInt = someParam->min;
                maxInt = someParam->max;
                deltaInt = *(new_var = &someParam->delta);
                if ((controllerInfo[0].unk0[4].button & PAD_BUTTON_A) || ((analogButtonInfo[0][0] & 0x200) && (controllerInfo[0].unk0[0].button & PAD_BUTTON_A)))
                {
                    if (rangeFlags & 2)
                        someintval = someintval + deltaInt;
                    else if (deltaInt > 0)
                        someintval = MIN(someintval + deltaInt, (s32)maxInt);
                    else
                        someintval = MIN(someintval + deltaInt, (u32)maxInt);
                }
                else
                {
                    if (rangeFlags & 1)
                        someintval = someintval - deltaInt;
                    else if (deltaInt > 0)
                        someintval = MAX(someintval - deltaInt, (s32)minInt);
                    else
                        someintval = MAX(someintval - deltaInt, (u32)minInt);
                }
                switch ((u8)item->type)
                {
                case 1:
                case 2:
                    *(s32 *)temp_r4_6 = someintval;
                    break;
                case 10:
                    window->unk18 = (u32)someintval;
                    break;
                case 3:
                case 4:
                    *(s16 *)temp_r4_6 = someintval;
                    break;
                case 5:
                case 6:
                    *(s8 *)temp_r4_6 = someintval;
                    break;
                }
                break;
            case 7:
                someParamF = item->param;
                somefloatval = *(float *)temp_r4_6;
                deltaFloat = someParamF->delta;
                deltaFloat *= /*1.0f*/ 2.0f;
                rangeFlags = someParamF->flags;
                minFloat = someParamF->min;
                maxFloat = someParamF->max;
                if ((controllerInfo[0].unk0[4].button & 0x100) || ((analogButtonInfo[0][0] & 0x200) && (controllerInfo[0].unk0[0].button & 0x100)))
                {
                    if (rangeFlags & 2)
                        somefloatval = somefloatval + deltaFloat;
                    else
                        somefloatval = MIN(somefloatval + deltaFloat, maxFloat);
                }
                else
                {
                    if (rangeFlags & 1)
                        somefloatval = somefloatval - deltaFloat;
                    else
                        somefloatval = MAX(somefloatval - deltaFloat, minFloat);
                }
                *(float *)temp_r4_6 = somefloatval;
                break;
            case ITEM_BIT:
                *pValue = (s32) (*pValue ^ (u32)item->param);
                break;
            case 9:
                window->items = (struct WindowItem *) pValue;
                break;
            }
        }
        break;
    }

    window->unk24 /= 2;
    window->unk28 /= 2;
}

void draw_window_frame(struct WindowDesc *arg0);
void func_8002FCAC(int arg0, int arg1);

static void set_some_params(int a, int b, int c, int d)
{
    lbl_802F1E1C = a;
    lbl_802F1E20 = b;
    lbl_802F1E24 = c;
    lbl_802F1E28 = d;
}

u8 datafiller[0x1AF4] = {0};

void func_8002EA40(struct WindowDesc *desc, u32 arg1)
{
    struct WindowItem *item;
    int var_r27;
    int var_r26;
    int var_r25;
    char *var_r5_2;
    void *pValue;
    int var_r5;

    if (arg1 != 0)
        func_8002FCAC(desc->unk24, desc->unk28);
    else
        func_8002FCAC(0, 0);
    func_80030030(desc->unk0, desc->unk4, desc->unk8, desc->unkC);
    set_some_params(desc->unk0, desc->unk4, desc->unk8, desc->unkC);
    if (arg1 != 0)
        window_set_text_color(2);
    else
        window_set_text_color(0);
    draw_window_frame(desc);
    window_set_text_color(0);
    set_some_params(desc->unk0 + 1, desc->unk4 + 1, desc->unk8 - 2, desc->unkC - 2);
    var_r26 = 0;
    var_r25 = 0;
    var_r27 = 0;
    item = desc->items;
    while (item->type != ITEM_END)
    {
        if (item->unk4 > 0)
            var_r26 = desc->unk0 + item->unk4;
        else
            var_r26 -= item->unk4;
        if (item->unk8 > 0)
            var_r25 = desc->unk4 + item->unk8;
        else
            var_r25 -= item->unk8;
        u_debug_set_cursor_pos(var_r26, var_r25);
        window_set_text_color(0);
        switch (item->type & 0xFF)
        {
        case ITEM_NONE:
        case 26:
            if (var_r27 == 0 && arg1 != 0)
                window_set_text_color(2);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case ITEM_BIT:
        case 9:
        case 10:
            if (arg1 != 0)
                window_set_text_color(2);
            break;
        }
        if (item->type & 0x200)
            var_r5 = 0;
        else
            var_r5 = desc->unk18;
        pValue = (u8 *)item->pValue + (desc->structSize * var_r5);
        switch (item->type & 0xFF)
        {
        case 0:
        case 9:
        case 11:
        case ITEM_NONE:
            window_printf(2, item->format);
            break;
        case 1:
        case 14:
            window_printf(2, item->format, *(int *)pValue);
            break;
        case 2:
        case 15:
            window_printf(2, item->format, *(int *)pValue);
            break;
        case 10:
            window_printf(2, item->format, desc->unk18);
            break;
        case 3:
        case 16:
            window_printf(2, item->format, *(u16 *)pValue);
            break;
        case 4:
        case 17:
            window_printf(2, item->format, *(s16 *)pValue);
            break;
        case 5:
        case 18:
            window_printf(2, item->format, *(u8 *)pValue);
            break;
        case 6:
        case 19:
            window_printf(2, item->format, *(s8 *)pValue);
            break;
        case 7:
        case ITEM_FLOAT32:
            window_printf(2, item->format, *(float *)pValue);
            break;
        case 21:
            window_printf(2, item->format, (char *)pValue);
            break;
        case ITEM_STRING:
            window_printf(2, item->format, *(char **)pValue);
            break;
        case ITEM_BIT:
        case 23:
            if (*(u32 *)pValue & (u32)item->param)
                var_r5_2 = " ON";
            else
                var_r5_2 = "OFF";
            window_printf(2, item->format, var_r5_2);
            break;
        case 24:
            if (*(u16 *)pValue & (u16)item->param)
                var_r5_2 = " ON";
            else
                var_r5_2 = "OFF";
            window_printf(2, item->format, var_r5_2);
            break;
        case 26:
            window_printf(2, item->format, desc->unk18);
            break;
        case 25:
            window_printf(2, item->format, ((u32 *)item->param)[*(u8 *)pValue]);
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
            window_printf(2, item->format, u_stateNames[((s8 *)item->pValue)[desc->unk18]]);
            break;
        }
        if (desc->unk1C == var_r27 && arg1 != 0)
        {
            u_debug_set_cursor_pos(var_r26 - 1, var_r25);
            window_set_text_color(1);
            u_set_window_text(2, "\x1C");
        }
        item++;
        var_r27++;
    }
    func_8002FCAC(0, 0);
}

void draw_window_frame(struct WindowDesc *desc)
{
    int i;

    set_some_params(desc->unk0, desc->unk4, desc->unk8, desc->unkC);
    u_debug_set_cursor_pos(desc->unk0, desc->unk4);
    u_set_window_text(2, "\x18");
    u_debug_set_cursor_pos(desc->unk0 + desc->unk8 - 1, desc->unk4);
    u_set_window_text(2, "\x19");
    u_debug_set_cursor_pos(desc->unk0, desc->unk4 + desc->unkC - 1);
    u_set_window_text(2, "\x1A");
    u_debug_set_cursor_pos(desc->unk0 + desc->unk8 - 1, desc->unk4 + desc->unkC - 1);
    u_set_window_text(2, "\x1B");

    for (i = 1; i < desc->unk8 - 1; i++)
    {
        u_debug_set_cursor_pos(desc->unk0 + i, desc->unk4);
        u_set_window_text(2, "\x16");
    }

    for (i = 1; i < desc->unkC - 1; i++)
    {
        u_debug_set_cursor_pos(desc->unk0, desc->unk4 + i);
        u_set_window_text(2, "\x17");
    }

    for (i = 1; i < desc->unk8 - 1; i++)
    {
        u_debug_set_cursor_pos(desc->unk0 + i, desc->unk4 + desc->unkC - 1);
        u_set_window_text(2, "\x16");
    }

    for (i = 1; i < desc->unkC - 1; i++)
    {
        u_debug_set_cursor_pos(desc->unk0 + desc->unk8 - 1, desc->unk4 + i);
        u_set_window_text(2, "\x17");
    }
}

void func_8002F0E4(void)
{
    u8 *var_r30_2;
    struct WindowDesc **var_r30;
    int var_r31;
    int var_r29;
    int var_r27;
    s8 *var_r26;
    s8 *var_r25;
    s8 *var_r24;
    int temp_r0;
    int temp_r3;
    int temp_r5;
    int temp_r8;
    int var_r5;
    int var_r6;
    int var_r8;
    int var_r9;
    struct WindowDesc *temp_r7;

    GXSetZMode_cached(1, GX_LESS, 1);
    var_r30 = lbl_80205688;
    for (var_r29 = 0; var_r29 < 16; var_r29++, var_r30++)
    {
        temp_r7 = *var_r30;
        if (temp_r7 != NULL)
        {
            temp_r5 = temp_r7->unk0;
            temp_r8 = temp_r7->unk4;
            var_r6 = (temp_r5 * 12) + 6;
            var_r5 = (temp_r8 * 12) + 6;
            var_r8 = ((temp_r5 + temp_r7->unk8) * 12) - 6;
            var_r9 = ((temp_r8 + temp_r7->unkC) * 12) - 6;
            if (var_r29 == lbl_802F1E08 && lbl_802F1EA8 != 0U)
            {
                temp_r0 = temp_r7->unk24;
                temp_r3 = temp_r7->unk28;
                var_r6 += temp_r0;
                var_r5 += temp_r3;
                var_r8 += temp_r0;
                var_r9 += temp_r3;
            }
            draw_some_window_quad_2(var_r6, var_r5, var_r8, var_r9);
        }
    }
    GXSetZMode_cached(1, GX_ALWAYS, 0);
    GXLoadTexObj_cached(&u_unkBitmapTPL->texObjs[31], 0);
    GXInitTexObjLOD(&u_unkBitmapTPL->texObjs[31], GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0U, 0U, GX_ANISO_1);

    var_r26 = (s8 *)lbl_80202880;
    var_r30_2 = lbl_80203F84;
    var_r25 = (s8 *)lbl_80203F84_2;
    var_r24 = (s8 *)lbl_80203F84_3;
    for (var_r27 = 0; var_r27 < 0x25; var_r27++)
    {
        for (var_r31 = 0; var_r31 < 0x35; var_r31++)
        {
            if (*var_r26 != 0x20)
            {
                draw_some_window_quad_1(
                    *var_r25 + (var_r31 * 12),
                    *var_r24 + (var_r27 * 12),
                    *var_r26,
                    *var_r30_2);
            }
            var_r26++;
            var_r30_2++;
            var_r25++;
            var_r24++;
        }
    }
    GXSetZMode_cached(1, GX_LEQUAL, 1);
}

void window_init(void)
{
    s32 i;

    lbl_802F1E08 = 0;
    lbl_802F1EA8 = 0U;
    for (i = 0; i < 16; i++)
        bss_3DA0[i].unk20 = 0;
    memset(lbl_80201928, 0x20, 0x7A9);
    memset(lbl_802020D4, 0x20, 0x7A9);
    memset(lbl_80202880, 0x20, 0x7A9);
    memset(lbl_80201928_1704, 0, 0x7A9);
    memset(lbl_802037D8, 0, 0x7A9);
    memset(lbl_80203F84, 0, 0x7A9);
}

static void dont_inline_func_8002DD5C(void)
{
    func_8002DD5C();
}

void window_main(void)
{
    int spC;
    int var_r31;
    int temp_r22;
    int var_r22;
    int temp_r4_2;
    int var_ctr;
    int var_ctr_3;
    u32 *var_r3_5;
    u8 *var_r12;
    int var_r3_7;
    u8 *var_r4;
    struct WindowDesc **var_r4_2;
    u8 *var_r5;
    u8 *var_r6;
    u8 *var_r7;
    u8 *var_r8;
    u8 *var_r9;
    s8 r11;
    u8 *var_r10;
    int var_ctr_4;
    struct WindowDesc *temp_r3;
    struct WindowDesc *temp_r3_7;

    var_r22 = 1;

    lbl_802F1E40 = ((controllerInfo[0].unk0[4].button & 8) || ((controllerInfo[0].unk0[0].button & 8) && (analogButtonInfo[0][0] & 0x200)));
    lbl_802F1E44 = ((controllerInfo[0].unk0[4].button & 4) || ((controllerInfo[0].unk0[0].button & 4) && (analogButtonInfo[0][0] & 0x200)));
    lbl_802F1E48 = ((controllerInfo[0].unk0[4].button & 1) || ((controllerInfo[0].unk0[0].button & 1) && (analogButtonInfo[0][0] & 0x200)));
    lbl_802F1E4C = ((controllerInfo[0].unk0[4].button & 2) || ((controllerInfo[0].unk0[0].button & 2) && (analogButtonInfo[0][0] & 0x200)));

    var_r3_5 = (u32 *)lbl_80203F84_2;
    for (var_ctr = 487; var_ctr >= 0; var_ctr--)
        *var_r3_5++ = 0;
    var_r3_5 = (u32 *)lbl_80203F84_3;
    for (var_ctr = 487; var_ctr >= 0; var_ctr--)
        *var_r3_5++ = 0;

    memcpy(lbl_80202880, lbl_802020D4, 0x7A9);
    memcpy(lbl_80203F84, lbl_802037D8, 0x7A9);

    var_r12 = lbl_80201928;
    var_r8 = lbl_80202880;
    var_r9 = lbl_80201928_1704;
    var_r10 = lbl_80203F84;
    for (var_r3_7 = 0; var_r3_7 < 0x25; var_r3_7++)
    {
        var_r4 = var_r12;
        var_r5 = var_r8;
        var_r6 = var_r9;
        var_r7 = var_r10;
        for (var_ctr_3 = 0; var_ctr_3 < 0x35; var_ctr_3++)
        {
            r11 = *var_r4;
            if (r11 != 0x20)
            {
                *var_r5 = r11;
                *var_r7 = *var_r6;
            }
            var_r4++;
            var_r5++;
            var_r6++;
            var_r7++;
        }
        var_r12 += 0x35;
        var_r8 += 0x35;
        var_r9 += 0x35;
        var_r10 += 0x35;
    }

    memset(lbl_80201928, 0x20, 0x7A9);
    memset(lbl_80201928_1704, 0, 0x7A9);

    if (analogButtonInfo[0][0] & 0x100)
    {
        if ((dipSwitches & 1) && (controllerInfo[0].unk0[2].button & 0x1000))
        {
            func_8002E06C(&lbl_801B7474);
        }
        else if (controllerInfo[0].unk0[2].button & 0x800)
        {
            if (lbl_802F1EA8 != 0U)
            {
                temp_r3 = lbl_80205688[lbl_802F1E08];
                if (temp_r3 != NULL)
                {
                    lbl_802F1EA8 = 1U;
                    temp_r3->unk20 = 0;

                    var_r4_2 = &lbl_80205688[lbl_802F1E08];
                    for (var_ctr_4 = lbl_802F1E08; var_ctr_4 < 15; var_ctr_4++, var_r4_2++)
                        *var_r4_2 = *(var_r4_2 + 1);
                    *var_r4_2 = NULL;

                    func_8002DE38();
                }
            }
        }
        else if (controllerInfo[0].unk0[4].button & 0x100)
        {
            func_8002DC54();
            var_r22 = 0;
        }
        else if ((controllerInfo[0].unk0[4].button & 0x200) && ((u32) lbl_802F1EA8 != 0U))
        {
            dont_inline_func_8002DD5C();
        }
    }
    lbl_802F1C90 = u_lightToPrint;
    lbl_801F3A08 = s_u_lightPool[u_lightToPrint];
    lbl_802F1E50 = lbl_801F3A08.spotFn;
    lbl_802F1C74 = lbl_802F1C75;
    // this doesn't match as a loop
    lbl_802F1C6C[0] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[0];
    lbl_802F1C6C[1] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[1];
    lbl_802F1C6C[2] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[2];
    lbl_802F1C6C[3] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[3];
    lbl_802F1C6C[4] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[4];
    lbl_802F1C6C[5] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[5];
    lbl_802F1C6C[6] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[6];
    lbl_802F1C6C[7] = s_lightGroups[lbl_802F1C75].lightPoolIdxs[7];
    lbl_801F39FC = s_lightGroups[lbl_802F1C75].ambient;

    temp_r22 = lbl_802F1DE8;
    spC = lbl_802F1DE4;
    lbl_802F1E51 = lbl_80201500[temp_r22];
    if (var_r22 != 0 && lbl_802F1EA8 != 0 && lbl_80205688[lbl_802F1E08] != 0)
        process_window(lbl_80205688[lbl_802F1E08]);
    lbl_801F3A08.spotFn = lbl_802F1E50;

    s_u_lightPool[lbl_802F1C90] = lbl_801F3A08;
    // this doesn't match as a loop
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[0] = lbl_802F1C6C[0];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[1] = lbl_802F1C6C[1];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[2] = lbl_802F1C6C[2];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[3] = lbl_802F1C6C[3];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[4] = lbl_802F1C6C[4];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[5] = lbl_802F1C6C[5];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[6] = lbl_802F1C6C[6];
    s_lightGroups[lbl_802F1C74].lightPoolIdxs[7] = lbl_802F1C6C[7];
    s_lightGroups[lbl_802F1C74].ambient = lbl_801F39FC;

    lbl_80201500[temp_r22] = lbl_802F1E51;

    if (g_soundDesc[lbl_802F1DE8].unk8 == 1)
    {
        if (lbl_802F1DE8 == g_soundGroupDesc[lbl_802F1DE4].unused)
            lbl_802F1DE8 = g_soundGroupDesc[lbl_802F1DE4+1].unused - 1;
        else
            lbl_802F1DE8 = g_soundGroupDesc[lbl_802F1DE4].unused + 1;
    }
    if (spC != lbl_802F1DE4)
        lbl_802F1DE8 = g_soundGroupDesc[lbl_802F1DE4].unused + 1;
    if (lbl_802F1E54 != 0)
    {
        SoundReqDirect(lbl_802F1DE8);
        lbl_802F1E54 = 0;
    }
    if (lbl_802F1E58 != 0)
    {
        SoundOff(lbl_802F1DE8);
        lbl_802F1E58 = 0;
    }
    for (var_r31 = 15; var_r31 >= 0; var_r31--)
    {
        if (lbl_80205688[var_r31] != NULL)
        {
            if (var_r31 == lbl_802F1E08 && lbl_802F1EA8 != 0)
                func_8002EA40(lbl_80205688[var_r31], 1);
            else
                func_8002EA40(lbl_80205688[var_r31], 0);
        }
    }
    window_set_text_color(0);
    if (lbl_802F1EA8 != 0U)
    {
        temp_r3_7 = lbl_80205688[lbl_802F1E08];
        if (temp_r3_7 != NULL)
        {
            lbl_802F1E30 = temp_r3_7->unk0;
            lbl_802F1E34 = temp_r3_7->unk4;
            lbl_802F1E38 = temp_r3_7->unk8;
            lbl_802F1E3C = temp_r3_7->unkC;
            return;
        }
    }
    lbl_802F1E30 = 0;
    lbl_802F1E34 = 0;
    lbl_802F1E38 = 0;
    lbl_802F1E3C = 0;
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
    u_set_window_text(arg0, buf);
    va_end(args);
    return 0;
}

void window_set_text_color(int arg0)
{
    lbl_802F1E2C = arg0;
}

void u_set_window_text(int arg0, const char *text)
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

    while (*text != 0)
    {
        if (lbl_802F1E10 >= 0x35)
        {
            lbl_802F1E10 = 0;
            lbl_802F1E14++;
        }
        if (*text == '\n')
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
            *var_r5 = *text;
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
        text++;
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

int window_printf_1(const char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    u_set_window_text(1, buf);
    va_end(args);
    return 0;
}

int window_printf_2(const char *fmt, ...)
{
    va_list args;
    char buf[0x200];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    u_set_window_text(0, buf);
    va_end(args);
    return 0;
}

void u_debug_print(char *arg0)
{
    u_set_window_text(0, arg0);
}

int u_printf_if_debug(int unused, char *fmt, ...)
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

void draw_some_window_quad_1(int arg0, int arg1, s8 arg2, u8 arg3)
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

void draw_some_window_quad_2(float x1, float y1, float x2, float y2)
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
