/**
 * MIT License
 * Copyright (c) [2025] [saphieron]
 */

#ifndef I2C_HANDLER_H
#define I2C_HANDLER_H

#include <stdint.h>

uint32_t i2c_handler_initialise(uint32_t baudrate);
void i2c_handler_disable(void);
int32_t i2c_handler_selectHwInstance(uint8_t device_num);
uint32_t i2c_handler_set_baudrate(uint32_t baudrate);

int32_t i2c_handler_write(uint8_t addr, uint8_t* buffer, uint32_t amount);
int32_t i2c_handler_read(uint8_t addr, uint8_t* buffer, uint32_t amount);

#endif //I2C_HANDLER_H
