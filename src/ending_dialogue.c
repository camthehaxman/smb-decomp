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

void effect_ending_ballfrag_init(struct Effect *effect)
{
    effect->timer = 0xE10;
    effect->cameras = 0xFFFF;
}

void effect_ending_ballfrag_main(struct Effect *effect)
{
    Vec spC;
    float temp_f2;

    if (!(endingInfo.unk4 & 8))
    {
        mathutil_mtxA_from_rotate_y(effect->rotY);
        mathutil_mtxA_rotate_x(effect->rotX);
        mathutil_mtxA_rotate_z(effect->rotZ);
        mathutil_mtxA_rigid_inv_tf_vec(&effect->vel, &spC);
        temp_f2 = mathutil_vec_dot_prod(&effect->unk88, &spC);
        if (temp_f2 < 0.0f)
        {
            temp_f2 *= -0.3f;
            spC.x += temp_f2 * effect->unk88.x;
            spC.y += temp_f2 * effect->unk88.y;
            spC.z += temp_f2 * effect->unk88.z;
        }
        else
        {
            temp_f2 *= -0.05f;
            spC.x += temp_f2 * effect->unk88.x;
            spC.y += temp_f2 * effect->unk88.y;
            spC.z += temp_f2 * effect->unk88.z;
        }
        spC.x *= 0.99f;
        spC.y *= 0.99f;
        spC.z *= 0.99f;
        mathutil_mtxA_tf_vec(&spC, &effect->vel);
        effect->vel.y += -0.004899996f;
        effect->pos.x += effect->vel.x;
        effect->pos.y += effect->vel.y;
        effect->pos.z += effect->vel.z;
        effect->rotX = (0.96f * effect->rotX) + (6144.0f * effect->vel.y);
        effect->rotY = (0.96f * effect->rotY) + (6144.0f * effect->vel.z);
        effect->rotZ = (0.96f * effect->rotZ) + (6144.0f * effect->vel.x);
    }
}

struct MyNode
{
    struct OrdTblNode node;
    u32 lightGroup;
    struct Effect *effect;
};

static void ballfrag_draw_func(struct MyNode *);

void effect_ending_ballfrag_draw(struct Effect *effect)
{
    struct MyNode *node;
    struct OrdTblNode *entry;

    mathutil_mtxA_from_mtxB();
    entry = ord_tbl_get_entry_for_pos(&effect->pos);
    node = ord_tbl_alloc_node(0x10U);
    node->node.drawFunc = (OrdTblDrawFunc)ballfrag_draw_func;
    node->lightGroup = peek_light_group();
    node->effect = effect;
    ord_tbl_insert_node(entry, &node->node);
}

static void ballfrag_draw_func(struct MyNode *node)
{
    struct Effect *effect;
    struct GMAModelEntry *modelEntries;

    load_light_group_cached(node->lightGroup);
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

// AiAi Dialogue

static char *aiaiDialogue0_jp[] =
{
    "h/KONNDOHA SARANI",
    "h/UENOk/REBERUh/DE ASONNDENE",
    NULL,
};

static char *aiaiDialogue0_en[] =
{
    "Try a higher level",
    "next time",
    NULL,
};

static char *aiaiDialogue1_jp[] =
{
    "h/KOREDE k/KIMIh/MO",
    "h/ICHININNMAENO Ok/SARUh/SANNDAYO",
    NULL,
};

static char *aiaiDialogue1_en[] =
{
    "Now you've become",
    "cool monkey like me",
    NULL,
};

static char *aiaiDialogue2_jp[] =
{
    "k/KIMIh/TTEk/SUGOIh/NE! KOREKARAMO",
    "k/MONNKI-BO-RUh/WO YOROSIKU!",
    NULL,
};

static char *aiaiDialogue2_en[] =
{
    "You are so cool! Remember,",
    "Monkey Ball is the best game for you!",
    NULL,
};

static char *aiaiDialogue3_jp[] =
{
    "h/KOKOMADE, k/PUREIh/DEKITAk/KIMIh/HAk/SUGOIh/YO!",
    "h/KOKOMADENOk/PUREIh/, OTUKARESAMADESITA!",
    "p/TSUGI/h/NIk/PUREIh/SURUTOKIHA, SAIGOMADE",
    "h/IKERUYOUNI, GANNBATTEMITENE!",
    NULL,
};

static char *aiaiDialogue3_en[] =
{
    "Wow! You've made it this far!",
    "You're incredible!",
    "The next time you play,",
    "let's try to make it",
    "all the way to the end!",
    NULL,
};

static char *aiaiDialogue4_jp[] =
{
    "h/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUh/WOk/GETTOh/DEKIRUYO!",
    NULL,
};

static char *aiaiDialogue4_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Only %n left to",
    "unlock the next Mini Game!",
    NULL,
};

