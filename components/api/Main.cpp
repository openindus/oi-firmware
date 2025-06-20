/**
 * @file Main.cpp
 * @brief Main entry point for the OpenIndus project.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "System.h"

extern "C" void app_main(void)
{
    int err = System::init();
    if (err == ERROR_NONE) {
        System::start();
    } else {
        System::handleError(err);
    }
}