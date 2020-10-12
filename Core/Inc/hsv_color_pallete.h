/*
 * hsv_color_pallete.h
 *
 *  Created on: Oct 12, 2020
 *      Author: u
 */

#ifndef INC_HSV_COLOR_PALLETE_H_
#define INC_HSV_COLOR_PALLETE_H_

#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint8_t R;  /* Red */
    uint8_t G;  /* Green */
    uint8_t B;  /* Blue */
} RGB_TypeDef;

typedef struct
{
    uint8_t H;  /* Hue */
    uint8_t S;  /* Saturation */
    uint8_t V;  /* Value */
} HSV_TypeDef;

typedef enum
{
    RAINBOW,      /* Standart rainbow from red to purple*/
    YODA_RAINBOW, /* Rainbow with colors in other order*/
    SUNNY,        /* Yellow, orange, red mix */
    COLDY,        /* Cyan, light blue and blue mix */
    GREENY,       /* Hues of green and yellow */
    WATERFALL        /* Russian Federation flag colors =) */
} CPallete_Name_TypeDef;

void hsv2rgb(HSV_TypeDef* HSV, RGB_TypeDef* RGB, CPallete_Name_TypeDef name);

#endif /* INC_HSV_COLOR_PALLETE_H_ */
