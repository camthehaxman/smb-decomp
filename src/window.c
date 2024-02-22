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
static s32 windowCursorStartX;
static s32 windowCursorX;
static s32 windowCursorY;
static s8 textOffsetX;
static s8 textOffsetY;
static u32 u_windowBoxX;
static u32 u_windowBoxY;
static u32 u_windowBoxWidth;
static u32 u_windowBoxHeight;
static u8 windowColorId;
static u32 currWindowX;
static u32 currWindowY;
static u32 currWindowWidth;
static u32 currWindowHeight;
static BOOL u_selectionRelated1;
static BOOL u_selectionRelated2;
static u32 unused1;
static u32 unused2;
static s8 lbl_802F1E50;
static s8 lbl_802F1E51;
static s32 lbl_802F1E54;
static s32 lbl_802F1E58;

#define RANGE_NOMIN (1 << 0)  // ignore min and allow value to decrease indefinitely
#define RANGE_NOMAX (1 << 1)  // ignore max and allow value to increase indefinitely

struct RangeInt
{
    u32 flags;
    s32 min, max;  // Minimum and maximum value allowed
    s32 delta;     // Amount to increment or decrement
};

struct RangeFloat
{
    u32 flags;
    float min, max;  // Minimum and maximum value allowed
    float delta;     // Amount to increment or decrement
};

struct WindowItem
{
    u32 type;      // Type of item. See the enum WindowItemType below.
    s32 x;         // x position (column) of the item within its window
    s32 y;         // y position (row) of the item within its window
    char *format;  // A printf-style format string, which displays the value pointed to by "pValue".
    void *pValue;  // Pointer to a value to display and/or modify. The type of value pointed to is
                   // specified by "type".
    void *param;   // An additional parameter. For values that can be modified, this is a pointer to
                   // a RangeInt or RangeFloat which specifies the minimum and maximum allowed value
                   // as well as the delta for how much the value is incremented or decremented.
};

// specifies the parameter to the format string
enum WindowItemType
{
    //                       format arg   pValue                  param                 remarks
    WI_SUBWINDOW   =  0,  // none         pointer to WindowDesc   none                  Displays a label which switches to the window specified in pValue when clicked.
    WI_EDIT_U32    =  1,  // %d           pointer to u32          pointer to RangeInt   Displays and edits the u32 value at pValue
    WI_EDIT_S32    =  2,  // %d           pointer to s32          pointer to RangeInt   Displays and edits the s32 value at pValue
    WI_EDIT_U16    =  3,  // %d           pointer to u16          pointer to RangeInt   Displays and edits the u16 value at pValue
    WI_EDIT_S16    =  4,  // %d           pointer to s16          pointer to RangeInt   Displays and edits the s16 value at pValue
    WI_EDIT_U8     =  5,  // %d           pointer to u8           pointer to RangeInt   Displays and edits the u8 value at pValue
    WI_EDIT_S8     =  6,  // %d           pointer to s8           pointer to RangeInt   Displays and edits the s8 value at pValue
    WI_EDIT_F32    =  7,  // %f           pointer to f32          pointer to RangeFloat Displays and edits the f32 value at pValue
    WI_EDIT_BIT32  =  8,  // %s           pointer to u32          mask (u32)            Displays "ON" or "OFF" depending whether bit specified in param is set in the value. Toggles the bit when clicked
    WI_SETWINITEMS =  9,
    WI_EDIT_INDEX  = 10,  // %d           none                    pointer to RangeInt   Displays and edits the window's structIndex value. Value is clamped by the range specified in param
    WI_OPENWIN     = 11,
    WI_LABEL       = 13,  // none         none                    none                  A simple text label
    WI_U32         = 14,  // %d           pointer to u32          none                  Displays the u32 value at pValue
    WI_S32         = 15,  // %d           pointer to s32          none                  Displays the s32 value at pValue
    WI_U16         = 16,  // %d           pointer to u16          none                  Displays the u16 value at pValue
    WI_S16         = 17,  // %d           pointer to s16          none                  Displays the s16 value at pValue
    WI_U8          = 18,  // %d           pointer to u8           none                  Displays the u8 value at pValue
    WI_S8          = 19,  // %d           pointer to s8           none                  Displays the s8 value at pValue
    WI_F32         = 20,  // %f           pointer to f32          none                  Displays the f32 value at pValue
    WI_STRING      = 21,  // %s           pointer to char array   none                  Displays the string at pValue
    WI_STRINGPTR   = 22,  // %s           pointer to char*        none                  Displays the string at pValue. Unlike WI_STRING, pValue is a double pointer (pointer to pointer to char array).
    WI_BIT32       = 23,  // %s           pointer to u32          mask (u32)            Displays "ON" or "OFF" depending whether bit specified in param is set in the value
    WI_BIT16       = 24,  // %s           pointer to u16          mask (u16)            Displays "ON" or "OFF" depending whether bit specified in param is set in the value
    WI_STRINGTABLE = 25,  // %s           pointer to s8           sring table (char**)  Displays the nth string in the string table, where n is the s8 value at pValue
    WI_INDEX       = 26,  // %i           none                    none                  Displays the window's structIndex value
    WI_HEAPINFO    = 29,  // none         none                    none                  Displays information about the memory heaps
    WI_POOLSTATE   = 30,  // %s           pointer to statusList   none                  Displays a string representing the state in statusList, using structIndex as the index
                          //              in g_poolInfo
    WI_END         = 31,  // none         none                    none                  Marks the end of the WindowItem list
};

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

static char *offOnStrTable[] = { "OFF", "ON" };

static struct RangeInt   lbl_801B3B18 = { RANGE_NOMIN|RANGE_NOMAX, 0, 100, 1 };
static struct RangeFloat posScaleRange = { RANGE_NOMIN|RANGE_NOMAX, -10.0f, 10.0f, 0.1f };
static struct RangeFloat lbl_801B3B38 = { RANGE_NOMIN|RANGE_NOMAX, -10.0f, 10.0f, 0.001f };
static struct RangeFloat lbl_801B3B38_2 = { RANGE_NOMIN|RANGE_NOMAX, -10.0f, 10.0f, 0.00001f };
static struct RangeFloat lbl_801B3B38_3 = { RANGE_NOMIN|RANGE_NOMAX, -10.0f, 10.0f, 0.01f };
static struct RangeInt   angleRange = { RANGE_NOMIN|RANGE_NOMAX, 0, 0, 128 };
static struct RangeInt   lbl_801B3B78 = { 0, 0, 1, 1 };

/* Dip Switch debug window */

struct WindowItem dipSwitchWindowItems[] =
{
    {WI_LABEL,      1,  1, "Dip Switch",           NULL,         NULL},
    {WI_EDIT_BIT32, 2,  3, "                  %s", &dipSwitches, (void *)DIP_DEBUG},
    {WI_EDIT_BIT32, 2,  4, "                  %s", &dipSwitches, (void *)DIP_DISP},
    {WI_EDIT_BIT32, 2,  5, "                  %s", &dipSwitches, (void *)DIP_STCOLI},
    {WI_EDIT_BIT32, 2,  6, "                  %s", &dipSwitches, (void *)DIP_TRIANGLE},
    {WI_EDIT_BIT32, 2,  8, "                  %s", &dipSwitches, (void *)DIP_BALL_TGT},
    {WI_EDIT_BIT32, 2,  9, "                  %s", &dipSwitches, (void *)DIP_JOINT_OFF},
    {WI_EDIT_BIT32, 2, 10, "                  %s", &dipSwitches, (void *)DIP_APE_COLOR},
    {WI_EDIT_BIT32, 2, 11, "                  %s", &dipSwitches, (void *)DIP_TIME_STOP},
    {WI_EDIT_BIT32, 2, 13, "                  %s", &dipSwitches, (void *)DIP_KARAKUCHI},
    {WI_EDIT_BIT32, 2, 14, "                  %s", &dipSwitches, (void *)DIP_NAMEENTRY},
    {WI_EDIT_BIT32, 2, 15, "                  %s", &dipSwitches, (void *)DIP_SHADOW_TEST},
    {WI_EDIT_BIT32, 2, 16, "                  %s", &dipSwitches, (void *)DIP_FALL_DISP},
    {WI_EDIT_BIT32, 2, 18, "                  %s", &dipSwitches, (void *)DIP_TEST_CAM},
    {WI_EDIT_BIT32, 2, 19, "                  %s", &dipSwitches, (void *)DIP_NO_INTR},
    {WI_EDIT_BIT32, 2, 20, "                  %s", &dipSwitches, (void *)DIP_CAPTURE},
    {WI_EDIT_BIT32, 2, 21, "                  %s", &dipSwitches, (void *)DIP_OLD_BALL},
    {WI_EDIT_BIT32, 2, 23, "                  %s", &dipSwitches, (void *)DIP_NO_KARI},
    {WI_EDIT_BIT32, 2, 24, "                  %s", &dipSwitches, (void *)DIP_PLAY_STG_ALL},
    {WI_EDIT_BIT32, 2, 25, "                  %s", &dipSwitches, (void *)DIP_PLAY_PNT_X10},
    {WI_EDIT_BIT32, 2, 26, "                  %s", &dipSwitches, (void *)DIP_SARU_0},
    {WI_EDIT_BIT32, 2, 28, "                  %s", &dipSwitches, (void *)DIP_SWITCH20},
    {WI_EDIT_BIT32, 2, 29, "                  %s", &dipSwitches, (void *)DIP_SWITCH21},
    {WI_EDIT_BIT32, 2, 30, "                  %s", &dipSwitches, (void *)DIP_SWITCH22},
    {WI_EDIT_BIT32, 2, 31, "                  %s", &dipSwitches, (void *)DIP_SWITCH23},
    {WI_EDIT_BIT32, 2, 33, "                  %s", &dipSwitches, (void *)DIP_SWITCH24},
    {WI_EDIT_BIT32, 2, 34, "                  %s", &dipSwitches, (void *)DIP_SWITCH25},
    {WI_EDIT_BIT32, 2, 35, "                  %s", &dipSwitches, (void *)DIP_SWITCH26},
    {WI_EDIT_BIT32, 2, 36, "                  %s", &dipSwitches, (void *)DIP_SWITCH27},
    {WI_EDIT_BIT32, 2, 38, "                  %s", &dipSwitches, (void *)DIP_SWITCH28},
    {WI_EDIT_BIT32, 2, 39, "                  %s", &dipSwitches, (void *)DIP_SWITCH29},
    {WI_EDIT_BIT32, 2, 40, "                  %s", &dipSwitches, (void *)DIP_SWITCH30},
    {WI_EDIT_BIT32, 2, 41, "                  %s", &dipSwitches, (void *)DIP_SWITCH31},
    {WI_LABEL,      3,  3, "DIP_DEBUG",            NULL,         NULL},
    {WI_LABEL,      3,  4, "DIP_DISP",             NULL,         NULL},
    {WI_LABEL,      3,  5, "DIP_STCOLI",           NULL,         NULL},
    {WI_LABEL,      3,  6, "DIP_TRIANGLE",         NULL,         NULL},
    {WI_LABEL,      3,  8, "DIP_BALL_TGT",         NULL,         NULL},
    {WI_LABEL,      3,  9, "DIP_JOINT_OFF",        NULL,         NULL},
    {WI_LABEL,      3, 10, "DIP_APE_COLOR",        NULL,         NULL},
    {WI_LABEL,      3, 11, "DIP_TIME_STOP",        NULL,         NULL},
    {WI_LABEL,      3, 13, "DIP_KARAKUCHI",        NULL,         NULL},
    {WI_LABEL,      3, 14, "DIP_NAMEENTRY",        NULL,         NULL},
    {WI_LABEL,      3, 15, "DIP_SHADOW_TEST",      NULL,         NULL},
    {WI_LABEL,      3, 16, "DIP_FALL_DISP",        NULL,         NULL},
    {WI_LABEL,      3, 18, "DIP_TEST_CAM",         NULL,         NULL},
    {WI_LABEL,      3, 19, "DIP_NO_INTR",          NULL,         NULL},
    {WI_LABEL,      3, 20, "DIP_CAPTURE",          NULL,         NULL},
    {WI_LABEL,      3, 21, "DIP_OLD_BALL",         NULL,         NULL},
    {WI_LABEL,      3, 23, "DIP_NO_KARI",          NULL,         NULL},
    {WI_LABEL,      3, 24, "DIP_PLAY_STG_ALL",     NULL,         NULL},
    {WI_LABEL,      3, 25, "DIP_PLAY_PNT_x10",     NULL,         NULL},
    {WI_LABEL,      3, 26, "DIP_SARU_0",           NULL,         NULL},
    {WI_LABEL,      3, 28, "DIP_SWITCH20",         NULL,         NULL},
    {WI_LABEL,      3, 29, "DIP_SWITCH21",         NULL,         NULL},
    {WI_LABEL,      3, 30, "DIP_SWITCH22",         NULL,         NULL},
    {WI_LABEL,      3, 31, "DIP_SWITCH23",         NULL,         NULL},
    {WI_LABEL,      3, 33, "DIP_SWITCH24",         NULL,         NULL},
    {WI_LABEL,      3, 34, "DIP_SWITCH25",         NULL,         NULL},
    {WI_LABEL,      3, 35, "DIP_SWITCH26",         NULL,         NULL},
    {WI_LABEL,      3, 36, "DIP_SWITCH27",         NULL,         NULL},
    {WI_LABEL,      3, 38, "DIP_SWITCH28",         NULL,         NULL},
    {WI_LABEL,      3, 39, "DIP_SWITCH29",         NULL,         NULL},
    {WI_LABEL,      3, 40, "DIP_SWITCH30",         NULL,         NULL},
    {WI_LABEL,      3, 41, "DIP_SWITCH31",         NULL,         NULL},
    {WI_END,        0,  0, NULL,                   NULL,         NULL},
};

