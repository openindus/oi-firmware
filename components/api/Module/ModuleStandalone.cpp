/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleStandalone.cpp
 * @brief Generic functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ModuleStandalone.h"
#include "ModulePinout.h"

static const char MODULE_TAG[] = "Module";

void ModuleStandalone::init()
{
    /* Init GPIO service */
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    /* Initialize NVS */ 
    esp_err_t err = nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* GPIO19 and GPIO20 are USB interface, force reset/config before setting them as GPIO */
    gpio_reset_pin(GPIO_NUM_19);
    gpio_reset_pin(GPIO_NUM_20);

    /* LED */
    ESP_LOGI(MODULE_TAG, "Init LED");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    /* eFuse - Board info */
    char board_type[16];
    getBoardType(board_type);
    ESP_LOGI(MODULE_TAG, "Board type       : %s", board_type);
    ESP_LOGI(MODULE_TAG, "Serial number    : %d", getSerialNum());
    char hardware_version[16];
    getHardwareVersion(hardware_version);
    ESP_LOGI(MODULE_TAG, "Hardware version : %s", hardware_version);

    /* Temperature sensor */
    ESP_LOGI(MODULE_TAG, "Init Temperature sensor");
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

/**
 * @brief Reset the current board
 * 
 */
void ModuleStandalone::restart()
{
    ESP_LOGD(MODULE_TAG, "restart now");
    vTaskDelay(pdMS_TO_TICKS(50)); // wait for message to send
    esp_restart();
}

/**
 * @brief Turn the led on with the given color
 * 
 * @param[in] LedColor_t color
 *
 */
void ModuleStandalone::ledOn(LedColor_t color)
{
    Led::on(color);
}

/**
 * @brief Turn the led off
 * 
 */
void ModuleStandalone::ledOff()
{
    Led::off();
}

/**
 * @brief Blink the led with given color and period
 * 
 * @param[in] LedColor_t color
 * @param[in] uint32_t period un millisecond
 */
void ModuleStandalone::ledBlink(LedColor_t color, uint32_t period)
{
    Led::blink(color, period);
}

/**
 * @brief Get the internal temperature of the mcu
 * 
 * @return float temperature
 */
float ModuleStandalone::getTemperature(void)
{
    float tsens_out = 0;
    temp_sensor_read_celsius(&tsens_out);
    return tsens_out;
}

void ModuleStandalone::getBoardType(char* board_type)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        strcpy(board_type, data.board_type);
    } else {
        ESP_LOGW(MODULE_TAG, "Board type is not defined !");
        strcpy(board_type, "undefined");
    }
    return;
}

int ModuleStandalone::getSerialNum(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        return data.serial_number;
    } else {
        ESP_LOGW(MODULE_TAG, "Serial number is not defined !");
        return 0;
    }
    
}

void ModuleStandalone::getHardwareVersion(char* version)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        strcpy(version, data.hardware_version);
    } else {
        ESP_LOGW(MODULE_TAG, "Hardware version is not defined !");
        strcpy(version, "undefined");
    }
    return;
}

void ModuleStandalone::getSoftwareVersion(char* version)
{
    const esp_app_desc_t* app_info = esp_ota_get_app_description();
    strcpy(version,app_info->version);
    return;
}

bool ModuleStandalone::setBoardInfo(char* board_type, int serial_num, char* hardware_version)
{
    ESP_LOGW(MODULE_TAG, "This operation can be done only once !");

    Module_eFuse_Info_t data;
    memset(&data, 0, sizeof(Module_eFuse_Info_t));
    strcpy(data.board_type, board_type);
    data.serial_number = serial_num;
    strcpy(data.hardware_version, hardware_version);

    esp_err_t err = esp_efuse_write_key(EFUSE_BLK_KEY0, ESP_EFUSE_KEY_PURPOSE_USER, &data, sizeof(Module_eFuse_Info_t));
    
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_TAG, "Error in eFuse write: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}