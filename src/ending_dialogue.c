#include <stdio.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "course.h"
#include "effect.h"
#include "ending.h"
#include "gma.h"
#include "gxutil.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "ord_tbl.h"

struct GameResultInfo
{
    s32 playPoints;
    s32 unk4;
    s32 unk8;
    u8 unlockedMinigames;
    u8 lockedMinigames;
    s32 canUnlockMinigame;
    s32 unk14;
    s32 unk18;
    s32 hasMaxContinues;
    u8 continues;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
};

struct Struct800C1418
{
	u32 unk0;
	void *unk4[6];
};

void get_game_results(struct GameResultInfo *arg0);
char **get_ending_dlg_lines(struct GameResultInfo *arg0, char ***arg1);
void copy_dialogue_string(struct GameResultInfo *res, struct Struct800C1418 *out, char *dest, char **lines);

void effect_ending_ballfrag_init(struct Effect *arg0)
{
    arg0->timer = 0xE10;
    arg0->cameras = 0xFFFF;
}

void effect_ending_ballfrag_main(struct Effect *arg0)
{
    Vec spC;
    float temp_f2;

    if (!(endingInfo.unk4 & 8))
    {
        mathutil_mtxA_from_rotate_y(arg0->rotY);
        mathutil_mtxA_rotate_x(arg0->rotX);
        mathutil_mtxA_rotate_z(arg0->rotZ);
        mathutil_mtxA_rigid_inv_tf_vec(&arg0->vel, &spC);
        temp_f2 = mathutil_vec_dot_prod(&arg0->unk88, &spC);
        if (temp_f2 < 0.0f)
        {
            temp_f2 *= -0.3f;
            spC.x += temp_f2 * arg0->unk88.x;
            spC.y += temp_f2 * arg0->unk88.y;
            spC.z += temp_f2 * arg0->unk88.z;
        }
        else
        {
            temp_f2 *= -0.05f;
            spC.x += temp_f2 * arg0->unk88.x;
            spC.y += temp_f2 * arg0->unk88.y;
            spC.z += temp_f2 * arg0->unk88.z;
        }
        spC.x *= 0.99f;
        spC.y *= 0.99f;
        spC.z *= 0.99f;
        mathutil_mtxA_tf_vec(&spC, &arg0->vel);
        arg0->vel.y += -0.004899996f;
        arg0->pos.x += arg0->vel.x;
        arg0->pos.y += arg0->vel.y;
        arg0->pos.z += arg0->vel.z;
        arg0->rotX = (0.96f * arg0->rotX) + (6144.0f * arg0->vel.y);
        arg0->rotY = (0.96f * arg0->rotY) + (6144.0f * arg0->vel.z);
        arg0->rotZ = (0.96f * arg0->rotZ) + (6144.0f * arg0->vel.x);
    }
}

struct MyNode
{
    struct OrdTblNode node;
    u32 unk8;
    struct Effect *effect;
};

void lbl_800C0C94(struct MyNode *);

void effect_ending_ballfrag_draw(struct Effect *effect)
{
    struct MyNode *temp_r3;
    struct OrdTblNode *temp_r30;

    mathutil_mtxA_from_mtxB();
    temp_r30 = ord_tbl_get_entry_for_pos(&effect->pos);
    temp_r3 = ord_tbl_alloc_node(0x10U);
    temp_r3->node.drawFunc = (OrdTblDrawFunc)lbl_800C0C94;
    temp_r3->unk8 = peek_light_group();
    temp_r3->effect = effect;
    ord_tbl_insert_node(temp_r30, &temp_r3->node);
}

void lbl_800C0C94(struct MyNode *node)
{
    struct Effect *effect;
    struct GMAModelEntry *modelEntries;

    load_light_group_cached(node->unk8);
    effect = node->effect;
    mathutil_mtxA_from_mtxB_translate(&effect->pos);
    mathutil_mtxA_rotate_y(effect->rotY);
    mathutil_mtxA_rotate_x(effect->rotX);
    mathutil_mtxA_rotate_z(effect->rotZ);
    u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
    modelEntries = commonGma->modelEntries;
    if (effect->unkA0 < 0)
    {
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_culled_sort_none(modelEntries[clearHemisphereInsideParts[0]].model);
        avdisp_set_z_mode(1, 3, 1);
        avdisp_draw_model_culled_sort_none(modelEntries[clearHemisphereOutsideParts[0]].model);
    }
    else
    {
        s16 *r30 = coloredBallPartModelIDs[effect->unkA0];

        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_none(modelEntries[r30[0]].model);
        avdisp_draw_model_unculled_sort_none(modelEntries[r30[6]].model);
        avdisp_set_z_mode(1, 3, 1);
        avdisp_draw_model_unculled_sort_none(modelEntries[r30[3]].model);
    }
}

