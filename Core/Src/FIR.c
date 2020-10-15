/*
 * FIR.c
 *
 *  Created on: Oct 13, 2020
 *      Author: u
 */

#include "FIR.h"
#include "sound_process.h"

float screan_buf[I2S_PCM_LEFT_CHAN_LEN];
void SIN_screan_gen(void)
    {
    for (u32 i = 0; i < I2S_PCM_LEFT_CHAN_LEN; i++)
	{
	screan_buf[i] = sinf((PI * i) / (I2S_PCM_LEFT_CHAN_LEN - 1));
	}
    }

/**************************************************************
 WinFilter version 0.8
 http://www.winfilter.20m.com
 akundert@hotmail.com

 Filter type: Low Pass
 Filter model: Rectangular Window
 Sampling Frequency: 48 KHz
 Cut Frequency: 3.000000 KHz
 Coefficents Quantization: float
 ***************************************************************/
#define Ntap 32

float fir(float NewSample)
    {
    float FIRCoef[Ntap] =
	{
		-0.00895334511452619900,
		-0.01773178824138216300,
		-0.02495831119627491000,
		-0.02927511364676441500,
		-0.02951400500312740100,
		-0.02485450782330384700,
		-0.01494930620851395200,
		-0.00000000000000007408,
		0.01922826580783954300,
		0.04145750440594751800,
		0.06500918645069687900,
		0.08796673419497248700,
		0.10837040891977311000,
		0.12442248822183287000,
		0.13467900152375276000,
		0.13820557541815492000,
		0.13467900152375276000,
		0.12442248822183287000,
		0.10837040891977311000,
		0.08796673419497248700,
		0.06500918645069687900,
		0.04145750440594751800,
		0.01922826580783954300,
		-0.00000000000000007408,
		-0.01494930620851395200,
		-0.02485450782330384700,
		-0.02951400500312740100,
		-0.02927511364676441500,
		-0.02495831119627491000,
		-0.01773178824138216300,
		-0.00895334511452619900,
		-0.00000000000000000935
	};

    static float x[Ntap]; //input samples
    float y = 0;            //output sample
    int n;

    //shift the old samples
    for (n = Ntap - 1; n > 0; n--)
	x[n] = x[n - 1];

    //Calculate the new output
    x[0] = NewSample;
    for (n = 0; n < Ntap; n++)
	y += FIRCoef[n] * x[n];

    return y;
    }

