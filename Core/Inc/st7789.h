/*
 * st7789.h
 *
 *  Created on: Sep 26, 2020
 *      Author: u
 */

#ifndef INC_ST7789_H_
#define INC_ST7789_H_

#include "main.h"
#include "spi.h"

typedef struct LCD_structure
    {
	SPI_HandleTypeDef *spi_str;
	u16 LCD_dc_pin;
	GPIO_TypeDef *LCD_dc_port;
	u16 LCD_res_pin;
	GPIO_TypeDef *LCD_res_port;
	u16 LCD_blk_pin;
	GPIO_TypeDef *LCD_blk_port;
    } LCD_str;

//==============================================================================
// Настройки подключения к интерфейсу дисплея
//==============================================================================
// Сигнал RESET
#define ST7789_RESET_Port     LCD_RES_GPIO_Port
#define ST7789_RESET_Pin      LCD_RES_Pin

// Сигнал DC
#define ST7789_DC_Port        LCD_DC_GPIO_Port
#define ST7789_DC_Pin         LCD_DC_Pin

//Подсветка
#define ST7789_BLK_Port        LCD_BLK_GPIO_Port
#define ST7789_BLK_Pin         LCD_BLK_Pin

#define ST7789_Width 240
#define ST7789_Height 240

#define RGB565(r, g, b)         (((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))&0xFF)<<8) | (((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))&0xFF00)>>8)

//#define RGB565(r, g, b)         (((b & 0x1F) << 8) | ((g & 0x7) << 13)| ((g & 0x38)>>3)| ((r & 0x1F) << 3))
#define HUE(h)  ({ typeof(h) h1 = h % 360; h1 < 0 ? 360 + h1 : h1; })
// Базовые цвета
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
//==============================================================================

// Битовые маски настройки цветности ST7789
#define ST7789_ColorMode_65K    0x50
#define ST7789_ColorMode_262K   0x60
#define ST7789_ColorMode_12bit  0x03
#define ST7789_ColorMode_16bit  0x05
#define ST7789_ColorMode_18bit  0x06
#define ST7789_ColorMode_16M    0x07

// Смещение матрицы относительно строк/столбцов контроллера
#define ST7789_X_Start          0
#define ST7789_Y_Start          0

// Набор команд
#define ST7789_Cmd_SWRESET      0x01

#define ST7789_Cmd_SLPIN        0x10
#define ST7789_Cmd_SLPOUT       0x11
#define ST7789_Cmd_PTLON        0x12
#define ST7789_Cmd_NORON        0x13
#define ST7789_Cmd_INVOFF       0x20
#define ST7789_Cmd_INVON        0x21
#define ST7789_Cmd_GAMSET       0x26
#define ST7789_Cmd_DISPOFF      0x28
#define ST7789_Cmd_DISPON       0x29
#define ST7789_Cmd_CASET        0x2A
#define ST7789_Cmd_RASET        0x2B
#define ST7789_Cmd_RAMWR        0x2C

#define ST7789_Cmd_PTLAR        0x30
#define ST7789_Cmd_COLMOD       0x3A
#define ST7789_Cmd_MADCTL       0x36    // Memory data access control

#define ST7789_Cmd_MADCTL_MY    0x80
#define ST7789_Cmd_MADCTL_MX    0x40
#define ST7789_Cmd_MADCTL_MV    0x20
#define ST7789_Cmd_MADCTL_ML    0x10
#define ST7789_Cmd_MADCTL_RGB   0x00

#define ST7789_Cmd_RDID1        0xDA
#define ST7789_Cmd_RDID2        0xDB
#define ST7789_Cmd_RDID3        0xDC
#define ST7789_Cmd_RDID4        0xDD
//==============================================================================

#define ST7789_MADCTL_MY        0x80
#define ST7789_MADCTL_MX        0x40
#define ST7789_MADCTL_MV        0x20
#define ST7789_MADCTL_ML        0x10
#define ST7789_MADCTL_BGR       0x08
#define ST7789_MADCTL_MH        0x04

// Процедура отправки команды в дисплей
void ST7789_SendCmd(uint8_t Cmd, LCD_str *lcd);
// Процедура отправки данных (параметров) в дисплей
void ST7789_SendData(uint8_t Data, LCD_str *lcd);
// Процедура аппаратного сброса дисплея (ножкой RESET)
void ST7789_HardReset(LCD_str *lcd);
// Процедура программного сброса дисплея
void ST7789_SoftReset(LCD_str *lcd);
// Процедура включения/отключения режима сна
void ST7789_SleepMode(uint8_t Mode, LCD_str *lcd);
// Процедура включения/отключения режима частичного заполнения экрана
void ST7789_InversionMode(uint8_t Mode, LCD_str *lcd);
// Процедура включения/отключения питания дисплея
void ST7789_DisplayPower(uint8_t On, LCD_str *lcd);
// Процедура выбора кривой гамма-коррекции
void ST7789_GammaSet(uint8_t CurveNum, LCD_str *lcd);
// Процедура настройки формата цвета
void ST7789_ColorModeSet(uint8_t ColorMode, LCD_str *lcd);
// Процедура настройки отображения
void ST7789_MemAccessModeSet(uint8_t Rotation, uint8_t VertMirror,
	uint8_t HorizMirror, uint8_t IsBGR, LCD_str *lcd);
// Процедура управления подсветкой (без поддержки ШИМ)
void st7789_SetBL(uint8_t Value, LCD_str *lcd);
// Процедура установка границ экрана для заполнения
void ST7789_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
	LCD_str *lcd);
// Процедура закрашивает экран цветом color
void ST7789_FillScreen(uint16_t color, LCD_str *lcd);
// Процедура окрашивает 1 пиксель дисплея
void ST7789_DrawPixel(int16_t x, int16_t y, uint16_t color, LCD_str *lcd);
// Процедура заполнения прямоугольника цветом color
void ST7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color,
	LCD_str *lcd);
// Процедура заполнения прямоугольной области из буфера. Порядок заполнения экрана Y - X
void ST7789_DrawPartYX(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t *pBuff, LCD_str *lcd);
// Процедура заполнения прямоугольной области из буфера. Порядок заполнения экрана X - Y
void ST7789_DrawPartXY(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t *pBuff, LCD_str *lcd);
// Процедура заполнения строчки цветом из буфера color
void ST7789_FillLine(int16_t x, uint16_t *color, LCD_str *lcd);
u16 HSV_to_RGB565(u16 hsv_h, u16 hsv_s, u8 hsv_v);
void ST7789_Init(LCD_str *lcd);

#endif /* INC_ST7789_H_ */
