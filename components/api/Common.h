/**
 * @file Common.h
 * @brief OpenIndus API - Common header file
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
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
#include <list>
#include <algorithm>
#include <array>
#include <cstring>
#include <regex.h>

#ifndef LINUX_ARM

/* ESP Components */
#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

/* FreeRTOS Components */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#endif