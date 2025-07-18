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


#ifndef SSD1306_FONT_H
#define SSD1306_FONT_H


 // Vertical bitmaps, A-Z, 0-9. Each is 8 pixels high and wide
 // These are defined vertically to make them quick to copy to FB

extern uint8_t simple_font[];
extern uint8_t ascii_column_font[];

#endif //SSD1306_FONT_H
