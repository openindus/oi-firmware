/**
 * @file Relays.h
 * @brief Relays
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

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

    static int init(const gpio_num_t *gpio, int num);

public:

    static int digitalWrite(Relay_Num_t num, uint8_t level);
    static int digitalRead(Relay_Num_t num);
    static int toggleOutput(Relay_Num_t num);


private:

    /* Number of relay */
    static uint8_t _nb; 

    /* GPIOs */
    static const gpio_num_t *_gpio_num;

    
    static uint8_t* _level;
    static SemaphoreHandle_t _mutex;
};