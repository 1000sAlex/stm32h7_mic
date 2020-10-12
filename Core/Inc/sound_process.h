/*
 * sound_process.h
 *
 *  Created on: Oct 12, 2020
 *      Author: u
 */

#ifndef INC_SOUND_PROCESS_H_
#define INC_SOUND_PROCESS_H_

#include "main.h"

#define I2S_INPUT_RAW_SEMPLS_NUM 256
#define I2S_PCM_LEFT_CHAN_LEN (I2S_INPUT_RAW_SEMPLS_NUM/2)
#define I2S_DECIMATION_BUF_LEN (I2S_PCM_LEFT_CHAN_LEN/2)
#define I2S_FFT_RESULT_BUF_LEN (I2S_PCM_LEFT_CHAN_LEN)
#define I2S_FFT_RESULT_REAL_BUF_LEN (I2S_FFT_RESULT_BUF_LEN/2)
s32 PCM_rx_buf[I2S_INPUT_RAW_SEMPLS_NUM];
float32_t PCM_rx_buf2[I2S_PCM_LEFT_CHAN_LEN];
float32_t decimation_buf[I2S_DECIMATION_BUF_LEN];
float32_t FFT_result[I2S_FFT_RESULT_BUF_LEN];
float32_t FFT_result_real[I2S_FFT_RESULT_REAL_BUF_LEN];

void i2s_dma_full_Rx(void);
void i2s_dma_half_Rx(void);
#endif /* INC_SOUND_PROCESS_H_ */
