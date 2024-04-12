/**
 * @file AnalogLsStandalone.cpp
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */
#include "AnalogLsStandalone.h"

#if defined(OI_ANALOG_LS)

static const char TAG[] = "AnalogLsStandalone";

int AnalogLsStandalone::init(void)
{
    ModuleStandalone::init();

    /* Initialize the SPI bus */
    spi_bus_config_t busConfig = {
        .mosi_io_num = ANALOG_LS_SPI_PIN_MOSI,
        .miso_io_num = ANALOG_LS_SPI_PIN_MISO,
        .sclk_io_num = ANALOG_LS_SPI_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 32,
        .flags = 0,
        .intr_flags = 0
    };

    esp_err_t err = spi_bus_initialize(ANALOG_LS_SPI_HOST, &busConfig, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus");
        return -1;
    }

    return 0;
}

#endif