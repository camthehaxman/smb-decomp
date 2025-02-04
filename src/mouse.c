#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "bitmap.h"
#include "input.h"
#include "mouse.h"
#include "pool.h"
#include "sprite.h"
#include "window.h"

struct Mouse {
    s16 posHorizontal;
    s16 posVertical;
    s16 unk04;
    s16 unk06;
    s16 unk08;
    s16 unk0a;
    s16 spriteIdx;
    s16 unk0e;
};

struct Mouse mouse;

#define INVALID_SPRITE_INDEX -1;

# define HORIZONTAL_DEFAULT 0x140;
# define VERTICAL_DEFAULT 0xf0;
void ev_mouse_init(void)
{
    mouse.posHorizontal = HORIZONTAL_DEFAULT;
    mouse.posVertical = VERTICAL_DEFAULT;
    mouse.spriteIdx = INVALID_SPRITE_INDEX;
    return;
}

# define HORIZONTAL_MIN 0x00
# define HORIZONTAL_MAX 0x270
# define VERTICAL_MIN 0x00
# define VERTICAL_MAX 0x1D0

void ev_mouse_main(void)
{
    struct Sprite *_spriteInfo;
    int phi_r8;
    s8 *status;
    u16 _button;
    int i;
    
    if (
        (lbl_802F1EA8 == 0U)
        && ((dipSwitches & DIP_TEST_CAM) == 0)
    ) {
        mouse.unk04 = mouse.posHorizontal;
        mouse.unk06 = mouse.posVertical;

        phi_r8 = 1;
        if (analogInputs[0].held & ANALOG_TRIGGER_RIGHT) {
            phi_r8 = 8;
        }
        _button = controllerInfo[0].repeat.button;
        if ((_button & PAD_BUTTON_UP) != 0) {
            // press D-pad up
            mouse.posVertical -= phi_r8;
        }
        if ((_button & PAD_BUTTON_DOWN) != 0) {
            // press D-pad down
            mouse.posVertical += phi_r8;
        }
        
        if ((_button & PAD_BUTTON_LEFT) != 0) {
            // press D-pad left
            mouse.posHorizontal -= phi_r8;
        }
        if ((_button & PAD_BUTTON_RIGHT) != 0) {
            // press D-pad right
            mouse.posHorizontal += phi_r8;
        }
        
        mouse.posHorizontal += (controllerInfo[0].held.stickX * 0.1);
        mouse.posVertical += (-controllerInfo[0].held.stickY * 0.1);

        // Horizontal Check
        if (mouse.posHorizontal < HORIZONTAL_MIN) {
            mouse.posHorizontal = HORIZONTAL_MIN;
        }
        if (mouse.posHorizontal > HORIZONTAL_MAX) {
            mouse.posHorizontal = HORIZONTAL_MAX;
        }

        // Vertical Check
        if (mouse.posVertical < VERTICAL_MIN) {
            mouse.posVertical = VERTICAL_MIN;
        }
        if (mouse.posVertical > VERTICAL_MAX) {
            mouse.posVertical = VERTICAL_MAX;
        }

        mouse.unk08 = mouse.posHorizontal - mouse.unk04;
        mouse.unk0a = mouse.posVertical - mouse.unk06;

        if ( (controllerInfo[0].pressed.button & PAD_BUTTON_A) != 0 ) {
            // if press "A button"
            
            if ( mouse.spriteIdx >= 0 ) {
                mouse.spriteIdx = INVALID_SPRITE_INDEX;
            } else {
                mouse.spriteIdx = INVALID_SPRITE_INDEX;
                _spriteInfo = &spriteWork[0];
                status = g_poolInfo.spritePool.statusList;
                for (
                    i = 0;
                    i < g_poolInfo.spritePool.count;
                    i++, _spriteInfo++, status++
                    ) {
                    if (
                        *status != STAT_NULL
                        && mouse.posHorizontal >= _spriteInfo->left
                        && mouse.posHorizontal <= _spriteInfo->right
                        && mouse.posVertical >= _spriteInfo->top
                        && mouse.posVertical <= _spriteInfo->bottom
                    ) {
                        mouse.spriteIdx = i;
                        break;
                    }
                }
            }
        }
        
        if (mouse.spriteIdx >= 0) {
            _spriteInfo = &spriteWork[mouse.spriteIdx];
            while(_spriteInfo->unk50 != NULL) {
                _spriteInfo = _spriteInfo->unk50;
            }
            _spriteInfo->x = _spriteInfo->x + mouse.unk08;
            _spriteInfo->y = _spriteInfo->y + mouse.unk0a;
        }
    }
    return;
}

void ev_mouse_dest(void){
    return;
}

void mouse_draw(void)
{
    NLsprarg _naomiSprite;
    struct Sprite *_spriteInfo;

    if (
        (lbl_802F1EA8 == 0U)
    && ((dipSwitches & DIP_TEST_CAM) == 0)
    ) {
        _naomiSprite.sprno = BMP_COM_arrow;
        _naomiSprite.x = mouse.posHorizontal;
        _naomiSprite.y = mouse.posVertical;
        _naomiSprite.z = 0.01f;
        _naomiSprite.zm_x = 1.0f;
        _naomiSprite.zm_y = 1.0f;
        _naomiSprite.u0 = 0.0f;
        _naomiSprite.v0 = 0.0f;
        _naomiSprite.u1 = 1.0f;
        _naomiSprite.v1 = 1.0f;
        _naomiSprite.ang = 0x0;
        _naomiSprite.trnsl = 1.0f;
        _naomiSprite.listType = NLSPR_LISTTYPE_AUTO;
        _naomiSprite.attr = NLSPR_DISP_LT;
        _naomiSprite.base_color = -1;
        _naomiSprite.offset_color = 0x0;
        nlSprPut(&_naomiSprite);
        if (mouse.spriteIdx >= 0) {
            _spriteInfo = &spriteWork[mouse.spriteIdx];
            reset_text_draw_settings();
            set_text_pos( (_spriteInfo->left - 8), (_spriteInfo->top - 8) );
            sprite_puts( "+" );
            set_text_pos( (_spriteInfo->left - 8), _spriteInfo->bottom );
            sprite_puts( "+" );
            set_text_pos( _spriteInfo->right, (_spriteInfo->top - 8) );
            sprite_puts( "+" );
            set_text_pos( _spriteInfo->right, _spriteInfo->bottom );
            sprite_puts( "+" );
        }
    }
    return;
}
