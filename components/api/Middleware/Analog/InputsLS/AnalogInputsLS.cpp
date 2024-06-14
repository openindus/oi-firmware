/**
 * @file AnalogInputsLS.cpp
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsLS.h"

static const char TAG[] = "AnalogInputsLS";

int ADC_Device::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "ADC init.");

    /* Initialize ADC device */
    ads114s0x_init(&_device, &_config);

    ads114s0x_wakeup(_device);

    ads114s0x_reg_id_t id;
    ads114s0x_read_register(_device, ADS114S0X_REG_ID, (uint8_t*)&id, sizeof(ads114s0x_reg_id_t));
    if (id.dev_id == ADS114S0X_DEV_ID_ADS114S08) {
        ESP_LOGI(TAG, "ADC device: ADS114S08\n");
    } else if (id.dev_id == ADS114S0X_DEV_ID_ADS114S06) {
        ESP_LOGI(TAG, "ADC device: ADS114S06\n");
    } else {
        ESP_LOGE(TAG, "ADC device not found");
    }

    return ret;
}

int ADC_Device::test(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "ADC test");

    /* Test */
    ads114s0x_reg_inpmux_t inpmux;
    ads114s0x_read_register(_device, ADS114S0X_REG_INPMUX, (uint8_t*)&inpmux, sizeof(ads114s0x_reg_inpmux_t));
    printf("%d\n", inpmux.muxn);

    inpmux.muxn = ADS114S0X_AIN3;
    ads114s0x_write_register(_device, ADS114S0X_REG_INPMUX, (uint8_t*)&inpmux, sizeof(ads114s0x_reg_inpmux_t));

    ads114s0x_read_register(_device, ADS114S0X_REG_INPMUX, (uint8_t*)&inpmux, sizeof(ads114s0x_reg_inpmux_t));
    printf("%d\n", inpmux.muxn);

    /* data rate */
    printf("sizeof(ads114s0x_reg_datarate_t): %d\n", sizeof(ads114s0x_reg_datarate_t));
    ads114s0x_reg_datarate_t datarate;
    ads114s0x_read_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarate, sizeof(ads114s0x_reg_datarate_t));

    return ret;
}

ADC_Device* AnalogInputsLS::_adc;

int AnalogInputsLS::init(ADC_Device* adc)
{
    int ret = 0;

    if (adc == NULL) {
        ESP_LOGE(TAG, "NULL ptr");
        return -1;
    } else {
        _adc = adc;
    }

    ret |= _adc->init();
    ret |= _adc->test();

    return ret;
}