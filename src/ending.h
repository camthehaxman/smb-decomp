#ifndef _SRC_ENDING_H_
#define _SRC_ENDING_H_

struct SomeBigEndingStruct_sub3
{
    struct Ape *unk0;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    /*0x0A*/ s16 unkA;
    /*0x0C*/ Vec unkC;
    /*0x18*/ Vec unk18;
             u8 filler24[0x30-0x24];
             S16Vec unk30;
             u16 unk36;
    /*0x38*/ s16 unk38;
    /*0x3A*/ s16 unk3A;
    void *unk3C;
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

struct SomeBigEndingStruct_sub4
{
    s8 unk0;
    s16 unk2;
    s16 unk4;
    s32 unk8;
    Vec unkC;
    Vec unk18;
    Vec unk24;
    S16Vec unk30;
    s16 unk36, unk38, unk3A;
    float unk3C, unk40, unk44;
    float unk48;
    struct SomeBigEndingStruct_sub4_sub *unk4C;
    struct SomeBigEndingStruct_sub4_sub2 *unk50;
};

struct SomeBigEndingStruct
{
    float unk0;
    struct SomeBigEndingStruct_sub4 unk4[512];
    struct SomeBigEndingStruct_sub3 unkA804[4];
    struct SomeStruct unkAA04;
    struct SomeBigEndingStruct_sub unkAA54;
    float unkAAA8;
    float unkAAAC;
    s32 unkAAB0;
    char *unkAAB4[2];
    u8 fillerAABC[0xAACC-0xAABC];
    u32 unkAACC;
    s32 unkAAD0;
};

struct Struct802C6BD8
{
    s16 unk0;
    s16 unk2;
    u32 unk4;
    struct SomeBigEndingStruct *unk8;
};

extern struct Struct802C6BD8 lbl_802C6BD8;

#endif