static struct WindowDesc dipSwitchWindow = { 9, 0, 25, 29, dipSwitchWindowItems, 0, 0, 0, 0, 0, 0 };

/* Game debug window */

static struct WindowItem gameWindowItems[] =
{
    {WI_LABEL, 1, 1, "Game", NULL, NULL},
    {WI_END,   0, 0, NULL,   NULL, NULL},
};

static struct WindowDesc gameWindow = { 0, 0, 10, 10, gameWindowItems, 0, 0, 0, 0, 0, 0 };

/* Mode debug window */

static struct WindowItem modeWIndowItems[] =
{
    {WI_LABEL,     1, 1, "Mode",          NULL,              NULL},
    {WI_S16,       3, 3, "main_mode: %d", &gameMode,         NULL},
    {WI_STRINGPTR, 3, 4, "  %-14s",       &modeStringPtr,    NULL},
    {WI_S16,       3, 5, " sub_mode: %d", &gameSubmode,      NULL},
    {WI_STRINGPTR, 3, 6, "  %-14s",       &submodeStringPtr, NULL},
    {WI_S32,       3, 8, "mode.cntr: %d", &modeCtrl,         NULL},
    {WI_END,       0, 0, NULL,            NULL,              NULL},
};

static struct WindowDesc modeWindow = { 33, 0, 20, 11, modeWIndowItems, 0, 0, 0, 0, 0, 0 };

/* Event debug window */

static struct WindowItem eventWindowItems[] =
{
    {WI_LABEL,        1,  1, "Event", NULL,                NULL},
    {WI_STRINGPTR,    2,  3, "%10s",  &eventInfo[0].name,  NULL},
    {WI_STRINGPTR,    2,  4, "%10s",  &eventInfo[1].name,  NULL},
    {WI_STRINGPTR,    2,  5, "%10s",  &eventInfo[2].name,  NULL},
    {WI_STRINGPTR,    2,  6, "%10s",  &eventInfo[3].name,  NULL},
    {WI_STRINGPTR,    2,  7, "%10s",  &eventInfo[4].name,  NULL},
    {WI_STRINGPTR,    2,  8, "%10s",  &eventInfo[5].name,  NULL},
    {WI_STRINGPTR,    2,  9, "%10s",  &eventInfo[6].name,  NULL},
    {WI_STRINGPTR,    2, 10, "%10s",  &eventInfo[7].name,  NULL},
    {WI_STRINGPTR,    2, 11, "%10s",  &eventInfo[8].name,  NULL},
    {WI_STRINGPTR,    2, 12, "%10s",  &eventInfo[9].name,  NULL},
    {WI_STRINGPTR,    2, 13, "%10s",  &eventInfo[10].name, NULL},
    {WI_STRINGPTR,    2, 14, "%10s",  &eventInfo[11].name, NULL},
    {WI_STRINGPTR,    2, 15, "%10s",  &eventInfo[12].name, NULL},
    {WI_STRINGPTR,    2, 16, "%10s",  &eventInfo[13].name, NULL},
    {WI_STRINGPTR,    2, 17, "%10s",  &eventInfo[14].name, NULL},
    {WI_STRINGPTR,    2, 18, "%10s",  &eventInfo[15].name, NULL},
    {WI_STRINGPTR,    2, 19, "%10s",  &eventInfo[16].name, NULL},
    {WI_STRINGPTR,    2, 20, "%10s",  &eventInfo[17].name, NULL},
    {WI_STRINGPTR,    2, 21, "%10s",  &eventInfo[18].name, NULL},
    {WI_STRINGPTR,    2, 22, "%10s",  &eventInfo[19].name, NULL},
    {WI_STRINGPTR,    2, 23, "%10s",  &eventInfo[20].name, NULL},
    {WI_STRINGTABLE, 13,  3, "%s",    &eventInfo[0],       &poolStatusNames},
    {WI_STRINGTABLE, 13,  4, "%s",    &eventInfo[1],       &poolStatusNames},
    {WI_STRINGTABLE, 13,  5, "%s",    &eventInfo[2],       &poolStatusNames},
    {WI_STRINGTABLE, 13,  6, "%s",    &eventInfo[3],       &poolStatusNames},
    {WI_STRINGTABLE, 13,  7, "%s",    &eventInfo[4],       &poolStatusNames},
    {WI_STRINGTABLE, 13,  8, "%s",    &eventInfo[5],       &poolStatusNames},
    {WI_STRINGTABLE, 13,  9, "%s",    &eventInfo[6],       &poolStatusNames},
    {WI_STRINGTABLE, 13, 10, "%s",    &eventInfo[7],       &poolStatusNames},
    {WI_STRINGTABLE, 13, 11, "%s",    &eventInfo[8],       &poolStatusNames},
    {WI_STRINGTABLE, 13, 12, "%s",    &eventInfo[9],       &poolStatusNames},
    {WI_STRINGTABLE, 13, 13, "%s",    &eventInfo[10],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 14, "%s",    &eventInfo[11],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 15, "%s",    &eventInfo[12],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 16, "%s",    &eventInfo[13],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 17, "%s",    &eventInfo[14],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 18, "%s",    &eventInfo[15],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 19, "%s",    &eventInfo[16],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 20, "%s",    &eventInfo[17],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 21, "%s",    &eventInfo[18],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 22, "%s",    &eventInfo[19],      &poolStatusNames},
    {WI_STRINGTABLE, 13, 23, "%s",    &eventInfo[20],      &poolStatusNames},
    {WI_END,          0,  0, NULL,    NULL,                NULL},
};

static struct WindowDesc eventWindow = {0, 11, 27, 26, eventWindowItems, 0, 0, 0, 0, 0, 0};

/* Camera debug window */

static struct RangeInt cameraIdRange = {0, 0, 3, 1};

static struct WindowItem cameraWindowItems[] =
{
    {WI_LABEL,      1,  1, "Camera",         NULL,                    NULL},
    {WI_EDIT_INDEX, 2,  3, "id:       [%d]", NULL,                    &cameraIdRange},
    {WI_F32,        2, -1, "camx:%8.3f",     &cameraInfo[0].eye.x,    NULL},
    {WI_F32,        2, -1, "camy:%8.3f",     &cameraInfo[0].eye.y,    NULL},
    {WI_F32,        2, -1, "camz:%8.3f",     &cameraInfo[0].eye.z,    NULL},
    {WI_F32,        2, -1, "intx:%8.3f",     &cameraInfo[0].lookAt.x, NULL},
    {WI_F32,        2, -1, "inty:%8.3f",     &cameraInfo[0].lookAt.y, NULL},
    {WI_F32,        2, -1, "intz:%8.3f",     &cameraInfo[0].lookAt.z, NULL},
    {WI_U16,        2, -1, "angx:    %04hX", &cameraInfo[0].rotX,     NULL},
    {WI_U16,        2, -1, "angy:    %04hX", &cameraInfo[0].rotY,     NULL},
    {WI_U16,        2, -1, "angz:    %04hX", &cameraInfo[0].rotZ,     NULL},
    {WI_END,        0,  0, NULL,             NULL,                    NULL},
};

