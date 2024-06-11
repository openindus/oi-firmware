/**
 * @file AnalogLs.cpp
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogLs.h"

#if defined(OI_ANALOG_LS)

static const char TAG[] = "AnalogLs";

ads114s0x_device_t* _device;

ads114s0x_config_t _config = {
    .host_id = SPI2_HOST,
    .sclk_freq = SPI_MASTER_FREQ_8M/8,
    .start_sync = GPIO_NUM_NC,
    .reset = GPIO_NUM_NC,
    .cs = GPIO_NUM_47,
    .drdy = GPIO_NUM_NC
};

int AnalogLsStandalone::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "Init.");

    /* --- Module --- */
    ret |= ModuleStandalone::init(TYPE_OI_ANALOG_LS);

    /* --- ADC --- */

    ESP_LOGI(TAG, "ADC init.");

    /* Initialize the ADC */
    ads114s0x_init(&_device, &_config);

    ads114s0x_wakeup(_device);

    ads114s0x_reg_id_t id;
    ads114s0x_read_register(_device, ADS114S0X_REG_ID, (uint8_t*)&id, sizeof(ads114s0x_reg_id_t));
    
    if (id.dev_id == ADS114S0X_DEV_ID_ADS114S08) {
        ESP_LOGI(TAG, "device id: ADS114S08\n");
    } else if (id.dev_id == ADS114S0X_DEV_ID_ADS114S06) {
        ESP_LOGI(TAG, "device id: ADS114S06\n");
    } else {
        ESP_LOGI(TAG, "Undefined id\n");
    }

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

    // ...

    /* --- Digipot --- */

    /* --- Digital thermometer --- */

    return ret;
}

#endif