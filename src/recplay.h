#ifndef _SRC_RECPLAY_H_
#define _SRC_RECPLAY_H_

enum
{
    REPLAY_FLAG_GOAL        = 1 << 0,
    REPLAY_FLAG_FALLOUT     = 1 << 1,
    REPLAY_FLAG_TIME_OVER   = 1 << 2,
    REPLAY_FLAG_BONUS_STAGE = 1 << 3,
    REPLAY_FLAG_4           = 1 << 4,
    REPLAY_FLAG_EXTRA       = 1 << 5,
    REPLAY_FLAG_MASTER      = 1 << 6,
    REPLAY_FLAG_BONUS_CLEAR = 1 << 7,
    REPLAY_FLAG_15          = 1 << 15,
};

struct ReplayHeader
{
    u16 flags;
    u8 stageId;
    u8 difficulty;  // 0 = beginner, 1 = advanced, 2 = expert
    u8 floorNum;
    u8 character;
    char playerName[4];
    float grade;  // Some priority scoring value which determines which replays are shown during the ADV sequence
    s32 unk10;
    u8 filler14[4];
};

struct ReplayBallFrame
{
    Vec pos;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    u32 unk18;
    float unk1C;
};

struct ReplayWorldFrame
{
    s16 rotX;
    s16 rotZ;
};

struct ReplayInfo
{
    s32 u_replayIndexes[4];
    float u_timeOffset;
    s32 u_playerId;
};

extern struct ReplayInfo g_recplayInfo;

void recplay_init(void);
void recplay_load_builtin_replays(void);
void recplay_init_first(void);
void ev_recplay_init(void);
void ev_recplay_main(void);
void ev_recplay_dest(void);
int recplay_find_new_index(void);
void u_recplay_reset_recording_state(void);
void recplay_set_player_replay_index(int, int);
void recplay_start_recording(void);
void recplay_stop_recording(void);
void recplay_set_recording_stop_timer(int);
void recplay_record_goal(int);
void recplay_record_fallout(int);
void recplay_record_timeover(int);
void recplay_record_bonus_clear(int);
void recplay_record_player_name(char *playerName);
void func_80049514(int);
float recplay_get_time(int);
void recplay_get_ball_frame(int, struct ReplayBallFrame *, float);
void recplay_get_world_frame(int, struct ReplayWorldFrame *, float);
float recplay_get_info_timer(int, float);
void recplay_get_header(int, struct ReplayHeader *);
float recplay_get_stage_timer(float, int);

void u_replay_test_init(void);
void u_replay_test_main(void);
void func_8004A800(void);

#endif