static struct WindowDesc cameraWindow = {36, 22, 17, 15, cameraWindowItems, sizeof(struct Camera), 0, 0, 0, 0, 0};

/* Effect debug window */

static struct WindowItem effectWindowItems[] =
{
    {WI_LABEL, 1, 1, "Effect", NULL, NULL},
    {WI_END,   0, 0, NULL,     NULL, NULL},
};

static struct WindowDesc effectWindow = {0, 0, 10, 10, effectWindowItems, 0, 0, 0, 0, 0, 0};

/* Sprite debug window */

static struct RangeFloat spritePosRange = { RANGE_NOMIN|RANGE_NOMAX, 0.0f, 640.0f, 1.0f };
static struct RangeInt   spriteIdRange = { 0, 0, MAX_SPRITES-1, 1 };
static struct RangeInt   fontIdRange = { 0, 0, 178, 1 };
static struct RangeInt   alignRange = { 0, 0, 8, 1 };
static struct RangeInt   spriteColorRange = { 0, 0, 255, 1 };
static struct RangeFloat spriteOpacityRange = { 0, 0.0f, 1.0f, 0.01f };

static struct WindowItem spriteWindowItems[] =
{
    {WI_LABEL,       1,  1, "Sprite",            NULL,                     NULL},
    {WI_EDIT_INDEX,  3,  3, "Sprite ID : %3d",   NULL,                     &spriteIdRange},
    {WI_POOLSTATE,   3, -1, "%15s",              &g_poolInfo.spriteBuf,    NULL},
    {WI_STRING,      3, -2, "%s",                &spriteWork[0].text,      NULL},
    {WI_EDIT_F32,    3, -1, "locate_dx : %4.1f", &spriteWork[0].x,         &spritePosRange},
    {WI_EDIT_F32,    3, -1, "locate_dy : %4.1f", &spriteWork[0].y,         &spritePosRange},
    {WI_EDIT_F32,    3, -1, "    depth : %4.1f", &spriteWork[0].depth,     &posScaleRange},
    {WI_EDIT_U8,     3, -1, "     font : %3d",   &spriteWork[0].fontId,    &fontIdRange},
    {WI_STRINGTABLE, 3, -1, "  %s",              &spriteWork[0].fontId,    &fontStrArray},
    {WI_EDIT_S8,     3, -1, "    align : %3d",   &spriteWork[0].textAlign, &alignRange},
    {WI_EDIT_U8,     3, -1, "      red :  %02X", &spriteWork[0].mulR,      &spriteColorRange},
    {WI_EDIT_U8,     3, -1, "    green :  %02X", &spriteWork[0].mulG,      &spriteColorRange},
    {WI_EDIT_U8,     3, -1, "     bule :  %02X", &spriteWork[0].mulB,      &spriteColorRange},
    {WI_EDIT_U8,     3, -1, "ofs   red :  %02X", &spriteWork[0].addR,      &spriteColorRange},
    {WI_EDIT_U8,     3, -1, "ofs green :  %02X", &spriteWork[0].addG,      &spriteColorRange},
    {WI_EDIT_U8,     3, -1, "ofs  bule :  %02X", &spriteWork[0].addB,      &spriteColorRange},
    {WI_EDIT_S16,    3, -1, "     zang : %04hX", &spriteWork[0].rotation,  &angleRange},
    {WI_EDIT_F32,    3, -1, "     zm_x : %4.1f", &spriteWork[0].scaleX,    &posScaleRange},
    {WI_EDIT_F32,    3, -1, "     zm_y : %4.1f", &spriteWork[0].scaleY,    &posScaleRange},
    {WI_EDIT_F32,    3, -1, "    trnsl : %4.2f", &spriteWork[0].opacity,   &spriteOpacityRange},
    {WI_S16,         3, -2, "    timer : %5d",   &spriteWork[0].counter,   NULL},
    {WI_S32,         3, -1, "    para1 : %5d",   &spriteWork[0].userVar,   NULL},
    {WI_END,         0,  0, NULL,                NULL,                     NULL},
};

static struct WindowDesc spriteWindow = {0, 0, 24, 30, spriteWindowItems, sizeof(struct Sprite), 0, 0, 0, 0, 0};

/* Light Param debug window */

static char *spotFnStrTable[] =
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
static struct RangeFloat lbl_801B5098 = { RANGE_NOMAX, 0.1f, 1.0f, 0.1f };
static struct RangeFloat lbl_801B50A8 = { RANGE_NOMIN|RANGE_NOMAX, 0.0f, 1.0f, 0.1f };
static struct RangeFloat lbl_801B50B8 = { RANGE_NOMIN|RANGE_NOMAX, 0.0f, 1.0f, 0.01f };
static struct RangeFloat lbl_801B50C8 = { RANGE_NOMIN|RANGE_NOMAX, 0.0f, 1.0f, 0.01f };
static struct RangeInt   lbl_801B50D8 = { 0, 0, 6, 1 };
static struct RangeFloat lbl_801B50E8 = { 0, 0.0f, 90.0f, 0.1f };
static struct RangeInt   lbl_801B50F8 = { 0, 1, 199, 1 };

static struct WindowItem lightParamWindowItems[] =
{
    {WI_LABEL,        1,  1, "Light Param",  NULL,                     NULL},
    {WI_EDIT_S32,     2,  3, "data ID : %d", &u_lightToPrint,          &lbl_801B5058},
    {WI_EDIT_S8,      2, -2, "stat : ",      &lbl_801F3A08,            &lbl_801B3B78},
    {WI_STRINGTABLE,  9,  0, "%s",           &lbl_801F3A08,            &offOnStrTable},
    {WI_EDIT_S8,      2, -1, "name : ",      &lbl_801F3A08.u_id,       &lbl_801B5068},
    {WI_STRINGTABLE,  9,  0, "%s",           &lbl_801F3A08.u_id,       &s_lightIdNames},
    {WI_EDIT_S16,     2, -1, "sub  : %d",    &lbl_801F3A08.u_inst,     &lbl_801B3B18},
    {WI_EDIT_S8,      2, -1, "type : ",      &lbl_801F3A08.type,       &lbl_801B5078},
    {WI_STRINGTABLE,  9,  0, "%s",           &lbl_801F3A08.type,       &lightTypeNames},
    {WI_LABEL,        2, -2, "RGB",          NULL,                     NULL},
    {WI_EDIT_F32,     3, -1, "%4.2f",        &lbl_801F3A08.red,        &lbl_801B5088},
    {WI_EDIT_F32,     8,  0, "%4.2f",        &lbl_801F3A08.green,      &lbl_801B5088},
    {WI_EDIT_F32,    13,  0, "%4.2f",        &lbl_801F3A08.blue,       &lbl_801B5088},
    {WI_EDIT_F32,     2, -2, "px:%f",        &lbl_801F3A08.pos.x,      &posScaleRange},
    {WI_EDIT_F32,     2, -1, "py:%f",        &lbl_801F3A08.pos.y,      &posScaleRange},
    {WI_EDIT_F32,     2, -1, "pz:%f",        &lbl_801F3A08.pos.z,      &posScaleRange},
    {WI_EDIT_S16,     2, -2, "xa:%04X",      &lbl_801F3A08.rotX,       &angleRange},
    {WI_EDIT_S16,     2, -1, "ya:%04X",      &lbl_801F3A08.rotY,       &angleRange},
    {WI_EDIT_F32,     2, -2, "leng:%5.1f",   &lbl_801F3A08.refDist,    &lbl_801B5098},
    {WI_EDIT_F32,     2, -1, "att0:%5.2f",   &lbl_801F3A08.k0,         &lbl_801B50A8},
    {WI_EDIT_F32,     2, -1, "att1:%5.2f",   &lbl_801F3A08.k1,         &lbl_801B50B8},
    {WI_EDIT_F32,     2, -1, "att2:%5.2f",   &lbl_801F3A08.k2,         &lbl_801B50C8},
    {WI_EDIT_S8,      2, -2, "cone  :",      &lbl_802F1E50,            &lbl_801B50D8},
    {WI_STRINGTABLE,  9,  0, "%s",           &lbl_802F1E50,            &spotFnStrTable},
    {WI_EDIT_F32,     2, -1, "cutoff:%4.1f", &lbl_801F3A08.spotCutoff, &lbl_801B50E8},
    {WI_EDIT_S8,      2, -2, "DISP:",        &lbl_802F1C8C,            &lbl_801B3B78},
    {WI_STRINGTABLE,  7,  0, "%s",           &lbl_802F1C8C,            &offOnStrTable},
    {WI_EDIT_S8,      2, -1, "\"SAVE\"",     &u_printLight,            &lbl_801B3B78},
    {WI_EDIT_S8,      2, -1, "\"COPY\"",     &lbl_802F1C84,            &lbl_801B3B78},
    {WI_EDIT_S32,     9,  0, "%2d / ",       &lbl_802F1C80,            &lbl_801B5058},
    {WI_EDIT_S32,    14,  0, "st%03d",       &lightingStageId,         &lbl_801B50F8},
    {WI_END,          0,  0, NULL,           NULL,                     },
};

static struct WindowDesc lightParamWindow = {0, 0, 21, 33, lightParamWindowItems, 0, 0, 0, 0, 0, 0};

/* Light Group debug window */

static struct RangeInt lbl_801B549C = { 0, 0, 21, 1 };
static struct RangeInt lbl_801B54AC = { 0, -1, 31, 1 };

