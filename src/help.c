#include <stddef.h>
#include <dolphin.h>

#include "global.h"
#include "input.h"
#include "mode.h"
#include "sprite.h"

void func_80082024(s8 *, struct Sprite *);
void lbl_80083910(struct Sprite *);
void lbl_800841E8(struct Sprite *);

extern s8 lbl_802F2020;
extern u8 lbl_802F2021;
extern u8 lbl_802F2022;
extern u8 lbl_802F2023;
extern u8 lbl_802F2024;
extern u8 lbl_802F2025;
extern float lbl_802F2028;
extern float lbl_802F202C;
extern float lbl_802F2030;
extern float lbl_802F2034;
extern float lbl_802F2038;
extern float lbl_802F203C;
extern float lbl_802F2040;
extern float lbl_802F2044;
extern float lbl_802F2048;
extern float lbl_802F204C;
extern float lbl_802F2050;
extern float lbl_802F2054;
extern float lbl_802F2058;

void func_80081F30(void)
{
    struct Sprite *temp_r3;

    temp_r3 = create_sprite();
    if (temp_r3 != NULL)
    {
        temp_r3->tag = 0xA;
        temp_r3->x = 320.0f;
        temp_r3->y = 240.0f;
        temp_r3->textAlign = 0;
        temp_r3->unk4C = 0.002f;
        temp_r3->userVar = 0;
        temp_r3->fontId = 0xB1;
        if (gameMode == 1)
            temp_r3->mainFunc = func_80082024;
        temp_r3->drawFunc = lbl_80083910;
        temp_r3->destFunc = lbl_800841E8;
        strcpy(temp_r3->text, "how to");
        lbl_802F2020 = 0xF;
        lbl_802F2021 = 0;
        lbl_802F2022 = 0;
        lbl_802F2023 = 0;
        lbl_802F2024 = 0;
        lbl_802F2025 = 0;
        lbl_802F2028 = 1.0f;
        lbl_802F202C = 0.0f;
        lbl_802F2030 = 1.0f;
        lbl_802F2034 = 0.0f;
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

s8 lbl_801C2318[100] = {0};

void func_80082024(s8 *arg0, struct Sprite *arg1)
{
    s32 temp_r0_4;
    s32 var_r4;
    s8 temp_r0_3;
    s8 temp_r5;
    s8 temp_r5_2;
    s8 temp_r6_3;
    s8 temp_r6_4;
    s8 temp_r6_5;
    s8 temp_r8;
    struct ControllerInfo *temp_r7;
    u16 *temp_r6;
    u16 *temp_r6_2;
    u16 temp_r10;
    u8 temp_r0;
    u8 temp_r0_2;
    u8 temp_r4;
    u8 temp_r4_2;

    var_r4 = 0;
    if ((s8) lbl_802F2020 > 0)
    {
        lbl_802F2020 -= 1;
    }
    if ((s16) gameMode == 1)
    {
        if (((controllerInfo[0].unk0[4].button & 1) || (analogButtonInfo[0][4] & 1) || (((controllerInfo[0].unk0[0].button & 1) || (analogButtonInfo[0][0] & 1)) && (analogButtonInfo[0][0] & 0x200)) || (controllerInfo[1].unk0[4].button & 1) || (analogButtonInfo[1][4] & 1) || (((controllerInfo[1].unk0[0].button & 1) || (analogButtonInfo[1][0] & 1)) && (analogButtonInfo[1][0] & 0x200)) || (controllerInfo[2].unk0[4].button & 1) || (analogButtonInfo[2][4] & 1) || (((controllerInfo[2].unk0[0].button & 1) || (analogButtonInfo[2][0] & 1)) && (analogButtonInfo[2][0] & 0x200)) || (controllerInfo[3].unk0[4].button & 1) || (analogButtonInfo[3][4] & 1) || (((controllerInfo[3].unk0[0].button & 1) || (analogButtonInfo[3][0] & 1)) && (analogButtonInfo[3][0] & 0x200))) && (((lbl_802F2020 == 0))))
        {
            temp_r4 = lbl_802F2021 - 1;
            lbl_802F2021 = temp_r4;
            if ((s8) temp_r4 < 0)
            {
                lbl_802F2021 = lbl_801C2318[modeCtrl.gameType + 9] - 1;
            }
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
        else if (((controllerInfo[0].unk0[4].button & 2) || (analogButtonInfo[0][4] & 2) || (((controllerInfo[0].unk0[0].button & 2) || (analogButtonInfo[0][0] & 2)) && (analogButtonInfo[0][0] & 0x200)) || (controllerInfo[1].unk0[4].button & 2) || (analogButtonInfo[1][4] & 2) || (((controllerInfo[1].unk0[0].button & 2) || (analogButtonInfo[1][0] & 2)) && (analogButtonInfo[1][0] & 0x200)) || (controllerInfo[2].unk0[4].button & 2) || (analogButtonInfo[2][4] & 2) || (((controllerInfo[2].unk0[0].button & 2) || (analogButtonInfo[2][0] & 2)) && (analogButtonInfo[2][0] & 0x200)) || (controllerInfo[3].unk0[4].button & 2) || (analogButtonInfo[3][4] & 2) || (((controllerInfo[3].unk0[0].button & 2) || (analogButtonInfo[3][0] & 2)) && (analogButtonInfo[3][0] & 0x200))) && ((s8) lbl_802F2020 == 0))
        {
            temp_r0 = lbl_802F2021 + 1;
            lbl_802F2021 = temp_r0;
            if ((s8) temp_r0 == lbl_801C2318[modeCtrl.gameType + 9])
            {
                lbl_802F2021 = 0;
            }
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
    }
    else
    {
        temp_r8 = (s8) (u8) pauseMenuState.padId;
        temp_r7 = &controllerInfo[temp_r8];
        temp_r10 = temp_r7->unk0[4].button;
        if (((controllerInfo[temp_r8].unk0[4].button & 1) || (analogButtonInfo[temp_r8][4] & 1) || (((controllerInfo[temp_r8].unk0[0].button & 1) || (analogButtonInfo[temp_r8][0] & 1)) && (analogButtonInfo[temp_r8][0] & 0x200))) && (lbl_802F2020 != 0))
        {
            temp_r4_2 = lbl_802F2021 - 1;
            lbl_802F2021 = temp_r4_2;
            if ((s8) temp_r4_2 < 0)
            {
                lbl_802F2021 = lbl_801C2318[modeCtrl.gameType] - 1;
            }
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
        else if (((controllerInfo[temp_r8].unk0[4].button & 2) || (analogButtonInfo[temp_r8][4] & 2) || (((controllerInfo[temp_r8].unk0[0].button & 2) || (analogButtonInfo[temp_r8][0] & 2)) && (analogButtonInfo[temp_r8][0] & 0x200))) && ((s8) lbl_802F2020 == 0))
        {
            temp_r0_2 = lbl_802F2021 + 1;
            lbl_802F2021 = temp_r0_2;
            if ((s8) temp_r0_2 == lbl_801C2318[modeCtrl.gameType])
            {
                lbl_802F2021 = 0;
            }
            u_play_sound_1(0x169);
            lbl_802F2020 = 0xF;
            var_r4 = 1;
        }
    }
    lbl_802F2022 = lbl_802F2021;
    lbl_802F2025 = lbl_802F2021;
    switch ((s32) modeCtrl.gameType)                /* irregular */
    {
    case 3:
        if ((s16) gameMode == 1)
        {
            if ((s8) lbl_802F2021 == 4)
            {
                lbl_802F2025 = 2;
                lbl_802F2022 = 2;
            }
            else if ((s8) lbl_802F2021 > 0)
            {
                lbl_802F2025 = lbl_802F2021 + 2;
                lbl_802F2022 = 1;
                if (var_r4 != 0)
                {
                    lbl_802F2023 = 0x2D;
                }
            }
        }
        else if ((s32) lbl_802F2022 == 1)
        {
            lbl_802F2025 = lbl_801EED88.unkC + 3;
        }
        break;
    case 5:
        temp_r0_3 = (s8) lbl_802F2021;
        switch (temp_r0_3)                          /* switch 1; irregular */
        {
        case 0:                                     /* switch 1 */
            lbl_802F2022 = 0;
            break;
        case 1:                                     /* switch 1 */
            lbl_802F2022 = 1;
            break;
        case 2:                                     /* switch 1 */
            lbl_802F2022 = 2;
            break;
        case 3:                                     /* switch 1 */
            lbl_802F2022 = 3;
            break;
        }
        break;
    case 6:
        temp_r6_3 = (s8) lbl_802F2021;
        switch (temp_r6_3)                          /* switch 2; irregular */
        {
        case 0:                                     /* switch 2 */
            lbl_802F2022 = 0;
            break;
        case 1:                                     /* switch 2 */
            lbl_802F2022 = 4;
            break;
        default:
			break;
        case 4:                                    /* switch 2 */
            lbl_802F2022 = 1;
            if ((s16) gameMode == 1)
            {
                lbl_802F2025 = (u8) temp_r6_3;
                if (var_r4 != 0)
                {
                    lbl_802F2023 = 0x2D;
                }
            }
            else
            {
                lbl_802F2025 = lbl_802F1BF4.unk4 + 2;
            }
            break;
        }
        break;
    case 7:
        temp_r6_4 = (s8) lbl_802F2021;
        if (temp_r6_4 != 0)
        {
            if ((temp_r6_4 >= 0) && (temp_r6_4 < 3))
            {
                lbl_802F2022 = 1;
                if ((s16) gameMode == 1)
                {
                    lbl_802F2025 = (u8) temp_r6_4;
                    if (var_r4 != 0)
                    {
                        lbl_802F2023 = 0x2D;
                    }
                }
                else
                {
                    lbl_802F2025 = lbl_802F1BF0 + 1;
                }
            }
        }
        else
        {
            lbl_802F2022 = 0;
            if (((s16) gameMode == 1) && (var_r4 != 0))
            {
                lbl_802F2023 = 0x2D;
            }
        }
        break;
    case 8:
        temp_r6_5 = (s8) lbl_802F2021;
        switch (temp_r6_5)                          /* switch 3; irregular */
        {
        case 0:                                     /* switch 3 */
            lbl_802F2022 = 0;
            break;
        case 1:                                     /* switch 3 */
            lbl_802F2022 = 4;
            break;
        default:                                    /* switch 3 */
            lbl_802F2022 = 1;
            if ((s16) gameMode == 1)
            {
                lbl_802F2025 = (u8) temp_r6_5;
                if (var_r4 != 0)
                {
                    lbl_802F2023 = 0x2D;
                }
            }
            else
            {
                lbl_802F2025 = lbl_802F1BE8.unk0 + 2;
            }
            break;
        }
        break;
    }
    if ((s8) lbl_802F2023 > 0)
    {
        lbl_802F2023 -= 1;
    }
    if ((s8) lbl_802F2023 == 0)
    {
        lbl_802F2024 = lbl_802F2021;
    }
    temp_r0_4 = arg1->userVar;
    switch (temp_r0_4)                              /* switch 4; irregular */
    {
    case 0:                                         /* switch 4 */
        lbl_802F202C = (f32) ((f64) lbl_802F202C + (0.1 * (100.0 - (f64) lbl_802F202C)));
        lbl_802F2028 = (f32) ((f64) lbl_802F2028 + (0.1 * -lbl_802F2028));
        if (lbl_802F202C > 79.0)
        {
            lbl_802F2030 = (f32) ((f64) lbl_802F2030 + (0.1 * (80.0 - (f64) lbl_802F2030)));
        }
        if (((s8) lbl_802F2022 == 0) || ((s8) lbl_802F2022 == 4))
        {
            lbl_802F2034 = (f32) ((f64) lbl_802F2034 + (0.1 * (1.0 - (f64) lbl_802F2034)));
        }
        else if ((lbl_802F2038 < 0.1) && (lbl_802F2040 < 0.1))
        {
            lbl_802F2034 = (f32) ((f64) lbl_802F2034 + (0.1 * -lbl_802F2034));
        }
        if ((s8) lbl_802F2022 == 0)
        {
            if ((lbl_802F2034 > 0.95) && (lbl_802F2040 < 0.01))
            {
                lbl_802F2038 = (f32) ((f64) lbl_802F2038 + (0.2 * (1.0 - (f64) lbl_802F2038)));
            }
            if (lbl_802F2038 > 0.99)
            {
                lbl_802F203C = (f32) ((f64) lbl_802F203C + (0.1 * (1.0 - (f64) lbl_802F203C)));
            }
        }
        else
        {
            lbl_802F203C = (f32) ((f64) lbl_802F203C + (0.3 * -lbl_802F203C));
            if (lbl_802F203C < 0.01)
            {
                lbl_802F2038 = (f32) ((f64) lbl_802F2038 + (0.3 * -lbl_802F2038));
            }
        }
        if ((s8) lbl_802F2022 == 4)
        {
            if ((lbl_802F2034 > 0.95) && (lbl_802F2038 < 0.01))
            {
                lbl_802F2040 = (f32) ((f64) lbl_802F2040 + (0.2 * (1.0 - (f64) lbl_802F2040)));
            }
            if (lbl_802F2040 > 0.99)
            {
                lbl_802F2044 = (f32) ((f64) lbl_802F2044 + (0.1 * (1.0 - (f64) lbl_802F2044)));
            }
        }
        else
        {
            lbl_802F2044 = (f32) ((f64) lbl_802F2044 + (0.3 * -lbl_802F2044));
            if (lbl_802F2044 < 0.01)
            {
                lbl_802F2040 = (f32) ((f64) lbl_802F2040 + (0.3 * -lbl_802F2040));
            }
        }
        if (((s8) lbl_802F2022 == 1) && ((s8) lbl_802F2023 == 0))
        {
            if ((lbl_802F2038 < 0.01) && (lbl_802F2040 < 0.01))
            {
                lbl_802F204C = (f32) ((f64) lbl_802F204C + (0.2 * -lbl_802F204C));
            }
            if (lbl_802F204C < 0.01)
            {
                lbl_802F2048 = (f32) ((f64) lbl_802F2048 + (0.2 * (1.0 - (f64) lbl_802F2048)));
            }
            if (lbl_802F2048 > 0.99)
            {
                lbl_802F2050 = (f32) ((f64) lbl_802F2050 + (0.1 * (1.0 - (f64) lbl_802F2050)));
            }
        }
        else
        {
            lbl_802F2050 = (f32) ((f64) lbl_802F2050 + (0.3 * -lbl_802F2050));
            if (lbl_802F2050 < 0.01)
            {
                lbl_802F2048 = (f32) ((f64) lbl_802F2048 + (0.3 * -lbl_802F2048));
            }
            if (lbl_802F2048 < 0.01)
            {
                lbl_802F204C = (f32) ((f64) lbl_802F204C + (0.1 * (1.0 - (f64) lbl_802F204C)));
            }
        }
        if ((s8) lbl_802F2022 == 2)
        {
            if ((lbl_802F2048 < 0.01) && (lbl_802F2034 < 0.1))
            {
                lbl_802F2054 = (f32) ((f64) lbl_802F2054 + (0.1 * -lbl_802F2054));
            }
        }
        else
        {
            lbl_802F2054 = (f32) ((f64) lbl_802F2054 + (0.1 * (1.0 - (f64) lbl_802F2054)));
        }
        if ((s8) lbl_802F2022 == 3)
        {
            if (lbl_802F2034 < 0.1)
            {
                lbl_802F2058 = (f32) ((f64) lbl_802F2058 + (0.1 * -lbl_802F2058));
                return;
            }
            return;
        }
        lbl_802F2058 = (f32) ((f64) lbl_802F2058 + (0.1 * (1.0 - (f64) lbl_802F2058)));
        return;
    case 1:                                         /* switch 4 */
        lbl_802F2028 = (f32) ((f64) lbl_802F2028 + (0.1 * (1.0 - (f64) lbl_802F2028)));
        break;
    }
}
