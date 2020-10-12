/*
 * sound_process.c
 *
 *  Created on: Oct 12, 2020
 *      Author: u
 */

#include "sound_process.h"
#include "usart.h"
#include "FIR.h"

extern arm_rfft_fast_instance_f32 S;
extern LCD_str LCD1;
extern float screan_buf[];

volatile float mass = 0;
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
    static u8 mas_last;
    color[mas_last] = RGB565(0, 0, 0);
    mas_last = (u8) mass*5;
    color[(u8) mass*5] = RGB565(0, 255, 0);

    ST7789_FillLine(line, color, &LCD1);
    }

void get_firs_half_pcm_data(void)
    {
    //берем каждый второй семпл, так как у нас только левый канал
    for (u32 i = 0; i < I2S_INPUT_RAW_SEMPLS_NUM / 4; i++)
	{
	PCM_rx_buf2[i] = fir((float) (PCM_rx_buf[i * 2] >> 8)) * screan_buf[i];
	}
    }

void get_second_half_pcm_data(void)
    {
    //берем каждый второй семпл, так как у нас только левый канал
    for (u32 i = I2S_INPUT_RAW_SEMPLS_NUM / 4; i < I2S_INPUT_RAW_SEMPLS_NUM / 2;
	    i++)
	{
	PCM_rx_buf2[i] = fir((float) (PCM_rx_buf[i * 2] >> 8)) * screan_buf[i];
	}
    }

void Get_real_fft_result(float *in, float *out)
    {
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	out[i] = sqrtf(in[i * 2] * in[i * 2] + in[i * 2 + 1] * in[i * 2 + 1]);
	}
    }

void Uart_upd(float *data)
    {
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	Uart_IntWrite((s32) (data[i]));
	uart_send_char('\n');
	}
    //uart_send_char('\n');
    }
#define DECIMATION_ORDER 4
static u8 dec_step = 0;
u8 Downsampling(void)
    {
    for (volatile u32 i = 0; i < I2S_DECIMATION_BUF_LEN / DECIMATION_ORDER; i++)
	{
	float sum = 0;
	for (u8 j = 0; j < DECIMATION_ORDER; j++)
	    {
	    sum = sum + PCM_rx_buf2[i * DECIMATION_ORDER + j];
	    }
	downsampling_buf[i
		+ (dec_step * (I2S_DECIMATION_BUF_LEN / DECIMATION_ORDER))] =
		sum / DECIMATION_ORDER;
	}
    dec_step++;
    if (dec_step == DECIMATION_ORDER)
	{
	dec_step = 0;
	return 1;
	}
    return 0;
    }
volatile float peak_res = 0;
void peak_detector(void)
    {
    float sum = 0;
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	sum = sum + FFT_result_real[i];
	}
    peak_res = sum / I2S_FFT_RESULT_REAL_BUF_LEN;
    }

void max_power_dot_calc(void)
    {
    float sum = 0;
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	sum = sum + FFT_result_real[i] * i;
	}
    mass = sum / (peak_res * I2S_FFT_RESULT_REAL_BUF_LEN);
    }

void LCD_upd(float *data)
    {
    static u8 lcd_line = 0;
    if (lcd_line <= 240)
	{
	LCD(data, lcd_line);
	lcd_line++;
	}
    else
	{
	lcd_line = 0;
	}
    }

void i2s_dma_full_Rx(void)
    {
    static u32 button_flag = 0;
    LD1_GPIO_Port->ODR |= LD1_Pin;
    get_second_half_pcm_data();
    if (Downsampling() == 1)
	{
	arm_rfft_fast_f32(&S, downsampling_buf, FFT_result, 0);
	Get_real_fft_result(FFT_result, FFT_result_real);
	peak_detector();
	max_power_dot_calc();
//кнопка
	if (((B1_GPIO_Port->IDR & B1_Pin) != 0) && (button_flag == 0))
	    {
	    Uart_upd(downsampling_buf);
	    }
	else
	    {
	    LCD_upd(FFT_result_real);
	    }
	button_flag = B1_GPIO_Port->IDR & B1_Pin;
	}
    LD1_GPIO_Port->ODR &= ~ LD1_Pin;
    }

void i2s_dma_half_Rx(void)
    {
    get_firs_half_pcm_data();
    }
