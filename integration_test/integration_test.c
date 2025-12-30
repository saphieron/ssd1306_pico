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
void run_font_test();

void test_render_image(ssd1306_device_t* device, uint8_t* image, uint8_t imgSizeX, uint8_t imgSizeY);
void test_write_string(ssd1306_device_t* device);
void test_inversion(ssd1306_device_t* device);
void test_draw_lines(ssd1306_device_t* device);
void render_page_through_string(ssd1306_device_t* device, char* text);

int main() {
    stdio_init_all();
    sleep_ms(5000);

    printf("Hardware test for SSD1306 driver\n");

    //If you want to use the i2c hardware instance 1, call i2c_handler_selectHwInstance first.
    uint32_t status = i2c_handler_initialise(SSD1306_I2C_CLK * 1000);

    printf("initialised i2c, got status code %u\n", status);

    ssd1306_device_t device;
    uint8_t screenBuffer[SSD1306_deriveBufferSizeFromScreenSize(128, 32)];
    memset(screenBuffer, 0xBF, SSD1306_deriveBufferSizeFromScreenSize(128, 32));

    // run through the complete initialization process
    SSD1306_init(&device, SSD1306_I2C_ADDR, 128, 32, screenBuffer);
    if (device.lastError != SSD1306_ERROR_NO_ERROR) {
        while (1) {
            printf("Failed to initialise ssd1306 library. Error code %d\n", device.lastError);
            sleep_ms(1000);
        }
    }
    printf("initialised ssd1306\n");

    while (1) {
        run_hardware_test(&device);
        run_string_test(&device);
        run_font_test(&device);
    }
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


int run_hardware_test(ssd1306_device_t* device) {
    SSD1306_clearArea(device);

    SSD1306_sendRawCommand(device, SSD1306_SET_ALL_ON);    // Set all pixels on
    sleep_ms(500);
    SSD1306_sendRawCommand(device, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    sleep_ms(500);

    // render 3 cute little raspberries
    ssd1306_render_area_t raspberry_area = {
        start_page: 0,
        end_page : IMG_HEIGHT - 1,
        start_col : 0,
        end_col : IMG_WIDTH - 1
    };

    // Render image in a specific section.
    ssd1306_render_area_t originalRenderArea = device->renderArea;
    device->renderArea = raspberry_area;
    test_render_image(device, raspberry26x32, IMG_WIDTH, IMG_HEIGHT);
    device->renderArea = originalRenderArea;
    SSD1306_clearArea(device);

    test_write_string(device);
    test_inversion(device);
    test_draw_lines(device);
}

void test_render_image(ssd1306_device_t* device, uint8_t* image, uint8_t imgSizeX, uint8_t imgSizeY) {
    size_t imageDataLenght = imgSizeX * (imgSizeY / 8);
    uint8_t offset = 5 + imgSizeX; // 5px padding

    for (int i = 0; i < 3; i++) {
        SSD1306_renderArea(device, image, imageDataLenght);
        device->renderArea.start_col += offset;
        device->renderArea.end_col += offset;
    }
    SSD1306_setScrolling(device, true);
    sleep_ms(5000);
    SSD1306_setScrolling(device, false);

}

void test_write_string(ssd1306_device_t* device) {
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
        SSD1306_writeStringAt(device, 5, y, text[i]);
        y += 8;
    }
    SSD1306_renderFullArea(device);
    sleep_ms(3000);
}

void test_draw_lines(ssd1306_device_t* device) {
    printf("Draw looping lines effect\n");
    //Draw some lines
    bool pix = true;
    for (int i = 0; i < 2;i++) {
        for (int x = 0;x < SSD1306_WIDTH;x++) {
            SSD1306_drawLine(device, x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
            SSD1306_renderFullArea(device);
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0;y--) {
            SSD1306_drawLine(device, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
            SSD1306_renderFullArea(device);
        }
        pix = false;
    }
}

void test_inversion(ssd1306_device_t* device) {
    printf("Invert screen\n");
    // Test the display invert function
    SSD1306_sendRawCommand(device, SSD1306_SET_INV_DISP);
    sleep_ms(3000);
    SSD1306_sendRawCommand(device, SSD1306_SET_NORM_DISP);
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

void run_string_test(ssd1306_device_t* device) {

    printf("Test Render a long thring by cycling through it\n");
    SSD1306_clearArea(device);

    // SSD1306_send_raw_cmd(SSD1306_I2C_ADDR, SSD1306_SET_ALL_ON);    // Set all pixels on
    // sleep_ms(500);
    SSD1306_sendRawCommand(device, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    sleep_ms(500);

    render_page_through_string(device, test_text);
    printf("Test Render a long thring by cycling through it, End\n");
}

void render_page_through_string(ssd1306_device_t* device, char* text) {
    char screen_content[4][17];
    memset(screen_content, 0x00, sizeof(screen_content));

    uint8_t screen_line = 0;

    uint16_t pos_in_text = 0;
    uint16_t line_start_point = 0;
    uint16_t length_to_copy;
    bool partiallyFullScreenContent = false;
    bool copiedFullIgnoreLinebreak = false;
    while (text[pos_in_text] != 0) {
        // char currentChar = text[pos_in_text];
        length_to_copy = pos_in_text - line_start_point + 1;
        // printf("%c/%u\n", currentChar, length_to_copy);
        if (text[pos_in_text] == '\n') {
            // TODO: the newline is copied over, not processed properly, and also not ignored

            if (length_to_copy == 1 && copiedFullIgnoreLinebreak) {
                copiedFullIgnoreLinebreak = false;
                pos_in_text++;
                line_start_point = pos_in_text;
                continue;
            }
            strncpy(screen_content[screen_line], text + line_start_point, length_to_copy - 1);
            line_start_point += length_to_copy;

            memset(screen_content[screen_line] + length_to_copy - 1, ' ', 17 - length_to_copy); // 0 char also looks empty, i.e. like space
            screen_content[screen_line][16] = '\0';
            ++screen_line;
            length_to_copy = 0;
            partiallyFullScreenContent = true;
        }

        if (length_to_copy == 16) {
            strncpy(screen_content[screen_line], text + line_start_point, 16);
            line_start_point += 16;
            screen_content[screen_line][16] = '\0';
            ++screen_line;
            copiedFullIgnoreLinebreak = true;
            length_to_copy = 0;
            partiallyFullScreenContent = true;
        }

        if (screen_line >= 4) {
            printf("rendering text:\n1'%s'\n2'%s'\n3'%s'\n4'%s'\n", screen_content[0], screen_content[1], screen_content[2], screen_content[3]);
            uint8_t y = 0;
            for (size_t i = 0; i < 4; i++) {
                SSD1306_writeStringAt(device, 0, y, screen_content[i]);
                y += 8;
            }
            SSD1306_renderFullArea(device);
            sleep_ms(5000);
            screen_line = 0;
            memset(screen_content, 0, sizeof(screen_content));
            partiallyFullScreenContent = false;
        }
        ++pos_in_text;
    }
    if (partiallyFullScreenContent) {
        SSD1306_clearArea(device);
        strncpy(screen_content[screen_line], text + line_start_point, 16);
        printf("final rendering text:\n1'%s'\n2'%s'\n3'%s'\n4'%s'\n", screen_content[0], screen_content[1], screen_content[2], screen_content[3]);
        uint8_t y = 0;
        for (size_t i = 0; i < screen_line; i++) {
            SSD1306_writeStringAt(device, 0, y, screen_content[i]);
            y += 8;
        }
        SSD1306_renderFullArea(device);
    } else {
        printf("No rest to render: pos_in_text %u, length_to_copy %u, screen_line %u\n", pos_in_text, length_to_copy, screen_line);
    }
    sleep_ms(5000);

    SSD1306_clearArea(device);
}

void run_font_test(ssd1306_device_t* device) {
    char dummyText[97];
    memset(dummyText, 0, 97);
    for (size_t i = 32; i < 128; i++) {
        dummyText[i - 32] = ((char)i);
    }

    SSD1306_clearArea(device);

    SSD1306_sendRawCommand(device, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    sleep_ms(500);

    printf("render font test '%s'\n", dummyText);
    SSD1306_clearArea(device);

    SSD1306_sendRawCommand(device, SSD1306_SET_ALL_ON);    // Set all pixels on
    sleep_ms(500);
    SSD1306_sendRawCommand(device, SSD1306_SET_ENTIRE_ON); // go back to following RAM for pixel state
    sleep_ms(500);

    render_page_through_string(device, dummyText);

    printf("\nrender font test end\n");
}
