/**
 * game.c - Implements the "game" mode, which handles gameplay control flow for the main game
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "course.h"
#include "effect.h"
#include "ending.h"
#include "event.h"
#include "game.h"
#include "hud.h"
#include "info.h"
#include "input.h"
#include "item.h"
#include "light.h"
#include "load.h"
#include "mathutil.h"
#include "memcard.h"
#include "minimap.h"
#include "mode.h"
#include "name_entry.h"
#include "pool.h"
#include "recplay.h"
#include "recplay_cmpr.h"
#include "rend_efc.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stobj.h"
#include "textbox.h"
#include "window.h"
#include "world.h"

s8 lbl_802F1C25;
s8 lbl_802F1C24;
s32 lbl_802F1C20;
s32 lbl_802F1C1C;
s32 lbl_802F1C18;

void mode_game_func(void)
{
    if (!(gameSubmode > SMD_GAME_TOP && gameSubmode < SMD_GAME_BOTTOM))
    {
        window_set_cursor_pos(10, 10);
        window_printf_2("sub_mode: error %d in Game", gameSubmode);
        return;
    }

    gameSubmodeFuncs[gameSubmode]();
}

void submode_game_first_init_func(void)
{
    int i;

    modeCtrl.courseFlags &= ~(1 << 6);
    modeCtrl.currPlayer = 0;
    event_finish_all();
    free_all_bitmap_groups_except_com();
    func_800249D4();
    func_80044920();
    event_start(EVENT_BALL);
    event_start(EVENT_SOUND);
    call_bitmap_load_group(BMP_NML);
    BALL_FOREACH( ball->state = 1; )
    BALL_FOREACH( ball->ape->flags |= (1 << 5); )
    minimap_set_state(MINIMAP_STATE_4);
    info_init_first();
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1)
        reset_earned_play_points();
    if (stageSelection.difficulty != 0)
        infoWork.currFloor = stageSelection.levelNum;
    lbl_802F1C18 = -1;
    func_800AEAD0();
    func_800AF164();
    recplay_init_first();
    if (modeCtrl.courseFlags & (1 << 0))
        start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 30);
    lbl_802F1C20 = -1;
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MAIN_NORMAL:
        for (i = 0; i < 4; i++)
            setup_camera_viewport(i, 0.0f, 0.0f, 1.0f, 1.0f);
        break;
    default:
        if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION && modeCtrl.playerCount == 3)
        {
            switch (modeCtrl.splitscreenMode)
            {
            case SPLITSCREEN_1P_WIDE:
            default:
                camera_setup_splitscreen_viewports(3);
                break;
            case SPLITSCREEN_2P_WIDE:
                setup_camera_viewport(0, 0.0f, 0.5f, 0.5f, 0.5f);
                setup_camera_viewport(1, 0.0f, 0.0f, 1.0f, 0.5f);
                setup_camera_viewport(2, 0.5f, 0.5f, 0.5f, 0.5f);
                setup_camera_viewport(3, 0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case SPLITSCREEN_3P_WIDE:
                setup_camera_viewport(0, 0.0f, 0.0f, 0.5f, 0.5f);
                setup_camera_viewport(1, 0.5f, 0.0f, 0.5f, 0.5f);
                setup_camera_viewport(2, 0.0f, 0.5f, 1.0f, 0.5f);
                setup_camera_viewport(3, 0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case SPLITSCREEN_4_SPLIT:
                camera_setup_splitscreen_viewports(4);
                break;
            }
        }
        else
            camera_setup_splitscreen_viewports(modeCtrl.playerCount);
        break;
    }

    gameSubmodeRequest = SMD_GAME_READY_INIT;
}

/* when restarting a level from the Continue screen */
void submode_game_restart_init_func(void)
{
    event_finish_all();
    free_all_bitmap_groups_except_com();
    func_80044920();
    event_start(EVENT_BALL);
    event_start(EVENT_SOUND);
    call_bitmap_load_group(BMP_NML);
    BALL_FOREACH( ball->state = 1; )
    minimap_set_state(MINIMAP_STATE_4);
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1)
        reset_earned_play_points();
    gameSubmodeRequest = SMD_GAME_READY_INIT;
}

const s16 countdownSounds[] =
{
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2A,
    0x2B,
    0x2C,
};

const s16 backgroundSongs[] =
{
    -1,
#define DEFINE_BG(id, fname, oldfname, song, backdropColor) song,
    BACKGROUND_LIST
#undef DEFINE_BG
    // What are these for?
    -1,
    -1,
    -1,
    -1,
};

void submode_game_ready_init_func(void)
{
    int r30;

    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 120;
    u_clear_buffers_2_and_5();
    event_finish_all();
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MAIN_NORMAL:
        if (!(modeCtrl.courseFlags & (1 << 8)))
            u_init_player_data_1();
        else
            u_init_player_data_2();
        floor_to_stage_id(modeCtrl.difficulty, infoWork.currFloor, modeCtrl.courseFlags);
        break;
    }
    load_stage(loadingStageId);
    event_start(EVENT_STAGE);
    event_start(EVENT_WORLD);
    event_start(EVENT_BALL);
    event_start(EVENT_STOBJ);
    event_start(EVENT_INFO);
    event_start(EVENT_ITEM);
    event_start(EVENT_OBJ_COLLISION);
    if (modeCtrl.courseFlags & 1)
        event_start(EVENT_COURSE);
    event_start(EVENT_MINIMAP);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    event_start(EVENT_RECPLAY);
    event_start(EVENT_EFFECT);
    event_start(EVENT_REND_EFC);
    event_start(EVENT_BACKGROUND);
    event_start(EVENT_VIBRATION);
    rend_efc_mirror_enable();
    event_suspend(EVENT_WORLD);
    light_init(currStageId);

    // preload the next stage early to reduce loading times
    r30 = u_get_next_stage_id_not_practice();
    if (r30 != -1 && r30 != lbl_802F1C18)
    {
        preload_stage_files(r30);
        lbl_802F1C18 = r30;
        if (debugFlags & (1 << 2))
            printf("pre_load_stage init:%d now:%d\n", r30, currStageId);
    }

    infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_08;
    BALL_FOREACH( ball->state = 2; )
    camera_set_state_all(CAMERA_STATE_READY_INIT);
    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION
     && modeCtrl.playerCount == 3
     && modeCtrl.splitscreenMode == SPLITSCREEN_4_SPLIT)
    {
        cameraInfo[3].flags |= 0x70;
        cameraInfo[3].state = 0x44;
    }
    if (infoWork.attempts == 1)
    {
        modeCtrl.submodeTimer = 360;
        camera_set_state_all(0x26);
        hud_show_stage_name_banner();
    }
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MAIN_COMPETITION:
        hud_show_competition_mode_info();
        break;
    default:
        hud_show_normal_mode_info();
        break;
    }
    r30 = backgroundSongs[backgroundInfo.bgId];
    if (r30 != -1 && r30 != lbl_802014E0.unk0 && r30 + 1 != lbl_802014E0.unk0)
    {
        if (r30 == lbl_802F1C20)
        {
            u_play_music(r30 + 1, 0);
            lbl_802F1C20 = -1;
        }
        else
        {
            u_play_music(r30, 0);
            if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
                lbl_802F1C20 = r30;
        }
    }
    else
    {
        if (lbl_802014E0.unk0 == -1)
            u_play_music(1, 3);
        else if (r30 == -1)
            u_play_music(0, 1);
    }
    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        int sp4[] = { 0x186, 0x187, 0x188, 0x189, 0x18A };
        int soundId = sp4[infoWork.currFloor - 1];

        if (lbl_802F1FB0 == infoWork.currFloor)
            soundId = 0x184;
        u_play_sound_0(soundId);
    }
    lbl_802F1F80 = ballInfo[modeCtrl.currPlayer].score;
    gameSubmodeRequest = SMD_GAME_READY_MAIN;
}

