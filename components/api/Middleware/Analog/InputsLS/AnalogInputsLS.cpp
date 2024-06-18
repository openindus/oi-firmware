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
    ads114s0x_enable(_device);

    ads114s0x_wakeup(_device);

    ads114s0x_reg_id_t id;
    ads114s0x_read_register(_device, ADS114S0X_REG_ID, (uint8_t*)&id, sizeof(ads114s0x_reg_id_t));
    if (id.dev_id == ADS114S0X_DEV_ID_ADS114S08) {
        ESP_LOGI(TAG, "ADC device: ADS114S08");
    } else if (id.dev_id == ADS114S0X_DEV_ID_ADS114S06) {
        ESP_LOGI(TAG, "ADC device: ADS114S06");
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

int Multiplexer::init(void)
{
    int ret = 0;

    gpio_config_t config = {
        .pin_bit_mask = ((1ULL << _inputPins[0]) |
                        (1ULL << _inputPins[1]) |
                        (1ULL << _inputPins[2]) |
                        (1ULL << _outputPins[0]) |
                        (1ULL << _outputPins[1]) |
                        (1ULL << _outputPins[2])),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ret |= gpio_config(&config);

    return ret;
}

int Multiplexer::route(int input, int output)
{
    int ret = 0;

    if (input < 0 || input > 7 || output < 0 || output > 7) {
        ESP_LOGE(TAG, "%s: invalid input/output range", __FUNCTION__);
        return -1; 
    }

    // Set the input control pins
    for(int i = 0; i < 3; i++) {
        ret |= gpio_set_level(_inputPins[i], (input >> i) & 1);
    }

    // Set the output control pins
    for(int i = 0; i < 3; i++) {
        ret |= gpio_set_level(_outputPins[i], (output >> i) & 1);
    }

    return ret;
}

ADC_Device* AnalogInputsLS::_adcDevice = NULL;
Multiplexer* AnalogInputsLS::_highSideMux = NULL;
Multiplexer* AnalogInputsLS::_lowSideMux = NULL;

int AnalogInputsLS::init(void)
{
    int ret = 0;

    /* ADC */
    if (_adcDevice != NULL) {
        ret |= _adcDevice->init();
        ret |= _adcDevice->test();
    } else {
        ESP_LOGE(TAG, "Failed to initialize ADC device");
        ret |= -1;
    }

    /* Multiplexer (high side/ low side)*/
    if ((_highSideMux != NULL) && (_lowSideMux != NULL)) {
        ret |= _highSideMux->init();
        ret |= _lowSideMux->init();
    } else {
        ESP_LOGE(TAG, "Failed to initialize multiplexer");
        ret |= -1;
    }

    /* Digipot */

    /* Digital thermometer */

    return ret;
}