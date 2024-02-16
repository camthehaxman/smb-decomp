#ifndef _SRC_RECPLAY_CMPR_H_
#define _SRC_RECPLAY_CMPR_H_

struct ReplayBallFrame;
struct ReplayWorldFrame;
struct ReplayHeader;

void func_8004AFC0(void);
void cmp_recplay_init_first(void);
void dummy_8004AFD4(void);
void recplay_cmpr_record_frame(void);
void recplay_cmpr_dest(void);
void recplay_cmpr_start_recording(void);
void recplay_cmpr_stop_recording(void);
void recplay_cmpr_record_goal(void);
void recplay_cmpr_record_fallout(void);
void recplay_cmpr_record_timeover(void);
void recplay_cmpr_record_score(void);
void recplay_cmpr_record_bonus_clear(void);
void recplay_cmpr_record_player_name(char *);
void func_8004B70C(void);
float recplay_cmpr_get_time(void);
void recplay_cmpr_get_ball_frame(float, struct ReplayBallFrame *);
void recplay_cmpr_get_world_frame(float, struct ReplayWorldFrame *);
float recplay_cmpr_get_info_timer(float);
float func_8004C254(float);
void recplay_cmpr_get_header(struct ReplayHeader *);
u32 recplay_cmpr_calc_compressed_size(void);
u32 recplay_cmpr_get_data(void *);
int recplay_cmpr_set_data(const void *);
int func_8004C70C(void);
void func_8004C754(void);
void func_8004C780(void);
void func_8004C7A8(void);

#endif
