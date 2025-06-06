#pragma once

#include <driver/i2c.h>
#include "esp_err.h"
#include "esp_log.h"
#ifdef __cplusplus
extern "C" {
#endif

#define I2C_CLK 400000

/* Sensor device address */
#define SENSOR_ADDRESS  0b1001000

typedef enum
{
    STDS75_TEMP = 0x00,
    STDS75_CONF = 0x01,
    STDS75_OS = 0x02,
    STDS75_HYS = 0x03
} stds75_register_pointer_t;

esp_err_t STDS75_i2c_init(void);

float STDS75_get_temperature(void);

esp_err_t STDS75_init(i2c_port_t port, uint8_t addr, gpio_num_t os_int);

#ifdef __cplusplus
}
#endif