void submode_game_ready_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (infoWork.attempts == 1 && (g_currPlayerButtons[0] & PAD_BUTTON_A) && modeCtrl.submodeTimer > 120)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 120)
    {
        struct Sprite *sprite = find_sprite_with_tag(15);
        if (sprite != NULL)
            sprite->userVar = 15;
        hud_show_ready_banner(120);
        minimap_set_state(MINIMAP_STATE_OPEN);
    }
    if (modeCtrl.submodeTimer == 24.0)
        BALL_FOREACH( ball->state = BALL_STATE_3; )
    if (g_currPlayerButtons[2] & PAD_BUTTON_A)
        minimap_zoom();
    if (--modeCtrl.submodeTimer <= 0)
        gameSubmodeRequest = SMD_GAME_PLAY_INIT;
}

/**
 * Play Submode
 */

void submode_game_play_init_func(void)
{
    int i;

    if (debugFlags & 0xA)
        return;
    event_resume(EVENT_WORLD);
    hud_show_go_banner(60);
    infoWork.flags &= ~(INFO_FLAG_TIMER_PAUSED|INFO_FLAG_08);
    u_recplay_reset_recording_state();
    g_recplayInfo.u_playerId = 0;
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MAIN_COMPETITION:
        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            g_recplayInfo.u_replayIndexes[i] = i + 7;
            recplay_set_player_replay_index(i, g_recplayInfo.u_replayIndexes[i]);
        }
        break;
    case GAMETYPE_MAIN_NORMAL:
        g_recplayInfo.u_playerId = modeCtrl.currPlayer;
        // fall through
    default:
        g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId] = recplay_find_new_index();
        recplay_set_player_replay_index(g_recplayInfo.u_playerId, g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]);
        break;
    }
    recplay_start_recording();
    WORLD_FOREACH( world->state = WORLD_STATE_INPUT_INIT; )
    camera_set_state_all(0);
    gameSubmodeRequest = SMD_GAME_PLAY_MAIN;
}

void submode_game_play_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (infoWork.timerCurr % 60 == 59)  // each second
    {
        if (infoWork.timerCurr <= 10 * 60)
        {
            // countdown
            u_play_sound_0(6);
            u_play_sound_0(countdownSounds[infoWork.timerCurr / 60]);
        }
        else
            u_play_sound_0(0x3D806);
        if (infoWork.timerCurr / 60 == 10)
        {
            u_play_sound_0(7);  // hurry up?
            hud_show_hurry_up_banner();
        }
    }
    if (infoWork.timerCurr <= 5 * 60 && infoWork.timerCurr % 60 == 43)
        u_play_sound_0(6);
    if (infoWork.timerCurr <= 10 * 60 && infoWork.timerCurr % 60 == 51)
        u_play_sound_0(6);
    if (g_currPlayerButtons[2] & PAD_BUTTON_A)
        minimap_zoom();

    if (infoWork.flags & INFO_FLAG_GOAL)
    {
        infoWork.flags &= ~INFO_FLAG_GOAL;
        gameSubmodeRequest = SMD_GAME_GOAL_INIT;
    }
    else if (infoWork.flags & INFO_FLAG_TIMEOVER)
    {
        infoWork.flags &= ~INFO_FLAG_TIMEOVER;
        BALL_FOREACH( ball->state = BALL_STATE_0; )
        gameSubmodeRequest = SMD_GAME_TIMEOVER_INIT;
    }
    else if (infoWork.flags & INFO_FLAG_FALLOUT)
    {
        infoWork.flags &= ~INFO_FLAG_FALLOUT;
        camera_set_state_all(4);
        recplay_cmpr_record_score();
        gameSubmodeRequest = SMD_GAME_RINGOUT_INIT;
    }
    else if (infoWork.flags & INFO_FLAG_BONUS_CLEAR)
    {
        infoWork.flags &= ~INFO_FLAG_BONUS_CLEAR;
        recplay_cmpr_record_score();
        gameSubmodeRequest = SMD_GAME_BONUS_CLEAR_INIT;
    }
}

void submode_game_goal_init_func(void)
{
    int nextStage;

    if (debugFlags & 0xA)
        return;

    if (!(infoWork.flags & INFO_FLAG_13))
    {
        modeCtrl.submodeTimer = 360;
        modeCtrl.courseFlags &= ~(1 << 10);
    }
    else
    {
        modeCtrl.submodeTimer = 120;
        modeCtrl.courseFlags &= ~(1 << 10);
        u_play_sound_0(11);
        u_play_sound_0(0x128);
    }
    modeCtrl.unk3C = modeCtrl.submodeTimer;

    // preload the next stage early to reduce loading times
    nextStage = u_get_next_stage_id_not_practice();
    if (nextStage != -1 && nextStage != lbl_802F1C18)
    {
        preload_stage_files(nextStage);
        lbl_802F1C18 = nextStage;
    }

    minimap_set_state(MINIMAP_STATE_CLOSE);
    camera_set_state_all(14);
    if (!(infoWork.flags & INFO_FLAG_13))
        hud_show_goal_banner(0x168);
    else
        hud_show_time_over_banner(120);
    if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE) && modeCtrl.gameType != GAMETYPE_MAIN_COMPETITION)
        u_give_points(3, 0);
    if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
        recplay_cmpr_record_score();
    gameSubmodeRequest = SMD_GAME_GOAL_MAIN;
}

void submode_game_goal_main_func(void)
{
    int r31;

    if (debugFlags & 0xA)
        return;

    r31 = (infoWork.flags & INFO_FLAG_13) != 0;
    if (!r31 && modeCtrl.submodeTimer == 330)
        u_play_sound_0(8);
    if (modeCtrl.submodeTimer == modeCtrl.unk3C - 60)
        recplay_stop_recording();
    if (!r31)
    {
        BALL_FOREACH(
            if (!(ball->flags & BALL_FLAG_REVERSE_GRAVITY) && (ball->ape->flags & (1 << 14)))
            {
                modeCtrl.courseFlags |= 0x400;
                u_play_sound_0(0x126);
            }
        )
    }
    if (!r31 && (modeCtrl.courseFlags & (1 << 10))
     && modeCtrl.submodeTimer > 60 && modeCtrl.submodeTimer < 240)
        modeCtrl.submodeTimer = 60;
    if (--modeCtrl.submodeTimer <= 0)
    {
        infoWork.flags &= ~INFO_FLAG_13;
        gameSubmodeRequest = SMD_GAME_GOAL_REPLAY_INIT;
    }
}

void submode_game_goal_replay_init_func(void)
{
    float f1;
    int r31;

    if (debugFlags & 0xA)
        return;

    event_finish(EVENT_VIBRATION);
    modeCtrl.submodeTimer = 210;
    f1 = mathutil_vec_len(&infoWork.unk10);
    modeCtrl.submodeTimer += f1 * 300.0;
    if (modeCtrl.submodeTimer > 300.0)
        modeCtrl.submodeTimer = 300;
    modeCtrl.courseFlags &= ~(1 << 10);
    modeCtrl.unk18 = 30;
    BALL_FOREACH( ball->flags &= ~BALL_FLAG_REVERSE_GRAVITY; )
    BALL_FOREACH( ball->state = BALL_STATE_REPLAY_INIT_2; )
    WORLD_FOREACH( world->state = 6; )
    camera_set_state_all(16);
    ball_set_ape_flags_80037B20();
    erase_effect(ET_PAPERFRAG);
    erase_effect(ET_LEVITATE);
    g_recplayInfo.u_playerId = infoWork.playerId;
    infoWork.flags |= INFO_FLAG_REPLAY;
    g_recplayInfo.u_timeOffset = MIN(modeCtrl.submodeTimer - 60, recplay_get_time(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]));
    animate_anim_groups(recplay_get_stage_timer(g_recplayInfo.u_timeOffset, g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]));
    r31 = recplay_get_info_timer(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId], g_recplayInfo.u_timeOffset);
    release_captured_item(r31);
    relink_goaltape(r31);
    hud_show_replay_text(modeCtrl.submodeTimer);
    if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE) && modeCtrl.gameType != GAMETYPE_MAIN_COMPETITION)
        u_give_points(2, 0);
    recplay_cmpr_record_score();
    gameSubmodeRequest = SMD_GAME_GOAL_REPLAY_MAIN;
}

