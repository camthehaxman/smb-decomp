#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <dolphin.h>

#include "global.h"
#include "mathutil.h"
#include "sprite.h"
#include "textbox.h"

struct TextBoxLine
{
    s8 unk0;
    char text[0x81];
    s16 unk82;
};

struct TextBoxLine textBoxLines[4][20];
FORCE_BSS_ORDER(textBoxLines)

struct TextBox textBoxesBackup[4];
FORCE_BSS_ORDER(textBoxesBackup)

struct TextBox textBoxes[4];
FORCE_BSS_ORDER(textBoxes)

float lbl_802F200C;
float lbl_802F2008;

static void update_textbox(int id, struct TextBox *b);
static void clear_lines(int a);

void textbox_init(void)
{
    int i;
    struct TextBox *tboxA;
    struct TextBox *tboxB;

    tboxA = textBoxes;
    tboxB = textBoxesBackup;
    for (i = 0; i < 4; i++, tboxA++, tboxB++)
    {
        memset(tboxA, 0, sizeof(*tboxA));
        memset(tboxB, 0, sizeof(*tboxB));
        tboxA->state = TEXTBOX_STATE_INACTIVE;
        tboxA->timer = 0;
        tboxA->callback = NULL;
        clear_lines(i);
    }
    lbl_802F200C = -1.0f;
    lbl_802F2008 = 0.0f;
}

void textbox_main(void)
{
    int i;
    struct TextBox *tbox;

    tbox = textBoxes;
    for (i = 0; i < 4; i++, tbox++)
    {
        if (tbox->state != TEXTBOX_STATE_INACTIVE)
        {
            if (tbox->callback != NULL)
                tbox->callback(tbox);
            update_textbox(i, tbox);
        }
    }
}

static void update_textbox(int id, struct TextBox *tbox)
{
    s32 i;
    struct TextBoxLine *line;
    s8 numColumns;
    int j;
    float maxWidth;

    switch (tbox->state)
    {
    case TEXTBOX_STATE_INIT:
        tbox->timer++;
        if (tbox->timer == tbox->timerMax)
        {
            tbox->state = TEXTBOX_STATE_FADEIN;
            tbox->timer = 0;
            if (textBoxesBackup[id].state == TEXTBOX_STATE_FADEOUT)
            {
                tbox->state = TEXTBOX_STATE_FADEOUT;
                textBoxesBackup[id].state = TEXTBOX_STATE_INACTIVE;
            }
        }
        if (tbox->numColumns == 0)
        {
            maxWidth = 0.0f;
            numColumns = 0;
            line = textBoxLines[id];
            for (i = 0; i < 20; i++, line++)
            {
                if (line->unk0 == 0)
                    break;
                if (tbox->numRows == 1)
                {
                    numColumns = u_get_ascii_text_width(line->text);
                    maxWidth = u_get_jpn_text_width(FONT_JAP_24x24_2P, line->text);
                }
                else
                {
                    float f0;
                    if (numColumns < u_get_ascii_text_width(line->text))
                        numColumns = u_get_ascii_text_width(line->text);
                    f0 = u_get_jpn_text_width(FONT_JAP_24x24_2P, line->text);
                    if (maxWidth < f0)
                        maxWidth = f0;
                }
            }
            tbox->numColumns = numColumns;
            tbox->textWidth = maxWidth;
        }
        break;
    case TEXTBOX_STATE_3:
        tbox->state = TEXTBOX_STATE_INIT;
        tbox->timer = 0;
        break;
    case TEXTBOX_STATE_FADEIN:
        tbox->timer++;
        line = textBoxLines[id];
        j = 0;
        for (i = 0; i < 20; i++, line++)
        {
            if (line->unk0 != 0)
                j++;
        }
        if (j > tbox->numRows)
        {
            tbox->state = TEXTBOX_STATE_SCROLL;
            tbox->timer = 0;
        }
        break;
    case TEXTBOX_STATE_SCROLL:  // scrolling
        tbox->timer++;
        if (tbox->timer == 16)
        {
            s32 i;
            tbox->state = TEXTBOX_STATE_FADEIN;
            // move lines up
            line = textBoxLines[id];
            for (i = 0; i < 19; i++, line++)
                memcpy(line, line + 1, sizeof(*line));
            line->unk0 = 0;
            line->unk82 = 0;
        }
        break;
    case TEXTBOX_STATE_FADEOUT:
        tbox->timer++;
        if (tbox->timer == tbox->timerMax)
        {
            tbox->state = TEXTBOX_STATE_INACTIVE;
            tbox->timer = 0;
            tbox->callback = NULL;
            clear_lines(id);
        }
        break;
    case TEXTBOX_STATE_21:
        tbox->timer++;
        if (tbox->timer == tbox->timerMax)
        {
            s32 i;

            for (i = 0; i < tbox->numRows; i++)
            {
                line = textBoxLines[id];
                for (j = 0; j < 19; j++, line++)
                    memcpy(line, line + 1, sizeof(*line));
                line->unk0 = 0;
                line->unk82 = 0;
            }
            memcpy(tbox, &textBoxesBackup[id], sizeof(*tbox));
            tbox->state = TEXTBOX_STATE_INIT;
            tbox->timer = 0;
            textBoxesBackup[id].state = TEXTBOX_STATE_INACTIVE;
        }
        break;
    case TEXTBOX_STATE_22:
        tbox->timer++;
        if (tbox->timer == tbox->timerMax)
        {
            tbox->state = TEXTBOX_STATE_2;
            tbox->timer = 0;
            clear_lines(id);
        }
        break;
    }

    line = textBoxLines[id];
    for (i = 0; i < 20; i++, line++)
    {
        if (tbox->state >= TEXTBOX_STATE_FADEIN
         && line->unk0 != 0
         && line->unk0 == 2
         && (tbox->state == TEXTBOX_STATE_FADEIN || tbox->state == TEXTBOX_STATE_SCROLL))
        {
            if (i < tbox->numRows
             && (i == 0 || textBoxLines[id][i - 1].unk0 != 2))
                line->unk82 += 2;
            if (line->unk82 >= u_get_ascii_text_width(line->text))
                line->unk0 = 1;
        }
    }
}

