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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "ssd1306.h"
#include "ssd1306_font.h"

#ifdef USE_ASCII_FONT
#define FONT_TO_USE ascii_column_font
#pragma message "using ascii font"
#else
#define FONT_TO_USE simple_font
#pragma message "using simple_font"
#endif //USE_ASCII_FONT


 /* Example code to talk to an SSD1306-based OLED display

    The SSD1306 is an OLED/PLED driver chip, capable of driving displays up to
    128x64 pixels.

    NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
    GPIO (and therefore I2C) cannot be used at 5v.

    You will need to use a level shifter on the I2C lines if you want to run the
    board at 5v.

    Connections on Raspberry Pi Pico board, other boards may vary.

    GPIO PICO_DEFAULT_I2C_SDA_PIN (on Pico this is GP4 (pin 6)) -> SDA on display
    board
    GPIO PICO_DEFAULT_I2C_SCL_PIN (on Pico this is GP5 (pin 7)) -> SCL on
    display board
    3.3v (pin 36) -> VCC on display board
    GND (pin 38)  -> GND on display board
 */

void SSD1306_send_raw_cmd(uint8_t dev_addr, uint8_t cmd) {
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command
    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = { 0x80, cmd };
    // i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buf, 2, false);
    i2c_handler_write(dev_addr, buf, 2);
}

void SSD1306_send_raw_cmd_list(uint8_t dev_addr, uint8_t* buf, int num) {
    for (int i = 0;i < num;i++) {
        SSD1306_send_raw_cmd(dev_addr, buf[i]);
    }
}

void SSD1306_send_buf(uint8_t dev_addr, uint8_t buf[], int buflen) {
    // in horizontal addressing mode, the column address pointer auto-increments
    // and then wraps around to the next page, so we can send the entire frame
    // buffer in one gooooooo!

    // copy our frame buffer into a new buffer because we need to add the control byte
    // to the beginning

    uint8_t* temp_buf = malloc(buflen + 1); //TODO: try to do this without malloc.

    temp_buf[0] = 0x40; //TODO: figure out why 0x40, give it a name.
    memcpy(temp_buf + 1, buf, buflen);

    // i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, temp_buf, buflen + 1, false);
    i2c_handler_write(SSD1306_I2C_ADDR, temp_buf, buflen + 1);
    free(temp_buf);
}

#ifndef count_of
#define count_of(a) (sizeof(a)/sizeof((a)[0]))
#endif



// Initialisation of the SSD1306 driver to a known, working state.
// It is posible to initialise the driver manually with \ref SSD1306_send_raw_cmd_list().
// Some of these commands are not strictly necessary as the reset
// process defaults to some of these but they are shown here
// to demonstrate what the initialization sequence looks like
// Some configuration values are recommended by the board manufacturer
int32_t SSD1306_init(uint8_t dev_addr, uint8_t screen_width, uint8_t screen_height) {

    // TODO: give unnamed constants a name.
    // TODO: check if the bit masking is strictly necessary, or it couldn't just be another constant
    uint8_t screen_size_config_value = 0x02;
    if (screen_width != 128) {
        return SSD1306_ERROR_SCREEN_WIDTH_NOT_SUPPORTED;
    }
    if (screen_height == 32) {
        screen_size_config_value = 0x02;
    } else if (screen_height == 64) {
        screen_size_config_value = 0x12;
    } else {
        return SSD1306_ERROR_SCREEN_HEIGHT_NOT_SUPPORTED;
    }

    uint8_t cmds[] = {
        SSD1306_SET_DISP,               // set display off
        /* memory mapping */
        SSD1306_SET_MEM_MODE,           // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
        SSD1306_MEM_MODE_HORIZONTAL,    // horizontal addressing mode
        /* resolution and layout */
        SSD1306_SET_DISP_START_LINE,    // set display start line to 0
        SSD1306_SET_SEG_REMAP | 0x01,   // set segment re-map, column address 127 is mapped to SEG0
        SSD1306_SET_MUX_RATIO,          // set multiplex ratio
        SSD1306_HEIGHT - 1,             // Display height - 1
        SSD1306_SET_COM_OUT_DIR | 0x08, // set COM (common) output scan direction. Scan from bottom up, COM[N-1] to COM0
        SSD1306_SET_DISP_OFFSET,        // set display offset
        SSD1306_NO_DISPLAY_OFFSET,      // no offset
        SSD1306_SET_COM_PIN_CFG,        // set COM (common) pins hardware configuration. Board specific magic number.
        // 0x02 Works for 128x32, 0x12 Possibly works for 128x64. Other options 0x22, 0x32 are untested
        screen_size_config_value,
        /* timing and driving scheme */
        SSD1306_SET_DISP_CLK_DIV,       // set display clock divide ratio
        SSD1306_DISP_CLK_DIV_1,                           // div ratio of 1, standard freq
        SSD1306_SET_PRECHARGE,          // set pre-charge period
        SSD1306_PRECHARGE_VCC_INTERNAL, // Vcc internally generated on our board
        SSD1306_SET_VCOM_DESEL,         // set VCOMH deselect level
        SSD1306_VCOM_DESEL_0_83_VCC,    // 0.83xVcc
        /* display */
        SSD1306_SET_CONTRAST,           // set contrast control
        0xFF,                           // Full contrast
        SSD1306_SET_ENTIRE_ON,          // set entire display on to follow RAM content
        SSD1306_SET_NORM_DISP,          // set normal (not inverted) display
        SSD1306_SET_CHARGE_PUMP,        // set charge pump
        SSD1306_CHARGE_PUMP_VCC_INTERNAL, // Vcc internally generated on our board
        SSD1306_SET_SCROLL | 0x00,      // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
        SSD1306_SET_DISP | 0x01,        // turn display on
    };

    SSD1306_send_raw_cmd_list(dev_addr, cmds, count_of(cmds));
    return 0;
}

