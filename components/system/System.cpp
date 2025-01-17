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

#include "Global.h"
#ifndef LINUX_ARM
#include "CLI.h"
#include "UsbConsole.h"
#include "ControllerSlave.h"
#endif
#include "ControllerMaster.h"
#include "OpenIndus.h"

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
    err |= Core::init();
#elif defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
    err |= Discrete::init();
#elif defined(OI_STEPPER) || defined(OI_STEPPER_VE)
    err |= Stepper::init();
#elif defined(OI_MIXED)
    err |= Mixed::init();
#elif defined(OI_RELAY_HP)
    err |= RelayHP::init();
#elif defined(OI_ANALOG_LS)
    err |= AnalogLS::init();
#elif defined(OI_DC)
    err |= Dc::init();
#endif

    /* Controller init */
#if defined(MODULE_MASTER)
    err |= ControllerMaster::init();
#elif defined(MODULE_SLAVE)
    err |= ControllerSlave::init();
#endif

#ifndef LINUX_ARM
    /* Command line interface init */
    CLI::init();
#endif

    if (err != 0) {
#ifndef LINUX_ARM
        ESP_LOGE(TAG, "Failed to initialize module");
#endif
        Module::ledBlink(LED_RED, 250);

#ifndef LINUX_ARM
        UsbConsole::begin(true); // Force console to start, convenient for debugging
#endif
        return;
    } else {
        /* Module Initialized */
        Module::ledBlink(LED_BLUE, 1000);
    }

#ifndef LINUX_ARM
    /* Check reset reason */
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_POWERON) && 
        (reason != ESP_RST_SW) && 
        (reason != ESP_RST_UNKNOWN)) {
        ESP_LOGE(TAG, "Reset reason : %d", reason);
        Module::ledBlink(LED_RED, 1000); // Error
        UsbConsole::begin(true); // Force console to start, convenient for debugging
        return;
    }
#endif

#if defined(MODULE_SLAVE)

    UsbConsole::begin(true); // Force console on slave module
    return;

#else

#ifndef LINUX_ARM
    /* Start a task which listen for user to input "console" */
    UsbConsole::listen();
#endif

#ifndef LINUX_ARM
    /* Wait for slaves modules to init and give time to user script to enable console */
    vTaskDelay(500/portTICK_PERIOD_MS);
#endif

    /* On master module, call autoId */
#if defined(MODULE_MASTER)
    if (ControllerMaster::autoId()) {
        Module::ledBlink(LED_GREEN, 1000); // Paired
    } else {
        Module::ledBlink(LED_RED, 1000); // Paired error
#ifndef LINUX_ARM
        UsbConsole::begin(true); // Force console to start, convenient for debugging
#endif
        return;
    }
#endif

#ifndef LINUX_ARM
    if (!UsbConsole::begin()) { // console will start only if user input "console" during startup
        /* Start main task if console is not started  */
        ESP_LOGI(TAG, "Create main task");
        vTaskDelay(10);
        xTaskCreate(_mainTask, "Main task", 8192, NULL, 1, NULL);
#if defined(FORCE_CONSOLE)
        UsbConsole::begin(true); // Force console, will failed if Serial.begin() is called in user code
#endif
    }
#endif
    
#endif
}

#ifndef LINUX_ARM
extern "C" void app_main()
#else
int main(void)
#endif
{
#if defined(ARDUINO)
    initArduino();
#endif
    System::init();
#ifdef LINUX_ARM
    return 0;
#endif
}
