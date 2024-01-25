/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file FlashLoader.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/md5_hash.h"

class FlashLoader
{
public:

    static void begin(void);
    static void write(uint8_t* data, size_t length);
    static void check(uint8_t md5Sum[16], size_t progSize);
    static void end(void);

};