void textbox_destroy_all(void)
{
    int i;
    struct TextBox *tbox;

    for (tbox = textBoxes, i = 0; i < 4; i++, tbox++)
        tbox->state = TEXTBOX_STATE_INACTIVE;
}

static void draw_textbox(int a, struct TextBox *b);

void textbox_draw_all(void)
{
    int i;
    struct TextBox *tbox;

    for (tbox = textBoxes, i = 0; i < 4; i++, tbox++)
    {
        if (tbox->state != TEXTBOX_STATE_INACTIVE)
            draw_textbox(i, tbox);
    }
}

static void calc_textbox_text_pos(struct TextBox *a, int *b, int *c);

static void draw_textbox(int a, struct TextBox *tbox)
{
    int x, y;
    int textX, textY;
    float xscale, yscale;
    float opacity;
    float numRows;  // why is this a float?
    float numColumns;  // why is this a float
    struct TextBoxLine *line;
    int row, col;
    float f26;
    float f25;
    float t;
    float f1;
    float f0;
    int scrollOffset;
    float opacity2;

    if (tbox->state == TEXTBOX_STATE_INACTIVE
     || tbox->state == TEXTBOX_STATE_2
     || tbox->state == TEXTBOX_STATE_3)
        return;
    if (tbox->numColumns == 0)
        return;

    reset_text_draw_settings();
    set_text_font(tbox->style == TEXTBOX_STYLE_SPIKY ? FONT_ICON_SD2 : FONT_ICON_SD);
    func_80071B1C(a * 0.01f + 0.059999999776482585);
    set_text_mul_color(tbox->bgColor);
    t = (float)tbox->timer / (float)tbox->timerMax;
    if (tbox->state == TEXTBOX_STATE_INIT)
    {
        xscale = (t < 0.5) ? t * 2.0f : 1.0;
        yscale = (t < 0.5) ? 0.1 : (t - 0.5) * 1.8 + 0.1;
        opacity = (t < 0.5) ? t * 2.0f : 1.0;
    }
    else if (tbox->state >= TEXTBOX_STATE_FADEOUT && t > 0.5)
    {
        t = (t - 0.5) * 2.0;
        xscale = (t < 0.5) ? 1.0 : t * 2.0f;
        yscale = (t < 0.5) ? (1.0 - t) * 1.8 + 0.1 : 0.1;
        opacity = 1.0 - t;
    }
    else
    {
        xscale = 1.0f;
        yscale = 1.0f;
        opacity = 1.0f;
    }
    set_text_scale(xscale, yscale);
    set_text_opacity(opacity);
    calc_textbox_text_pos(tbox, &x, &y);
    x += 0.5 * ((1.0 - xscale) * (TEXTBOX_FONT_SIZE * 2 + tbox->textWidth));
    y += 0.5 * (TEXTBOX_FONT_SIZE * ((1.0 - yscale) * tbox->numRows));
    if (tbox->style == TEXTBOX_STYLE_THIN_BORDER)
    {
        x += TEXTBOX_FONT_SIZE/2;
        y += TEXTBOX_FONT_SIZE/2;
    }

    // draw borders (using special characters from font)
    numRows = tbox->numRows;
    numColumns = tbox->numColumns;
    set_text_pos(x - TEXTBOX_FONT_SIZE, y - TEXTBOX_FONT_SIZE);
    for (row = -1; row <= numRows; row++)
    {
        for (col = -1; col <= numColumns; col++)
        {
            f26 = xscale;
            f25 = yscale;
            if (tbox->style == TEXTBOX_STYLE_THIN_BORDER && (row == -1 || row == numRows))
                f25 *= 0.5;
            if (tbox->style == TEXTBOX_STYLE_THIN_BORDER && (col == -1 || col == numColumns))
                f26 *= 0.5;
            set_text_scale(f26, f25);

            if (col == -1)  // left border
            {
                if (row == -1)
                    sprite_puts("\x01");  // draw top left corner
                else if (row == numRows)
                    sprite_puts("\x0B");  // draw bottom left corner
                else
                    sprite_puts("\x06");  // draw left border
            }
            else if (col == numColumns)  // right border
            {
                if (row == -1)
                    sprite_puts("\x03");  // draw top right corner
                else if (row == numRows)
                    sprite_puts("\x0D");  // draw bottom right corner
                else
                    sprite_puts("\x08");  // draw right border

            }
            else if (tbox->style == TEXTBOX_STYLE_SPIKY
             && (row == -1 || row == numRows))
            {
                float f1 = mathutil_floor(tbox->textWidth / TEXTBOX_FONT_SIZE);
                if (col < f1)
                {
                    set_text_scale((tbox->textWidth / f1 / TEXTBOX_FONT_SIZE) * f26, f25);
                    if (row == -1)
                        sprite_puts("\x02");  // draw top border
                    else if (row == numRows)
                        sprite_puts("\x0C");  // draw bottom border
                    else
                        sprite_puts("\x07");  // draw inside background
                    set_text_scale(f26, f25);
                }
            }
            else if (col == 0)
            {
                set_text_scale(f26 * tbox->textWidth / TEXTBOX_FONT_SIZE, f25);
                if (row == -1)
                    sprite_puts("\x02");  // draw top border
                else if (row == numRows)
                    sprite_puts("\x0C");  // draw bottom border
                else
                    sprite_puts("\x07");  // draw inside background
                set_text_scale(f26, f25);
            }
        }
        sprite_puts("\n");
    }

    func_80071B1C(a * 0.01f + 0.05);
    if (tbox->state >= 20)
        f1 = 1.0f - tbox->timer / 15.0f;
    else if (tbox->state == 10)
        f1 = tbox->timer / 15.0f;
    else
        f1 = 0.0f;
    f0 = CLAMP(f1, 0.0f, 1.0f);
    set_text_opacity(f0);
    
    // draw arrow
    switch (tbox->style)
    {
        float zero;
    case TEXTBOX_STYLE_TOP_LEFT:
        set_text_pos(
            x - TEXTBOX_FONT_SIZE,
            y);
        sprite_puts("\x04");  // left arrow
        break;
    case TEXTBOX_STYLE_CENTER_LEFT:
        set_text_pos(
            x - TEXTBOX_FONT_SIZE,
            y + ((tbox->numRows - 1) * TEXTBOX_FONT_SIZE) * 0.5f);
        sprite_puts("\x04");  // left arrow
        break;
    case TEXTBOX_STYLE_BOTTOM_LEFT:
        set_text_pos(
            x - TEXTBOX_FONT_SIZE,
            y + (tbox->numRows - 1) * TEXTBOX_FONT_SIZE);
        sprite_puts("\x04");  // left arrow
        break;
    case TEXTBOX_STYLE_TOP_RIGHT:
        set_text_pos(
            x + tbox->textWidth,
            y);
        sprite_puts("\x05");  // right arrow
        break;
    case TEXTBOX_STYLE_CENTER_RIGHT:
        set_text_pos(
            x + tbox->textWidth,
            y + ((tbox->numRows - 1) * TEXTBOX_FONT_SIZE) * 0.5f);
        sprite_puts("\x05");  // right arrow
        break;
    case TEXTBOX_STYLE_BOTTOM_RIGHT:
        set_text_pos(
            x + tbox->textWidth,
            y + (tbox->numRows - 1) * TEXTBOX_FONT_SIZE);
        sprite_puts("\x05");  // right arrow
        break;
    case TEXTBOX_STYLE_LEFT_UP:
        zero = 0.0f;
        set_text_pos(
            (x + zero) + 14.0f,
            y - TEXTBOX_FONT_SIZE);
        sprite_puts("\x10");  // up arrow
        break;
    case TEXTBOX_STYLE_CENTER_UP:
        set_text_pos(
            x + tbox->textWidth * 0.5f - 10.0f,
            y - TEXTBOX_FONT_SIZE);
        sprite_puts("\x10");  // up arrow
        break;
    case TEXTBOX_STYLE_RIGHT_UP:
        set_text_pos(
            x + tbox->textWidth - 34.0f,
            y - TEXTBOX_FONT_SIZE);
        sprite_puts("\x10");  // up arrow
        break;
    case TEXTBOX_STYLE_LEFT_DOWN:
        zero = 0.0f;
        set_text_pos(
            (x + zero) + 14.0f,
            y + (tbox->numRows * TEXTBOX_FONT_SIZE));
        sprite_puts("\x15");  // down arrow
        break;
    case TEXTBOX_STYLE_CENTER_DOWN:
    case TEXTBOX_STYLE_SPIKY:
        set_text_pos(
            x + tbox->textWidth * 0.5f - 10.0f,
            y + (tbox->numRows * TEXTBOX_FONT_SIZE));
        sprite_puts("\x15");  // down arrow
        break;
    case TEXTBOX_STYLE_RIGHT_DOWN:
        set_text_pos(
            x + tbox->textWidth - 34.0f,
            y + (tbox->numRows * TEXTBOX_FONT_SIZE));
        sprite_puts("\x15");  // down arrow
        break;
    }

    set_text_scale(xscale, yscale);
    if (tbox->state < 10)
    {
        reset_text_draw_settings();
        return;
    }
    func_80071B50(0x220000);
    set_text_font(FONT_JAP_24x24_2P);
    func_80071B1C(a * 0.01f + 0.05);
    set_text_mul_color(RGBA(0, 0, 0, 0));
    if (tbox->state == TEXTBOX_STATE_SCROLL)
        scrollOffset = tbox->timer * -1.5;
    else
        scrollOffset = 0;
    if (tbox->state >= TEXTBOX_STATE_FADEOUT)
    {
        float temp_f1_2 = tbox->timerMax * 0.5;
        opacity2 = (tbox->timer < temp_f1_2) ? 1.0f - tbox->timer * (1.0f / temp_f1_2) : 0.0;
    }
    else
        opacity2 = 1.0f;
    
    // draw text
    line = textBoxLines[a];
    for (row = 0; row < 20; row++, line++)
    {
        if (line->unk0 == 0)
            break;
        lbl_802F200C = line->unk82;
        lbl_802F2008 = u_get_ascii_text_width(line->text);
        if (tbox->state == TEXTBOX_STATE_SCROLL)
        {
            if (row == 0)
                opacity2 = tbox->timer < 8 ? 1.0 - tbox->timer * 0.14 : 0.0;
            else
                opacity2 = 1.0f;
        }
        set_text_opacity(opacity2);
        if (tbox->textWidth > 0.0)
        {
            float f0 = MIN(tbox->textWidth / u_get_jpn_text_width(FONT_JAP_24x24_2P, line->text), 1.0);
            set_text_scale(f0, 1.0f);
        }
        else
            set_text_scale(1.0f, 1.0f);
        calc_textbox_text_pos(tbox, &textX, &textY);
        set_text_pos(textX, textY + row * TEXTBOX_FONT_SIZE + scrollOffset);
        sprite_puts(line->text);
    }
    lbl_802F200C = -1.0f;
    reset_text_draw_settings();
}

