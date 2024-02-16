#ifndef _SRC_MOT_APE_H_
#define _SRC_MOT_APE_H_

extern struct GMA *charaGMAs[8];
extern GXTexObj *lbl_802B47E0[4];

extern Mtx **u_animTransformMatrices;

void mot_ape_8008A55C(u32 a, struct BodyPartThing *b, int c, int d);
// ? mot_ape_8008A7F0();
// ? u_free_character_graphics();
// ? u_load_character_graphics();
//struct GMAShape *next_shape(struct GMAShape *);
void mot_ape_init(void);
void u_something_with_skel_model_names(void);
void ape_destroy(struct Ape *);
// ? u_make_ape_sub();
struct Ape *u_make_ape(enum Character);
void mot_ape_set_some_var_1(int);
void mot_ape_8008BA2C(struct Ape *, int, int);
void new_ape_stat_motion(struct Ape *, int, int, int, float);
void mot_ape_set_some_var_2(int);
void u_switch_ape_character_lod_maybe(struct Ape *, int);
void mot_ape_8008BFB4(struct Ape *ape, int b, int *c, float *d);
void mot_ape_8008BFDC(struct Ape *, u16, u16);
void ape_face_dir(struct Ape *, Vec *);
void mot_ape_set_quat_from_vec(struct Ape *, Vec *);
void u_set_global_skelanim_speed_scale(float);
void ape_skel_anim_main(struct Ape *);
// ? u_draw_ape_transformed();
void mot_ape_thread(struct Ape *ape, int b);

#endif
