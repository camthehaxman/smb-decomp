#ifndef _SRC_VIBRATION_H_
#define _SRC_VIBRATION_H_

enum VibrationState
{
	VIBRATION_STATE_0,
	VIBRATION_STATE_1,
	VIBRATION_STATE_2,
	VIBRATION_STATE_3,
	VIBRATION_STATE_4,
};

void ev_vibration_init(void);
void ev_vibration_main(void);
void ev_vibration_dest(void);
void vibration_control(int contId, enum VibrationState state, int duration);
extern void vibration_set_cont_enable_mask(u8 contMask);
u8 vibration_get_cont_enable_mask(void);

#endif
