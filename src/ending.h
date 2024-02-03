#ifndef _SRC_ENDING_H_
#define _SRC_ENDING_H_

struct EndingSceneCharacter
{
    struct Ape *ape;
    s16 state;
    s16 timer;
    s16 unk8;
    /*0x0A*/ s16 unkA;
    /*0x0C*/ Vec pos;
    /*0x18*/ Vec vel;
             u8 filler24[0x30-0x24];
             S16Vec unk30;
             u16 unk36;
    /*0x38*/ s16 unk38;
    /*0x3A*/ s16 unk3A;
    void *model;
    u32 unk40;
    Vec unk44;
    s16 unk50;
    s16 unk52;
    s16 unk54;
    s16 unk56;
    s16 unk58;
    s16 unk5A;
    s16 unk5C;
    u8 filler5E[0x70-0x5E];
    /*0x70*/ u32 unk70;  // -0x578c
    Vec unk74;
};  // size = 0x80

struct SomeBigEndingStruct_sub
{
    s16 unk0;
    s16 unk2;
    Vec unk4;
    Vec unk10;
    s16 unk1C;
    s16 unk1E;
    Vec unk20;
    Vec unk2C;
    Vec unk38;
    s16 unk44;
    s16 unk46;
    Vec unk48;
};  // size = 0x54

struct SomeStruct
{
    s8 unk0;
    s16 unk2;
    s16 unk4;
    Vec unk8;
    Vec unk14;
    Vec unk20;
    Vec unk2C;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    Vec unk44;
};  // size = 0x50

// either GMAModel or NlModel, possibly a union?
struct SomeBigEndingStruct_sub4_sub2
{
    u8 filler0[0x8];
    Vec unk8;
    float unk14;
};

struct SomeBigEndingStruct_sub4_sub
{
    u8 filler0[4];
    Vec unk4;
};

struct EndingSceneObject
{
    s8 isActive;
    s16 unk2;
    s16 unk4;
    s32 unk8;
    Vec pos;
    Vec vel;
    Vec unk24;
    S16Vec unk30;
    s16 unk36, unk38, unk3A;
    float unk3C, unk40, unk44;
    float unk48;
    struct SomeBigEndingStruct_sub4_sub *unk4C;
    struct SomeBigEndingStruct_sub4_sub2 *model;
};

struct EndingCharaDialogue
{
	s32 numLines;
	char *lines[6];
};

struct EndingWork
{
    float unk0;
    struct EndingSceneObject objects[512];
    struct EndingSceneCharacter characters[4];
    struct SomeStruct unkAA04;
    struct SomeBigEndingStruct_sub unkAA54;
    float unkAAA8;
    float unkAAAC;
    /*0xAAB0*/ struct EndingCharaDialogue charaDialogue;
    u32 unkAACC;
    char dialogueBuf[0x1800];
};

struct EndingInfo
{
    s16 state;
    s16 nextState;
    u32 flags;
    struct EndingWork *work;
};

extern struct EndingInfo endingInfo;

// ending.c
void ending_init(void);
int ending_main(void);
void ending_finish(void);
void u_ending_draw(void);

// ending_camera.c
void ending_camera_init(void);
void ending_camera_emptyfunc1(void);
void ending_camera_emptyfunc2(void);
void ending_camera_emptyfunc3(void);

// ending_dialogue.c
void effect_ending_ballfrag_init(struct Effect *);
void effect_ending_ballfrag_main(struct Effect *);
void effect_ending_ballfrag_draw(struct Effect *);
void effect_ending_ballfrag_destroy(struct Effect *);
void ending_prepare_chara_dialogue(struct EndingCharaDialogue *out, char *buffer);
void ending_prepare_chara_dialogue_test(int index, struct EndingCharaDialogue *out, char *buffer);

#endif
