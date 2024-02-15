#ifndef _SRC_BITMAP_H_
#define _SRC_BITMAP_H_

#include <dolphin/types.h>
#include <dolphin/OSAlloc.h>

#include "types.h"
#include "nlsprite.h"

enum BitmapGroupID
{
    BMP_COM,
    BMP_ADV,
    BMP_END,
    BMP_RNK,
    BMP_SEL,
    BMP_NML,
    BMP_BWL,
    BMP_RAC,
    BMP_BIL,
    BMP_FGT,
    BMP_GLF,
    BMP_TGT,
    BMP_HOW,
    BMP_CMD,
    BMP_ALL,
};

// COM group (0)
enum
{
    BMP_COM_ASCII = (BMP_COM << 8),
    BMP_COM_arrow,
    BMP_COM_icon_button,
    BMP_COM_icon_gol_00,
    BMP_COM_icon_gol_afraid01,
    BMP_COM_DUMMY05,
    BMP_COM_asc_ball8x16_02,
    BMP_COM_icon_gol_afraid02,
    BMP_COM_icon_gol_afraid03,
    BMP_COM_icon_gol_afraid04,
    BMP_COM_icon_gol_afraid05,
    BMP_COM_icon_gol_angry01,
    BMP_COM_banana_01,
    BMP_COM_copyright_02,
    BMP_COM_icon_gol_angry02,
    BMP_COM_icon_ape_00,
    BMP_COM_icon_ape_afraid01,  // 0x10
    BMP_COM_icon_ape_afraid02,
    BMP_COM_icon_ape_afraid03,
    BMP_COM_icon_ape_afraid04,
    BMP_COM_icon_ape_afraid05,
    BMP_COM_icon_ape_angry01,
    BMP_COM_icon_ape_angry02,
    BMP_COM_icon_ape_angry03,
    BMP_COM_icon_ape_blink01,
    BMP_COM_icon_ape_blink02,
    BMP_COM_icon_ape_booing01,
    BMP_COM_icon_ape_booing02,
    BMP_COM_icon_ape_booing03,
    BMP_COM_icon_ape_booing04,
    BMP_COM_icon_ape_wakka,
    BMP_COM_asc12x12,
    BMP_COM_icon_gal_00,  // 0x20
    BMP_COM_icon_gal_afraid01,
    BMP_COM_icon_gal_afraid02,
    BMP_COM_icon_gal_afraid03,
    BMP_COM_icon_gal_afraid04,
    BMP_COM_icon_gal_angry01,
    BMP_COM_icon_gal_angry02,
    BMP_COM_icon_gal_angry03,
    BMP_COM_icon_gal_blink01,
    BMP_COM_icon_gal_blink02,
    BMP_COM_icon_gal_booing01,
    BMP_COM_icon_gal_booing02,
    BMP_COM_icon_gal_booing03,
    BMP_COM_icon_gal_booing04,
    BMP_COM_icon_gal_smile01,
    BMP_COM_icon_gal_smile02,
    BMP_COM_icon_gal_smile03,  // 0x30
    BMP_COM_icon_gal_smile04,
    BMP_COM_icon_gal_wakka,
    BMP_COM_icon_kid_00,
    BMP_COM_icon_kid_afraid01,
    BMP_COM_icon_kid_afraid02,
    BMP_COM_icon_kid_afraid03,
    BMP_COM_icon_kid_angry01,
    BMP_COM_icon_kid_angry02,
    BMP_COM_icon_kid_angry03,
    BMP_COM_icon_kid_blink01,
    BMP_COM_icon_kid_blink02,
    BMP_COM_icon_kid_booing01,
    BMP_COM_icon_kid_booing02,
    BMP_COM_icon_kid_booing03,
    BMP_COM_icon_kid_booing04,
    BMP_COM_icon_kid_smile01,  // 0x40
    BMP_COM_icon_kid_smile02,
    BMP_COM_icon_kid_wakka,
    BMP_COM_icon_kmh_01,
    BMP_COM_menu_kiwaku,
    BMP_COM_icon_smile01,
    BMP_COM_icon_smile02,
    BMP_COM_icon_smile03,
    BMP_COM_icon_smile04,
    BMP_COM_mes_sake24x24,
    BMP_COM_mes_waku24x24,
    BMP_COM_white_mask8x8,
    BMP_COM_frame,
    BMP_COM_icon_gol_angry03,
    BMP_COM_icon_gol_blink01,
    BMP_COM_icon_gol_blink02,
    BMP_COM_asc_tama72x64_new,  // 0x50
    BMP_COM_icon_gol_booing01,
    BMP_COM_str_sega,
    BMP_COM_icon_gol_booing02,
    BMP_COM_icon_gol_booing03,
    BMP_COM_banana_10,
    BMP_COM_icon_gol_booing04,
    BMP_COM_icon_gol_smile01,
    BMP_COM_game_icon_mph,
    BMP_COM_menu_kiwaku_l,
    BMP_COM_asc_jap24x24_new,
    BMP_COM_icon_gol_smile02,
    BMP_COM_icon_gol_smile03,
    BMP_COM_icon_gol_smile04,
    BMP_COM_icon_gol_wakka,
    BMP_COM_menu_kiwaku_l2,
};

