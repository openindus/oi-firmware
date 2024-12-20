#include "stds75.h"

i2c_port_t _i2c_master_port = I2C_NUM_0;
uint8_t _i2c_address = 0;

// static const char SENSOR_TAG[] = "stds75";

/**
 * @brief Configures I2C parameters and initialize I2C driver
 * @retval None
*/
esp_err_t STDS75_i2c_init()
{
    const uint8_t config_register[2] = {STDS75_CONF, 0b01111000};   // RC1 = 1, RC0 = 1, FT1 = 1, FT0 = 0, POL = 0, M = 0, SD = 0
    return i2c_master_write_to_device(_i2c_master_port, _i2c_address, config_register, 2, 1000);
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
    i2c_master_write_read_device(_i2c_master_port, _i2c_address, &addr_temperature_register, 1, temperature_data, 2, 1000);
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
esp_err_t STDS75_init(i2c_port_t port, uint8_t addr, gpio_num_t os_int)
{
    esp_err_t ret = ESP_OK;

    _i2c_master_port = port;
    _i2c_address = addr;

     gpio_config_t config = {
        .pin_bit_mask = (1ULL << os_int),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ret |= gpio_config(&config);

    ret |= STDS75_i2c_init();

    return ret;
}