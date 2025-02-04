#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "adv.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "course.h"
#include "input.h"
#include "info.h"
#include "item.h"
#include "mathutil.h"
#include "mode.h"
#include "name_entry.h"
#include "pool.h"
#include "recplay.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "window.h"

s8 lbl_802F1CB0[8];
s32 practiceBestScore;
s32 practiceBestFlag;

struct Struct801F3A58 infoWork;

static void info_reset(void);

void info_init_first(void)
{
    infoWork.unk8 = 0;
    infoWork.attempts = 1;
    infoWork.currFloor = 1;
    infoWork.livesLost = 0;
    infoWork.continuesUsed = 0;
    if (modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
    {
        practiceBestScore = 0;
        practiceBestFlag = 0;
    }
}

u32 lbl_801F3A8C[4];                    FORCE_BSS_ORDER(lbl_801F3A8C)
struct Struct801F3A58 playerInfos[4];  FORCE_BSS_ORDER(playerInfos)

void ev_info_init(void)
{
    info_reset();

    // Initialize timer
    infoWork.timerCurr = 60 * 60;
    if (modeCtrl.courseFlags & 1)
        infoWork.timerCurr = u_get_stage_time_limit();
    if (gameSubmode == SMD_ADV_INFO_INIT)
        infoWork.timerCurr = 90 * 60;
    if (gameSubmode == SMD_GAME_NAMEENTRY_READY_INIT)
        infoWork.timerCurr = 60 * 60;
    infoWork.timerMax = infoWork.timerCurr;

    if (is_bonus_stage(currStageId))
        infoWork.flags |= INFO_FLAG_BONUS_STAGE;
    if (is_final_floor(modeCtrl.difficulty, infoWork.currFloor, modeCtrl.courseFlags) != 0)
        infoWork.flags |= INFO_FLAG_FINAL_FLOOR;
}

void ev_info_main(void)
{
    s8 *status;
    int i;
    struct Ball *ballBackup;
    int r20;
    struct Ball *ball;
    struct ReplayHeader spC8;
    struct PhysicsBall sp6C;

    if (debugFlags & 0xA)
        return;

    // handle goal
    ballBackup = currentBall;
    ball = ballInfo;
    status = g_poolInfo.playerPool.statusList;
    r20 = 0;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
    {
        u32 goalId;
        s32 sp64;

        if (*status == STAT_NULL || *status == STAT_FREEZE)
            continue;

        currentBall = ball;
        if (!check_ball_entered_goal(ball, &goalId, &sp64))
            continue;
        infoWork.playerId = ball->playerId;
        switch (modeCtrl.gameType)
        {
        case GAMETYPE_MAIN_COMPETITION:
            if (ball->flags & BALL_FLAG_24)
            {
                recplay_get_header(g_recplayInfo.u_replayIndexes[ball->playerId], &spC8);
                if (!(spC8.flags & 1))
                    continue;
            }
            if (ball->flags & BALL_FLAG_24)
            {
                infoWork.flags &= ~(INFO_FLAG_REPLAY|INFO_FLAG_11);
                ball->flags |= BALL_FLAG_GOAL;
                ball->state = 5;
                ball->unk150 = ball->pos;
            }
            if (!(ball->flags & BALL_FLAG_GOAL) && !(infoWork.flags & INFO_FLAG_05))
            {
                recplay_set_recording_stop_timer(0x5A);
                infoWork.flags &= ~(INFO_FLAG_REPLAY|INFO_FLAG_11);
                r20++;
                ball->flags |= BALL_FLAG_GOAL;
                ball->state = 5;
                ball->unk150 = ball->pos;
                cameraInfo[i].state = 14;
                if (r20 == 1)
                    infoWork.unk2C++;
                if (!(infoWork.flags & INFO_FLAG_REPLAY) && !(infoWork.flags & INFO_FLAG_BONUS_STAGE))
                {
                    ball->rank = infoWork.unk2C;
                    if (ball->rank == 1)  // first place
                    {
                        ball->winStreak++;
                        ball->unk128 = 0;
                    }
                    else if (ball->rank == modeCtrl.playerCount)  // last place
                    {
                        ball->winStreak = 0;
                        ball->unk128++;
                    }
                    else
                    {
                        ball->winStreak = 0;
                        ball->unk128 = 0;
                    }
                    func_80023DB8(ball);
                    create_rank_icon(ball);
                }
                if (infoWork.unk2C == 1)
                    func_800245E4(ball, goalId, sp64);
                recplay_record_goal(ball->playerId);
            }
            init_physball_from_ball(ball, &sp6C);
            if (sp64 != sp6C.animGroupId)
                tf_physball_to_anim_group_space(&sp6C, sp64);
            break_goaltape(goalId, &sp6C);
            ball->unk12A = infoWork.timerCurr;
            func_80024860(ball);
            break;
        case GAMETYPE_MINI_FIGHT:
            break;
        default:
            if (ball->flags & BALL_FLAG_11)
                break;
            if (ball->flags & BALL_FLAG_24)
            {
                recplay_get_header(g_recplayInfo.u_replayIndexes[ball->playerId], &spC8);
                if (!(spC8.flags & 1))
                    break;
            }
            infoWork.flags &= ~(INFO_FLAG_REPLAY|INFO_FLAG_11);
            ball->flags |= BALL_FLAG_GOAL;
            ball->state = BALL_STATE_GOAL_INIT;
            if (!(infoWork.flags & INFO_FLAG_05))
            {
                u_time_over_all_competition_mode_balls();
                func_800245E4(ball, goalId, sp64);
                if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL)
                    func_800AEDDC();
            }
            recplay_record_goal(ball->playerId);
            if (gameSubmode == SMD_ADV_GAME_PLAY_MAIN)
                infoWork.flags |= INFO_FLAG_GOAL;
            init_physball_from_ball(ball, &sp6C);
            if (sp64 != sp6C.animGroupId)
                tf_physball_to_anim_group_space(&sp6C, sp64);
            break_goaltape(goalId, &sp6C);
            ball->unk12A = infoWork.timerCurr;
            func_80024860(ball);
            break;
        }
    }
    if (r20 > 1)
        infoWork.unk2C += r20 - 1;
    if (r20 > 0 && infoWork.unk2C >= modeCtrl.playerCount - 1)
        u_time_over_all_competition_mode_balls();
    currentBall = ballBackup;

    infoWork.bananasLeft = 0;

    {
        struct Item *item = g_itemInfo;
        s8 *status = g_poolInfo.itemPool.statusList;

        for (i = 0; i < g_poolInfo.itemPool.count; item++, i++, status++)
        {
            if (*status != STAT_NULL && *status != STAT_DEST && item->type == 0 && (item->flags & (1 << 1)))
                infoWork.bananasLeft++;
        }
    }

    if ((infoWork.flags & INFO_FLAG_BONUS_STAGE)
     && !(infoWork.flags & INFO_FLAG_05)
     && infoWork.bananasLeft == 0)
    {
        infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_05|INFO_FLAG_BONUS_CLEAR;
        recplay_record_bonus_clear(ball->playerId);
        BALL_FOREACH( ball->flags |= BALL_FLAG_13; )
    }

    // Press X+Y in debug mode to instantly complete level
    if (!(infoWork.flags & INFO_FLAG_05)
     && !(infoWork.flags & INFO_FLAG_TIMER_PAUSED)
     && (dipSwitches & DIP_DEBUG)
     && ((infoWork.flags & INFO_FLAG_BONUS_STAGE) || decodedStageLzPtr->goals != NULL)
     && (g_currPlayerButtons[0] & PAD_BUTTON_X) && (g_currPlayerButtons[0] & PAD_BUTTON_Y))
    {
        struct StageGoal *goal;
        int goalId;

        if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL)
            infoWork.playerId = modeCtrl.currPlayer;
        else
            infoWork.playerId = 0;

        goalId = 0;
        goal = decodedStageLzPtr->animGroups[0].goals;
        for (i = 0; i < decodedStageLzPtr->animGroups[0].goalCount; i++, goal++)
        {
            if (goal->type == 'B')
            {
                goalId = i;
                break;
            }
        }
        // Holding up, right, or down selects the red, green, and blue goals, respectively
        if ((g_currPlayerButtons[0] & PAD_BUTTON_UP)
         || (g_currPlayerButtons[0] & PAD_BUTTON_RIGHT)
         || (g_currPlayerButtons[0] & PAD_BUTTON_DOWN))
        {
            // fake match
            goal = ((volatile struct StageAnimGroup *)&decodedStageLzPtr->animGroups[0])->goals;
            for (i = 0; i < decodedStageLzPtr->animGroups[0].goalCount; i++, goal++)
            {
                if ((g_currPlayerButtons[0] & PAD_BUTTON_UP) && goal->type == 'R')
                {
                    goalId = i;
                    break;
                }
                if ((g_currPlayerButtons[0] & PAD_BUTTON_RIGHT) && goal->type == 'G')
                {
                    goalId = i;
                    break;
                }
                if ((g_currPlayerButtons[0] & PAD_BUTTON_DOWN) && goal->type == 'B')
                {
                    goalId = i;
                    break;
                }
            }
        }
        infoWork.goalEntered = goalId;
        infoWork.unkE = 0;
        infoWork.unk10 = ball->vel;
        infoWork.unk1C = infoWork.timerCurr;
        if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE) && (g_currPlayerButtons[0] & PAD_BUTTON_LEFT))
            infoWork.unk22 = 10;

        BALL_FOREACH(
            if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE))
            {
                if (!(ball->flags & BALL_FLAG_GOAL))
                {
                    ball->flags |= BALL_FLAG_GOAL;
                    ball->state = 5;
                    ball->unk150 = ball->pos;
                }
            }
            else
                ball->flags |= BALL_FLAG_13;
        )

        u_time_over_all_competition_mode_balls();
        if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE))
        {
            struct PhysicsBall sp8;

            init_physball_from_ball(&ballInfo[0], &sp8);
            break_goaltape(infoWork.goalEntered, &sp8);
            ball->unk12A = infoWork.timerCurr;
            u_play_sound_0(0x16);
        }
    }

    if (!(infoWork.flags & INFO_FLAG_05) && !(advDemoInfo.flags & (1 << 8)))
    {
        status = g_poolInfo.playerPool.statusList;
        ball = ballInfo;
        for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
        {
            if (*status == STAT_NULL || *status == STAT_FREEZE)
                continue;
            if (ball->flags & BALL_FLAG_11)
                continue;
            if (func_800246F4(ball) == 0)
                continue;
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MAIN_COMPETITION:
                ball->state = 19;
                ball->flags |= BALL_FLAG_11;
                ball->unk150 = ball->pos;
                break;
            case GAMETYPE_MINI_FIGHT:
                ball->flags |= BALL_FLAG_11;
                break;
            default:
                infoWork.flags |= INFO_FLAG_FALLOUT|INFO_FLAG_TIMER_PAUSED;
                ball->flags |= BALL_FLAG_11;
                recplay_record_fallout(ball->playerId);
                break;
            }
        }
    }

    if (!(infoWork.flags & INFO_FLAG_TIMER_PAUSED) && !(dipSwitches & DIP_TIME_STOP))
    {
        infoWork.unk8++;
        if (!(infoWork.flags & INFO_FLAG_11))
            infoWork.timerCurr--;
        
        // handle time over
        if (infoWork.timerCurr <= 0)
        {
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MAIN_COMPETITION:
                if (infoWork.unk2C > 0)
                {
                    u_time_over_all_competition_mode_balls();
                    infoWork.flags |= INFO_FLAG_13;
                    break;
                }
                infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_TIMEOVER;
                recplay_record_timeover(ball->playerId);
                if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE))
                    BALL_FOREACH( ball->winStreak = 0; ball->unk128++; )
                BALL_FOREACH( ball->flags |= BALL_FLAG_TIMEOVER; )
                break;
            case 4:
                infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_TIMEOVER;
                break;
            default:
                {
                    struct Ball *ball;

                    ball = currentBall;
                    infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_TIMEOVER;
                    recplay_record_timeover(ball->playerId);
                    ball->flags |= BALL_FLAG_TIMEOVER;
                }
                break;
            }
        }
    }

    BALL_FOREACH( ball_ape_yang(ball); )
}

