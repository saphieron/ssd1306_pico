# ssd1306_pico

An attempt to create a small library out of the Raspberry Pi Pico SDK's [example demo](https://github.com/raspberrypi/pico-examples/tree/84e8d489ca321a4be90ee49e36dc29e5c645da08/i2c/ssd1306_i2c) for controlling SSD1306 based OLED displays.

It is meant to work against an I2C wrapper to be independent of a hardware platform's concrete implementation.

## Usage

To use this library, you only need to incorporate the ssd1306_* files and provide an implementation to the i2c_handler.h functions. Any necessary initialisation for your I2C driver has to be realised by your code.

## Example use on Pi Pico
An example of using the driver is implemented as an integration "test", or rather demo, for the Raspberry Pi Pico 2W. To compile it follow the [Pi Pico SDK Getting started guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) and build the files under `integration_test/`.