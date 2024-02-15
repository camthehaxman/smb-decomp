/* NOTE: If you wish to make changes to this file, please define
 * NONMATCHING, as shifting data around will break the assembly version
 * of draw_controls_info */
//#define NONMATCHING
#include <math.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "bitmap.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "sound.h"
#include "sprite.h"

static void help_sprite_draw(struct Sprite *);
static void help_sprite_destroy(struct Sprite *);

static s8 lbl_802F2020;
static s8 currPage2;
static s8 unkPageNum;
static s8 lbl_802F2023;
static s8 lbl_802F2024;
static s8 currPage;
static float lbl_802F2028;
static float lbl_802F202C;
static float lbl_802F2030;
static float contScale;
static float lbl_802F2038;
static float lbl_802F203C;
static float lbl_802F2040;
static float lbl_802F2044;
static float lbl_802F2048;
static float lbl_802F204C;
static float lbl_802F2050;
static float lbl_802F2054;
static float lbl_802F2058;

void func_80081F30(void)
{
    struct Sprite *sprite;

    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->tag = 10;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->textAlign = ALIGN_LT;
        sprite->unk4C = 0.002f;
        sprite->userVar = 0;
        sprite->fontId = FONT_JAP_24x24_2P;
        if (gameMode == MD_SEL)
            sprite->mainFunc = help_sprite_main;
        sprite->drawFunc = help_sprite_draw;
        sprite->destFunc = help_sprite_destroy;
        strcpy(sprite->text, "how to");
        lbl_802F2020 = 0xF;
        currPage2 = 0;
        unkPageNum = 0;
        lbl_802F2023 = 0;
        lbl_802F2024 = 0;
        currPage = 0;
        lbl_802F2028 = 1.0f;
        lbl_802F202C = 0.0f;
        lbl_802F2030 = 1.0f;
        contScale = 0.0f;
        lbl_802F2038 = 0.0f;
        lbl_802F203C = 0.0f;
        lbl_802F2040 = 0.0f;
        lbl_802F2044 = 0.0f;
        lbl_802F2048 = 0.0f;
        lbl_802F204C = 1.0f;
        lbl_802F2050 = 0.0f;
        lbl_802F2054 = 1.0f;
        lbl_802F2058 = 1.0f;
    }
}

// number of help pages for each gametype
static s8 pageCounts[][9] =
{
    { 2, 2, 2, 3, 3, 4, 3, 2, 3 },  // in game
    { 2, 2, 2, 5, 3, 4, 4, 3, 4 },  // in SEL mode
};

static char *helpTitlesNormal[] =
{
    "a/Normal Mode movement",
    "a/Normal Mode rules",
};

static char *helpTitlesCompetition[] =
{
    "a/Competition Mode movement",
    "a/Competition Mode rules",
};

static char *helpTitlesPractice[] =
{
    "a/Practice Mode movement",
    "a/Practice Mode rules",
};

static char *helpTitlesRace[] =
{
    "a/Monkey Race controls",
    "a/One Course Race rules",
    "a/About Items",
    "a/One Course Race rules",
    "a/Grand Prix rules",
    "a/Time Attack Rules",
};

static char *helpTitlesFight[] =
{
    "a/Monkey Fight controls",
    "a/Monkey Fight rules",
    "a/About Items",
};

static char *helpTitlesTarget[] =
{
    "a/Monkey Target controls",
    "a/Monkey Target rules",
    "a/About Help Items",
    "a/About the Wheel of Danger",
};

static char *helpTitlesBilliards[] =
{
    "a/View Mode controls",
    "a/Shot Mode controls",
    "a/Vs. Mode rules",
    "a/Tournament Mode rules",
};

static char *helpTitlesBowling[] =
{
    "a/Monkey Bowling controls",
    "a/Standard Mode rules",
    "a/Challenge Mode rules",
};

static char *helpTitlesGolf[] =
{
    "a/Putt Mode controls",
    "a/Camera Mode controls",
    "a/Stroke Play rules",
    "a/Match Play rules",
};

static char **helpPageTitles[] =
{
    helpTitlesNormal,
    helpTitlesCompetition,
    helpTitlesPractice,
    helpTitlesRace,
    helpTitlesFight,
    helpTitlesTarget,
    helpTitlesBilliards,
    helpTitlesBowling,
    helpTitlesGolf,
};

struct SomeMessageThing2
{
    u32 unk0;
    s8 unk4[8];
};

