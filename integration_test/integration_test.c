#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <i2c_test_wrapper.h>
#include <ssd1306.h>



// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9


int run_hardware_test();
void run_string_test();

void test_render_image(uint8_t* buf, ssd1306_render_area_t* area, ssd1306_render_area_t* image);
void test_write_string(uint8_t* buf, ssd1306_render_area_t* area);
void test_inversion();
void test_draw_lines(uint8_t* buf, ssd1306_render_area_t* complete_display_area);


int main() {
    stdio_init_all();

    printf("Hardware test for SSD1306 driver\n");

    //If you want to use the i2c hardware instance 1, call i2c_handler_selectHwInstance first.
    uint32_t status = i2c_handler_initialise(SSD1306_I2C_CLK * 1000);

    printf("initialised i2c, got status code %u\n", status);

    // run through the complete initialization process
    SSD1306_init(SSD1306_I2C_ADDR, 128, 32);
    printf("initialised ssd1306\n");

    // run_hardware_test();
    run_string_test();
    return 0;
}




#define IMG_WIDTH 26
#define IMG_HEIGHT 32

static uint8_t raspberry26x32[] = { 0x0, 0x0, 0xe, 0x7e, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xfe, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0x7e, 0x1e, 0x0, 0x0, 0x0, 0x80, 0xe0, 0xf8, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xf8, 0xe0, 0x80, 0x0, 0x0, 0x1e, 0x7f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x7f, 0x1e, 0x0, 0x0, 0x0, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff,
    0xff, 0x7f, 0x7f, 0x3f, 0x3f, 0x1f, 0x1f, 0xf, 0x7, 0x3, 0x0, 0x0 };


int run_hardware_test() {
    // Initialize render area for entire frame (SSD1306_WIDTH pixels by SSD1306_NUM_PAGES pages)
    ssd1306_render_area_t complete_display_area = {
        start_col: 0,
        end_col : SSD1306_WIDTH - 1,
        start_page : 0,
        end_page : SSD1306_NUM_PAGES - 1
    };

    SSD1306_get_buflen_from_render_area(&complete_display_area);

    //TODO: if this doesnt work with the buflen, go back to the hardcoded max buffer size
    // zero the entire display
    uint8_t buf[complete_display_area.buflen];
    SSD1306_clear_area(SSD1306_I2C_ADDR, buf, &complete_display_area);

    SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ALL_ON);    // Set all pixels on
    sleep_ms(500);
    SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    sleep_ms(500);

    // render 3 cute little raspberries
    ssd1306_render_area_t raspberry_area = {
        start_page: 0,
        end_page : (IMG_HEIGHT / SSD1306_PAGE_HEIGHT) - 1
    };

    while (1) {

        printf("Looping over test cases\n");

        test_render_image(buf, &complete_display_area, &raspberry_area);
        test_write_string(buf, &complete_display_area);
        test_inversion();
        test_draw_lines(buf, &complete_display_area);
    }
}

void test_render_image(uint8_t* buf, ssd1306_render_area_t* area, ssd1306_render_area_t* image) {
    image->start_col = 0;
    image->end_col = IMG_WIDTH - 1;

    printf("Render and scroll images\n");
    SSD1306_get_buflen_from_render_area(image);
    uint8_t offset = 5 + IMG_WIDTH; // 5px padding

    for (int i = 0; i < 3; i++) {
        SSD1306_render_area(SSD1306_I2C_ADDR, raspberry26x32, image);
        image->start_col += offset;
        image->end_col += offset;
    }
    SSD1306_set_scrolling(SSD1306_I2C_ADDR, true);
    sleep_ms(5000);
    SSD1306_set_scrolling(SSD1306_I2C_ADDR, false);

}

void test_write_string(uint8_t* buf, ssd1306_render_area_t* area) {
    printf("Write string\n");
    // Now show off some text
    char* text[] = {
        "A long time ago",
        "  on an OLED ",
        "   display",
        " far far away",
        "Lived a small",
        "red raspberry",
        "by the name of",
        "    PICO"
    };
    int y = 0;
    for (uint i = 0;i < count_of(text); i++) {
        SSD1306_write_string_at(buf, 5, y, text[i]);
        y += 8;
    }
    SSD1306_render_area(SSD1306_I2C_ADDR, buf, area);
    sleep_ms(3000);
}