void clear_lines(int id)
{
    int i;
    struct TextBoxLine *line = textBoxLines[id];

    for (i = 0; i < 20; i++, line++)
    {
        line->unk0 = 0;
        line->unk82 = 0;
        strcpy(line->text, "");
    }
}

/* Computes the position of the text area of the textbox,
 * taking into account its borders */
static void calc_textbox_text_pos(struct TextBox *tbox, int *x, int *y)
{
    float textWidth = tbox->textWidth;
    float textHeight = (tbox->numRows - 1) * (float)TEXTBOX_FONT_SIZE;

    switch (tbox->style)
    {
    default:
        *x = tbox->x - textWidth * 0.5f;
        *y = tbox->y - tbox->numRows * 12.0f;
        break;
    case TEXTBOX_STYLE_TOP_LEFT:
        *x = tbox->x + 18;
        *y = tbox->y - 12;
        break;
    case TEXTBOX_STYLE_CENTER_LEFT:
        *x = tbox->x + 18;
        *y = tbox->y - 12 - textHeight * 0.5;
        break;
    case TEXTBOX_STYLE_BOTTOM_LEFT:
        *x = tbox->x + 18;
        *y = tbox->y - 12 - textHeight;
        break;
    case TEXTBOX_STYLE_TOP_RIGHT:
        *x = tbox->x - textWidth - 18.0f;
        *y = tbox->y - 12;
        break;
    case TEXTBOX_STYLE_CENTER_RIGHT:
        *x = tbox->x - textWidth - 18.0f;
        *y = tbox->y - 12 - textHeight * 0.5f;
        break;
    case TEXTBOX_STYLE_BOTTOM_RIGHT:
        *x = tbox->x - textWidth - 18.0f;
        *y = tbox->y - 12 - textHeight;
        break;
    case TEXTBOX_STYLE_LEFT_UP:
        *x = tbox->x - TEXTBOX_FONT_SIZE;
        *y = tbox->y + 18;
        break;
    case TEXTBOX_STYLE_CENTER_UP:
        *x = tbox->x - textWidth * 0.5;
        *y = tbox->y + 18;
        break;
    case TEXTBOX_STYLE_RIGHT_UP:
        *x = tbox->x - textWidth + TEXTBOX_FONT_SIZE;
        *y = tbox->y + 18;
        break;
    case TEXTBOX_STYLE_LEFT_DOWN:
        *x = tbox->x - TEXTBOX_FONT_SIZE;
        *y = tbox->y - 42 - textHeight;
        break;
    case TEXTBOX_STYLE_CENTER_DOWN:
    case TEXTBOX_STYLE_SPIKY:
        *x = tbox->x - textWidth * 0.5;
        *y = tbox->y - 42 - textHeight;
        break;
    case TEXTBOX_STYLE_RIGHT_DOWN:
        *x = tbox->x - textWidth + TEXTBOX_FONT_SIZE;
        *y = tbox->y - 42 - textHeight;
        break;
    }
}

