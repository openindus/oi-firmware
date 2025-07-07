/**
 * @file Cloud.hpp
 * @brief Header file for the Cloud API component.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

class Cloud {
public:
    /**
     * @brief Initializes the Cloud API.
     * @return true if initialization is successful, false otherwise.
     */
    bool init(void);

    /**
     * @brief Connects to the cloud service.
     * @return true if connection is successful, false otherwise.
     */
    bool connect(void);

    /**
     * @brief Disconnects from the cloud service.
     */
    void disconnect(void);

    /**
     * @brief Checks if the device is connected to the cloud.
     * @return true if connected, false otherwise.
     */
    bool isConnected(void) const;
};