static char *aiaiDialogue5_jp[] =
{
    "h/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENNBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUYO!",
    NULL,
};

static char *aiaiDialogue5_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock a Mini Game!",
    NULL,
};

static char *aiaiDialogue6_jp[] =
{
    "%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "h/SAIGONOk/MINIGE-MUh/WOk/GETTOh/SHITENE!",
    NULL,
};

static char *aiaiDialogue6_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock the last Mini Game!",
    NULL,
};

static char *aiaiDialogue7_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENNBUDE%tk/POINNTOh/DANE!",
    "h/ATO%nk/POINNTOh/DE",
    "h/k/KONNTEXINYU-h/p/KAISUUh/WOHUYASERUYO!",
    NULL,
};

static char *aiaiDialogue7_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Only %n left to",
    "increase your number of continues!",
    NULL,
};

static char *aiaiDialogue8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "k/KONNTEXINYU-p/KAISUU/h/GA%cp/KAIsuu/h/NINATTAYO!",
    NULL,
};

static char *aiaiDialogue8_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you increase your number",
    "of continues to %c!",
    NULL,
};

static char *aiaiDialogue9_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "h/KOREDEk/KONNTEXINYU-h/GAp/NANNKAI/h/DEMO",
    "h/DEKIRUYOUNINATTAYO!",
    NULL,
};

static char *aiaiDialogue9_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Now, you can play with",
    "infinite continues!",
    NULL,
};

static char *aiaiDialogue10_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/KIMIh/HA, SUBETENOMONOWOk/GETTOh/SHITEIRUYO!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/GETTOh/DEKIRU",
    "k/PUREIPOINNTOh/KIROKUNIk/TYARENJIh/DAYO!",
    NULL,
};

static char *aiaiDialogue10_en[] =
{
    "You got %p!",
    "You've unlocked everything!",
    "Now, you can try to set the",
    "high score for the most Play Points!",
    NULL,
};

static char *aiaiDialogue11_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/SAIKO-h/HA, %bk/POINNTOh/DANE!",
    "k/SAIKO-h/KIROKUWOKOERARERUYOUNI",
    "h/KOREKARAMO, GANBATTENE!",
    NULL,
};

static char *aiaiDialogue11_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best to try and beat",
    "the high score, okay?",
    NULL,
};

static char *aiaiDialogue12_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/KORETTE, k/SAIKO-h/KIROKUDAYO!",
    "k/YATTAh/NE! KOREKARAMO, SARANIUENO",
    "h/KIROKUWOMEZASITE, GANBATTENE!",
    NULL,
};

static char *aiaiDialogue12_en[] =
{
    "You got %p!",
    "That's a new high score!",
    "Congratulations!",
    "That was truly a great game!",
    NULL,
};

static char *aiaiDialogue13_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/KOREYORIk/SUGOIh/KIROKUHANAIYO!",
    "k/KIMIh/HA, k/KANPEKIh/DANE!",
    "h/TYO-k/MIRAKURUh/NAk/PUREIh/DATTAYO!",
    NULL,
};

static char *aiaiDialogue13_en[] =
{
    "You got %p!",
    "There's no record better than this!",
    "You are the greatest!",
    "That was truly a great game!",
    NULL,
};

