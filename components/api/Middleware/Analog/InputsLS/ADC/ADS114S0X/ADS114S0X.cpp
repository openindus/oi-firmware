/**
 * @file ADS114S0X.cpp
 * @brief ADS114S0X
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ADS114S0X.h"

static const char TAG[] = "ADS114S0X";

QueueHandle_t ADS114S0X::_queue = NULL;
std::vector<uint16_t> ADS114S0X::_data;

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
            uint16_t adcCode;
            if (ads114s0x_read_data(device, &adcCode) == 0) {
                _data.push_back(adcCode);
            }
        }
    }
}

int ADS114S0X::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "ADC init.");

    /* Initialize ADS114S0X device */
    ads114s0x_init(&_device, &_config);
    ads114s0x_hard_reset(_device);

    ads114s0x_wakeup(_device);

    ads114s0x_reg_id_t id;
    ads114s0x_read_register(_device, ADS114S0X_REG_ID, 
        (uint8_t*)&id, sizeof(ads114s0x_reg_id_t));
    if (id.dev_id == ADS114S0X_DEV_ID_ADS114S08) {
        ESP_LOGI(TAG, "ADC device: ADS114S08");
    } else if (id.dev_id == ADS114S0X_DEV_ID_ADS114S06) {
        ESP_LOGI(TAG, "ADC device: ADS114S06");
    } else {
        ESP_LOGE(TAG, "ADC device not found");
    }

    ads114s0x_reg_sys_t sys;
    ads114s0x_read_register(_device, ADS114S0X_REG_SYS, 
        (uint8_t*)&sys, sizeof(ads114s0x_reg_sys_t));
    sys.sendstat = 1; // Enable STATUS (data read)
    sys.crc = 0; // Disable CRC (data read)
    sys.sys_mon = 0b000;
    ads114s0x_write_register(_device, ADS114S0X_REG_SYS, 
        (uint8_t*)&sys, sizeof(ads114s0x_reg_sys_t));

    ads114s0x_reg_datarate_t datarate;
    ads114s0x_read_register(_device, ADS114S0X_REG_DATARATE, 
        (uint8_t*)&datarate, sizeof(ads114s0x_reg_datarate_t));
    datarate.mode = 0; //1; // Single shot conversion
    ads114s0x_write_register(_device, ADS114S0X_REG_DATARATE, 
        (uint8_t*)&datarate, sizeof(ads114s0x_reg_datarate_t));

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
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_REF, 
        (uint8_t*)&ref, sizeof(ads114s0x_reg_ref_t));

    /* PGA */
    ads114s0x_reg_pga_t pga = {
        .gain = ADS114S0X_PGA_GAIN_4,
        .pga_en = 1,
        .delay = ADS114S0X_DELAY_14_TMOD
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_PGA, 
        (uint8_t*)&pga, sizeof(ads114s0x_reg_pga_t));

    /* Excitation */
    ads114s0x_reg_idac_t idac = {
        .imag = ADS114S0X_IDAC_1000_UA,
        .reserved = 0,
        .psw = 0,
        .fl_rail_en = 0,
        .i1mux = ADS114S0X_AINCOM,              // IDAC1 to AINCOM
        .i2mux = ADS114S0X_NOT_CONNECTED
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_IDACMAG, 
        (uint8_t*)&idac, sizeof(ads114s0x_reg_idac_t));

    return ret;
}

int ADS114S0X::read(std::vector<uint16_t>* adcCode, 
    ADC_Input_t inputP, ADC_Input_t inputN, uint32_t timeout_ms)
{
    int ret = 0;

    /* Mux */
    ads114s0x_reg_inpmux_t inpmux = {
        .muxn = inputN,
        .muxp = inputP
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_INPMUX, 
        (uint8_t*)&inpmux, sizeof(ads114s0x_reg_inpmux_t));

    _data.clear();

    /* Start conversion */
    ads114s0x_start(_device);
    ads114s0x_self_offset_calib(_device);
    vTaskDelay(timeout_ms / portTICK_PERIOD_MS);
    ads114s0x_stop(_device);

    if (adcCode != NULL) {
        adcCode->resize(_data.size());
        std::copy(_data.begin(), _data.end(), adcCode->begin());
    }

    return ret;
}
