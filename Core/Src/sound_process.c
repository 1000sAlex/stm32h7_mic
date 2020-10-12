/*
 * sound_process.c
 *
 *  Created on: Oct 12, 2020
 *      Author: u
 */

#include "sound_process.h"
#include "usart.h"

extern arm_rfft_fast_instance_f32 S;
extern LCD_str LCD1;

u16 color[240];

#define K 0.1
static float max_val_avg = 0;
static float max_val = 0;
void LCD(float *data, u8 line)
    {
    max_val = 0;
    for (u32 i = 1; i < I2S_FFT_RESULT_REAL_BUF_LEN / 2; i++)
	{
	if (data[i] < 0)
	    {
	    data[i] = data[i] * -1.0;
	    }
	if (data[i] > max_val)
	    {
	    max_val = data[i];
	    }
	}
    max_val_avg = (max_val_avg * (1 - K)) + (max_val * K);
    HSV_TypeDef hsv;
    RGB_TypeDef rgb;
    hsv.S = 255;
    hsv.V = 255;
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	hsv.H = (u8) ((data[i] / max_val_avg) * 255.0);
	hsv2rgb(&hsv, &rgb, 5);
	u16 col = RGB565(rgb.R, rgb.G, rgb.B);
	color[i * 3 + 0] = col;
	color[i * 3 + 1] = col;
	color[i * 3 + 2] = col;
	//color[i * 4 + 3] = col;
	}
    ST7789_FillLine(line, color, &LCD1);
    }

void get_firs_half_pcm_data(void)
    {
    for (u32 i = 0; i < I2S_INPUT_RAW_SEMPLS_NUM / 4; i++)
	{
	PCM_rx_buf2[i] = (float) (PCM_rx_buf[i * 2] >> 8);
	}
    }

void get_second_half_pcm_data(void)
    {
    for (u32 i = I2S_INPUT_RAW_SEMPLS_NUM / 4; i < I2S_INPUT_RAW_SEMPLS_NUM / 2;
	    i++)
	{
	PCM_rx_buf2[i] = (float) (PCM_rx_buf[i * 2] >> 8);
	}
    }

void i2s_dma_full_Rx(void)
    {
    static u8 lcd_line = 0;
    static u32 button_flag = 0;
    LD1_GPIO_Port->ODR |= LD1_Pin;
    get_second_half_pcm_data();
    arm_rfft_fast_f32(&S, PCM_rx_buf2, FFT_result, 0);
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	FFT_result_real[i] = sqrtf(
		FFT_result[i * 2] * FFT_result[i * 2]
			+ FFT_result[i * 2 + 1] * FFT_result[i * 2 + 1]) / 64;
	}
    if (((B1_GPIO_Port->IDR & B1_Pin) != 0) && (button_flag == 0))
	{
//	for (u32 i = 0; i < 64; i++)
//	    {
//	    decimation_buf[i] = (PCM_rx_buf2[i * 2] + PCM_rx_buf2[i * 2 + 1])
//		    / 2;
//	    }
	for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	    {
	    Uart_IntWrite((s32) (FFT_result_real[i]));
	    uart_send_char('\n');
	    }
	}
    else
	{
	if (lcd_line <= 240)
	    {
	    LCD(FFT_result_real, lcd_line);
	    lcd_line++;
	    }
	else
	    {
	    lcd_line = 0;
	    }
	}
    button_flag = B1_GPIO_Port->IDR & B1_Pin;
    LD1_GPIO_Port->ODR &= ~ LD1_Pin;
    //uart_send_char('\n');
    }

void i2s_dma_half_Rx(void)
    {
    get_firs_half_pcm_data();
    }