void submode_game_goal_replay_main_func(void)
{
    int nextStage;

    if (debugFlags & 0xA)
        return;

    if (currentBall->state == 4)
        infoWork.flags &= ~INFO_FLAG_REPLAY;

    BALL_FOREACH(
        if (!(ball->flags & BALL_FLAG_REVERSE_GRAVITY) && (ball->ape->flags & (1 << 14)))
        {
            ball->flags &= ~(BALL_FLAG_08|BALL_FLAG_IGNORE_GRAVITY);
            ball->flags |= BALL_FLAG_REVERSE_GRAVITY;
            modeCtrl.courseFlags |= 0x400;
            u_play_sound_0(0x126);
        }
    )

    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        BOOL tick = FALSE;

        // tick banana counters
        BALL_FOREACH(
            if ((globalAnimTimer & 3) == 0 && ball->bananaBonus > 0)
            {
                int bananas = 1;
                if (ball->bananaBonus > 100)
                    bananas = 41;
                else if (ball->bananaBonus > 10)
                    bananas = 3;
                ball->bananas += bananas;
                ball->bananaBonus -= bananas;
                ball->bananas = MIN(ball->bananas, 999);
                tick = TRUE;
            }
        )
        if (tick)
            u_play_sound_0(0x67);
    }
    modeCtrl.unk18--;
    nextStage = u_get_next_stage_id();
    if (!is_load_queue_not_empty()
     && (g_currPlayerButtons[2] & PAD_BUTTON_A)
     && modeCtrl.unk18 < 0 && nextStage > 0)
        modeCtrl.submodeTimer = 0;
    if (nextStage < 0 && modeCtrl.submodeTimer == 60)
    {
        start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 61);
        u_play_music(60, 2);
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    infoWork.flags &= ~INFO_FLAG_REPLAY;
    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        BALL_FOREACH(
            if (ball->bananaBonus > 0)
            {
                ball->bananas += ball->bananaBonus;
                ball->bananaBonus = 0;
                ball->bananas = MIN(ball->bananas, 999);
            }
        )
    }

    // go to result screen if this is the end of competition mode
    if (nextStage < 0 && modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        gameSubmodeRequest = SMD_GAME_RESULT_INIT;
        return;
    }

    if (nextStage < 0)  // reached end of level set
    {
        u32 gotoExtra;

        if (modeCtrl.difficulty == DIFFICULTY_EXPERT)
            gotoExtra = ((modeCtrl.courseFlags & 1) && infoWork.continuesUsed == 0);
        else
            gotoExtra = ((modeCtrl.courseFlags & 1) && infoWork.livesLost == 0);

        if (gotoExtra && (modeCtrl.courseFlags & COURSE_FLAG_EXTRA) && modeCtrl.difficulty != DIFFICULTY_EXPERT)
            gotoExtra = FALSE;
        if (gotoExtra && (modeCtrl.courseFlags & COURSE_FLAG_MASTER))
            gotoExtra = FALSE;

        if (gotoExtra)
            gameSubmodeRequest = SMD_GAME_EXTRA_INIT;
        else
        {
            func_80017140();
            gameSubmodeRequest = SMD_GAME_ENDING_INIT;
        }
    }
    else
    {
        // advance to next stage
        if (modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        {
            infoWork.attempts++;
            u_play_music(100, 8);
        }
        else
            infoWork.attempts = 1;
        loadingStageId = nextStage;
        gameSubmodeRequest = SMD_GAME_READY_INIT;
    }
}

// Continue screen

void submode_game_continue_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    func_80017140();
    modeCtrl.submodeTimer = 659;
    modeCtrl.unk10 = 1;
    modeCtrl.courseFlags &= ~(1 << 2);
    event_finish(EVENT_STAGE);
    event_finish(EVENT_WORLD);
    event_finish(EVENT_STOBJ);
    event_finish(EVENT_ITEM);
    event_finish(EVENT_OBJ_COLLISION);
    event_finish(EVENT_INFO);
    event_finish(EVENT_SPRITE);
    event_finish(EVENT_VIBRATION);
    event_start(EVENT_SPRITE);
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL)
    {
        if (get_available_continues() == 0)
        {
            modeCtrl.unk10 = 0;
            modeCtrl.submodeTimer = (modeCtrl.playerCount == 1) ? 480 : 180;
            modeCtrl.courseFlags |= (1 << 2);
        }
        else
            hud_show_continue_interface();
    }
    else
        hud_show_continue_interface();
    u_smth_with_lights_smd_continue(currStageId);

    BALL_FOREACH( ball->state = 18; )
    BALL_FOREACH(
        if (ball->ape != NULL)
            ball->ape->flags |= 0x100;
    )

    camera_set_state_all(0x19);
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1)
    {
        record_play_points();
        show_play_points_textbox(1, 0x140, 0x198);
    }
    lbl_802F1C20 = lbl_802014E0.unk0;
    u_play_music(40, 0);
    gameSubmodeRequest = SMD_GAME_CONTINUE_MAIN;
}

void submode_game_continue_main_func(void)
{
    int normalSinglePlayer = (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1);

    if (debugFlags & 0xA)
        return;

    if (!(modeCtrl.courseFlags & (1 << 2)))
    {
        if (!normalSinglePlayer || is_play_points_textbox_done())
        {
            int r29 = modeCtrl.unk10;

            if ((g_currPlayerButtons[2] & PAD_BUTTON_LEFT) || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_LEFT))
                r29 = 1;
            if ((g_currPlayerButtons[2] & PAD_BUTTON_RIGHT) || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_RIGHT))
                r29 = 0;
            if (r29 != modeCtrl.unk10)
            {
                u_play_sound_0(9);
                modeCtrl.unk10 = r29;
            }
        }
        if (!normalSinglePlayer || is_play_points_textbox_done())
        {
            if (g_currPlayerButtons[2] & PAD_BUTTON_A)
            {
                currentBall->ape->flags &= ~(1 << 8);
                if (modeCtrl.unk10 == 1)
                {
                    infoWork.continuesUsed++;
                    infoWork.attempts = 1;
                    currentBall->ape->flags |= 0x800;
                    textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
                    u_play_sound_0(10);
                    u_play_sound_0(80);
                    modeCtrl.submodeTimer = 60;
                    modeCtrl.courseFlags |= (1 << 2);
                    textbox_set_properties(1, TEXTBOX_STATE_FADEOUT, NULL);
                }
                else
                {
                    textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
                    u_play_sound_0(48);
                    currentBall->ape->flags |= 0x40000;
                    modeCtrl.submodeTimer = 60;
                    modeCtrl.courseFlags |= (1 << 2);
                }
            }
        }
    }
    else
    {
        if (normalSinglePlayer && modeCtrl.unk10 == 0 && modeCtrl.submodeTimer > 180
         && is_play_points_textbox_done() && (g_currPlayerButtons[2] & PAD_BUTTON_A))
            modeCtrl.submodeTimer = 180;
        if (modeCtrl.unk10 == 0 && modeCtrl.submodeTimer == 60)
        {
            if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL
             && (u_calc_rank_of_score_record(modeCtrl.difficulty, &lbl_802C67D4[modeCtrl.currPlayer][0]) >= 0
                 || (dipSwitches & (DIP_DEBUG|DIP_NAMEENTRY)) == (DIP_DEBUG|DIP_NAMEENTRY)))
                start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(255, 255, 255, 0), modeCtrl.submodeTimer);
            else
                start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), modeCtrl.submodeTimer);
            textbox_set_properties(1, TEXTBOX_STATE_FADEOUT, NULL);
        }

        if (--modeCtrl.submodeTimer > 0)
            return;
        if (modeCtrl.unk10 == 1)
            gameSubmodeRequest = SMD_GAME_RESTART_INIT;
        else if (modeCtrl.courseFlags & COURSE_FLAG_EXTRA)
        {
            if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1)
                modeCtrl.courseFlags |= 0x100000;
            gameSubmodeRequest = SMD_GAME_ENDING_INIT;
        }
        else
            gameSubmodeRequest = SMD_GAME_NAMEENTRY_READY_INIT;
    }
}

