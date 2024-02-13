#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "adv.h"
#include "ape_icon.h"
#include "ball.h"
#include "bitmap.h"
#include "info.h"
#include "mathutil.h"
#include "mode.h"
#include "mot_ape.h"
#include "nlsprite.h"
#include "sprite.h"
#include "textbox.h"

struct ApeIconInfo apeIconInfo;

static const u32 neutralFaceTable[] =
{
    BMP_COM_icon_ape_00,
    BMP_COM_icon_gal_00,
    BMP_COM_icon_kid_00,
    BMP_COM_icon_gol_00,
};

static const u32 angryFaceTable[] =
{
    BMP_COM_icon_ape_angry01,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry03,
    BMP_COM_icon_ape_angry03,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry03,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry03,

    BMP_COM_icon_gal_angry01,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry03,
    BMP_COM_icon_gal_angry03,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry03,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry03,

    BMP_COM_icon_kid_angry01,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry03,
    BMP_COM_icon_kid_angry03,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry03,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry03,

    BMP_COM_icon_gol_angry01,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry03,
    BMP_COM_icon_gol_angry03,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry03,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_gol_angry03,
};

static const u32 blinkFaceTable[] =
{
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink02,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_00,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_00,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink02,

    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink02,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_00,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_00,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink02,

    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink02,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_00,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_00,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink02,

    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink02,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_00,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_00,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink02,
};

const u32 smileFaceTable[] =
{
    BMP_COM_icon_smile01,
    BMP_COM_icon_smile02,
    BMP_COM_icon_smile03,
    BMP_COM_icon_smile04,
    BMP_COM_icon_smile03,
    BMP_COM_icon_smile04,

    BMP_COM_icon_gal_smile01,
    BMP_COM_icon_gal_smile02,
    BMP_COM_icon_gal_smile03,
    BMP_COM_icon_gal_smile04,
    BMP_COM_icon_gal_smile03,
    BMP_COM_icon_gal_smile04,

    BMP_COM_icon_kid_smile01,
    BMP_COM_icon_kid_smile02,
    BMP_COM_icon_kid_smile01,
    BMP_COM_icon_kid_smile02,
    BMP_COM_icon_kid_smile01,
    BMP_COM_icon_kid_smile02,

    BMP_COM_icon_gol_smile01,
    BMP_COM_icon_gol_smile02,
    BMP_COM_icon_gol_smile03,
    BMP_COM_icon_gol_smile04,
    BMP_COM_icon_gol_smile03,
    BMP_COM_icon_gol_smile04,
};

static const u32 blinkFaceTable2[] =
{
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink02,
    BMP_COM_icon_ape_blink01,

    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink02,
    BMP_COM_icon_gal_blink01,

    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink02,
    BMP_COM_icon_kid_blink01,

    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink02,
    BMP_COM_icon_gol_blink01,
};

static const u32 afraidFaceTable[] =
{
    BMP_COM_icon_ape_afraid01,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid03,
    BMP_COM_icon_ape_afraid03,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid03,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid03,

    BMP_COM_icon_gal_afraid01,
    BMP_COM_icon_gal_afraid02,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid04,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid04,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid04,

    BMP_COM_icon_kid_afraid01,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid03,
    BMP_COM_icon_kid_afraid03,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid03,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid03,

    BMP_COM_icon_gol_afraid01,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid03,
    BMP_COM_icon_gol_afraid03,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid03,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid03,
};

static const u32 afraidFaceTable2[] =
{
    BMP_COM_icon_ape_afraid03,
    BMP_COM_icon_gal_afraid04,
    BMP_COM_icon_kid_afraid03,
    BMP_COM_icon_gol_afraid03,
};

static const u32 afraidFaceTable3[] =
{
    BMP_COM_icon_ape_afraid05,
    BMP_COM_icon_ape_afraid05,
    BMP_COM_icon_ape_afraid04,

    BMP_COM_icon_gal_afraid01,
    BMP_COM_icon_gal_afraid02,
    BMP_COM_icon_gal_afraid03,

    BMP_COM_icon_kid_afraid01,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid03,

    BMP_COM_icon_gol_afraid05,
    BMP_COM_icon_gol_afraid05,
    BMP_COM_icon_gol_afraid04,
};

