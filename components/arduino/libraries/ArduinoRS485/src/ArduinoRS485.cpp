/**
 * @file ArduinoRS485.cpp
 * @brief ArduinoRS485 library for OpenIndus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ArduinoRS485.h"

#if defined(CONFIG_OI_CORE)
    RS485Class RS485(Core::rs);
#else
    RS485Class RS485(RS(spi_host_device_t(SPI3_HOST), GPIO_NUM_5, GPIO_NUM_4));
#endif
