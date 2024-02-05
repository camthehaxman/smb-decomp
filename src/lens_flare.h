#ifndef _SRC_LENS_FLARE_H_
#define _SRC_LENS_FLARE_H_

void lens_flare_init(void);
void lens_flare_main(void);
void lens_flare_draw(int cameraId);
void lens_flare_draw_mask(int cameraId);
void lens_flare_set_light_position(Point3d *pos);
void lens_flare_set_light_angle(int angleY, int angleX);
void lens_flare_set_scale(float scale);

#endif