void effect_ending_ballfrag_destroy(struct Effect *effect) {}

void func_800C0DC0(s32 *arg0, char *arg1)
{
    struct GameResultInfo sp10;
    char **r3;

    get_game_results(&sp10);
    r3 = get_ending_dlg_lines(&sp10, NULL);
    copy_dialogue_string(&sp10, (void *)arg0, arg1, r3);
}

#pragma force_active on

// AiAi Dialogue

char *lbl_801E32B8_jp[] =
{
    "h/KONNDOHA SARANI",
    "h/UENOk/REBERUh/DE ASONNDENE",
    NULL,
};

char *lbl_801E32B8[] =
{
    "Try a higher level",
    "next time",
    NULL,
};

char *lbl_801E3330_jp[] =
{
    "h/KOREDE k/KIMIh/MO",
    "h/ICHININNMAENO Ok/SARUh/SANNDAYO",
    NULL,
};

char *lbl_801E3330[] =
{
    "Now you've become",
    "cool monkey like me",
    NULL,
};

char *lbl_801E33D0_jp[] =
{
    "k/KIMIh/TTEk/SUGOIh/NE! KOREKARAMO",
    "k/MONNKI-BO-RUh/WO YOROSIKU!",
    NULL,
};

char *lbl_801E33D0[] =
{
    "You are so cool! Remember,",
    "Monkey Ball is the best game for you!",
    NULL,
};

char *lbl_801E3518_jp[] =
{
    "h/KOKOMADE, k/PUREIh/DEKITAk/KIMIh/HAk/SUGOIh/YO!",
    "h/KOKOMADENOk/PUREIh/, OTUKARESAMADESITA!",
    "p/TSUGI/h/NIk/PUREIh/SURUTOKIHA, SAIGOMADE",
    "h/IKERUYOUNI, GANNBATTEMITENE!",
    NULL,
};

char *lbl_801E3518[] =
{
    "Wow! You've made it this far!",
    "You're incredible!",
    "The next time you play,",
    "let's try to make it",
    "all the way to the end!",
    NULL,
};

char *lbl_801E3614_jp[] =
{
    "h/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUh/WOk/GETTOh/DEKIRUYO!",
    NULL,
};

char *lbl_801E3614[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Only %n left to",
    "unlock the next Mini Game!",
    NULL,
};

char *lbl_801E36E8_jp[] =
{
    "h/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENNBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUYO!",
    NULL,
};

char *lbl_801E36E8[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock a Mini Game!",
    NULL,
};

char *lbl_801E3784_jp[] =
{
    "%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "h/SAIGONOk/MINIGE-MUh/WOk/GETTOh/SHITENE!",
    NULL,
};

char *lbl_801E3784[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock the last Mini Game!",
    NULL,
};

char *lbl_801E3820_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENNBUDE%tk/POINNTOh/DANE!",
    "h/ATO%nk/POINNTOh/DE",
    "h/k/KONNTEXINYU-h/p/KAISUUh/WOHUYASERUYO!",
    NULL,
};

char *lbl_801E3820[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Only %n left to",
    "increase your number of continues!",
    NULL,
};

char *lbl_801E38AC_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "k/KONNTEXINYU-p/KAISUU/h/GA%cp/KAIsuu/h/NINATTAYO!",
    NULL,
};

char *lbl_801E38AC[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you increase your number",
    "of continues to %c!",
    NULL,
};

char *lbl_801E3948_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "h/KOREDEk/KONNTEXINYU-h/GAp/NANNKAI/h/DEMO",
    "h/DEKIRUYOUNINATTAYO!",
    NULL,
};

char *lbl_801E3948[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Now, you can play with",
    "infinite continues!",
    NULL,
};

char *lbl_801E3A58_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/KIMIh/HA, SUBETENOMONOWOk/GETTOh/SHITEIRUYO!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/GETTOh/DEKIRU",
    "k/PUREIPOINNTOh/KIROKUNIk/TYARENJIh/DAYO!",
    NULL,
};

char *lbl_801E3A58[] =
{
    "You got %p!",
    "You've unlocked everything!",
    "Now, you can try to set the",
    "high score for the most Play Points!",
    NULL,
};

