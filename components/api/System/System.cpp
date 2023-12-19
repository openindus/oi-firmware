/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file System.cpp
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "System.h"
#if defined(ARDUINO)
#include "Arduino.h"
#endif

static const char SYSTEM_TAG[] = "System";

void System::_mainTask(void *pvParameters)
{
    setup();
    while(1) {
        loop();
    }
}

void System::init(void)
{
    /* Module init */
#if defined(CONFIG_CORE)
    Core::init();
#elif defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)
    Discrete::init();
#elif defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)
    Stepper::init();
#elif defined(CONFIG_MIXED)
    Mixed::init();
#elif defined(CONFIG_RELAY_HP) || defined(CONFIG_RELAY_LP)
    Relay::init();
#elif defined(CONFIG_BRUSHLESS)
    Brushless::init();
#elif defined(CONFIG_ANALOG_LS)
    Analogls::init();
#endif

    MODULE_INITIALIZED();

#if defined(CONFIG_CORE) && defined(CONFIG_AUTO_ID) && !defined(CONFIG_MODULE_STANDALONE)
    vTaskDelay(500/portTICK_PERIOD_MS);
    if (ModuleMaster::autoId()) {
        MODULE_PAIRED();
    }
#endif

    /* Check reset reason */
#if defined(CONFIG_CORE) || defined(CONFIG_MODULE_STANDALONE)
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_POWERON) && 
        (reason != ESP_RST_SW) && 
        (reason != ESP_RST_UNKNOWN)) {
        ESP_LOGE(SYSTEM_TAG, "Reset reason : %d", reason);
        MODULE_ERROR();
    } else {
        /* Main task */
        ESP_LOGI(SYSTEM_TAG, "Create main task");
        xTaskCreate(_mainTask, "Main task", 8192, NULL, 1, NULL);
    }
#endif

    /* Console */
    ConsoleModule::registerCli();
#if defined(CONFIG_CORE)
    ConsoleCore::registerCli();
#if !defined(CONFIG_MODULE_STANDALONE)
    ConsoleMaster::registerCli();
#endif
#elif defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)
    ConsoleDiscrete::registerCli();
#elif defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)
    ConsoleStepper::registerCli();
#endif
    UsbConsole::begin();
}

extern "C" void app_main()
{
#if defined(ARDUINO)
    initArduino();
#endif
    System::init();
}