void ev_info_dest(void)
{
    info_reset();
}

static void info_reset(void)
{
    int unk8 = infoWork.unk8;
    int attempts = infoWork.attempts;
    int currFloor = infoWork.currFloor;
    int livesLost = infoWork.livesLost;
    int continuesUsed = infoWork.continuesUsed;
    int u_currStageId = infoWork.u_currStageId;

    memset(&infoWork, 0, sizeof(infoWork));

    infoWork.unk8  = unk8;
    infoWork.attempts = attempts;
    infoWork.currFloor = currFloor;
    infoWork.livesLost = livesLost;
    infoWork.continuesUsed = continuesUsed;
    infoWork.u_currStageId = u_currStageId;
    infoWork.unk22 = 1;
    if (modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        practiceBestFlag = 0;
}

BOOL check_ball_entered_goal(struct Ball *ball, u32 *outGoalId, s32 *outGoalAnimGroupId)
{
    struct PhysicsBall physBall;
    struct StageAnimGroup *stageAg;
    int goalId;
    int animGroupId;

    init_physball_from_ball(ball, &physBall);
    stageAg = decodedStageLzPtr->animGroups;
    goalId = 0;
    for (animGroupId = 0; animGroupId < decodedStageLzPtr->animGroupCount; animGroupId++, stageAg++)
    {
        if (stageAg->goalCount > 0)
        {
            struct StageGoal *goal;
            int agGoalIdx;

            if (animGroupId != physBall.animGroupId)
                tf_physball_to_anim_group_space(&physBall, animGroupId);
            goal = stageAg->goals;
            for (agGoalIdx = 0; agGoalIdx < stageAg->goalCount; agGoalIdx++, goal++)
            {
                struct ColiRect goalTrigger;

                mathutil_mtxA_from_translate(&goal->pos);
                mathutil_mtxA_rotate_z(goal->rotZ);
                mathutil_mtxA_rotate_y(goal->rotY);
                mathutil_mtxA_rotate_x(goal->rotX);
                goalTrigger.pos.x = 0.0f;
                goalTrigger.pos.y = 1.0f;
                goalTrigger.pos.z = 0.0f;
                mathutil_mtxA_tf_point(&goalTrigger.pos, &goalTrigger.pos);
                goalTrigger.rot.x = goal->rotX;
                goalTrigger.rot.y = goal->rotY;
                goalTrigger.rot.z = goal->rotZ;
                goalTrigger.width = 2.0f;
                goalTrigger.height = 2.0f;
                if (test_line_intersects_rect(&physBall.pos, &physBall.prevPos, &goalTrigger))
                {
                    *outGoalId = goalId;
                    *outGoalAnimGroupId = animGroupId;
                    return TRUE;
                }
                goalId++;
            }
        }
    }
    return FALSE;
}

void u_time_over_all_competition_mode_balls(void)
{
    if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
        infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_05|INFO_FLAG_BONUS_CLEAR|INFO_FLAG_10;
    else
        infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_05|INFO_FLAG_GOAL;

    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        BALL_FOREACH(
            if (!(ball->flags & BALL_FLAG_GOAL))
            {
                ball->flags |= (BALL_FLAG_08|BALL_FLAG_IGNORE_GRAVITY);
                ball->winStreak = 0;
                ball->unk128++;
                ball->flags |= BALL_FLAG_TIMEOVER;
            }
        )
    }
}

