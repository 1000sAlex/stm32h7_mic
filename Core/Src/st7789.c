/*
 * st7789.c
 *
 *  Created on: Sep 26, 2020
 *      Author: u
 */

#include "st7789.h"
#include "gpio.h"

//==============================================================================
// Процедура отправки команды в дисплей
//==============================================================================

//extern SPI_HandleTypeDef hspi4;

void ST7789_SendCmd(uint8_t Cmd, LCD_str *lcd)
    {
    lcd->LCD_dc_port->ODR &= ~lcd->LCD_dc_pin;
//    while (LL_SPI_IsActiveFlag_TXC(SPI4) == 0)
//	;
//    LL_SPI_TransmitData8(SPI4, Cmd);
//    while (__HAL_SPI_GET_FLAG(lcd->spi_str, SPI_FLAG_TXP) == 0)
//	;
    while (HAL_SPI_GetState(lcd->spi_str) != HAL_SPI_STATE_READY)
	;
    HAL_SPI_Transmit(lcd->spi_str, &Cmd, 1, 0xFFFF);
    }
//==============================================================================

//==============================================================================
// Процедура отправки данных (параметров) в дисплей
//==============================================================================
void ST7789_SendData(uint8_t Data, LCD_str *lcd)
    {
    lcd->LCD_dc_port->ODR |= lcd->LCD_dc_pin;

//    while (LL_SPI_IsActiveFlag_TXC(SPI4) == 0)
//	;
//    LL_SPI_TransmitData8(SPI4, Data);
//    while (__HAL_SPI_GET_FLAG(lcd->spi_str, SPI_FLAG_TXP) == 0)
//	;
    while (HAL_SPI_GetState(lcd->spi_str) != HAL_SPI_STATE_READY)
	;
    HAL_SPI_Transmit(lcd->spi_str, &Data, 1, 0xFFFF);

//SPI_send8b(ST7789_SPI_periph, &Data, 1);
    }
//==============================================================================

//==============================================================================
// Процедура программного сброса дисплея
//==============================================================================
void ST7789_SoftReset(LCD_str *lcd)
    {
    ST7789_SendCmd(ST7789_Cmd_SWRESET, lcd);
    HAL_Delay(130);
    }
//==============================================================================

//==============================================================================
// Процедура аппаратного сброса дисплея (ножкой RESET)
//==============================================================================
void ST7789_HardReset(LCD_str *lcd)
    {
    lcd->LCD_res_port->ODR &= ~lcd->LCD_res_pin;
    HAL_Delay(50);
    lcd->LCD_res_port->ODR |= lcd->LCD_res_pin;
    HAL_Delay(100);
    }
//==============================================================================

//==============================================================================
// Процедура включения/отключения режима сна
//==============================================================================
void ST7789_SleepMode(uint8_t Mode, LCD_str *lcd)
    {
    if (Mode)
	ST7789_SendCmd(ST7789_Cmd_SLPIN, lcd);
    else
	ST7789_SendCmd(ST7789_Cmd_SLPOUT, lcd);
    HAL_Delay(50);
    }
//==============================================================================

