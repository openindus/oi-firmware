/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OISystem.h
 * @brief OpenIndus protocole messages
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <map>
#include <vector>
#include <algorithm>

#include "esp_log.h"

#include "OIModule.h"
#include "OIBus.h"
#include "OIFunction.h"

#ifdef CONFIG_OI_CORE
#define DISABLE_RS_TASK
#endif

class OISystem
{

public:

    /**
     * @brief Construct a new OISystem object
     * 
     */
    OISystem() {}

    /**
     * @brief Destroy the OISystem object
     * 
     */
    ~OISystem() {}

    /*  user functions */

    /**
     * @brief Initialize the system
     * 
     */
    void init(void);

    /**
     * @brief Initialize the bus and the current module, 
     * create task to manage messages.
     */
    void start(void);

    /**
     * @brief Delete bus task, deinitialize all modules.
     * 
     */
    void stop(void);

    /**
     * @brief Print the status of the system.
     * @todo
     */
    void status(void);

    /*  Module functions */

    /**
     * @brief Set the main Module object.
     * 
     * @param module Current module
     */
    void setModule(OIModuleMaster* module);

    /**
     * @brief Get the main Module object
     * 
     * @return OIModuleMaster* 
     */
    OIModuleMaster* getModule(void);

    /* SubModule fonctions */

    /**
     * @brief Set the Sub Module object
     * 
     * @param module Submodule to add in the system
     */
    void setSubModule(OIModuleSlave* module);

    /**
     * @brief Get the Sub Module object
     * 
     * @param id of module
     * @return OIModuleSlave* 
     */
    OIModuleSlave* getSubModule(uint8_t id);

    /**
     * @brief Enter download mode to update a board on the rail
     * 
     */
    void downloadMode();

    /**
     * @brief Set ID automatically
     * 
     */
    void automaticId(void);

    /**
     * @brief Set the Oi Id object
     * 
     * @param id 
     */
    inline void setOiId(uint32_t id) {
        _oiIdList.push_back(id);
    }

private:

 
    OIModuleMaster *_module;
    std::map<uint8_t, OIModuleSlave*> _submodules;

    static TaskHandle_t _busCanTaskHandle;
    static TaskHandle_t _busRsTaskHandle;

    static void _busRsTask(void *pvParameters);
    static void _busCanTask(void *pvParameters);

    std::vector<uint32_t> _oiIdList;
};

extern OISystem System;