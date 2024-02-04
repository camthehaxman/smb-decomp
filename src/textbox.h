#ifndef _SRC_TEXTBOX_H_
#define _SRC_TEXTBOX_H_

enum
{
    TEXTBOX_STATE_INACTIVE =  0,
    TEXTBOX_STATE_INIT     =  1,
    TEXTBOX_STATE_2        =  2,
    TEXTBOX_STATE_3        =  3,
    TEXTBOX_STATE_FADEIN   = 10,
    TEXTBOX_STATE_SCROLL   = 11,
    TEXTBOX_STATE_FADEOUT  = 20,
    TEXTBOX_STATE_21       = 21,
    TEXTBOX_STATE_22       = 22,
};

enum
{
    TEXTBOX_STYLE_TOP_LEFT     = 1,  // left-pointing arrow on top
    TEXTBOX_STYLE_CENTER_LEFT  = 2,  // left-pointing arrow in middle
    TEXTBOX_STYLE_BOTTOM_LEFT  = 3,  // left-pointing arrow on bottom

    TEXTBOX_STYLE_TOP_RIGHT    = 4,  // right-pointing arrow on top
    TEXTBOX_STYLE_CENTER_RIGHT = 5,  // right-pointing arrow in middle
    TEXTBOX_STYLE_BOTTOM_RIGHT = 6,  // right-pointing arrow on bottom

    TEXTBOX_STYLE_LEFT_UP      = 7,  // up-pointing arrow on left
    TEXTBOX_STYLE_CENTER_UP    = 8,  // up-pointing arrow in middle
    TEXTBOX_STYLE_RIGHT_UP     = 9,  // up-pointing arrow on right

    TEXTBOX_STYLE_LEFT_DOWN    = 10,  // down-pointing arrow on right
    TEXTBOX_STYLE_CENTER_DOWN  = 11,  // down-pointing arrow in middle
    TEXTBOX_STYLE_RIGHT_DOWN   = 12,  // down-pointing arrow on right

    TEXTBOX_STYLE_SPIKY        = 13,  // spiky with a down-pointing arrow in middle
    TEXTBOX_STYLE_PLAIN        = 14,  // no arrow
    TEXTBOX_STYLE_THIN_BORDER  = 15,  // centered with thin border
};

struct TextBox
{
    /*0x00*/ s32 state;
    /*0x04*/ s32 timerMax;
    /*0x08*/ u32 timer;
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    /*0x10*/ float textWidth;  // width of text in pixels
    /*0x14*/ s8 numColumns;  // width of text in font units (number of columns
    /*0x15*/ s8 numRows;  // height of text in font units (number of rows)
    /*0x16*/ s8 style;
    s8 unk17;
    s8 unk18;
    s8 unk19;
    /*0x1C*/ void (*callback)(struct TextBox *);
    /*0x20*/ s32 id;
    /*0x24*/ s32 bgColor;
};  // size = 0x28

// width and height of the textbox characters (including ones used for its border)
// While Japanese characters are always 24 pixels wide, Latin characters are variable width.
#define TEXTBOX_FONT_SIZE 24

extern struct TextBox textBoxes[4];

void textbox_init(void);
void textbox_main(void);
void textbox_destroy_all(void);
void textbox_draw_all(void);
void textbox_set_properties(int id, int state, struct TextBox *template);
void textbox_add_text(int id, const char *str);
void textbox_add_textf(int id, const char *fmt, ...);

#endif