static const u32 booingFaceTable[] =
{
    BMP_COM_icon_ape_booing01,
    BMP_COM_icon_ape_booing02,
    BMP_COM_icon_ape_booing03,
    BMP_COM_icon_ape_booing04,

    BMP_COM_icon_gal_booing01,
    BMP_COM_icon_gal_booing02,
    BMP_COM_icon_gal_booing03,
    BMP_COM_icon_gal_booing04,

    BMP_COM_icon_kid_booing01,
    BMP_COM_icon_kid_booing02,
    BMP_COM_icon_kid_booing03,
    BMP_COM_icon_kid_booing04,

    BMP_COM_icon_gol_booing01,
    BMP_COM_icon_gol_booing02,
    BMP_COM_icon_gol_booing03,
    BMP_COM_icon_gol_booing04,
};

static const u32 booingFaceTable2[] =
{
    BMP_COM_icon_ape_booing01,
    BMP_COM_icon_ape_booing02,
    BMP_COM_icon_ape_booing03,
    BMP_COM_icon_ape_booing04,

    BMP_COM_icon_gal_booing01,
    BMP_COM_icon_gal_booing02,
    BMP_COM_icon_gal_booing03,
    BMP_COM_icon_gal_booing04,

    BMP_COM_icon_kid_booing01,
    BMP_COM_icon_kid_booing02,
    BMP_COM_icon_kid_booing03,
    BMP_COM_icon_kid_booing04,

    BMP_COM_icon_gol_booing01,
    BMP_COM_icon_gol_booing02,
    BMP_COM_icon_gol_booing03,
    BMP_COM_icon_gol_booing04,    
};

static const u32 unknownFaceTable[] =
{
    BMP_COM_icon_ape_wakka,
    BMP_COM_icon_gal_wakka,
    BMP_COM_icon_kid_wakka,
    BMP_COM_icon_gol_wakka,
    BMP_COM_icon_kid_booing03,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_kid_smile02,
    BMP_COM_icon_kid_booing03,
    BMP_COM_icon_ape_afraid05,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid02,
};

