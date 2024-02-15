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
#include "perf.h"
#include "pool.h"
#include "sound.h"
#include "sprite.h"
#include "stobj.h"
#include "window.h"

// each character is 12x12 pixels
#define SCREEN_ROWS (448/12)
#define SCREEN_COLUMNS (640/12)

u32 debugFlags;
u32 dipSwitches;
u32 lbl_802F1ED8;
u16 lbl_802F1ED4;
u16 lbl_802F1ED2;
u16 lbl_802F1ED0;
float lbl_802F1ECC;
float lbl_802F1EC8;
float lbl_802F1EC4;
float lbl_802F1EC0;
float lbl_802F1EBC;
float lbl_802F1EB8;
s32 lbl_802F1EB4;
s32 lbl_802F1EB0;
u32 lbl_802F1EAC;
u32 lbl_802F1EA8;
char *lbl_802F1EA4;
u16 lbl_802F1EA2;
u16 lbl_802F1EA0;
u32 lbl_802F1E9C;
u32 lbl_802F1E98;
u32 lbl_802F1E94;
u32 lbl_802F1E90;
u32 lbl_802F1E8C;
u32 lbl_802F1E88;
u32 lbl_802F1E84;
u32 lbl_802F1E80;
u32 lbl_802F1E7C;
float lbl_802F1E78;
float lbl_802F1E74;
float lbl_802F1E70;
float lbl_802F1E6C;
u16 lbl_802F1E6A;
u16 lbl_802F1E68;
u16 lbl_802F1E66;
u16 lbl_802F1E64;
float lbl_802F1E60;
float lbl_802F1E5C;

static int currWindowIndex;
static s32 lbl_802F1E0C;
static s32 windowCursorX;
static s32 windowCursorY;
static s8 lbl_802F1E18;
static s8 lbl_802F1E19;
static u32 lbl_802F1E1C;
static u32 lbl_802F1E20;
static u32 lbl_802F1E24;
static u32 lbl_802F1E28;
static u8 windowColorId;
static u32 unusedWindowX;
static u32 unusedWindowY;
static u32 unusedWindowWidth;
static u32 unusedWindowHeight;
static s32 lbl_802F1E40;
static s32 lbl_802F1E44;
static u32 lbl_802F1E48;
static u32 lbl_802F1E4C;
static s8 lbl_802F1E50;
static s8 lbl_802F1E51;
static s32 lbl_802F1E54;
static s32 lbl_802F1E58;

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
    s32 x;
    s32 y;
    char *format;
    void *pValue;
    void *param;  // usage of this depends on "type"
};  // size = 0x18

struct WindowDesc
{
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    struct WindowItem *items;
    u32 structSize;
    u32 structIndex;
    s32 selection;
    s32 isActive;
    s32 unk24;
    s32 unk28;
};

static char *lbl_802F0828[] = { "OFF", "ON" };

// .data

static struct RangeInt   lbl_801B3B18 = { 3, 0, 100, 1 };
static struct RangeFloat posScaleRange = { 3, -10.0f, 10.0f, 0.1f };
static struct RangeFloat lbl_801B3B38 = { 3, -10.0f, 10.0f, 0.001f };
static struct RangeFloat lbl_801B3B38_2 = { 3, -10.0f, 10.0f, 0.00001f };
static struct RangeFloat lbl_801B3B38_3 = { 3, -10.0f, 10.0f, 0.01f };
static struct RangeInt   angleRange = { 3, 0, 0, 128 };
static struct RangeInt   lbl_801B3B78 = { 0, 0, 1, 1 };

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

