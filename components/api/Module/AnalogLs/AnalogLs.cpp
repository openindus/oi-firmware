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
    .host_id = ANALOG_LS_SPI_HOST,
    .sclk_freq = ANALOG_LS_ADC_SPI_FREQ/8,
    .start_sync = ANALOG_LS_ADC_PIN_START_SYNC,
    .reset = ANALOG_LS_ADC_PIN_RESET,
    .cs = ANALOG_LS_ADC_PIN_CS,
    .drdy = ANALOG_LS_ADC_PIN_DRDY
};

int AnalogLsStandalone::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "Init.");

    /* --- Module --- */
    ret |= ModuleStandalone::init(TYPE_OI_ANALOG_LS);

    /* --- ADC --- */

    ESP_LOGI(TAG, "ADC init.");

    /* Initialize SPI bus */
    spi_bus_config_t spiConfig = {
        .mosi_io_num = ANALOG_LS_SPI_PIN_MOSI,
        .miso_io_num = ANALOG_LS_SPI_PIN_MISO,
        .sclk_io_num = ANALOG_LS_SPI_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0
    };
    ret |= spi_bus_initialize(ANALOG_LS_SPI_HOST, &spiConfig, SPI_DMA_CH_AUTO);

    /* Initialize ADC device */
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