void SSD1306_set_scrolling(uint8_t dev_addr, bool on) {
    // configure horizontal scrolling
    //TODO: reevaluate what this is meant to be saying.
    uint8_t cmds[] = {
        SSD1306_SET_HORIZ_SCROLL | 0x00,
        0x00, // dummy byte
        0x00, // start page 0
        0x00, // time interval
        0x03, // end page 3 SSD1306_NUM_PAGES ??
        0x00, // dummy byte
        0xFF, // dummy byte
        SSD1306_SET_SCROLL | (on ? 0x01 : 0) // Start/stop scrolling
    };

    SSD1306_send_raw_cmd_list(dev_addr, cmds, count_of(cmds));
}


void SSD1306_render_area(uint8_t dev_addr, uint8_t* buf, ssd1306_render_area_t* area) {
    // update a portion of the display with a render area
    uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        area->start_col,
        area->end_col,
        SSD1306_SET_PAGE_ADDR,
        area->start_page,
        area->end_page
    };

    SSD1306_send_raw_cmd_list(dev_addr, cmds, count_of(cmds));
    SSD1306_send_buf(dev_addr, buf, area->buflen);
}

void SSD1306_clear_area(uint8_t dev_addr, uint8_t* buf, ssd1306_render_area_t* area) {
    memset(buf, 0, area->buflen);
    SSD1306_render_area(dev_addr, buf, area);
}

//Set the pixel in a given buffer that is then passed on to the display
void SSD1306_set_pixel(uint8_t* buf, int x, int y, bool on) {
    assert(x >= 0 && x < SSD1306_WIDTH && y >= 0 && y < SSD1306_HEIGHT);

    // The calculation to determine the correct bit to set depends on which address
    // mode we are in. This code assumes horizontal
    //TODO: figure out code for the other modes?

    // The video ram on the SSD1306 is split up in to 8 rows, one bit per pixel.
    // Each row is 128 long by 8 pixels high, each byte vertically arranged, so byte 0 is x=0, y=0->7,
    // byte 1 is x = 1, y=0->7 etc

    // This code could be optimised, but is like this for clarity. The compiler
    // should do a half decent job optimising it anyway.
    const int BytesPerRow = SSD1306_WIDTH; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8

    int byte_idx = (y / 8) * BytesPerRow + x;
    uint8_t byte = buf[byte_idx];

    if (on)
        byte |= 1 << (y % 8);
    else
        byte &= ~(1 << (y % 8));

    buf[byte_idx] = byte;
}

// Basic Bresenhams.
void SSD1306_draw_line(uint8_t* buf, int x0, int y0, int x1, int y1, bool on) {

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true) {
        SSD1306_set_pixel(buf, x0, y0, on);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

//TODO: create more letters for lower case
//TODO: also figure out if there's a nicer way to write this
static inline int get_font_index(char ch) {
#ifdef USE_ASCII_FONT
    if(ch > 127){
        return 0;
    }
    return ch;
#else
    ch = toupper(ch);
    if (ch >= 'A' && ch <= 'Z') {
        return  ch - 'A' + 1;
    } else if (ch >= '0' && ch <= '9') {
        return  ch - '0' + 27;
    } else return  0; // Not got that char so space.
#endif //USE_ASCII_FONT
}

void SSD1306_write_char_at(uint8_t* buf, int16_t x, int16_t y, uint8_t ch) {
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
        return;

    // For the moment, only write on Y row boundaries (every 8 vertical pixels)
    y = y / 8;

    // ch = toupper(ch); //TODO: assume also supporting lower case letters
    int idx = get_font_index(ch);
    int fb_idx = y * 128 + x;

    for (int i = 0;i < 8;i++) {
        buf[fb_idx++] = FONT_TO_USE[idx * 8 + i];
    }
}

void SSD1306_write_string_at(uint8_t* buf, int16_t x, int16_t y, char* str) {
    // Cull out any string off the screen
    if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
        return;

    while (*str) {
        SSD1306_write_char_at(buf, x, y, *str++);
        x += 8;
    }
}


void SSD1306_get_buflen_from_render_area(ssd1306_render_area_t* area) {
    // calculate how long the flattened buffer will be for a render area
    area->buflen = (area->end_col - area->start_col + 1) * (area->end_page - area->start_page + 1);
}

