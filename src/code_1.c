#include <stddef.h>
#include <dolphin.h>

#include "global.h"
#include "adv.h"
#include "ball.h"
#include "info.h"
#include "mathutil.h"
#include "mode.h"
#include "nlsprite.h"
#include "sprite.h"
#include "textbox.h"

// todo: this is also in hud.c
/*extern*/ struct
{
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    s32 unk10;
    s32 unk14;
} lbl_80292D18;

void func_8008BFB4(struct Ape *ape, int b, int *c, float *d);

const u32 lbl_80118938[] = {
    0xF,
    0x20,
    0x33,
    3,
};
const u32 lbl_80118938_0x10[] = {
    0x15,
    0x16,
    0x17,
    0x17,
    0x16,
    0x16,
    0x16,
    0x17,
    0x16,
    0x16,
    0x16,
    0x17,
    0x25,
    0x26,
    0x27,
    0x27,
    0x26,
    0x26,
    0x26,
    0x27,
    0x26,
    0x26,
    0x26,
    0x27,
    0x37,
    0x38,
    0x39,
    0x39,
    0x38,
    0x38,
    0x38,
    0x39,
    0x38,
    0x38,
    0x38,
    0x39,
    0xB,
    0xE,
    0x4D,
    0x4D,
    0xE,
    0xE,
    0xE,
    0x4D,
    0xE,
    0xE,
    0xE,
    0x4D,
    0x18,
    0x19,
    0x18,
    0xF,
    0x18,
    0x18,
    0x18,
    0x18,
    0x18,
    0xF,
    0x18,
    0x19,
    0x28,
    0x29,
    0x28,
    0x20,
    0x28,
    0x28,
    0x28,
    0x28,
    0x28,
    0x20,
    0x28,
    0x29,
    0x3A,
    0x3B,
    0x3A,
    0x33,
    0x3A,
    0x3A,
    0x3A,
    0x3A,
    0x3A,
    0x33,
    0x3A,
    0x3B,
    0x4E,
    0x4F,
    0x4E,
    3,
    0x4E,
    0x4E,
    0x4E,
    0x4E,
    0x4E,
    3,
    0x4E,
    0x4F,
}; /* const */

const u32 lbl_80118938_0x190[(0x220-0x190)/4] =
{
	0
};

const u32 lbl_80118938_0x220[(0x320-0x220)/4] =
{
	0
};

const u32 lbl_80118938_0x320[69] =
{
	0
};