void submode_game_timeover_init_func(void)
{
    int i;

    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 120;
    event_suspend(EVENT_STAGE);
    event_finish(EVENT_WORLD);
    event_finish(EVENT_VIBRATION);
    BALL_FOREACH( ball->state = 0; )
    minimap_set_state(MINIMAP_STATE_CLOSE);
    if (!(infoWork.flags & INFO_FLAG_BONUS_STAGE))
        u_play_sound_0(11);
    u_play_sound_0(0x128);
    camera_set_state_all(21);
    for (i = 0; i < 4; i++)
    {
        Vec sp8;

        sp8.x = 0.0f;
        sp8.y = 0.75f;
        sp8.z = RAND_FLOAT() - 0.5f;
        shake_camera(i, modeCtrl.submodeTimer, &sp8);
    }
    recplay_stop_recording();
    hud_show_time_over_banner(modeCtrl.submodeTimer);
    BALL_FOREACH( u_play_sound_0(28); )
    gameSubmodeRequest = SMD_GAME_TIMEOVER_MAIN;
}

void submode_game_timeover_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        loadingStageId = u_get_next_stage_id();
        if (loadingStageId < 0 && modeCtrl.submodeTimer == 60)
        {
            start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 61);
            u_play_music(60, 2);
        }
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    if (lose_life())
    {
        // still has lives
        if ((infoWork.flags & INFO_FLAG_BONUS_STAGE) || modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
        {
            loadingStageId = u_get_next_stage_id();
            infoWork.attempts = (modeCtrl.gameType != GAMETYPE_MAIN_PRACTICE) ? 1 : infoWork.attempts + 1;
        }
        else
            infoWork.attempts++;
        if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION && loadingStageId < 0)
            gameSubmodeRequest = SMD_GAME_RESULT_INIT;
        else
            gameSubmodeRequest = SMD_GAME_READY_INIT;
    }
    else
    {
        // no more lives
        modeCtrl.unk14 = 1;
        switch (modeCtrl.gameType)
        {
        case GAMETYPE_MAIN_COMPETITION:
            gameSubmodeRequest = SMD_GAME_OVER_INIT;
            break;
        default:
            gameSubmodeRequest = SMD_GAME_CONTINUE_INIT;
            break;
        }
    }
}

void submode_game_ringout_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 270;
    modeCtrl.unk18 = 60;
    minimap_set_state(MINIMAP_STATE_CLOSE);
    u_play_sound_0((currentBall->lives == 1) ? 81 : 29);
    u_play_sound_0(21);
    lbl_802F1C1C = -1;
    hud_show_fallout_banner(modeCtrl.submodeTimer);
    gameSubmodeRequest = SMD_GAME_RINGOUT_MAIN;
}

void submode_game_ringout_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (currentBall->state == 4)
        infoWork.flags &= ~INFO_FLAG_REPLAY;
    if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
    {
        if (modeCtrl.submodeTimer == 260.0)
        {
            recplay_stop_recording();
            modeCtrl.submodeTimer = 110;
        }
    }
    else
    {
        if (modeCtrl.submodeTimer == 240.0)
        {
            recplay_stop_recording();
            BALL_FOREACH( ball->state = BALL_STATE_REPLAY_INIT_1; )
            WORLD_FOREACH( world->state = 8; )
            camera_set_state_all(5);
            g_recplayInfo.u_timeOffset = MIN(recplay_get_time(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]), 120.0f);
            animate_anim_groups(recplay_get_stage_timer(g_recplayInfo.u_timeOffset, g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]));
            infoWork.flags |= INFO_FLAG_REPLAY;
            release_captured_item(recplay_get_info_timer(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId], g_recplayInfo.u_timeOffset));
            hud_show_replay_text(modeCtrl.submodeTimer);
            lbl_802F1C1C = 30;
        }
        if (modeCtrl.submodeTimer == 150.0)
        {
            BALL_FOREACH( ball->state = BALL_STATE_REPLAY_INIT_1; )
            camera_set_state_all(7);
            g_recplayInfo.u_timeOffset = MIN(recplay_get_time(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]), modeCtrl.submodeTimer - 60);
            animate_anim_groups(recplay_get_stage_timer(g_recplayInfo.u_timeOffset, g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]));
            infoWork.flags |= INFO_FLAG_REPLAY;
            release_captured_item(recplay_get_info_timer(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId], g_recplayInfo.u_timeOffset));
            lbl_802F1C1C = 60;
        }
    }
    modeCtrl.unk18--;
    if ((g_currPlayerButtons[2] & PAD_BUTTON_A) && modeCtrl.unk18 < 0
     && !(infoWork.flags & (INFO_FLAG_BONUS_STAGE|INFO_FLAG_BONUS_CLEAR)) && currentBall->lives > 1)
    {
        recplay_stop_recording();
        modeCtrl.submodeTimer = 0;
    }
    if (lbl_802F1C1C > 0)
        lbl_802F1C1C--;
    if (lbl_802F1C1C == 0 && currentBall->vel.y < -0.2)
    {
        u_play_sound_0((currentBall->lives == 1) ? 81 : 29);
        u_play_sound_0(21);
        lbl_802F1C1C = -1;
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    infoWork.flags &= ~INFO_FLAG_REPLAY;
    event_suspend(EVENT_STAGE);

    if (lose_life())
    {
        // still has lives
        if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
        {
            loadingStageId = u_get_next_stage_id();
            infoWork.attempts = (modeCtrl.gameType != GAMETYPE_MAIN_PRACTICE) ? 1 : infoWork.attempts + 1;
        }
        else
            infoWork.attempts++;
        gameSubmodeRequest = SMD_GAME_READY_INIT;
    }
    else
    {
        // no more lives
        modeCtrl.unk14 = 0;
        switch (modeCtrl.gameType)
        {
        case GAMETYPE_MAIN_COMPETITION:
            gameSubmodeRequest = SMD_GAME_OVER_INIT;
            break;
        default:
            gameSubmodeRequest = SMD_GAME_CONTINUE_INIT;
            break;
        }
    }
}

void submode_game_bonus_clear_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 180;
    modeCtrl.courseFlags &= ~(1 << 10);
    event_finish(EVENT_WORLD);
    event_finish(EVENT_VIBRATION);
    minimap_set_state(MINIMAP_STATE_CLOSE);
    BALL_FOREACH( ball->state = BALL_STATE_GOAL_INIT; )
    BALL_FOREACH( ball->flags |= BALL_FLAG_08|BALL_FLAG_IGNORE_GRAVITY; )
    camera_set_state_all(14);
    if (infoWork.flags & INFO_FLAG_10)
        hud_show_time_over_banner(modeCtrl.submodeTimer);
    else
    {
        hud_show_perfect_banner(modeCtrl.submodeTimer);
        recplay_stop_recording();
    }
    gameSubmodeRequest = SMD_GAME_BONUS_CLEAR_MAIN;
}

void submode_game_bonus_clear_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (modeCtrl.submodeTimer == 150.0)
        recplay_stop_recording();

    BALL_FOREACH(
        if (!(ball->flags & BALL_FLAG_REVERSE_GRAVITY) && (ball->ape->flags & BALL_FLAG_14))
        {
            ball->flags &= ~(BALL_FLAG_08|BALL_FLAG_IGNORE_GRAVITY);
            ball->flags |= BALL_FLAG_REVERSE_GRAVITY;
            modeCtrl.courseFlags |= (1 << 10);
            u_play_sound_0(0x126);
        }
    )
    if ((modeCtrl.courseFlags & (1 << 10))
     && modeCtrl.submodeTimer > 60 && modeCtrl.submodeTimer < 240)
        modeCtrl.submodeTimer = 60;
    loadingStageId = u_get_next_stage_id();
    if (loadingStageId < 0 && modeCtrl.submodeTimer == 60)
    {
        start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 61);
        u_play_music(60, 2);
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    BALL_FOREACH( ball->flags &= ~BALL_FLAG_REVERSE_GRAVITY; )
    if (loadingStageId < 0 && modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
        gameSubmodeRequest = SMD_GAME_RESULT_INIT;
    else
    {
        infoWork.attempts = (modeCtrl.gameType != GAMETYPE_MAIN_PRACTICE) ? 1 : infoWork.attempts + 1;
        gameSubmodeRequest = SMD_GAME_READY_INIT;
    }
}