//==============================================================================
// Процедура включения/отключения режима частичного заполнения экрана
//==============================================================================
void ST7789_InversionMode(uint8_t Mode, LCD_str *lcd)
    {
    if (Mode)
	ST7789_SendCmd(ST7789_Cmd_INVON, lcd);
    else
	ST7789_SendCmd(ST7789_Cmd_INVOFF, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура включения/отключения питания дисплея
//==============================================================================
void ST7789_DisplayPower(uint8_t On, LCD_str *lcd)
    {
    if (On)
	ST7789_SendCmd(ST7789_Cmd_DISPON, lcd);
    else
	ST7789_SendCmd(ST7789_Cmd_DISPOFF, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура установки начального и конечного адресов колонок
//==============================================================================
static void ST7789_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd,
	LCD_str *lcd)
    {
    if (ColumnStart > ColumnEnd)
	return;
    if (ColumnEnd > ST7789_Height)
	return;
    ST7789_SendCmd(ST7789_Cmd_CASET, lcd);
    ST7789_SendData(ColumnStart >> 8, lcd);
    ST7789_SendData(ColumnStart & 0xFF, lcd);
    ST7789_SendData(ColumnEnd >> 8, lcd);
    ST7789_SendData(ColumnEnd & 0xFF, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура установки начального и конечного адресов строк
//==============================================================================
static void ST7789_RowSet(uint16_t RowStart, uint16_t RowEnd, LCD_str *lcd)
    {
    if (RowStart > RowEnd)
	return;
    if (RowEnd > ST7789_Width)
	return;
    ST7789_SendCmd(ST7789_Cmd_RASET, lcd);
    ST7789_SendData(RowStart >> 8, lcd);
    ST7789_SendData(RowStart & 0xFF, lcd);
    ST7789_SendData(RowEnd >> 8, lcd);
    ST7789_SendData(RowEnd & 0xFF, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура установка границ экрана для заполнения
//==============================================================================
void ST7789_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
	LCD_str *lcd)
    {
    ST7789_ColumnSet(x0, x1, lcd);
    ST7789_RowSet(y0, y1, lcd);

    ST7789_SendCmd(ST7789_Cmd_RAMWR, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура записи данных в дисплей
//==============================================================================
void ST7789_RamWrite(uint16_t *pBuff, uint16_t Len, LCD_str *lcd)
    {
    while (__HAL_SPI_GET_FLAG(lcd->spi_str, SPI_FLAG_TXP) == 0)
	;
    lcd->LCD_dc_port->ODR |= lcd->LCD_dc_pin;
    //HAL_SPI_Transmit(lcd->spi_str, ((u8*) pBuff + 1), Len, 0xFFFF);
    HAL_SPI_Transmit(lcd->spi_str, ((u8*) pBuff), Len * 2, 0xFFFF);
    }

//==============================================================================

//==============================================================================
// Процедура выбора кривой гамма-коррекции
//==============================================================================
void ST7789_GammaSet(uint8_t CurveNum, LCD_str *lcd)
    {
    if (CurveNum > 4)
	return;

    ST7789_SendCmd(ST7789_Cmd_GAMSET, lcd);
    ST7789_SendData(1 << (CurveNum - 1), lcd);
    }
//==============================================================================

//==============================================================================
// Процедура настройки формата цвета
//==============================================================================
void ST7789_ColorModeSet(uint8_t ColorMode, LCD_str *lcd)
    {
    ST7789_SendCmd(ST7789_Cmd_COLMOD, lcd);
    ST7789_SendData(ColorMode & 0x77, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура настройки отображения
//==============================================================================
void ST7789_MemAccessModeSet(uint8_t Rotation, uint8_t VertMirror,
	uint8_t HorizMirror, uint8_t IsBGR, LCD_str *lcd)
    {
    uint8_t Value;
    Rotation &= 7;

    ST7789_SendCmd(ST7789_Cmd_MADCTL, lcd);

// Настройка направления заполнения экрана
    switch (Rotation)
	{
    case 0:
	Value = 0;
	break;
    case 1:
	Value = ST7789_MADCTL_MX;
	break;
    case 2:
	Value = ST7789_MADCTL_MY;
	break;
    case 3:
	Value = ST7789_MADCTL_MX | ST7789_MADCTL_MY;
	break;
    case 4:
	Value = ST7789_MADCTL_MV;
	break;
    case 5:
	Value = ST7789_MADCTL_MV | ST7789_MADCTL_MX;
	break;
    case 6:
	Value = ST7789_MADCTL_MV | ST7789_MADCTL_MY;
	break;
    case 7:
	Value = ST7789_MADCTL_MV | ST7789_MADCTL_MX | ST7789_MADCTL_MY;
	break;
    default:
	break;
	}

    if (VertMirror)
	Value = ST7789_MADCTL_ML;
    if (HorizMirror)
	Value = ST7789_MADCTL_MH;

// Использование порядка цветов BGR вместо RGB
    if (IsBGR)
	Value |= ST7789_MADCTL_BGR;

    ST7789_SendData(Value, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура окрашивает 1 пиксель дисплея
//==============================================================================
void ST7789_DrawPixel(int16_t x, int16_t y, uint16_t color, LCD_str *lcd)
    {
    if ((x < 0) || (x >= ST7789_Width) || (y < 0) || (y >= ST7789_Height))
	return;

    ST7789_SetWindow(x, y, x, y, lcd);
    ST7789_RamWrite(&color, 1, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура заполнения прямоугольника цветом color
//==============================================================================
void ST7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color,
	LCD_str *lcd)
    {
    if ((x >= 240) || (y >= 240))
	return;

    if ((x + w) > 240)
	w = ST7789_Width - x;

    if ((y + h) > 240)
	h = ST7789_Height - y;

    ST7789_SetWindow(x, y, x + w - 1, y + h - 1, lcd);
    for (uint32_t i = 0; i < (h * w); i++)
	ST7789_RamWrite(&color, 1, lcd);
    }

//==============================================================================
// Процедура заполнения строчки цветом из буфера color
//==============================================================================
void ST7789_FillLine(int16_t x, uint16_t *color, LCD_str *lcd)
    {
    while (HAL_SPI_GetState(lcd->spi_str) != HAL_SPI_STATE_READY)
	;
    ST7789_SetWindow(x, 0, x, ST7789_Height, lcd);
    //ST7789_RamWrite(color, ST7789_Height, lcd);
    lcd->LCD_dc_port->ODR |= lcd->LCD_dc_pin;
    HAL_SPI_Transmit_DMA(lcd->spi_str, (u8*) color, ST7789_Height * 2);
    }
//==============================================================================

//==============================================================================
// Процедура заполнения прямоугольной области из буфера. Порядок заполнения экрана Y - X
//==============================================================================
void ST7789_DrawPartYX(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t *pBuff, LCD_str *lcd)
    {
    if ((x >= ST7789_Width) || (y >= ST7789_Height))
	return;

    if ((x + w - 1) >= ST7789_Width)
	w = ST7789_Width - x;

    if ((y + h - 1) >= ST7789_Height)
	h = ST7789_Height - y;

    ST7789_SetWindow(x, y, x + w - 1, y + h - 1, lcd);

    for (uint32_t i = 0; i < (h * w); i++)
	ST7789_RamWrite(pBuff++, 1, lcd);
    }
//==============================================================================

//==============================================================================
// Процедура заполнения прямоугольной области из буфера. Порядок заполнения экрана X - Y
//==============================================================================
void ST7789_DrawPartXY(int16_t x, int16_t y, int16_t w, int16_t h,
	uint16_t *pBuff, LCD_str *lcd)
    {
    if ((x >= ST7789_Width) || (y >= ST7789_Height))
	return;

    if ((x + w - 1) >= ST7789_Width)
	w = ST7789_Width - x;

    if ((y + h - 1) >= ST7789_Height)
	h = ST7789_Height - y;

    for (uint16_t iy = y; iy < y + h; iy++)
	{
	ST7789_SetWindow(x, iy, x + w - 1, iy + 1, lcd);
	for (x = w; x > 0; x--)
	    ST7789_RamWrite(pBuff++, 1, lcd);
	}
    }
//==============================================================================

//==============================================================================
// Процедура закрашивает экран цветом color
//==============================================================================
void ST7789_FillScreen(uint16_t color, LCD_str *lcd)
    {
    ST7789_FillRect(0, 0, 240, 240, color, lcd);
    }
//==============================================================================

u16 HSV_to_RGB565(u16 hsv_h, u16 hsv_s, u8 hsv_v)
    {
    u8 r, g, b;
    // @formatter:off
    if (hsv_s > 0xFF){hsv_s = 0xFF;}
    int hue = HUE(hsv_h);

    int sector = hue / 60;
    int angle = sector & 1 ? 60 - hue % 60 : hue % 60;

    int high = hsv_v;
    int low = (255 - hsv_s) * high / 255;
    int middle = low + (high - low) * angle / 60;

    switch (sector)
	{
    case 0: r = high;g = middle;b = low;break;// red -> yellow
    case 1:r = middle;g = high;b = low;break; // yellow -> green
    case 2: r = low;g = high;b = middle;break;// green -> cyan
    case 3: r = low;g = middle;b = high;break;// cyan -> blue
    case 4: r = middle;g = low;b = high;break;// blue -> magenta
    case 5: r = high;g = low;b = middle;break;// magenta -> red
    default:
	break;
	}
    // @formatter:on
    return RGB565(r, g, b);
    }
//==============================================================================
// Процедура инициализации дисплея
//==============================================================================
void ST7789_Init(LCD_str *lcd)
    {
    // Сброс дисплея
    ST7789_HardReset(lcd);
    // Отправляем последовательность инициализирующих команд
    ST7789_SleepMode(0, lcd);
    //delay_ms(30);
    ST7789_ColorModeSet(ST7789_ColorMode_65K | ST7789_ColorMode_16bit, lcd);
    ST7789_MemAccessModeSet(4, 0, 0, 0, lcd);
    ST7789_InversionMode(1, lcd);
    ST7789_DisplayPower(1, lcd);
    HAL_Delay(5);
    ST7789_FillScreen(RGB565(0, 0, 0), lcd);
    ST7789_GammaSet(1, lcd);
    lcd->LCD_blk_port->ODR |= lcd->LCD_blk_pin;
    }
//==============================================================================