// ADV group (1)
enum
{
    BMP_ADV_logo_av256 = (BMP_ADV << 8),  // 0x100
    BMP_ADV_adv_logo_plus,
    BMP_ADV_logo_sega512,
    BMP_ADV_adv_title_bg,
    BMP_ADV_adv_title_mnk_gawa,
    BMP_ADV_adv_fukidashi_h,
    BMP_ADV_adv_fukidashi_h_mini,
    BMP_ADV_logo_plus256x40,
    BMP_ADV_adv_title_spr_gawa,
    BMP_ADV_adv_title_mnk_a,
    BMP_ADV_adv_title_mnk_l,
    BMP_ADV_adv_title_mnk_b,
    BMP_ADV_adv_title_mnk_m,
    BMP_ADV_adv_title_mnk_o,
    BMP_ADV_adv_title_mnk_n,
    BMP_ADV_adv_title_mnk_k,
    BMP_ADV_adv_title_mnk_e,  // 0x110
    BMP_ADV_adv_title_mnk_y,
    BMP_ADV_adv_title_mnk_kage,
    BMP_ADV_adv_title_spr_s,
    BMP_ADV_adv_title_spr_u,
    BMP_ADV_adv_title_spr_p,
    BMP_ADV_adv_title_spr_e,
    BMP_ADV_adv_title_spr_r,
    BMP_ADV_adv_title_spr_kage,
    BMP_ADV_adv_title_tm,
};

// RNK group (3)
enum
{
    BMP_RNK_asc_ball26x31 = (BMP_RNK << 8),  // 0x300
    BMP_RNK_asc_ball30x31,
    BMP_RNK_rnk_ball_a,
    BMP_RNK_rnk_ball_g,
    BMP_RNK_rnk_ball_i,
    BMP_RNK_rnk_ball_k,
    BMP_RNK_rnk_ball_n,
    BMP_RNK_rnk_ball_r,
    BMP_RNK_rnk_catgword,
    BMP_RNK_rnk_ex2_icon,
    BMP_RNK_rnk_ex_icon,
    BMP_RNK_rnk_lines,
    BMP_RNK_rnk_monkeyball_logo,
    BMP_RNK_rnk_obibar_blue,
    BMP_RNK_rnk_obibar_grn,
    BMP_RNK_rnk_obibar_red,
    BMP_RNK_rnk_obibar_yer,  // 0x310
    BMP_RNK_rnk_obiword,
    BMP_RNK_rnk_rank,
    BMP_RNK_rnk_mini_asc32x32,
    BMP_RNK_rnk_mini_bar_bg,
    BMP_RNK_rnk_mini_bg,
    BMP_RNK_rnk_mini_cursor,
    BMP_RNK_rnk_mini_icon_best,
    BMP_RNK_rnk_mini_icon_star,
    BMP_RNK_rnk_mini_kiwaku,
    BMP_RNK_rnk_mini_kiwaku_l,
    BMP_RNK_rnk_mini_line,
    BMP_RNK_rnk_mini_mark_r,
    BMP_RNK_rnk_mini_mark_t,
    BMP_RNK_rnk_mini_num,
    BMP_RNK_rnk_mini_ptn,
    BMP_RNK_rnk_mini_pts,  // 0x320
    BMP_RNK_rnk_mini_puramai,
    BMP_RNK_rnk_mini_rank,
    BMP_RNK_rnk_mini_score_num,
    BMP_RNK_rnk_mini_sort,
    BMP_RNK_rnk_mini_player,
    BMP_RNK_rnk_mini_bar_w,
    BMP_RNK_rnk_mini_mark_tgt1,
    BMP_RNK_rnk_mini_mark_tgt2,
};