// sprite callback for the monkey icon on the title screen as well
// as the in-game life counter
void monkey_sprite_draw(struct Sprite *sprite)
{
    s32 i;
    u32 bvar;
    s32 var_r15;
    u32 var_r14;
    int bmpId;
    struct TextBox *tbox;
    s32 temp_r17_2;
    struct Ball *ball;
    NLsprarg params;
    int sp18_2;
    s16 angle;
    s32 temp_r0_2;
    s32 var_r0_3;
    f32 sp14;
    f32 temp_f4;
    f64 a;

    ball = currentBall;
    mathutil_vec_len(&ball->vel);
    if (!(debugFlags & 0xA))
        apeIconInfo.frameNum++;

    bvar = ((modeCtrl.courseFlags & 1)
           && modeCtrl.difficulty == DIFFICULTY_BEGINNER
           && !(modeCtrl.courseFlags & COURSE_FLAG_EXTRA)
           && ball->lives == 1);

    if (apeIconInfo.frameNum > 32)
    {
        var_r14 = 0;
        if (apeIconInfo.unk14 == 0x12)
            apeIconInfo.emotion = 0;
        else if (bvar)
        {
            apeIconInfo.emotion = 2;
            if (apeIconInfo.frameNum == 0x12C)
                var_r14 = 1;
        }
        else if (find_sprite_with_tag(16) != NULL)
            apeIconInfo.emotion = 0xB;
        else if ((infoWork.flags & 0x20) && modeCtrl.submodeTimer == 32)
            apeIconInfo.emotion = 4;
        else if (infoWork.flags & 0x20)
            apeIconInfo.emotion = 3;
        else if (gameSubmode == SMD_GAME_RINGOUT_MAIN && !(infoWork.flags & 0x40) && modeCtrl.submodeTimer <= 0x20)
            apeIconInfo.emotion = 10;
        else if (gameSubmode == SMD_GAME_RINGOUT_MAIN && (infoWork.flags & 0x40) && modeCtrl.submodeTimer <= 0x20)
            apeIconInfo.emotion = 15;
        else if (gameSubmode == SMD_GAME_RINGOUT_MAIN && !(infoWork.flags & 0x40) && modeCtrl.submodeTimer <= 0x40 && apeIconInfo.angle > 0 && apeIconInfo.angle <= 0x1000)
            apeIconInfo.emotion = 13;
        else if (gameSubmode == SMD_GAME_RINGOUT_MAIN && !(infoWork.flags & 0x40))
        {
            apeIconInfo.emotion = 8;
            if (!(infoWork.flags & 0x40) && ball->lives == 5)
                apeIconInfo.unk10 = -0x258;
        }
        else if (gameSubmode == SMD_GAME_TIMEOVER_MAIN && modeCtrl.submodeTimer == 32)
            apeIconInfo.emotion = 12;
        else if (gameSubmode == SMD_GAME_TIMEOVER_MAIN)
        {
            apeIconInfo.emotion = 9;
            if (!(infoWork.flags & 0x40) && ball->lives == 5)
                apeIconInfo.unk10 = -0x258;
        }
        else if (ball->flags & 2)
        {
            apeIconInfo.emotion = 14;
            if (apeIconInfo.frameNum == 0x78)
                var_r14 = 1;
        }
        else if (gameMode != MD_SEL)
        {
            switch (ball->ape->unk0->unk32)
            {
            case 0xF:
            case 0x10:
            case 0x11:
            case 0x1F:
            case 0x20:
            case 0x72:
            case 0x73:
                apeIconInfo.emotion = 14;
                if (apeIconInfo.frameNum == 0x12C)
                    var_r14 = 1;
                break;
            case 0xD:
            case 0x1D:
            case 0x71:
                apeIconInfo.emotion = 16;
                if (apeIconInfo.frameNum == 120)
                    var_r14 = 1;
                break;
            case 0xE:
            case 0x1E:
            case 0x6B:
                apeIconInfo.emotion = 1;
                if (apeIconInfo.frameNum == 0x12C)
                    var_r14 = 1;
                break;
            default:
                apeIconInfo.emotion = 0;
                break;
            }
        }
        if (apeIconInfo.emotion != apeIconInfo.unk4 || var_r14 != 0)
        {
            apeIconInfo.frameNum = 0;
            switch (apeIconInfo.unk4)
            {
            case 1:
                apeIconInfo.emotion = 5;
                break;
            case 2:
                apeIconInfo.emotion = 6;
                break;
            case 3:
            case 11:
                if (apeIconInfo.emotion != 11)
                    apeIconInfo.emotion = 4;
                break;
            case 14:
                apeIconInfo.emotion = 15;
                break;
            case 16:
                apeIconInfo.emotion = 17;
                break;
            }
            if (apeIconInfo.emotion == 8)
                apeIconInfo.angle = (1800.0f * mathutil_sin(apeIconInfo.angle));
            apeIconInfo.unk4 = apeIconInfo.emotion;
        }
    }
    if (gameMode != MD_GAME)
    {
        if (apeIconInfo.unk14 == 14 && modeCtrl.submodeTimer == 0x10DF)
            apeIconInfo.unk10 = 32;
        if (apeIconInfo.unk10 > 0 && !(debugFlags & 0xA))
            apeIconInfo.unk10--;
        if (sprite->userVar == 1)
        {
            sprite->userVar = 2;
            apeIconInfo.unk10 = -31;
        }
        if (apeIconInfo.unk10 < -1 && !(debugFlags & 0xA))
            apeIconInfo.unk10++;
    }
    else
    {
        if (apeIconInfo.unk10 > 0 && !(debugFlags & 0xA))
            apeIconInfo.unk10 -= 1;
        else if (find_sprite_with_tag(16) != NULL && find_sprite_with_tag(16)->counter == 0x95)
            apeIconInfo.unk10 = 32;
        if (apeIconInfo.unk10 < 0 && !(debugFlags & 0xA))
        {
            if (gameSubmode == SMD_GAME_READY_INIT && apeIconInfo.unk10 < -32)
                apeIconInfo.unk10 = -32;
            apeIconInfo.unk10 += 1;
        }
    }
    bmpId = u_get_monkey_bitmap_id(apeIconInfo.emotion, apeIconInfo.frameNum, playerCharacterSelection[modeCtrl.currPlayer]);
    if (!bvar && gameMode == MD_GAME)
    {
        func_8008BFB4(ball->ape, 0, &sp18_2, &sp14);
        temp_r0_2 = mathutil_ceil(sp14 * 0.266660004854f);
        if (sp18_2 >= 0 && sp18_2 < 8)
        {
            switch (sp18_2)
            {
            case 0:
                if (temp_r0_2 == 0)
                    bmpId = neutralFaceTable[playerCharacterSelection[modeCtrl.currPlayer]];
                else
                    bmpId = smileFaceTable[temp_r0_2 - 1 + playerCharacterSelection[modeCtrl.currPlayer] * 6];
                break;
            case 1:
                bmpId = angryFaceTable[temp_r0_2 + (playerCharacterSelection[modeCtrl.currPlayer] * 0xC)];
                break;
            case 4:
                if (temp_r0_2 == 0)
                    bmpId = neutralFaceTable[playerCharacterSelection[modeCtrl.currPlayer]];
                else
                    bmpId = afraidFaceTable[temp_r0_2 - 1 + playerCharacterSelection[modeCtrl.currPlayer] * 0xC];
                break;
            case 5:
                if (temp_r0_2 == 0)
                    bmpId = neutralFaceTable[playerCharacterSelection[modeCtrl.currPlayer]];
                else
                    bmpId = booingFaceTable[temp_r0_2 - 1 + playerCharacterSelection[modeCtrl.currPlayer] * 4];
                break;
            }
        }
    }

    angle = apeIconInfo.angle;
    if (!(debugFlags & 0xA))
    {
        switch (apeIconInfo.emotion)
        {
        case 9:
            break;
        default:
            angle += 0x180;
            break;
        case 1:
            angle += 0x280;
            break;
        case 6:
        case 7:
            angle += 0x100;
            break;
        case 2:
            angle += 0x80;
            break;
        case 8:
            angle -= MIN(apeIconInfo.frameNum << 5, 0x1000);
            break;
        case 10:
        case 13:
            angle += 0.1 * -(u16)angle;
            break;
        case 12:
            angle += 0.1 * -angle;
            break;
        }
    }
    apeIconInfo.angle = angle;

    switch (apeIconInfo.emotion)
    {
    default:
        angle = (1800.0f * mathutil_sin(angle));
        break;
    case 8:
    case 10:
    case 13:
        angle = apeIconInfo.angle;
        break;
    }

    if (ball->lives < 5
     || (ball->lives == 5 && apeIconInfo.unk10 > 0)
     || gameMode != MD_GAME
     || bvar)
    {
        // Display ape face icons for each life

        if (apeIconInfo.unk14 == 0xE)
            var_r15 = 3;
        else if (apeIconInfo.unk14 == 0x12)
            var_r15 = 2;
        else if (gameMode == MD_SEL)
            var_r15 = 4;
        else if (bvar != 0)
            var_r15 = 2;
        else
            var_r15 = ball->lives;
        temp_f4 = apeIconInfo.unk14 == 0xE ? 0.3 : gameMode == MD_SEL ? 1.0 : 0.5;
        a = temp_f4;
        for (i = 0; i < var_r15 - 1; i++)
        {
            tbox = &textBoxes[1];
            if (gameMode == MD_SEL)
                params.sprno = u_get_monkey_bitmap_id(apeIconInfo.emotion, apeIconInfo.frameNum, i);
            else
                params.sprno = bmpId;

            if (bvar)
                params.x = sprite->x + (10.0f * mathutil_sin((globalAnimTimer * 0x60) + 0x4000));
            else if (gameMode == MD_SEL)
            {
                params.x = sprite->x + (i * 90);
                if (apeIconInfo.unk10 < 0)
                    params.x += (apeIconInfo.unk10 + 0x1F) * 5;
            }
            else if (gameMode != MD_GAME)
            {
                params.x = sprite->x + (i * 356);
                if (apeIconInfo.unk10 < 0)
                    params.x += (i == 0) ?  (apeIconInfo.unk10 + 31) * -5 : (apeIconInfo.unk10 + 0x1F) * 5;
            }
            else if (var_r15 == 5 && apeIconInfo.unk10 > 0)
                params.x = sprite->x + (0.03125 * apeIconInfo.unk10 * (i * 80));
            else if (apeIconInfo.unk10 < 0)
                params.x = sprite->x + (0.03125 * (apeIconInfo.unk10 + 32) * (i * 80));
            else
                params.x = sprite->x + (i * 80);

            if (bvar)
                params.y = sprite->y + (4.0f * mathutil_sin(globalAnimTimer << 9));
            else if (gameMode != MD_GAME)
            {
                if (apeIconInfo.unk10 > 0)
                {
                    if (apeIconInfo.unk10 >= 16)
                        params.y = sprite->y - (3.0 * (apeIconInfo.unk10 - 0x10));
                    else
                        params.y = sprite->y - (12.0f * mathutil_sin((0x10 - apeIconInfo.unk10) << 0xB));
                }
                else
                    params.y = sprite->y;
            }
            else if (i == (var_r15 - 2))
            {
                if (apeIconInfo.emotion == 11)
                {
                    if (apeIconInfo.frameNum < 16)
                        params.y = sprite->y - (3.0 * (0x10 - apeIconInfo.frameNum));
                    else if (apeIconInfo.frameNum < 32)
                        params.y = sprite->y - (12.0f * mathutil_sin((apeIconInfo.frameNum - 0x10) << 0xB));
                    else
                        params.y = sprite->y;
                }
                else if (apeIconInfo.unk10 <= 0)
                    params.y = sprite->y;
                else
                    continue;
            }
            else
                params.y = sprite->y;

            if ((s32)advSubmode == 14 && apeIconInfo.unk10 < 0)
            {
                if (i == 0)
                    var_r0_3 = (apeIconInfo.unk10 + 31) << 11;
                else
                    var_r0_3 = -((apeIconInfo.unk10 + 31) << 11);
                params.ang = angle + var_r0_3;
            }
            else if (gameMode != MD_GAME && apeIconInfo.unk10 < 0)
            {
                var_r0_3 = -((apeIconInfo.unk10 + 31) << 11);
                params.ang = angle + var_r0_3;
            }
            else
                params.ang = angle;

            if (bvar)
            {
                if (tbox->state == 1)
                {
                    params.trnsl = (0.25 * tbox->timer) / 30.0;
                }
                else if (tbox->state >= 20)
                {
                    params.trnsl = (0.25 * (30 - tbox->timer)) / 30.0;
                }
                else if (tbox->state >= 10)
                    params.trnsl = 0.25f;
                else
                    params.trnsl = 0.0f;
            }
            else if (gameMode != MD_GAME && apeIconInfo.unk10 > 0)
                params.trnsl = 0.03125 * (32 - apeIconInfo.unk10);
            else if (gameMode != MD_GAME && apeIconInfo.unk10 < 0)
                params.trnsl = -0.032258 * apeIconInfo.unk10;
            else if (i == var_r15 - 2)
            {
                if (apeIconInfo.emotion == 11)
                    params.trnsl = 0.03125 * apeIconInfo.frameNum;
                else if ((apeIconInfo.emotion == 10 || apeIconInfo.emotion == 12) && !(infoWork.flags & 0x40))
                    params.trnsl = ((modeCtrl.submodeTimer % 10) < 5) ? 0.25 + (0.25 * (modeCtrl.submodeTimer / 10)) : 0.0;
                else
                     params.trnsl = 1.0f;
            }
            else
                params.trnsl = 1.0f;
            if (gameMode == MD_SEL && (i != playerCharacterSelection[modeCtrl.currPlayer]))
                params.trnsl *= 0.6;

            if (gameMode == MD_SEL && (i != playerCharacterSelection[modeCtrl.currPlayer]))
            {
                params.zm_x = a * 0.5;
                params.zm_y = (0.65 * a) * 0.5;
            }
            else
            {
                params.zm_x = a;
                params.zm_y = 0.65 * a;
            }

            if (gameMode == MD_SEL && i == playerCharacterSelection[modeCtrl.currPlayer])
                params.z = sprite->unk4C + (0.01 * i) - 0.02;
            else
                params.z = sprite->unk4C + (0.01 * i);

            params.u0 = 0.0f;
            params.v0 = 0.0f;
            params.u1 = 1.0f;
            params.v1 = 1.0f;
            params.listType = NLSPR_LISTTYPE_AUTO;
            params.attr = NLSPR_DISP_CC | NLSPR_UNKFLAG_12;
            params.base_color = -1U;
            params.offset_color = 0;
            nlSprPut(&params);
            if (bvar)
            {
                params.sprno = u_get_monkey_bitmap_id(-1, 0, playerCharacterSelection[modeCtrl.currPlayer]);
                params.z = sprite->unk4C - 0.01;
                nlSprPut(&params);
            }
        }
    }
    else
    {
        // Display X number of lives

        params.sprno = bmpId;
        params.x = sprite->x;
        params.y = sprite->y;
        params.z = sprite->unk4C;
        params.zm_x = 0.5f;
        params.zm_y = 0.325f;
        params.u0 = 0.0f;
        params.v0 = 0.0f;
        params.u1 = 1.0f;
        params.v1 = 1.0f;
        params.ang = angle;
        params.trnsl = 1.0f;
        params.listType = NLSPR_LISTTYPE_AUTO;
        params.attr = NLSPR_DISP_CC | NLSPR_UNKFLAG_12;
        params.base_color = -1U;
        params.offset_color = 0;
        nlSprPut(&params);

        if (apeIconInfo.emotion == 11)
        {
            if (apeIconInfo.frameNum < 0x10)
                params.y = sprite->y - (3.0 * (16 - apeIconInfo.frameNum));
            else if (apeIconInfo.frameNum < 0x20)
                params.y = sprite->y - (12.0f * mathutil_sin((apeIconInfo.frameNum - 0x10) << 0xB));
            else
                params.y = sprite->y;
            params.z = sprite->unk4C - 0.01;
            params.trnsl = 0.03125 * apeIconInfo.frameNum;
            nlSprPut(&params);
        }
        reset_text_draw_settings();
        set_text_font(FONT_ASC_16x16);
        func_80071B1C(sprite->unk4C);
        set_text_pos(36.0f + params.x, 8.0f + params.y);
        func_80072AC0("X%d", ball->lives - 1);

#ifndef NONMATCHING
        tbox == 0; i == 0;  // needed to match
#endif
    }
}