void monkey_sprite_draw(struct Sprite *arg0 /*r26*/)
{
    s32 var_r29;
    u32 var_r30;
    s32 var_r15;
    u32 var_r14;
    s32 var_r28;
    struct TextBox *temp_r27;
    s32 temp_r17_2;
    struct Ball *temp_r17;
    NLsprarg sp1C;
    int sp18_2;
    s16 var_r3;
    s32 temp_r0_2;
    s32 var_r0_3;
    f32 sp14;
    f32 temp_f4;
    f64 a;

    temp_r17 = currentBall;
    mathutil_vec_len(&temp_r17->vel);
    if (!(debugFlags & 0xA))
        lbl_80292D18.unk8++;

	var_r30 = ((modeCtrl.courseFlags & 1)
           && modeCtrl.difficulty == 0
	       && !(modeCtrl.courseFlags & 8)
	       && temp_r17->lives == 1);

    if (lbl_80292D18.unk8 > 0x20)
    {
        var_r14 = 0;
        if (lbl_80292D18.unk14 == 0x12)
            lbl_80292D18.unk0 = 0;
        else if (var_r30)
        {
            lbl_80292D18.unk0 = 2;
            if (lbl_80292D18.unk8 == 0x12C)
                var_r14 = 1;
        }
        else if (find_sprite_with_tag(0x10) != NULL)
            lbl_80292D18.unk0 = 0xB;
        else if ((infoWork.flags & 0x20) && modeCtrl.submodeTimer == 0x20)
            lbl_80292D18.unk0 = 4;
        else if (infoWork.flags & 0x20)
            lbl_80292D18.unk0 = 3;
        else if (gameSubmode == 0x3B && !(infoWork.flags & 0x40) && modeCtrl.submodeTimer <= 0x20)
            lbl_80292D18.unk0 = 0xA;
        else if (gameSubmode == 0x3B && (infoWork.flags & 0x40) && modeCtrl.submodeTimer <= 0x20)
            lbl_80292D18.unk0 = 0xF;
        else if (gameSubmode == 0x3B && !(infoWork.flags & 0x40) && modeCtrl.submodeTimer <= 0x40 && lbl_80292D18.unkC > 0 && lbl_80292D18.unkC <= 0x1000)
            lbl_80292D18.unk0 = 0xD;
        else if (gameSubmode == 0x3B && !(infoWork.flags & 0x40))
        {
            lbl_80292D18.unk0 = 8;
            if (!(infoWork.flags & 0x40) && temp_r17->lives == 5)
                lbl_80292D18.unk10 = -0x258;
        }
        else if (gameSubmode == 0x39 && modeCtrl.submodeTimer == 0x20)
            lbl_80292D18.unk0 = 0xC;
        else if (gameSubmode == 0x39)
        {
            lbl_80292D18.unk0 = 9;
            if (!(infoWork.flags & 0x40) && temp_r17->lives == 5)
                lbl_80292D18.unk10 = -0x258;
        }
        else if (temp_r17->flags & 2)
        {
            lbl_80292D18.unk0 = 0xE;
            if (lbl_80292D18.unk8 == 0x78)
                var_r14 = 1;
        }
        else if (gameMode != 1)
        {
			switch (temp_r17->ape->unk0->unk32)
			{
			case 0xF:
			case 0x10:
			case 0x11:
			case 0x1F:
			case 0x20:
			case 0x72:
			case 0x73:
				lbl_80292D18.unk0 = 0xE;
				if (lbl_80292D18.unk8 == 0x12C)
					var_r14 = 1;
				break;
			case 0xD:
			case 0x1D:
			case 0x71:
		    	lbl_80292D18.unk0 = 0x10;
				if (lbl_80292D18.unk8 == 0x78)
					var_r14 = 1;
				break;
			case 0xE:
			case 0x1E:
			case 0x6B:
				lbl_80292D18.unk0 = 1;
                if (lbl_80292D18.unk8 == 0x12C)
                    var_r14 = 1;
                break;
            default:
				lbl_80292D18.unk0 = 0;
				break;
			}
        }
        if (lbl_80292D18.unk0 != lbl_80292D18.unk4 || var_r14 != 0)
        {
            lbl_80292D18.unk8 = 0;
            switch (lbl_80292D18.unk4)
            {
            case 1:
                lbl_80292D18.unk0 = 5;
                break;
            case 2:
                lbl_80292D18.unk0 = 6;
                break;
            case 3:
            case 11:
                if (lbl_80292D18.unk0 != 0xB)
                {
                    lbl_80292D18.unk0 = 4;
                }
                break;
            case 14:
                lbl_80292D18.unk0 = 0xF;
                break;
            case 16:
                lbl_80292D18.unk0 = 0x11;
                break;
            }
            if (lbl_80292D18.unk0 == 8)
                lbl_80292D18.unkC = (1800.0f * mathutil_sin(lbl_80292D18.unkC));
            lbl_80292D18.unk4 = lbl_80292D18.unk0;
        }
    }
    if (gameMode != 2)
    {
        if (lbl_80292D18.unk14 == 0xE && modeCtrl.submodeTimer == 0x10DF)
            lbl_80292D18.unk10 = 0x20;
        if (lbl_80292D18.unk10 > 0 && !(debugFlags & 0xA))
            lbl_80292D18.unk10--;
        if (arg0->userVar == 1)
        {
            arg0->userVar = 2;
            lbl_80292D18.unk10 = -0x1F;
        }
        if (lbl_80292D18.unk10 < -1 && !(debugFlags & 0xA))
            lbl_80292D18.unk10++;
    }
    else
    {
        if (lbl_80292D18.unk10 > 0 && !(debugFlags & 0xA))
            lbl_80292D18.unk10 -= 1;
        else if (find_sprite_with_tag(0x10) != NULL && find_sprite_with_tag(0x10)->counter == 0x95)
            lbl_80292D18.unk10 = 0x20;
        if (lbl_80292D18.unk10 < 0 && !(debugFlags & 0xA))
        {
            if (gameSubmode == 0x30 && lbl_80292D18.unk10 < -0x20)
                lbl_80292D18.unk10 = -32;
            lbl_80292D18.unk10 += 1;
        }
    }
    var_r28 = u_get_monkey_bitmap_id(lbl_80292D18.unk0, lbl_80292D18.unk8, playerCharacterSelection[modeCtrl.currPlayer]);
    if (!var_r30 && gameMode == 2)
    {
        func_8008BFB4(temp_r17->ape, 0, &sp18_2, &sp14);
        temp_r0_2 = mathutil_ceil(sp14 * 0.266660004854f);
        if (sp18_2 >= 0 && sp18_2 < 8)
        {
            switch (sp18_2)
            {
            case 0:
                if (temp_r0_2 == 0)
                    var_r28 = lbl_80118938[playerCharacterSelection[modeCtrl.currPlayer]];
                else
                    var_r28 = lbl_80118938_0x190[temp_r0_2 - 1 + playerCharacterSelection[modeCtrl.currPlayer] * 6];
                break;
            case 1:
                var_r28 = lbl_80118938_0x10[temp_r0_2 + (playerCharacterSelection[modeCtrl.currPlayer] * 0xC)];
                break;
            case 4:
                if (temp_r0_2 == 0)
                    var_r28 = lbl_80118938[playerCharacterSelection[modeCtrl.currPlayer]];
                else
                    var_r28 = lbl_80118938_0x220[temp_r0_2 - 1 + playerCharacterSelection[modeCtrl.currPlayer] * 0xC];
                break;
            case 5:
                if (temp_r0_2 == 0)
                    var_r28 = lbl_80118938[playerCharacterSelection[modeCtrl.currPlayer]];
                else
                    var_r28 = lbl_80118938_0x320[temp_r0_2 - 1 + playerCharacterSelection[modeCtrl.currPlayer] * 4];
                break;
            }
        }
    }
    var_r3 = lbl_80292D18.unkC;
    if (!(debugFlags & 0xA))
    {
        switch (lbl_80292D18.unk0)
        {
        default:
            var_r3 += 0x180;
            break;
        case 1:
            var_r3 += 0x280;
            break;
        case 6:
        case 7:
            var_r3 += 0x100;
            break;
        case 2:
            var_r3 += 0x80;
            break;
        case 8:
            var_r3 -= MIN(lbl_80292D18.unk8 << 5, 0x1000);
            break;
        case 10:
        case 13:
            var_r3 += 0.1 * -(u16)var_r3;
            break;
        case 12:
            var_r3 += 0.1 * -var_r3;
            break;
        }
    }
    lbl_80292D18.unkC = var_r3;

    switch (lbl_80292D18.unk0)
    {
	default:
        var_r3 = (1800.0f * mathutil_sin(var_r3));
		break;
	case 8:
	case 10:
	case 13:
		var_r3 = lbl_80292D18.unkC;
		break;
	}

    if (temp_r17->lives < 5 || (( temp_r17->lives == 5) && (lbl_80292D18.unk10 > 0)) || (gameMode != 2) || (var_r30 != 0U))
    {
        if (lbl_80292D18.unk14 == 0xE)
            var_r15 = 3;
        else if (lbl_80292D18.unk14 == 0x12)
            var_r15 = 2;
        else if (gameMode == 1)
            var_r15 = 4;
        else if (var_r30 != 0)
            var_r15 = 2;
        else
            var_r15 = temp_r17->lives;
        temp_f4 = lbl_80292D18.unk14 == 0xE ? 0.3 : gameMode == 1 ? 1.0 : 0.5;
        a = temp_f4;
        for (var_r29 = 0; var_r29 < var_r15 - 1; var_r29++)
        {
            temp_r27 = &textBoxes[1];
            if (gameMode == 1)
                sp1C.sprno = u_get_monkey_bitmap_id(lbl_80292D18.unk0, lbl_80292D18.unk8, var_r29);
            else
                sp1C.sprno = var_r28;
            if (var_r30 != 0U)
                sp1C.x = arg0->x + (10.0f * mathutil_sin((globalAnimTimer * 0x60) + 0x4000));
            else if (gameMode == 1)
            {

                sp1C.x = arg0->x + (var_r29 * 90);
                if (lbl_80292D18.unk10 < 0)
                    sp1C.x += (lbl_80292D18.unk10 + 0x1F) * 5;
            }
            else if (gameMode != 2)
            {
                sp1C.x = arg0->x + (var_r29 * 356);
                if (lbl_80292D18.unk10 < 0)
                    sp1C.x += (var_r29 == 0) ?  (lbl_80292D18.unk10 + 0x1F) * -5 : (lbl_80292D18.unk10 + 0x1F) * 5;
            }
            else if (var_r15 == 5 && lbl_80292D18.unk10 > 0)
                sp1C.x = arg0->x + (0.03125 * lbl_80292D18.unk10 * (var_r29 * 80));
            else if (lbl_80292D18.unk10 < 0)
                sp1C.x = arg0->x + (0.03125 * (lbl_80292D18.unk10 + 0x20) * (var_r29 * 80));
            else
                sp1C.x = arg0->x + (var_r29 * 80);

            if (var_r30 != 0U)
                sp1C.y = arg0->y + (4.0f * mathutil_sin(globalAnimTimer << 9));
            else if (gameMode != 2)
            {
                if (lbl_80292D18.unk10 > 0)
                {
                    if (lbl_80292D18.unk10 >= 0x10)
                        sp1C.y = arg0->y - (3.0 * (lbl_80292D18.unk10 - 0x10));
                    else
                        sp1C.y = arg0->y - (12.0f * mathutil_sin((0x10 - lbl_80292D18.unk10) << 0xB));
                }
                else
                    sp1C.y = arg0->y;
            }
            else if (var_r29 == (var_r15 - 2))
            {
                if (lbl_80292D18.unk0 == 0xB)
                {
                    if (lbl_80292D18.unk8 < 0x10)
                        sp1C.y = arg0->y - (3.0 * (0x10 - lbl_80292D18.unk8));
                    else if (lbl_80292D18.unk8 < 0x20)
                        sp1C.y = arg0->y - (12.0f * mathutil_sin((lbl_80292D18.unk8 - 0x10) << 0xB));
                    else
                        sp1C.y = arg0->y;
                }
                else if (lbl_80292D18.unk10 <= 0)
                    sp1C.y = arg0->y;
                else
                    continue;
            }
            else
                sp1C.y = arg0->y;
            if ((s32)advSubmode == 0xE && lbl_80292D18.unk10 < 0)
            {
                if (var_r29 == 0)
                    var_r0_3 = (lbl_80292D18.unk10 + 0x1F) << 0xB;
                else
                    var_r0_3 = -((lbl_80292D18.unk10 + 0x1F) << 0xB);
                sp1C.ang = var_r3 + var_r0_3;
            }
            else if (gameMode != 2 && lbl_80292D18.unk10 < 0)
            {
                var_r0_3 = -((lbl_80292D18.unk10 + 0x1F) << 0xB);
                sp1C.ang = var_r3 + var_r0_3;
            }
            else
                sp1C.ang = var_r3;

            if (var_r30)
            {
                if (temp_r27->state == 1)
                {
                    sp1C.trnsl = (0.25 * temp_r27->timer) / 30.0;
                }
                else if (temp_r27->state >= 0x14)
                {
                    sp1C.trnsl = (0.25 * (30 - temp_r27->timer)) / 30.0;
                }
                else if (temp_r27->state >= 0xA)
                    sp1C.trnsl = 0.25f;
                else
                    sp1C.trnsl = 0.0f;
            }
            else if (gameMode != 2 && lbl_80292D18.unk10 > 0)
                sp1C.trnsl = 0.03125 * (0x20 - lbl_80292D18.unk10);
            else if (gameMode != 2 && lbl_80292D18.unk10 < 0)
                sp1C.trnsl = -0.032258 * lbl_80292D18.unk10;
            else if (var_r29 == var_r15 - 2)
            {
                if (lbl_80292D18.unk0 == 11)
                    sp1C.trnsl = 0.03125 * lbl_80292D18.unk8;
                else if ((lbl_80292D18.unk0 == 10 || lbl_80292D18.unk0 == 12) && !(infoWork.flags & 0x40))
                    sp1C.trnsl = ((modeCtrl.submodeTimer % 10) < 5) ? 0.25 + (0.25 * (modeCtrl.submodeTimer / 10)) : 0.0;
                else
                     sp1C.trnsl = 1.0f;
            }
            else
                sp1C.trnsl = 1.0f;
            if (gameMode == 1 && (var_r29 != playerCharacterSelection[modeCtrl.currPlayer]))
            {
                sp1C.trnsl *= 0.6;
            }
            if (gameMode == 1 && (var_r29 != playerCharacterSelection[modeCtrl.currPlayer]))
            {
                sp1C.zm_x = a * 0.5;
                sp1C.zm_y = (0.65 * a) * 0.5;
            }
            else
            {
                sp1C.zm_x = a;
                sp1C.zm_y = 0.65 * a;
            }
            if (gameMode == 1 && var_r29 == playerCharacterSelection[modeCtrl.currPlayer])
                sp1C.z = arg0->unk4C + (0.01 * var_r29) - 0.02;
            else
                sp1C.z = arg0->unk4C + (0.01 * var_r29);
            sp1C.u0 = 0.0f;
            sp1C.v0 = 0.0f;
            sp1C.u1 = 1.0f;
            sp1C.v1 = 1.0f;
            sp1C.listType = -1;
            sp1C.attr = 0x100A;
            sp1C.base_color = -1U;
            sp1C.offset_color = 0;
            nlSprPut(&sp1C);
            if (var_r30)
            {
                sp1C.sprno = u_get_monkey_bitmap_id(-1, 0, playerCharacterSelection[modeCtrl.currPlayer]);
                sp1C.z = arg0->unk4C - 0.01;
                nlSprPut(&sp1C);
            }
        }
        return;
    }
    sp1C.sprno = var_r28;
    sp1C.x = arg0->x;
    sp1C.y = arg0->y;
    sp1C.z = arg0->unk4C;
    sp1C.zm_x = 0.5f;
    sp1C.zm_y = 0.325f;
    sp1C.u0 = 0.0f;
    sp1C.v0 = 0.0f;
    sp1C.u1 = 1.0f;
    sp1C.v1 = 1.0f;
    sp1C.ang = var_r3;
    sp1C.trnsl = 1.0f;
    sp1C.listType = -1;
    sp1C.attr = 0x100A;
    sp1C.base_color = -1U;
    sp1C.offset_color = 0;
    nlSprPut(&sp1C);

    if (lbl_80292D18.unk0 == 0xB)
    {
        if (lbl_80292D18.unk8 < 0x10)
            sp1C.y = arg0->y - (3.0 * (16 - lbl_80292D18.unk8));
        else if (lbl_80292D18.unk8 < 0x20)
            sp1C.y = arg0->y - (12.0f * mathutil_sin((lbl_80292D18.unk8 - 0x10) << 0xB));
        else
            sp1C.y = arg0->y;
        sp1C.z = arg0->unk4C - 0.01;
        sp1C.trnsl = 0.03125 * lbl_80292D18.unk8;
        nlSprPut(&sp1C);
    }
    reset_text_draw_settings();
    set_text_font(0x5F);
    func_80071B1C(arg0->unk4C);
    set_text_pos(36.0f + sp1C.x, 8.0f + sp1C.y);
    func_80072AC0("X%d", temp_r17->lives - 1);

    // needed to match
    temp_r27 == 0;
    var_r29 == 0;
}
