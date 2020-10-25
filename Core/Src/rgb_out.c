/*
 * rgb_out.c
 *
 *  Created on: Oct 25, 2020
 *      Author: u
 */

#include "rgb_out.h"

void RGB_init(RGB_out_str *rgb)
    {
    HAL_TIM_PWM_Start(rgb->tim_str, rgb->r_chan);
    HAL_TIM_PWM_Start(rgb->tim_str, rgb->g_chan);
    HAL_TIM_PWM_Start(rgb->tim_str, rgb->b_chan);
    }

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    if (x > in_max)
	{
	return out_max;
	}
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void HSV_to_RGB888(u16 hsv_h, u16 hsv_s, u8 hsv_v, RGB_out_str *rgb)
    {
    // @formatter:off
    if (hsv_s > 0xFF){hsv_s = 0xFF;}
    int hue = HUE(hsv_h);

    int sector = hue / 60;
    int angle = sector & 1 ? 60 - hue % 60 : hue % 60;

    int high = hsv_v;
    int low = (255 - hsv_s) * high / 255;
    int middle = low + (high - low) * angle / 60;

    switch (sector)
	{
    case 0: rgb->r = high; rgb->g = middle; rgb->b = low;break;// red -> yellow
    case 1: rgb->r = middle; rgb->g = high; rgb->b = low;break; // yellow -> green
    case 2: rgb->r = low; rgb->g = high; rgb->b = middle;break;// green -> cyan
    case 3: rgb->r = low; rgb->g = middle; rgb->b = high;break;// cyan -> blue
    case 4: rgb->r = middle; rgb->g = low; rgb->b = high;break;// blue -> magenta
    case 5: rgb->r = high; rgb->g = low; rgb->b = middle;break;// magenta -> red
    default:
	break;
	}
    }

void RGB_out(RGB_out_str *rgb)
    {
    u32 *r_chan = (u32*) ((u32) &rgb->tim_str->Instance->CCR1 + rgb->r_chan);
    u32 *g_chan = (u32*) ((u32) &rgb->tim_str->Instance->CCR1 + rgb->g_chan);
    u32 *b_chan = (u32*) ((u32) &rgb->tim_str->Instance->CCR1 + rgb->b_chan);
    (*r_chan) = rgb->r;
    (*g_chan) = rgb->g;
    (*b_chan) = rgb->b;
    }
