/**
 * Original sources from:
 * https://github.com/raspberrypi/pico-examples/tree/84e8d489ca321a4be90ee49e36dc29e5c645da08/i2c/ssd1306_i2c
 *
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ****************
 *
 * Modifications:
 * MIT License
 * Copyright (c) [2025] [saphieron]
 */

#ifndef SSD1306_H
#define SSD1306_H

#include "i2c_handler.h"

 // Define the size of the display we have attached. This can vary, make sure you
 // have the right size defined or the output will look rather odd!
 // Code has been tested on 128x32 and 128x64 OLED displays

#define SSD1306_HEIGHT              32
#define SSD1306_WIDTH               128

#define SSD1306_I2C_ADDR            0x3C

// 400 is usual, but often these can be overclocked to improve display response.
// Tested at 1000 on both 32 and 84 pixel height devices and it worked.
#define SSD1306_I2C_CLK             400
//#define SSD1306_I2C_CLK             1000


// commands (see datasheet)
#define SSD1306_SET_MEM_MODE        0x20
#define SSD1306_SET_COL_ADDR        0x21
#define SSD1306_SET_PAGE_ADDR       0x22
#define SSD1306_SET_HORIZ_SCROLL    0x26
#define SSD1306_SET_SCROLL          0x2E

#define SSD1306_SET_DISP_START_LINE 0x40

#define SSD1306_SET_CONTRAST        0x81
#define SSD1306_SET_CHARGE_PUMP     0x8D

#define SSD1306_SET_SEG_REMAP       0xA0
#define SSD1306_SET_ENTIRE_ON       0xA4
#define SSD1306_SET_ALL_ON          0xA5
#define SSD1306_SET_NORM_DISP       0xA6
#define SSD1306_SET_INV_DISP        0xA7
#define SSD1306_SET_MUX_RATIO       0xA8
#define SSD1306_SET_DISP            0xAE
#define SSD1306_SET_COM_OUT_DIR     0xC0
#define SSD1306_SET_COM_OUT_DIR_FLIP 0xC0

#define SSD1306_SET_DISP_OFFSET     0xD3
#define SSD1306_SET_DISP_CLK_DIV    0xD5
#define SSD1306_SET_PRECHARGE       0xD9
#define SSD1306_SET_COM_PIN_CFG     0xDA
#define SSD1306_SET_VCOM_DESEL      0xDB

#define SSD1306_PAGE_HEIGHT         8
#define SSD1306_NUM_PAGES           (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN             (SSD1306_NUM_PAGES * SSD1306_WIDTH)

#define SSD1306_WRITE_MODE         0xFE
#define SSD1306_READ_MODE          0xFF

// command config values
#define SSD1306_MEM_MODE_HORIZONTAL        0x00 //others exist
#define SSD1306_NO_DISPLAY_OFFSET 0x00
#define SSD1306_DISP_CLK_DIV_1 0x80
#define SSD1306_PRECHARGE_VCC_INTERNAL 0xF1
#define SSD1306_VCOM_DESEL_0_83_VCC 0x30
#define SSD1306_CHARGE_PUMP_VCC_INTERNAL 0x14

//Errors
#define SSD1306_ERROR_SCREEN_WIDTH_NOT_SUPPORTED -1
#define SSD1306_ERROR_SCREEN_HEIGHT_NOT_SUPPORTED -2


typedef struct render_area {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;

    int buflen;
} ssd1306_render_area_t;


int32_t SSD1306_init(uint8_t dev_addr, uint8_t screen_width, uint8_t screen_height);
void SSD1306_send_raw_cmd(uint8_t dev_addr, uint8_t cmd);
void SSD1306_send_raw_cmd_list(uint8_t dev_addr, uint8_t* buf, int num);
void SSD1306_set_scrolling(uint8_t dev_addr, bool on);
void SSD1306_render_area(uint8_t dev_addr, uint8_t* buf, ssd1306_render_area_t* area);
void SSD1306_set_pixel(uint8_t* buf, int x, int y, bool on);
void SSD1306_draw_line(uint8_t* buf, int x0, int y0, int x1, int y1, bool on);
void SSD1306_write_char_at(uint8_t* buf, int16_t x, int16_t y, uint8_t ch);
void SSD1306_write_string_at(uint8_t* buf, int16_t x, int16_t y, char* str);
void SSD1306_get_buflen_from_render_area(ssd1306_render_area_t* area);

#endif /* SSD1306_H */
