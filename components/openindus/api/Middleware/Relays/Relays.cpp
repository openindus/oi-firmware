/**
 * @file Relays.cpp
 * @brief Relays
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Relays.h"
#include "esp_log.h"

static const char TAG[] = "Relays";

uint8_t Relays::_nb;
const gpio_num_t* Relays::_gpioNum;
uint8_t* Relays::_level;
SemaphoreHandle_t Relays::_mutex;

int Relays::init(const gpio_num_t *gpio, int nb)
{
    int err = 0;

    /* Save number of DOUT */
    _nb = nb;

    /* Init memory and copy gpio numbers in _gpioNum table */
    _gpioNum = gpio;

    /* Init memory of _level */
    _level = (uint8_t*) calloc(nb, sizeof(uint8_t));

    gpio_config_t doutConf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        doutConf.pin_bit_mask |= (1ULL <<_gpioNum[i]);
    }
    err |= gpio_config(&doutConf);
    
    /* Create mutex */
    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    _registerCLI();

    return err;
}

int Relays::digitalWrite(Relay_Num_t num, uint8_t level)
{
    if (num >= _nb || num < 0)
    {
        ESP_LOGE(TAG, "Invalid RELAY_%d", num+1);
        return -1;
    }

    xSemaphoreTake(_mutex, portMAX_DELAY);
    _level[num] = level;
    xSemaphoreGive(_mutex);
    gpio_set_level(_gpioNum[num], level);

    return 0;
}

int Relays::digitalRead(Relay_Num_t num)
{
    if (num >= _nb || num < 0)
    {
        ESP_LOGE(TAG, "Invalid RELAY_%d", num+1);
        return -1;
    }
    
    return _level[num];
}

int Relays::toggleOutput(Relay_Num_t num)
{
    if (num >= _nb || num < 0)
    {
        ESP_LOGE(TAG, "Invalid RELAY_%d", num+1);
        return -1;
    }

    xSemaphoreTake(_mutex, portMAX_DELAY);
    _level[num] = !_level[num];
    xSemaphoreGive(_mutex);
    gpio_set_level(_gpioNum[num], _level[num]);

    return 0;
}