void submode_game_over_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    event_finish(EVENT_SOUND);
    if (eventInfo[EVENT_SPRITE].state == EV_STATE_INACTIVE)
        event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    modeCtrl.submodeTimer = 120;
    if (!(modeCtrl.courseFlags & ((1 << 5)|(1 << 6))))
        BALL_FOREACH( u_play_sound_0(0x22C); )
    hud_show_game_over_banner(120);
    if (screenFadeInfo.timer == 0)
    {
        if (!(modeCtrl.courseFlags & ((1 << 5)|(1 << 6))))
            start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 120);
        else
            start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(255, 255, 255, 0), 120);
    }
    u_play_music(0x2A, 0);
    gameSubmodeRequest = SMD_GAME_OVER_MAIN;
}

void submode_game_over_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (modeCtrl.submodeTimer == 90)
        u_play_sound_0(0x240);
    if (--modeCtrl.submodeTimer > 0)
        return;

    // Can spend play points?
    if (modeCtrl.playerCount == 1
     && is_minigame_unlocked(GAMETYPE_MINI_BILLIARDS)
     && is_minigame_unlocked(GAMETYPE_MINI_BOWLING)
     && is_minigame_unlocked(GAMETYPE_MINI_GOLF)
     && !are_all_continues_unlocked() && g_totalPlayPoints >= 2500)
        gameSubmodeRequest = SMD_GAME_OVER_POINT_INIT;

    else if (func_8009F4C4() == 1)
    {
        memcard_set_mode(MC_MODE_SAVE_GAMEDATA_3);
        event_start(EVENT_MEMCARD);
        gameSubmodeRequest = SMD_GAME_OVER_SAVE;
    }
    else
        gameSubmodeRequest = SMD_GAME_OVER_DEST;
}

void submode_game_over_point_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 300;
    event_finish_all();
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    show_play_points_textbox(2, 0x140, 0x168);
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 15);
    u_play_music(0x3E, 0);
    gameSubmodeRequest = SMD_GAME_OVER_POINT_MAIN;
}

void submode_game_over_point_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (is_play_points_textbox_done() && modeCtrl.submodeTimer > 30 && (g_currPlayerButtons[2] & PAD_BUTTON_A))
        modeCtrl.submodeTimer = 30;
    if (modeCtrl.submodeTimer == 30)
        textbox_set_properties(1, TEXTBOX_STATE_FADEOUT, NULL);
    if (modeCtrl.submodeTimer == 15)
        start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 15);
    if (--modeCtrl.submodeTimer > 0)
        return;
    if (func_8009F4C4() == 1)
    {
        memcard_set_mode(MC_MODE_SAVE_GAMEDATA_3);
        event_start(EVENT_MEMCARD);
        gameSubmodeRequest = SMD_GAME_OVER_SAVE;
    }
    else
        gameSubmodeRequest = SMD_GAME_OVER_DEST;
}

void submode_game_over_save_func(void)
{
    if (eventInfo[EVENT_MEMCARD].state != EV_STATE_RUNNING)
        gameSubmodeRequest = SMD_GAME_OVER_DEST;
}

void submode_game_over_dest_func(void)
{
    if (debugFlags & 0xA)
        return;

    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MAIN_COMPETITION:
        g_poolInfo.playerPool.statusList[0] = STAT_NULL;
        g_poolInfo.playerPool.statusList[1] = STAT_NULL;
        g_poolInfo.playerPool.statusList[2] = STAT_NULL;
        g_poolInfo.playerPool.statusList[3] = STAT_NULL;
        modeCtrl.currPlayer = 0;
        currentBall = &ballInfo[modeCtrl.currPlayer];
        SoundGroupFree();
        break;
    default:
        mark_player_finished(modeCtrl.currPlayer);
        break;
    }
    if (are_all_players_finished())
    {
        reset_camera_viewport();
        modeCtrl.unk10 = 0;
        gameModeRequest = MD_ADV;
        gameSubmodeRequest = SMD_ADV_TITLE_REINIT;
    }
    else
    {
        start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 1);
        gameSubmodeRequest = SMD_GAME_READY_INIT;
    }
}

static char *nameEntryTextJpn[] =  // unused
{
    "h/NAMAEWO OSIETE!",
    "h/ONAMAE OSIETE!",
    "h/NAMAEWO OSIETEHOSHIIDEk/CHU!",
    "k/NAMAE OSIERO! UHO UHOHO!",
};

static char *nameEntryText[] =
{
    "ENTER YOUR NAME!",
    "ENTER YOUR NAME!",
    "ENTER YOUR NAME!",
    "ENTER YOUR NAME!",
};

void submode_game_nameentry_ready_init_func(void)
{
    struct TextBox tbox;

    if (debugFlags & 0xA)
        return;

    modeCtrl.courseFlags &= ~(1 << 5);
    if (!(modeCtrl.courseFlags & (1 << 0)) && (dipSwitches & (DIP_DEBUG|DIP_NAMEENTRY)) != (DIP_DEBUG|DIP_NAMEENTRY))
    {
        gameSubmodeRequest = SMD_GAME_OVER_INIT;
        return;
    }
    if (modeCtrl.gameType != GAMETYPE_MAIN_NORMAL
     || (u_calc_rank_of_score_record(modeCtrl.difficulty, &lbl_802C67D4[modeCtrl.currPlayer][0]) < 0
         && (dipSwitches & (DIP_DEBUG|DIP_NAMEENTRY)) != (DIP_DEBUG|DIP_NAMEENTRY)))
    {
        gameSubmodeRequest = SMD_GAME_OVER_INIT;
        return;
    }
    modeCtrl.submodeTimer = 300;
    modeCtrl.courseFlags |= (1 << 5);
    load_stage(199);
    event_finish_all();
    event_start(EVENT_BALL);
    event_start(EVENT_STAGE);
    event_start(EVENT_WORLD);
    event_start(EVENT_INFO);
    event_start(EVENT_OBJ_COLLISION);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    event_start(EVENT_RECPLAY);
    event_start(EVENT_STOBJ);
    event_start(EVENT_EFFECT);
    event_start(EVENT_REND_EFC);
    event_start(EVENT_BACKGROUND);
    event_start(EVENT_NAME_ENTRY);
    event_start(EVENT_VIBRATION);
    BALL_FOREACH(
        if (ball->ape != NULL)
            ball->ape->flags &= ~(1 << 3);
    );
    infoWork.flags |= INFO_FLAG_TIMER_PAUSED;
    memset(&tbox, 0, sizeof(tbox));
    tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
    tbox.x = 320;
    tbox.y = 144;
    tbox.numColumns = 0;
    tbox.numRows = 1;
    tbox.callback = NULL;
    textbox_set_properties(1, TEXTBOX_STATE_INIT, &tbox);
    textbox_add_text(1, nameEntryText[playerCharacterSelection[modeCtrl.currPlayer]]);
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(255, 255, 255, 0), 30);
    BALL_FOREACH( ball->state = 15; )
    camera_set_state_all(31);
    u_play_music(66, 0);
    gameSubmodeRequest = SMD_GAME_NAMEENTRY_READY_MAIN;
}

void submode_game_nameentry_ready_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (modeCtrl.submodeTimer == 120.0)
    {
        textbox_set_properties(1, TEXTBOX_STATE_FADEOUT, NULL);
        hud_show_name_entry_info(
            u_calc_rank_of_score_record(modeCtrl.difficulty, &lbl_802C67D4[modeCtrl.currPlayer][0]),
            lbl_802C67D4[modeCtrl.currPlayer][0].score);
        hud_show_name_entry_banner(modeCtrl.submodeTimer);
    }
    if (modeCtrl.submodeTimer == 180)
        camera_set_state_all(33);
    if (--modeCtrl.submodeTimer > 0)
        return;
    gameSubmodeRequest = SMD_GAME_NAMEENTRY_INIT;
}

