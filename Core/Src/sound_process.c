/*
 * sound_process.c
 *
 *  Created on: Oct 12, 2020
 *      Author: u
 */

#include "sound_process.h"
#include "usart.h"
#include "FIR.h"
#include "rgb_out.h"
#include "rgb_process.h"

extern arm_rfft_fast_instance_f32 S;
extern LCD_str LCD1;

volatile float peak_res = 0;
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
	    //data[i] = 0;
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
    mas_last = (u8) mass * 5;
    color[(u8) mass * 5] = RGB565(0, 255, 0);

    ST7789_FillLine(line, color, &LCD1);
    }

void get_firs_half_pcm_data(void)
    {
    //берем каждый второй семпл, так как у нас только левый канал
    for (u32 i = 0; i < I2S_INPUT_RAW_SEMPLS_NUM / 4; i++)
	{
	PCM_rx_buf2[i] = fir((float) (PCM_rx_buf[i * 2] >> 8));
	}
    }

void get_second_half_pcm_data(void)
    {
    //берем каждый второй семпл, так как у нас только левый канал
    for (u32 i = I2S_INPUT_RAW_SEMPLS_NUM / 4; i < I2S_INPUT_RAW_SEMPLS_NUM / 2;
	    i++)
	{
	PCM_rx_buf2[i] = fir((float) (PCM_rx_buf[i * 2] >> 8));
	}
    }

void Get_real_fft_result(float *in, float *out)
    {
    for (u32 i = 0; i < I2S_FFT_RESULT_REAL_BUF_LEN; i++)
	{
	out[i] = sqrtf(in[i * 2] * in[i * 2] + in[i * 2 + 1] * in[i * 2 + 1]);
	}
    }

void Uart_upd(float *data, u32 len)
    {
    for (u32 i = 0; i < len; i++)
	{
	Uart_IntWrite((s32) (data[i]));
	uart_send_char('\n');
	}
    //uart_send_char('\n');
    }
#define DECIMATION_ORDER 8
float downsampling_buf2[128];
u8 Downsampling(void)
    {
    static u8 dec_step = 0;
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
	downsampling_buf2[i
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

#define OUT_MAX 1.0
#define OUT_MIN -1.0

#define COR_LEN 16

float cor[128 - COR_LEN];
volatile float cor_avg = 0;
volatile float cor_avg2 = 0;
float corelation(float *in)
    {
    float in_max = in[0];
    float in_min = in[0];
    float val[128];

    for (u32 i = 0; i < 128; i++)
	{
	if (in_max < in[i])
	    {
	    in_max = in[i];
	    }
	if (in_min > in[i])
	    {
	    in_min = in[i];
	    }
	}
    for (u32 i = 0; i < 128; i++)
	{
	val[i] = (in[i] - in_min) * (OUT_MAX - OUT_MIN)
		/ (in_max - in_min)+ OUT_MIN;
	}
    for (u32 i = COR_LEN; i < 128 - COR_LEN; i++)
	{
	float sum = 0;
	for (u32 j = 0; j < COR_LEN; j++)
	    {
	    sum = val[j] * val[i + j];
	    }
	cor[i - COR_LEN] = (sum * sum) * 10;
	}
    for (u32 i = 0; i < 128 - COR_LEN; i++)
	{
	cor_avg = cor_avg * (1.0 - 0.001) + cor[i] * 0.001;
	}
    cor_avg2 = cor_avg2 * (1.0 - 0.001) + cor_avg * 0.001;
    return 0;
    }

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
static u32 button_flag = 0;
float FFT_result_real_avg[64];
#define K_AVG 0.0001
void FFT_res_avg(float *in, float *out)
    {
    static u8 first_time_flag = 0;
    if (first_time_flag < 50)
	{
	for (u32 i = 0; i < 64; i++)
	    {
	    out[i] = (out[i] * (1 - 0.1)) + (in[i] * 0.1);
	    }
	first_time_flag++;
	}
    for (u32 i = 0; i < 64; i++)
	{
	out[i] = (out[i] * (1 - K_AVG)) + (in[i] * K_AVG);
	}
    }
float dx_data[64];
void FFT_dx(float *avg, float *new, float *out)
    {
    for (u32 i = 0; i < 64; i++)
	{
	out[i] = new[i] - avg[i];
	}
    }

void spectrum_avg(float *in, float *out)
    {
    for (u32 i = 0; i < 64; i++)
	{
	out[i] = avg(out[i], in[i], 0.01);
	}
    }

float downsempling_long[2048];
void i2s_dma_full_Rx(void)
    {
//static u8 count = 0;
    LD1_GPIO_Port->ODR |= LD1_Pin;
    get_second_half_pcm_data();

    if (Downsampling() == 1)
	{
	arm_rfft_fast_f32(&S, downsampling_buf, FFT_result, 0);
	Get_real_fft_result(FFT_result, FFT_result_real);
	peak_detector();
	max_power_dot_calc();
	RGB_out_calc(mass, peak_res);
	//FFT_res_avg(FFT_result_real, FFT_result_real_avg);
	//FFT_dx(FFT_result_real_avg, FFT_result_real, dx_data);
	spectrum_avg(FFT_result_real,FFT_result_real_avg);
	//corelation(downsampling_buf2);
	static u8 count = 0;
	static u8 adder = 0;
	if ((((B1_GPIO_Port->IDR & B1_Pin) == 0) && (button_flag != 0))) //кнопка
	//|| (count != 4))
	    {
	    adder = 1;
	    }
	button_flag = B1_GPIO_Port->IDR & B1_Pin;
	if (adder != 0)
	    {
	    if ((count >= 50) && (count < 66))
		{
		for (u32 i = 0; i < 128; i++)
		    {
		    downsempling_long[(count - 50) * 128 + i] =
			    downsampling_buf2[i];
		    }
		}
	    count = count + adder;
	    }
	if (count == 100)
	    {
	    count = 0;
	    adder = 0;
	    Uart_upd(FFT_result_real_avg, 64);
	    }
	LCD_upd(FFT_result_real);

	}
    LD1_GPIO_Port->ODR &= ~ LD1_Pin;
    }

void i2s_dma_half_Rx(void)
    {
    get_firs_half_pcm_data();
    }
