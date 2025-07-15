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


int main() {
    run_hardware_test();
    // stdio_init_all();

    // // I2C Initialisation. Using it at 400Khz.
    // i2c_init(I2C_PORT, 400 * 1000);

    // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);
    // // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // while (true) {
    //     printf("Hello, world!\n");
    //     sleep_ms(1000);
    // }

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
    stdio_init_all();

    printf("Hardware test for SSD1306 driver\n");
    //TODO: reevaluate this init part
    // TODO replace i2c inits with the handler implementation

    // printf("SSD1306 OLED display test\n");
    // bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    // bi_decl(bi_program_description("SSD1306 OLED driver I2C example for the Raspberry Pi Pico"));

    // // I2C is "open drain", pull ups to keep signal high when no data is being sent
    // i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    // gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    // gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    // gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    // gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    //If you want to use the i2c hardware instance 1, call i2c_handler_selectHwInstance first.
    uint32_t status = i2c_handler_initialise(SSD1306_I2C_CLK * 1000);

    printf("initialised i2c, got status code %u\n", status);

    // run through the complete initialization process
    SSD1306_init(SSD1306_I2C_ADDR);

    printf("initialised ssd1306\n");


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
    memset(buf, 0, complete_display_area.buflen);
    SSD1306_render_area(SSD1306_I2C_ADDR, buf, &complete_display_area);

    // intro sequence: flash the screen 3 times
    for (int i = 0; i < 3; i++) {
        SSD1306_send_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ALL_ON);    // Set all pixels on
        sleep_ms(500);
        SSD1306_send_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
        sleep_ms(500);
    }

    // render 3 cute little raspberries
    ssd1306_render_area_t raspberry_area = {
        start_page: 0,
        end_page : (IMG_HEIGHT / SSD1306_PAGE_HEIGHT) - 1
    };

    while (1) {

        printf("Looping over test cases\n");

        raspberry_area.start_col = 0;
        raspberry_area.end_col = IMG_WIDTH - 1;

        printf("Render and scroll images\n");
        SSD1306_get_buflen_from_render_area(&raspberry_area);
        uint8_t offset = 5 + IMG_WIDTH; // 5px padding

        for (int i = 0; i < 3; i++) {
            SSD1306_render_area(SSD1306_I2C_ADDR, raspberry26x32, &raspberry_area);
            raspberry_area.start_col += offset;
            raspberry_area.end_col += offset;
        }
        SSD1306_scroll(SSD1306_I2C_ADDR, true);
        sleep_ms(5000);
        SSD1306_scroll(SSD1306_I2C_ADDR, false);

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
        SSD1306_render_area(SSD1306_I2C_ADDR, buf, &complete_display_area);
        sleep_ms(3000);

        printf("Invert screen\n");
        // Test the display invert function
        SSD1306_send_cmd(SSD1306_I2C_ADDR, SSD1306_SET_INV_DISP);
        sleep_ms(3000);
        SSD1306_send_cmd(SSD1306_I2C_ADDR, SSD1306_SET_NORM_DISP);

        printf("Draw looping lines effect\n");
        //Draw some lines
        bool pix = true;
        for (int i = 0; i < 2;i++) {
            for (int x = 0;x < SSD1306_WIDTH;x++) {
                SSD1306_draw_line(buf, x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
                SSD1306_render_area(SSD1306_I2C_ADDR, buf, &complete_display_area);
            }

            for (int y = SSD1306_HEIGHT - 1; y >= 0;y--) {
                SSD1306_draw_line(buf, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
                SSD1306_render_area(SSD1306_I2C_ADDR, buf, &complete_display_area);
            }
            pix = false;
        }

    }
}
