/*
 * rgb_process.h
 *
 *  Created on: Oct 25, 2020
 *      Author: u
 */

#ifndef INC_RGB_PROCESS_H_
#define INC_RGB_PROCESS_H_

#include "main.h"

void RGB_out_calc(float in, float raw_sum);
float avg(float last, float in, float k);

#endif /* INC_RGB_PROCESS_H_ */
