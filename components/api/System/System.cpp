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
    int err = 0;

    /* Module init */
#if defined(OI_CORE)
    Core::init();
#elif defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
    Discrete::init();
#elif defined(OI_STEPPER) || defined(OI_STEPPER_VE)
    Stepper::init();
#elif defined(OI_MIXED)
    err |= MixedStandalone::init();
    err |= MixedSlave::init();
    err |= MixedCLI::init();
#elif defined(OI_RELAY_HP) || defined(OI_RELAY_LP)
    Relay::init();
#elif defined(OI_BRUSHLESS)
    Brushless::init();
#elif defined(OI_ANALOG_LS)
    Analogls::init();
#endif

    if (err != 0) {
        ESP_LOGE(SYSTEM_TAG, "Failed to initialize module");
        /** @todo: led blink quickly or something like that */
    }

    /* Register command Line Interface (CLI) commands */
    ConsoleModule::registerCli();
#if defined(OI_CORE)
    ConsoleCore::registerCli();
#elif defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
    ConsoleDiscrete::registerCli();
#elif defined(OI_STEPPER) || defined(OI_STEPPER_VE)
    ConsoleStepper::registerCommand();
    MotorStepperParamCLI::registerCommand();
#endif
#if defined(MODULE_MASTER)
    ConsoleMaster::registerCli();
#endif
    UsbConsole::listen(); // start a task which listen for user to input "console"

    /* Set module led to blue */
    MODULE_INITIALIZED();

    /* Wait for slaves modules to init and give time to user script to enable console */
#if !defined(MODULE_SLAVE)
    vTaskDelay(500/portTICK_PERIOD_MS);
#endif

    /* On master module, call autoId */
#if defined(MODULE_MASTER)
    if (ModuleMaster::autoId()) {
        MODULE_PAIRED();
    }
#endif

#if defined(MODULE_MASTER) || defined(MODULE_STANDALONE)
    /* Check reset reason */
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_POWERON) && 
        (reason != ESP_RST_SW) && 
        (reason != ESP_RST_UNKNOWN)) {
        ESP_LOGE(SYSTEM_TAG, "Reset reason : %d", reason);
        MODULE_ERROR();
        UsbConsole::begin(true); // Force console to start, convenient for debugging
    } else {
        if (!UsbConsole::begin()) { // console will start only if user input "console" during startup
            /* Start main task if console is not started  */
            ESP_LOGI(SYSTEM_TAG, "Create main task");
            vTaskDelay(1);
            xTaskCreate(_mainTask, "Main task", 8192, NULL, 1, NULL);
#if defined(CONFIG_FORCE_CONSOLE)
            UsbConsole::begin(true); // Force console, will failed if Serial.begin() is called in user code
#endif
        }
    }
#else // defined(MODULE_SLAVE)
    UsbConsole::begin(true); // Force console on slave module
#endif
}

extern "C" void app_main()
{
#if defined(ARDUINO)
    initArduino();
#endif
    System::init();
}