/* Configures a textbox. If template is not NULL, parameters are copied from it. */
void textbox_set_properties(int id, int newState, struct TextBox *template)
{
    int bgColor;
    int style;
    int x;
    int y;
    struct TextBox *tbox = &textBoxes[id];

    if (newState == TEXTBOX_STATE_FADEOUT && tbox->state >= TEXTBOX_STATE_FADEOUT)
    {
        tbox->state = TEXTBOX_STATE_FADEOUT;
        return;
    }
    if (newState == TEXTBOX_STATE_FADEOUT && tbox->state != 1 && tbox->state < TEXTBOX_STATE_FADEIN)
        return;
    if (template == NULL)
        template = tbox;

    tbox->timerMax = 30;
    tbox->timer = 0;
    style = (template->style == 0) ? tbox->style : template->style;
    x = (template->x == 0) ? tbox->x : template->x;
    y = (template->y == 0) ? tbox->y : template->y;
    bgColor = (template->bgColor == 0) ? RGBA(255, 255, 255, 0) : template->bgColor;
    if (newState == TEXTBOX_STATE_INIT || newState == TEXTBOX_STATE_2)
    {
        clear_lines(id);
        tbox->id = id;
        tbox->state = newState;
        tbox->x = x;
        tbox->y = y;
        tbox->numColumns = template->numColumns;
        tbox->textWidth = tbox->numColumns * TEXTBOX_FONT_SIZE;
        tbox->numRows = template->numRows;
        tbox->style = template->style;
        tbox->bgColor = bgColor;
        tbox->unk17 = template->unk17;
        tbox->unk18 = template->unk18;
        tbox->unk19 = template->unk19;
        tbox->callback = template->callback;
        return;
    }

    if (newState == TEXTBOX_STATE_21)
    {
        if (tbox->state < TEXTBOX_STATE_FADEIN)
        {
            tbox->state = TEXTBOX_STATE_3;
            tbox->x = x;
            tbox->y = y;
            tbox->numColumns = template->numColumns;
            tbox->textWidth = tbox->numColumns * TEXTBOX_FONT_SIZE;
            tbox->numRows = template->numRows;
            tbox->style = style;
            tbox->bgColor = bgColor;
            return;
        }
        tbox->state = newState;
        memcpy(&textBoxesBackup[id], tbox, sizeof(textBoxesBackup[id]));
        textBoxesBackup[id].x = x;
        textBoxesBackup[id].y = y;
        textBoxesBackup[id].numColumns = template->numColumns;
        textBoxesBackup[id].numRows = template->numRows;
        textBoxesBackup[id].style = style;
        textBoxesBackup[id].bgColor = bgColor;
        return;
    }

    tbox->state = newState;
}