void submode_game_nameentry_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    hud_show_go_banner(60);
    infoWork.flags &= ~INFO_FLAG_TIMER_PAUSED;
    WORLD_FOREACH( world->state = WORLD_STATE_INPUT_INIT; )
    BALL_FOREACH( ball->state = 4; )
    camera_set_state_all(35);
    gameSubmodeRequest = SMD_GAME_NAMEENTRY_MAIN;
}

void submode_game_nameentry_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    BALL_FOREACH(
        if (ball->state == 0)
            ball->state = 4;
    )
    if (!(infoWork.flags & INFO_FLAG_TIMER_PAUSED) && (infoWork.timerCurr % 60 == 59))
    {
        if (infoWork.timerCurr <= 600)
        {
            u_play_sound_0(6);
            u_play_sound_0(countdownSounds[infoWork.timerCurr / 60]);
        }
        else
            u_play_sound_0(6);
        if (infoWork.timerCurr / 60 == 10)
            u_play_sound_0(7);
    }
    if (u_is_name_entry_finished() != 0)
    {
        infoWork.flags |= INFO_FLAG_TIMER_PAUSED;
        event_finish(EVENT_NAME_ENTRY);
        gameSubmodeRequest = SMD_GAME_OVER_INIT;
    }
}

void submode_game_ending_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    modeCtrl.courseFlags |= (1 << 6);
    ending_init();
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1
     && !(modeCtrl.courseFlags & COURSE_FLAG_FAILED_EXTRA))
        record_play_points();
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 30);
    u_play_music(68, 0);
    u_play_sound_0(0x46);
    gameSubmodeRequest = SMD_GAME_ENDING_MAIN;
}

void submode_game_ending_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (!ending_main())
    {
        ending_finish();
        gameSubmodeRequest = SMD_GAME_ROLL_INIT;
        lbl_802F22C8 |= 1 << (modeCtrl.difficulty + 2);
        if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1
         && !(modeCtrl.courseFlags & COURSE_FLAG_FAILED_EXTRA))
            buy_extra_continues();
    }
}

/**
 * Roll Submode - Credits (staff roll)
 */

void submode_game_roll_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    credits_init();
    modeCtrl.submodeTimer = 600;
    gameSubmodeRequest = SMD_GAME_ROLL_MAIN;
}

void submode_game_roll_main_func(void)
{
    if (debugFlags & 0xA)
        return;

    credits_main();
    if (modeCtrl.submodeTimer <= 0)
    {
        credits_finish();
        submodeFinishFunc = NULL;
        gameSubmodeRequest = SMD_GAME_NAMEENTRY_READY_INIT;
        lbl_802F22C8 |= 2;
    }
}

/**
 * Extra Submode - Cutscene before entering Extra or Master
 */

#pragma force_active on
char *lbl_80175910[][6] =
{
    {
        "h/KONNDOHA SARANI",
        "h/UENOk/REBERUh/DE ASONNDENE",
        "h/KOREDE k/KIMIh/MO",
        "h/ICHININNMAENO Ok/SARUh/SANNDAYO",
        "k/KIMIh/TTEk/SUGOIh/NE! KOREKARAMO",
        "k/MONNKI-BO-RUh/WO YOROSIKU!",
    },
    {
        "h/KONNDOHA MOTTO",
        "h/UENOk/REBERUh/WO MEZASHITENE",
        "h/ANATAMO KOREDE",
        "h/ICHININNMAENO Ok/SARUh/SANNNE",
        "h/ANATATTEk/SUGOIh/WA! KOREKARAMO",
        "k/PUREI h/SHIMAKUTTENE",
    },
    {
        "h/MOTTO UENOk/REBERUh/DE",
        "h/ASOBITAIDEk/CHU",
        "h/KOREDE RIPPANA Ok/SARUh/SANNNO",
        "h/NAKAMAIRIDEk/CHU",
        "k/SUGOIh/DEk/CHU! h/KOREKARAMO",
        "h/ZUTTO ASONNDEHOSIIDEk/CHU",
    },
    {
        "h/MIKANNSEIDEk/GOZARUh/YO",
        "h/UHO UHOHO",
        "k/BANANAh/KAIJINNDEHAk/GOZARAh/NUYO",
        "h/UHO UHOHO",
        "h/UHO UHOHO",
        "k/CHU!",
    },
};

char *lbl_80175AA8[][6] =
{
    {
        "z/Let's try higher level",
        "z/next time",
        "z/Now you've become",
        "z/cool monkey like me",
        "z/You are so cool! Remember,",
        "z/Monkey Ball is the best game for you!",
    },
    {
        "z/Let's try higher level",
        "z/next time",
        "z/Now you've become",
        "z/pretty monkey like me",
        "z/You are so fantastic! Remember,",
        "z/Monkey Ball is the best game for you!",
    },
    {
        "z/I want to play higher level",
        "z/next time",
        "z/Now you've become",
        "z/grown up monkey like me",
        "z/You are such grown up! Remember,",
        "z/Monkey Ball is the best game for you!",
    },
    {
        "z/Let's try higher level",
        "z/next time",
        "z/Now you've become",
        "z/pretty monkey like me",
        "z/You are so fantastic! Remember,",
        "z/Monkey Ball is the best game for you!",
    },
};
#pragma force_active reset

char *extraIntroSpeech[][4] =
{
    {
        "z9/a/Congratulations! You didn't miss anything!",
        "a/You get to play the extra stages!",
        "a/They're all very difficult,",
        "a/but try your best!",
    },
    {
        "z9/a/I can't believe you didn't miss anything!",
        "z8/a/But, you still have to clear the extra stages!",
        "a/Make sure to try your hardest",
        "a/on these stages, too!",
    },
    {
        "a/Goo goo! You didn't miss anyting!",
        "a/You can pway da extra stages!",
        "a/It's a wittle bit hard,",
        "a/but do your best!",
    },
    {
        "z9/a/You passed everything without missing!!!",
        "a/Now, onto the extra stages!",
        "a/It's time for a real challenge!",
        "a/You must rule over all the stages!",
    },
};

char *expertExIntroSpeech[][4] =
{
    {
        "z8/a/You cleared all the stages without continuing!",
        "a/You can play the extra stages now!",
        "a/They're all very difficult,",
        "a/but try your best!",
    },
    {
        "z7/a/You cleared everything without having to continue!",
        "z7/a/But, there's still extra stages that you have to clear!",
        "a/Make sure to try your hardest",
        "a/on these stages, too!",
    },
    {
        "z9/a/You passed eberyting without continuing!",
        "a/You can pway da extra stages now!",
        "a/It's a wittle bit hard,",
        "a/but do your best!",
    },
    {
        "z7/a/Congratulations! You cleared everything without continuing!",
        "a/Onto the extra stages!",
        "a/You must rule over all the stages!",
        "a/Go go go!!!",
    },
};

char *masterIntroSpeech[][4] =
{
    {
        "z8/a/Congratulations! You cleared the extra stages!",
        "z8/a/Now you can try to clear the master stages!",
        "a/If you can pass these,",
        "a/you will be the best player ever!",
    },
    {
        "z7/a/Wow\x81\x63 I can't believe you passed the extra stages!",
        "a/You really are something!",
        "a/But, there's still more!",
        "z7/a/Now try to clear the master stages! Good luck!",
    },
    {
        "z7/a/You cleared da extra stages, too??? You are gweat!",
        "z8/a/But, now you have to pway da master stages!",
        "a/Dese ones are bery hard,",
        "a/but do your best!",
    },
    {
        "a/You cleared the extra stages!!!!",
        "a/Congraaaaaatulations! Good job!",
        "a/Now, onto your final challenge!",
        "a/The master stages!",
    },
};

void submode_game_extra_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 600;
    if (modeCtrl.courseFlags & COURSE_FLAG_EXTRA)
        modeCtrl.courseFlags |= COURSE_FLAG_MASTER;
    modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
    u_smth_with_lights_smd_extra(currStageId);
    event_finish(EVENT_CAMERA);
    event_finish(EVENT_SPRITE);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    BALL_FOREACH( ball->state = 13; )
    camera_set_state_all(51);
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 120);
    if (backgroundSongs[backgroundInfo.bgId] != -1)
        u_play_music(0xFFEC0000 | (backgroundSongs[backgroundInfo.bgId] + 1), 6);
    u_play_music(120, 3);
    gameSubmodeRequest = SMD_GAME_EXTRA_WAIT;
}

