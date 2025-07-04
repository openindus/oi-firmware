/**
 * @file System.cpp
 * @brief System
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "System.h"

#include "OpenIndus.h"
#include "Master.h"
#include "Slave.h"
#include "OSAL.h"
#include "UsbConsole.h"

#if defined(OI_CORE) && defined(CONFIG_TEST)
Core core;
Discrete discrete;
Mixed mixed;
RelayHP relay;
Stepper stepper;
#endif

static const char TAG[] = "System";

TaskHandle_t System::_mainTaskHandle = NULL;

int System::init(void)
{
    int err = 0;

    /*--- Harware init ---*/
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
    err |= Master::init();
#elif defined(MODULE_SLAVE)
    err |= Slave::init();
#endif

    if (err == 0) {
        LOGI(TAG, "Hardware initialized successfully");
        Module::ledBlink(LED_BLUE, 1000); // Hardware Initialized
    } else {
        LOGE(TAG, "Failed to initialize hardware");
        Module::ledBlink(LED_RED, 1000); // Error
        return ERROR_HARDWARE_INIT;
    }

    /*--- Master management ---*/
#if defined(MODULE_MASTER)
    delay(500);             // Wait for modules to be ready
    Master::resetModules(); // Reset all modules
    if (Master::autoId()) { // Auto ID modules
        LOGI(TAG, "Auto ID modules done");
        Module::ledBlink(LED_GREEN, 1000); // Auto pairing done
    } else {
        LOGE(TAG, "Failed to auto ID modules");
        Module::ledBlink(LED_RED, 1000); // Error
        return ERROR_AUTO_ID;
    }
#endif

    /*--- Check boot error ---*/
    if (!Module::checkBootError()) {
        LOGI(TAG, "No boot error detected");
    } else {
        LOGE(TAG, "Boot error detected");
        Module::ledBlink(LED_RED, 1000); // Error
        return ERROR_BOOT;
    }

    return ERROR_NONE;
}

void System::start(void)
{
    /*--- Start console / main task ---*/
#if defined(MODULE_MASTER)
    if (UsbConsole::checkUserActivation(500)) { // Check if user wants to activate console
        UsbConsole::begin();
    } else {
#if defined(FORCE_CONSOLE)
        UsbConsole::begin(); // Force start console
#endif
        _createMainTask();
    }
#elif defined(MODULE_SLAVE)
    UsbConsole::begin();
#if defined(FORCE_MAIN_TASK)
    _createMainTask(); // Force start main task
#endif
#else // Standalone mode
#if defined(FORCE_CONSOLE)
    UsbConsole::begin(); // Force start console
#endif
    _createMainTask();
#endif
}

void System::stop(void)
{
    if (_mainTaskHandle != NULL) {
        vTaskDelete(_mainTaskHandle);
        _mainTaskHandle = NULL;
    }

    /** @todo UsbConsole::end() */
}

__attribute__((weak)) void System::handleError(int errorCode)
{
    LOGE(TAG, "Error code : %d", errorCode);
#if defined(FORCE_CONSOLE)
    UsbConsole::begin(); // Force start console
#endif
#if defined(FORCE_MAIN_TASK)
    _createMainTask(); // Force start main task 
#endif
}

void System::_createMainTask(void)
{
    if (_mainTaskHandle == NULL) {
        BaseType_t ret = xTaskCreate(
            [](void *) {
                setup();
                while (1) {
                    loop();
                }
            },
            "Main task", 8192, NULL, 1, &_mainTaskHandle);
        if (ret != pdPASS) {
            LOGE(TAG, "Failed to create main task");
        } else {
            LOGI(TAG, "Main task started successfully");
        }
    }
}