static struct WindowItem lightGroupWindowItems[] =
{
    {WI_LABEL,        1,  1, "Light Group",   NULL, 0},
    {WI_EDIT_S8,      2,  3, "group ID : %d", &lbl_802F1C75,               &lbl_801B549C},
    {WI_STRINGTABLE,  3, -1, "%s",            &lbl_802F1C75,               &s_lightGroupNames},
    {WI_EDIT_S8,      3, -2, "edit : ",       &s_u_lightGroupsInitialized, &lbl_801B3B78},
    {WI_STRINGTABLE, 10,  0, "%s",            &s_u_lightGroupsInitialized, &offOnStrTable},
    {WI_EDIT_S8,      3, -2, "%2d",           &lbl_802F1C6C[0],            &lbl_801B54AC},
    {WI_EDIT_S8,     -3,  0, "%2d",           &lbl_802F1C6C[1],            &lbl_801B54AC},
    {WI_EDIT_S8,     -3,  0, "%2d",           &lbl_802F1C6C[2],            &lbl_801B54AC},
    {WI_EDIT_S8,     -3,  0, "%2d",           &lbl_802F1C6C[3],            &lbl_801B54AC},
    {WI_EDIT_S8,      3, -1, "%2d",           &lbl_802F1C6C[4],            &lbl_801B54AC},
    {WI_EDIT_S8,     -3,  0, "%2d",           &lbl_802F1C6C[5],            &lbl_801B54AC},
    {WI_EDIT_S8,     -3,  0, "%2d",           &lbl_802F1C6C[6],            &lbl_801B54AC},
    {WI_EDIT_S8,     -3,  0, "%2d",           &lbl_802F1C6C[7],            &lbl_801B54AC},
    {WI_EDIT_F32,     3, -2, "amb r: %4.2f",  &lbl_801F39FC.r,             &lbl_801B5088},
    {WI_EDIT_F32,     0, -1, "amb g: %4.2f",  &lbl_801F39FC.g,             &lbl_801B5088},
    {WI_EDIT_F32,     0, -1, "amb b: %4.2f",  &lbl_801F39FC.b,             &lbl_801B5088},
    {WI_S32,          2, -2, "CalcLgtGrp:%d", &lbl_802F1C68,               NULL},
    {WI_S32,          2, -1, "    RegSet:%d", &lbl_802F1C64,               NULL},
    {WI_S32,          2, -1, "SetLgtGrp :%d", &lbl_802F1C60,               NULL},
    {WI_S32,          2, -1, "    RegSet:%d", &s_numLightObjsLoaded,       NULL},
    {WI_END,          0,  0, 0,               NULL,                        NULL},
};

static struct WindowDesc lightGroupWindow = { 36, 0, 17, 16, lightGroupWindowItems, 0, 0, 0, 0, 0, 0 };

/* Fog debug window */

static char *fogTypeStrTable[] =
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
static struct RangeFloat lbl_801B57E8 = { RANGE_NOMIN|RANGE_NOMAX, -10.0f, 10.0f, 1.0f };
static struct RangeInt   lbl_801B57F8 = { 0, 0, 255, 1 };

static struct WindowItem fogWindowItems[] =
{
    {WI_LABEL,       1,  1, "Fog",           NULL,                NULL},
    {WI_EDIT_S8,     2, -2, "sw : ",         &fogInfo.enabled, &lbl_801B3B78},
    {WI_STRINGTABLE, 7,  0, "%s",            &fogInfo.enabled, &offOnStrTable},
    {WI_EDIT_S8,     2, -2, "type : ",       &fogInfo.unk0,    &lbl_801B57D8},
    {WI_STRINGTABLE, 9,  0, "%s",            &fogInfo.unk0,    &fogTypeStrTable},
    {WI_EDIT_F32,    2, -2, "start : %5.1f", &fogInfo.unk4,    &lbl_801B57E8},
    {WI_EDIT_F32,    2, -1, "end   : %5.1f", &fogInfo.unk8,    &lbl_801B57E8},
    {WI_EDIT_U8,     2, -2, "color r :%3d",  &fogInfo.r,       &lbl_801B57F8},
    {WI_EDIT_U8,     2, -1, "color g :%3d",  &fogInfo.g,       &lbl_801B57F8},
    {WI_EDIT_U8,     2, -1, "color b :%3d",  &fogInfo.b,       &lbl_801B57F8},
    {WI_END,         0,  0, NULL,            NULL,             NULL},
};

static struct WindowDesc fogWindow = { 0, 0, 28, 16, fogWindowItems, 0, 0, 0, 0, 0, 0 };

/* Sound debug window */

static struct RangeInt lbl_801B598C = { 0, 0, 100, 5 };
static struct RangeInt lbl_801B599C = { 0, 0, 16, 1 };
static struct RangeInt lbl_801B59AC = { 0, 0, 1061, 1 };
static struct RangeInt lbl_801B59BC = { 0, -128, 127, 1 };
static struct RangeInt lbl_801B59CC = { RANGE_NOMIN|RANGE_NOMAX, 0, 100, 1 };

static struct WindowItem soundWindowItems[] =
{
    {WI_LABEL,      1,  1, "Sound",                NULL,                     NULL},
    {WI_U32,        2, -2, " RAM:%08X",            &g_soundTotalBytesLoaded, NULL},
    {WI_U32,        2, -1, "ARAM:%08X",            &g_soundAramTop,          NULL},
    {WI_EDIT_U8,    2, -2, "SE  vol : %3d",        &g_soundSEVol,            &lbl_801B598C},
    {WI_EDIT_U8,    2, -1, "BGM vol : %3d",        &g_soundBGMVol,           &lbl_801B598C},
    {WI_LABEL,      2, -2, "VOL OFS",              NULL,                     NULL},
    {WI_EDIT_S32,   4, -1, "GRP : ",               &u_someSoundGroupId,      &lbl_801B599C},
    {WI_STRINGPTR, 10,  0, "%s",                   &u_someSoundGroupName,    NULL},
    {WI_EDIT_S32,   4, -1, "ID  : ",               &u_someSoundId,           &lbl_801B59AC},
    {WI_STRINGPTR, 10,  0, "%s",                   &u_someSoundName,         NULL},
    {WI_EDIT_S8,    4, -1, "ofs : %d",             &lbl_802F1E51,            &lbl_801B59BC},
    {WI_EDIT_S32,   4, -1, "REQ",                  &lbl_802F1E54,            &lbl_801B3B78},
    {WI_EDIT_S32,   4, -1, "OFF",                  &lbl_802F1E58,            &lbl_801B3B78},
    {WI_EDIT_S32,   2, -2, "stream loop ofs : %d", &lbl_802F1DD0,            &lbl_801B59CC},
    {WI_EDIT_S32,   2, -1, "stream vol ofs  : %d", &lbl_802F1DCC,            &lbl_801B59CC},
    {WI_END,        0,  0, NULL,                   NULL,                     NULL},
};

static struct WindowDesc soundWindow = { 0, 0, 35, 20, soundWindowItems, 0, 0, 0, 0, 0, 0 };

/* Monkey Fight debug window */

static struct WindowItem fightWindowItems[] =
{
    {WI_LABEL,    1,  1, "Monkey Fight",           NULL,          NULL},
    {WI_EDIT_U16, 2,  3, "CAM ANG X : 0x%04X",     &lbl_802F1EA2, &angleRange},
    {WI_EDIT_U16, 2,  4, "    ANG Y : 0x%04X",     &lbl_802F1EA0, &angleRange},
    {WI_EDIT_F32, 2,  5, "BALL POWER: %4.2f",      &lbl_802F1E9C, &posScaleRange},
    {WI_EDIT_S32, 2,  7, "BLT INTERVAL   : %4d",   &lbl_802F1E98, &lbl_801B3B18},
    {WI_EDIT_S32, 2,  8, "BLT TRIGER MODE: %4d",   &lbl_802F1E94, &lbl_801B3B18},
    {WI_EDIT_S32, 2,  9, "BLT SWING MODE : %4d",   &lbl_802F1E90, &lbl_801B3B18},
    {WI_EDIT_S32, 2, 10, "BLT TIME       : %4d",   &lbl_802F1E8C, &lbl_801B3B18},
    {WI_EDIT_F32, 2, 11, "BLT SPD        : %4.2f", &lbl_802F1E88, &posScaleRange},
    {WI_EDIT_F32, 2, 12, "BLT POWER      : %4.3f", &lbl_802F1E84, &lbl_801B3B38},
    {WI_EDIT_F32, 2, 13, "BLT REACT POWER: %4.3f", &lbl_802F1E80, &lbl_801B3B38},
    {WI_EDIT_F32, 2, 14, "BLT COLI RAD   : %4.2f", &lbl_802F1E7C, &posScaleRange},
    {WI_EDIT_F32, 2, 15, "        LV COEF: %4.2f", &lbl_802F1E78, &posScaleRange},
    {WI_EDIT_F32, 2, 16, "BLT RANGE      : %4.2f", &lbl_802F1E74, &posScaleRange},
    {WI_EDIT_F32, 2, 17, "        LV COEF: %4.2f", &lbl_802F1E70, &posScaleRange},
    {WI_EDIT_F32, 2, 18, "BLT BALL BREAK : %4.2f", &lbl_802F1E6C, &posScaleRange},
    {WI_EDIT_S16, 2, 20, "KILL TOP POINT : %4d",   &lbl_802F1E6A, &lbl_801B3B18},
    {WI_EDIT_S16, 2, 21, "KILL POINT     : %4d",   &lbl_802F1E68, &lbl_801B3B18},
    {WI_EDIT_S16, 2, 22, "FALL POINT     : %4d",   &lbl_802F1E66, &lbl_801B3B18},
    {WI_EDIT_S16, 2, 23, "PUNCH HIT POINT: %4d",   &lbl_802F1E64, &lbl_801B3B18},
    {WI_EDIT_F32, 2, 25, "KILL DECIDE SEC: %4.2f", &lbl_802F1E60, &posScaleRange},
    {WI_EDIT_F32, 2, 27, "MUTEKI SEC     : %4.2f", &lbl_802F1E5C, &posScaleRange},
    {WI_END,      0,  0, NULL,                     NULL,          NULL},
};

static struct WindowDesc fightWindow = { 24, 0, 32, 36, fightWindowItems, 0, 0, 0, 0, 0, 0 };

/* Unused Mini Race CPU debug window */

static struct RangeInt unusedRange1 = { 0, 0, 255, 1 };

static struct WindowItem raceCpuWindowItems[] =
{
    {WI_LABEL, 1,  1, "MINI RACE CPU", NULL, NULL},
    {WI_END,   0,  0, NULL,            NULL, NULL},
};

static struct WindowDesc raceCpuWindow = { 28, 25, 24, 11, raceCpuWindowItems, 0x14, 0, 0, 0, 0, 0 };

/* Monkey Race debug window */

static struct RangeInt   unusedRange2 = { RANGE_NOMIN|RANGE_NOMAX, 0, 100, 128 };
static struct RangeFloat unusedRange3 = { RANGE_NOMIN|RANGE_NOMAX, -10.0f, 10.0f, 0.02f };

static struct WindowItem raceWindowItems[] =
{
    {WI_LABEL, 1, 1, "Monkey Race", NULL, NULL},
    {WI_END,   0, 0, NULL,          NULL, NULL},
};

