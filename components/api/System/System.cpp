/**
 * @file System.cpp
 * @brief System main
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "System.h"
// #if defined(ARDUINO)
// #include "Arduino.h"
// #endif

#include "Global.h"
#ifndef LINUX_ARM
#include "SlaveController.h"
#include "UsbConsole.h"
#endif
#include "MasterController.h"
#include "OpenIndus.h"
#include "OSAL.h"

static const char TAG[] = "System";

void System::_mainTask(void *pvParameters)
{
    setup();
    while (1) {
        loop();
    }
}

int System::init(void)
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
    err |= MasterController::init();
#elif defined(MODULE_SLAVE)
    err |= SlaveController::init();
#endif

    return (err == 0);
}

void System::start(void)
{
    /* --- Initialize module --- */
    if (!System::init() || Board::checkBootError()) {
        LOGE(TAG, "Failed to initialize module");
        Module::ledBlink(LED_RED, 1000);
#if !defined(LINUX_ARM)
        UsbConsole::begin(true); // Force console to start, convenient for debugging
#endif
#if !defined(FORCED_START)
        return;
#endif
    } else {
        Module::ledBlink(LED_BLUE, 1000); // Module Initialized
    }

    /* --- Slave Module --- */
#if defined(MODULE_SLAVE)
#if !defined(LINUX_ARM)
    UsbConsole::begin(true); // Force console on slave module
#endif
#if !defined(FORCED_START)
    return;
#endif
#else
    UsbConsole::listen(); // Start a task which listen for user to input "console"
#endif 

    delay(500); // Wait for slaves modules to init and give time to user script to enable console

    /* --- Master Module --- */
#if defined(MODULE_MASTER)
    if (MasterController::autoId()) {
        Module::ledBlink(LED_GREEN, 1000); // Paired
    } else {
        Module::ledBlink(LED_RED, 1000); // Paired error
#if !defined(LINUX_ARM)
        UsbConsole::begin(true); // Force console to start, convenient for debugging
#endif
#if !defined(FORCED_START)
        return;
#endif
    }
#endif

    /* --- Main task --- */
#if !defined(LINUX_ARM)
    if (!UsbConsole::begin()) { // console will start only if user input "console" during startup
        ESP_LOGI(TAG, "Create main task");
        vTaskDelay(10);
        xTaskCreate(_mainTask, "Main task", 8192, NULL, 1, NULL);
#if defined(FORCE_CONSOLE)
        UsbConsole::begin(true); // Force console, will failed if Serial.begin() is called in user code
#endif
    }
#endif
}

#ifndef LINUX_ARM
extern "C" void app_main()
#else
int main(void)
#endif
{
// #if defined(ARDUINO)
//     initArduino();
// #endif
    System::start();
#ifdef LINUX_ARM
    return 0;
#endif
}
