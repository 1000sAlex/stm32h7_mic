/*
 * hsv_color_pallete.c
 *
 *  Created on: Oct 12, 2020
 *      Author: u
 */

#include "hsv_color_pallete.h"
typedef struct
    {
	RGB_TypeDef *Colors;
	uint8_t ColorsTotal;
    } CPallete_TypeDef;

/* Colors            R   G   B*/
#define _WHITE      {255,255,255}
#define _BLACK      {0,	0,    0}
#define _RED        {255,  0,  0}
#define _ORANGE     {255,128,  0}
#define _YELLOW     {255,255,  0}
#define _GREENLIME  {191,255,  0}
#define _LIGHTGREEN {128,255,  0}
#define _GREEN      {  0,255,  0}
#define _CYAN       {  0,255,255}
#define _LIGHTBLUE  {  0,128,255}
#define _BLUE       {  0,  0,255}
#define _PURPLE     {128,  0,255}

/* Color palletes
 *
 * Strongly recomended to set _COLORS_TOTAL with one of this values:
 * 2, 4, 8, 16, 32, 64, 128
 */

/*RAINBOW*/
#define RAINBOW_COLORS_TOTAL    8
RGB_TypeDef rainbow_colors[RAINBOW_COLORS_TOTAL] =
    {
    _RED,
_ORANGE,
_YELLOW,
_GREEN,
_CYAN,
_BLUE,
_PURPLE,
_RED
}
;
#define _RAINBOW  {rainbow_colors, RAINBOW_COLORS_TOTAL}

/*YODA_RAINBOW*/
#define YODA_RAINBOW_COLORS_TOTAL    8
RGB_TypeDef yoda_rainbow_colors[YODA_RAINBOW_COLORS_TOTAL] =
    {
    _CYAN,
_PURPLE,
_BLUE,
_YELLOW,
_GREEN,
_ORANGE,
_RED,
_CYAN
}
;
#define _YODA_RAINBOW  {yoda_rainbow_colors, YODA_RAINBOW_COLORS_TOTAL}

/*SUNNY*/
#define SUNNY_COLORS_TOTAL      4
RGB_TypeDef sunny_colors[SUNNY_COLORS_TOTAL] =
    {
    _YELLOW,
_ORANGE,
_RED,
_YELLOW
}
;
#define _SUNNY  {sunny_colors, SUNNY_COLORS_TOTAL}

/*COLDY*/
#define COLDY_COLORS_TOTAL      4
RGB_TypeDef coldy_colors[COLDY_COLORS_TOTAL] =
    {
    _CYAN,
_LIGHTBLUE,
_BLUE,
_CYAN
}
;
#define _COLDY  {coldy_colors, COLDY_COLORS_TOTAL}

/*GREENY*/
#define GREENY_COLORS_TOTAL     4
RGB_TypeDef greeny_colors[GREENY_COLORS_TOTAL] =
    {
    _LIGHTGREEN,
_GREENLIME,
_YELLOW,
_LIGHTGREEN
}
;
#define _GREENY  {greeny_colors, GREENY_COLORS_TOTAL}

/*RUSSIA*/
#define RUSSIA_COLORS_TOTAL     4
RGB_TypeDef russia_colors[RUSSIA_COLORS_TOTAL] =
    {
    _WHITE,
_BLUE,
_RED,
_WHITE
}
;
#define _RUSSIA  {russia_colors, RUSSIA_COLORS_TOTAL}

/*RUSSIA*/
#define WATERFALL_COLORS_TOTAL     4
RGB_TypeDef waterfall_colors[RUSSIA_COLORS_TOTAL] =
    {
    _BLACK,
_BLUE,
_YELLOW,
_RED
}
;
#define _WATERFALL  {waterfall_colors, WATERFALL_COLORS_TOTAL}

/* Private variables ---------------------------------------------------------*/
/*List of color palletes must be the same order as in CPallete_Name_TypeDef*/
const CPallete_TypeDef cpallete[] =
    {
    _RAINBOW,
_YODA_RAINBOW,
_SUNNY,
_COLDY,
_GREENY,
_WATERFALL
}
;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * brief   Convert HSV values to RGB in the current color pallete
 * params
 *       HSV : struct to get input values
 *       RGB : struct to put calculated values
 *       name: color pallete name
 * retval  none
 */
void hsv2rgb(HSV_TypeDef *HSV, RGB_TypeDef *RGB, CPallete_Name_TypeDef name)
    {
    uint8_t tempR;
    uint8_t tempG;
    uint8_t tempB;
    int16_t diff;

    uint8_t sector_basecolor;
    uint8_t next_sector_basecolor;
    const uint8_t hues_per_sector = 256 / cpallete[name].ColorsTotal;
    uint8_t hue_in_sector;

//    hues_per_sector = 256 / cpallete[name].ColorsTotal;
    hue_in_sector = HSV->H % hues_per_sector;

    sector_basecolor = HSV->H / hues_per_sector;
//    if (sector_basecolor == (cpallete[name].ColorsTotal - 1))
//	next_sector_basecolor = 0;
//    else
    next_sector_basecolor = sector_basecolor + 1;

    /* Get Red from Hue */
    diff = ((cpallete[name].Colors[next_sector_basecolor].R
	    - cpallete[name].Colors[sector_basecolor].R) / hues_per_sector)
	    * hue_in_sector;
    if ((cpallete[name].Colors[sector_basecolor].R + diff) < 0)
	tempR = 0;
    else if ((cpallete[name].Colors[sector_basecolor].R + diff) > 255)
	tempR = 255;
    else
	tempR = (uint8_t) (cpallete[name].Colors[sector_basecolor].R + diff);

    /* Get Green from Hue */
    diff = ((cpallete[name].Colors[next_sector_basecolor].G
	    - cpallete[name].Colors[sector_basecolor].G) / hues_per_sector)
	    * hue_in_sector;
    if ((cpallete[name].Colors[sector_basecolor].G + diff) < 0)
	tempG = 0;
    if ((cpallete[name].Colors[sector_basecolor].G + diff) > 255)
	tempG = 255;
    else
	tempG = (uint8_t) (cpallete[name].Colors[sector_basecolor].G + diff);

    /* Get Blue from Hue */
    diff = ((cpallete[name].Colors[next_sector_basecolor].B
	    - cpallete[name].Colors[sector_basecolor].B) / hues_per_sector)
	    * hue_in_sector;
    if ((cpallete[name].Colors[sector_basecolor].B + diff) < 0)
	tempB = 0;
    if ((cpallete[name].Colors[sector_basecolor].B + diff) > 255)
	tempB = 255;
    else
	tempB = (uint8_t) (cpallete[name].Colors[sector_basecolor].B + diff);

    /* Saturation regulation */
    tempR = (255 - ((255 - tempR) * (HSV->S)) / 255);
    tempG = (255 - ((255 - tempG) * (HSV->S)) / 255);
    tempB = (255 - ((255 - tempB) * (HSV->S)) / 255);

    /* Value (brightness) regulation to get final result */
    RGB->R = (tempR * (HSV->V)) / 255;
    RGB->G = (tempG * (HSV->V)) / 255;
    RGB->B = (tempB * (HSV->V)) / 255;
    }