static struct WindowDesc raceWindow = { 23, 19, 30, 17, raceWindowItems, 0, 0, 0, 0, 0, 0 };

/* Performance debug window */

static struct WindowItem perfWindowItems[] =
{
    {WI_LABEL,      1,  1, "Performance", NULL,                   NULL},
    {WI_LABEL,      3,  3, "MAIN LOOP",   NULL,                   NULL},
    {WI_STRING,     3,  5, "%13s",        "PRE_PROCESS",          NULL},
    {WI_STRING,     3,  6, "%13s",        "SOUND_SYSTEM",         NULL},
    {WI_STRING,     3,  7, "%13s",        "INPUT",                NULL},
    {WI_STRING,     3,  8, "%13s",        "DEBUG_CONTROL",        NULL},
    {WI_STRING,     3,  9, "%13s",        "LOAD_MAIN",            NULL},
    {WI_STRING,     3, 10, "%13s",        "MODE",                 NULL},
    {WI_STRING,     3, 11, "%13s",        "CHECK_STATUS",         NULL},
    {WI_STRING,     3, 12, "%13s",        "EVENT",                NULL},
    {WI_STRING,     3, 13, "%13s",        "POLY_DISP",            NULL},
    {WI_STRING,     3, 14, "%13s",        "BITMAP_DISP",          NULL},
    {WI_STRING,     3, 15, "%13s",        "WINDOW_MAIN",          NULL},
    {WI_STRING,     3, 16, "%13s",        "EPI_PROCESS",          NULL},
    {WI_STRING,     3, 17, "%13s",        "SYNC_WAIT",            NULL},
    {WI_STRING,     3, 18, "%13s",        "SHADOW_ERASE",         NULL},
    {WI_S32,       17,  5, "%5d",         &perfInfo.unk0,         NULL},
    {WI_S32,       17,  6, "%5d",         &perfInfo.soundTime,    NULL},
    {WI_S32,       17,  7, "%5d",         &perfInfo.inputTime,    NULL},
    {WI_S32,       17,  8, "%5d",         &perfInfo.debugTime,    NULL},
    {WI_S32,       17,  9, "%5d",         &perfInfo.loadTime,     NULL},
    {WI_S32,       17, 10, "%5d",         &perfInfo.unk14,        NULL},
    {WI_S32,       17, 11, "%5d",         &perfInfo.unk18,        NULL},
    {WI_S32,       17, 12, "%5d",         &perfInfo.eventTime,    NULL},
    {WI_S32,       17, 13, "%5d",         &perfInfo.polydispTime, NULL},
    {WI_S32,       17, 14, "%5d",         &perfInfo.bitmapTime,   NULL},
    {WI_S32,       17, 15, "%5d",         &perfInfo.windowTime,   NULL},
    {WI_S32,       17, 16, "%5d",         &perfInfo.gpwaitTime,   NULL},
    {WI_S32,       17, 17, "%5d",         &perfInfo.unk30,        NULL},
    {WI_S32,       17, 18, "%5d",         &perfInfo.unk34,        NULL},
    {WI_LABEL,     24,  3, "EVENT",       NULL,                   NULL},
    {WI_STRINGPTR, 24,  5, "%10s",        &eventInfo[0].name,     NULL},
    {WI_STRINGPTR, 24,  6, "%10s",        &eventInfo[1].name,     NULL},
    {WI_STRINGPTR, 24,  7, "%10s",        &eventInfo[2].name,     NULL},
    {WI_STRINGPTR, 24,  8, "%10s",        &eventInfo[3].name,     NULL},
    {WI_STRINGPTR, 24,  9, "%10s",        &eventInfo[4].name,     NULL},
    {WI_STRINGPTR, 24, 10, "%10s",        &eventInfo[5].name,     NULL},
    {WI_STRINGPTR, 24, 11, "%10s",        &eventInfo[6].name,     NULL},
    {WI_STRINGPTR, 24, 12, "%10s",        &eventInfo[7].name,     NULL},
    {WI_STRINGPTR, 24, 13, "%10s",        &eventInfo[8].name,     NULL},
    {WI_STRINGPTR, 24, 14, "%10s",        &eventInfo[9].name,     NULL},
    {WI_STRINGPTR, 24, 15, "%10s",        &eventInfo[10].name,    NULL},
    {WI_STRINGPTR, 24, 16, "%10s",        &eventInfo[11].name,    NULL},
    {WI_STRINGPTR, 24, 17, "%10s",        &eventInfo[12].name,    NULL},
    {WI_STRINGPTR, 24, 18, "%10s",        &eventInfo[13].name,    NULL},
    {WI_STRINGPTR, 24, 19, "%10s",        &eventInfo[14].name,    NULL},
    {WI_STRINGPTR, 24, 20, "%10s",        &eventInfo[15].name,    NULL},
    {WI_STRINGPTR, 24, 21, "%10s",        &eventInfo[16].name,    NULL},
    {WI_STRINGPTR, 24, 22, "%10s",        &eventInfo[17].name,    NULL},
    {WI_STRINGPTR, 24, 23, "%10s",        &eventInfo[18].name,    NULL},
    {WI_STRINGPTR, 24, 24, "%10s",        &eventInfo[19].name,    NULL},
    {WI_STRINGPTR, 24, 25, "%10s",        &eventInfo[20].name,    NULL},
    {WI_S32,       35,  5, "%5d",         &eventInfo[0].time,     NULL},
    {WI_S32,       35,  6, "%5d",         &eventInfo[1].time,     NULL},
    {WI_S32,       35,  7, "%5d",         &eventInfo[2].time,     NULL},
    {WI_S32,       35,  8, "%5d",         &eventInfo[3].time,     NULL},
    {WI_S32,       35,  9, "%5d",         &eventInfo[4].time,     NULL},
    {WI_S32,       35, 10, "%5d",         &eventInfo[5].time,     NULL},
    {WI_S32,       35, 11, "%5d",         &eventInfo[6].time,     NULL},
    {WI_S32,       35, 12, "%5d",         &eventInfo[7].time,     NULL},
    {WI_S32,       35, 13, "%5d",         &eventInfo[8].time,     NULL},
    {WI_S32,       35, 14, "%5d",         &eventInfo[9].time,     NULL},
    {WI_S32,       35, 15, "%5d",         &eventInfo[10].time,    NULL},
    {WI_S32,       35, 16, "%5d",         &eventInfo[11].time,    NULL},
    {WI_S32,       35, 17, "%5d",         &eventInfo[12].time,    NULL},
    {WI_S32,       35, 18, "%5d",         &eventInfo[13].time,    NULL},
    {WI_S32,       35, 19, "%5d",         &eventInfo[14].time,    NULL},
    {WI_S32,       35, 20, "%5d",         &eventInfo[15].time,    NULL},
    {WI_S32,       35, 21, "%5d",         &eventInfo[16].time,    NULL},
    {WI_S32,       35, 22, "%5d",         &eventInfo[17].time,    NULL},
    {WI_S32,       35, 23, "%5d",         &eventInfo[18].time,    NULL},
    {WI_S32,       35, 24, "%5d",         &eventInfo[19].time,    NULL},
    {WI_S32,       35, 25, "%5d",         &eventInfo[20].time,    NULL},
    {WI_END,        0,  0, NULL,          NULL,                   NULL},
};

static struct WindowDesc perfWindow = { 5, 7, 43, 29, perfWindowItems, 0, 0, 0, 0, 0, 0 };

/* Input Test debug window */

static struct RangeInt lbl_801B6900 = { 0, 0, 3, 1 };

