/*
 * rgb_process.c
 *
 *  Created on: Oct 25, 2020
 *      Author: u
 */

#include "rgb_process.h"
#include "sound_process.h"
#include "usart.h"
#include "rgb_out.h"

extern RGB_out_str RGB;
#define H_AVG_K 0.01
#define V_AVG_K 0.01

float avg(float last, float in, float k)
    {
    return (last * (1.0 - k)) + (in * k);
    }

u8 silent_or_music(float raw_sum, float avg_sum, float treshold, u32 silent_len,
	u32 music_len)
    {
    static u32 silent_count = 0;
    static u32 music_count = 0;
    static u8 result = 0;
    if (raw_sum < (avg_sum * treshold))
	{
	if (silent_count < silent_len)
	    {
	    silent_count++;
	    }
	else
	    {
	    if (silent_count < (silent_len * 2))
		{
		silent_count++;
		}
	    result = 0;
	    }
	if (music_count > 0)
	    {
	    music_count--;
	    }
	}
    else
	{
	if (music_count < music_len)
	    {
	    music_count++;
	    }
	else
	    {
	    if (music_count < (music_len * 2))
		{
		music_count++;
		}
	    result = 1;
	    }
	if (silent_count > 0)
	    {
	    silent_count--;
	    }
	}

    }

void RGB_out_calc(float in, float raw_sum)
    {
    static u8 firs_time = 0; //счетчик для инициализации всех
    static float h_offset = 0;
    static float in_avg = 0;
    static float h_avg = 0;
    static float v_avg = 0;
    if (firs_time < 250)
	{
	h_avg = avg(h_avg, in, 0.1);
	v_avg = avg(v_avg, raw_sum, 0.1);
	firs_time++;
	}
    v_avg = avg(v_avg, raw_sum, V_AVG_K);
    in_avg = avg(in_avg, in, H_AVG_K);

    float h = ((in - in_avg) * 5 + h_offset + 180);
    h_avg = avg(h_avg, h, 0.2);
    u16 v = (u16) map(raw_sum, 0, v_avg * 2, 0, 255);
    HSV_to_RGB888((u16) h_avg, 0xFF, v, &RGB);
    h_offset += 0.1;
    if (h_offset >= 360)
	{
	h_offset = 0;
	}
    RGB_out(&RGB);
//    Uart_IntWrite((s32) raw_sum);
//    uart_send_char('\t');
//    Uart_IntWrite((s32) v_avg);
//    uart_send_char('\n');
    }