void submode_game_extra_wait_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (modeCtrl.submodeTimer > 60 && modeCtrl.submodeTimer < 480 && (g_currPlayerButtons[2] & PAD_BUTTON_A))
        modeCtrl.submodeTimer = 60;
    if (modeCtrl.submodeTimer == 540)
    {
        struct TextBox tbox;

        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
        tbox.x = 320;
        tbox.y = 200;
        tbox.numRows = 2;
        tbox.callback = NULL;
        textbox_set_properties(1, TEXTBOX_STATE_INIT, &tbox);
        if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
        {
            textbox_add_text(1, masterIntroSpeech[playerCharacterSelection[currentBall->playerId]][0]);
            textbox_add_text(1, masterIntroSpeech[playerCharacterSelection[currentBall->playerId]][1]);
        }
        else if (modeCtrl.difficulty == DIFFICULTY_EXPERT)
        {
            textbox_add_text(1, expertExIntroSpeech[playerCharacterSelection[currentBall->playerId]][0]);
            textbox_add_text(1, expertExIntroSpeech[playerCharacterSelection[currentBall->playerId]][1]);
        }
        else
        {
            textbox_add_text(1, extraIntroSpeech[playerCharacterSelection[currentBall->playerId]][0]);
            textbox_add_text(1, extraIntroSpeech[playerCharacterSelection[currentBall->playerId]][1]);
        }
    }
    if (modeCtrl.submodeTimer == 300)
    {
        struct TextBox tbox;

        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
        tbox.x = 320;
        tbox.y = 200;
        tbox.numRows = 2;
        tbox.callback = 0;
        textbox_set_properties(1, TEXTBOX_STATE_21, &tbox);
        if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
        {
            textbox_add_text(1, masterIntroSpeech[playerCharacterSelection[currentBall->playerId]][2]);
            textbox_add_text(1, masterIntroSpeech[playerCharacterSelection[currentBall->playerId]][3]);
        }
        else if (modeCtrl.difficulty == DIFFICULTY_EXPERT)
        {
            textbox_add_text(1, expertExIntroSpeech[playerCharacterSelection[currentBall->playerId]][2]);
            textbox_add_text(1, expertExIntroSpeech[playerCharacterSelection[currentBall->playerId]][3]);
        }
        else
        {
            textbox_add_text(1, extraIntroSpeech[playerCharacterSelection[currentBall->playerId]][2]);
            textbox_add_text(1, extraIntroSpeech[playerCharacterSelection[currentBall->playerId]][3]);
        }
    }
    if (modeCtrl.submodeTimer == 60)
    {
        textbox_set_properties(1, TEXTBOX_STATE_FADEOUT, NULL);
        start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 60);
        u_play_music(60, 2);
    }
    if ((600 - modeCtrl.submodeTimer) % 120 == 0)
        func_8007EB2C((600 - modeCtrl.submodeTimer) / 120);
    if (--modeCtrl.submodeTimer > 0)
        return;
    infoWork.currFloor = 1;
    course_first();
    loadingStageId = infoWork.u_currStageId;
    start_screen_fade(FADE_IN, RGBA(0, 0, 0, 0), 30);
    infoWork.attempts = 1;
    gameSubmodeRequest = SMD_GAME_READY_INIT;
}

void submode_game_result_init_func(void)
{
    int *playerCountPtr;

    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 0;
    lbl_802F1C24 = 0;
    event_finish(EVENT_CAMERA);
    event_finish(EVENT_SPRITE);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    if (currStageId == ST_125_DANCE_MASTER || currStageId == ST_116_ALTERNATE_ROLL_MASTER)
    {
        stageInfo.unk1C |= 1;
        animate_anim_groups(0.0f);
    }
    BALL_FOREACH( ball->state = 11; )
    camera_set_state_all(56);

    playerCountPtr = &modeCtrl.playerCount;  // fake match
    if (*playerCountPtr == 3)
    {
        switch (modeCtrl.splitscreenMode)
        {
        default:
        case SPLITSCREEN_1P_WIDE:
            camera_setup_splitscreen_viewports(3);
            break;
        case SPLITSCREEN_2P_WIDE:
            setup_camera_viewport(0, 0.0f, 0.5f, 0.5f, 0.5f);
            setup_camera_viewport(1, 0.0f, 0.0f, 1.0f, 0.5f);
            setup_camera_viewport(2, 0.5f, 0.5f, 0.5f, 0.5f);
            setup_camera_viewport(3, 0.0f, 0.0f, 0.0f, 0.0f);
            break;
        case SPLITSCREEN_3P_WIDE:
            setup_camera_viewport(0, 0.0f, 0.0f, 0.5f, 0.5f);
            setup_camera_viewport(1, 0.5f, 0.0f, 0.5f, 0.5f);
            setup_camera_viewport(2, 0.0f, 0.5f, 1.0f, 0.5f);
            setup_camera_viewport(3, 0.0f, 0.0f, 0.0f, 0.0f);
            break;
        case SPLITSCREEN_4_SPLIT:
            camera_setup_splitscreen_viewports(4);
            cameraInfo[3].flags |= 0x70;
            cameraInfo[3].state = 12;
            break;
        }
    }
    else
        camera_setup_splitscreen_viewports(*playerCountPtr);

    func_8007ECB8();
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 60);
    u_play_music(62, 0);
    u_play_music(60, 3);
    u_play_sound_0(0x1E0);
    gameSubmodeRequest = SMD_GAME_RESULT_MAIN;
}

double force_float_order_802F2C00(void) { return 0.5; }

void func_800165C0(struct Ball *ball)
{
    int i;
    struct Effect effect;
    Vec sp10;
    u16 cameraMask = 1 << ball->playerId;

    memset(&effect, 0, sizeof(effect));
    effect.type = ET_PAPERFRAG;
    mathutil_mtxA_from_translate(&ball->pos);
    mathutil_mtxA_translate_xyz(0.0f, 2.0f, 0.0f);
    sp10.x = 0.0f;
    sp10.y = 0.0f;
    for (i = 2; i > 0; i--)
    {
        float var = 0.1f;

        sp10.z = (RAND_FLOAT() + 1.0) * var * 0.5;
        mathutil_mtxA_rotate_y(rand() & 0x7FFF);
        mathutil_mtxA_rotate_x(rand() & 0x7FFF);
        mathutil_mtxA_tf_point(&sp10, &effect.pos);
        effect.rotX = rand() & 0x7FFF;
        effect.rotY = rand() & 0x7FFF;
        effect.rotZ = rand() & 0x7FFF;
        effect.cameras = cameraMask;
        spawn_effect(&effect);
    }
}

u32 lbl_801761F0[] =
{
    0x000001DC,
    0x000001DD,
    0x000001DE,
    0x000001DF,
};

struct Struct801EEEC0
{
    s32 playerId;
    s32 bananas;
    s32 unk8;
};

u8 lbl_801EEEC0[0xC];

FORCE_BSS_ORDER(lbl_801EEEC0)

struct Struct801EEEC0 lbl_801EEECC[4];

static inline void u_something_with_rankings(struct Ball *ball)
{
    int j;
    int k;

    for (j = 0; j < modeCtrl.playerCount; j++)
    {
        if (ball->bananas >= lbl_801EEECC[j].bananas)
        {
            struct Struct801EEEC0 *r9;

            //for (k = modeCtrl.playerCount - 2; k >= j; k--)
            //    lbl_801EEECC[k + 1] = lbl_801EEECC[k];
            k = modeCtrl.playerCount - 2;
            r9 = lbl_801EEECC + k;
            while (k >= j)
            {
                r9[1] = r9[0];
                r9--; k--;
            }
            lbl_801EEECC[j].playerId = ball->playerId;
            lbl_801EEECC[j].bananas = ball->bananas;
            break;
        }
    }
}


