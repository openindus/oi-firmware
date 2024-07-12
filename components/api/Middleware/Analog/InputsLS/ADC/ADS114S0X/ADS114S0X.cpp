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

    ads114s0x_reg_sys_t sysReg;
    ads114s0x_read_register(_device, ADS114S0X_REG_SYS, 
        (uint8_t*)&sysReg, sizeof(ads114s0x_reg_sys_t));
    sysReg.sendstat = 1; // Enable STATUS (data read)
    sysReg.crc = 0; // Disable CRC (data read)
    sysReg.sys_mon = 0b000; // Disable
    ads114s0x_write_register(_device, ADS114S0X_REG_SYS, 
        (uint8_t*)&sysReg, sizeof(ads114s0x_reg_sys_t));

    ads114s0x_reg_datarate_t datarateReg;
    ads114s0x_read_register(_device, ADS114S0X_REG_DATARATE, 
        (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));
    datarateReg.mode = 0; //1; // Single shot conversion
    ads114s0x_write_register(_device, ADS114S0X_REG_DATARATE, 
        (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));

    /* Data ready */
    _queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(_task, "_task", 2048, this->_device, 10, NULL);
    ads114s0x_add_data_ready_isr_handler(_device, _isr, NULL);

    return ret;
}

int ADS114S0X::config(ADS114S0X_Gain_e gain, ADS114S0X_Reference_e reference, bool useExcitation)
{
    int ret = 0;

    /* Reference */
    ads114s0x_reg_ref_t refReg = {
        .refcon = ADS114S0X_REF_ALWAYS_ON,      // Internal always on
        .refsel = ADS114S0X_REF_REFP0_REFNO,    // Default ref
        .refn_buf = 0,                          // Enable
        .refp_buf = 0,                          // Enable
        .fl_ref_en = 1,                         // Enable reference monitor
    };
    if (reference == REF_INTERNAL_2V5) {
        refReg.refsel = ADS114S0X_REF_INTERNAL_2_5V;
    } else if (reference == REF_EXTERNAL_IDAC1) {
        refReg.refsel = ADS114S0X_REF_REFP1_REFN1;
    } else {
        ESP_LOGE(TAG, "Invalid reference");
        return -1;
    }
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_REF, 
        (uint8_t*)&refReg, sizeof(ads114s0x_reg_ref_t));

    /* PGA */
    ads114s0x_reg_pga_t pgaReg = {
        .gain = static_cast<ads114s0x_pga_gain_e>(log2(static_cast<int>(gain))),
        .pga_en = 1,
        .delay = ADS114S0X_DELAY_14_TMOD
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_PGA, 
        (uint8_t*)&pgaReg, sizeof(ads114s0x_reg_pga_t));

    /* Excitation */
    ads114s0x_reg_idac_t idacReg = {
        .imag = ADS114S0X_IDAC_OFF,
        .reserved = 0,
        .psw = 0,
        .fl_rail_en = 0,
        .i1mux = ADS114S0X_NOT_CONNECTED,
        .i2mux = ADS114S0X_NOT_CONNECTED
    };
    if (useExcitation) {
        idacReg.imag = ADS114S0X_IDAC_1000_UA;
        idacReg.i1mux = ADS114S0X_AINCOM; // IDAC1 to AINCOM
    }
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_IDACMAG, 
        (uint8_t*)&idacReg, sizeof(ads114s0x_reg_idac_t));

    return ret;
}

int ADS114S0X::read(std::vector<uint16_t>* adcCode, 
    ADC_Input_t inputP, ADC_Input_t inputN, bool useVbias)
{
    int ret = 0;

    if (inputP < 0 || inputP > ADS114S0X_INPUT_MAX || 
        inputN < 0 || inputN > ADS114S0X_INPUT_MAX) {
        ESP_LOGE(TAG, "%s: invalid input range", __FUNCTION__);
        return -1; 
    }

    /* VBias */
    if (useVbias) {
        ads114s0x_reg_vbias_t vbiasReg = {
            .vb_ain0 = (uint8_t)(inputP == 0 || inputN == 0),
            .vb_ain1 = (uint8_t)(inputP == 1 || inputN == 1),
            .vb_ain2 = (uint8_t)(inputP == 2 || inputN == 2),
            .vb_ain3 = (uint8_t)(inputP == 3 || inputN == 3),
            .vb_ain4 = (uint8_t)(inputP == 4 || inputN == 4),
            .vb_ain5 = (uint8_t)(inputP == 5 || inputN == 5),
            .vb_ain6 = (uint8_t)(inputP == 6 || inputN == 6),
            .vb_level = 0
        };
        ret |= ads114s0x_write_register(_device, ADS114S0X_REG_VBIAS, 
            (uint8_t*)&vbiasReg, sizeof(ads114s0x_reg_vbias_t));
    }

    /* Mux */
    ads114s0x_reg_inpmux_t inpmuxReg = {
        .muxn = static_cast<ads114s0x_adc_input_e>(inputN),
        .muxp = static_cast<ads114s0x_adc_input_e>(inputP)
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_INPMUX, 
        (uint8_t*)&inpmuxReg, sizeof(ads114s0x_reg_inpmux_t));

    _data.clear();

    /* Start conversion */
    ads114s0x_start(_device);
    ads114s0x_self_offset_calib(_device);
    vTaskDelay(_convTimeMs / portTICK_PERIOD_MS);
    ads114s0x_stop(_device);

    if (adcCode != NULL) {
        adcCode->resize(_data.size());
        std::copy(_data.begin(), _data.end(), adcCode->begin());
    }

    return ret;
}