static char *aiaiDialogue14_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/SAIKO-h/HA, %bk/POINNTOh/DANE!",
    "h/MOUITIDO, %bk/POINNTOh/DASERUKA",
    "k/TYARENJIh/SHITEMITENE!",
    NULL,
};

static char *aiaiDialogue14_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get %b!",
    NULL,
};

static char *meemeeDialogue0_jp[] =
{
    "h/KONNDOHA MOTTO",
    "h/UENOk/REBERUh/WO MEZASHITENE",
    NULL,
};

// MeeMee Dialogue

static char *meemeeDialogue0_en[] =
{
    "Try a higher level",
    "next time",
    NULL,
};

static char *meemeeDialogue1_jp[] =
{
    "h/ANATAMO KOREDE",
    "h/ICHININNMAENO Ok/SARUh/SANNNE",
    NULL,
};

static char *meemeeDialogue1_en[] =
{
    "Now you've become",
    "pretty monkey like me",
    NULL,
};

static char *meemeeDialogue2_jp[] =
{
    "h/ANATATTEk/SUGOIh/WA! KOREKARAMO",
    "k/PUREI h/SHIMAKUTTENE",
    NULL,
};

static char *meemeeDialogue2_en[] =
{
    "You are so fantastic! Remember,",
    "Monkey Ball is the best game for you!",
    NULL,
};

static char *meemeeDialogue3_jp[] =
{
    "h/KOKOMADEk/PUREIh/DEKITAANATAHA, k/ERAIh/WA!",
    "p/JIBUNN/h/DEp/JIBUNN/h/WOHOMETEAGETEMOIIKAMO!",
    "p/IMA/p/KAIsuu/h/HAAKIRAMETAKEDO, p/TSUGI/h/NOk/PUREIh/DEHA",
    "k/KURIAh/DEKIRUYOUNI, GANNBATTENE!",
    NULL,
};

static char *meemeeDialogue3_en[] =
{
    "Wow! You've made it this far!",
    "You really are something!",
    "I guess you should take a break.",
    "Next time, let's try to make it",
    "all the way to the end!",
    NULL,
};

static char *meemeeDialogue4_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUh/GAk/GETTOh/DEKITYATTARISHITE.",
    NULL,
};

static char *meemeeDialogue4_en[] =
{
    "You got %p!",
    "All together, that's %t!",
    "You might be able to unlock a game",
    "with %n_ more points!",
    NULL,
};

static char *meemeeDialogue5_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUWAYO!",
    NULL,
};

static char *meemeeDialogue5_en[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Since you got a whole lot more points,",
    "you can unlock any Mini Game",
    "you want!",
    NULL,
};

static char *meemeeDialogue6_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "h/SAIGONOk/MINIGE-MUh/MOk/GETTOh/DEKIRUWAYO!",
    NULL,
};

static char *meemeeDialogue6_en[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Since you got a whole lot more points,",
    "you'll be able to unlock",
    "the last Mini Game!",
    NULL,
};

static char *meemeeDialogue7_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/ATO%nk/POINNTOh/DE",
    "k/KONNTEXINYU-p/KAISUU/h/WOHUYASERUWAYO!",
    NULL,
};

static char *meemeeDialogue7_en[] =
{
    "You got %p!",
    "All together, that's %t!",
    "You can increase",
    "your number of continues",
    "if you get %n_ more points!",
    NULL,
};

static char *meemeeDialogue8_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "k/KONNTEXINYU-p/KAISUU/h/GA%cp/KAIsuu/h/NINATTAWA!",
    NULL,
};

static char *meemeeDialogue8_en[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Since you got a whole lot more points,",
    "you'll be able to increase",
    "your number of continues to %c!",
    NULL,
};

static char *meemeeDialogue9_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/KOREDEk/KONNTEXINYU-h/GAp/NANNKAI/h/DEMO",
    "h/DEKIRUYOUNINATTAWAYO!",
    NULL,
};

