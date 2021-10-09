/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIModule.cpp
 * @brief Generic functions for OIModule
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "OIModule.h"

#include "nvs_flash.h"
#include "nvs.h"

// temp_sensor.h works only on esp32s2
#ifndef CONFIG_IDF_TARGET_ESP32
#include "driver/temp_sensor.h"
#endif

static const char OI_MODULE_TAG[] = "OIModule";

uint8_t OIModule::_id;
OIModule* OIModule::_instance[];
int OIModule::_nbInstance = 0;

uint8_t OIModule::getHardwareId(void)
{
    uint8_t nvsId = UNDEFINED_ID;

    // Initialize NVS
    esp_err_t err = nvs_flash_init_partition(OI_NVS_PARTITION);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open
    ESP_LOGV(OI_MODULE_TAG, "Opening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t readId;
    err = nvs_open_from_partition(OI_NVS_PARTITION, OI_NVM_NAMESPACE, NVS_READWRITE, &readId);
    if (err != ESP_OK) {
        ESP_LOGE(OI_MODULE_TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        // Read Id
        err = nvs_get_u8(readId, OI_NVM_KEY_ID, &nvsId);
        switch (err) {
            case ESP_OK:
                ESP_LOGV(OI_MODULE_TAG, "Hardware id is %d", nvsId);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGW(OI_MODULE_TAG,"Hardware was not initialized yet : default value (%d)", nvsId);
                break;
            default :
                ESP_LOGE(OI_MODULE_TAG,"Error reading nvs memory");
        }       

        // Close
        nvs_close(readId);
    }   

    return nvsId;
}

void OIModule::setHardwareId(uint8_t id)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init_partition(OI_NVS_PARTITION);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open
    ESP_LOGV(OI_MODULE_TAG, "Opening Non-Volatile Storage (NVS) handle...");
    nvs_handle_t readId;
    err = nvs_open_from_partition(OI_NVS_PARTITION, OI_NVM_NAMESPACE, NVS_READWRITE, &readId);
    if (err != ESP_OK) {
        ESP_LOGE(OI_MODULE_TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        // Write
        ESP_LOGV(OI_MODULE_TAG, "Update Id in NVS Memory : %d", id);

        err = nvs_set_u8(readId, OI_NVM_KEY_ID, id);
        if (err != ESP_OK)
        {
            ESP_LOGE(OI_MODULE_TAG, "Fail to write id in memory");
        }
        err = nvs_commit(readId);

        if (err != ESP_OK)
        {
            ESP_LOGE(OI_MODULE_TAG, "Fail commit nvs memory");
        }

        // Close
        nvs_close(readId);
    }
    // Update OIModuleMaster _id attribute
    _id = id;
}

void OIModuleMaster::init()
{
    ledonewire_install(OIMODULE_PIN_LED);
    ledonewire_blink(LED_BLUE, 1000);

    _id = getHardwareId();

    /* Temperature sensor */
#ifndef CONFIG_OI_CORE
    ESP_LOGI(OI_MODULE_TAG, "Initializing Temperature sensor");
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor_get_config(&temp_sensor);
    temp_sensor.dac_offset = TSENS_DAC_DEFAULT; // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
#endif

    /* OI-ID Configuration */

    /* Configure ADC */
    adc1_config_width(OIMODULE_OI_ID_ADC_WIDTH);
    adc1_config_channel_atten((adc1_channel_t)OIMODULE_OI_ID_ADC_CHANNEL, OIMODULE_OI_ID_ADC_ATTEN);

    /* Characterize ADC */
    esp_adc_cal_characteristics_t* _adc_chars = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(OIMODULE_OI_ID_ADC_UNIT, OIMODULE_OI_ID_ADC_ATTEN, OIMODULE_OI_ID_ADC_WIDTH, 1100, _adc_chars);

    uint32_t adc_reading = 0;

    /* Multisampling */
    for (int i = 0; i < 64; i++)
    {
        adc_reading += adc1_get_raw((adc1_channel_t)OIMODULE_OI_ID_ADC_CHANNEL);
    }
    adc_reading /= 64;

    /* Convert adc_reading to voltage in mV */
    _oiId = esp_adc_cal_raw_to_voltage(adc_reading, _adc_chars);
}

void OIModuleMaster::restart() const
{
    ESP_LOGV(OI_MODULE_TAG, "restart now");
    vTaskDelay(pdMS_TO_TICKS(50)); // wait for message to send
    esp_restart();
}

void OIModuleMaster::ledOn(LedColor_t color) const
{
    ledonewire_on(color);
}

void OIModuleMaster::ledOff() const
{
    ledonewire_off();
}

void OIModuleMaster::ledBlink(LedColor_t color, uint32_t period) const
{
    ledonewire_blink(color, period);
}

float OIModuleMaster::getInternalTemp(void) const
{
    float tsens_out = 0;
#ifndef CONFIG_OI_CORE
    temp_sensor_read_celsius(&tsens_out);
#endif
    return tsens_out;
}