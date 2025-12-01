/**
 * @file Relays.h
 * @brief Relays
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef enum {
    RELAY_1 = 0,
    RELAY_2,
    RELAY_3,
    RELAY_4,
    RELAY_5,
    RELAY_6,
    RELAY_MAX
} Relay_Num_t;

class Relays
{
protected:
    static int init(const gpio_num_t *gpio, int nb);

public:

    /**
     * @brief Set the output state of a relay
     * @param num Relay number (RELAY_1 to RELAY_MAX)
     * @param level Output level (0 for LOW/OFF, 1 for HIGH/ON)
     * @return 0 on success, -1 on error
     */
    static int digitalWrite(Relay_Num_t num, uint8_t level);

    /**
     * @brief Read the current state of a relay
     * @param num Relay number (RELAY_1 to RELAY_MAX)
     * @return Current relay state (0 for LOW/OFF, 1 for HIGH/ON), -1 on error
     */
    static int digitalRead(Relay_Num_t num);

    /**
     * @brief Toggle the output state of a relay
     * @param num Relay number (RELAY_1 to RELAY_MAX)
     * @return 0 on success, -1 on error
     */
    static int toggleOutput(Relay_Num_t num);

private:
    static uint8_t _nb; // number of relays
    static const gpio_num_t *_gpioNum;
    static uint8_t* _level;
    static SemaphoreHandle_t _mutex;

    static int _registerCLI(void);
};