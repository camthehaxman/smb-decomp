#ifndef _SRC_COURSE_H_
#define _SRC_COURSE_H_

struct MemcardContents;
struct Sprite;

extern u32 playPointYieldPerDifficulty[3];

struct Struct8027CE24
{
    s16 unk0;
    s16 unk2;
    u32 unk4;
};  // size = 8

extern struct Struct8027CE24 lbl_8027CE24[6];

extern u32 g_playPointsEarned;
extern s8 g_currFloorStreak;
extern u32 g_totalPlayPoints;
extern u32 g_recordPlayPoints;
extern u32 lbl_802F1FB4;
extern int lbl_802F1FB0;
extern u32 lbl_802F1FAC;
extern u32 lbl_802F1FA8;
extern int u_isCompetitionModeCourse;

void course_init(void);
void reset_earned_play_points(void);
void reset_curr_floor_streak(void);
void record_play_points(void);
void buy_extra_continues(void);
void show_play_points_textbox(int, s16, s16);
int is_play_points_textbox_done(void);
void course_first(void);
void ev_course_init(void);
void ev_course_main(void);
void ev_course_dest(void);
int u_get_stage_time_limit(void);
int floor_to_stage_id(int, int, int);
int course_floor_count(int, int);
u32 is_final_floor();
u32 is_bonus_stage(int stageId);
int func_800673BC(void);
int is_minigame_unlocked(int);
int get_max_continues(void);
int get_available_continues(void);
int are_all_continues_unlocked(void);
void game_sprint_course(struct Sprite *sprite);
void save_course_completion_data(struct MemcardContents *data);
void load_course_completion_data(struct MemcardContents *data);

#endif
