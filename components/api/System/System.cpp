/**
 * @file System.cpp
 * @brief System main
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "System.h"

#include "Master.h"
#include "OSAL.h"
#include "OpenIndus.h"
#include "Slave.h"
#include "UsbConsole.h"

static const char TAG[] = "System";

TaskHandle_t System::_mainTaskHandle = NULL;

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

#if defined(MODULE_MASTER)
    err |= Master::init(); // Initialize master
#elif defined(MODULE_SLAVE)
    err |= Slave::init(); // Initialize slave
#endif

    return (err == 0);
}

void System::start(void)
{
    if (_mainTaskHandle == NULL) {
        xTaskCreate(_mainTask, "Main task", 8192, NULL, 1, &_mainTaskHandle);
    }
}

__attribute__((weak)) void System::handleError(int errorCode)
{
    UsbConsole::begin(); // Start console
#if defined(FORCE_START)
    System::start(); // Start main task
#endif
}

void System::stop(void)
{
    if (_mainTaskHandle != NULL) {
        vTaskDelete(_mainTaskHandle);
        _mainTaskHandle = NULL;
    }
}

extern "C" void app_main(void)
{
    /*--- Boot ---*/
    if (Module::checkBootError()) {
        LOGE(TAG, "Boot error detected");
        Module::ledBlink(LED_RED, 1000); // Error
        System::handleError(ERROR_BOOT);
        return;
    }

    /*--- Harware init ---*/
    if (System::init()) {                 // Initialize modules
        Module::ledBlink(LED_BLUE, 1000); // Module Initialized
    } else {
        LOGE(TAG, "Failed to initialize module");
        Module::ledBlink(LED_RED, 1000); // Error
        System::handleError(ERROR_MODULE_INIT);
        return;
    }

    /*--- Master management ---*/
#if defined(MODULE_MASTER)
    delay(500);                            // Wait for modules to be ready
    Master::resetModules();                // Reset all modules
    if (Master::autoId()) {                // Auto ID modules
        Module::ledBlink(LED_GREEN, 1000); // Auto pairing done
    } else {
        LOGE(TAG, "Failed to auto ID modules");
        Module::ledBlink(LED_RED, 1000); // Error
        System::handleError(ERROR_AUTO_ID);
        return;
    }
#endif

    /*--- Console and main task ---*/
#if defined(MODULE_MASTER)
    if (UsbConsole::checkUserActivation(500)) { // Check if user wants to activate console
        UsbConsole::begin();                    // Start console
    } else {
#if defined(FORCE_CONSOLE)
        UsbConsole::begin(); // Start console
#endif
        System::start(); // Start main task
    }
#elif defined(MODULE_SLAVE)
    UsbConsole::begin(); // Start console
#else
    System::start(); // Start main task
#endif
}