#ifndef _MINIMAP_EVENT_H_
#define _MINIMAP_EVENT_H_

enum MinimapState
{
	MINIMAP_STATE_0 = 0,
	MINIMAP_STATE_CLOSE = 1,
	MINIMAP_STATE_2 = 2,
	MINIMAP_STATE_OPEN = 3,
	MINIMAP_STATE_4 = 4,  // reset?
};

void ev_minimap_init(void);
void ev_minimap_main(void);
void ev_minimap_dest(void);
void minimap_set_state(enum MinimapState state);
void func_80084794(void *unused);
void minimap_zoom(void);
void minimap_draw(void);

#endif