static struct WindowDesc dipSwitchWindow = { 9, 0, 25, 29, lbl_801B3D98, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B43F4[] =
{
    {ITEM_NONE, 1, 1, "Game", NULL, NULL},
    {ITEM_END, 0, 0, NULL,   NULL, NULL},
};

static struct WindowDesc lbl_801B4424 = { 0, 0, 10, 10, lbl_801B43F4, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B4480[] =
{
    {ITEM_NONE, 1, 1, "Mode",          NULL,              NULL},
    {17, 3, 3, "main_mode: %d", &gameMode,         NULL},
    {ITEM_STRING, 3, 4, "  %-14s",       &modeStringPtr,    NULL},
    {17, 3, 5, " sub_mode: %d", &gameSubmode,      NULL},
    {ITEM_STRING, 3, 6, "  %-14s",       &submodeStringPtr, NULL},
    {15, 3, 8, "mode.cntr: %d", &modeCtrl,         NULL},
    {ITEM_END, 0, 0, NULL,            NULL,              NULL},
};

static struct WindowDesc modeWindow = { 33, 0, 20, 11, lbl_801B4480, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B4554[] =
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

static struct WindowDesc eventWindow = {0, 11, 27, 26, lbl_801B4554, 0, 0, 0, 0, 0, 0};

static struct RangeInt lbl_801B49A0 = {0, 0, 3, 1};

static struct WindowItem lbl_801B4A38[] =
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

static struct WindowDesc cameraWindow = {36, 22, 17, 15, lbl_801B4A38, sizeof(struct Camera), 0, 0, 0, 0, 0};

static struct WindowItem lbl_801B4B84[] =
{
    {ITEM_NONE, 1, 1, "Effect", NULL, NULL},
    {ITEM_END, 0, 0, NULL,     NULL, NULL},
};

static struct WindowDesc effectWindow = {0, 0, 10, 10, lbl_801B4B84, 0, 0, 0, 0, 0, 0};

static struct RangeFloat lbl_801B4BE0 = { 3, 0.0f, 640.0f, 1.0f };
static struct RangeInt   lbl_801B4BF0 = { 0, 0, 63, 1 };
static struct RangeInt   lbl_801B4C00 = { 0, 0, 178, 1 };
static struct RangeInt   lbl_801B4C10 = { 0, 0, 8, 1 };
static struct RangeInt   lbl_801B4C20 = { 0, 0, 255, 1 };
static struct RangeFloat lbl_801B4C30 = { 0, 0.0f, 1.0f, 0.01f };

static struct WindowItem lbl_801B4D94[] =
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

static struct WindowDesc spriteWindow = {0, 0, 24, 30, lbl_801B4D94, sizeof(struct Sprite), 0, 0, 0, 0, 0};

static char *lbl_801B503C[] =
{
    "GX_SP_OFF",
    "GX_SP_FLAT",
    "GX_SP_COS",
    "GX_SP_COS2",
    "GX_SP_SHARP",
    "GX_SP_RING1",
    "GX_SP_RING2",
};

static struct RangeInt   lbl_801B5058 = { 0, 0, 31, 1 };
static struct RangeInt   lbl_801B5068 = { 0, 0, 7, 1 };
static struct RangeInt   lbl_801B5078 = { 0, 0, 8, 1 };
static struct RangeFloat lbl_801B5088 = { 0, 0.0f, 1.0f, 0.01f };
static struct RangeFloat lbl_801B5098 = { 2, 0.1f, 1.0f, 0.1f };
static struct RangeFloat lbl_801B50A8 = { 3, 0.0f, 1.0f, 0.1f };
static struct RangeFloat lbl_801B50B8 = { 3, 0.0f, 1.0f, 0.01f };
static struct RangeFloat lbl_801B50C8 = { 3, 0.0f, 1.0f, 0.01f };
static struct RangeInt   lbl_801B50D8 = { 0, 0, 6, 1 };
static struct RangeFloat lbl_801B50E8 = { 0, 0.0f, 90.0f, 0.1f };
static struct RangeInt   lbl_801B50F8 = { 0, 1, 199, 1 };

static struct WindowItem lbl_801B5170[] =
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
    { 7,  3, -1, "%4.2f",        &lbl_801F3A08.red,        &lbl_801B5088},
    { 7,  8,  0, "%4.2f",        &lbl_801F3A08.green,      &lbl_801B5088},
    { 7, 13,  0, "%4.2f",        &lbl_801F3A08.blue,       &lbl_801B5088},
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

static struct WindowDesc lightParamWindow = {0, 0, 21, 33, lbl_801B5170, 0, 0, 0, 0, 0, 0};

static struct RangeInt lbl_801B549C = { 0, 0, 21, 1 };
static struct RangeInt lbl_801B54AC = { 0, -1, 31, 1 };

static struct WindowItem lbl_801B5538[] =
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

static struct WindowDesc lightGroupWindow = { 36, 0, 17, 16, lbl_801B5538, 0, 0, 0, 0, 0, 0 };

static char *lbl_801B57B8[] =
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

static struct RangeInt   lbl_801B57D8 = { 0, 0, 7, 1 };
static struct RangeFloat lbl_801B57E8 = { 3, -10.0f, 10.0f, 1.0f };
static struct RangeInt   lbl_801B57F8 = { 0, 0, 255, 1 };

static struct WindowItem fogWindowItems[] =
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

static struct WindowDesc fogWindow = { 0, 0, 28, 16, fogWindowItems, 0, 0, 0, 0, 0, 0 };

static struct RangeInt lbl_801B598C = { 0, 0, 100, 5 };
static struct RangeInt lbl_801B599C = { 0, 0, 16, 1 };
static struct RangeInt lbl_801B59AC = { 0, 0, 1061, 1 };
static struct RangeInt lbl_801B59BC = { 0, -128, 127, 1 };
static struct RangeInt lbl_801B59CC = { 3, 0, 100, 1 };

static struct WindowItem lbl_801B5A50[] =
{
    {13,  1,  1, "Sound",                0, 0},
    {14,  2, -2, " RAM:%08X",            &g_soundTotalBytesLoaded, 0},
    {14,  2, -1, "ARAM:%08X",            &g_soundAramTop, 0},
    { 5,  2, -2, "SE  vol : %3d",        &lbl_802F1DF5, &lbl_801B598C},
    { 5,  2, -1, "BGM vol : %3d",        &u_volumeRelated1, &lbl_801B598C},
    {13,  2, -2, "VOL OFS",              0, 0},
    { 2,  4, -1, "GRP : ",               &lbl_802F1DE4, &lbl_801B599C},
    {22, 10,  0, "%s",                   &lbl_802F1DDC, 0},
    { 2,  4, -1, "ID  : ",               &lbl_802F1DE8, &lbl_801B59AC},
    {22, 10,  0, "%s",                   &lbl_802F1DE0, 0},
    { 6,  4, -1, "ofs : %d",             &lbl_802F1E51, &lbl_801B59BC},
    { 2,  4, -1, "REQ",                  &lbl_802F1E54, &lbl_801B3B78},
    { 2,  4, -1, "OFF",                  &lbl_802F1E58, &lbl_801B3B78},
    { 2,  2, -2, "stream loop ofs : %d", &lbl_802F1DD0, &lbl_801B59CC},
    { 2,  2, -1, "stream vol ofs  : %d", &lbl_802F1DCC, &lbl_801B59CC},
    {31,  0,  0, 0, 0, 0},
};

static struct WindowDesc soundWindow = { 0, 0, 35, 20, lbl_801B5A50, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B5DE0[] =
{
    {13,  1,  1, "Monkey Fight", 0, 0},
    { 3,  2,  3, "CAM ANG X : 0x%04X", &lbl_802F1EA2, &angleRange},
    { 3,  2,  4, "    ANG Y : 0x%04X", &lbl_802F1EA0, &angleRange},
    { 7,  2,  5, "BALL POWER: %4.2f", &lbl_802F1E9C, &posScaleRange},
    { 2,  2,  7, "BLT INTERVAL   : %4d", &lbl_802F1E98, &lbl_801B3B18},
    { 2,  2,  8, "BLT TRIGER MODE: %4d", &lbl_802F1E94, &lbl_801B3B18},
    { 2,  2,  9, "BLT SWING MODE : %4d", &lbl_802F1E90, &lbl_801B3B18},
    { 2,  2, 10, "BLT TIME       : %4d", &lbl_802F1E8C, &lbl_801B3B18},
    { 7,  2, 11, "BLT SPD        : %4.2f", &lbl_802F1E88, &posScaleRange},
    { 7,  2, 12, "BLT POWER      : %4.3f", &lbl_802F1E84, &lbl_801B3B38},
    { 7,  2, 13, "BLT REACT POWER: %4.3f", &lbl_802F1E80, &lbl_801B3B38},
    { 7,  2, 14, "BLT COLI RAD   : %4.2f", &lbl_802F1E7C, &posScaleRange},
    { 7,  2, 15, "        LV COEF: %4.2f", &lbl_802F1E78, &posScaleRange},
    { 7,  2, 16, "BLT RANGE      : %4.2f", &lbl_802F1E74, &posScaleRange},
    { 7,  2, 17, "        LV COEF: %4.2f", &lbl_802F1E70, &posScaleRange},
    { 7,  2, 18, "BLT BALL BREAK : %4.2f", &lbl_802F1E6C, &posScaleRange},
    { 4,  2, 20, "KILL TOP POINT : %4d", &lbl_802F1E6A, &lbl_801B3B18},
    { 4,  2, 21, "KILL POINT     : %4d", &lbl_802F1E68, &lbl_801B3B18},
    { 4,  2, 22, "FALL POINT     : %4d", &lbl_802F1E66, &lbl_801B3B18},
    { 4,  2, 23, "PUNCH HIT POINT: %4d", &lbl_802F1E64, &lbl_801B3B18},
    { 7,  2, 25, "KILL DECIDE SEC: %4.2f", &lbl_802F1E60, &posScaleRange},
    { 7,  2, 27, "MUTEKI SEC     : %4.2f", &lbl_802F1E5C, &posScaleRange},
    {31,  0,  0, 0, 0, 0},
};

static struct WindowDesc fightWindow = { 24, 0, 32, 36, lbl_801B5DE0, 0, 0, 0, 0, 0, 0 };

static struct RangeInt unusedRange1 = { 0, 0, 255, 1 };

static struct WindowItem raceCpuWindow[] =
{
    {13,  1,  1, "MINI RACE CPU", 0, 0},
    {31,  0,  0, 0, 0, 0},
};

static struct WindowDesc lbl_801B6054_desc = { 28, 25, 24, 11, raceCpuWindow, 0x14, 0, 0, 0, 0, 0 };

static struct RangeInt   unusedRange2 = { 3, 0, 100, 128 };
static struct RangeFloat unusedRange3 = { 3, -10.0f, 10.0f, 0.02f };

static struct WindowItem raceWindow[] =
{
    {13,  1,  1, "Monkey Race", 0, 0},
    {31,  0,  0, 0, 0, 0},
};

static struct WindowDesc lbl_801B610C = { 23, 19, 30, 17, raceWindow, 0, 0, 0, 0, 0, 0 };


static struct WindowItem lbl_801B61E4[] =
{
    {13,  1,  1, "Performance", NULL, NULL},
    {13,  3,  3, "MAIN LOOP", NULL, NULL},
    {21,  3,  5, "%13s", "PRE_PROCESS", NULL},
    {21,  3,  6, "%13s", "SOUND_SYSTEM", NULL},
    {21,  3,  7, "%13s", "INPUT", NULL},
    {21,  3,  8, "%13s", "DEBUG_CONTROL", NULL},
    {21,  3,  9, "%13s", "LOAD_MAIN", NULL},
    {21,  3, 10, "%13s", "MODE", NULL},
    {21,  3, 11, "%13s", "CHECK_STATUS", NULL},
    {21,  3, 12, "%13s", "EVENT", NULL},
    {21,  3, 13, "%13s", "POLY_DISP", NULL},
    {21,  3, 14, "%13s", "BITMAP_DISP", NULL},
    {21,  3, 15, "%13s", "WINDOW_MAIN", NULL},
    {21,  3, 16, "%13s", "EPI_PROCESS", NULL},
    {21,  3, 17, "%13s", "SYNC_WAIT", NULL},
    {21,  3, 18, "%13s", "SHADOW_ERASE", NULL},
    {15, 17,  5, "%5d", &perfInfo.unk0, NULL},
    {15, 17,  6, "%5d", &perfInfo.soundTime, NULL},
    {15, 17,  7, "%5d", &perfInfo.inputTime, NULL},
    {15, 17,  8, "%5d", &perfInfo.debugTime, NULL},
    {15, 17,  9, "%5d", &perfInfo.loadTime, NULL},
    {15, 17, 10, "%5d", &perfInfo.unk14, NULL},
    {15, 17, 11, "%5d", &perfInfo.unk18, NULL},
    {15, 17, 12, "%5d", &perfInfo.eventTime, NULL},
    {15, 17, 13, "%5d", &perfInfo.polydispTime, NULL},
    {15, 17, 14, "%5d", &perfInfo.bitmapTime, NULL},
    {15, 17, 15, "%5d", &perfInfo.windowTime, NULL},
    {15, 17, 16, "%5d", &perfInfo.gpwaitTime, NULL},
    {15, 17, 17, "%5d", &perfInfo.unk30, NULL},
    {15, 17, 18, "%5d", &perfInfo.unk34, NULL},
    {13, 24,  3, "EVENT", NULL, NULL},
    {22, 24,  5, "%10s", &eventInfo[0].name, NULL},
    {22, 24,  6, "%10s", &eventInfo[1].name, NULL},
    {22, 24,  7, "%10s", &eventInfo[2].name, NULL},
    {22, 24,  8, "%10s", &eventInfo[3].name, NULL},
    {22, 24,  9, "%10s", &eventInfo[4].name, NULL},
    {22, 24, 10, "%10s", &eventInfo[5].name, NULL},
    {22, 24, 11, "%10s", &eventInfo[6].name, NULL},
    {22, 24, 12, "%10s", &eventInfo[7].name, NULL},
    {22, 24, 13, "%10s", &eventInfo[8].name, NULL},
    {22, 24, 14, "%10s", &eventInfo[9].name, NULL},
    {22, 24, 15, "%10s", &eventInfo[10].name, NULL},
    {22, 24, 16, "%10s", &eventInfo[11].name, NULL},
    {22, 24, 17, "%10s", &eventInfo[12].name, NULL},
    {22, 24, 18, "%10s", &eventInfo[13].name, NULL},
    {22, 24, 19, "%10s", &eventInfo[14].name, NULL},
    {22, 24, 20, "%10s", &eventInfo[15].name, NULL},
    {22, 24, 21, "%10s", &eventInfo[16].name, NULL},
    {22, 24, 22, "%10s", &eventInfo[17].name, NULL},
    {22, 24, 23, "%10s", &eventInfo[18].name, NULL},
    {22, 24, 24, "%10s", &eventInfo[19].name, NULL},
    {22, 24, 25, "%10s", &eventInfo[20].name, NULL},
    {15, 35,  5, "%5d", &eventInfo[0].time, NULL},
    {15, 35,  6, "%5d", &eventInfo[1].time, NULL},
    {15, 35,  7, "%5d", &eventInfo[2].time, NULL},
    {15, 35,  8, "%5d", &eventInfo[3].time, NULL},
    {15, 35,  9, "%5d", &eventInfo[4].time, NULL},
    {15, 35, 10, "%5d", &eventInfo[5].time, NULL},
    {15, 35, 11, "%5d", &eventInfo[6].time, NULL},
    {15, 35, 12, "%5d", &eventInfo[7].time, NULL},
    {15, 35, 13, "%5d", &eventInfo[8].time, NULL},
    {15, 35, 14, "%5d", &eventInfo[9].time, NULL},
    {15, 35, 15, "%5d", &eventInfo[10].time, NULL},
    {15, 35, 16, "%5d", &eventInfo[11].time, NULL},
    {15, 35, 17, "%5d", &eventInfo[12].time, NULL},
    {15, 35, 18, "%5d", &eventInfo[13].time, NULL},
    {15, 35, 19, "%5d", &eventInfo[14].time, NULL},
    {15, 35, 20, "%5d", &eventInfo[15].time, NULL},
    {15, 35, 21, "%5d", &eventInfo[16].time, NULL},
    {15, 35, 22, "%5d", &eventInfo[17].time, NULL},
    {15, 35, 23, "%5d", &eventInfo[18].time, NULL},
    {15, 35, 24, "%5d", &eventInfo[19].time, NULL},
    {15, 35, 25, "%5d", &eventInfo[20].time, NULL},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc perfWindow = { 5, 7, 43, 29, lbl_801B61E4, 0, 0, 0, 0, 0, 0 };

static struct RangeInt lbl_801B6900 = { 0, 0, 3, 1 };

static struct WindowItem lbl_801B6AD4[] =
{
    {13,  1,  1, "Input Test", NULL, NULL},
    {10,  2,  3, "PAD           [%d]", NULL, &lbl_801B6900},
    {24,  2, -2, "BUTTON LEFT   %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_LEFT},
    {24,  2, -1, "BUTTON RIGHT  %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_RIGHT},
    {24,  2, -1, "BUTTON DOWN   %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_DOWN},
    {24,  2, -1, "BUTTON UP     %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_UP},
    {24,  2, -1, "TRIGGER Z     %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_TRIGGER_Z},
    {24,  2, -1, "TRIGGER R     %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_TRIGGER_R},
    {24,  2, -1, "TRIGGER L     %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_TRIGGER_L},
    {24,  2, -1, "BUTTON A      %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_A},
    {24,  2, -1, "BUTTON B      %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_B},
    {24,  2, -1, "BUTTON X      %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_X},
    {24,  2, -1, "BUTTON Y      %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_Y},
    {24,  2, -1, "BUTTON MENU   %3s", &controllerInfo[0].unk0[0].button, (void *)PAD_BUTTON_MENU},
    {19,  2, -2, "stickX       %4d", &controllerInfo[0].unk0[0].stickX, NULL},
    {19,  2, -1, "stickY       %4d", &controllerInfo[0].unk0[0].stickY, NULL},
    {19,  2, -1, "substickX    %4d", &controllerInfo[0].unk0[0].substickX, NULL},
    {19,  2, -1, "substickY    %4d", &controllerInfo[0].unk0[0].substickY, NULL},
    {18,  2, -1, "triggerLeft  %4d", &controllerInfo[0].unk0[0].triggerLeft, NULL},
    {18,  2, -1, "triggerRight %4d", &controllerInfo[0].unk0[0].triggerRight, NULL},
    {18,  2, -1, "analogA      %4d", &controllerInfo[0].unk0[0].analogA, NULL},
    {18,  2, -1, "analogB      %4d", &controllerInfo[0].unk0[0].analogB, NULL},
    {18,  2, -1, "err          %4d", &controllerInfo[0].unk0[0].err, NULL},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc inputWindow = { 0, 0, 21, 30, lbl_801B6AD4, sizeof(struct ControllerInfo), 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B6D8C[] =
{
    {13,  1,  1, "Window Info", NULL, NULL},
    {15,  3,  3, "locateX : %3d", &unusedWindowX, NULL},
    {15,  3,  4, "locateY : %3d", &unusedWindowY, NULL},
    {15,  3,  5, "  sizeX : %3d", &unusedWindowWidth, NULL},
    {15,  3,  6, "  sizeY : %3d", &unusedWindowHeight, NULL},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc windowWindow = { 34, 28, 19, 9, lbl_801B6D8C, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B6F28[] =
{
    {13,  1,  1, "Change Param", NULL, NULL},
    { 3,  2,  3, "xang   %04hX", &lbl_802F1ED4, &angleRange},
    { 3,  2, -1, "yang   %04hX", &lbl_802F1ED2, &angleRange},
    { 3,  2, -1, "zang   %04hX", &lbl_802F1ED0, &angleRange},
    { 7,  2, -1, "xpos   %4.1f", &lbl_802F1ECC, &posScaleRange},
    { 7,  2, -1, "ypos   %4.1f", &lbl_802F1EC8, &posScaleRange},
    { 7,  2, -1, "zpos   %4.1f", &lbl_802F1EC4, &posScaleRange},
    { 7,  2, -1, "float  %4.1f", &lbl_802F1EC0, &posScaleRange},
    { 7,  2, -1, "float2 %4.1f", &lbl_802F1EBC, &posScaleRange},
    { 7,  2, -1, "float3 %4.1f", &lbl_802F1EB8, &posScaleRange},
    { 2,  2, -2, "xint   %4d",   &lbl_802F1EB4, &lbl_801B3B18},
    { 2,  2, -1, "yint   %4d",   &lbl_802F1EB0, &lbl_801B3B18},
    { 2,  2, -1, "zint   %4d",   &lbl_802F1EAC, &lbl_801B3B18},
    { 5,  2, -2, "char   %4o",   &lbl_802F1EA4, &lbl_801B3B18},
    {19,  2, -1, "          %c", &lbl_802F1EA4, NULL},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc lbl_801B70A8 = { 0, 18, 15, 19, lbl_801B6F28, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B7134[] =
{
    {13,  1,  1, "Bumper Param", NULL, NULL},
    { 7,  2,  2, "LOD LEVEL[0]: %4.5f", &lbl_8028C0B0.unk0[0], &lbl_801B3B38},
    { 7,  2,  3, "LOD LEVEL[1]: %4.5f", &lbl_8028C0B0.unk0[1], &lbl_801B3B38},
    { 7,  2,  4, "LOD LEVEL[2]: %4.5f", &lbl_8028C0B0.unk0[2], &lbl_801B3B38},
    { 7,  2,  5, "LOD LEVEL[3]: %4.5f", &lbl_8028C0B0.unk0[3], &lbl_801B3B38},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc lbl_801B71C4 = { 4, 4, 32, 8, lbl_801B7134, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B71FC[] =
{
    {13,  1,  1, "Heap Free", NULL, NULL},
    {29,  2,  3, NULL, NULL, NULL},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc lbl_801B7244 = { 0, 0, 38, 11, lbl_801B71FC, 0, 0, 0, 0, 0, 0 };

static struct WindowItem lbl_801B727C[] =
{
    {13,  1,  1, "Main Menu", NULL, NULL},
    { 0,  2, -2, "Dip Switch", &dipSwitchWindow, NULL},
    { 0,  2, -1, "Game", &lbl_801B4424, NULL},
    { 0,  2, -1, "Mode", &modeWindow, NULL},
    { 0,  2, -1, "Event", &eventWindow, NULL},
    { 0,  2, -1, "Camera", &cameraWindow, NULL},
    { 0,  2, -1, "Effect", &effectWindow, NULL},
    { 0,  2, -1, "Sprite", &spriteWindow, NULL},
    { 0,  2, -1, "Light Param", &lightParamWindow, NULL},
    { 0,  2, -1, "Light Group", &lightGroupWindow, NULL},
    { 0,  2, -1, "Fog", &fogWindow, NULL},
    { 0,  2, -1, "Sound", &soundWindow, NULL},
    { 0,  2, -1, "Bumper Param", &lbl_801B71C4, NULL},
    { 0,  2, -2, "Monkey Fight", &fightWindow, NULL},
    { 0,  2, -1, "Monkey Race", &lbl_801B610C, NULL},
    { 0,  2, -2, "Performance", &perfWindow, NULL},
    { 0,  2, -1, "Heap Free", &lbl_801B7244, NULL},
    { 0,  2, -1, "Input Test", &inputWindow, NULL},
    { 0,  2, -2, "Change Param", &lbl_801B70A8, NULL},
    { 0,  2, -1, "Window Info", &windowWindow, NULL},
    {31,  0,  0, NULL, NULL, NULL},
};

static struct WindowDesc mainMenuWindow = { 4, 2, 20, 30, lbl_801B727C, 0, 0, 0, 0, 0, 0 };

// bss

static u8 screenBuffer1[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer2[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer3[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer4[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer5[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer6[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer7[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBuffer8[SCREEN_ROWS * SCREEN_COLUMNS];

static struct WindowDesc *windowList[16];
static struct WindowDesc windowWork[16];

static void draw_window_frame(struct WindowDesc *arg0);
static void func_8002FCAC(int arg0, int arg1);
static void clear_buffer_region(int arg0, int arg1, int arg2, int arg3);
static void draw_char(int x, int y, s8 arg2, u8 colorId);
static void draw_some_window_quad_2(float x1, float y1, float x2, float y2);

static void func_8002DC54(void)
{
    struct WindowDesc **var_r3;
    int var_r4;

    if (lbl_802F1EA8 == 0)
    {
        var_r3 = &windowList[15];
        for (var_r4 = 15; var_r4 >= 0; var_r4--, var_r3--)
        {
            if (*var_r3 != 0)
                break;
        }
        if (var_r4 >= 0)
        {
            currWindowIndex = var_r4;
            lbl_802F1EA8 = 1;
        }
    }
    else
    {
        currWindowIndex --;
        if (currWindowIndex < 0 || windowList[currWindowIndex] == NULL)
        {
            currWindowIndex = 0;
            lbl_802F1EA8 = 0;
        }
    }
}

static void func_8002DD5C_inline(void)
{
    struct WindowDesc **r6 = &windowList[currWindowIndex];
    void *r4 = *r6;
    int i;

    for (i = currWindowIndex; i > 0; i--, r6--)
        *r6 = *(r6 - 1);
    windowList[0] = r4;
    currWindowIndex = 0;
}

static void func_8002DD5C(void)
{
    if (currWindowIndex == 0)
    {
        struct WindowDesc **r3 = windowList;
        struct WindowDesc *r4 = *r3;
        int i;

        for (i = 0; i < 15 && *(r3 + 1) != 0; i++, r3++)
            *r3 = *(r3 + 1);
        *r3 = r4;
        currWindowIndex = i;
    }
    else
    {
        func_8002DD5C_inline();
    }
}

#pragma dont_inline on
static void cycle_window(void)
{
    int r6;
    int i;

    for (i = 0; i < 16; i++)
    {
        r6 = ((currWindowIndex - i) + 16) % 16;
        if (windowList[r6] != 0)
        {
            currWindowIndex = r6;
            lbl_802F1EA8 = 1;
            break;
        }
    }
    if (i >= 16)
    {
        currWindowIndex = 0;
        lbl_802F1EA8 = 0;
    }
}
#pragma dont_inline reset

static void window_open(struct WindowDesc *arg0)
{
    int i;
    struct WindowDesc **r6;
    struct WindowDesc *var_r7;
    struct WindowDesc *new_var;

    lbl_802F1EA8 = 1;
    if (windowList[15] == NULL)
    {
        for (i = 0; i < 16; i++)
        {
            if (!windowWork[i].isActive)
                break;
        }
        if (i < 16)
        {
            new_var = &windowWork[i];
            var_r7 = new_var;
            var_r7->x = arg0->x;
            var_r7->y = arg0->y;
            var_r7->width = arg0->width;
            var_r7->height = arg0->height;
            var_r7->items = arg0->items;
            var_r7->structSize = arg0->structSize;
            var_r7->structIndex = arg0->structIndex;
            var_r7->selection = arg0->selection;
            var_r7->isActive = TRUE;
            var_r7->unk24 = arg0->unk24;
            var_r7->unk28 = arg0->unk28;
            r6 = windowList;
            for (i = 0; i < 16; i++, r6++)
            {
                void *old = *r6;
                *r6 = var_r7;
                var_r7 = old;
                if (old == NULL)
                    break;
            }
            currWindowIndex = 0;
        }
    }
}

static void process_window(struct WindowDesc *window)
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
    struct WindowDesc *subWindow;
    u32 *pValue;
    s32 *new_var;

    // resize window
    if ((analogButtonInfo[0][4] & 0x80) || ((analogButtonInfo[0][0] & 0x80) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->height--;
        window->height = MAX(window->height, 3);
    }
    else if ((analogButtonInfo[0][4] & 0x40) || ((analogButtonInfo[0][0] & 0x40) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->height++;
        window->height = MIN(window->height, SCREEN_ROWS - window->y);
    }
    if ((analogButtonInfo[0][4] & 0x10) || ((analogButtonInfo[0][0] & 0x10) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->width--;
        window->width = MAX(window->width, 3);
    }
    else if ((analogButtonInfo[0][4] & 0x20) || ((analogButtonInfo[0][0] & 0x20) && (analogButtonInfo[0][0] & 0x200)))
    {
        window->width++;
        window->width = MIN(window->width, SCREEN_COLUMNS - window->x);
    }
    if ((analogButtonInfo[0][4] & 8) || ((analogButtonInfo[0][0] & 8) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r5 = window->y - 1;
        var_r6 = MAX(temp_r5, 0);
        window->unk28 += (window->y - var_r6) * 12;
        window->y = var_r6;
    }
    else if ((analogButtonInfo[0][4] & 4) || ((analogButtonInfo[0][0] & 4) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r5_2 = window->y;
        var_r6_2 = temp_r5_2 + 1;
        temp_r3_4 = SCREEN_ROWS - window->height;
        var_r6_2 = MIN(var_r6_2, temp_r3_4);
        window->unk28 += (temp_r5_2 - var_r6_2) * 12;
        window->y = var_r6_2;
    }
    if ((analogButtonInfo[0][4] & 1) || ((analogButtonInfo[0][0] & 1) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r4 = window->x - 1;
        var_r6 = MAX(temp_r4, 0);
        window->unk24 += (window->x - var_r6) * 12;
        window->x = var_r6;
    }
    else if ((analogButtonInfo[0][4] & 2) || ((analogButtonInfo[0][0] & 2) && (analogButtonInfo[0][0] & 0x200)))
    {
        temp_r5_2 = window->x;
        var_r6_2 = temp_r5_2 + 1;
        temp_r3_4 = SCREEN_COLUMNS - window->width;
        var_r6_2 = MIN(var_r6_2, temp_r3_4);
        window->unk24 += (temp_r5_2 - var_r6_2) * 12;
        window->x = var_r6_2;
    }

    if (lbl_802F1E40 != 0)
    {
        temp_r5_4 = window->selection;
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
        window->selection = var_r6_3;
    }
    else if (lbl_802F1E44 != 0)
    {
        var_r7 = temp_r6_3 = window->selection;
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
        window->selection = var_r7;
    }
    item = &window->items[window->selection];
    switch ((s32)item->type)
    {
    case 0:
        if (controllerInfo[0].unk0[4].button & PAD_BUTTON_A)
        {
            subWindow = item->pValue;
            if (subWindow != NULL)
            {
                lbl_802F1EA8 = 1U;
                windowList[currWindowIndex]->x = subWindow->x;
                windowList[currWindowIndex]->y = subWindow->y;
                windowList[currWindowIndex]->width = subWindow->width;
                windowList[currWindowIndex]->height = subWindow->height;
                windowList[currWindowIndex]->items = subWindow->items;
                windowList[currWindowIndex]->structSize = subWindow->structSize;
                windowList[currWindowIndex]->structIndex = subWindow->structIndex;
                windowList[currWindowIndex]->selection = subWindow->selection;
            }
        }
        break;
    case 11:
        if ((controllerInfo[0].unk0[4].button & PAD_BUTTON_A) && item->pValue != NULL)
            window_open(item->pValue);
        break;
    default:
        if ((controllerInfo[0].unk0[4].button & PAD_BUTTON_A)
         || (controllerInfo[0].unk0[4].button & PAD_BUTTON_B)
         || ((analogButtonInfo[0][0] & 0x200) && ((controllerInfo[0].unk0[0].button & PAD_BUTTON_A) || (controllerInfo[0].unk0[0].button & PAD_BUTTON_B))))
        {
            if (item->type & 0x200)
                var_r8 = 0;
            else
                var_r8 = window->structIndex;
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
                    someintval = window->structIndex;
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
                    window->structIndex = (u32)someintval;
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
                somefloatval = 1.0f;
                deltaFloat = someParamF->delta;
                deltaFloat = deltaFloat * somefloatval;//dumb(deltaFloat, 1.0f);
                somefloatval = *(float *)temp_r4_6;
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

static void set_some_params(int a, int b, int c, int d)
{
    lbl_802F1E1C = a;
    lbl_802F1E20 = b;
    lbl_802F1E24 = c;
    lbl_802F1E28 = d;
}

static void window_show_items(struct WindowDesc *window, u32 arg1)
{
    struct WindowItem *item;
    int i;
    int x, y;
    char *onOff;
    void *pValue;
    int var_r5;

    if (arg1 != 0)
        func_8002FCAC(window->unk24, window->unk28);
    else
        func_8002FCAC(0, 0);
    clear_buffer_region(window->x, window->y, window->width, window->height);
    set_some_params(window->x, window->y, window->width, window->height);
    if (arg1 != 0)
        window_set_text_color(2);
    else
        window_set_text_color(0);
    draw_window_frame(window);
    window_set_text_color(0);
    set_some_params(window->x + 1, window->y + 1, window->width - 2, window->height - 2);
    x = 0;
    y = 0;
    i = 0;
    item = window->items;
    while (item->type != ITEM_END)
    {
        if (item->x > 0)
            x = window->x + item->x;
        else
            x -= item->x;
        if (item->y > 0)
            y = window->y + item->y;
        else
            y -= item->y;
        window_set_cursor_pos(x, y);
        window_set_text_color(0);
        switch (item->type & 0xFF)
        {
        case ITEM_NONE:
        case 26:
            if (i == 0 && arg1 != 0)
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
            var_r5 = window->structIndex;
        pValue = (u8 *)item->pValue + (window->structSize * var_r5);
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
            window_printf(2, item->format, window->structIndex);
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
                onOff = " ON";
            else
                onOff = "OFF";
            window_printf(2, item->format, onOff);
            break;
        case 24:
            if (*(u16 *)pValue & (u16)(u32)item->param)
                onOff = " ON";
            else
                onOff = "OFF";
            window_printf(2, item->format, onOff);
            break;
        case 26:
            window_printf(2, item->format, window->structIndex);
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
            window_printf(2, item->format, u_stateNames[((s8 *)item->pValue)[window->structIndex]]);
            break;
        }
        if (window->selection == i && arg1 != 0)
        {
            // draw selection arrow
            window_set_cursor_pos(x - 1, y);
            window_set_text_color(1);
            u_set_window_text(2, "\x1C");
        }
        item++;
        i++;
    }
    func_8002FCAC(0, 0);
}

static void draw_window_frame(struct WindowDesc *window)
{
    int i;

    set_some_params(window->x, window->y, window->width, window->height);

    // top left corner
    window_set_cursor_pos(window->x, window->y);
    u_set_window_text(2, "\x18");

    // top right corner
    window_set_cursor_pos(window->x + window->width - 1, window->y);
    u_set_window_text(2, "\x19");

    // bottom left corner
    window_set_cursor_pos(window->x, window->y + window->height - 1);
    u_set_window_text(2, "\x1A");

    // bottom right corner
    window_set_cursor_pos(window->x + window->width - 1, window->y + window->height - 1);
    u_set_window_text(2, "\x1B");

    // top border
    for (i = 1; i < window->width - 1; i++)
    {
        window_set_cursor_pos(window->x + i, window->y);
        u_set_window_text(2, "\x16");
    }

    // left border
    for (i = 1; i < window->height - 1; i++)
    {
        window_set_cursor_pos(window->x, window->y + i);
        u_set_window_text(2, "\x17");
    }

    // bottom border
    for (i = 1; i < window->width - 1; i++)
    {
        window_set_cursor_pos(window->x + i, window->y + window->height - 1);
        u_set_window_text(2, "\x16");
    }

    // right border
    for (i = 1; i < window->height - 1; i++)
    {
        window_set_cursor_pos(window->x + window->width - 1, window->y + i);
        u_set_window_text(2, "\x17");
    }
}

void window_draw(void)
{
    u8 *var_r30_2;
    struct WindowDesc **var_r30;
    int col;
    int i;
    int row;
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
    var_r30 = windowList;
    for (i = 0; i < 16; i++, var_r30++)
    {
        temp_r7 = *var_r30;
        if (temp_r7 != NULL)
        {
            temp_r5 = temp_r7->x;
            temp_r8 = temp_r7->y;
            var_r6 = (temp_r5 * 12) + 6;
            var_r5 = (temp_r8 * 12) + 6;
            var_r8 = ((temp_r5 + temp_r7->width) * 12) - 6;
            var_r9 = ((temp_r8 + temp_r7->height) * 12) - 6;
            if (i == currWindowIndex && lbl_802F1EA8 != 0U)
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

    var_r26 = (s8 *)screenBuffer3;
    var_r30_2 = screenBuffer6;
    var_r25 = (s8 *)screenBuffer7;
    var_r24 = (s8 *)screenBuffer8;
    for (row = 0; row < SCREEN_ROWS; row++)
    {
        for (col = 0; col < SCREEN_COLUMNS; col++)
        {
            if (*var_r26 != 0x20)
            {
                draw_char(
                    *var_r25 + (col * 12),
                    *var_r24 + (row * 12),
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

    currWindowIndex = 0;
    lbl_802F1EA8 = 0U;
    for (i = 0; i < 16; i++)
        windowWork[i].isActive = FALSE;
    memset(screenBuffer1, 0x20, sizeof(screenBuffer1));
    memset(screenBuffer2, 0x20, sizeof(screenBuffer2));
    memset(screenBuffer3, 0x20, sizeof(screenBuffer3));
    memset(screenBuffer4, 0, sizeof(screenBuffer4));
    memset(screenBuffer5, 0, sizeof(screenBuffer5));
    memset(screenBuffer6, 0, sizeof(screenBuffer6));
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
    u32 *bufPtr;
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

    lbl_802F1E40 = ((controllerInfo[0].unk0[4].button & PAD_BUTTON_UP) || ((controllerInfo[0].unk0[0].button & PAD_BUTTON_UP) && (analogButtonInfo[0][0] & 0x200)));
    lbl_802F1E44 = ((controllerInfo[0].unk0[4].button & PAD_BUTTON_DOWN) || ((controllerInfo[0].unk0[0].button & PAD_BUTTON_DOWN) && (analogButtonInfo[0][0] & 0x200)));
    lbl_802F1E48 = ((controllerInfo[0].unk0[4].button & PAD_BUTTON_LEFT) || ((controllerInfo[0].unk0[0].button & PAD_BUTTON_LEFT) && (analogButtonInfo[0][0] & 0x200)));
    lbl_802F1E4C = ((controllerInfo[0].unk0[4].button & PAD_BUTTON_RIGHT) || ((controllerInfo[0].unk0[0].button & PAD_BUTTON_RIGHT) && (analogButtonInfo[0][0] & 0x200)));

    // clear buffers (not using memset for some reason)
    bufPtr = (u32 *)screenBuffer7;
    for (var_ctr = SCREEN_ROWS * SCREEN_COLUMNS / 4 - 3; var_ctr >= 0; var_ctr--)
        *bufPtr++ = 0;
    bufPtr = (u32 *)screenBuffer8;
    for (var_ctr = SCREEN_ROWS * SCREEN_COLUMNS / 4 - 3; var_ctr >= 0; var_ctr--)
        *bufPtr++ = 0;

    memcpy(screenBuffer3, screenBuffer2, sizeof(screenBuffer3));
    memcpy(screenBuffer6, screenBuffer5, sizeof(screenBuffer6));

    var_r12 = screenBuffer1;
    var_r8 = screenBuffer3;
    var_r9 = screenBuffer4;
    var_r10 = screenBuffer6;
    for (var_r3_7 = 0; var_r3_7 < SCREEN_ROWS; var_r3_7++)
    {
        var_r4 = var_r12;
        var_r5 = var_r8;
        var_r6 = var_r9;
        var_r7 = var_r10;
        for (var_ctr_3 = 0; var_ctr_3 < SCREEN_COLUMNS; var_ctr_3++)
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
        var_r12 += SCREEN_COLUMNS;
        var_r8 += SCREEN_COLUMNS;
        var_r9 += SCREEN_COLUMNS;
        var_r10 += SCREEN_COLUMNS;
    }

    memset(screenBuffer1, 0x20, sizeof(screenBuffer1));
    memset(screenBuffer4, 0, sizeof(screenBuffer4));

    if (analogButtonInfo[0][0] & 0x100)
    {
        if ((dipSwitches & 1) && (controllerInfo[0].unk0[2].button & PAD_BUTTON_START))
        {
            window_open(&mainMenuWindow);
        }
        else if (controllerInfo[0].unk0[2].button & PAD_BUTTON_Y)
        {
            if (lbl_802F1EA8 != 0U)
            {
                temp_r3 = windowList[currWindowIndex];
                if (temp_r3 != NULL)
                {
                    lbl_802F1EA8 = 1U;
                    temp_r3->isActive = FALSE;

                    var_r4_2 = &windowList[currWindowIndex];
                    for (var_ctr_4 = currWindowIndex; var_ctr_4 < 15; var_ctr_4++, var_r4_2++)
                        *var_r4_2 = *(var_r4_2 + 1);
                    *var_r4_2 = NULL;

                    cycle_window();
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
    if (var_r22 != 0 && lbl_802F1EA8 != 0 && windowList[currWindowIndex] != 0)
        process_window(windowList[currWindowIndex]);
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
        if (windowList[var_r31] != NULL)
        {
            if (var_r31 == currWindowIndex && lbl_802F1EA8 != 0)
                window_show_items(windowList[var_r31], 1);
            else
                window_show_items(windowList[var_r31], 0);
        }
    }
    window_set_text_color(0);
    if (lbl_802F1EA8 != 0U)
    {
        temp_r3_7 = windowList[currWindowIndex];
        if (temp_r3_7 != NULL)
        {
            unusedWindowX = temp_r3_7->x;
            unusedWindowY = temp_r3_7->y;
            unusedWindowWidth = temp_r3_7->width;
            unusedWindowHeight = temp_r3_7->height;
            return;
        }
    }
    unusedWindowX = 0;
    unusedWindowY = 0;
    unusedWindowWidth = 0;
    unusedWindowHeight = 0;
}

void window_set_cursor_pos(int x, int y)
{
    windowCursorX = x;
    lbl_802F1E0C = x;
    windowCursorY = y;
}

void window_move_cursor(int dx, int dy)
{
    windowCursorX += dx;
    windowCursorY += dy;
}

static void func_8002FCAC(int arg0, int arg1)
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
    windowColorId = arg0;
}

void u_set_window_text(int arg0, const char *text)
{
    u8 *var_r5;
    u8 *var_r6;
    u8 *var_r7;
    u8 *var_r8;
    int xmin;
    int ymin;
    int xmax;
    int ymax;

    switch (arg0)
    {
    case 0:
        xmin = 0;
        ymin = 0;
        xmax = 0x34;
        var_r5 = screenBuffer1 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        ymax = 0x24;
        var_r6 = screenBuffer4 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r7 = NULL;
        var_r8 = NULL;
        break;
    case 1:
        xmin = 0;
        ymin = 0;
        xmax = 0x34;
        var_r5 = screenBuffer2 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        ymax = 0x24;
        var_r6 = screenBuffer5 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r7 = NULL;
        var_r8 = NULL;
        break;
    default:
        xmin = lbl_802F1E1C;
        ymin = lbl_802F1E20;
        xmax = xmin + lbl_802F1E24 - 1;
        var_r5 = screenBuffer3  + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        ymax = ymin + lbl_802F1E28 - 1;
        var_r6 = screenBuffer6 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r7 = screenBuffer7 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r8 = screenBuffer8 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        break;
    }

    while (*text != 0)
    {
        if (windowCursorX >= SCREEN_COLUMNS)
        {
            // wrap to new line
            windowCursorX = 0;
            windowCursorY++;
        }
        if (*text == '\n')
        {
            if (windowCursorX >= lbl_802F1E0C)
                windowCursorY++;
            windowCursorX = lbl_802F1E0C - 1;
            switch (arg0)
            {
            case 0:
                var_r5 = screenBuffer1 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r6 = screenBuffer4 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                break;
            case 1:
                var_r5 = screenBuffer2 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r6 = screenBuffer5 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                break;
            default:
                var_r5 = screenBuffer3 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r6 = screenBuffer6 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r7 = screenBuffer7 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r8 = screenBuffer8 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                break;
            }
        }
        else if (windowCursorX >= xmin && windowCursorX <= xmax
         && windowCursorY >= ymin && windowCursorY <= ymax)
        {
            *var_r5 = *text;
            *var_r6 = windowColorId;
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
        windowCursorX++;
    }
}

void u_clear_buffers_2_and_5(void)
{
    memset(screenBuffer2, 0x20, sizeof(screenBuffer2));
    memset(screenBuffer5, 0, sizeof(screenBuffer5));
}

static void clear_buffer_region(int arg0, int arg1, int arg2, int arg3)
{
    u8 *var_r8;
    u8 *var_r9;
    int row;
    int col;

    for (row = arg1; row < arg1 + arg3; row++)
    {
        var_r8 = screenBuffer3 + row * SCREEN_COLUMNS + arg0;
        var_r9 = screenBuffer6 + row * SCREEN_COLUMNS + arg0;

        for (col = arg0; col < arg0 + arg2; col++)
        {
            *var_r8++ = 0x20;
            *var_r9++ = 0;
        }
    }
}

#pragma force_active on
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
#pragma force_active reset

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

#define RGBA_alt(r, g, b, a) (((r) << 24) | ((g) << 16) | ((b) << 8) | (a))

static void draw_char(int x, int y, s8 arg2, u8 colorId)
{
    static u32 textColors[] =
    {
        RGBA_alt(255, 255, 255, 255),
        RGBA_alt(255,   0,   0, 255),
        RGBA_alt(  0, 255,   0, 255),
        RGBA_alt(  0,   0, 255, 255),
        RGBA_alt(255, 255,   0, 255),
        RGBA_alt(255,   0, 255, 255),
        RGBA_alt(  0, 255, 255, 255),
        RGBA_alt(  0,   0,   0, 255),
    };

    if (arg2 < 0x80)
    {
        float u = (12.0 * (float)(arg2 & 0xF)) / 192.0;
        float v = (12.0 * (float)((arg2 >> 4) & 0xF)) / 96.0;
        float x1 = x;
        float y1 = y;
        u32 temp;
        GXColor sp20;
        GXColor sp1C = {0};

        y1 *= 1.0714285714285714;
        temp = textColors[colorId & 0xFF];
        sp20.r = (temp >> 24) /*& 0xFF*/;
        sp20.g = (temp >> 16) & 0xFF;
        sp20.b = (temp >> 8) /*& 0xFF*/;
        sp20.a = temp & 0xFF;

        GXSetTevColor(GX_TEVREG0, sp20);
        GXSetTevColor(GX_TEVREG1, sp1C);
        GXBegin(GX_QUADS, GX_VTXFMT7, 4);
        GXPosition3f32(x1,         y1,         -0.0078125f);
        GXTexCoord2f32(u,          v + 0.010416666666666666);
        GXPosition3f32(x1 + 12.0f, y1,         -0.0078125f);
        GXTexCoord2f32(u + 0.0625, 0.010416666666666666 + v);
        GXPosition3f32(x1 + 12.0f, y1 + 12.0f, -0.0078125f);
        GXTexCoord2f32(u + 0.0625, v + 0.125);
        GXPosition3f32(x1,         y1 + 12.0f, -0.0078125f);
        GXTexCoord2f32(u,          v + 0.125);
        GXEnd();
    }
}

static void draw_some_window_quad_2(float x1, float y1, float x2, float y2)
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

#pragma force_active on
u32 lbl_802F0980 = 32;
#pragma force_active reset
