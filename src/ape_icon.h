#ifndef _SRC_APE_ICON_H_
#define _SRC_APE_ICON_H_

struct ApeIconInfo
{
    s32 emotion;
    s32 unk4;
    s32 frameNum;
    s16 angle;
    s32 unk10;
    s32 unk14;
};

void monkey_sprite_draw(struct Sprite *);
int u_get_monkey_bitmap_id(int, int, int);

#endif