static struct SomeMessageThing2 lbl_801C2318_0x32C[] =
{
    { 0xC24, { 0x05, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC23, { 0x05, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC26, { 0x05, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC22, { 0x05, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC25, { 0x05, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC19, { 0x05, 0x1A, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC1A, { 0x05, 0x1A, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC1C, { 0x05, 0x1A, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC1B, { 0x05, 0x1A, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC1E, { 0x05, 0x1A, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC1D, { 0x05, 0x1A, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC29, { 0x05, 0x1A, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0xC20, { 0x05, 0x1A, 0x31, 0x43, 0x55, 0x00, 0x00, 0x00 } },
    { 0xC1F, { 0x05, 0x1A, 0x2C, 0x43, 0x55, 0x00, 0x00, 0x00 } },
    { 0xC21, { 0x05, 0x1A, 0x2C, 0x43, 0x55, 0x67, 0x79, 0x00 } },
};

static char *lbl_801C2718[] =
{
    "a/Control Stick",
    "a/Roll the ball",
    NULL,
};

static char *lbl_801C274C[] =
{
    "a/START/PAUSE Button",
    "a/Pause Menu",
    NULL,
};

static char *lbl_801C2790[] =
{
    "a/A Button",
    "a/Adjust the size of",
    "a/the on-screen map",
    NULL,
};

static char *lbl_801C27AC[] =
{
    "a/A Button",
    "a/Use Item",
    NULL,
};

static char *lbl_801C27D0[] =
{
    "a/A Button",
    "a/Punch",
    "a/Return to the stage",
    NULL,
};

static char *lbl_801C2888[] =
{
    "a/Control Stick",
    "a/-Before taking flight-",
    "a/Roll the ball",
    "a/-During flight-",
    "a/Push p/SANNKAKU_U/a/ to move down",
    "a/Push p/SANNKAKU_D/a/ to move up",
    "p/SANNKAKU_L/a/ or p/SANNKAKU_R/a/ will turn",
    NULL,
};

static char *lbl_801C28DC[] =
{
    "a/A Button",
    "a/Change to flight mode",
    "a/Change to landing mode",
    NULL,
};

static char *lbl_801C2934[] =
{
    "a/Control Stick",
    "a/Press p/SANNKAKU_L/a/ or p/SANNKAKU_R/a/ to",
    "a/change the direction",
    NULL,
};

static char *lbl_801C2980[] =
{
    "a/A Button",
    "a/Once you choose an angle,",
    "a/it will change to shot mode.",
    NULL,
};

static char *lbl_801C29C4[] =
{
    "a/B Button",
    "a/Return to previous",
    "a/camera view",
    NULL,
};

static char *lbl_801C29F4[] =
{
    "a/C Stick",
    "a/Camera movement",
    NULL,
};

static char *lbl_801C2A1C[] =
{
    "a/X Button",
    "a/Ball number",
    NULL,
};

static char *lbl_801C2A64[] =
{
    "a/Y Button",
    "a/Change between",
    "a/2D top view and 3D view",
    NULL,
};

static char *lbl_801C2A9C[] =
{
    "a/Control Stick",
    "a/Change the shot point",
    "a/of the ball",
    NULL,
};

static char *lbl_801C2AAC[] =
{
    "a/A Button",
    "a/Shot",
    NULL,
};

static char *lbl_801C2AD0[] =
{
    "a/B Button",
    "a/Return to View Mode",
    NULL,
};

static char *lbl_801C2B04[] =
{
    "a/Y Button",
    "a/Change the strength",
    "a/of your shot",
    NULL,
};

static char *lbl_801C2B68[] =
{
    "a/Control Stick",
    "a/Press p/SANNKAKU_L/a/ or p/SANNKAKU_R/a/ to change",
    "a/your throwing position",
    NULL,
};

static char *lbl_801C2BCC[] =
{
    "a/A Button",
    "a/Select:",
    "a/1. Throwing position",
    "a/2. Throwing direction",
    "a/3. Throwing strength",
    NULL,
};

static char *lbl_801C2C08[] =
{
    "a/B Button",
    "a/Cancel the",
    "a/throwing position",
    NULL,
};

static char *lbl_801C2C28[] =
{
    "a/X Button",
    "a/Switch score",
    NULL,
};

static char *lbl_801C2C50[] =
{
    "a/L R Buttons",
    "a/Ball spin",
    NULL,
};

static char *lbl_801C2CC8[] =
{
    "a/Control Stick",
    "a/Press p/SANNKAKU_L/a/ or p/SANNKAKU_R/a/ to change",
    "a/your hitting direction",
    "a/Press p/SANNKAKU_U/a/ or p/SANNKAKU_D/a/ to change",
    "a/your hitting strength",
    NULL,
};

static char *lbl_801C2CFC[] =
{
    "a/A Button",
    "a/Hit the ball",
    "a/Select",
    NULL,
};

static char *lbl_801C2D34[] =
{
    "a/B Button",
    "a/Change hitting strength",
    "a/Cancel",
    NULL,
};

static char *lbl_801C2D5C[] =
{
    "a/C Stick",
    "a/Change to camera mode",
    NULL,
};

static char *lbl_801C2D68[] =
{
    "a/X Button",
    "a/Change to camera mode",
    NULL,
};

static char *lbl_801C2D84[] =
{
    "a/Y Button",
    "a/View score",
    NULL,
};

static char *lbl_801C2E34[] =
{
    "a/Control Stick",
    "a/Press p/SANNKAKU_L/a/ or p/SANNKAKU_R/a/ to turn",
    "a/the camera right or left",
    "a/Press p/SANNKAKU_U/a/ or p/SANNKAKU_D/a/ to move",
    "a/the camera forward or back",
    NULL,
};

static char *lbl_801C2E7C[] =
{
    "a/A Button",
    "a/Face the camera point",
    "a/Return to Putt Mode",
    NULL,
};

static char *lbl_801C2ED0[] =
{
    "a/B Button",
    "a/Return to Putt Mode without",
    "a/changing where you are facing.",
    NULL,
};

static char *lbl_801C2EFC[] =
{
    "a/C Stick",
    "a/Move the",
    "a/camera point",
    NULL,
};

static char *lbl_801C2F1C[] =
{
    "a/X Button",
    "a/Lower camera",
    NULL,
};

static char *lbl_801C2F38[] =
{
    "a/Y Button",
    "a/Raise camera",
    NULL,
};

struct ControlsTextbox
{
    s8 unk0;
    s16 x;
    s16 y;
    char **text;
};

static struct ControlsTextbox controlsTextboxes[] =
{
    { 0x03, 0x000F, 0x0087, lbl_801C2718 },
    { 0x00, 0x00F3, 0x0157, lbl_801C274C },
    { 0x08, 0x01D7, 0x00F6, lbl_801C2790 },
    { 0x01, 0x01D7, 0x00F6, lbl_801C27AC },
    { 0x05, 0x01D7, 0x00F6, lbl_801C27D0 },
    { 0x0E, 0x000F, 0x0087, lbl_801C2888 },
    { 0x05, 0x01D7, 0x00F6, lbl_801C28DC },
    { 0x0B, 0x000F, 0x0087, lbl_801C2934 },
    { 0x00, 0x000F, 0x013F, lbl_801C274C },
    { 0x08, 0x01D7, 0x00F7, lbl_801C2980 },
    { 0x09, 0x01D7, 0x014F, lbl_801C29C4 },
    { 0x04, 0x00F3, 0x0157, lbl_801C29F4 },
    { 0x00, 0x01D7, 0x009F, lbl_801C2A1C },
    { 0x07, 0x00F3, 0x004F, lbl_801C2A64 },
    { 0x0B, 0x000F, 0x0087, lbl_801C2A9C },
    { 0x01, 0x01D7, 0x00F7, lbl_801C2AAC },
    { 0x02, 0x01D7, 0x014F, lbl_801C2AD0 },
    { 0x07, 0x00F3, 0x004F, lbl_801C2B04 },
    { 0x0B, 0x000F, 0x0087, lbl_801C2B68 },
    { 0x0C, 0x01D7, 0x00F7, lbl_801C2BCC },
    { 0x09, 0x00F3, 0x0157, lbl_801C2C08 },
    { 0x00, 0x01D7, 0x009F, lbl_801C2C28 },
    { 0x00, 0x00F3, 0x004F, lbl_801C2C50 },
    { 0x0D, 0x000F, 0x0087, lbl_801C2CC8 },
    { 0x05, 0x01D7, 0x00F7, lbl_801C2CFC },
    { 0x06, 0x01D7, 0x014F, lbl_801C2D34 },
    { 0x04, 0x00F3, 0x0157, lbl_801C2D5C },
    { 0x00, 0x01D7, 0x009F, lbl_801C2D68 },
    { 0x00, 0x00F3, 0x004F, lbl_801C2D84 },
    { 0x0D, 0x000F, 0x0087, lbl_801C2E34 },
    { 0x08, 0x01D7, 0x00F7, lbl_801C2E7C },
    { 0x09, 0x01D7, 0x014F, lbl_801C2ED0 },
    { 0x0A, 0x00F3, 0x0157, lbl_801C2EFC },
    { 0x00, 0x01D7, 0x009F, lbl_801C2F1C },
    { 0x00, 0x00F3, 0x004F, lbl_801C2F38 },
};

static s8 lbl_802F1190[] = { 0x00, 0x01, 0x02, 0xFF };
static s8 lbl_802F1194[] = { 0x00, 0x01, 0xFF };
static s8 lbl_802F1198[] = { 0x00, 0x01, 0x02, 0xFF };
static s8 lbl_802F119C[] = { 0x00, 0x01, 0x03, 0xFF };
static s8 lbl_802F11A0[] = { 0x00, 0x01, 0x04, 0xFF };
static s8 lbl_802F11A4[] = { 0x05, 0x01, 0x06, 0xFF };
static s8 lbl_802F11A8[] = { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0xFF };
static s8 lbl_802F11B0[] = { 0x08, 0x0C, 0x0E, 0x0F, 0x10, 0x11, 0xFF };
static s8 lbl_802F11B8[] = { 0x08, 0x12, 0x13, 0x14, 0x15, 0x16, 0xFF };
static s8 lbl_802F11C0[] = { 0x08, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0xFF };
static s8 lbl_802F11C8[] = { 0x08, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0xFF };

static s8 *lbl_801C2318_0xDD0[] =
{
    lbl_802F1190,
    lbl_802F1194,
    lbl_802F1198,
    lbl_802F119C,
    lbl_802F11A0,
    lbl_802F11A4,
    lbl_802F11A8,
    lbl_802F11B8,
    lbl_802F11C0,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    lbl_802F11B0,
    NULL,
    lbl_802F11C8,
};

struct SomeHelpThing
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    u32 unk10;
};  // size = 0x14

static struct SomeHelpThing lbl_801C2318_0xE18[] =
{
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 320.0f, 346.0f, 320.0f, 238.0f, 0x00A3ABBA },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 163.0f, 346.0f, 310.0f, 238.0f, 0x00A3ABBA },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 476.0f, 339.0f, 381.0f, 250.0f, 0x00DF0029 },
    { 320.0f, 346.0f, 350.0f, 300.0f, 0x00EE9C00 },
    { 476.0f, 183.0f, 434.0f, 213.0f, 0x00A3ABBA },
    { 320.0f, 127.0f, 381.0f, 190.0f, 0x00A3ABBA },
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 476.0f, 339.0f, 381.0f, 250.0f, 0x00DF0029 },
    { 320.0f, 127.0f, 381.0f, 190.0f, 0x00A3ABBA },
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 320.0f, 346.0f, 369.0f, 250.0f, 0x00DF0029 },
    { 476.0f, 183.0f, 434.0f, 213.0f, 0x00A3ABBA },
    { 320.0f, 125.0f, 250.0f, 161.0f, 0x00A3ABBA },
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 476.0f, 339.0f, 381.0f, 250.0f, 0x00DF0029 },
    { 320.0f, 346.0f, 350.0f, 300.0f, 0x00EE9C00 },
    { 476.0f, 183.0f, 434.0f, 213.0f, 0x00A3ABBA },
    { 320.0f, 123.0f, 381.0f, 190.0f, 0x00A3ABBA },
    { 163.0f, 161.0f, 222.0f, 211.0f, 0x0000B2EB },
    { 476.0f, 282.0f, 411.0f, 235.0f, 0x00009F62 },
    { 476.0f, 339.0f, 381.0f, 250.0f, 0x00DF0029 },
    { 320.0f, 346.0f, 350.0f, 300.0f, 0x00EE9C00 },
    { 476.0f, 183.0f, 434.0f, 213.0f, 0x00A3ABBA },
    { 320.0f, 123.0f, 381.0f, 190.0f, 0x00A3ABBA },
};

#define SOME_CONTROLLER_MACRO(btn) \
( \
    (controllerInfo[0].unk0[4].button & (btn)) \
 || (analogButtonInfo[0][4] & (btn)) \
 || (((controllerInfo[0].unk0[0].button & (btn)) || (analogButtonInfo[0][0] & (btn))) && (analogButtonInfo[0][0] & PAD_BUTTON_B)) \
 || (controllerInfo[1].unk0[4].button & (btn)) \
 || (analogButtonInfo[1][4] & (btn)) \
 || (((controllerInfo[1].unk0[0].button & (btn)) || (analogButtonInfo[1][0] & (btn))) && (analogButtonInfo[1][0] & PAD_BUTTON_B)) \
 || (controllerInfo[2].unk0[4].button & (btn)) \
 || (analogButtonInfo[2][4] & (btn)) \
 || (((controllerInfo[2].unk0[0].button & (btn)) || (analogButtonInfo[2][0] & (btn))) && (analogButtonInfo[2][0] & PAD_BUTTON_B)) \
 || (controllerInfo[3].unk0[4].button & (btn)) \
 || (analogButtonInfo[3][4] & (btn)) \
 || (((controllerInfo[3].unk0[0].button & (btn)) || (analogButtonInfo[3][0] & (btn))) && (analogButtonInfo[3][0] & PAD_BUTTON_B))\
)

void help_sprite_main(s8 *status, struct Sprite *sprite)
{
    int var_r4;
    int temp_r6;

    var_r4 = 0;
    if (lbl_802F2020 > 0)
        lbl_802F2020--;
    if (gameMode == MD_SEL)
    {
        if (SOME_CONTROLLER_MACRO(PAD_BUTTON_LEFT) && lbl_802F2020 == 0)
        {
            if (--currPage2 < 0)
                currPage2 = pageCounts[1][modeCtrl.gameType] - 1;
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
        else if (SOME_CONTROLLER_MACRO(PAD_BUTTON_RIGHT) && lbl_802F2020 == 0)
        {
            if (++currPage2 == pageCounts[1][modeCtrl.gameType])
                currPage2 = 0;
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
    }
    else
    {
        if (((controllerInfo[pauseMenuState.padId].unk0[4].button & 1) || (analogButtonInfo[pauseMenuState.padId][4] & 1) || (((controllerInfo[pauseMenuState.padId].unk0[0].button & 1) || (analogButtonInfo[pauseMenuState.padId][0] & 1)) && (analogButtonInfo[pauseMenuState.padId][0] & 0x200))) && (lbl_802F2020 == 0))
        {
            if (--currPage2 < 0)
                currPage2 = pageCounts[0][modeCtrl.gameType] - 1;
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
        else if (((controllerInfo[pauseMenuState.padId].unk0[4].button & 2) || (analogButtonInfo[pauseMenuState.padId][4] & 2) || (((controllerInfo[pauseMenuState.padId].unk0[0].button & 2) || (analogButtonInfo[pauseMenuState.padId][0] & 2)) && (analogButtonInfo[pauseMenuState.padId][0] & 0x200))) && ((s8) lbl_802F2020 == 0))
        {
            if (++currPage2 == pageCounts[0][modeCtrl.gameType])
                currPage2 = 0;
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
    }
    unkPageNum = currPage2;
    currPage = currPage2;
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MINI_RACE:
        if (gameMode == MD_SEL)
        {
            if (currPage2 == 4)
            {
                currPage = 2;
                unkPageNum = 2;
            }
            else if (currPage2 > 0)
            {
                currPage = currPage2 + 2;
                unkPageNum = 1;
                if (var_r4 != 0)
                    lbl_802F2023 = 0x2D;
            }
        }
        else if (unkPageNum == 1)
            currPage = lbl_801EED88.unkC + 3;
        break;
    case GAMETYPE_MINI_TARGET:
        switch (currPage2)
        {
        case 0: unkPageNum = 0; break;
        case 1: unkPageNum = 1; break;
        case 2: unkPageNum = 2; break;
        case 3: unkPageNum = 3; break;
        }
        break;
    case GAMETYPE_MINI_BILLIARDS:
        temp_r6 = currPage2;
        switch (temp_r6)
        {
        case 0:
            unkPageNum = 0;
            break;
        case 1:
            unkPageNum = 4;
            break;
        case 2:
        case 3:
            unkPageNum = 1;
            if (gameMode == MD_SEL)
            {
                currPage = temp_r6;
                if (var_r4 != 0)
                    lbl_802F2023 = 0x2D;
            }
            else
                currPage = lbl_802F1BF4.unk4 + 2;
            break;
        }
        break;
    case GAMETYPE_MINI_BOWLING:
        temp_r6 = currPage2;
        switch (temp_r6)
        {
        case 0:
            unkPageNum = 0;
            if (gameMode == MD_SEL && var_r4 != 0)
                lbl_802F2023 = 0x2D;
            break;
        case 1:
        case 2:
            unkPageNum = 1;
            if (gameMode == MD_SEL)
            {
                currPage = temp_r6;
                if (var_r4 != 0)
                    lbl_802F2023 = 0x2D;
            }
            else
                currPage = lbl_802F1BF0 + 1;
            break;
        }
        break;
    case GAMETYPE_MINI_GOLF:
        temp_r6 = currPage2;
        switch (temp_r6)
        {
        case 0:
            unkPageNum = 0;
            break;
        case 1:
            unkPageNum = 4;
            break;
        case 2:
        case 3:
            unkPageNum = 1;
            if (gameMode == MD_SEL)
            {
                currPage = temp_r6;
                if (var_r4 != 0)
                    lbl_802F2023 = 0x2D;
            }
            else
                currPage = lbl_802F1BE8.unk0 + 2;
            break;
        }
        break;
    }
    if (lbl_802F2023 > 0)
        lbl_802F2023--;
    if (lbl_802F2023 == 0)
        lbl_802F2024 = currPage2;
    switch (sprite->userVar)
    {
    case 0:
        lbl_802F2028 += 0.1 * -lbl_802F2028;
        lbl_802F202C += 0.1 * (100.0 - lbl_802F202C);
        if (lbl_802F202C > 79.0)
            lbl_802F2030 += 0.1 * (80.0 - lbl_802F2030);
        if (unkPageNum == 0 || unkPageNum == 4)
            contScale += 0.1 * (1.0 - contScale);
        else if (lbl_802F2038 < 0.1 && lbl_802F2040 < 0.1)
            contScale += 0.1 * -contScale;
        if (unkPageNum == 0)
        {
            if (contScale > 0.95 && lbl_802F2040 < 0.01)
                lbl_802F2038 += 0.2 * (1.0 - lbl_802F2038);
            if (lbl_802F2038 > 0.99)
                lbl_802F203C += 0.1 * (1.0 - lbl_802F203C);
        }
        else
        {
            lbl_802F203C += 0.3 * -lbl_802F203C;
            if (lbl_802F203C < 0.01)
                lbl_802F2038 += 0.3 * -lbl_802F2038;
        }
        if (unkPageNum == 4)
        {
            if (contScale > 0.95 && lbl_802F2038 < 0.01)
                lbl_802F2040 += 0.2 * (1.0 - lbl_802F2040);
            if (lbl_802F2040 > 0.99)
                lbl_802F2044 += 0.1 * (1.0 - lbl_802F2044);
        }
        else
        {
            lbl_802F2044 += 0.3 * -lbl_802F2044;
            if (lbl_802F2044 < 0.01)
                lbl_802F2040 += 0.3 * -lbl_802F2040;
        }
        if (unkPageNum == 1 && lbl_802F2023 == 0)
        {
            if (lbl_802F2038 < 0.01 && lbl_802F2040 < 0.01)
                lbl_802F204C += (0.2 * -lbl_802F204C);
            if (lbl_802F204C < 0.01)
                lbl_802F2048 += 0.2 * (1.0 - lbl_802F2048);
            if (lbl_802F2048 > 0.99)
                lbl_802F2050 += 0.1 * (1.0 - lbl_802F2050);
        }
        else
        {
            lbl_802F2050 += 0.3 * -lbl_802F2050;
            if (lbl_802F2050 < 0.01)
                lbl_802F2048 += 0.3 * -lbl_802F2048;
            if (lbl_802F2048 < 0.01)
                lbl_802F204C += 0.1 * (1.0 - lbl_802F204C);
        }
        if (unkPageNum == 2)
        {
            if (lbl_802F2048 < 0.01 && contScale < 0.1)
                lbl_802F2054 += 0.1 * -lbl_802F2054;
        }
        else
            lbl_802F2054 += 0.1 * (1.0 - lbl_802F2054);
        if (unkPageNum == 3)
        {
            if (contScale < 0.1)
                lbl_802F2058 += 0.1 * -lbl_802F2058;
        }
        else
            lbl_802F2058 += 0.1 * (1.0 - lbl_802F2058);
        break;
    case 1:
        lbl_802F2028 += 0.1 * (1.0 - lbl_802F2028);
        break;
    }
}

#ifdef NONMATCHING
// https://decomp.me/scratch/Oe5CA
void draw_controls_info(s32 arg0, struct Sprite *arg1, NLsprarg *arg2)
{
    float temp_f21;
    float temp_f20;
    float temp_f17;
    float temp_f16;
    float temp_f15;
    s32 var_r21;
    s32 var_r20;
    u32 temp_r20;
    s8 *temp_r23;
    u32 var_r0;
    struct ControlsTextbox *temp_r19;
    struct SomeHelpThing *temp_r19_3;
    u32 temp_r18;
    struct SomeMessageThing2 *temp_r28;
    u8 unused[8];

    temp_r23 = lbl_801C2318_0xDD0[modeCtrl.gameType + (arg0 * 9)];
    if (temp_r23 != NULL)
    {
        for (var_r21 = 0; temp_r23[var_r21] != -1; var_r21++)
        {
            temp_r19 = &controlsTextboxes[temp_r23[var_r21]];
            func_80071B1C(arg1->unk4C - 0.002);
            temp_r28 = &lbl_801C2318_0x32C[temp_r19->unk0];
            arg2->sprno = temp_r28->unk0;
            arg2->x = temp_r19->x;
            arg2->y = temp_r19->y;
            nlSprPut(arg2);
            for (var_r20 = 0; temp_r19->text[var_r20] != NULL; var_r20++)
            {
                float temp_f16;
                float temp_f15 = (var_r20 == 0) ? 0.541666 : 0.5;
                float temp_f17 = (var_r20 == 0) ? 0.541666 : 0.58;

                var_r0 = (var_r20 == 0) ? RGBA(255, 255, 255, 0) : RGBA(0, 0, 0, 0);
                temp_f16 = u_get_jpn_text_width(arg1->fontId, temp_r19->text[var_r20]);
                temp_f15 = MIN(140.0 / temp_f16, temp_f15);
                set_text_mul_color(var_r0);
                set_text_scale(temp_f15, temp_f17);
                set_text_pos(
                    (76.0f + arg2->x) - (0.5 * temp_f16 * temp_f15),
                    arg2->y + temp_r28->unk4[var_r20]);
                u_draw_text(temp_r19->text[var_r20]);
            }
            temp_r19_3 = &lbl_801C2318_0xE18[temp_r23[var_r21]];

            temp_f17 = arg2->z;
            temp_f16 = arg2->zm_x;
            temp_f15 = arg2->zm_y;
            temp_r20 = arg2->attr;
            temp_r18 = arg2->base_color;
            temp_f21 = temp_r19_3->unk8 - temp_r19_3->unk0;
            temp_f20 = temp_r19_3->unkC - temp_r19_3->unk4;
            arg2->z += 0.0005;
            arg2->trnsl = (arg0 != 0) ? lbl_802F2044 : lbl_802F203C;
            arg2->base_color = temp_r19_3->unk10;
            arg2->sprno = BMP_HOW_how_cont_bar;
            arg2->x = (0.5 * temp_f21) + temp_r19_3->unk0;
            arg2->y = (0.5 * temp_f20) + temp_r19_3->unk4;
            arg2->zm_x = 1.0f;
            arg2->zm_y = 0.125 * mathutil_sqrt((temp_f21 * temp_f21) + (temp_f20 * temp_f20));
            arg2->ang = mathutil_atan2(temp_f21, temp_f20);
            arg2->attr = (arg1->flags & ~0xF) | NLSPR_DISP_CC;
            nlSprPut(arg2);
            arg2->sprno = BMP_HOW_how_cont_arrow;
            arg2->x = temp_r19_3->unk8;
            arg2->y = temp_r19_3->unkC;
            arg2->z -= 0.001;
            arg2->zm_x = 1.0f;
            arg2->zm_y = 1.0f;
            arg2->ang += 0x8000;
            nlSprPut(arg2);
            if (temp_r23[var_r21] == 0x16)
            {
                temp_f21 = 140.0f + (temp_r19_3->unk8 - temp_r19_3->unk0);
                arg2->sprno = BMP_HOW_how_cont_bar;
                arg2->x = (0.5 * temp_f21) + temp_r19_3->unk0;
                arg2->y = (0.5 * temp_f20) + temp_r19_3->unk4;
                arg2->z = temp_f17;
                arg2->zm_x = 1.0f;
                arg2->zm_y = 0.125 * mathutil_sqrt((temp_f21 * temp_f21) + (temp_f20 * temp_f20));
                arg2->ang = mathutil_atan2(temp_f21, temp_f20);
                nlSprPut(arg2);
                arg2->sprno = BMP_HOW_how_cont_arrow;
                arg2->x = 140.0f + temp_r19_3->unk8;
                arg2->y = temp_r19_3->unkC;
                arg2->z -= 0.001;
                arg2->zm_x = 1.0f;
                arg2->zm_y = 1.0f;
                arg2->ang += 0x8000;
                nlSprPut(arg2);
            }
            arg2->z = temp_f17;
            arg2->zm_x = temp_f16;
            arg2->zm_y = temp_f15;
            arg2->ang = 0;
            arg2->trnsl = 1.0f;
            arg2->attr = temp_r20;
            arg2->base_color = temp_r18;
        }
    }
}
#else
void help_c_force_float_order(void)
{
    volatile double d;
    volatile float f;
    d = 0.002;
    d = 0.541666;
    d = 0.5;
    d = 0.58;
    d = 140.0;
    f = 76.0f;
    d = 0.0005;
    d = 0.125;
    d = 0.001;
    f = 140.0f;
}
asm void draw_controls_info(s32 arg0, struct Sprite *arg1, NLsprarg *arg2)
{
    nofralloc
#include "../asm/nonmatchings/draw_controls_info.s"
}
#pragma peephole on
#endif

static u32 lbl_801C2318_0x10D4[] =
{
    RGBA(0x37, 0x61, 0xE2, 0),
    RGBA(0x19, 0xA8, 0x71, 0),
    RGBA(0xFA, 0xCE, 0x19, 0),
    RGBA(0xE2, 0x19, 0x3E, 0),
    RGBA(0x96, 0x28, 0x97, 0),
};

static u32 lbl_801C3400[] =
{
    RGBA(0x7E, 0x42, 0x1F, 0),
    RGBA(0xE2, 0x19, 0x3E, 0),
    RGBA(0xFA, 0xCE, 0x19, 0),
    RGBA(0x96, 0x28, 0x97, 0),
    RGBA(0x19, 0xA8, 0x71, 0),
    RGBA(0x37, 0x61, 0xE2, 0),
};

static u32 lbl_801C3418[] =
{
    RGBA(0x37, 0x61, 0xE2, 0),
    RGBA(0x19, 0xA8, 0x71, 0),
    RGBA(0xFA, 0xCE, 0x19, 0),
    RGBA(0xE2, 0x19, 0x3E, 0),
};

static u32 lbl_801C3428[] =
{
    RGBA(0x7E, 0x42, 0x1F, 0),
    RGBA(0xE2, 0x19, 0x3E, 0),
    RGBA(0xFA, 0xCE, 0x19, 0),
    RGBA(0x96, 0x28, 0x97, 0),
    RGBA(0x19, 0xA8, 0x71, 0),
    RGBA(0x37, 0x61, 0xE2, 0),
};

static u32 lbl_801C3440[] =
{
    RGBA(0x37, 0x61, 0xE2, 0),
    RGBA(0x19, 0xA8, 0x71, 0),
    RGBA(0xFA, 0xCE, 0x19, 0),
    RGBA(0xE2, 0x19, 0x3E, 0),
    RGBA(0x96, 0x28, 0x97, 0),
};

static u32 *messageBgColors[] =
{
    NULL,
    lbl_801C3400,
    lbl_801C3418,
    lbl_801C3428,
    lbl_801C3440,
};

static float lbl_801C3468[] =
{
    112.0f,
    194.0f,
    276.0f,
    358.0f,
};

static float lbl_801C3478[] =
{
    112.0f,
    176.0f,
    240.0f,
    304.0f,
    368.0f,
};

static float lbl_801C348C[] =
{
    98.0f,
    154.0f,
    210.0f,
    266.0f,
    322.0f,
    378.0f,
};

static float *lbl_801C2318_0x118C[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    lbl_801C3468,
    lbl_801C3478,
    lbl_801C348C,
};

static u32 lbl_801C34C0[] =
{
    BMP_COM_icon_ape_00,
    BMP_COM_icon_gal_00,
    BMP_COM_icon_kid_00,
    BMP_COM_icon_gol_00,
};

static u32 lbl_801C34D0[] =
{
    BMP_COM_icon_ape_00,
    BMP_COM_icon_gal_00,
    BMP_COM_icon_kid_afraid03,
    BMP_COM_icon_gol_afraid03,
};

static u32 lbl_801C34E0[] =
{
    BMP_COM_ASCII,
    BMP_COM_ASCII,
    BMP_COM_ASCII,
    BMP_COM_ASCII,
    BMP_COM_icon_ape_00,
};

static u32 *lbl_801C2318_0x11DC[] =
{
    lbl_801C34C0,
    lbl_801C34D0,
    lbl_801C34E0,
};

static u32 lbl_801C3500[] =
{
    BMP_HOW_how_item_rac_box,
    BMP_HOW_how_item_rac_spd,
    BMP_HOW_how_item_rac_bnn,
    BMP_HOW_how_item_rac_bom,
    BMP_HOW_how_item_rac_pol,
    BMP_HOW_how_item_rac_ice,
};

static u32 lbl_801C3518[] =
{
    BMP_HOW_how_item_fgt_box,
    BMP_HOW_how_item_fgt_lng,
    BMP_HOW_how_item_fgt_big,
    BMP_HOW_how_item_fgt_vor,
};

static u32 lbl_801C3528[] =
{
    BMP_COM_icon_ape_00,
    BMP_HOW_how_item_tgt_wind,
    BMP_HOW_how_item_tgt_stop,
    BMP_HOW_how_item_tgt_double,
    BMP_HOW_how_item_tgt_heavy,
    BMP_HOW_how_item_tgt_triple,
};

static u32 lbl_801C3540[] =
{
    BMP_COM_icon_ape_00,
    BMP_HOW_how_item_tgt_saf,
    BMP_HOW_how_item_tgt_kum,
    BMP_HOW_how_item_tgt_tog,
    BMP_HOW_how_item_tgt_bom,
};

static u32 *lbl_801C2318_0x123C[] =
{
    NULL,
    lbl_801C3500,
    lbl_801C3518,
    lbl_801C3528,
    lbl_801C3540,
};

static char *rulesMessagesNormal[] =
{
    "a/If you can guide the ball to the goal without falling off",
    "a/the track, you'll continue on to the next stage!",

    "a/There are delicious bananas scattered throughout the stages!",
    "a/If you eat 100, you'll get an extra life!",

    "a/If you fall off the track, you'll lose a life!",
    "a/If you run out of time, you'll also lose a life!",

    "a/If you keep falling off or running out of time and",
    "a/you run out of extra monkeys, the game will be over!",
};

static char *rulesMessagesCompetition[] =
{
    "a/Try and beat your opponent to the goal!",
    "a/Get there first and receive tons of points!",

    "a/Once the determined number of matches has finished,",
    "a/the person with the most points wins!",

    "a/Even if you fall off the track, you will return to the start of the track!",
    "a/You can continue to play until someone reaches the goal or the time runs out!",

    "a/Once the time runs out, the game is over! Even if no one has",
    "a/reached the goal, you will continue onto the next stage!",
};

static char *rulesMessagesPractice[] =
{
    "a/You can select any stage that you've played before",
    "a/and practice doing it over and over!",

    "a/Use the Control Stick to guide the ball to the goal",
    "a/without falling off the track! Good luck!",

    "a/When you want to practice a new stage, just press the START/PAUSE",
    "a/Button, select the Stage Select option, and change the stage.",

    "a/When you want to quit practicing, just press the START/PAUSE",
    "a/Button and select the Exit Game option.",
};

static char *rulesMessagesRace[] =
{
    // One Course Race rules

    "a/You pick one course and race! Roll your ball",
    "a/as fast as you can and try to come in first!",

    "a/Once you complete the selected number of laps, you will reach",
    "a/the goal! Once all the places have been filled, the race will end.",

    "a/If you run into an Item Box, you will receive an item!",
    "a/You can hold up to 3 items! Use them by pressing the A Button.",

    "a/If you are playing 1 player, there is a time limit! If the time",
    "a/runs out, the game will end and you will be forced to retire!",

    // Grand Prix rules

    "a/Race on all the courses and get points by winning!",
    "a/The player with the most points at the end of all the races wins!",

    "a/Once you complete the selected number of laps, you will reach the",
    "a/goal! The better place you come in, the more points you will get!",

    "a/If you run into an Item Box, you will receive an item!",
    "a/You can hold up to 3 items! Use them by pressing the A Button.",

    "a/If you are playing 1 player, there is a time limit! If the time",
    "a/runs out, the game will end and you will be forced to retire!",

    // Time Attack rules

    "a/Pick one course and see how fast",
    "a/you can complete it by yourself!",

    "a/In this mode, you won't find any Item Boxes,",
    "a/but you will start the race with three Speed Up items!",

    "a/Try and find the best places to use your Speed Up items",
    "a/to get as fast a time as possible to rank in!",

    "a/There is a time limit in this mode!! If the time runs out,",
    "a/the game will end and you will be forced to retire!",
};

static char *rulesMessagesFight[] =
{
    "a/Press the A Button to punch your opponent!",
    "a/If you knock your opponent off the track, you'll receive points!",

    "a/When the time limit runs out,",
    "a/the person with the most points wins!",

    "a/The person with the most wins over the determined number of matches wins!",
    "a/If there's a tie in the final round, the game will go into sudden death!",

    "a/If an Item Box appears, punch it to break it open!",
    "a/If you pick up an item, your punches will become stronger!",
};

static char *rulesMessagesTarget[] =
{
    "a/After jumping off the ramp, while in the air, if you switch to",
    "a/flight mode, your ball will open up and act like a glider!",

    "a/Once you get close to the target, close the ball to go into",
    "a/landing mode. Try to land on top of the target!",

    "a/You will receive points for the target you land on! You will",
    "a/get to jump the determined number of times and add your score up!",

    "a/If you get a banana, you will be able to use Help Items. Also,",
    "a/the Wheel of Danger may put obstacles on the course.",
};

static char *rulesMessagesBilliards[] =
{
    // Vs. Mode rules

    "a/In 2 player mode, the game has similar rules as 9 ball.",
    "a/Whoever pockets the 9 ball first wins.",

    "a/Start by pocketing the ball with the lowest number and work your",
    "a/way up! If you miss, it becomes your opponent's turn.",

    "a/After you hit the ball with the lowest number,",
    "a/and if it goes in a pocket, you can continue playing!",

    "a/If your own ball falls into a pocket, it's considered a foul,",
    "a/and it becomes your opponent's turn.",

    // Tournament Mode rules

    "a/In 1 player mode, you can play against the COM. To win,",
    "a/you must pocket the 9 ball before your opponent.",

    "a/Start by pocketing the ball with the lowest number and work",
    "a/your way up! If you miss, it becomes your opponent's turn.",

    "a/After you hit the ball with the lowest number,",
    "a/and if it goes in a pocket, you can continue playing!",

    "a/If your own ball falls into a pocket, it's considered",
    "a/a foul, and it becomes your opponent's turn.",
};

static char *rulesMessagesBowling[] =
{
    // Standard Mode rules

    "a/At the start, you will use the Control Stick to move left or right in",
    "a/order to determine your throwing position. Press the A Button to decide.",

    "a/Next, an arrow that shows the direction of the throw will appear.",
    "a/Press the A Button to stop the arrow from moving back and forth!",

    "a/After that, the throwing strength gauge will appear. Press the",
    "a/A Button when the meter reaches the strength you want to throw with!",

    "a/Lastly, before the ball gets thrown, you can press",
    "a/the L or R Button to add spin to your ball!",

    "a/Throw the ball with the right angle, strength, and spin to knock",
    "a/over all the pins! Your score will be totaled up from 10 frames!",

    // Challenge Mode rules

    "a/Try to knock down",
    "a/all the pins with one throw!",

    "a/Pins can be set in 10 different possible patterns!",
    "a/You can choose whatever pattern you want!",

    "a/Once you master how to spin the ball, try to knock down",
    "a/the pins in all the different patterns!",

    "a/You have 12 tries to knock down the pins! Once you've thrown",
    "a/the ball 12 times, it's game over!",
};

char *somespacesidk[] =
{
    " ",
    " ",
};

static char *rulesMessagesGolf[] =
{
    // Stroke Play rules

    "a/Play the selected number of holes, and total the scores.",
    "a/The player with the lowest total score will get the better grade.",

    "a/Hit the ball and put it in the cup! The player with",
    "a/the fewer hits will get the better grade.",

    "a/If the ball falls off the track, it counts as an Out",
    "a/of Bounds! A penalty of 1 stroke will be incurred!",

    "a/If you can't knock the ball into the hole within 9 hits, you will",
    "a/be forced to give up! You will be given a score of 10 for that hole.",

    // Match Play rules

    "a/The player to win each hole will receive points for that hole.",
    "a/The player with the least number of strokes wins.",

    "a/If the difference in the score between the losing player and winning",
    "a/player becomes too great for the player to catch up, the game will end.",

    "a/If the ball falls off the track, it counts as an Out",
    "a/of Bounds! A penalty of 1 stroke will be incurred!",

    "a/If you can't knock the ball into the hole within 9 hits, you will",
    "a/be forced to give up! You will be given a score of 10 for that hole.",
};

static char **rulesMessages[] =
{
    rulesMessagesNormal,
    rulesMessagesCompetition,
    rulesMessagesPractice,
    rulesMessagesRace,
    rulesMessagesFight,
    rulesMessagesTarget,
    rulesMessagesBilliards,
    rulesMessagesBowling,
    rulesMessagesGolf,
};

static char *otherMessagesRace1[] =
{
    "a/If you run into an Item Box, you will receive an item,",
    "a/but only luck will determine what item you receive.",

    "a/Speed Star : For a short time,",
    "a/your speed will be increased!",

    "a/Banana Peel : You will drop a banana peel on the ground",
    "a/and if anyone touches it, they will slip!",

    "a/Bowling Bomber : You will release a bomb",
    "a/and if you hit your opponent, it will explode!",

    "a/Polygon Ball : For a short period,",
    "a/your opponent's ball will become polygonal!",

    "a/Ice Cube : For a short period, you will turn your",
    "a/opponent into ice and they will slide all over!",
};

static char *otherMessagesRace2[] =
{
    "a/If you break open an Item Box, you will find an item inside!",
    "a/If you pick up an item, your punches will become stronger!",

    "a/Long Punch : The length of your punches will increase,",
    "a/and you can even hit far away opponents!",

    "a/Big Punch : Your glove becomes huge making it easy",
    "a/for you to hit your opponent!",

    "a/Spinning Punch : For a short period of time,",
    "a/you will do spinning punches!",
};

static char *otherMessagesTarget1[] =
{
    "a/If you set the Help Items to ON in the Gameplay Settings,",
    "a/you will be able to get bananas and use Help Items.",

    "a/Wind 0 mph : Eliminate wind",
    "a/to make it easier to fly!",

    "a/Brake Ball : After you land, this will slow",
    "a/you down more quickly, so you won't roll as much!",

    "a/Double Score : Whatever target you land on,",
    "a/it will double your score!",

    "a/Sticky Landing : When you land on a target,",
    "a/you stick to the place that you land on!",

    "a/Triple Score : Whatever target you land on,",
    "a/it will triple your score!",
};

static char *otherMessagesTarget2[] =
{
    "a/If you turn the Wheel of Danger ON in the Gameplay settings,",
    "a/then before each flight, the Wheel of Danger will spin!",

    "a/Safe (where there's no symbols) : No traps",
    "a/will be set. You can fly without a worry.",

    "a/Smoke Screen : A fog-like cloud",
    "a/makes the targets hard to see!",

    "a/Spiked Ball : There are spiked balls floating in the sky!",
    "a/If you fly into one, you'll be forced to land!",

    "a/Big Bomb : There are bombs set on top of some",
    "a/of the targets! If you land on one, it will explode!",
};

static char **otherMessages[] =
{
    NULL,
    NULL,
    NULL,
    otherMessagesRace1,
    otherMessagesRace2,
    otherMessagesTarget1,
    otherMessagesTarget2,
    NULL,
    NULL,
};

static void draw_help_blurbs(s8 arg0, struct Sprite *sprite, NLsprarg *params, s8 itemCount, s8 arg4, s8 arg5, s8 arg6)
{
    float temp_f24;
    float temp_f25;
    float temp_f25_2;
    float var_f28;
    char *line1;
    char *line2;
    s8 i;

    window_set_cursor_pos(1, 1);

    for (i = 0; i < itemCount; i++)
    {
        if (arg0 == 0)
        {
            var_f28 = 42.0f;
            params->y = (400.0f * lbl_802F204C) + lbl_801C2318_0x118C[itemCount][i];
        }
        else if (arg0 == 4)
        {
            var_f28 = 42.0f - (640.0f * lbl_802F2058);
            params->y = lbl_801C2318_0x118C[itemCount][i];
        }
        else
        {
            var_f28 = 42.0f + (640.0f * lbl_802F2054);
            params->y = lbl_801C2318_0x118C[itemCount][i];
        }
        if (arg0 == 0)
            params->base_color = lbl_801C2318_0x10D4[i];
        else
            params->base_color = messageBgColors[arg0][i];

        temp_f24 = (itemCount == 6) ? 0.86666 : 1.0;
        params->zm_y = temp_f24;
        params->sprno = BMP_HOW_how_bar;
        params->x = var_f28;
        if ((arg0 == 0) || ((arg0 >= 3) && (i == 0)))
        {
            params->u0 = 0.0f;
            params->u1 = 0.333333f;
        }
        else
        {
            params->u0 = 0.333333f;
            params->u1 = 0.666666f;
        }
        params->zm_x = params->u1 - params->u0;
        nlSprPut(params);
        params->sprno = BMP_HOW_how_bar;
        temp_f25 = 32.0f + var_f28;
        params->x = temp_f25;
        params->u0 = 0.333333f;
        params->u1 = 0.666666f;
        if (arg0 == 0)
            params->zm_x = (lbl_802F2048 * (14.775 * (params->u1 - params->u0)));
        else
            params->zm_x = (14.775 * (params->u1 - params->u0));
        nlSprPut(params);
        params->zm_x = 1.0f;
        params->sprno = BMP_HOW_how_bar;
        if (arg0 == 0)
            params->x = temp_f25 + (492.0f * lbl_802F2048);
        else
            params->x = temp_f25 + 492.0f;
        params->u0 = 0.666666f;
        params->u1 = 1.0f;
        params->zm_x = params->u1 - params->u0;
        nlSprPut(params);
        var_f28 = 3.0f + var_f28;
        params->sprno = BMP_HOW_how_bar_circle;
        params->x = var_f28;
        params->y += 2.0f;
        params->z = sprite->unk4C - 0.001;
        params->u0 = 0.0f;
        params->u1 = 1.0f;
        params->zm_x = params->u1 - params->u0;
        params->base_color = RGBA(255, 255, 255, 0);
        if ((arg0 == 0) || ((arg0 >= 3) && (i == 0)))
            nlSprPut(params);
        if (arg0 == 0)
            params->sprno = lbl_801C2318_0x11DC[arg5][i];
        else
            params->sprno = lbl_801C2318_0x123C[arg0][i];
        params->x = var_f28;
        params->y += (itemCount == 6) ? 6 : 7;
        temp_f25_2 = params->y;
        params->z = (sprite->unk4C - 0.002);
        params->u0 = 0.0f;
        params->u1 = 1.0f;
        if (arg0 == 0 || params->sprno == SMD_ADV_GAME_READY_INIT)
        {
            params->zm_x = 0.4f;
            params->zm_y = (0.26 * temp_f24);
        }
        else
        {
            params->x += 3.0f;
            switch (arg0)
            {
            case 1:
                params->x += 2.0f;
                if (i > 0)
                    params->x += 1.0f;
                if (i == 0)
                    params->y -= 2.0f;
                break;
            case 2:
                params->x += 2.0f;
                if (i > 0)
                    params->x += 2.0f;
                if (i == 0)
                    params->y -= 2.0f;
                break;
            case 3:
                params->y += 2.0f;
                break;
            case 4:
                params->x += 4.0f;
                break;
            }
            params->zm_x = 1.0f;
            params->zm_y = temp_f24 + 0.0;
        }
        params->base_color = RGBA(255, 255, 255, 0);
        if (params->sprno != 0)
            nlSprPut(params);
        else
        {
            set_text_scale(1.3f, 1.3f);
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_pos(14.0f + params->x, 4.0f + params->y);
            func_80072AC0("%d", i + 1);
            set_text_scale(0.68f, 0.7f);
            set_text_mul_color(RGBA(255, 255, 192, 0));
        }
        params->z = sprite->unk4C;
        params->zm_y = 1.0f;
        params->x = var_f28;
        params->y = temp_f25_2;

        set_text_opacity(arg0 == 0 ? lbl_802F2050 : 1.0);
        if (arg0 == 0)
        {
            line1 = rulesMessages[modeCtrl.gameType][(2 * arg4 * arg6 + 2 * i) + 0];
            line2 = rulesMessages[modeCtrl.gameType][(2 * arg4 * arg6 + 2 * i) + 1];
        }
        else if (arg0 == 4)
        {
            line1 = otherMessages[modeCtrl.gameType + 1][(2 * arg4 * arg6 + 2 * i) + 0];
            line2 = otherMessages[modeCtrl.gameType + 1][(2 * arg4 * arg6 + 2 * i) + 1];
        }
        else
        {
            line1 = otherMessages[modeCtrl.gameType + 0][(2 * arg4 * arg6 + 2 * i) + 0];
            line2 = otherMessages[modeCtrl.gameType + 0][(2 * arg4 * arg6 + 2 * i) + 1];
        }

        set_text_scale(MIN(474.0 / u_get_jpn_text_width(sprite->fontId, line1), 0.68), 0.7f);
        set_text_pos(56.0f + params->x, params->y - 1.0f);
        u_draw_text(line1);

        set_text_scale(MIN(474.0 / u_get_jpn_text_width(sprite->fontId, line2), 0.68), 0.7f);
        set_text_pos(56.0f + params->x, params->y + ((itemCount == 6) ? 16 : 21));
        u_draw_text(line2);

        set_text_opacity(1.0f);
        set_text_scale(0.68f, 0.7f);
    }
}

static void help_sprite_draw(struct Sprite *sprite)
{
    NLsprarg params;
    s8 var_r29;
    s8 var_r30;
    s8 var_r31;
    s8 var_r27;
    char *title;
    int temp_r25_2;
    s8 var_r9;
    u8 temp_r25_3;

    params.z = sprite->unk4C;
    params.u0 = 0.0f;
    params.v0 = 0.0f;
    params.u1 = 1.0f;
    params.v1 = 1.0f;
    params.zm_x = 1.0f;
    params.zm_y = 1.0f;
    params.trnsl = 1.0f;
    params.ang = 0;
    params.base_color = RGBA(255, 255, 255, (int)(255.0f * sprite->opacity));
    params.offset_color = RGBA(0, 0, 0, 0);
    params.listType = -1;
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_LT;
    reset_text_draw_settings();
    set_text_font(sprite->fontId);
    func_80071B1C(sprite->unk4C);
    params.sprno = BMP_COM_white_mask8x8;
    params.x = 320.1f;
    params.y = 240.1f;
    params.z = 0.001 + sprite->unk4C;
    params.zm_x = lbl_802F202C;
    params.zm_y = lbl_802F2030;
    params.trnsl = 0.6666f;
    params.base_color = RGBA(192, 192, 48, 0);
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_CC;
    nlSprPut(&params);

    params.z = sprite->unk4C;
    params.zm_x = 1.0f;
    params.zm_y = 1.0f;
    params.trnsl = 1.0f;
    params.base_color = RGBA(255, 255, 255, (int)(255.0f * sprite->opacity));
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_LT;
    params.sprno = BMP_HOW_how_kiwaku;
    params.x = 112.0f;
    params.y = 18.0f - (56.0f * lbl_802F2028);
    nlSprPut(&params);

    title = helpPageTitles[modeCtrl.gameType][currPage];
    func_80071B50(0x220000);
    set_text_scale(0.95f, 1.0f);
    func_80071B1C(sprite->unk4C - 0.001);
    set_text_mul_color(RGBA(32, 16, 0, 0));
    set_text_pos(
        3.0 + (320.0 - (0.95 * (0.5 * u_get_jpn_text_width(sprite->fontId, title)))),
        3.0f + (15.0f + params.y));
    u_draw_text(title);
    func_80071B1C(sprite->unk4C - 0.002);
    set_text_mul_color(RGBA(255, 128, 0, 0));
    set_text_pos(
        320.0 - (0.95 * (0.5 * u_get_jpn_text_width(sprite->fontId, title))),
        15.0f + params.y);
    u_draw_text(title);
    func_80071B50(0x20000);
    set_text_scale(1.0f, 1.0f);
    func_80071B1C(sprite->unk4C);
    set_text_mul_color(RGBA(255, 255, 255, 0));
    set_text_mul_color(RGBA(0, 0, 0, 0));
    set_text_scale(0.5f, 0.6f);
    params.sprno = BMP_HOW_how_arrow;
    params.x = 24.0f - (40.0f * lbl_802F2028);
    params.y = 36.0f;
    nlSprPut(&params);

    set_text_pos(6.5 + params.x - 4.0, 40.0f + params.y);
    u_draw_text("a/Back");
    params.x = 576.0f + (40.0f * lbl_802F2028);
    params.y = 36.0f;
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_LT | NLSPR_FLIP_H;
    nlSprPut(&params);

    set_text_pos(6.5 + params.x - 4.0, 40.0f + params.y);
    u_draw_text("a/Next");
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_LT;
    set_text_mul_color(RGBA(255, 255, 255, 0));
    set_text_scale(1.0f, 1.0f);
    set_text_mul_color(RGBA(255, 255, 255, 0));
    set_text_pos(446.0f + (160.0f * lbl_802F2028), 440.0f);
    u_draw_text("p/LEVER/");
    temp_r25_2 = 255.0 * (1.0 - fabs(mathutil_sin(powerOnTimer << 9)));
    set_text_mul_color(RGBA(0, 255, 255, 0));
    set_text_add_color(RGBA(temp_r25_2, temp_r25_2, temp_r25_2, 0));
    set_text_pos(422.0f + (160.0f * lbl_802F2028), 440.0f);
    u_draw_text("p/SANNKAKU_L/a/  p/SANNKAKU_R/");
    set_text_mul_color(RGBA(0, 0, 0, 0));
    set_text_add_color(RGBA(0, 0, 0, 0));
    set_text_pos(48.0f + (30.0f + (422.0f + (160.0f * lbl_802F2028))), 440.0f);
    set_text_scale(0.8f, 1.0f);
    u_draw_text("a/Movement");
    set_text_scale(1.0f, 1.0f);
    set_text_mul_color(RGBA(255, 255, 255, 0));

    // draw controller
    params.sprno = BMP_HOW_how_cont;
    params.x = 320.0f;
    params.y = 254.0f;
    params.zm_x = contScale;
    params.zm_y = contScale;
    params.ang = 65535.0 * (1.0 - contScale);
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_CC;
    nlSprPut(&params);

    params.zm_x = 1.0f;
    params.zm_y = 1.0f;
    params.ang = 0;
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_LT;
    temp_r25_3 = sprite->fontId;
    params.zm_x = 1.0f;
    params.zm_y = lbl_802F2038;
    params.z = sprite->unk4C - 0.001;
    sprite->fontId = 0xB1;
    set_text_font(sprite->fontId);
    set_text_opacity(lbl_802F203C);
    func_80071B50(0x220000);
    draw_controls_info(0, sprite, &params);
    params.zm_x = 1.0f;
    params.zm_y = lbl_802F2040;
    params.z = sprite->unk4C - 0.001;
    set_text_opacity(lbl_802F2044);
    draw_controls_info(1, sprite, &params);
    params.z = sprite->unk4C;
    params.zm_x = 1.0f;
    params.zm_y = 1.0f;
    params.ang = 0;
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_LT;
    sprite->fontId = temp_r25_3;
    set_text_font(sprite->fontId);
    set_text_mul_color(RGBA(255, 255, 255, 0));
    func_80071B1C(sprite->unk4C);
    set_text_scale(1.0f, 1.0f);
    set_text_opacity(1.0f);
    func_80071B50(0x20000);
    set_text_scale(0.68f, 0.7f);
    set_text_mul_color(RGBA(255, 255, 192, 0));
    func_80071B1C(sprite->unk4C - 0.002);

    var_r27 = 0;
    var_r29 = 4;
    var_r31 = 4;
    var_r9 = 0;
    var_r30 = (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL || modeCtrl.gameType == GAMETYPE_MINI_GOLF) ? 1 : 0;
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MINI_RACE:
        if (gameMode == MD_SEL)
        {
            if (lbl_802F2024 > 0 && lbl_802F2024 < 4)
                var_r9 = lbl_802F2024 - 1;
        }
        else
            var_r9 = lbl_801EED88.unkC;
        break;
    case GAMETYPE_MINI_BILLIARDS:
        if (gameMode == MD_SEL)
        {
            if (lbl_802F2024 > 1)
                var_r9 = lbl_802F2024 - 2;
        }
        else
            var_r9 = lbl_802F1BF4.unk4;
        break;
    case GAMETYPE_MINI_BOWLING:
        if (gameMode == MD_SEL)
        {
            if (lbl_802F2024 > 0)
                var_r9 = lbl_802F2024 - 1;
            var_r31 = 5;
        }
        else
        {
            var_r31 = 5;
            var_r9 = lbl_802F1BF0;
        }
        if (var_r9 == 0)
        {
            var_r29 = 5;
            var_r30 = 2;
        }
        break;
    case GAMETYPE_MINI_GOLF:
        if (gameMode == MD_SEL)
        {
            if (lbl_802F2024 > 1)
                var_r9 = lbl_802F2024 - 2;
        }
        else
            var_r9 = lbl_802F1BE8.unk0;
        break;
    }
    draw_help_blurbs(0, sprite, &params, var_r29, var_r31, var_r30, var_r9);
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MINI_RACE:
        var_r27 = 1;
        var_r29 = 6;
        var_r31 = 6;
        break;
    case GAMETYPE_MINI_FIGHT:
        var_r27 = 2;
        var_r29 = 4;
        var_r31 = 4;
        break;
    case GAMETYPE_MINI_TARGET:
        var_r27 = 3;
        var_r29 = 6;
        var_r31 = 6;
        break;
    }
    if (var_r27 != 0)
        draw_help_blurbs(var_r27, sprite, &params, var_r29, var_r31, var_r30, 0);
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MINI_TARGET:
        var_r27 = 4;
        var_r29 = 5;
        var_r31 = 5;
        break;
    }
    if (var_r27 == 4)
        draw_help_blurbs(var_r27, sprite, &params, var_r29, var_r31, var_r30, 0);
}

static void help_sprite_destroy(struct Sprite *sprite)
{
    OSHeapHandle heap;

    if (modeCtrl.gameType == GAMETYPE_MINI_TARGET || modeCtrl.gameType == GAMETYPE_MINI_GOLF)
        heap = OSSetCurrentHeap(charaHeap);
    call_bitmap_free_group(BMP_HOW);
    if (modeCtrl.gameType == GAMETYPE_MINI_TARGET || modeCtrl.gameType == GAMETYPE_MINI_GOLF)
        OSSetCurrentHeap(heap);
}