static char *meemeeDialogue9_en[] =
{
    "You got %p!",
    "All together, that's %t!",
    "Now, you can play with",
    "infinite continues!",
    NULL,
};

static char *meemeeDialogue10_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ANATANIAGERARERUMONOHA, MOUNAIWA!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/PUREIPOINNTOh/WO",
    "h/DOREDAKETORERUKANI, TYOUSENNYO!",
    NULL,
};

static char *meemeeDialogue10_en[] =
{
    "You got %p!",
    "You've unlocked everything!",
    "Now, you can try to see how many",
    "points you can get in 1 player mode!",
    NULL,
};

static char *meemeeDialogue11_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "k/SAIKO-h/HA, %bk/POINNTOh/YO!",
    "k/SAIKO-h/KIROKUWONURIKAERARERUYOUNI",
    "h/KOREKARAMO, ASOBITUDUKETENE!",
    NULL,
};

static char *meemeeDialogue11_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best and play more to beat",
    "the high score, okay?",
    NULL,
};

static char *meemeeDialogue12_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KOREHA, k/SAIKO-h/KIROKUDAWA!",
    "k/SUGGO~I! h/KOREKARAMO",
    "h/UENOKIROKUWOMEZASHITE, GANBATTENE!",
    NULL,
};

static char *meemeeDialogue12_en[] =
{
    "You got %p!",
    "You got the high score!",
    "Wow! I can't believe it!",
    "Now just try to beat",
    "your own record!",
    NULL,
};

static char *meemeeDialogue13_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KOREIJYOUNOKIROKUHA, DENAIWAYO!",
    "h/ANATATTE, k/PA-FEKUTOh/NE!",
    "h/MOU, k/SUTEKIp/./k/TOKIMEKIp/./k/DA~ISUKIh/YO!",
    NULL,
};

static char *meemeeDialogue13_en[] =
{
    "You got %p!",
    "There's no record better than this!",
    "You are perfect!",
    "You are the greatest!",
    NULL,
};

static char *meemeeDialogue14_jp[] =
{
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KIROKUHA,%bk/POINNTOh/YO!",
    "h/%bk/POINNTOh/TOIUk/SAIKO-h/KIROKUWO",
    "h/MATADASERUKA, TYOUSENNSITENE!",
    NULL,
};

static char *meemeeDialogue14_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get the high score",
    "of %b!",
    NULL,
};

static char *lbl_801E4814_jp[] =
{
    "h/MOTTO UENOk/REBERUh/DE",
    "h/ASOBITAIDEk/CHU",
    NULL,
};

// Baby Dialogue

static char *babyDialogue0_en[] =
{
    "I want to play higher level",
    "next time",
    NULL,
};

static char *lbl_801E487C_jp[] =
{
    "h/KOREDE RIPPANA Ok/SARUh/SANNNO",
    "h/NAKAMAIRIDEk/CHU",
    NULL,
};

static char *babyDialogue1_en[] =
{
    "Now you've become",
    "grown up monkey like me",
    NULL,
};

static char *lbl_801E48F4_jp[] =
{
    "k/SUGOIh/DEk/CHU! h/KOREKARAMO",
    "h/ZUTTO ASONNDEHOSIIDEk/CHU",
    NULL,
};

static char *babyDialogue2_en[] =
{
    "You are such grown up! Remember,",
    "Monkey Ball is the best game for you!",
    NULL,
};

static char *lbl_801E4A10_jp[] =
{
    "h/KONOk/SUTE-JIh/MADEk/PUREIh/DEKIRUNANNTE",
    "k/SUGOIh/DEk/TYU! ODOROKIh/DEk/TYU!",
    "h/TOTTEMOk/MUZUKAh/TIIDEk/TYUh/KEDO",
    "h/TUGIHAk/SAIGOh/MADEITTEHOTIIDEk/TYU!",
    NULL,
};

static char *babyDialogue3_en[] =
{
    "Wow! You've made it this far!",
    "You are weally good!",
    "It's a wittle hard, but let's try",
    "to make it all da way to da end!",
    NULL,
};

