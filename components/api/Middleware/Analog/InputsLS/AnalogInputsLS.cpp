/**
 * @file AnalogInputsLS.cpp
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsLS.h"

static const char TAG[] = "AnalogInputsLS";

QueueHandle_t ADS114S0X::_queue = NULL;

void IRAM_ATTR ADS114S0X::_isr(void* arg)
{
    uint32_t buffer = (uint32_t) arg;
    xQueueSendFromISR(_queue, &buffer, NULL);
}

void ADS114S0X::_task(void* arg)
{
    ads114s0x_device_t* device = (ads114s0x_device_t*) arg;
    uint32_t buffer;
    while(1) {
        if (xQueueReceive(_queue, &buffer, portMAX_DELAY)) {
            uint16_t data;
            if (ads114s0x_read_data(device, &data) == 0) {
                printf("data : %d (0x%02x)\n", data, data);
            } else {
                printf("read data failed !\n");
            }
        }
    }
}

int ADS114S0X::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "ADS114S0X init.");

    /* Initialize ADS114S0X device */
    ads114s0x_init(&_device, &_config);
    ads114s0x_hard_reset(_device);

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

    ads114s0x_reg_sys_t sys;
    ads114s0x_read_register(_device, ADS114S0X_REG_SYS, (uint8_t*)&sys, sizeof(ads114s0x_reg_sys_t));
    sys.sendstat = 1; // Enable STATUS (data read)
    sys.crc = 0; // Disable CRC (data read)
    sys.sys_mon = 0b000;
    ads114s0x_write_register(_device, ADS114S0X_REG_SYS, (uint8_t*)&sys, sizeof(ads114s0x_reg_sys_t));

    ads114s0x_reg_datarate_t datarate;
    ads114s0x_read_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarate, sizeof(ads114s0x_reg_datarate_t));
    datarate.mode = 0; //1; // Single shot conversion
    ads114s0x_write_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarate, sizeof(ads114s0x_reg_datarate_t));

    /* Data ready */
    _queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(_task, "_task", 2048, this->_device, 10, NULL);
    ads114s0x_add_data_ready_isr_handler(_device, _isr, NULL);

    return ret;
}

int ADS114S0X::config(void)
{
    int ret = 0;

    /* Reference */
    ads114s0x_reg_ref_t ref = {
        .refcon = ADS114S0X_REF_ALWAYS_ON,      // Internal always on
        .refsel = ADS114S0X_REF_REFP1_REFN1,    // Select REFP1, REFN1 (input reference)
        .refn_buf = 0,                          // Enable
        .refp_buf = 0,                          // Enable
        .fl_ref_en = 1,                         // Enable reference monitor
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_REF, (uint8_t*)&ref, sizeof(ads114s0x_reg_ref_t));

    /* PGA */
    ads114s0x_reg_pga_t pga = {
        .gain = ADS114S0X_PGA_GAIN_4,
        .pga_en = 1,
        .delay = ADS114S0X_DELAY_14_TMOD
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_PGA, (uint8_t*)&pga, sizeof(ads114s0x_reg_pga_t));

    /* Excitation */
    ads114s0x_reg_idac_t idac = {
        .imag = ADS114S0X_IDAC_1000_UA,
        .reserved = 0,
        .psw = 0,
        .fl_rail_en = 0,
        .i1mux = ADS114S0X_AINCOM,              // IDAC1 to AINCOM
        .i2mux = ADS114S0X_NOT_CONNECTED
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_IDACMAG, (uint8_t*)&idac, sizeof(ads114s0x_reg_idac_t));

    /* Mux */
    ads114s0x_reg_inpmux_t inpmux = {
        .muxn = ADS114S0X_AIN0,
        .muxp = ADS114S0X_AIN5
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_INPMUX, (uint8_t*)&inpmux, sizeof(ads114s0x_reg_inpmux_t));

    return ret;
}

int ADS114S0X::startConversion(void)
{
    int ret = 0;
    ret |= ads114s0x_start(_device);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    ret |= ads114s0x_self_offset_calib(_device);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ret |= ads114s0x_stop(_device);
    vTaskDelay(500 / portTICK_PERIOD_MS);

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

ADS114S0X* AnalogInputsLS::_adc = NULL;
Multiplexer* AnalogInputsLS::_highSideMux = NULL;
Multiplexer* AnalogInputsLS::_lowSideMux = NULL;

int AnalogInputsLS::init(void)
{
    int ret = 0;

    /* ADC */
    if (_adc != NULL) {
        ret |= _adc->init();
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

int AnalogInputsLS::test2WireRTD(void)
{
    int ret = 0;

    /* MUX Excitation */
    _highSideMux->route(1, 0); // IDAC1 to A+ (AIN5)
    _lowSideMux->route(0, 3); // A- (AIN0) to RBIAS RTD

    /* ADC */
    _adc->config();
    _adc->startConversion();

    return ret;
}