void func_80023DB8(struct Ball *ball)
{
    int r5;

    if (ball->rank < 1 || ball->rank > 3)
        return;

    r5 = lbl_802F1CB0[ball->rank];
    if ((modeCtrl.courseFlags & (1 << 11)) && ball->winStreak > 0)
        r5 *= ball->winStreak;
    ball->bananaBonus = r5;
}

struct Struct801818D0
{
    float u1;
    float v1;
    float u2;
    float v2;
};

static struct Struct801818D0 rankTexOffsets[4] =
{
    {   0,  0, 160, 48 },
    {   0,  0, 160, 48 },
    {   0, 48, 128, 48 },
    { 160,  0,  88, 48 },
};

static void bonus_count_sprite_main(s8 *dummy, struct Sprite *sprite)
{
    struct Ball *ball = &ballInfo[sprite->userVar];

    sprite->opacity += (1.0f - sprite->opacity) * 0.1;
    if (sprite->bmpId >= 100)
        sprintf(sprite->text, "BONUS  +%3d", ball->bananaBonus);
    else if (sprite->bmpId >= 10)
        sprintf(sprite->text, "BONUS  +%2d", ball->bananaBonus);
    else
        sprintf(sprite->text, "BONUS  +%1d", ball->bananaBonus);
}

static void bonus_banana_sprite_main(s8 *dummy, struct Sprite *sprite)
{
    sprite->opacity += (1.0f - sprite->opacity) * 0.1;
}

