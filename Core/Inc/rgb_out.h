/*
 * rgb_out.h
 *
 *  Created on: Oct 25, 2020
 *      Author: u
 */

#ifndef INC_RGB_OUT_H_
#define INC_RGB_OUT_H_

#include "main.h"

typedef struct RGB_out_structure
    {
	TIM_HandleTypeDef *tim_str;
	u16 r;
	u16 g;
	u16 b;
	u32 r_chan;
	u32 g_chan;
	u32 b_chan;
    } RGB_out_str;

void RGB_init(RGB_out_str *rgb);
void HSV_to_RGB888(u16 hsv_h, u16 hsv_s, u8 hsv_v, RGB_out_str *rgb);
float map(float x, float in_min, float in_max, float out_min, float out_max);
void RGB_out(RGB_out_str *rgb);

#endif /* INC_RGB_OUT_H_ */
