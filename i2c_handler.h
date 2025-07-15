/**
 * MIT License
 * Copyright (c) [2025] [saphieron]
 */

#ifndef I2C_HANDLER_H
#define I2C_HANDLER_H

#include <stdint.h>

int32_t i2c_handler_write(uint8_t addr, uint8_t* buffer, uint32_t amount);

#endif //I2C_HANDLER_H
