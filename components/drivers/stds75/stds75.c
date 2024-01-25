#include "stds75.h"

int i2c_master_port = I2C_NUM_0;

static const char SENSOR_TAG[] = "stds75";

/**
 * @brief Configures I2C parameters and initialize I2C driver
 * @retval None
*/
void STDS75_i2c_init(gpio_num_t sda, gpio_num_t scl){
    i2c_config_t i2ccfg = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = sda,
    .scl_io_num = scl,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = I2C_CLK,
    .clk_flags = 0
    };

    esp_err_t err_config = i2c_param_config(i2c_master_port, &i2ccfg);
    if(err_config != ESP_OK)
        ESP_LOGE(SENSOR_TAG, "Bad settings of the I2C configuration parameters");
    else
        ESP_LOGI(SENSOR_TAG, "The parameters are configured");
    ESP_ERROR_CHECK(err_config);
    esp_err_t err_install = i2c_driver_install(i2c_master_port, i2ccfg.mode, 0, 0, 0);
    if(err_config != ESP_OK)
        ESP_LOGE(SENSOR_TAG, "Bad installation of the I2C driver");
    else
        ESP_LOGI(SENSOR_TAG, "The I2C driver is installed");
    ESP_ERROR_CHECK(err_install);
    const uint8_t config_register[2] = {STDS75_CONF, 0b01111000};   // RC1 = 1, RC0 = 1, FT1 = 1, FT0 = 0, POL = 0, M = 0, SD = 0
    i2c_master_write_to_device(i2c_master_port, SENSOR_ADDRESS, config_register, 2, 1000);
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
    i2c_master_write_read_device(i2c_master_port, SENSOR_ADDRESS, &addr_temperature_register, 1, temperature_data, 2, 1000);
    if(temperature_data[0] >= 0x80)
        Tref = (float)(-(~((temperature_data[0] << 8) + (temperature_data[1])) & bitmask) + 1) * 0.0625 / 16;     // Operation of 2's complement conversion
    else
        Tref = (float)((temperature_data[0] << 8) + (temperature_data[1])) * 0.0625 / 16;
    // ESP_LOGI(SENSOR_TAG, "Tref = %f °C", Tref);
    return Tref;
}

/**
 * @brief Initialization of the temperature sensor driver
 * @retval None
*/
void STDS75_init(gpio_num_t sda, gpio_num_t scl, gpio_num_t os_int){
    STDS75_i2c_init(sda, scl);
    gpio_set_direction(os_int, GPIO_MODE_INPUT);
}