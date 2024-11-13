/**
 * @file ADS114S0X.cpp
 * @brief ADS114S0X
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "_ADS114S0X.h"

static const char TAG[] = "ADS114S0X";

QueueHandle_t ADS114S0X::_queue = NULL;

void IRAM_ATTR ADS114S0X::_isr(void* arg)
{
    xQueueSendFromISR(_queue, NULL, NULL);
}

int ADS114S0X::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "ADC init.");

    /* Initialize ADS114S0X device */
    ret |= ads114s0x_init(&_device, &_config);
    ret |= ads114s0x_hard_reset(_device);

    ret |= ads114s0x_wakeup(_device);

    /* Check adc communication */
    ads114s0x_reg_id_t id;
    ret |= ads114s0x_read_register(_device, ADS114S0X_REG_ID, 
        (uint8_t*)&id, sizeof(ads114s0x_reg_id_t));
    if (id.dev_id == ADS114S0X_DEV_ID_ADS114S08) {
        ESP_LOGI(TAG, "ADC device: ADS114S08");
    } else if (id.dev_id == ADS114S0X_DEV_ID_ADS114S06) {
        ESP_LOGI(TAG, "ADC device: ADS114S06");
    } else {
        ESP_LOGE(TAG, "ADC device not found");
    }

    /* If init failed, return now */
    if (ret != 0) {
        return ret;
    }

    /* Set default system value */
    ads114s0x_reg_sys_t sysReg;
    ret |= ads114s0x_read_register(_device, ADS114S0X_REG_SYS, (uint8_t*)&sysReg, sizeof(ads114s0x_reg_sys_t));
    sysReg.sendstat = 1; // Enable STATUS (data read)
    sysReg.crc = 0; // Disable CRC (data read)
    sysReg.sys_mon = 0b000; // Disable
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_SYS, (uint8_t*)&sysReg, sizeof(ads114s0x_reg_sys_t));

    /* Set default data rate to 20SPS */
    ads114s0x_reg_datarate_t datarateReg;
    ret |= ads114s0x_read_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));
    datarateReg.mode = 0; // Continous mode conversion for calibration --> then we will put it in single shot
    datarateReg.dr = ADS114S0X_DATA_RATE_20_SPS; // default to 20SPS
    datarateReg.clk = 0; // Internal clock
    datarateReg.g_chop = 0; // Global chop disable
    datarateReg.filter = 1; // Low latency filter
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));

    /* Data ready */
    _queue = xQueueCreate(10, 0);
    ret |= ads114s0x_add_data_ready_isr_handler(_device, _isr, NULL);
    
    // Calibration 
    ESP_LOGI(TAG, "Calibrating ADC...");
    ret |= ads114s0x_start(_device);
    ret |= ads114s0x_self_offset_calib(_device);
    vTaskDelay(pdMS_TO_TICKS(200));
    ret |= ads114s0x_stop(_device);
    xQueueReset(_queue);
    ESP_LOGI(TAG, "Calibrating ADC Done !");
    
    // Now that conversion is done, start in single shot
    datarateReg.mode = 0; // Single shot conversion
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));

    return ret;
}

int ADS114S0X::setDataRate(ads114s0x_data_rate_e dataRate) 
{
    int ret = 0;
    ads114s0x_reg_datarate_t datarateReg;
    ret |= ads114s0x_read_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));
    datarateReg.dr = dataRate;
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_DATARATE, (uint8_t*)&datarateReg, sizeof(ads114s0x_reg_datarate_t));
    return ret;
}

int ADS114S0X::setStabilizationTime(int t) 
{
    int ret = 0;
    _stabilizationTime = t;
    return ret;    
}

int ADS114S0X::setInternalMux(ads114s0x_adc_input_e inputP, ads114s0x_adc_input_e inputN)
{
    int ret = 0;
    ads114s0x_reg_inpmux_t inpmuxReg = {
        .muxn = inputN,
        .muxp = inputP
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_INPMUX, (uint8_t*)&inpmuxReg, sizeof(ads114s0x_reg_inpmux_t));
    return ret;
}

int ADS114S0X::setPGAGain(ads114s0x_pga_gain_e gain)
{
    int ret = 0;
    ads114s0x_reg_pga_t pgaReg = {
        .gain = gain,
        .pga_en = 1,
        .delay = ADS114S0X_DELAY_14_TMOD
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_PGA, (uint8_t*)&pgaReg, sizeof(ads114s0x_reg_pga_t));
    return ret;
}

int ADS114S0X::setReference(ads114s0x_ref_selection_e reference)
{
    int ret = 0;
    ads114s0x_reg_ref_t refReg = {
        .refcon = ADS114S0X_REF_ALWAYS_ON,      // Internal always on
        .refsel = reference,                    // Default ref
        .refn_buf = 0,                          // Enable
        .refp_buf = 0,                          // Enable
        .fl_ref_en = 1,                         // Enable reference monitor
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_REF, (uint8_t*)&refReg, sizeof(ads114s0x_reg_ref_t));
    return ret;
}

int ADS114S0X::setExcitation(ads114s0x_idac_magnitude_e current)
{
    int ret = 0;
    ads114s0x_reg_idac_t idacReg = {
        .imag = current,
        .reserved = 0,
        .psw = 0,
        .fl_rail_en = 0,
        .i1mux = (current==ADS114S0X_IDAC_OFF)?ADS114S0X_NOT_CONNECTED:ADS114S0X_AINCOM,
        .i2mux = ADS114S0X_NOT_CONNECTED
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_IDACMAG, (uint8_t*)&idacReg, sizeof(ads114s0x_reg_idac_t));
    return ret;
}

int ADS114S0X::setBias(ads114s0x_adc_input_e input)
{
    int ret = 0;
    ads114s0x_reg_vbias_t vbiasReg = {
        .vb_ain0 = (uint8_t)(input == 0),
        .vb_ain1 = (uint8_t)(input == 1),
        .vb_ain2 = (uint8_t)(input == 2),
        .vb_ain3 = (uint8_t)(input == 3),
        .vb_ain4 = (uint8_t)(input == 4),
        .vb_ain5 = (uint8_t)(input == 5),
        .vb_ain6 = (uint8_t)(input == 6),
        .vb_level = 0
    };
    ret |= ads114s0x_write_register(_device, ADS114S0X_REG_VBIAS, (uint8_t*)&vbiasReg, sizeof(ads114s0x_reg_vbias_t));
    return ret;
}

void ADS114S0X::waitStabilization() 
{
    vTaskDelay(_stabilizationTime);
}

int16_t ADS114S0X::read()
{
    int16_t ret = 0;
    int16_t adcCode;
    
    /* Start conversion */
    ret |= ads114s0x_start(_device);
     
    if (xQueueReceive(_queue, NULL, pdMS_TO_TICKS(1000)) == pdTRUE) {
       ret |= ads114s0x_read_data(_device, &adcCode);
    } else {
        ret = -1;
        ESP_LOGE(TAG, "ADC read timeout");
    }

    /* Return adcCode if ret is 0, else return ret */
    return (ret == 0)?adcCode:ret;
}