static char *lbl_801E4AE4_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUGETTOh/DEk/TYU! BABU~!",
    NULL,
};

static char *babyDialogue4_en[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Only %n weft to",
    "unwock the next Mini Game!",
    NULL,
};

static char *lbl_801E4B6C_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUDEk/TYU!",
    NULL,
};

static char *babyDialogue5_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock a Mini Game!",
    NULL,
};

static char *lbl_801E4C28_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "k/MINIGE-MUh/WOZENNBUk/GETTOh/DEKIRUDEk/TYU!",
    NULL,
};

static char *babyDialogue6_en[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Since you got a wot of points,",
    "you can unwock the last Mini Game!",
    NULL,
};

static char *lbl_801E4CB4_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/ATO%nk/POINNTOh/DE",
    "k/KONNTEXINYU-h/WOHUYASERUDEk/TYU!",
    NULL,
};

static char *babyDialogue7_en[] =
{
    "You got %p!",
    "All togedder, you have %p!",
    "Only %n weft to",
    "incwease your number of continues!",
    NULL,
};

static char *lbl_801E4D34_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "k/KONNTEXINYU-h/GA%ch/KAININATTADEk/TYU!",
    NULL,
};

static char *babyDialogue8_en[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Since you got a wot of points,",
    "you incwease your number",
    "of continues!",
    NULL,
};

static char *lbl_801E4DBC_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/NANNKAIDEMOk/KONNTEXINYU-h/GA",
    "h/DEKIRUYOUNINATTADEk/TYU! BABU~!",
    NULL,
};

static char *babyDialogue9_en[] =
{
    "You got %p!",
    "All togeder, you have %t!",
    "Now, you can pway with",
    "infinite continues!",
    NULL,
};

static char *lbl_801E4E5C_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/MOU, k/PUREZENNTOh/SURUMONOHANAIDEk/TYU!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/PUREIPOINNTOh/WO",
    "h/IKUTUk/GETTOh/SURUKANIk/TYARENJIh/DEk/TYU!",
    NULL,
};

static char *babyDialogue10_en[] =
{
    "You got %p!",
    "You've unwocked everyting!",
    "Now, you can try to set the",
    "high score for the most Play Points!",
    NULL,
};

static char *lbl_801E4F04_jp[] =
{
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "k/SAIKO-h/HA%bk/POINNTOh/DEk/TYU!",
    "k/SAIKO-h/KIROKU, MEZATITE",
    "h/KOREKARAMO, k/PUREIh/SURUDEk/TYU!",
    NULL,
};

static char *babyDialogue11_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best to try and beat",
    "the high score!! Let's go!!",
    NULL,
};

static char *lbl_801E4FCC_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/KOREHA, k/SAIKO-h/KIROKUDEk/TYU!",
    "h/YATTADEk/TYU! h/URETIIDEk/TYU!",
    "k/SUGOIh/DEk/TYU! UKIUKIh/DEk/TYU!",
    NULL,
};

static char *babyDialogue12_en[] =
{
    "You got %p!",
    "That's a new high score!",
    "Congwatuwations!",
    "That was truly a gweat game!",
    NULL,
};

static char *lbl_801E5078_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "k/PA-FEKUTOh/NAKIROKUDEk/TYU!",
    "h/KOREYORIk/SUGOIh/KIROKUHA",
    "h/MOUNAIDEk/TYU! ODOROKIh/DEk/TYU!",
    NULL,
};

static char *babyDialogue13_en[] =
{
    "You got %p!",
    "There's no wecord better than this!",
    "You are the greatest!",
    "That was truly a gweat game!",
    NULL,
};

static char *lbl_801E5134_jp[] =
{
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "k/SAIKO-h/HA,%bk/POINNTOh/DEk/TYU!",
    "h/%bk/POINNTOh/WOMATAk/GETTOh/DEKIRUKA",
    "k/TYARENNJIh/TITEHOSIIDEk/TYU! BABU~!",
    NULL,
};