char *lbl_801E3B34_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/SAIKO-h/HA, %bk/POINNTOh/DANE!",
    "k/SAIKO-h/KIROKUWOKOERARERUYOUNI",
    "h/KOREKARAMO, GANBATTENE!",
    NULL,
};

char *lbl_801E3B34[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best to try and beat",
    "the high score, okay?",
    NULL,
};

char *lbl_801E3C14_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/KORETTE, k/SAIKO-h/KIROKUDAYO!",
    "k/YATTAh/NE! KOREKARAMO, SARANIUENO",
    "h/KIROKUWOMEZASITE, GANBATTENE!",
    NULL,
};

char *lbl_801E3C14[] =
{
    "You got %p!",
    "That's a new high score!",
    "Congratulations!",
    "That was truly a great game!",
    NULL,
};

char *lbl_801E3CE4_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/KOREYORIk/SUGOIh/KIROKUHANAIYO!",
    "k/KIMIh/HA, k/KANPEKIh/DANE!",
    "h/TYO-k/MIRAKURUh/NAk/PUREIh/DATTAYO!",
    NULL,
};

char *lbl_801E3CE4[] =
{
    "You got %p!",
    "There's no record better than this!",
    "You are the greatest!",
    "That was truly a great game!",
    NULL,
};

char *lbl_801E3D7C_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/SAIKO-h/HA, %bk/POINNTOh/DANE!",
    "h/MOUITIDO, %bk/POINNTOh/DASERUKA",
    "k/TYARENJIh/SHITEMITENE!",
    NULL,
};

char *lbl_801E3D7C[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get %b!",
    NULL,
};

char *lbl_801E3DD0_jp[] =
{
    "h/KONNDOHA MOTTO",
    "h/UENOk/REBERUh/WO MEZASHITENE",
    NULL,
};

// MeeMee Dialogue

char *lbl_801E3DD0[] =
{
    "Try a higher level",
    "next time",
    NULL,
};

char *lbl_801E3E34_jp[] =
{
    "h/ANATAMO KOREDE",
    "h/ICHININNMAENO Ok/SARUh/SANNNE",
    NULL,
};

char *lbl_801E3E34[] =
{
    "Now you've become",
    "pretty monkey like me",
    NULL,
};

char *lbl_801E3EA8_jp[] =
{
    "h/ANATATTEk/SUGOIh/WA! KOREKARAMO",
    "k/PUREI h/SHIMAKUTTENE",
    NULL,
};

char *lbl_801E3EA8[] =
{
    "You are so fantastic! Remember,",
    "Monkey Ball is the best game for you!",
    NULL,
};

char *lbl_801E3FE8_jp[] =
{
    "h/KOKOMADEk/PUREIh/DEKITAANATAHA, k/ERAIh/WA!",
    "p/JIBUNN/h/DEp/JIBUNN/h/WOHOMETEAGETEMOIIKAMO!",
    "p/IMA/p/KAIsuu/h/HAAKIRAMETAKEDO, p/TSUGI/h/NOk/PUREIh/DEHA",
    "k/KURIAh/DEKIRUYOUNI, GANNBATTENE!",
    NULL,
};

char *lbl_801E3FE8[] =
{
    "Wow! You've made it this far!",
    "You really are something!",
    "I guess you should take a break.",
    "Next time, let's try to make it",
    "all the way to the end!",
    NULL,
};

char *lbl_801E40D8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUh/GAk/GETTOh/DEKITYATTARISHITE.",
    NULL,
};

char *lbl_801E40D8[] =
{
    "You got %p!",
    "All together, that's %t!",
    "You might be able to unlock a game",
    "with %n_ more points!",
    NULL,
};

char *lbl_801E41A8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUWAYO!",
    NULL,
};

char *lbl_801E41A8[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Since you got a whole lot more points,",
    "you can unlock any Mini Game",
    "you want!",
    NULL,
};

char *lbl_801E4234_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "h/SAIGONOk/MINIGE-MUh/MOk/GETTOh/DEKIRUWAYO!",
    NULL,
};

char *lbl_801E4234[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Since you got a whole lot more points,",
    "you'll be able to unlock",
    "the last Mini Game!",
    NULL,
};

char *lbl_801E42D8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/ATO%nk/POINNTOh/DE",
    "k/KONNTEXINYU-p/KAISUU/h/WOHUYASERUWAYO!",
    NULL,
};

