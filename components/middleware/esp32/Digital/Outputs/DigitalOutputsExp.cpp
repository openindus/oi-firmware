/**
 * @file DigitalOutputsExp.cpp
 * @brief Digital output expander
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsExp.h"
#include <string.h>

static const char TAG[] = "DigitalOutputsExp";

uint8_t DigitalOutputsExp::_nb;
ioex_num_t *DigitalOutputsExp::_ioex_num;
ioex_num_t *DigitalOutputsExp::_ioex_current;
ioex_device_t **DigitalOutputsExp::_ioex;
uint8_t *DigitalOutputsExp::_level;
SemaphoreHandle_t DigitalOutputsExp::_mutex;

int DigitalOutputsExp::init(ioex_device_t **ioex, const ioex_num_t *ioex_num,
                            const ioex_num_t *ioex_current, int nb)
{
    int err = 0;

    /* Save number of DOUT */
    _nb = nb;

    /* Save pointer to ioex */
    _ioex = ioex;

    /* Init memory and copy gpio numbers in _gpio_num table */
    _ioex_num = (ioex_num_t *)calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_num, ioex_num, nb * sizeof(ioex_num_t));

    /* Init memory and copy adc channels in _adc_current table */
    _ioex_current = (ioex_num_t *)calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_current, ioex_current, nb * sizeof(ioex_num_t));

    /* Init memory of _level */
    _level = (uint8_t *)calloc(nb, sizeof(uint8_t));

    /* Init DOUT */
    ESP_LOGI(TAG, "Init DOUT");
    ioex_config_t doutConf = {
        .pin_bit_mask   = 0,
        .mode           = IOEX_OUTPUT,
        .pull_mode      = IOEX_FLOATING,
        .interrupt_type = IOEX_INTERRUPT_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        doutConf.pin_bit_mask |= (1ULL << _ioex_num[i]);
        // /!\ Set level before setting to output
        err |= ioex_set_level(*_ioex, _ioex_num[i], IOEX_LOW);
    }
    err |= ioex_config(*_ioex, &doutConf);

    ESP_LOGI(TAG, "Init DOUT current");
    ioex_config_t doutSensorConf = {
        .pin_bit_mask   = 0,
        .mode           = IOEX_INPUT,
        .pull_mode      = IOEX_FLOATING,
        .interrupt_type = IOEX_INTERRUPT_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        doutSensorConf.pin_bit_mask |= (1ULL << _ioex_current[i]);
    }
    err |= ioex_config(*_ioex, &doutSensorConf);

    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    /* Create control task for overcurrent */
    ESP_LOGI(TAG, "Create control task");
    xTaskCreate(_controlTask, "Control task", 4096, NULL, 1, NULL);

    return err;
}

void DigitalOutputsExp::digitalWrite(DOut_Num_t num, bool level)
{
    if (num < _nb) {
        // Stor level
        xSemaphoreTake(_mutex, portMAX_DELAY);
        _level[num] = level;
        xSemaphoreGive(_mutex);
        // Set level
        ioex_set_level(*_ioex, _ioex_num[num], (ioex_level_t)level);
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
    }
}

void DigitalOutputsExp::toggleOutput(DOut_Num_t num)
{
    int level;
    if (num < _nb) {
        // Read level
        level = (ioex_get_level(*_ioex, _ioex_num[num]) == 1 ? 0 : 1);
        // Stor level
        xSemaphoreTake(_mutex, portMAX_DELAY);
        _level[num] = level;
        xSemaphoreGive(_mutex);
        // Write level
        digitalWrite(num, level);
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
    }
}

int DigitalOutputsExp::outputIsOvercurrent(DOut_Num_t num)
{
    if (num < _nb) {
        return ioex_get_level(*_ioex, _ioex_current[num]);
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
        return 0.0f;
    }
}

/**
 * @brief Every 500ms check if there is a power error on DOUT or
 * if output is in error: desactivate for 5 secondes then retry
 */
void DigitalOutputsExp::_controlTask(void *pvParameters)
{
    uint8_t *state;
    state = (uint8_t *)calloc(DigitalOutputsExp::_nb, sizeof(uint8_t));

    while (1) {
        /* Checking if DOUT is in overcurrent */
        for (int i = 0; i < DigitalOutputsExp::_nb; i++) {
            // If error happened
            if (DigitalOutputsExp::outputIsOvercurrent((DOut_Num_t)i) == 1) {
                ESP_LOGE(TAG, "Current on DOUT_%u is too high", i + 1);
                ioex_set_level(*(DigitalOutputsExp::_ioex), DigitalOutputsExp::_ioex_num[i],
                               IOEX_LOW);
                state[i] = 1;
            } else if (state[i] == 10) { // Retry after 10 loops
                state[i] = 0;
                // Set output at user choice (do not set HIGH if user setted this pin LOW during
                // error)
                xSemaphoreTake(_mutex, portMAX_DELAY);
                ioex_set_level(*(DigitalOutputsExp::_ioex), DigitalOutputsExp::_ioex_num[i],
                               (ioex_level_t)DigitalOutputsExp::_level[i]);
                xSemaphoreGive(_mutex);
            } else if (state[i] != 0) { // increase error counter to reach 10
                state[i]++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    free(state);
}