static void win_streak_sprite_main(s8 *dummy, struct Sprite *sprite)
{
    if (sprite->counter > 0)
        sprite->counter--;
    if (sprite->counter == 0)
    {
        if (RAND_FLOAT() < 0.01)
            sprite->counter = 45;
    }
    sprite->opacity += (1.0f - sprite->opacity) * 0.1;
    sprite->addR = mathutil_sin(sprite->counter * 0x2B8) * 255.0f;
    sprite->addG = sprite->addR;
    sprite->addB = sprite->addR;
}

void rank_icon_sprite_main(s8 *dummy, struct Sprite *sprite)
{
    sprite->counter++;
    if (sprite->counter <= 15)
    {
        sprite->scaleX = 1.0 + 0.2f * (15.0f - sprite->counter);
        sprite->scaleY = sprite->scaleX;
        sprite->opacity = sprite->counter * 0.066666;
    }
    if (sprite->counter > 60 && sprite->counter < 0x69)
        sprite->y -= 1.0f;
    if (sprite->userVar != 0 && sprite->counter == 0x78)
    {
        struct Ball *ball = &ballInfo[sprite->bmpId];
        struct Viewport *vp = &cameraInfo[ball->playerId].sub28.vp;
        struct Sprite *countSprite = create_sprite();
        struct Sprite *bananaSprite;

        if (countSprite != NULL)
        {
            countSprite->x = (vp->left + vp->width * 0.5) * 640.0;
            countSprite->y = (vp->top + vp->height * 0.5) * 480.0;
            countSprite->fontId = FONT_JAP_24x24_2;
            countSprite->textAlign = ALIGN_CC;
            countSprite->userVar = ball->playerId;
            countSprite->mulR = 255;
            countSprite->mulG = 255;
            countSprite->mulB = 0;
            countSprite->opacity = 0.0f;
            countSprite->bmpId = ball->bananaBonus;  //! Huh? This is a text sprite
            countSprite->mainFunc = bonus_count_sprite_main;
            sprintf(countSprite->text, "BONUS  +000", ball->bananaBonus);  //! bad format
            bananaSprite = create_child_sprite(countSprite);
            if (bananaSprite != NULL)
            {
                bananaSprite->type = SPRITE_TYPE_BITMAP;
                bananaSprite->x = -140.0f;
                if (ball->bananaBonus < 10)
                    bananaSprite->x += 48.0f;
                else if (ball->bananaBonus < 100)
                    bananaSprite->x += 24.0f;
                bananaSprite->y = -2.0f;
                bananaSprite->bmpId = BMP_COM_banana_01;
                bananaSprite->textAlign = ALIGN_CC;
                bananaSprite->scaleX = 0.3f;
                bananaSprite->scaleY = 0.3f;
                bananaSprite->opacity = 0.0f;
                bananaSprite->mainFunc = bonus_banana_sprite_main;
                sprintf(bananaSprite->text, "bonus banana.pic");
            }
        }
        if ((modeCtrl.courseFlags & (1 << 11)) && ball->winStreak > 1)
        {
            struct Sprite *sprite = create_sprite();

            if (sprite != NULL)
            {
                sprite->x = (vp->left + vp->width * 0.5) * 640.0 + 130.0;
                sprite->y = (vp->top + vp->height * 0.5) * 480.0 + 21.0;
                sprite->fontId = FONT_ASC_20x20P;
                sprite->textAlign = ALIGN_CC;
                sprite->mulR = 255;
                sprite->mulG = 192;
                sprite->mulB = 0;
                sprite->opacity = 0.0f;
                sprite->scaleX = 0.45f;
                sprite->scaleY = 0.7f;
                sprite->counter = 45;
                sprite->mainFunc = win_streak_sprite_main;
                sprintf(sprite->text, "STRAIGHT\n VICTORIES X %d", ball->winStreak);
            }
        }
    }
}