static struct WindowItem inputWindowItems[] =
{
    {WI_LABEL,      1,  1, "Input Test",         NULL,                                    NULL},
    {WI_EDIT_INDEX, 2,  3, "PAD           [%d]", NULL,                                    &lbl_801B6900},
    {WI_BIT16,      2, -2, "BUTTON LEFT   %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_LEFT},
    {WI_BIT16,      2, -1, "BUTTON RIGHT  %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_RIGHT},
    {WI_BIT16,      2, -1, "BUTTON DOWN   %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_DOWN},
    {WI_BIT16,      2, -1, "BUTTON UP     %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_UP},
    {WI_BIT16,      2, -1, "TRIGGER Z     %3s",  &controllerInfo[0].held.button,       (void *)PAD_TRIGGER_Z},
    {WI_BIT16,      2, -1, "TRIGGER R     %3s",  &controllerInfo[0].held.button,       (void *)PAD_TRIGGER_R},
    {WI_BIT16,      2, -1, "TRIGGER L     %3s",  &controllerInfo[0].held.button,       (void *)PAD_TRIGGER_L},
    {WI_BIT16,      2, -1, "BUTTON A      %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_A},
    {WI_BIT16,      2, -1, "BUTTON B      %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_B},
    {WI_BIT16,      2, -1, "BUTTON X      %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_X},
    {WI_BIT16,      2, -1, "BUTTON Y      %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_Y},
    {WI_BIT16,      2, -1, "BUTTON MENU   %3s",  &controllerInfo[0].held.button,       (void *)PAD_BUTTON_MENU},
    {WI_S8,         2, -2, "stickX       %4d",   &controllerInfo[0].held.stickX,       NULL},
    {WI_S8,         2, -1, "stickY       %4d",   &controllerInfo[0].held.stickY,       NULL},
    {WI_S8,         2, -1, "substickX    %4d",   &controllerInfo[0].held.substickX,    NULL},
    {WI_S8,         2, -1, "substickY    %4d",   &controllerInfo[0].held.substickY,    NULL},
    {WI_U8,         2, -1, "triggerLeft  %4d",   &controllerInfo[0].held.triggerLeft,  NULL},
    {WI_U8,         2, -1, "triggerRight %4d",   &controllerInfo[0].held.triggerRight, NULL},
    {WI_U8,         2, -1, "analogA      %4d",   &controllerInfo[0].held.analogA,      NULL},
    {WI_U8,         2, -1, "analogB      %4d",   &controllerInfo[0].held.analogB,      NULL},
    {WI_U8,         2, -1, "err          %4d",   &controllerInfo[0].held.err,          NULL},
    {WI_END,        0,  0, NULL,                 NULL,                                    NULL},
};

static struct WindowDesc inputWindow = { 0, 0, 21, 30, inputWindowItems, sizeof(struct ControllerInfo), 0, 0, 0, 0, 0 };

/* Window Info debug window */

static struct WindowItem wininfoWindowItems[] =
{
    {WI_LABEL, 1,  1, "Window Info",   NULL,                NULL},
    {WI_S32,   3,  3, "locateX : %3d", &currWindowX,      NULL},
    {WI_S32,   3,  4, "locateY : %3d", &currWindowY,      NULL},
    {WI_S32,   3,  5, "  sizeX : %3d", &currWindowWidth,  NULL},
    {WI_S32,   3,  6, "  sizeY : %3d", &currWindowHeight, NULL},
    {WI_END,   0,  0, NULL,            NULL,                NULL},
};

static struct WindowDesc wininfoWindow = { 34, 28, 19, 9, wininfoWindowItems, 0, 0, 0, 0, 0, 0 };

/* Change Param debug window */

static struct WindowItem changeParamWindowItems[] =
{
    {WI_LABEL,    1,  1, "Change Param", NULL,          NULL},
    {WI_EDIT_U16, 2,  3, "xang   %04hX", &lbl_802F1ED4, &angleRange},
    {WI_EDIT_U16, 2, -1, "yang   %04hX", &lbl_802F1ED2, &angleRange},
    {WI_EDIT_U16, 2, -1, "zang   %04hX", &lbl_802F1ED0, &angleRange},
    {WI_EDIT_F32, 2, -1, "xpos   %4.1f", &lbl_802F1ECC, &posScaleRange},
    {WI_EDIT_F32, 2, -1, "ypos   %4.1f", &lbl_802F1EC8, &posScaleRange},
    {WI_EDIT_F32, 2, -1, "zpos   %4.1f", &lbl_802F1EC4, &posScaleRange},
    {WI_EDIT_F32, 2, -1, "float  %4.1f", &lbl_802F1EC0, &posScaleRange},
    {WI_EDIT_F32, 2, -1, "float2 %4.1f", &lbl_802F1EBC, &posScaleRange},
    {WI_EDIT_F32, 2, -1, "float3 %4.1f", &lbl_802F1EB8, &posScaleRange},
    {WI_EDIT_S32, 2, -2, "xint   %4d",   &lbl_802F1EB4, &lbl_801B3B18},
    {WI_EDIT_S32, 2, -1, "yint   %4d",   &lbl_802F1EB0, &lbl_801B3B18},
    {WI_EDIT_S32, 2, -1, "zint   %4d",   &lbl_802F1EAC, &lbl_801B3B18},
    {WI_EDIT_U8,  2, -2, "char   %4o",   &lbl_802F1EA4, &lbl_801B3B18},
    {WI_S8,       2, -1, "          %c", &lbl_802F1EA4, NULL},
    {WI_END,      0,  0, NULL,           NULL,          NULL},
};

static struct WindowDesc changeParamWindow = { 0, 18, 15, 19, changeParamWindowItems, 0, 0, 0, 0, 0, 0 };

/* Bumper Param debug window */

static struct WindowItem bumperParamWindowItems[] =
{
    {WI_LABEL,    1, 1, "Bumper Param",        NULL,                  NULL},
    {WI_EDIT_F32, 2, 2, "LOD LEVEL[0]: %4.5f", &lbl_8028C0B0.unk0[0], &lbl_801B3B38},
    {WI_EDIT_F32, 2, 3, "LOD LEVEL[1]: %4.5f", &lbl_8028C0B0.unk0[1], &lbl_801B3B38},
    {WI_EDIT_F32, 2, 4, "LOD LEVEL[2]: %4.5f", &lbl_8028C0B0.unk0[2], &lbl_801B3B38},
    {WI_EDIT_F32, 2, 5, "LOD LEVEL[3]: %4.5f", &lbl_8028C0B0.unk0[3], &lbl_801B3B38},
    {WI_END,      0, 0, NULL,                  NULL,                  NULL},
};

static struct WindowDesc bumperParamWindow = { 4, 4, 32, 8, bumperParamWindowItems, 0, 0, 0, 0, 0, 0 };

/* Heap Free debug window */

static struct WindowItem heapFreeWindowItems[] =
{
    {WI_LABEL,    1, 1, "Heap Free", NULL, NULL},
    {WI_HEAPINFO, 2, 3, NULL,        NULL, NULL},
    {WI_END,      0, 0, NULL,        NULL, NULL},
};

static struct WindowDesc heapFreeWindow = { 0, 0, 38, 11, heapFreeWindowItems, 0, 0, 0, 0, 0, 0 };

/* Main Menu debug window */

static struct WindowItem mainMenuWindowItems[] =
{
    {WI_LABEL,     1,  1, "Main Menu",    NULL,               NULL},
    {WI_SUBWINDOW, 2, -2, "Dip Switch",   &dipSwitchWindow,   NULL},
    {WI_SUBWINDOW, 2, -1, "Game",         &gameWindow,        NULL},
    {WI_SUBWINDOW, 2, -1, "Mode",         &modeWindow,        NULL},
    {WI_SUBWINDOW, 2, -1, "Event",        &eventWindow,       NULL},
    {WI_SUBWINDOW, 2, -1, "Camera",       &cameraWindow,      NULL},
    {WI_SUBWINDOW, 2, -1, "Effect",       &effectWindow,      NULL},
    {WI_SUBWINDOW, 2, -1, "Sprite",       &spriteWindow,      NULL},
    {WI_SUBWINDOW, 2, -1, "Light Param",  &lightParamWindow,  NULL},
    {WI_SUBWINDOW, 2, -1, "Light Group",  &lightGroupWindow,  NULL},
    {WI_SUBWINDOW, 2, -1, "Fog",          &fogWindow,         NULL},
    {WI_SUBWINDOW, 2, -1, "Sound",        &soundWindow,       NULL},
    {WI_SUBWINDOW, 2, -1, "Bumper Param", &bumperParamWindow, NULL},
    {WI_SUBWINDOW, 2, -2, "Monkey Fight", &fightWindow,       NULL},
    {WI_SUBWINDOW, 2, -1, "Monkey Race",  &raceWindow,        NULL},
    {WI_SUBWINDOW, 2, -2, "Performance",  &perfWindow,        NULL},
    {WI_SUBWINDOW, 2, -1, "Heap Free",    &heapFreeWindow,    NULL},
    {WI_SUBWINDOW, 2, -1, "Input Test",   &inputWindow,       NULL},
    {WI_SUBWINDOW, 2, -2, "Change Param", &changeParamWindow, NULL},
    {WI_SUBWINDOW, 2, -1, "Window Info",  &wininfoWindow,     NULL},
    {WI_END,       0,  0, NULL,           NULL,               NULL},
};

static struct WindowDesc mainMenuWindow = { 4, 2, 20, 30, mainMenuWindowItems, 0, 0, 0, 0, 0, 0 };

static char screenBufferChar1[SCREEN_ROWS * SCREEN_COLUMNS];
static char screenBufferChar2[SCREEN_ROWS * SCREEN_COLUMNS];
static char screenBufferChar3[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBufferColor1[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBufferColor2[SCREEN_ROWS * SCREEN_COLUMNS];
static u8 screenBufferColor3[SCREEN_ROWS * SCREEN_COLUMNS];
static s8 screenBufferXOffset[SCREEN_ROWS * SCREEN_COLUMNS];
static s8 screenBufferYOffset[SCREEN_ROWS * SCREEN_COLUMNS];

static struct WindowDesc *windowList[16];
static struct WindowDesc windowWork[16];

static void draw_window_frame(struct WindowDesc *arg0);
static void window_set_text_offset(int arg0, int arg1);
static void clear_buffer_region(int arg0, int arg1, int arg2, int arg3);
static void draw_char(int x, int y, s8 charIdx, u8 colorId);
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

    // Resize window with the C stick
    if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_CSTICK_UP))
    {
        window->height--;
        window->height = MAX(window->height, 3);
    }
    else if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_CSTICK_DOWN))
    {
        window->height++;
        window->height = MIN(window->height, SCREEN_ROWS - window->y);
    }
    if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_CSTICK_LEFT))
    {
        window->width--;
        window->width = MAX(window->width, 3);
    }
    else if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_CSTICK_RIGHT))
    {
        window->width++;
        window->width = MIN(window->width, SCREEN_COLUMNS - window->x);
    }

    // Move window with the analog stick
    if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_STICK_UP))
    {
        temp_r5 = window->y - 1;
        var_r6 = MAX(temp_r5, 0);
        window->unk28 += (window->y - var_r6) * 12;
        window->y = var_r6;
    }
    else if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_STICK_DOWN))
    {
        temp_r5_2 = window->y;
        var_r6_2 = temp_r5_2 + 1;
        temp_r3_4 = SCREEN_ROWS - window->height;
        var_r6_2 = MIN(var_r6_2, temp_r3_4);
        window->unk28 += (temp_r5_2 - var_r6_2) * 12;
        window->y = var_r6_2;
    }
    if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_STICK_LEFT))
    {
        temp_r4 = window->x - 1;
        var_r6 = MAX(temp_r4, 0);
        window->unk24 += (window->x - var_r6) * 12;
        window->x = var_r6;
    }
    else if (REPEAT_ANALOG_WITH_R_ACCEL(0, ANALOG_STICK_RIGHT))
    {
        temp_r5_2 = window->x;
        var_r6_2 = temp_r5_2 + 1;
        temp_r3_4 = SCREEN_COLUMNS - window->width;
        var_r6_2 = MIN(var_r6_2, temp_r3_4);
        window->unk24 += (temp_r5_2 - var_r6_2) * 12;
        window->x = var_r6_2;
    }

    if (u_selectionRelated1)
    {
        temp_r5_4 = window->selection;
        var_r6_3 = temp_r5_4;
        do
        {
            if (--var_r6_3 < 0)
            {
                item = window->items;
                while ((u8)item->type != WI_END)
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
    else if (u_selectionRelated2)
    {
        var_r7 = temp_r6_3 = window->selection;
        do
        {
            var_r7++;
            if ((u8)window->items[var_r7].type == WI_END)
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
    case WI_SUBWINDOW:
        if (controllerInfo[0].repeat.button & PAD_BUTTON_A)
        {
            subWindow = item->pValue;
            if (subWindow != NULL)
            {
                lbl_802F1EA8 = 1;
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
    case WI_OPENWIN:
        if ((controllerInfo[0].repeat.button & PAD_BUTTON_A) && item->pValue != NULL)
            window_open(item->pValue);
        break;
    default:
        if ((controllerInfo[0].repeat.button & PAD_BUTTON_A)
         || (controllerInfo[0].repeat.button & PAD_BUTTON_B)
         || ((analogInputs[0].held & ANALOG_TRIGGER_RIGHT) && ((controllerInfo[0].held.button & PAD_BUTTON_A) || (controllerInfo[0].held.button & PAD_BUTTON_B))))
        {
            if (item->type & 0x200)
                var_r8 = 0;
            else
                var_r8 = window->structIndex;
            pValue = item->pValue;
            temp_r4_6 = (void *)((u8 *)pValue + (window->structSize * var_r8));
            switch ((u8)item->type)
            {
            case WI_EDIT_U32:
            case WI_EDIT_S32:
            case WI_EDIT_U16:
            case WI_EDIT_S16:
            case WI_EDIT_U8:
            case WI_EDIT_S8:
            case WI_EDIT_INDEX:
                switch ((u8)item->type)
                {
                case WI_EDIT_INDEX:
                    someintval = window->structIndex;
                    break;
                case WI_EDIT_U32:
                    someintval = *(u32 *)temp_r4_6;
                    break;
                case WI_EDIT_S32:
                    someintval = *(s32 *)temp_r4_6;
                    break;
                case WI_EDIT_U16:
                    someintval = *(u16 *)temp_r4_6;
                    break;
                case WI_EDIT_S16:
                    someintval = *(s16 *)temp_r4_6;
                    break;
                case WI_EDIT_U8:
                    someintval = *(u8 *)temp_r4_6;
                    break;
                default:
                case WI_EDIT_S8:
                    someintval = *(s8 *)temp_r4_6;
                    break;
                }
                someParam = item->param;
                rangeFlags = someParam->flags;
                minInt = someParam->min;
                maxInt = someParam->max;
                deltaInt = *(new_var = &someParam->delta);
                if ((controllerInfo[0].repeat.button & PAD_BUTTON_A) || ((analogInputs[0].held & ANALOG_TRIGGER_RIGHT) && (controllerInfo[0].held.button & PAD_BUTTON_A)))
                {
                    if (rangeFlags & RANGE_NOMAX)
                        someintval = someintval + deltaInt;
                    else if (deltaInt > 0)
                        someintval = MIN(someintval + deltaInt, (s32)maxInt);
                    else
                        someintval = MIN(someintval + deltaInt, (u32)maxInt);
                }
                else
                {
                    if (rangeFlags & RANGE_NOMIN)
                        someintval = someintval - deltaInt;
                    else if (deltaInt > 0)
                        someintval = MAX(someintval - deltaInt, (s32)minInt);
                    else
                        someintval = MAX(someintval - deltaInt, (u32)minInt);
                }
                switch ((u8)item->type)
                {
                case WI_EDIT_U32:
                case WI_EDIT_S32:
                    *(s32 *)temp_r4_6 = someintval;
                    break;
                case WI_EDIT_INDEX:
                    window->structIndex = (u32)someintval;
                    break;
                case WI_EDIT_U16:
                case WI_EDIT_S16:
                    *(s16 *)temp_r4_6 = someintval;
                    break;
                case WI_EDIT_U8:
                case WI_EDIT_S8:
                    *(s8 *)temp_r4_6 = someintval;
                    break;
                }
                break;
            case WI_EDIT_F32:
                someParamF = item->param;
                somefloatval = 1.0f;
                deltaFloat = someParamF->delta;
                deltaFloat = deltaFloat * somefloatval;
                somefloatval = *(float *)temp_r4_6;
                rangeFlags = someParamF->flags;
                minFloat = someParamF->min;
                maxFloat = someParamF->max;
                if ((controllerInfo[0].repeat.button & PAD_BUTTON_A) || ((analogInputs[0].held & ANALOG_TRIGGER_RIGHT) && (controllerInfo[0].held.button & PAD_BUTTON_A)))
                {
                    if (rangeFlags & RANGE_NOMAX)
                        somefloatval = somefloatval + deltaFloat;
                    else
                        somefloatval = MIN(somefloatval + deltaFloat, maxFloat);
                }
                else
                {
                    if (rangeFlags & RANGE_NOMIN)
                        somefloatval = somefloatval - deltaFloat;
                    else
                        somefloatval = MAX(somefloatval - deltaFloat, minFloat);
                }
                *(float *)temp_r4_6 = somefloatval;
                break;
            case WI_EDIT_BIT32:
                *pValue = (s32) (*pValue ^ (u32)item->param);
                break;
            case WI_SETWINITEMS:
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
    u_windowBoxX = a;
    u_windowBoxY = b;
    u_windowBoxWidth = c;
    u_windowBoxHeight = d;
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
        window_set_text_offset(window->unk24, window->unk28);
    else
        window_set_text_offset(0, 0);
    clear_buffer_region(window->x, window->y, window->width, window->height);
    set_some_params(window->x, window->y, window->width, window->height);
    if (arg1 != 0)
        window_set_text_color(WINDOW_COLOR_GREEN);
    else
        window_set_text_color(WINDOW_COLOR_WHITE);
    draw_window_frame(window);
    window_set_text_color(WINDOW_COLOR_WHITE);
    set_some_params(window->x + 1, window->y + 1, window->width - 2, window->height - 2);
    x = 0;
    y = 0;
    i = 0;
    item = window->items;
    while (item->type != WI_END)
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
        window_set_text_color(WINDOW_COLOR_WHITE);
        switch (item->type & 0xFF)
        {
        case WI_LABEL:
        case WI_INDEX:
            if (i == 0 && arg1 != 0)
                window_set_text_color(WINDOW_COLOR_GREEN);
            break;
        case WI_EDIT_U32:
        case WI_EDIT_S32:
        case WI_EDIT_U16:
        case WI_EDIT_S16:
        case WI_EDIT_U8:
        case WI_EDIT_S8:
        case WI_EDIT_F32:
        case WI_EDIT_BIT32:
        case WI_SETWINITEMS:
        case WI_EDIT_INDEX:
            if (arg1 != 0)
                window_set_text_color(WINDOW_COLOR_GREEN);
            break;
        }
        if (item->type & 0x200)
            var_r5 = 0;
        else
            var_r5 = window->structIndex;
        pValue = (u8 *)item->pValue + (window->structSize * var_r5);
        switch (item->type & 0xFF)
        {
        case WI_SUBWINDOW:
        case WI_SETWINITEMS:
        case WI_OPENWIN:
        case WI_LABEL:
            window_printf(2, item->format);
            break;
        case WI_EDIT_U32:
        case WI_U32:
            window_printf(2, item->format, *(int *)pValue);
            break;
        case WI_EDIT_S32:
        case WI_S32:
            window_printf(2, item->format, *(int *)pValue);
            break;
        case WI_EDIT_INDEX:
            window_printf(2, item->format, window->structIndex);
            break;
        case WI_EDIT_U16:
        case WI_U16:
            window_printf(2, item->format, *(u16 *)pValue);
            break;
        case WI_EDIT_S16:
        case WI_S16:
            window_printf(2, item->format, *(s16 *)pValue);
            break;
        case WI_EDIT_U8:
        case WI_U8:
            window_printf(2, item->format, *(u8 *)pValue);
            break;
        case WI_EDIT_S8:
        case WI_S8:
            window_printf(2, item->format, *(s8 *)pValue);
            break;
        case WI_EDIT_F32:
        case WI_F32:
            window_printf(2, item->format, *(f32 *)pValue);
            break;
        case WI_STRING:
            window_printf(2, item->format, (char *)pValue);
            break;
        case WI_STRINGPTR:
            window_printf(2, item->format, *(char **)pValue);
            break;
        case WI_EDIT_BIT32:
        case WI_BIT32:
            if (*(u32 *)pValue & (u32)item->param)
                onOff = " ON";
            else
                onOff = "OFF";
            window_printf(2, item->format, onOff);
            break;
        case WI_BIT16:
            if (*(u16 *)pValue & (u16)(u32)item->param)
                onOff = " ON";
            else
                onOff = "OFF";
            window_printf(2, item->format, onOff);
            break;
        case WI_INDEX:
            window_printf(2, item->format, window->structIndex);
            break;
        case WI_STRINGTABLE:
            window_printf(2, item->format, ((u32 *)item->param)[*(u8 *)pValue]);
            break;
        case WI_HEAPINFO:
            window_printf(2, "                 free       size\n");
            window_printf(2, "      MainHeap %08X / %08X\n", OSCheckHeap(mainHeap), mainHeapSize);
            window_printf(2, "       SubHeap %08X / %08X\n", OSCheckHeap(subHeap), subHeapSize);
            window_printf(2, "     StageHeap %08X / %08X\n", OSCheckHeap(stageHeap), stageHeapSize);
            window_printf(2, "BackgroundHeap %08X / %08X\n", OSCheckHeap(backgroundHeap), backgroundHeapSize);
            window_printf(2, "     CharaHeap %08X / %08X\n", OSCheckHeap(charaHeap), charaHeapSize);
            break;
        case WI_POOLSTATE:
            window_printf(2, item->format, poolStatusNames[((s8 *)item->pValue)[window->structIndex]]);
            break;
        }
        if (window->selection == i && arg1 != 0)
        {
            // draw selection arrow
            window_set_cursor_pos(x - 1, y);
            window_set_text_color(WINDOW_COLOR_RED);
            u_set_window_text(2, "\x1C");
        }
        item++;
        i++;
    }
    window_set_text_offset(0, 0);
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
    u8 *colorPtr;
    struct WindowDesc **var_r30;
    int col;
    int i;
    int row;
    char *charPtr;
    s8 *xoffPtr;
    s8 *yoffPtr;
    int temp_r0;
    int temp_r3;
    int temp_r5;
    int temp_r8;
    int var_r5;
    int var_r6;
    int var_r8;
    int var_r9;
    struct WindowDesc *temp_r7;

    GXSetZMode_cached(GX_ENABLE, GX_LESS, GX_ENABLE);
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
    GXSetZMode_cached(GX_ENABLE, GX_ALWAYS, GX_DISABLE);
    GXLoadTexObj_cached(&u_unkBitmapTPL->texObjs[31], GX_TEXMAP0);
    GXInitTexObjLOD(&u_unkBitmapTPL->texObjs[31], GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);

    charPtr = screenBufferChar3;
    colorPtr = screenBufferColor3;
    xoffPtr = screenBufferXOffset;
    yoffPtr = screenBufferYOffset;
    for (row = 0; row < SCREEN_ROWS; row++)
    {
        for (col = 0; col < SCREEN_COLUMNS; col++)
        {
            if (*charPtr != ' ')
            {
                draw_char(
                    *xoffPtr + (col * 12),
                    *yoffPtr + (row * 12),
                    *charPtr,
                    *colorPtr);
            }
            charPtr++;
            colorPtr++;
            xoffPtr++;
            yoffPtr++;
        }
    }
    GXSetZMode_cached(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
}

void window_init(void)
{
    s32 i;

    currWindowIndex = 0;
    lbl_802F1EA8 = 0;
    for (i = 0; i < 16; i++)
        windowWork[i].isActive = FALSE;
    memset(screenBufferChar1, ' ', sizeof(screenBufferChar1));
    memset(screenBufferChar2, ' ', sizeof(screenBufferChar2));
    memset(screenBufferChar3, ' ', sizeof(screenBufferChar3));
    memset(screenBufferColor1, 0, sizeof(screenBufferColor1));
    memset(screenBufferColor2, 0, sizeof(screenBufferColor2));
    memset(screenBufferColor3, 0, sizeof(screenBufferColor3));
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
    char *char1Ptr;
    int var_r3_7;
    char *var_r4;
    struct WindowDesc **var_r4_2;
    char *var_r5;
    u8 *var_r6;
    u8 *var_r7;
    char *char3Ptr;
    u8 *color1Ptr;
    char r11;
    u8 *color3Ptr;
    int var_ctr_4;
    struct WindowDesc *temp_r3;
    struct WindowDesc *temp_r3_7;

    var_r22 = 1;

    u_selectionRelated1 = ((controllerInfo[0].repeat.button & PAD_BUTTON_UP) || ((controllerInfo[0].held.button & PAD_BUTTON_UP) && (analogInputs[0].held & ANALOG_TRIGGER_RIGHT)));
    u_selectionRelated2 = ((controllerInfo[0].repeat.button & PAD_BUTTON_DOWN) || ((controllerInfo[0].held.button & PAD_BUTTON_DOWN) && (analogInputs[0].held & ANALOG_TRIGGER_RIGHT)));
    unused1 = ((controllerInfo[0].repeat.button & PAD_BUTTON_LEFT) || ((controllerInfo[0].held.button & PAD_BUTTON_LEFT) && (analogInputs[0].held & ANALOG_TRIGGER_RIGHT)));
    unused2 = ((controllerInfo[0].repeat.button & PAD_BUTTON_RIGHT) || ((controllerInfo[0].held.button & PAD_BUTTON_RIGHT) && (analogInputs[0].held & ANALOG_TRIGGER_RIGHT)));

    // clear buffers (not using memset for some reason)
    bufPtr = (u32 *)screenBufferXOffset;
    for (var_ctr = SCREEN_ROWS * SCREEN_COLUMNS / 4 - 3; var_ctr >= 0; var_ctr--)
        *bufPtr++ = 0;
    bufPtr = (u32 *)screenBufferYOffset;
    for (var_ctr = SCREEN_ROWS * SCREEN_COLUMNS / 4 - 3; var_ctr >= 0; var_ctr--)
        *bufPtr++ = 0;

    memcpy(screenBufferChar3, screenBufferChar2, sizeof(screenBufferChar3));
    memcpy(screenBufferColor3, screenBufferColor2, sizeof(screenBufferColor3));

    char1Ptr = screenBufferChar1;
    char3Ptr = screenBufferChar3;
    color1Ptr = screenBufferColor1;
    color3Ptr = screenBufferColor3;
    for (var_r3_7 = 0; var_r3_7 < SCREEN_ROWS; var_r3_7++)
    {
        var_r4 = char1Ptr;
        var_r5 = char3Ptr;
        var_r6 = color1Ptr;
        var_r7 = color3Ptr;
        for (var_ctr_3 = 0; var_ctr_3 < SCREEN_COLUMNS; var_ctr_3++)
        {
            r11 = *var_r4;
            if (r11 != ' ')
            {
                *var_r5 = r11;
                *var_r7 = *var_r6;
            }
            var_r4++;
            var_r5++;
            var_r6++;
            var_r7++;
        }
        char1Ptr += SCREEN_COLUMNS;
        char3Ptr += SCREEN_COLUMNS;
        color1Ptr += SCREEN_COLUMNS;
        color3Ptr += SCREEN_COLUMNS;
    }

    memset(screenBufferChar1, ' ', sizeof(screenBufferChar1));
    memset(screenBufferColor1, 0, sizeof(screenBufferColor1));

    if (analogInputs[0].held & ANALOG_TRIGGER_LEFT)
    {
        if ((dipSwitches & DIP_DEBUG) && (controllerInfo[0].pressed.button & PAD_BUTTON_START))
        {
            window_open(&mainMenuWindow);
        }
        else if (controllerInfo[0].pressed.button & PAD_BUTTON_Y)
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
        else if (controllerInfo[0].repeat.button & PAD_BUTTON_A)
        {
            func_8002DC54();
            var_r22 = 0;
        }
        else if ((controllerInfo[0].repeat.button & PAD_BUTTON_B) && lbl_802F1EA8 != 0)
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

    temp_r22 = u_someSoundId;
    spC = u_someSoundGroupId;
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

    if (g_soundDesc[u_someSoundId].unk8 == 1)
    {
        if (u_someSoundId == g_soundGroupDesc[u_someSoundGroupId].unused)
            u_someSoundId = g_soundGroupDesc[u_someSoundGroupId+1].unused - 1;
        else
            u_someSoundId = g_soundGroupDesc[u_someSoundGroupId].unused + 1;
    }
    if (spC != u_someSoundGroupId)
        u_someSoundId = g_soundGroupDesc[u_someSoundGroupId].unused + 1;
    if (lbl_802F1E54 != 0)
    {
        SoundReqDirect(u_someSoundId);
        lbl_802F1E54 = 0;
    }
    if (lbl_802F1E58 != 0)
    {
        SoundOff(u_someSoundId);
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
    window_set_text_color(WINDOW_COLOR_WHITE);
    if (lbl_802F1EA8 != 0U)
    {
        temp_r3_7 = windowList[currWindowIndex];
        if (temp_r3_7 != NULL)
        {
            currWindowX = temp_r3_7->x;
            currWindowY = temp_r3_7->y;
            currWindowWidth = temp_r3_7->width;
            currWindowHeight = temp_r3_7->height;
            return;
        }
    }
    currWindowX = 0;
    currWindowY = 0;
    currWindowWidth = 0;
    currWindowHeight = 0;
}

void window_set_cursor_pos(int x, int y)
{
    windowCursorX = x;
    windowCursorStartX = x;
    windowCursorY = y;
}

void window_move_cursor(int dx, int dy)
{
    windowCursorX += dx;
    windowCursorY += dy;
}

static void window_set_text_offset(int x, int y)
{
    textOffsetX = x;
    textOffsetY = y;
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
    char *charPtr;
    u8 *colorPtr;
    s8 *var_r7;
    s8 *var_r8;
    int xmin;
    int ymin;
    int xmax;
    int ymax;

    switch (arg0)
    {
    case 0:
        xmin = 0;
        ymin = 0;
        xmax = SCREEN_COLUMNS - 1;
        charPtr = screenBufferChar1 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        ymax = SCREEN_ROWS - 1;
        colorPtr = screenBufferColor1 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r7 = NULL;
        var_r8 = NULL;
        break;
    case 1:
        xmin = 0;
        ymin = 0;
        xmax = SCREEN_COLUMNS - 1;
        charPtr = screenBufferChar2 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        ymax = SCREEN_ROWS - 1;
        colorPtr = screenBufferColor2 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r7 = NULL;
        var_r8 = NULL;
        break;
    default:
        xmin = u_windowBoxX;
        ymin = u_windowBoxY;
        xmax = xmin + u_windowBoxWidth - 1;
        charPtr = screenBufferChar3  + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        ymax = ymin + u_windowBoxHeight - 1;
        colorPtr = screenBufferColor3 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r7 = screenBufferXOffset + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
        var_r8 = screenBufferYOffset + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
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
            if (windowCursorX >= windowCursorStartX)
                windowCursorY++;
            windowCursorX = windowCursorStartX - 1;
            switch (arg0)
            {
            case 0:
                charPtr = screenBufferChar1 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                colorPtr = screenBufferColor1 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                break;
            case 1:
                charPtr = screenBufferChar2 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                colorPtr = screenBufferColor2 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                break;
            default:
                charPtr = screenBufferChar3 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                colorPtr = screenBufferColor3 + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r7 = screenBufferXOffset + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                var_r8 = screenBufferYOffset + (windowCursorY * SCREEN_COLUMNS) + windowCursorX;
                break;
            }
        }
        else if (windowCursorX >= xmin && windowCursorX <= xmax
         && windowCursorY >= ymin && windowCursorY <= ymax)
        {
            *charPtr = *text;
            *colorPtr = windowColorId;
            if (var_r7 != NULL)
                *var_r7 = textOffsetX;
            if (var_r8 != NULL)
                *var_r8 = textOffsetY;
        }
        if (var_r7 != NULL)
            var_r7++;
        if (var_r8 != NULL)
            var_r8++;
        charPtr++;
        text++;
        colorPtr++;
        windowCursorX++;
    }
}

void u_clear_buffers_2_and_5(void)
{
    memset(screenBufferChar2, ' ', sizeof(screenBufferChar2));
    memset(screenBufferColor2, 0, sizeof(screenBufferColor2));
}

static void clear_buffer_region(int x, int y, int width, int height)
{
    char *charPtr;
    u8 *colorPtr;
    int row;
    int col;

    for (row = y; row < y + height; row++)
    {
        charPtr = screenBufferChar3 + row * SCREEN_COLUMNS + x;
        colorPtr = screenBufferColor3 + row * SCREEN_COLUMNS + x;

        for (col = x; col < x + width; col++)
        {
            *charPtr++ = ' ';
            *colorPtr++ = 0;
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

static void draw_char(int x, int y, s8 charIdx, u8 colorId)
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

    if (charIdx < 0x80)
    {
        float u = (12.0 * (float)(charIdx & 0xF)) / 192.0;
        float v = (12.0 * (float)((charIdx >> 4) & 0xF)) / 96.0;
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

    GXLoadTexObj_cached(&u_unkBitmapTPL->texObjs[75], GX_TEXMAP0);
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