void submode_game_result_main_func(void)
{
    int tick;
    int i;
    int r3;

    if (debugFlags & 0xA)
        return;

    if (modeCtrl.submodeTimer == 180)
    {
        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            lbl_801EEECC[i].playerId = -1;
            lbl_801EEECC[i].bananas = -1;
            lbl_801EEECC[i].unk8 = 0;
        }

        BALL_FOREACH( u_something_with_rankings(ball); )

        {
            int i;
            int r7 = 0;
            int r8 = 0;

            r7 = 0;
            r8 = 0;
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (i > 0)
                {
                    if (lbl_801EEECC[i - 1].bananas != lbl_801EEECC[i].bananas)
                    {
                        r7 += r8;
                        r8 = 1;
                    }
                    else
                        r8++;
                }
                else
                {
                    r8 = 1;
                    r7++;
                }
                lbl_801EEECC[i].unk8 = (r7 == modeCtrl.playerCount) ? 4 : r7;
            }
        }
    }
    if (modeCtrl.submodeTimer > 180)
    {
        tick = FALSE;
        BALL_FOREACH(
            int j;
            BOOL r4 = FALSE;

            // tick banana counters
            if (modeCtrl.submodeTimer % 4 == 0 && ball->bananas > 0)
            {
                int bananas = 1;

                if (ball->bananas > 100)
                    bananas = 41;
                else if (ball->bananas > 10)
                    bananas = 3;
                ball->bananaBonus += bananas;
                ball->bananas -= bananas;
                tick = TRUE;
                if (ball->bananas == 0)
                    r4 = TRUE;
            }
            if (ball->rank == 0 && ball->bananas == 0)
                r4 = TRUE;
            if (!r4)
                continue;
            for (j = 0; j < modeCtrl.playerCount; j++)
            {
                if (ball->playerId == lbl_801EEECC[j].playerId)
                {
                    ball->rank = lbl_801EEECC[j].unk8;
                    break;
                }
            }
            hud_create_some_ranking_icon(ball);
            u_play_sound_0(0x68);
            if (ball->rank == 1)
                u_play_sound_0(0x1B);
            else if (ball->rank == 4)
                u_play_sound_0(0x3E);
            if (ball->rank == 1)
            {
                lbl_802F1C25 = lbl_801761F0[ball->playerId];
                lbl_802F1C24++;
            }
        )
        if (tick)
            u_play_sound_0(0x67);
    }
    if (++modeCtrl.submodeTimer < 240)
        return;

    r3 = 1;
    BALL_FOREACH(
        if (ball->bananas != 0)
            r3 = 0;
    )
    if (r3)
    {
        if (lbl_802F1C24 == 1)
            u_play_sound_0(lbl_802F1C25);
        else
            u_play_sound_0(0x1D1);
        modeCtrl.submodeTimer = 600;
        gameSubmodeRequest = SMD_GAME_RESULT_MENU;
    }
}

void submode_game_result_menu_func(void)
{
    if (debugFlags & 0xA)
        return;

    if (lbl_802F1C24 == 1)
    {
        BALL_FOREACH(
            if (ball->rank == 1)
                func_800165C0(ball);
        )
    }
    if (modeCtrl.submodeTimer > 60 && ((g_currPlayerButtons[2] & PAD_BUTTON_A) || (g_currPlayerButtons[2] & PAD_BUTTON_START)))
    {
        modeCtrl.submodeTimer = 60;
        u_play_sound_0(0xD1);
    }
    if (modeCtrl.submodeTimer == 60)
    {
        start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(255, 255, 255, 0), 60);
        u_play_music(60, 2);
    }
    if (--modeCtrl.submodeTimer == 0)
        func_80012434(modeCtrl.gameType);
}

void submode_game_intr_sel_init_func(void)
{
    if (debugFlags & 0xA)
        return;

    modeCtrl.submodeTimer = 480;
    event_finish_all();
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    record_play_points();
    show_play_points_textbox(0, 0x140, 0x168);
    start_screen_fade(FADE_UNK2|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 1);
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 15);
    u_play_music(0x3E, 0);
    gameSubmodeRequest = SMD_GAME_INTR_SEL_MAIN;
}

void submode_game_intr_sel_main_func(void)
{
    if (debugFlags & 0xA)
        return;
    if (is_play_points_textbox_done() && modeCtrl.submodeTimer > 30 && (g_currPlayerButtons[2] & PAD_BUTTON_A))
        modeCtrl.submodeTimer = 30;
    if (modeCtrl.submodeTimer == 30)
        textbox_set_properties(1, TEXTBOX_STATE_FADEOUT, NULL);
    if (modeCtrl.submodeTimer == 15)
        start_screen_fade(FADE_OUT|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 15);
    if (--modeCtrl.submodeTimer <= 0)
        func_80012434(-1);
}

int u_get_next_stage_id(void)
{
    if (modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        return currStageId;
    if (modeCtrl.courseFlags & (1 << 0))
        return infoWork.u_currStageId;
    if (currStageId + 1 > 200)
        return -1;
    return currStageId + 1;
}

int u_get_next_stage_id_not_practice(void)
{
    if (modeCtrl.courseFlags & (1 << 0))
        return infoWork.unk32;
    if (currStageId + 1 <= 200)
        return currStageId + 1;
    return -1;
}

/* Loses one life if the player is not in Competition or Practice mode.
 * Returns TRUE if the player still has lives. */
u32 lose_life(void)
{
    if ((infoWork.flags & INFO_FLAG_BONUS_STAGE)
     || modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION
     || modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        return TRUE;

    infoWork.livesLost++;
    BALL_FOREACH( ball->lives--; )
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL && modeCtrl.playerCount == 1)
        reset_curr_floor_streak();
    if (currentBall->lives > 0)
        return TRUE;
    return FALSE;
}

void func_80017140(void)
{
    func_800AEDE0();
}

// Returns the player whose turn it is next in normal mode
int get_next_player(void)
{
    int i;
    int nextPlayer;

    (void)modeCtrl.currPlayer;  // needed to match

    for (i = 0; i < 4; i++)
    {
        nextPlayer = (modeCtrl.currPlayer + i + 1) & 3;
        if (g_poolInfo.playerPool.statusList[nextPlayer] == STAT_FREEZE)
            break;
    }
    return nextPlayer;
}

void u_init_player_data_1(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != STAT_NULL)
            break;
    }
    modeCtrl.currPlayer = i;
    for (i = i + 1; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] == STAT_NORMAL)
            g_poolInfo.playerPool.statusList[i] = STAT_FREEZE;
    }
    modeCtrl.courseFlags |= (1 << 8);
    for (i = 0; i < 4; i++)
    {
        playerInfos[i] = infoWork;
        lbl_801F3A8C[i] = modeCtrl.courseFlags;
    }
}

void u_init_player_data_2(void)
{
    u32 nextPlayer;

    if (g_poolInfo.playerPool.statusList[modeCtrl.currPlayer] == STAT_NORMAL)
        g_poolInfo.playerPool.statusList[modeCtrl.currPlayer] = STAT_FREEZE;
    playerInfos[modeCtrl.currPlayer] = infoWork;
    lbl_801F3A8C[modeCtrl.currPlayer] = modeCtrl.courseFlags;
    nextPlayer = get_next_player();
    g_poolInfo.playerPool.statusList[nextPlayer] = STAT_NORMAL;
    if (modeCtrl.currPlayer != nextPlayer)
    {
        modeCtrl.currPlayer = nextPlayer;
        infoWork = playerInfos[modeCtrl.currPlayer];
        modeCtrl.courseFlags = lbl_801F3A8C[modeCtrl.currPlayer];
        loadingStageId = u_get_next_stage_id();
    }
}

// Marks a player as having reached Game Over
void mark_player_finished(int playerId)
{
    g_poolInfo.playerPool.statusList[playerId] = STAT_NULL;
}

// Returns true if all players have reached Game Over
BOOL are_all_players_finished(void)
{
    int ret = TRUE;
    int i;

    for (i = 0; i < 4; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] != STAT_NULL)
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}