char *lbl_801E42D8[] =
{
    "You got %p!",
    "All together, that's %t!",
    "You can increase",
    "your number of continues",
    "if you get %n_ more points!",
    NULL,
};

char *lbl_801E4374_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "k/KONNTEXINYU-p/KAISUU/h/GA%cp/KAIsuu/h/NINATTAWA!",
    NULL,
};

char *lbl_801E4374[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Since you got a whole lot more points,",
    "you'll be able to increase",
    "your number of continues to %c!",
    NULL,
};

char *lbl_801E43B8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/KOREDEk/KONNTEXINYU-h/GAp/NANNKAI/h/DEMO",
    "h/DEKIRUYOUNINATTAWAYO!",
    NULL,
};

char *lbl_801E43B8[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Now, you can play with",
    "infinite continues!",
    NULL,
};

char *lbl_801E44A4_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ANATANIAGERARERUMONOHA, MOUNAIWA!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/PUREIPOINNTOh/WO",
    "h/DOREDAKETORERUKANI, TYOUSENNYO!",
    NULL,
};

char *lbl_801E44A4[] =
{
    "You got %p!",
    "You've unlocked everything!",
    "Now, you can try to see how many",
    "points you can get in 1 player mode!",
    NULL,
};

char *lbl_801E4558_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "k/SAIKO-h/HA, %bk/POINNTOh/YO!",
    "k/SAIKO-h/KIROKUWONURIKAERARERUYOUNI",
    "h/KOREKARAMO, ASOBITUDUKETENE!",
    NULL,
};

char *lbl_801E4558[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best and play more to beat",
    "the high score, okay?",
    NULL,
};

char *lbl_801E4640_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KOREHA, k/SAIKO-h/KIROKUDAWA!",
    "k/SUGGO~I! h/KOREKARAMO",
    "h/UENOKIROKUWOMEZASHITE, GANBATTENE!",
    NULL,
};

char *lbl_801E4640[] =
{
    "You got %p!",
    "You got the high score!",
    "Wow! I can't believe it!",
    "Now just try to beat",
    "your own record!",
    NULL,
};

char *lbl_801E46F8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KOREIJYOUNOKIROKUHA, DENAIWAYO!",
    "h/ANATATTE, k/PA-FEKUTOh/NE!",
    "h/MOU, k/SUTEKIp/./k/TOKIMEKIp/./k/DA~ISUKIh/YO!",
    NULL,
};

char *lbl_801E46F8[] =
{
    "You got %p!",
    "There's no record better than this!",
    "You are perfect!",
    "You are the greatest!",
    NULL,
};

char *lbl_801E47A4_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KIROKUHA,%bk/POINNTOh/YO!",
    "h/%bk/POINNTOh/TOIUk/SAIKO-h/KIROKUWO",
    "h/MATADASERUKA, TYOUSENNSITENE!",
    NULL,
};

char *lbl_801E47A4[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get the high score",
    "of %b!",
    NULL,
};

char *lbl_801E4814_jp[] =
{
    "h/MOTTO UENOk/REBERUh/DE",
    "h/ASOBITAIDEk/CHU",
    NULL,
};

// Baby Dialogue

char *lbl_801E4814[] =
{
    "I want to play higher level",
    "next time",
    NULL,
};

char *lbl_801E487C_jp[] =
{
    "h/KOREDE RIPPANA Ok/SARUh/SANNNO",
    "h/NAKAMAIRIDEk/CHU",
    NULL,
};

char *lbl_801E487C[] =
{
    "Now you've become",
    "grown up monkey like me",
    NULL,
};

char *lbl_801E48F4_jp[] =
{
    "k/SUGOIh/DEk/CHU! h/KOREKARAMO",
    "h/ZUTTO ASONNDEHOSIIDEk/CHU",
    NULL,
};

char *lbl_801E48F4[] =
{
    "You are such grown up! Remember,",
    "Monkey Ball is the best game for you!",
    NULL,
};

char *lbl_801E4A10_jp[] =
{
    "h/KONOk/SUTE-JIh/MADEk/PUREIh/DEKIRUNANNTE",
    "k/SUGOIh/DEk/TYU! ODOROKIh/DEk/TYU!",
    "h/TOTTEMOk/MUZUKAh/TIIDEk/TYUh/KEDO",
    "h/TUGIHAk/SAIGOh/MADEITTEHOTIIDEk/TYU!",
    NULL,
};

char *lbl_801E4A10[] =
{
    "Wow! You've made it this far!",
    "You are weally good!",
    "It's a wittle hard, but let's try",
    "to make it all da way to da end!",
    NULL,
};