// SEL group (4)
enum
{
    BMP_SEL_sel_kiwaku_be_eex = (BMP_SEL << 8),  // 0x400
    BMP_SEL_sel_kiwaku_be_jex,
    BMP_SEL_sel_kiwaku_ad_eex,
    BMP_SEL_sel_kiwaku_ad_jex,
    BMP_SEL_sel_bg_sel,
    BMP_SEL_sel_flower4,
    BMP_SEL_sel_kiwaku_rac,
    BMP_SEL_sel_kiwaku_button,
    BMP_SEL_sel_kiwaku_sel,
    BMP_SEL_sel_kiwaku_fight,
    BMP_SEL_sel_chara_bar,
    BMP_SEL_sel_shine,
    BMP_SEL_sel_kiwaku_l,
    BMP_SEL_sel_kiwaku_s,
    BMP_SEL_sel_plate,
    BMP_SEL_sel_player,
    BMP_SEL_sel_player_x,  // 0x410
    BMP_SEL_sel_playmode,
    BMP_SEL_sel_chara,
    BMP_SEL_sel_flower1,
    BMP_SEL_sel_flower2,
    BMP_SEL_sel_flower3,
    BMP_SEL_sel_kiwaku_s2,
    BMP_SEL_sel_kiwaku_l2,
    BMP_SEL_sel_kiwaku_be,
    BMP_SEL_sel_kiwaku_ma,
    BMP_SEL_asc_sel12x24_num,
    BMP_SEL_sel_kiwaku_be_e,
    BMP_SEL_sel_kiwaku_be_j,
    BMP_SEL_sel_kiwaku_ad_e,
    BMP_SEL_sel_kiwaku_ad_j,
    BMP_SEL_sel_kiwaku_ex_e,
    BMP_SEL_sel_kiwaku_ex_j,  // 0x420
    BMP_SEL_sel_kiwaku_ma_e,
    BMP_SEL_sel_kiwaku_ma_j,
    BMP_SEL_sel_kiwaku_ad,
    BMP_SEL_sel_kiwaku_ex,
    BMP_SEL_sel_kiwaku_ex_eex,
    BMP_SEL_sel_kiwaku_ex_jex,
    BMP_SEL_sel_kiwaku_noise_l,
    BMP_SEL_sel_kiwaku_noise_s,
    BMP_SEL_sel_kiwaku_stage_l,
    BMP_SEL_sel_kiwaku_stage_s,
    BMP_SEL_sel_normal_level_ad_e,
    BMP_SEL_sel_normal_level_be_e,
    BMP_SEL_sel_normal_level_ex_e,
    BMP_SEL_sel_stg_comp_num,
    BMP_SEL_sel_stg_comp_num_ex,
    BMP_SEL_sel_stg_comp_icon,  // 0x430
    BMP_SEL_sel_kiwaku_noise_l2,
    BMP_SEL_sel_stg_comp_sel,
    BMP_SEL_sel_kiwaku_option,
    BMP_SEL_sel_kiwaku_rac_ad_e,
    BMP_SEL_sel_kiwaku_rac_ad_j,
    BMP_SEL_sel_kiwaku_rac_be_e,
    BMP_SEL_sel_kiwaku_rac_be_j,
    BMP_SEL_sel_kiwaku_rac_ex_e,
    BMP_SEL_sel_kiwaku_rac_ex_j,
    BMP_SEL_sel_playmode_golf,
};

// NML group (5)
enum
{
    BMP_NML_icon_bombtimer = (BMP_NML << 8),  // 0x500
    BMP_NML_icon_lv1234_j,
    BMP_NML_game_rank,
    BMP_NML_game_result_e3,
    BMP_NML_game_goal,
    BMP_NML_asc_ball22x22,
    BMP_NML_asc_komo16x16,
    BMP_NML_asc_tama32x32,
    BMP_NML_asc_ball18x16,
    BMP_NML_asc_ball20x20,
    BMP_NML_fukidashi,
    BMP_NML_game_player,
    BMP_NML_asc_ball26x38,
    BMP_NML_asc_ball16x22,
    BMP_NML_DUMMY14,
    BMP_NML_DUMMY15,
    BMP_NML_icon_bomb_hibi,  // 0x510
    BMP_NML_icon_bomb_part_a,
    BMP_NML_icon_bomb_part_b,
    BMP_NML_icon_bomb_part_c,
    BMP_NML_icon_bomb_part_d,
    BMP_NML_icon_bomb_part_e,
    BMP_NML_icon_bomb_part_f,
    BMP_NML_icon_bomb_part_g,
    BMP_NML_icon_bomb_part_h,
    BMP_NML_icon_bomb_part_i,
    BMP_NML_icon_bomb_part_j,
};

