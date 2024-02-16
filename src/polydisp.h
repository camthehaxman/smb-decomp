#ifndef _SRC_POLYDISP_H_
#define _SRC_POLYDISP_H_

struct PolyDisp
{
    u32 flags;
    Vec unk4;
    Vec unk10;
    Vec unk1C;
    Vec unk28;
    Vec unk34;
    Vec unk40;
};

extern struct PolyDisp polyDisp;

void polydisp_init(void);
void polydisp_main(void);
void draw_normal_game_scene(void);
void func_8000D5B8(void);
void draw_monkey(void);
void polydisp_set_some_color_based_on_curr_mode(float opacity);
void polydisp_set_nlobj_fade_color_from_goal_type(int);
void fade_color_base_default(void);
void fade_color_base_default_set(float a, float b, float c);
float get_height_mirror_plane(Vec *);
float get_height_world_mirror_plane(Vec *);
float func_8000E53C(Vec *pos);

#endif