char *lbl_801E4AE4_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUGETTOh/DEk/TYU! BABU~!",
    NULL,
};

char *lbl_801E4AE4[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Only %n weft to",
    "unwock the next Mini Game!",
    NULL,
};

char *lbl_801E4B6C_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUDEk/TYU!",
    NULL,
};

char *lbl_801E4B6C[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock a Mini Game!",
    NULL,
};

char *lbl_801E4C28_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "k/MINIGE-MUh/WOZENNBUk/GETTOh/DEKIRUDEk/TYU!",
    NULL,
};

char *lbl_801E4C28[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Since you got a wot of points,",
    "you can unwock the last Mini Game!",
    NULL,
};

char *lbl_801E4CB4_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/ATO%nk/POINNTOh/DE",
    "k/KONNTEXINYU-h/WOHUYASERUDEk/TYU!",
    NULL,
};

char *lbl_801E4CB4[] =
{
    "You got %p!",
    "All togedder, you have %p!",
    "Only %n weft to",
    "incwease your number of continues!",
    NULL,
};

char *lbl_801E4D34_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "k/KONNTEXINYU-h/GA%ch/KAININATTADEk/TYU!",
    NULL,
};

char *lbl_801E4D34[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Since you got a wot of points,",
    "you incwease your number",
    "of continues!",
    NULL,
};

char *lbl_801E4DBC_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/NANNKAIDEMOk/KONNTEXINYU-h/GA",
    "h/DEKIRUYOUNINATTADEk/TYU! BABU~!",
    NULL,
};

char *lbl_801E4DBC[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Now, you can pway with",
    "infinite continues!",
    NULL,
};

char *lbl_801E4E5C_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/MOU, k/PUREZENNTOh/SURUMONOHANAIDEk/TYU!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/PUREIPOINNTOh/WO",
    "h/IKUTUk/GETTOh/SURUKANIk/TYARENJIh/DEk/TYU!",
    NULL,
};

char *lbl_801E4E5C[] =
{
    "You got %p!",
    "You've unwocked everyting!",
    "Now, you can try to set the",
    "high score for the most Play Points!",
    NULL,
};

char *lbl_801E4F04_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "k/SAIKO-h/HA%bk/POINNTOh/DEk/TYU!",
    "k/SAIKO-h/KIROKU, MEZATITE",
    "h/KOREKARAMO, k/PUREIh/SURUDEk/TYU!",
    NULL,
};

char *lbl_801E4F04[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best to try and beat",
    "the high score!! Let's go!!",
    NULL,
};

char *lbl_801E4FCC_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/KOREHA, k/SAIKO-h/KIROKUDEk/TYU!",
    "h/YATTADEk/TYU! h/URETIIDEk/TYU!",
    "k/SUGOIh/DEk/TYU! UKIUKIh/DEk/TYU!",
    NULL,
};

char *lbl_801E4FCC[] =
{
    "You got %p!",
    "That's a new high score!",
    "Congwatuwations!",
    "That was truly a gweat game!",
    NULL,
};

char *lbl_801E5078_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "k/PA-FEKUTOh/NAKIROKUDEk/TYU!",
    "h/KOREYORIk/SUGOIh/KIROKUHA",
    "h/MOUNAIDEk/TYU! ODOROKIh/DEk/TYU!",
    NULL,
};

char *lbl_801E5078[] =
{
    "You got %p!",
    "There's no wecord better than this!",
    "You are the greatest!",
    "That was truly a gweat game!",
    NULL,
};

char *lbl_801E5134_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "k/SAIKO-h/HA,%bk/POINNTOh/DEk/TYU!",
    "h/%bk/POINNTOh/WOMATAk/GETTOh/DEKIRUKA",
    "k/TYARENNJIh/TITEHOSIIDEk/TYU! BABU~!",
    NULL,
};

char *lbl_801E5134[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get %b! Goo goo!!!",
    NULL,
};

char *lbl_801E51D8_jp[] =
{
    "k/SAIKYO~NOSARU MEZASHITE MOTTO",
    "k/UENO REBERUDE ASONDEKURE~!",
    NULL,
};

// GonGon dialogue

char *lbl_801E51D8[] =
{
    "Try to be the greatest monkey ever!",
    "Play the more difficult levels!",
    NULL,
};

char *lbl_801E5298_jp[] =
{
    "k/SUGOIJYANE~KA! ODOROKIDAZE!",
    "k/DAXA~KEDO UEHA MADA ARUZE!",
    NULL,
};

