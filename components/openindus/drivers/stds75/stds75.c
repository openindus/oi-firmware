#include "stds75.h"

i2c_master_dev_handle_t _i2c_dev_handle = NULL;

// static const char SENSOR_TAG[] = "stds75";

/**
 * @brief Configures I2C parameters and initialize I2C driver
 * @retval None
*/
esp_err_t STDS75_i2c_init()
{
    const uint8_t config_register[2] = {STDS75_CONF, 0b01111000};   // RC1 = 1, RC0 = 1, FT1 = 1, FT0 = 0, POL = 0, M = 0, SD = 0
    
    return i2c_master_transmit(_i2c_dev_handle, config_register, 2, 1000);
}

/**
 * @brief Measures the ambiant temperature
 * @retval float value
*/
float STDS75_get_temperature(void){
    const uint8_t addr_temperature_register = STDS75_TEMP;
    uint8_t temperature_data[2];
    uint32_t bitmask = 0xffff;
    float Tref;
    
    i2c_master_transmit_receive(_i2c_dev_handle, &addr_temperature_register, 1, temperature_data, 2, 1000);
    
    if(temperature_data[0] >= 0x80)
        Tref = (float)(-(~((temperature_data[0] << 8) + (temperature_data[1])) & bitmask) + 1) * 0.0625 / 16;     // Operation of 2's complement conversion
    else
        Tref = (float)((temperature_data[0] << 4) | (temperature_data[1] >> 4)) * 0.0625;
    // ESP_LOGI(SENSOR_TAG, "Tref = %f °C", Tref);
    return Tref;
}

/**
 * @brief Initialization of the temperature sensor driver
 * @retval None
*/
esp_err_t STDS75_init(i2c_master_bus_handle_t bus_handle, uint8_t addr, gpio_num_t os_int)
{
    esp_err_t ret = ESP_OK;

    // Create I2C device handle
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = I2C_CLK,
    };

    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &_i2c_dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    // Configure GPIO
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << os_int),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ret |= gpio_config(&gpio_conf);

    ret |= STDS75_i2c_init();

    return ret;
}