static char *babyDialogue14_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get %b! Goo goo!!!",
    NULL,
};

static char *gogonDialogue0_jp[] =
{
    "k/SAIKYO~NOSARU MEZASHITE MOTTO",
    "k/UENO REBERUDE ASONDEKURE~!",
    NULL,
};

// GonGon dialogue

static char *gogonDialogue0_en[] =
{
    "Try to be the greatest monkey ever!",
    "Play the more difficult levels!",
    NULL,
};

static char *gogonDialogue1_jp[] =
{
    "k/SUGOIJYANE~KA! ODOROKIDAZE!",
    "k/DAXA~KEDO UEHA MADA ARUZE!",
    NULL,
};

static char *gogonDialogue1_en[] =
{
    "Wow! You're really good!",
    "I'm surprised!",
    "Buuuut, there's still a more",
    "difficult level to beat!",
    NULL,
};

static char *gogonDialogue2_jp[] =
{
    "k/UHOUHO UHOHO~I! YATTA~!",
    "k/KOREDE SAIKYO~NOSARU DAZE~!",
    NULL,
};

static char *gogonDialogue2_en[] =
{
    "Oh wow!",
    "I can't believe it!",
    "You really are the greatest",
    "monkey ever!!",
    NULL,
};

static char *gogonDialogue3_jp[] =
{
    "k/KONOSUTE~JIMADE KORETA OMAEHA",
    "k/KANARINO UDEMAEDA~XTU! UHOUHO!",
    "k/TUGINI PUREISURUTOKIHA AKIRAMEZUNI",
    "k/ZENNBU KURIA DEKIRUYOUNI GANBARE!",
    NULL,
};

static char *gogonDialogue3_en[] =
{
    "You really are something to have",
    "made it this far! Congratulations!!",
    "Next time you play,",
    "let's take it all the way!!!",
    NULL,
};

static char *gogonDialogue4_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/ATO %nk/POINNTO DE",
    "k/MINIGEXE~MU GETTOXO~XTU! UHOHO!",
    NULL,
};

static char *gogonDialogue4_en[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Only %n left to",
    "unlock the next Mini Game!",
    "Go go go!!!",
    NULL,
};

static char *gogonDialogue5_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/MINIGE~MU GETTOXO~XTU! DAXA~!",
    NULL,
};

static char *gogonDialogue5_en[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you can unlock a Mini Game! Yeah!!!",
    NULL,
};

static char *gogonDialogue6_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/MINIGEXE~MU ZE~NNBU GETTOXO~XTU!",
    NULL,
};

static char *gogonDialogue6_en[] =
{
    "You got %p!",
    "All together, you have %t",
    "Since you got a lot of points,",
    "you can unlock the last Mini Game!",
    "Go for it!!!",
    NULL,
};

static char *gogonDialogue7_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/ATO %nk/POINNTO DE",
    "k/KONNTEXINYU~ HUE~RU! UHOUHO!",
    NULL,
};

static char *gogonDialogue7_en[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Only %n left to",
    "increase your number of continues!",
    NULL,
};

static char *gogonDialogue8_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/KONNTEXINYU~ %ck/ KAINI HUETA~XTU!",
    NULL,
};

static char *gogonDialogue8_en[] =
{
    "You got %p! Alright!!!",
    "All together, you have %t!",
    "Since you got a lot of points,",
    "you increase your number",
    "of continues to %c!",
    NULL,
};

static char *gogonDialogue9_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/DAXAKA~RA KONNTEXINYU~ NANNDOMO",
    "k/DEKIRUYOUNI NATTA~! NATTA~!",
    NULL,
};

static char *gogonDialogue9_en[] =
{
    "You got %p!",
    "All together, you have %t!",
    "Now, you can play with",
    "infinite continues! Good job!!",
    NULL,
};

static char *gogonDialogue10_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/MOU OMAENI YARERUMONO NAI!",
    "k/KOREKARAHA 1PUREIDE PUREIPOINNTO",
    "k/NANNTENN GETTODEKIRUKA TYARENNJI!",
    NULL,
};

