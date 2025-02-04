#ifndef _SRC_HUD_H_
#define _SRC_HUD_H_

void hud_show_pause_menu(void);
void func_80075E1C(int, struct Sprite *);
void hud_show_press_start_textbox(int);
void u_logo_plus_sprite_something(void);
void hud_show_adv_copyright_info(int);
void hud_show_title_banner(int);
void hud_show_title_menu(void);
void hud_show_title_screen_monkey_sprite(void);
void u_show_adv_ready_hud(void);
void hud_create_adv_demo_banana_sprite(int);
void u_show_eieipu_sprite(int);
void hud_show_stage_name_banner(void);
void hud_show_ready_banner(int);
void hud_show_normal_mode_info(void);
void hud_show_competition_mode_info();
void hud_show_go_banner(int);
void hud_show_goal_banner(int);
void hud_show_fallout_banner(int);
void hud_show_time_over_banner(int);
void hud_show_perfect_banner(int);
void hud_show_continue_interface(void);
void hud_show_game_over_banner(int);
void hud_show_1up_banner(int);
void hud_show_hurry_up_banner(void);
void hud_show_replay_text(int);
void hud_show_name_entry_info(int, int);
void hud_show_name_entry_banner(int);
void func_8007EB2C(int);
void func_8007ECB8(void);
void hud_create_some_ranking_icon(struct Ball *);
void hud_show_goal_score_info(int clearScore, int floorScore, u32 arg2);
void u_show_practice_mode_score_info(int);
void hud_show_bomb(float, float);

#endif