static void rank_icon_sprite_draw(struct Sprite *sprite)
{
    NLsprarg params;
    struct Struct801818D0 *r6;
    struct TPLTextureHeader *tex;

    params.sprno = BMP_NML_game_rank;
    params.ang = sprite->rotation;
    params.trnsl = sprite->opacity;
    params.listType = NLSPR_LISTTYPE_AUTO;
    params.attr = (sprite->flags & ~0xF) | NLSPR_DISP_CC;
    params.base_color = RGBA(sprite->mulR, sprite->mulG, sprite->mulB, (u8)(sprite->opacity * 255.0f));
    params.offset_color = RGBA(sprite->addR, sprite->addG, sprite->addB, 0);
    r6 = &rankTexOffsets[sprite->userVar];
    tex = &bitmapGroups[(params.sprno >> 8) & 0xFF].tpl->texHeaders[params.sprno & 0xFF];
    params.x = sprite->x;
    params.y = sprite->y;
    params.z = sprite->depth;
    params.u0 = r6->u1 / tex->width;
    params.v0 = r6->v1 / tex->height;
    params.u1 = params.u0 + r6->u2 / tex->width;
    params.v1 = params.v0 + r6->v2 / tex->height;
    params.zm_x = (params.u1 - params.u0) * sprite->scaleX;
    params.zm_y = (params.v1 - params.v0) * sprite->scaleY;
    nlSprPut(&params);
}

