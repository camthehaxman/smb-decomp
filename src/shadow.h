#ifndef _SRC_SHADOW_H_
#define _SRC_SHADOW_H_

struct Struct80092B98_child
{
    u8 unk0;
    GXTexObj unk4;
};

struct Struct80092B98
{
    Vec unk0;
    float unkC;
    Vec unk10;
    float unk1C;
    Vec unk20;
    u16 unk2C;
    u16 unk2E;
    float unk30;
    float unk34;
    GXColor unk38;
    struct Struct80092B98_child *unk3C;
};  // size = 0x40

struct Struct80092F90
{
    u16 unk0;
    u16 unk2;
    union { struct GMAModel *gma; struct NlModel *naomi; } model;
};

void func_80092498(void);
void func_800924CC(void);
void shadowerase_main(void);
int u_init_shadow_stuff_probably(struct Struct80092B98 *);
int func_80092D34(void);
void u_draw_ball_shadow(void);
void func_80092F90(struct Struct80092F90 *);
// ? func_800934B0();
// ? func_80093920();

#endif