static char *gogonDialogue10_en[] =
{
    "You got %p!",
    "You've unlocked everything! Wow!!!",
    "Now, you can try to set the",
    "high score for most Play Points!",
    NULL,
};

static char *gogonDialogue11_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~%bk/POINNTO! ZANNNENN!",
    "k/KOREKARAMO SAIKO~KIROKU MEZASITE",
    "k/PUREI SURUNO DA~XTU! UHOHO~I!",
    NULL,
};

static char *gogonDialogue11_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Do your best to try and beat",
    "the high score! You can do it!!!",
    NULL,
};

static char *gogonDialogue12_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~KIROKU DETTA~! DETTA~!",
    "k/UHO UHOHO! UHOUHO UHOHO~I!",
    "k/YATTA~ YATTA~! YATTA~GO~NN!",
    NULL,
};

static char *gogonDialogue12_en[] =
{
    "You got %p!",
    "That's a new high score!",
    "Congratulations!",
    "You really are the best!!! Woo hoo!!!",
    NULL,
};

static char *gogonDialogue13_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/KO KOREHA ODOROKINO DA~IKIROKU!",
    "k/KOREIJYO~ SUGOI KIROKU DENAI!",
    "k/OMAE TENNSA~I SUGOIZOXO~XTU!",
    NULL,
};

static char *gogonDialogue13_en[] =
{
    "You got %p!",
    "There's no record better than this!",
    "I can't believe it!!",
    "You must be a genius!!!",
    NULL,
};

static char *gogonDialogue14_jp[] =
{
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~%bk/POINNTO! ZANNNENN",
    "k/%bk/POINNTO MATA DASERUYO~NI",
    "k/GANNBARE~XTU! 1p/./k/2p/./k/3p/./k/DAXA~XTU!",
    NULL,
};

static char *gogonDialogue14_en[] =
{
    "You got %p!",
    "The high score is %b!",
    "Try one more time to see",
    "if you can get %b!",
    "Readyp/TENNTENN/a/ Go!!!",
    NULL,
};

static char **aiaiDialogue[] =
{
    aiaiDialogue0_en,
    aiaiDialogue1_en,
    aiaiDialogue2_en,
    aiaiDialogue3_en,
    aiaiDialogue4_en,
    aiaiDialogue5_en,
    aiaiDialogue6_en,
    aiaiDialogue7_en,
    aiaiDialogue8_en,
    aiaiDialogue9_en,
    aiaiDialogue10_en,
    aiaiDialogue11_en,
    aiaiDialogue12_en,
    aiaiDialogue13_en,
    aiaiDialogue14_en,
};

static char **meemeeDialogue[] =
{
    meemeeDialogue0_en,
    meemeeDialogue1_en,
    meemeeDialogue2_en,
    meemeeDialogue3_en,
    meemeeDialogue4_en,
    meemeeDialogue5_en,
    meemeeDialogue6_en,
    meemeeDialogue7_en,
    meemeeDialogue8_en,
    meemeeDialogue9_en,
    meemeeDialogue10_en,
    meemeeDialogue11_en,
    meemeeDialogue12_en,
    meemeeDialogue13_en,
    meemeeDialogue14_en,
};

static char **babyDialogue[] =
{
    babyDialogue0_en,
    babyDialogue1_en,
    babyDialogue2_en,
    babyDialogue3_en,
    babyDialogue4_en,
    babyDialogue5_en,
    babyDialogue6_en,
    babyDialogue7_en,
    babyDialogue8_en,
    babyDialogue9_en,
    babyDialogue10_en,
    babyDialogue11_en,
    babyDialogue12_en,
    babyDialogue13_en,
    babyDialogue14_en,
};

static char **gongonDialogue[] =
{
    gogonDialogue0_en,
    gogonDialogue1_en,
    gogonDialogue2_en,
    gogonDialogue3_en,
    gogonDialogue4_en,
    gogonDialogue5_en,
    gogonDialogue6_en,
    gogonDialogue7_en,
    gogonDialogue8_en,
    gogonDialogue9_en,
    gogonDialogue10_en,
    gogonDialogue11_en,
    gogonDialogue12_en,
    gogonDialogue13_en,
    gogonDialogue14_en,
};