static void add_textbox_line(int id, const char *str)
{
    int i;
    struct TextBoxLine *line;
    struct TextBox *tbox;

    tbox = &textBoxes[id];
    line = textBoxLines[id];
    for (i = 0; i < 20; i++, line++)
    {
        if (line->unk0 == 0)
        {
            line->unk0 = 2;
            strcpy(line->text, str);
            line->unk82 = 0;
            if (i == tbox->numRows && tbox->state == TEXTBOX_STATE_FADEIN)
            {
                tbox->state = TEXTBOX_STATE_SCROLL;
                tbox->timer = 0;
            }
            break;
        }
    }
}

void textbox_add_text(int id, const char *str)
{
    char buffer[0x200];
    int length = 0;
    int newLines = 0;
    
    while (*str != 0)
    {
        if (*str == '\n')
        {
            buffer[length] = 0;
            add_textbox_line(id, buffer);
            length = 0;
            newLines++;
        }
        else
        {
            buffer[length] = *str;
            length++;
        }
        str++;
    }
    buffer[length] = 0;
    add_textbox_line(id, buffer);
    if (newLines + 1 > textBoxesBackup[id].numRows)
        textBoxesBackup[id].numRows = newLines + 1;
}

void textbox_add_textf(int id, const char *fmt, ...)
{
    va_list args;
    char buffer[0x200];

    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    textbox_add_text(id, buffer);
}
