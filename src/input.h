#ifndef _SRC_INPUT_H_
#define _SRC_INPUT_H_

#include <dolphin/types.h>
#include <dolphin/pad.h>

// Input state

struct ControllerInfo
{
    PADStatus held;      // buttons that are down this frame
    PADStatus prevHeld;  // buttons that were down on the previous frame
    PADStatus pressed;   // buttons that went down on this frame
    PADStatus released;  // buttons that went up on this frame
    PADStatus repeat;    // buttons that repeat due to being held
};

extern struct ControllerInfo controllerInfo[4];
extern struct ControllerInfo lbl_801F3C60[4];

// Analog input state

// This structure contains information about analog inputs (such as when a stick is tilted far
// enough to be counted as a press in a direction.
struct AnalogInput
{
    u16 held;      // inputs that are down this frame
    u16 prevHeld;  // inputs that were down on the previous frame
    u16 pressed;   // inputs that went down on this frame. However, due to an oversight, this field
                   // is not correct and is the same as pressed.
    u16 released;  // inputs that went up on this frame
    u16 repeat;    // inputs that repeat due to being held
};

// Bits for each of the members of AnalogInput
// The first four are equivalent to the PAD_BUTTON_ bits to make it easy for the analog stick to
// emulate D-Pad presses
#define ANALOG_STICK_LEFT    (1 << 0)  // 0x0001
#define ANALOG_STICK_RIGHT   (1 << 1)  // 0x0002
#define ANALOG_STICK_DOWN    (1 << 2)  // 0x0004
#define ANALOG_STICK_UP      (1 << 3)  // 0x0008
#define ANALOG_CSTICK_LEFT   (1 << 4)  // 0x0010
#define ANALOG_CSTICK_RIGHT  (1 << 5)  // 0x0020
#define ANALOG_CSTICK_DOWN   (1 << 6)  // 0x0040
#define ANALOG_CSTICK_UP     (1 << 7)  // 0x0080
#define ANALOG_TRIGGER_LEFT  (1 << 8)  // 0x0100
#define ANALOG_TRIGGER_RIGHT (1 << 9)  // 0x0200

extern struct AnalogInput analogInputs[4];

// Returns true if the analog input is either repeated or held while holding the right trigger.
// This allows the player to scroll through menus faster by holding the right trigger.
#define REPEAT_ANALOG_WITH_R_ACCEL(cont, input) \
( \
    (analogInputs[cont].repeat & (input)) \
 || ((analogInputs[cont].held & (input)) && (analogInputs[cont].held & ANALOG_TRIGGER_RIGHT)) \
)

// Returns true if the analog stick or D-pad is either repeated or held while holding the right trigger.
// This allows the player to scroll through menus faster by holding the right trigger.
#define REPEAT_WITH_R_ACCEL(cont, btn) \
( \
    ((controllerInfo[cont].repeat.button & (btn)) || (analogInputs[cont].repeat & (btn))) \
 || ( \
        ((controllerInfo[cont].held.button & (btn)) || (analogInputs[cont].held & (btn))) \
     && (analogInputs[cont].held & ANALOG_TRIGGER_RIGHT) \
    ) \
)

// like REPEAT_WITH_R_ACCEL, but checks all controllers
#define REPEAT_WITH_R_ACCEL_ANY(btn) (REPEAT_WITH_R_ACCEL(0, btn) || REPEAT_WITH_R_ACCEL(1, btn) || REPEAT_WITH_R_ACCEL(2, btn) || REPEAT_WITH_R_ACCEL(3, btn))

// Returns true if any controller pressed the button
#define ANY_CONTROLLER_PRESSED(btn) \
( \
    (controllerInfo[0].pressed.button & (btn)) \
 || (controllerInfo[1].pressed.button & (btn)) \
 || (controllerInfo[2].pressed.button & (btn)) \
 || (controllerInfo[3].pressed.button & (btn)) \
)

//extern struct Struct801F3D88 g_currPlayerButtons;
extern u16 g_currPlayerButtons[6];
//extern struct Struct801F3D94 g_currPlayerAnalogButtons;
extern u16 g_currPlayerAnalogButtons[6];

void input_init(void);
void input_main(void);
void func_80025158(PADStatus *);
void handle_analog_inputs(void);
void get_key_repeats(void);
void func_80025640(void);
void handle_reset_input(PADStatus *);
void func_80025B1C(struct CoordsS8 *, s8 *);
void save_input_data(struct MemcardContents *);
void load_input_data(const struct MemcardContents *);

#endif