static char ***characterDialogues[] =
{
    aiaiDialogue,
    meemeeDialogue,
    babyDialogue,
    gongonDialogue,
};

static void get_game_results(struct GameResultInfo *arg0);
static char **get_ending_dlg_lines(const struct GameResultInfo *arg0, char ***arg1);
static void expand_dialogue_placeholders(const struct GameResultInfo *result, struct EndingCharaDialogue *out, char *buffer, char **lines);

void ending_prepare_chara_dialogue(struct EndingCharaDialogue *out, char *buffer)
{
    struct GameResultInfo result;
    char **lines;

    get_game_results(&result);
    lines = get_ending_dlg_lines(&result, NULL);
    expand_dialogue_placeholders(&result, out, buffer, lines);
}

void ending_prepare_chara_dialogue_test(int index, struct EndingCharaDialogue *out, char *buffer)
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
    expand_dialogue_placeholders(&result, out, buffer, lines);
}

static void get_game_results(struct GameResultInfo *out)
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

static char **get_ending_dlg_lines(const struct GameResultInfo *result, char ***out)
{
    struct GameResultInfo res = *result;
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
    else if (res.lockedMinigames != 0)
    {
        if (res.canUnlockMinigame == 0)
            lines = charaDlg[4];
        else if (res.lockedMinigames > 1)
            lines = charaDlg[5];
        else
            lines = charaDlg[6];
    }
    else if (res.unk14 == 0)
    {
        if (res.unk18 == 0)
            lines = charaDlg[7];
        else if (!res.hasMaxContinues)
            lines = charaDlg[8];
        else
            lines = charaDlg[9];
    }
    else if (res.unk2C != 0)
    {
        lines = charaDlg[10];
        lbl_802F22C8 |= 1;
    }
    else if (res.playPoints >= res.unk30)
    {
        if (res.playPoints >= res.unk34)
            lines = charaDlg[13];
        else
            lines = charaDlg[12];
    }
    else if (res.unk30 >= res.unk34)
        lines = charaDlg[14];
    else
        lines = charaDlg[11];
    if (out != NULL)
        *out = lines;
    return lines;
}

static void expand_dialogue_placeholders(const struct GameResultInfo *result, struct EndingCharaDialogue *out, char *buffer, char **lines)
{
	static char *pointsTexts[] = { " point", " points", "" };
	struct EndingCharaDialogue dialogue;
	const struct GameResultInfo res = *result;
	char **pSrcLine;
	const char *src;
	char *dst;
	const char *p;

	memset(&dialogue, 0, sizeof(dialogue));
	pSrcLine = lines;
	dst = buffer;
	while (*pSrcLine != NULL)
	{
		dialogue.lines[dialogue.numLines] = dst;
		src = *pSrcLine;
		while (*src != 0)
		{
			switch (*src)
			{
			case '%':
				switch (*++src)
				{
				case 'p':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", res.playPoints, (res.playPoints != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				case 't':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", res.unk4, (res.unk4 != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				case 'n':
					if (res.unk24 != 1)
						p = pointsTexts[1];
					else
						p = pointsTexts[0];
					if (src[1] == '_')
					{
						p = pointsTexts[2];
						src++;
					}
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", res.unk24, p);
					break;
				case 'c':
					sprintf(dst, "c/0xff0000/%dc/0x000000/", res.continues);
					break;
				case 'b':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", res.unk30, (res.unk30 != 1) ? pointsTexts[1] : pointsTexts[0]);
					break;
				case 'm':
					sprintf(dst, "c/0xff0000/%dc/0x000000/%s", res.unk34, (res.unk34 != 1) ? pointsTexts[1] : pointsTexts[0]);
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
		*dst++ = 0;
		pSrcLine++;
		dialogue.numLines++;
	}
	*out = dialogue;
}
