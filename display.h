/*
 * display.h
 *
 *  Created on: 27 de Nov de 2016
 *      Author: Saturnino Mateus
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define OLED_DISPLAY_WIDTH  96
#define OLED_DISPLAY_HEIGHT 64

typedef enum
{
    DISPLAY_COLOR_BLACK,
    DISPLAY_COLOR_WHITE,
} display_color_t;


void oled_init (void);
void display_putPixel(uint8_t x, uint8_t y, display_color_t color);
void display_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, display_color_t color);
void display_circle(uint8_t x0, uint8_t y0, uint8_t r, display_color_t color);
void display_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, display_color_t color);
void display_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, display_color_t color);
void display_clearScreen(display_color_t color);
void display_putString(uint8_t x, uint8_t y, uint8_t *pStr, display_color_t fb,
        display_color_t bg);
uint8_t display_putChar(uint8_t x, uint8_t y, uint8_t ch, display_color_t fb, display_color_t bg);
void display_new(void);
void display_write(uint32_t lReceivedValue);

#endif /* DISPLAY_H_ */