char *lbl_801E5298[] =
{
    "Wow! You're really good!",
    "I'm surprised!",
    "Buuuut, there's still a more",
    "difficult level to beat!",
    NULL,
};

char *lbl_801E5334_jp[] =
{
    "k/UHOUHO UHOHO~I! YATTA~!",
    "k/KOREDE SAIKYO~NOSARU DAZE~!",
    NULL,
};

char *lbl_801E5334[] =
{
    "Oh wow!",
    "I can't believe it!",
    "You really are the greatest",
    "monkey ever!!",
    NULL,
};

char *lbl_801E5468_jp[] =
{
    "k/KONOSUTE~JIMADE KORETA OMAEHA",
    "k/KANARINO UDEMAEDA~XTU! UHOUHO!",
    "k/TUGINI PUREISURUTOKIHA AKIRAMEZUNI",
    "k/ZENNBU KURIA DEKIRUYOUNI GANBARE!",
    NULL,
};

char *lbl_801E5468[] =
{
    "You really are something to have",
    "made it this far! Congratulations!!",
    "Next time you play,",
    "let's take it all the way!!!",
    NULL,
};

char *lbl_801E552C_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/ATO %nk/POINNTO DE",
    "k/MINIGEXE~MU GETTOXO~XTU! UHOHO!",
    NULL,
};

char *lbl_801E552C[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Only %n left to",
    "unlock the next Mini Game!",
    "Go go go!!!",
    NULL,
};

char *lbl_801E55BC_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/MINIGE~MU GETTOXO~XTU! DAXA~!",
    NULL,
};

char *lbl_801E55BC[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock a Mini Game! Yeah!!!",
    NULL,
};

char *lbl_801E5634_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/MINIGEXE~MU ZE~NNBU GETTOXO~XTU!",
    NULL,
};

char *lbl_801E5634[] =
{
    "You got %p!",
    "All together, you have %t",
    "Since you got a lot of points,",
    "you can unlock the last Mini Game!",
    "Go for it!!!",
    NULL,
};

char *lbl_801E5680_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/ATO %nk/POINNTO DE",
    "k/KONNTEXINYU~ HUE~RU! UHOUHO!",
    NULL,
};

char *lbl_801E5680[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Only %n left to",
    "increase your number of continues!",
    NULL,
};

char *lbl_801E56D0_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/KONNTEXINYU~ %ck/ KAINI HUETA~XTU!",
    NULL,
};

char *lbl_801E56D0[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you increase your number",
    "of continues to %c!",
    NULL,
};

char *lbl_801E5760_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/DAXAKA~RA KONNTEXINYU~ NANNDOMO",
    "k/DEKIRUYOUNI NATTA~! NATTA~!",
    NULL,
};

char *lbl_801E5760[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Now, you can play with",
    "infinite continues! Good job!!",
    NULL,
};

char *lbl_801E5838_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/MOU OMAENI YARERUMONO NAI!",
    "k/KOREKARAHA 1PUREIDE PUREIPOINNTO",
    "k/NANNTENN GETTODEKIRUKA TYARENNJI!",
    NULL,
};

char *lbl_801E5838[] =
{
    "You got %p!",
    "You've unlocked everything! Wow!!!",
    "Now, you can try to set the",
    "high score for most Play Points!",
    NULL,
};

char *lbl_801E58E8_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~%bk/POINNTO! ZANNNENN!",
    "k/KOREKARAMO SAIKO~KIROKU MEZASITE",
    "k/PUREI SURUNO DA~XTU! UHOHO~I!",
    NULL,
};

char *lbl_801E58E8[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best to try and beat",
    "the high score! You can do it!!!",
    NULL,
};

char *lbl_801E5998_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~KIROKU DETTA~! DETTA~!",
    "k/UHO UHOHO! UHOUHO UHOHO~I!",
    "k/YATTA~ YATTA~! YATTA~GO~NN!",
    NULL,
};

char *lbl_801E5998[] =
{
    "You got %p!",
    "That's a new high score!",
    "Congratulations!",
    "You really are the best!!! Woo hoo!!!",
    NULL,
};

char *lbl_801E5A54_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/KO KOREHA ODOROKINO DA~IKIROKU!",
    "k/KOREIJYO~ SUGOI KIROKU DENAI!",
    "k/OMAE TENNSA~I SUGOIZOXO~XTU!",
    NULL,
};

