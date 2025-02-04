#ifndef _SRC_INFO_H_
#define _SRC_INFO_H_

#include <dolphin/types.h>
#include <dolphin/mtx.h>

enum
{
    INFO_FLAG_GOAL         = (1 << 0),
    INFO_FLAG_TIMEOVER     = (1 << 1),
    INFO_FLAG_FALLOUT      = (1 << 2),
    INFO_FLAG_TIMER_PAUSED = (1 << 3),
    INFO_FLAG_REPLAY       = (1 << 4),
    INFO_FLAG_05           = (1 << 5),
    INFO_FLAG_BONUS_STAGE  = (1 << 6),
    INFO_FLAG_07           = (1 << 7),
    INFO_FLAG_08           = (1 << 8),  // 0x100
    INFO_FLAG_BONUS_CLEAR  = (1 << 9),  // 0x200  bonus clear (all bananas)?
    INFO_FLAG_10           = (1 << 10),  // 0x400
    INFO_FLAG_11           = (1 << 11),  // 0x800
    INFO_FLAG_FINAL_FLOOR  = (1 << 12),  // 0x1000
    INFO_FLAG_13           = (1 << 13),  // 0x2000
};

struct Struct801F3A58
{
    u32 flags;
    s16 timerCurr;  // current clock time (in 60ths of a second)
    s16 timerMax;  // maximum clock time (in 60ths of a second)
    u32 unk8;
    s16 goalEntered;
    s16 unkE;
    Vec unk10;
    s16 unk1C;
    /*0x1E*/ s16 attempts;  // number of tries on current stage
    /*0x20*/ s16 currFloor;  // also the round number in competition mode
    s16 unk22;
    /*0x24*/ s32 bananasLeft;
    /*0x28*/ s16 livesLost;
    /*0x2A*/ s16 continuesUsed;
    s16 unk2C;
    /*0x2E*/ s16 u_currStageId;  // yet another copy of the current stage ID? increments immediately when getting a goal
    s16 playerId;
    s16 unk32;
};

extern s8 lbl_802F1CB0[8];
extern s32 practiceBestScore;
extern s32 practiceBestFlag;

extern struct Struct801F3A58 infoWork;
extern u32 lbl_801F3A8C[4];
extern struct Struct801F3A58 playerInfos[4];

void info_init_first(void);
void ev_info_init(void);
void ev_info_main(void);
void ev_info_dest(void);
BOOL check_ball_entered_goal(struct Ball *, u32 *, s32 *);
void u_time_over_all_competition_mode_balls(void);
void func_80023DB8(struct Ball *);
void create_rank_icon(struct Ball *);
void func_800245E4(struct Ball *, int, int);
int func_800246F4(struct Ball *);
void func_80024860(struct Ball *);

#endif
