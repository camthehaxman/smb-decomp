#ifndef _SRC_MOT_APE_H_
#define _SRC_MOT_APE_H_

extern struct GMA *charaGMAs[8];
extern GXTexObj *lbl_802B47E0[4];

extern Mtx **u_animTransformMatrices;

// ? load_character_resources();
// ? func_80089A04();
// ? u_create_joints_probably();
// ? func_80089BD4();
// ? func_80089CBC();
// ? func_80089CF4();
// ? ape_dummy_1();
// ? ape_dummy_2();
// ? ape_dummy_3();
// ? ape_dummy_4();
// ? func_8008A124();
// ? func_8008A2C4();
// ? func_8008A3A4();
void func_8008A55C(u32 a, struct Struct802B39C0_B0_child *b, int c, int d);
// ? func_8008A7F0();
// ? u_free_character_graphics();
// ? u_load_character_graphics();
//struct GMAShape *next_shape(struct GMAShape *);
void mot_ape_init(void);
void func_8008B0AC(void);
void new_ape_close(struct Ape *);
// ? u_make_ape_sub();
struct Ape *u_make_ape(enum Character);
void func_8008BA24(int);
// ? func_8008BA2C();
void new_ape_stat_motion(struct Ape *, int, int, int, float);
void func_8008BEF8(int);
void u_switch_ape_character_lod_maybe(struct Ape *, int);
// ? func_8008BFB4();
void func_8008BFDC(struct Ape *, u16, u16);
void ape_face_dir(struct Ape *, Vec *);
void func_8008C408(struct Ape *, Vec *);
void u_mot_ape_set_some_var(float);
void new_ape_calc(struct Ape *);
// ? u_draw_ape_transformed();
void func_8008C924(struct Ape *ape, int b);
void func_8008CAAC(struct Ape *, float);
void u_init_ape_materials(enum Character, int, struct GMAModel *, struct GMAModel *);
void u_init_ape_materials_maybe_with_colors(struct Ape *ape, struct GMAModel *unused);

#endif
