#ifndef _SRC_AVDISP_H_
#define _SRC_AVDISP_H_

struct GMA;
struct GMAModel;
struct TPL;

struct TevStageInfo
{
    s32 tevStage;
    GXTexCoordID texCoordId;
    s32 unk8;
    /*0x0C*/ GXTexMapID texMapId;
    s32 tevIndStage;
    s32 unk14;
    u32 unk18;
    s32 unk1C;
    u32 unk20;
    GXTexCoordID u_texCoordId2;
    GXTexMapID u_someTexmapId2;
};

struct GCMMatState_Unit
{
    s32 unk0;
    void *unk4;
    void *unk8;
    struct TevStageInfo unkC;
};

void avdisp_init(void);
void func_8008D788(void);
void *avdisp_allocate_matrices(int count);
struct GMA *load_gma(char *fileName, struct TPL *tpl);
struct GMA *load_gma_from_aram(u32 aramSrc, u32 size, struct TPL *tpl);
void free_gma(struct GMA *gma);
struct TPL *load_tpl(char *fileName);
struct TPL *load_tpl_from_aram(u32 aramSrc, u32 size);
GXTexObj *create_tpl_tex_objs(struct TPL *tpl);
void free_tpl(struct TPL *tpl);
void avdisp_set_bound_sphere_scale(float);
void avdisp_set_ambient(float, float, float);
void avdisp_draw_model_culled_sort_translucent(struct GMAModel *);
void avdisp_draw_model_culled_sort_none(struct GMAModel *);
void avdisp_draw_model_culled_sort_all();
void avdisp_set_alpha(float alpha);
void avdisp_set_light_mask(u32 lightMask);
void avdisp_set_inf_light_dir(Vec *a);
void avdisp_set_inf_light_color(float, float, float);
void avdisp_set_z_mode(GXBool compareEnable, GXCompare compareFunc, GXBool updateEnable);
EnvMapFunc u_avdisp_set_some_func_1(EnvMapFunc);
Func802F20F0 u_avdisp_set_some_func_2(Func802F20F0);
void avdisp_draw_model_unculled_sort_translucent(struct GMAModel *a);
void avdisp_draw_model_unculled_sort_none(struct GMAModel *a);
void avdisp_draw_model_unculled_sort_all(struct GMAModel *a);
void u_avdisp_draw_model_4(struct GMAModel *model);
u32 avdisp_enable_custom_tex_mtx(u32);
void avdisp_set_custom_tex_mtx(int unused, Mtx mtx);
void avdisp_set_post_mult_color(float, float, float, float);
void avdisp_set_post_add_color(float, float, float, float);
void avdisp_enable_fog(int a);
void avdisp_set_fog_params(int, float, float);
void avdisp_set_fog_color(u8 a, u8 b, u8 c);

#endif