int u_get_monkey_bitmap_id(int emotion, int frame, int charaId)
{
    int idx;

    switch (emotion)
    {
    default:
        return neutralFaceTable[charaId];
    case 1:
        idx = frame >> 3;
        return angryFaceTable[(charaId * 12) + MIN(idx, 11)];
    case 2:
        idx = frame >> 4;
        return blinkFaceTable[(charaId * 12) + MIN(idx, 11)];
    case 3:
    case 11:
        idx = frame >> 3;
        return smileFaceTable[(charaId * 6) + MIN(idx, 5)];
    case 4:
        idx = frame >> 3;
        return smileFaceTable[(charaId * 6) + (idx < 5 ? 5 - idx : 0)];
    case 5:
        idx = frame >> 3;
        return angryFaceTable[(charaId * 12) + (idx < 3 ? 3 - idx : 0)];
    case 6:
        idx = frame >> 4;
        return blinkFaceTable2[(charaId * 3) + (idx < 2 ? 2 - idx : 0)];
    case 7:
        return neutralFaceTable[charaId];
    case 8:
    case 14:
        idx = frame >> 3;
        return afraidFaceTable[(charaId * 12) + MIN(idx, 11)];
    case 10:
    case 15:
        idx = frame >> 3;
        return afraidFaceTable3[(charaId * 3) + (idx < 2 ? 2 - idx : 0)];
    case 13:
        return afraidFaceTable2[charaId];
    case 9:
    case 16:
        idx = frame >> 3;
        return booingFaceTable[(charaId * 4) + MIN(idx, 3)];
    case 12:
    case 17:
        idx = frame >> 3;
        return booingFaceTable2[(charaId * 4) + (idx < 3 ? 3 - idx : 0)];
    case -1:
        return unknownFaceTable[charaId];
    }
}