// HOW group (12)
enum
{
    BMP_HOW_how_kiwaku = (BMP_HOW << 8),  // 0xC00
    BMP_HOW_how_bar,
    BMP_HOW_how_bar_circle,
    BMP_HOW_how_item_fgt_big,
    BMP_HOW_how_item_fgt_box,
    BMP_HOW_how_item_fgt_lng,
    BMP_HOW_how_item_fgt_vor,
    BMP_HOW_how_item_rac_bnn,
    BMP_HOW_how_item_rac_bom,
    BMP_HOW_how_item_rac_box,
    BMP_HOW_how_item_rac_ice,
    BMP_HOW_how_item_rac_pol,
    BMP_HOW_how_item_rac_spd,
    BMP_HOW_how_item_tgt_bom,
    BMP_HOW_how_item_tgt_bomb,
    BMP_HOW_how_item_tgt_double,
    BMP_HOW_how_item_tgt_heavy,  // 0xC10
    BMP_HOW_how_item_tgt_kum,
    BMP_HOW_how_item_tgt_kumo,
    BMP_HOW_how_item_tgt_saf,
    BMP_HOW_how_item_tgt_stop,
    BMP_HOW_how_item_tgt_tog,
    BMP_HOW_how_item_tgt_toge,
    BMP_HOW_how_item_tgt_triple,
    BMP_HOW_how_item_tgt_wind,
    BMP_HOW_how_waku2_g,
    BMP_HOW_how_waku2_r,
    BMP_HOW_how_waku3_g,
    BMP_HOW_how_waku3_m,
    BMP_HOW_how_waku3_o,
    BMP_HOW_how_waku3_r,
    BMP_HOW_how_waku4_b,
    BMP_HOW_how_waku4_g,  // 0xC20
    BMP_HOW_how_waku5_b,
    BMP_HOW_how_waku_b,
    BMP_HOW_how_waku_g,
    BMP_HOW_how_waku_m,
    BMP_HOW_how_waku_o,
    BMP_HOW_how_waku_r,
    BMP_HOW_how_arrow,
    BMP_HOW_how_cont,
    BMP_HOW_how_waku3_b,
    BMP_HOW_how_cont_arrow,
    BMP_HOW_how_cont_bar,
};

struct Bitmap
{
    /*0x00*/ float x;
    /*0x04*/ float y;
    /*0x08*/ float unk8;
    /*0x0C*/ float unkC;
    /*0x10*/ s16 imageId;
    /*0x12*/ s16 rotation;
    /*0x14*/ u8 unk14;
    /*0x15*/ u8 r;
    /*0x16*/ u8 g;
    /*0x17*/ u8 b;
    /*0x18*/ u8 a;
};

struct BitmapGroup
{
    /*0x00*/ int isLoaded;
    /*0x04*/ char *filename;
    /*0x08*/ char *name;
    /*0x0C*/ u32 unkC;
    /*0x10*/ struct TPL *tpl;
    /*0x14*/ OSHeapHandle heap;
};

extern s32 spriteParamsBufCount;
extern s32 spriteParamsBufState;
extern struct TPL *u_unkBitmapTPL;

struct asdf;

extern struct BitmapGroup bitmapGroups[];
extern char **bitmapNames[];
extern NLsprarg spriteParamsBuf[];

void bitmap_init(void);
struct TPL *bitmap_load_tpl(char *filename);
void bitmap_load_group(enum BitmapGroupID grpId);
void bitmap_free_group(enum BitmapGroupID grpId);
int bitmap_is_group_loaded(enum BitmapGroupID grpId);
void bitmap_free_tpl(struct TPL *tpl);
void u_bitmap_set_some_tpl(enum BitmapGroupID grpId);
void u_bitmap_frame_reset(void);
void bitmap_init_tev(void);
void bitmap_main(void);
// ? func_8002687C();
// ? bitmap_draw();
// ? bitmap_draw_normal_char();
// ? bitmap_draw_char();
void bitmap_draw_string(void);
void u_draw_all_naomi_sprites(void);

#endif
