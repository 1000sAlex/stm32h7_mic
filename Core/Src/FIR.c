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
 Filter model: Butterworth
 Filter order: 8
 Sampling Frequency: 48 KHz
 Cut Frequency: 5.000000 KHz
 Coefficents Quantization: float

 Z domain Zeros
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000
 z = -1.000000 + j 0.000000

 Z domain Poles
 z = 0.526737 + j -0.224550
 z = 0.526737 + j 0.224550
 z = 0.496757 + j -0.074364
 z = 0.496757 + j 0.074364
 z = 0.592847 + j -0.378242
 z = 0.592847 + j 0.378242
 z = 0.709134 + j -0.533682
 z = 0.709134 + j 0.533682
 ***************************************************************/
#define Ntap 16

float fir(float NewSample)
    {
    float FIRCoef[Ntap] =
	{
		-0.03620406484328212600,
		-0.04081791430215513100,
		-0.02269622832661600700,
		0.02239495921573753400,
		0.08894373002377205200,
		0.16115012710952284000,
		0.21736054131393703000,
		0.23861633690038825000,
		0.21736054131393703000,
		0.16115012710952284000,
		0.08894373002377205200,
		0.02239495921573753400,
		-0.02269622832661600700,
		-0.04081791430215513100,
		-0.03620406484328212600,
		-0.01887863728222035100
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

