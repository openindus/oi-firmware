/**
 * @file Module.cpp
 * @brief Module interface
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Module.h"
#include "ModulePinout.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static const char TAG[] = "Module";

uint16_t Module::_type = 0;
adc_oneshot_unit_handle_t Module::adcHandles[2] = {NULL, NULL};

int Module::init(uint16_t type)
{
    int err = 0;

    _type = type;

    /* Board */
    ESP_LOGI(TAG, "Board init.");
    err |= Board::init();

    /* Board info */
    err |= _initBoardInfos();

    /* LED */
    ESP_LOGI(TAG, "LED init.");
    Led::install(MODULE_GPIO_LED);
    Led::on(LED_BLUE);

    /* ADC */
    Module::_initADC();

    /* Bus */
#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_SLAVE) 
    ESP_LOGI(TAG, "Bus init");

    // Bus IO
    BusIO::Config_s config = {
        .adcChannelId = MODULE_ADC_CHANNEL_BUS_ID,
        .gpioNumSync = MODULE_GPIO_BUS_GPIO,
        .gpioModeSync = GPIO_MODE_INPUT_OUTPUT,
        .gpioNumPower = MODULE_GPIO_CMD_MOSFET_ALIM,
        .adcHandle = Module::adcHandles[0],
        .adcCaliHandle = NULL,
    };
#if defined(CONFIG_MODULE_MASTER)
    config.gpioModeSync = GPIO_MODE_INPUT_OUTPUT;
#elif defined(CONFIG_MODULE_SLAVE)
    config.gpioModeSync = GPIO_MODE_INPUT;
#endif
    err |= BusIO::init(&config);
#endif

    // Bus RS and CAN
    err |= BusRS::begin(MODULE_UART_RS_PORT, MODULE_GPIO_RS_UART_TX, MODULE_GPIO_RS_UART_RX);
    err |= BusCAN::begin(MODULE_GPIO_CAN_TX, MODULE_GPIO_CAN_RX);

    // Register CLI
    err |= Bus::init();

    return err;
}

int Module::_initBoardInfos(void)
{
    int err = 0;

    uint16_t local_type = getBoardType(); // read it only once to avoid multiple warning
    uint16_t local_variant = getHardwareVariant(); // read it only once to avoid multiple warning
    
    char local_name[16];
    ESP_LOGI(TAG, "Board type       : %u (%s)", local_type, BoardUtils::typeToName(local_type, local_name));
    ESP_LOGI(TAG, "Hardware variant : %u", local_variant);
    ESP_LOGI(TAG, "Serial number    : %d", getSerialNum());
    char str_date[13];
    time_t t = (time_t) getTimestamp();
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(str_date, sizeof(str_date), "(%d/%m/%Y)", &lt);
    ESP_LOGI(TAG, "Board timestamp  : %lli %s", getTimestamp(), str_date);
    char software_version[32];
    getSoftwareVersion(software_version);
    ESP_LOGI(TAG, "Software version : %s", software_version);

    if (local_type != _type) {
        // Hack because we cannot differentiate CONFIG_OI_CORE and CONFIG_OI_CORELITE for now...
        if (_type == TYPE_OI_CORE && local_type == TYPE_OI_CORELITE) {
            ESP_LOGI(TAG, "OICoreLite type checked");
        } else {
            char name[16];
            char local_name[16];
            ESP_LOGE(TAG, "Incorrect board type detected ! You have program the board as an %s and board is an %s", \
                BoardUtils::typeToName(_type, name), BoardUtils::typeToName(local_type, local_name));
            err |= -1; // Do not start the code because we are on wrong board
        }
    }

    return err;
}

void Module::_initADC(void)
{
    adc_oneshot_unit_init_cfg_t adcConfig = {};
    adcConfig.unit_id = ADC_UNIT_1;
    if (adc_oneshot_new_unit(&adcConfig, &Module::adcHandles[0]) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC unit 1");
    }
    adcConfig.unit_id = ADC_UNIT_2;
    if (adc_oneshot_new_unit(&adcConfig, &Module::adcHandles[1]) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC unit 2");
    }
}