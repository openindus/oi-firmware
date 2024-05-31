/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Module.cpp
 * @brief Generic functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "Module.h"

static const char TAG[] = "Module";
uint16_t ModuleStandalone::_type = 0;

int ModuleStandalone::init(uint16_t type)
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
    ESP_LOGI(TAG, "Init LED");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    /* eFuse - Board info */
    uint16_t local_type = getBoardType(); // read it only once to avoid multiple warning
    ESP_LOGI(TAG, "Board type       : %u", local_type);
    ESP_LOGI(TAG, "Serial number    : %d", getSerialNum());
    char hardware_version[4];
    getHardwareVersion(hardware_version);
    ESP_LOGI(TAG, "Hardware version : %.*s", 4, hardware_version);
    ESP_LOGI(TAG, "Board timestamp  : %lli", getTimestamp());
    char software_version[32];
    getSoftwareVersion(software_version);
    ESP_LOGI(TAG, "Software version : %s", software_version);

    if (local_type != type) {
        // Hack because we cannot differentiate OI_CORE and OI_CORELITE for now...
        if (type == TYPE_OI_CORE && local_type == TYPE_OI_CORELITE) {
            ESP_LOGE(TAG, "OICoreLite type checked");
        }
        else if (local_type == 0){
            ESP_LOGW(TAG, "Cannot check board type");
            _type = type; // Save type because the one in eFuse is wrong
        } 
        else {
            char name[16];
            ESP_LOGE(TAG, "Incorrect board type detected ! You have program the board as an %s and board is an %s", \
                     ModuleUtils::typeToName(type, name), ModuleUtils::typeToName(local_type, name));
            err |= -1; // Do not start the code because we are on wrong board
        }
    }

    /* Temperature sensor */
    ESP_LOGI(TAG, "Init Temperature sensor");
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
    ESP_LOGD(TAG, "restart now");
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
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.board_type;
        } else {
            ESP_LOGW(TAG, "eFuse BLOCK5 corrupted !");
            return _type;
        }
    } else {
        ESP_LOGW(TAG, "Board type is not defined !");
        return _type;
    }
}

uint32_t ModuleStandalone::getSerialNum(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.serial_number;
        } else {
            ESP_LOGW(TAG, "eFuse BLOCK5 corrupted !");
            return 0;
        }
    } else {
        ESP_LOGW(TAG, "Serial number is not defined !");
        return 0;
    }
}

void ModuleStandalone::getHardwareVersion(char hardware_version[4])
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            strcpy(hardware_version, data.hardware_version);
        } else {
            ESP_LOGW(TAG, "eFuse BLOCK5 corrupted !");
            strcpy(hardware_version, "none");
        }
    } else {
        ESP_LOGW(TAG, "Hardware version is not defined !");
        strcpy(hardware_version, "none");
    }
    return;
}

int64_t ModuleStandalone::getTimestamp(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Module_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Module_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.timestamp;
        } else {
            ESP_LOGW(TAG, "eFuse BLOCK5 corrupted !");
            return 0;
        }
    } else {
        ESP_LOGW(TAG, "Date code is not defined !");
        return 0;
    }
}

void ModuleStandalone::getSoftwareVersion(char software_version[32])
{
    const esp_app_desc_t* app_info = esp_ota_get_app_description();
    strcpy(software_version, app_info->version);
    return;
}

bool ModuleStandalone::setBoardInfo(uint16_t board_type, uint32_t serial_num, char hardware_version[4], int64_t timestamp)
{
    ESP_LOGW(TAG, "This operation can be done only once !");

    Module_eFuse_Info_t data;

    data.board_type = board_type;
    data.serial_number = serial_num;
    strcpy(data.hardware_version, hardware_version);
    data.timestamp = timestamp;
    memset(&data.reserved, 0, sizeof(data.reserved));
    data.checksum = _calculate_eFuse_checksum((uint8_t*)&data);

    esp_err_t err = esp_efuse_write_key(EFUSE_BLK_KEY5, ESP_EFUSE_KEY_PURPOSE_USER, &data, sizeof(Module_eFuse_Info_t));
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in eFuse write: %s", esp_err_to_name(err));
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