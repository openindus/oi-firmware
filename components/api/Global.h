/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Global.h
 * @brief OpenIndus common include
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

/* C/C++ - Standard library */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <array>
#include <cstring>

/* ESP Components */
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc_cal.h"
#include "esp_partition.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "esp_task_wdt.h"
#include "esp_ota_ops.h"
#include "esp_efuse.h"
#include "esp_intr_alloc.h"
#include "esp_console.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/uart.h"
#include "driver/twai.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "driver/adc.h"
#include "driver/usb_serial_jtag.h"
#include "driver/temp_sensor.h"
#include "soc/adc_channel.h"
#include "argtable3/argtable3.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "sdmmc_cmd.h"

/* OpenIndus Drivers */
#include "ads866x.h"
#include "dac8760.h"
#include "PS01.h"
#include "pcal6524.h"
#include "m41t62.h"
#include "STM32DriversComponents/M41T62.h"
#include "sc16is750.h"
#include "mcp25625.h"
#include "DriversComponents/MCP25625.h"
#include "stds75.h"
#include "cd74hc4052.h"
#include "ads114s0x.h"
#include "adc.h"