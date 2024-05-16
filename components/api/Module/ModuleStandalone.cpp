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

int ModuleStandalone::init()
{
    int err = 0;

    /* Init GPIO service */
    err |= gpio_install_isr_service(0);

    /* Initialize NVS */ 
    esp_err_t ret = nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    /* GPIO19 and GPIO20 are USB interface, force reset/config before setting them as GPIO */
    err |= gpio_reset_pin(GPIO_NUM_19);
    err |= gpio_reset_pin(GPIO_NUM_20);

    /* LED */
    ESP_LOGI(MODULE_TAG, "Init LED");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    /* eFuse - Board info */
    ESP_LOGI(MODULE_TAG, "Board type       : %u", getBoardType());
    ESP_LOGI(MODULE_TAG, "Serial number    : %d", getSerialNum());
    char hardware_version[4];
    getHardwareVersion(hardware_version);
    ESP_LOGI(MODULE_TAG, "Hardware version : %.*s", 4, hardware_version);
    char date_code[4];
    getDateCode(date_code);
    ESP_LOGI(MODULE_TAG, "Date code        : %.*s", 4, date_code);
    char software_version[32];
    getDateCode(software_version);
    ESP_LOGI(MODULE_TAG, "Software version : %s", software_version);

    /* Temperature sensor */
    ESP_LOGI(MODULE_TAG, "Init Temperature sensor");
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    err |= temp_sensor_get_config(&temp_sensor);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    err |= temp_sensor_set_config(temp_sensor);
    err |= temp_sensor_start();

    return err;
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

uint16_t ModuleStandalone::getBoardType(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.board_type;
        } else {
            ESP_LOGW(MODULE_TAG, "eFuse BLOCK0 corrupted !");
            return 0;
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Board type is not defined !");
        return 0;
    }
    return 0;
}

uint32_t ModuleStandalone::getSerialNum(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.serial_number;
        } else {
            ESP_LOGW(MODULE_TAG, "eFuse BLOCK0 corrupted !");
            return 0;
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Serial number is not defined !");
        return 0;
    }
}

void ModuleStandalone::getHardwareVersion(char hardware_version[4])
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            strcpy(hardware_version, data.hardware_version);
        } else {
            ESP_LOGW(MODULE_TAG, "eFuse BLOCK0 corrupted !");
            strcpy(hardware_version, "none");
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Hardware version is not defined !");
        strcpy(hardware_version, "none");
    }
    return;
}

void ModuleStandalone::getDateCode(char date_code[4])
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY0) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY0, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            strcpy(date_code, data.date_code);
        } else {
            ESP_LOGW(MODULE_TAG, "eFuse BLOCK0 corrupted !");
            strcpy(date_code, "none");
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Date code is not defined !");
        strcpy(date_code, "none");
    }
    return;
}

void ModuleStandalone::getSoftwareVersion(char software_version[32])
{
    const esp_app_desc_t* app_info = esp_ota_get_app_description();
    strcpy(software_version, app_info->version);
    return;
}

bool ModuleStandalone::setBoardInfo(uint16_t board_type, uint32_t serial_num, char hardware_version[4], char date_code[4])
{
    ESP_LOGW(MODULE_TAG, "This operation can be done only once !");

    Module_eFuse_Info_t data;
    memset(&data, 0, sizeof(Module_eFuse_Info_t));
    data.board_type = board_type;
    data.serial_number = serial_num;
    strcpy(data.hardware_version, hardware_version);
    strcpy(data.date_code, date_code);
    data.checksum = _calculate_eFuse_checksum((uint8_t*)&data);

    esp_err_t err = esp_efuse_write_key(EFUSE_BLK_KEY0, ESP_EFUSE_KEY_PURPOSE_USER, &data, sizeof(Module_eFuse_Info_t));
    
    if (err != ESP_OK) {
        ESP_LOGE(MODULE_TAG, "Error in eFuse write: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

uint8_t ModuleStandalone::_calculate_eFuse_checksum(uint8_t* data)
{
    uint8_t checksum = 0xFE;

    for (int i = 0; i < 31; i++) {
        checksum ^= data[i];
    }

    return checksum;
}

bool ModuleStandalone::_verify_eFuse_checksum(Module_eFuse_Info_t info)
{
    return (info.checksum == _calculate_eFuse_checksum((uint8_t*)&info));
}