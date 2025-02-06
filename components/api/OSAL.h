/**
 * @file OSAL.h
 * @brief OS Abstraction Layer
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

// delay function
#if defined(LINUX_ARM)
#include <unistd.h>
#define delay(ms)  usleep(ms*1000)
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define delay(ms)  vTaskDelay(pdMS_TO_TICKS(ms))
#endif

// log functions
#if defined(LINUX_ARM)
#include <stdio.h>
#define LOGI(format, ...) printf("INFO: " format "\n", ##__VA_ARGS__)
#define LOGW(format, ...) printf("WARN: " format "\n", ##__VA_ARGS__)
#define LOGE(format, ...) printf("ERROR: " format "\n", ##__VA_ARGS__)
#else
#include "esp_log.h"
#define LOGI(tag, ...) ESP_LOGI(tag, ##__VA_ARGS__)
#define LOGW(tag, ...) ESP_LOGW(tag, ##__VA_ARGS__)
#define LOGE(tag, ...) ESP_LOGE(tag, ##__VA_ARGS__)
#endif