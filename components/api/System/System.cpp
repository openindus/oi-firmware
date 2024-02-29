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

/* For testing purpose, it is possible to modify MODULEèTYPE without using menuconfig an re-compile everything */
#warning "These flags are for testing purpose only"
#undef  CONFIG_MODULE_SLAVE
#define CONFIG_MODULE_STANDALONE
#define CONFIG_FORCE_CONSOLE

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

    /* Register command Line Interface (CLI) commands */
    ConsoleModule::registerCli();
#if defined(CONFIG_CORE)
    ConsoleCore::registerCli();
#elif defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)
    ConsoleDiscrete::registerCli();
#elif defined(CONFIG_MIXED)
    ConsoleMixed::registerCli();
#elif defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)
    ConsoleStepper::registerCommand();
    MotorStepperParamCLI::registerCommand();
#endif
#if defined(CONFIG_MODULE_MASTER)
    ConsoleMaster::registerCli();
#endif
    UsbConsole::listen(); // start a task which listen for user to input "console"

    /* Set module led to blue */
    MODULE_INITIALIZED();

    /* Wait for slaves modules to init and give time to user script to enable console */
#if !defined(CONFIG_MODULE_SLAVE)
    vTaskDelay(500/portTICK_PERIOD_MS);
#endif

    /* On master module, call autoId */
#if defined(CONFIG_MODULE_MASTER)
    if (ModuleMaster::autoId()) {
        MODULE_PAIRED();
    }
#endif

#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_STANDALONE)
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
#else // defined(CONFIG_MODULE_SLAVE)
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