void create_rank_icon(struct Ball *ball)
{
    struct Viewport *vp;
    struct Sprite *rankIcon;

    if (ball->rank == 0)
        return;
    vp = &cameraInfo[ball->playerId].sub28.vp;
    rankIcon = create_sprite();
    if (rankIcon == NULL)
        return;
    rankIcon->x = (vp->left + vp->width * 0.5) * 640.0;
    rankIcon->y = (vp->top + vp->height * 0.5) * 480.0;
    rankIcon->type = 1;
    rankIcon->textAlign = ALIGN_CC;
    rankIcon->userVar = ball->rank;
    rankIcon->bmpId = ball->playerId;
    rankIcon->counter = 0;
    rankIcon->mainFunc = rank_icon_sprite_main;
    rankIcon->drawFunc = rank_icon_sprite_draw;
    strcpy(rankIcon->text, "ranking");
}

void func_800245E4(struct Ball *ball, int goalId, int c)
{
    infoWork.goalEntered = goalId;
    infoWork.unkE = c;
    infoWork.unk10 = ball->vel;
    infoWork.unk1C = infoWork.timerCurr;
    if (c > 0)
    {
        struct AnimGroupInfo *r29 = &animGroups[c];
        struct StageGoal *goal = &decodedStageLzPtr->goals[goalId];
        Vec sp20;
        Vec sp14;

        mathutil_mtxA_from_mtx(r29->prevTransform);
        mathutil_mtxA_tf_point(&goal->pos, &sp14);
        mathutil_mtxA_from_mtx(r29->transform);
        mathutil_mtxA_tf_point(&goal->pos, &sp20);

        infoWork.unk10.x += sp14.x - sp20.x;
        infoWork.unk10.y += sp14.y - sp20.y;
        infoWork.unk10.z += sp14.z - sp20.z;
    }
}

#pragma force_active on
int func_800246F4(struct Ball *ball)
{
    struct PhysicsBall sp18;
    struct StageAnimGroup *r30;
    int i;

    if (ball->pos.y < *decodedStageLzPtr->pFallOutY)
        return 1;
    init_physball_from_ball(ball, &sp18);
    r30 = decodedStageLzPtr->animGroups;
    for (i = 0; i < decodedStageLzPtr->animGroupCount; i++, r30++)
    {
        struct StageCollHdr_child2 *r28;
        int j;

        if (i != sp18.animGroupId)
            tf_physball_to_anim_group_space(&sp18, i);
        r28 = r30->unk88;
        for (j = 0; j < r30->unk84; j++, r28++)
        {
            Vec spC;

            mathutil_mtxA_from_translate(&r28->unk0);
            mathutil_mtxA_rotate_z(r28->unk1C);
            mathutil_mtxA_rotate_y(r28->unk1A);
            mathutil_mtxA_rotate_x(r28->unk18);
            mathutil_mtxA_rigid_inv_tf_point(&sp18.pos, &spC);
            spC.x /= r28->unkC.x;
            spC.y /= r28->unkC.y;
            spC.z /= r28->unkC.z;
            if (spC.x < -0.5 || spC.x > 0.5)
                continue;
            if (spC.y < -0.5 || spC.y > 0.5)
                continue;
            if (spC.z < -0.5 || spC.z > 0.5)
                continue;
            return 1;
        }
    }
    return 0;
}

// for time bonus?
void func_80024860(struct Ball *ball)
{
    lbl_802F1DFC = ball->ape->charaId;
    u_somePlayerId = ball->playerId;
    if (infoWork.timerCurr > (infoWork.timerMax >> 1))
    {
        if (infoWork.unk22 != 1)
            u_play_sound_0(0x2859);
        else
            u_play_sound_0(0x2858);
    }
    else
        u_play_sound_0(0x281B);
}
