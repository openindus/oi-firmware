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

/* NVS Memory */
#define NVM_NAMESPACE (const char*)"oi"
#define NVM_KEY_BOARD_TYPE (const char*)"board_type"
#define NVM_KEY_SERIAL_NUMBER (const char*)"serial_num"
#define NVM_KEY_VERSION_HARDWARE (const char*)"version_hw"
#define NVM_KEY_VERSION_SOFTWARE (const char*)"version_sw"

void ModuleStandalone::init()
{
    /* LED */
    ESP_LOGI(MODULE_TAG, "Init LED");
    ledonewire_install(MODULE_PIN_LED);
    ledonewire_on(LED_BLUE);

    /* NVS - Board info */
    ESP_LOGI(MODULE_TAG, "Board type      : %d", getBoardType());
    ESP_LOGI(MODULE_TAG, "Serial number   : %d", getSerialNum());

    /* Temperature sensor */
#if !defined(CONFIG_IDF_TARGET_ESP32)
    ESP_LOGI(MODULE_TAG, "Init Temperature sensor");
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
#endif
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
    ledonewire_on(color);
}

/**
 * @brief Turn the led off
 * 
 */
void ModuleStandalone::ledOff()
{
    ledonewire_off();
}

/**
 * @brief Blink the led with given color and period
 * 
 * @param[in] LedColor_t color
 * @param[in] uint32_t period un millisecond
 */
void ModuleStandalone::ledBlink(LedColor_t color, uint32_t period)
{
    ledonewire_blink(color, period);
}

/**
 * @brief Get the internal temperature of the mcu
 * 
 * @return float temperature
 */
float ModuleStandalone::getTemperature(void)
{
    float tsens_out = 0;
#if !defined(CONFIG_IDF_TARGET_ESP32)
    temp_sensor_read_celsius(&tsens_out);
#endif
    return tsens_out;
}

int ModuleStandalone::getBoardType(void)
{
    return _getBoardInfoFromNVS(NVM_KEY_BOARD_TYPE);
}

void ModuleStandalone::setBoardType(int boardType)
{
    _setBoardInfoToNVS(NVM_KEY_BOARD_TYPE, boardType);
}

int ModuleStandalone::getSerialNum(void)
{
    return _getBoardInfoFromNVS(NVM_KEY_SERIAL_NUMBER);
}

void ModuleStandalone::setSerialNum(int serialNum)
{
    _setBoardInfoToNVS(NVM_KEY_SERIAL_NUMBER, serialNum);
}

int32_t ModuleStandalone::_getBoardInfoFromNVS(const char* key)
{
    esp_err_t err = 0;
    nvs_handle_t handle;
    int32_t value;

    /* Initialize NVS */ 
    err = nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    
    ESP_LOGD(MODULE_TAG, "Opening Non-Volatile DigitalOutputage (NVS) handle...");
    err = nvs_open_from_partition(NVS_DEFAULT_PART_NAME, NVM_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        err = nvs_get_i32(handle, key, &value);
        switch (err) {
            case ESP_OK:
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGW(MODULE_TAG,"Undefined board info");
                break;
            default :
                ESP_LOGE(MODULE_TAG,"Error reading nvs memory");
        }
        nvs_close(handle);
    }
    return value;
}

void ModuleStandalone::_setBoardInfoToNVS(const char* key, int32_t value)
{
    esp_err_t err;
    nvs_handle_t handle;
    
    err = nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_LOGD(MODULE_TAG, "Opening Non-Volatile DigitalOutputage (NVS) handle...");
    err = nvs_open_from_partition(NVS_DEFAULT_PART_NAME, NVM_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        err = nvs_set_i32(handle, key, value);
        if (err != ESP_OK) {
            ESP_LOGE(MODULE_TAG, "Fail to write in memory");
        } else {
            err = nvs_commit(handle);
            if (err != ESP_OK)
                ESP_LOGE(MODULE_TAG, "Fail commit nvs memory");
        }
        nvs_close(handle);
    }
}