char *lbl_801E5A54[] =
{
    "You got %p!",
    "There's no record better than this!",
    "I can't believe it!!",
    "You must be a genius!!!",
    NULL,
};

char *lbl_801E5B08_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~%bk/POINNTO! ZANNNENN",
    "k/%bk/POINNTO MATA DASERUYO~NI",
    "k/GANNBARE~XTU! 1p/./k/2p/./k/3p/./k/DAXA~XTU!",
    NULL,
};

char *lbl_801E5B08[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get %b!",
    "Readyp/TENNTENN/a/ Go!!!",
    NULL,
};
#pragma force_active reset

char **aiaiDialogue[] =
{
    lbl_801E32B8,
    lbl_801E3330,
    lbl_801E33D0,
    lbl_801E3518,
    lbl_801E3614,
    lbl_801E36E8,
    lbl_801E3784,
    lbl_801E3820,
    lbl_801E38AC,
    lbl_801E3948,
    lbl_801E3A58,
    lbl_801E3B34,
    lbl_801E3C14,
    lbl_801E3CE4,
    lbl_801E3D7C,
};

char **meemeeDialogue[] =
{
    lbl_801E3DD0,
    lbl_801E3E34,
    lbl_801E3EA8,
    lbl_801E3FE8,
    lbl_801E40D8,
    lbl_801E41A8,
    lbl_801E4234,
    lbl_801E42D8,
    lbl_801E4374,
    lbl_801E43B8,
    lbl_801E44A4,
    lbl_801E4558,
    lbl_801E4640,
    lbl_801E46F8,
    lbl_801E47A4,
};

char **babyDialogue[] =
{
    lbl_801E4814,
    lbl_801E487C,
    lbl_801E48F4,
    lbl_801E4A10,
    lbl_801E4AE4,
    lbl_801E4B6C,
    lbl_801E4C28,
    lbl_801E4CB4,
    lbl_801E4D34,
    lbl_801E4DBC,
    lbl_801E4E5C,
    lbl_801E4F04,
    lbl_801E4FCC,
    lbl_801E5078,
    lbl_801E5134,
};

char **gongonDialogue[] =
{
    lbl_801E51D8,
    lbl_801E5298,
    lbl_801E5334,
    lbl_801E5468,
    lbl_801E552C,
    lbl_801E55BC,
    lbl_801E5634,
    lbl_801E5680,
    lbl_801E56D0,
    lbl_801E5760,
    lbl_801E5838,
    lbl_801E58E8,
    lbl_801E5998,
    lbl_801E5A54,
    lbl_801E5B08,
};

char ***characterDialogues[] =
{
    aiaiDialogue,
    meemeeDialogue,
    babyDialogue,
    gongonDialogue,
};

void func_800C0E1C(s32 index, void *arg1, char *arg2)
{
    struct GameResultInfo result;
    char ***charaDlg;
    char **lines;

    get_game_results(&result);
    result.playPoints = 1111;
    result.unk4 = 2222;
    result.unk24 = 3333;
    result.continues = 4;
    result.unk30 = 5555;
    charaDlg = characterDialogues[playerCharacterSelection[modeCtrl.currPlayer]];
    switch (index % 15)
    {
    case 0: lines = charaDlg[0]; break;
    case 1: lines = charaDlg[1]; break;
    case 2: lines = charaDlg[2]; break;
    case 3: lines = charaDlg[3]; break;
    case 4: lines = charaDlg[4]; break;
    case 5: lines = charaDlg[5]; break;
    case 6: lines = charaDlg[6]; break;
    case 7: lines = charaDlg[7]; break;
    case 8: lines = charaDlg[8]; break;
    case 9: lines = charaDlg[9]; break;
    case 10: lines = charaDlg[10]; break;
    case 11: lines = charaDlg[11]; break;
    case 12: lines = charaDlg[12]; break;
    case 13: lines = charaDlg[13]; break;
    case 14: lines = charaDlg[14]; break;
    }
    copy_dialogue_string(&result, arg1, arg2, lines);
}

