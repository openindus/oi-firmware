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

static const char TAG[] = "System";

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
    CoreStandalone::init();
    CoreMaster::init();
    CoreCLI::init();
#elif defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
    DiscreteStandalone::init();
    DiscreteSlave::init();
    DiscreteCLI::init();
#elif defined(OI_STEPPER) || defined(OI_STEPPER_VE)
    StepperStandalone::init();
    StepperSlave::init();
    StepperCLI::init();
    StepperParamCLI::init();
#elif defined(OI_MIXED)
    err |= MixedStandalone::init();
    err |= MixedSlave::init();
    err |= MixedCLI::init();
#elif defined(OI_RELAY_HP) || defined(OI_RELAY_LP)
    RelayStandalone::init();
    RelaySlave::init();
#elif defined(OI_BRUSHLESS)
    BrushlessStandalone::init();
    BrushlessSlave::init();
#elif defined(OI_ANALOG_LS)
    err |= AnalogLsStandalone::init();
    err |= AnalogLsSlave::init();
#endif
    ModuleCLI::init();
#if defined(MODULE_MASTER)
    ModuleMasterCLI::init();
#endif

    if (err != 0) {
        ESP_LOGE(TAG, "Failed to initialize module");
        ModuleStandalone::ledBlink(LED_RED, 250);
        UsbConsole::begin(true); // Force console to start, convenient for debugging
        return;
    } else {
        /* Module Initialized */
        ModuleStandalone::ledBlink(LED_BLUE, 1000);
    }

    /* Check reset reason */
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_POWERON) && 
        (reason != ESP_RST_SW) && 
        (reason != ESP_RST_UNKNOWN)) {
        ESP_LOGE(TAG, "Reset reason : %d", reason);
        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        UsbConsole::begin(true); // Force console to start, convenient for debugging
        return;
    }

#if defined(MODULE_SLAVE)

    UsbConsole::begin(true); // Force console on slave module
    return;

#else

    /* Start a task which listen for user to input "console" */
    UsbConsole::listen();
    
    /* Wait for slaves modules to init and give time to user script to enable console */
    vTaskDelay(200/portTICK_PERIOD_MS);

    /* On master module, call autoId */
#if defined(MODULE_MASTER)
    if (ModuleMaster::autoId()) {
        ModuleStandalone::ledBlink(LED_GREEN, 1000); // Paired
    } else {
        ModuleStandalone::ledBlink(LED_RED, 1000); // Paired error
        UsbConsole::begin(true); // Force console to start, convenient for debugging
        return;
    }
#endif

    if (!UsbConsole::begin()) { // console will start only if user input "console" during startup
        /* Start main task if console is not started  */
        ESP_LOGI(TAG, "Create main task");
        vTaskDelay(1);
        xTaskCreate(_mainTask, "Main task", 8192, NULL, 1, NULL);
#if defined(FORCE_CONSOLE)
        UsbConsole::begin(true); // Force console, will failed if Serial.begin() is called in user code
#endif
    }
    
#endif
}

extern "C" void app_main()
{
#if defined(ARDUINO)
    initArduino();
#endif
    System::init();
}