void test_draw_lines(uint8_t* buf, ssd1306_render_area_t* area) {
    printf("Draw looping lines effect\n");
    //Draw some lines
    bool pix = true;
    for (int i = 0; i < 2;i++) {
        for (int x = 0;x < SSD1306_WIDTH;x++) {
            SSD1306_draw_line(buf, x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
            SSD1306_render_area(SSD1306_I2C_ADDR, buf, area);
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0;y--) {
            SSD1306_draw_line(buf, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
            SSD1306_render_area(SSD1306_I2C_ADDR, buf, area);
        }
        pix = false;
    }

}

void test_inversion() {
    printf("Invert screen\n");
    // Test the display invert function
    SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_INV_DISP);
    sleep_ms(3000);
    SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_NORM_DISP);

}





char* test_text = "A SUNSET BLOOM\n\
\n\
Then we sat on the sand for some time and observed\n\
How the oceans that cover the world were perturbed\n\
By the tides from the orbiting moon overhead\n\
\"How relaxing the sound of the waves is,\" you said.\n\
\n\
I began to expound upon tidal effects\n\
When you asked me to stop, looking somewhat perplexed\n\
So I did not explain why the sunset turns red\n\
And we watched the occurrence in silence instead.";

void run_string_test() {
    ssd1306_render_area_t complete_display_area = {
        start_col: 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
    };

    SSD1306_get_buflen_from_render_area(&complete_display_area);

    //TODO: if this doesnt work with the buflen, go back to the hardcoded max buffer size
    // zero the entire display
    uint8_t buf[complete_display_area.buflen];
    SSD1306_clear_area(SSD1306_I2C_ADDR, buf, &complete_display_area);

    SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ALL_ON);    // Set all pixels on
    sleep_ms(500);
    SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    sleep_ms(500);

    while (1) {
        char screen_content[4][17];
        memset(screen_content, 0, sizeof(screen_content));

        uint8_t screen_line = 0;

        uint16_t pos_in_text = 0;
        uint16_t line_start_point = 0;
        uint16_t length_to_copy;
        bool copiedFullIgnoreLinebreak = false;
        while (test_text[pos_in_text] != 0) {
            length_to_copy = pos_in_text - line_start_point + 1;
            if (test_text[pos_in_text] == '\n') {
                // TODO: the newline is copied over, not processed properly, and also not ignored

                if (length_to_copy == 1 && copiedFullIgnoreLinebreak) {
                    copiedFullIgnoreLinebreak = false;
                    pos_in_text++;
                    line_start_point = pos_in_text;
                    continue;
                }
                strncpy(screen_content[screen_line], test_text + line_start_point, length_to_copy - 1);
                line_start_point += length_to_copy;

                memset(screen_content[screen_line] + length_to_copy - 1, '\0', 17 - length_to_copy + 1);
                ++screen_line;
                ++pos_in_text;
                length_to_copy = 0;
                continue;
            }

            if (length_to_copy == 16) {
                strncpy(screen_content[screen_line], test_text + line_start_point, 16);
                line_start_point += 16;
                screen_content[screen_line][16] = '\0';
                ++screen_line;
                copiedFullIgnoreLinebreak = true;
                length_to_copy = 0;
            }

            if (screen_line >= 4) {
                printf("rendering text:\n1'%s'\n2'%s'\n3'%s'\n4'%s'\n", screen_content[0], screen_content[1], screen_content[2], screen_content[3]);
                uint8_t y = 0;
                for (size_t i = 0; i < 4; i++) {
                    SSD1306_write_string_at(buf, 0, y, screen_content[i]);
                    y += 8;
                }
                SSD1306_render_area(SSD1306_I2C_ADDR, buf, &complete_display_area);
                sleep_ms(5000);
                screen_line = 0;
            }
            ++pos_in_text;
        }
        if (length_to_copy != 0) {
            strncpy(screen_content[screen_line], test_text + line_start_point, 16);
            printf("rendering text:\n1'%s'\n2'%s'\n3'%s'\n4'%s'\n", screen_content[0], screen_content[1], screen_content[2], screen_content[3]);
        }

        SSD1306_clear_area(SSD1306_I2C_ADDR, buf, &complete_display_area);
        sleep_ms(2000);

    }
}
