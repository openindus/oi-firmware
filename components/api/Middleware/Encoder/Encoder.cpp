/**
 * @file Encoder.cpp
 * @brief Encoder class implementation
 * @author kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Encoder.h"

static const char TAG[] = "Encoder";

// A flag to identify if pcnt isr service has been installed.
static bool is_pcnt_isr_service_installed = false;
static int max_glitch_us                  = 1;

/**
 * @brief Encoder initialization
 *
 * @param A Digital input number A
 * @param B Digital input number B
 * @param ppr Pulse per revolution
 * @return int
 */
int Encoder::begin(DIn_Num_t A, DIn_Num_t B, int16_t ppr)
{
    ESP_LOGI(TAG, "Encoder initialization");

    // Save the pulse per revolution
    _ppr = ppr;

    // Configure channel 0
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = _dinGpioMap[A],
        .ctrl_gpio_num  = _dinGpioMap[B],
        .lctrl_mode     = PCNT_MODE_DISABLE,
        .hctrl_mode     = PCNT_MODE_REVERSE,
        .pos_mode       = PCNT_COUNT_INC,
        .neg_mode       = PCNT_COUNT_DEC,
        .counter_h_lim  = (int16_t)(_ppr),
        .counter_l_lim  = (int16_t)(-_ppr),
        .unit           = _pcntUnit,
        .channel        = PCNT_CHANNEL_0,
    };
    pcnt_unit_config(&pcnt_config);

    // PCNT pause and reset value
    pcnt_counter_pause(_pcntUnit);
    pcnt_counter_clear(_pcntUnit);

    // register interrupt handler in a thread-safe way
    if (!is_pcnt_isr_service_installed) {
        ESP_ERROR_CHECK(pcnt_isr_service_install(0));
        // make sure pcnt isr service won't be installed more than one time
        is_pcnt_isr_service_installed = true;
    }

    pcnt_isr_handler_add(_pcntUnit, _pcntIsrHandler, this);

    pcnt_event_enable(_pcntUnit, PCNT_EVT_H_LIM);
    pcnt_event_enable(_pcntUnit, PCNT_EVT_L_LIM);

    /* Configure and enable the input filter */
    ESP_ERROR_CHECK(pcnt_set_filter_value(_pcntUnit, max_glitch_us * 80));
    if (max_glitch_us) {
        pcnt_filter_enable(_pcntUnit);
    } else {
        pcnt_filter_disable(_pcntUnit);
    }

    // Start the PCNT unit
    pcnt_counter_resume(_pcntUnit);

    // Create a task to calculate the speed of the encoder
    xTaskCreate(_speedTask, "speedTask", 2048, this, 10, NULL);

    return 0;
}

/**
 * @brief Encoder end
 *
 */
void Encoder::end(void) 
{
    ESP_LOGI(TAG, "Encoder end");
    pcnt_isr_handler_remove(_pcntUnit);
}

/**
 * @brief Reset the number of pulses and revolutions
 *
 */
void Encoder::reset(void)
{
    _revolutionCnt = 0;
    pcnt_counter_clear(_pcntUnit);
}

/**
 * @brief Get the number of revolutions
 *
 * @return int number of revolutions
 */
int Encoder::getRevolutions(void)
{
    return _revolutionCnt;
}

/**
 * @brief Get the number of pulses
 *
 * @return int number of pulses
 */
int Encoder::getPulses(void)
{
    int16_t val = 0;
    pcnt_get_counter_value(_pcntUnit, &val);
    return (val + (_revolutionCnt * _ppr));
}

/**
 * @brief Get the angle
 *
 * @return float angle in radians
 */
float Encoder::getAngle(void)
{
    int16_t val = 0;
    pcnt_get_counter_value(_pcntUnit, &val);
    if (_ppr == 0)
        return NAN;
    return (float)(((float)val / (float)_ppr) * 2.0 * 3.141592); // 2 * PI
}

/**
 * @brief Get the speed
 *
 * @return float speed in pulses per second
 */
float Encoder::getSpeed(void)
{
    return _speed;
}

/**
 * @brief PCNT ISR handler
 *
 * @param arg Pointer to the encoder object
 */
void Encoder::_pcntIsrHandler(void *arg)
{
    Encoder* encoder = (Encoder*)arg;
    uint32_t status = 0;
    pcnt_get_event_status(encoder->_pcntUnit, &status);

    if (status & PCNT_EVT_H_LIM) {
        encoder->_revolutionCnt += 1;
    } else if (status & PCNT_EVT_L_LIM) {
        encoder->_revolutionCnt -= 1;
    }
}

/**
 * @brief Task to calculate the speed of the encoder
 *
 * @param pvParameter Pointer to the encoder object
 */
void Encoder::_speedTask(void *pvParameter)
{
    Encoder* encoder = (Encoder*)pvParameter;
    int16_t last_pulse_count = 0;
    int16_t current_pulse_count = 0;
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(100); // 100 ms

    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        pcnt_get_counter_value(encoder->_pcntUnit, &current_pulse_count);
        int16_t pulse_difference = current_pulse_count - last_pulse_count;

        // Adjust for overflow/underflow
        if (pulse_difference > encoder->_ppr / 2) {
            pulse_difference -= encoder->_ppr;
        } else if (pulse_difference < -encoder->_ppr / 2) {
            pulse_difference += encoder->_ppr;
        }

        last_pulse_count = current_pulse_count;

        // Calculate speed in pulses per second
        float speed = (float)pulse_difference / (float)pdTICKS_TO_MS(frequency) * 1000.0;
        encoder->_speed = speed;
    }
}