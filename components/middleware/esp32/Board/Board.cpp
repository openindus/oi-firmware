/**
 * @file Board.cpp
 * @brief Board interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Board.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_efuse.h"
#include "esp_ota_ops.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/temp_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char TAG[] = "Board";

int Board::init(void)
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
void Board::restart(void)
{
    ESP_LOGD(TAG, "restart now");
    vTaskDelay(pdMS_TO_TICKS(50)); // wait for message to send
    esp_restart();
}

/**
 * @brief Get the internal temperature of the mcu
 * 
 * @return float temperature
 */
float Board::getTemperature(void)
{
    float tsens_out = 0;
    temp_sensor_read_celsius(&tsens_out);
    return tsens_out;
}

uint8_t Board::getBoardType(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Board_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Board_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.board_type;
        } else {
            ESP_LOGW(TAG, "eFuse BLOCK5 corrupted !");
            return 0;
        }
    } else {
        ESP_LOGW(TAG, "Board type is not defined !");
        return 0;
    }
}

uint8_t Board::getHardwareVariant(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Board_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Board_eFuse_Info_t)*8);
        if (_verify_eFuse_checksum(data)) {
            return data.hardware_variant;
        } else {
            ESP_LOGW(TAG, "eFuse BLOCK5 corrupted !");
            return 0;
        }
    } else {
        ESP_LOGW(TAG, "Hardware variant is not defined !");
        return 0;
    }
}

uint32_t Board::getSerialNum(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Board_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Board_eFuse_Info_t)*8);
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

int64_t Board::getTimestamp(void)
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY5) == false) {
        Board_eFuse_Info_t data;
        esp_efuse_read_block(EFUSE_BLK_KEY5, &data, 0, sizeof(Board_eFuse_Info_t)*8);
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

void Board::getSoftwareVersion(char software_version[32])
{
    const esp_app_desc_t* app_info = esp_ota_get_app_description();
    strcpy(software_version, app_info->version);
    return;
}

bool Board::setBoardInfo(uint8_t board_type, uint8_t hardware_variant, uint32_t serial_num, int64_t timestamp)
{
    ESP_LOGW(TAG, "This operation can be done only once !");

    Board_eFuse_Info_t data;

    data.hardware_variant = hardware_variant;
    data.board_type = board_type;
    data.serial_number = serial_num;
    data.timestamp = timestamp;
    memset(&data.reserved_1, 0, sizeof(data.reserved_1));
    memset(&data.reserved_2, 0, sizeof(data.reserved_2));
    data.checksum = _calculate_eFuse_checksum((uint8_t*)&data);

    esp_err_t err = esp_efuse_write_key(EFUSE_BLK_KEY5, ESP_EFUSE_KEY_PURPOSE_USER, &data, sizeof(Board_eFuse_Info_t));
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in eFuse write: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

uint8_t Board::_calculate_eFuse_checksum(uint8_t* data)
{
    uint8_t checksum = 0xFE;

    for (int i = 0; i < 31; i++) {
        checksum ^= data[i];
    }

    return checksum;
}

bool Board::_verify_eFuse_checksum(Board_eFuse_Info_t info)
{
    return (info.checksum == _calculate_eFuse_checksum((uint8_t*)&info));
}

bool Board::checkBootError(void)
{
    /* Check reset reason */
    esp_reset_reason_t reason;
    reason = esp_reset_reason();
    if ((reason != ESP_RST_POWERON) && (reason != ESP_RST_SW) && (reason != ESP_RST_UNKNOWN)) {
        ESP_LOGE(TAG, "Reset reason : %d", reason);
        return true;
    }
    else {
        return false;
    }
}