void get_game_results(struct GameResultInfo *out)
{
    struct GameResultInfo result;

    memset(&result, 0, sizeof(result));
    result.unlockedMinigames = 0;
    if (is_minigame_unlocked(GAMETYPE_MINI_BILLIARDS))
        result.unlockedMinigames++;
    if (is_minigame_unlocked(GAMETYPE_MINI_BOWLING))
        result.unlockedMinigames++;
    if (is_minigame_unlocked(GAMETYPE_MINI_GOLF))
        result.unlockedMinigames++;
    result.playPoints = g_playPointsEarned;
    result.unk4 = g_totalPlayPoints;
    result.unk8 = g_totalPlayPoints;
    result.lockedMinigames   = 3 - result.unlockedMinigames;
    result.canUnlockMinigame = (result.unk8 > 2500);
    result.unk14 = g_unlockFlags & 8;
    result.unk18 = (result.unk8 > 2500);
    result.continues = (u8)get_max_continues();
    {
        int i;
        int playPoints = result.unk8;
        for (i = 2500; i < playPoints; i += 2500)
            result.continues++;
    }
    result.hasMaxContinues = (result.continues > 9);
    result.unk24 = 2500 - result.unk8;
    result.unk28 = (result.unk14 != 0 && result.lockedMinigames == 0);
    result.unk2C = !(lbl_802F22C8 & 1);
    result.unk30 = g_maxPlayPointRecord;
    result.unk34 = playPointYieldPerDifficulty[modeCtrl.difficulty];
    *out = result;
}

char **get_ending_dlg_lines(struct GameResultInfo *arg0, char ***out)
{
    struct GameResultInfo sp10 = *arg0;
    char ***charaDlg = characterDialogues[playerCharacterSelection[modeCtrl.currPlayer]];
    char **lines;

    if (modeCtrl.playerCount > 1)
    {
        switch (modeCtrl.difficulty)
        {
        case DIFFICULTY_BEGINNER:
            lines = charaDlg[0];
            break;
        case DIFFICULTY_ADVANCED:
            lines = charaDlg[1];
            break;
        default:
        case DIFFICULTY_EXPERT:
            lines = charaDlg[2];
            break;
        }
    }
    else if (modeCtrl.courseFlags & COURSE_FLAG_FAILED_EXTRA)
        lines = charaDlg[3];
    else if (sp10.lockedMinigames != 0)
    {
        if (sp10.canUnlockMinigame == 0)
            lines = charaDlg[4];
        else if (sp10.lockedMinigames > 1)
            lines = charaDlg[5];
        else
            lines = charaDlg[6];
    }
    else if (sp10.unk14 == 0)
    {
        if (sp10.unk18 == 0)
            lines = charaDlg[7];
        else if (!sp10.hasMaxContinues)
            lines = charaDlg[8];
        else
            lines = charaDlg[9];
    }
    else if (sp10.unk2C != 0)
    {
        lines = charaDlg[10];
        lbl_802F22C8 |= 1;
    }
    else if (sp10.playPoints >= sp10.unk30)
    {
        if (sp10.playPoints >= sp10.unk34)
            lines = charaDlg[13];
        else
            lines = charaDlg[12];
    }
    else if (sp10.unk30 >= sp10.unk34)
        lines = charaDlg[14];
    else
        lines = charaDlg[11];
    if (out != NULL)
        *out = lines;
    return lines;
}

void copy_dialogue_string(struct GameResultInfo *res, struct Struct800C1418 *out, char *dest, char **lines)
{
	static char *pointsTexts[] = { " point", " points", "" };
	struct Struct800C1418 sp50;
	struct GameResultInfo result;
	char **pSrcLine;
	char *src;
	char *dst;
	char *p;

	result = *res;
	memset(&sp50, 0, sizeof(sp50));
	pSrcLine = lines;
	dst = dest;

	while (*pSrcLine != NULL)
	{
		sp50.unk4[sp50.unk0] = dst;
		src = *pSrcLine;
		while (*src != 0)
		{
			switch (*src)
			{
			case '%':
				switch (*++src)
				{
				case 'p':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", result.playPoints, (result.playPoints != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				case 't':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", result.unk4, (result.unk4 != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				case 'n':
					if (result.unk24 != 1)
						p = pointsTexts[1];
					else
						p = pointsTexts[0];
					if (src[1] == '_')
					{
						p = pointsTexts[2];
						src++;
					}
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", result.unk24, p);
					break;
				case 'c':
					sprintf(dst, "c/0xff0000/%dc/0x000000/", result.continues);
					break;
				case 'b':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", result.unk30, (result.unk30 != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				case 'm':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", result.unk34, (result.unk34 != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				default:
					*dst = 0;
					break;
				}
				src++;
				while (*dst != 0)
					dst++;
				break;  
			default:
				*dst = *src;
				src++;
				dst++;
				break;
			}
		}
		*dst = 0;
		dst++;
		pSrcLine++;
		sp50.unk0++;
	}
	*out = sp50;
}
