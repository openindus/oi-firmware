/**
 * @file DigitalOutputs.cpp
 * @brief Digital outputs
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputs.h"

#if !defined(OI_CORE)
#define DOUT_SENSOR_ADC_NO_OF_SAMPLES 64U
#define DOUT_SENSOR_RESISTOR_SENSE_VALUE 1200
#define DOUT_SENSOR_COEFF_BELOW_1A 1900
#define DOUT_SENSOR_VOLTAGE_BELOW_1A_mV 0.65f
#define DOUT_SENSOR_COEFF_BELOW_1_5A 1850
#define DOUT_SENSOR_VOLTAGE_BELOW_1_5A_mV 1.0f
#define DOUT_SENSOR_COEFF_BELOW_2A 1800
#define DOUT_SENSOR_VOLTAGE_BELOW_2A_mV 1.33f
#define DOUT_SENSOR_COEFF_ABOVE_2A 1750
#define DOUT_PWM_MAX_FREQUENCY_HZ 1000
#define DOUT_PWM_MIN_FREQUENCY_HZ 50
#endif

static const char TAG[] = "DigitalOutputs";

uint8_t DigitalOutputs::_nb;
DOut_Mode_t *DigitalOutputs::_mode;
bool *DigitalOutputs::_level;
#if defined(OI_CORE)
ioex_num_t *DigitalOutputs::_ioex_num;
ioex_num_t *DigitalOutputs::_ioex_current;
ioex_device_t **DigitalOutputs::_ioex;
#else
gpio_num_t *DigitalOutputs::_gpio_num;
adc_num_t *DigitalOutputs::_adc_current;
esp_adc_cal_characteristics_t DigitalOutputs::_adc1Characteristics;
esp_adc_cal_characteristics_t DigitalOutputs::_adc2Characteristics;
#endif
float DigitalOutputs::_overcurrentThreshold = 4.0f;
float DigitalOutputs::_overcurrentThresholdSum = 8.0f;
void (*DigitalOutputs::_overcurrentCallback)(void*) = NULL;
SemaphoreHandle_t DigitalOutputs::_mutex;

#if defined(OI_CORE)
int DigitalOutputs::init(ioex_device_t **ioex, const ioex_num_t *ioex_num, const ioex_num_t *ioex_current, int nb)
#else
int DigitalOutputs::init(const gpio_num_t *gpio, const adc_num_t *adc, int nb)
#endif
{
    int err = 0;
    _nb = nb;

    _mode = (DOut_Mode_t *)calloc(nb, sizeof(DOut_Mode_t)); // Initialize all outputs to digital mode
    if (_mode == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory");
    }
    memset(_mode, DOUT_MODE_DIGITAL, nb * sizeof(DOut_Mode_t));

    _level = (bool*)calloc(nb, sizeof(bool)); // Initialize output levels to LOW

    ESP_LOGI(TAG, "Init DigitalOutputs with %d outputs", nb);

#if defined(OI_CORE)
    _ioex = ioex;

    /* Init memory and copy gpio numbers in _gpio_num table */
    _ioex_num = (ioex_num_t *)calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_num, ioex_num, nb * sizeof(ioex_num_t));

    /* Init memory and copy adc channels in _adc_current table */
    _ioex_current = (ioex_num_t *)calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_current, ioex_current, nb * sizeof(ioex_num_t));

    /* Init DOUT */
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

#else
    /* Init memory and copy gpio numbers in _gpio_num table */
    _gpio_num = (gpio_num_t *)calloc(nb, sizeof(gpio_num_t));
    memcpy(_gpio_num, gpio, nb * sizeof(gpio_num_t));

    /* Init memory and copy adc channels in _adc_current table */
    _adc_current = (adc_num_t *)calloc(nb, sizeof(adc_num_t));
    memcpy(_adc_current, adc, nb * sizeof(adc_num_t));

    /* Init DOUT */
    gpio_config_t doutConf = {
        .pin_bit_mask = 0,
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        doutConf.pin_bit_mask |= (1ULL << _gpio_num[i]);
    }
    err |= gpio_config(&doutConf);

    /* Init DOUT current */
    err |= adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT);
    for (uint8_t i = 0; i < _nb; i++) {
        if (_adc_current[i].unit == ADC_UNIT_1) {
            err |= adc1_config_channel_atten((adc1_channel_t)_adc_current[i].channel, ADC_ATTEN_DB_11);
        } else if (_adc_current[i].unit == ADC_UNIT_2) {
            err |= adc2_config_channel_atten((adc2_channel_t)_adc_current[i].channel, ADC_ATTEN_DB_11);
        } else {
            ESP_LOGE(TAG, "Invalid ADC channel");
        }
    }

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, 1100, &_adc1Characteristics);
    for (uint8_t i = 0; i < _nb; i++) {
        if (_adc_current[i].unit == ADC_UNIT_2) {
            esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, 1100, &_adc2Characteristics);
            break;
        }
    }
#endif

    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    /* Create control task for overcurrent */
    ESP_LOGI(TAG, "Create control task");
    xTaskCreate(_controlTask, "Control task", 4096, NULL, 1, NULL);

    return err;
}

void DigitalOutputs::digitalWrite(DOut_Num_t num, bool level)
{
    if (num < _nb) {
        if (_mode[num] == DOUT_MODE_DIGITAL) {
            xSemaphoreTake(_mutex, portMAX_DELAY);
            _level[num] = level;
            xSemaphoreGive(_mutex);
#if defined(OI_CORE)
            ioex_set_level(*_ioex, _ioex_num[num], (ioex_level_t)level);
#else
            gpio_set_level(_gpio_num[num], level);
#endif
        } else {
            ESP_LOGE(TAG, "Invalid output mode");
        }
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
    }
}

void DigitalOutputs::toggleOutput(DOut_Num_t num)
{
    if (num < _nb) {
        if (_mode[num] == DOUT_MODE_DIGITAL) {
            xSemaphoreTake(_mutex, portMAX_DELAY);
            _level[num] = !_level[num];
            xSemaphoreGive(_mutex);
#if defined(OI_CORE)
            ioex_set_level(*_ioex, _ioex_num[num], (ioex_level_t)_level[num]);
#else
            gpio_set_level(_gpio_num[num], _level[num]);
#endif
        } else {
            ESP_LOGE(TAG, "Invalid output mode");
        }
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
    }
}

void DigitalOutputs::outputMode(DOut_Num_t num, DOut_Mode_t mode)
{
#if !defined(OI_CORE)
    if (num < _nb) {
        _mode[num] = mode;
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
    }
#else
    ESP_LOGW(TAG, "outputMode is not supported in OI_CORE");
#endif
}

void DigitalOutputs::setPWMFrequency(DOut_Num_t num, uint32_t freq)
{
#if !defined(OI_CORE)
    if (num < _nb) {
        if (_mode[num] == DOUT_MODE_PWM) {
            if (freq > DOUT_PWM_MAX_FREQUENCY_HZ) {
                ESP_LOGE(TAG, "To high frequency %d, max is %d", freq, DOUT_PWM_MAX_FREQUENCY_HZ);
                return;
            } else if (freq < DOUT_PWM_MIN_FREQUENCY_HZ) {
                ESP_LOGE(TAG, "To low frequency %d, min is %d", freq, DOUT_PWM_MIN_FREQUENCY_HZ);
                return;
            }

            ledc_timer_config_t ledcTimer = {
                .speed_mode      = LEDC_LOW_SPEED_MODE,
                .duty_resolution = LEDC_TIMER_14_BIT,
                .timer_num       = LEDC_TIMER_1,
                .freq_hz         = freq,
                .clk_cfg         = LEDC_AUTO_CLK,
            };
            ESP_ERROR_CHECK(ledc_timer_config(&ledcTimer));

            ledc_channel_config_t ledcChannel = {.gpio_num   = _gpio_num[num],
                                                 .speed_mode = LEDC_LOW_SPEED_MODE,
                                                 .channel = (ledc_channel_t)(LEDC_CHANNEL_0 + num),
                                                 .intr_type = LEDC_INTR_DISABLE,
                                                 .timer_sel = LEDC_TIMER_1,
                                                 .duty      = 0,
                                                 .hpoint    = 0,
                                                 .flags{.output_invert = 0}};
            ESP_ERROR_CHECK(ledc_channel_config(&ledcChannel));
        } else {
            ESP_LOGE(TAG, "Invalid output mode");
        }
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
        return;
    }
#else
    ESP_LOGW(TAG, "setPWMFrequency is not supported in OI_CORE");
#endif
}

void DigitalOutputs::setPWMDutyCycle(DOut_Num_t num, float duty)
{
#if !defined(OI_CORE)
    if (num < _nb) {
        if (_mode[num] == DOUT_MODE_PWM) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)(LEDC_CHANNEL_0 + num), (uint32_t)(duty * 16383.0f / 100.0f));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)(LEDC_CHANNEL_0 + num));
        } else {
            ESP_LOGE(TAG, "Invalid output mode");
        }
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
    }
#else
    ESP_LOGW(TAG, "setPWMDutyCycle is not supported in OI_CORE");
#endif
}

float DigitalOutputs::getOutputCurrent(DOut_Num_t num)
{
#if !defined(OI_CORE)
    return _adcReadCurrent(num);
#else
    ESP_LOGW(TAG, "getOutputCurrent is not supported in OI_CORE");
    return 0.0f;
#endif
}

int DigitalOutputs::outputIsOvercurrent(DOut_Num_t num)
{
    if (num < _nb) {
#if defined(OI_CORE)
        return ioex_get_level(*_ioex, _ioex_current[num]);
#else
        ESP_LOGW(TAG, "For current sensor with adc reading, call 'getOutputCurrent' function");
        return (_adcReadCurrent(num) > _overcurrentThreshold) ? 1 : 0;
#endif
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
        return 0.0f;
    }
}

#if !defined(OI_CORE)
float DigitalOutputs::_adcReadCurrent(DOut_Num_t num)
{
if (num < _nb) {
        int current_reading = 0;
        int adc_reading     = 0;
        float voltage       = 0.0f;

        for (int i = 0; i < DOUT_SENSOR_ADC_NO_OF_SAMPLES; i++) {
            if (_adc_current[num].unit == ADC_UNIT_1) {
                current_reading = adc1_get_raw((adc1_channel_t)_adc_current[num].channel);
            } else if (_adc_current[num].unit == ADC_UNIT_2) {
                adc2_get_raw((adc2_channel_t)_adc_current[num].channel, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, &current_reading);
            } else {
                current_reading = 0;
                ESP_LOGE(TAG, "Invalid ADC channel");
            }
            adc_reading += current_reading;
        }

        adc_reading /= DOUT_SENSOR_ADC_NO_OF_SAMPLES;

        // Convert adc_reading to voltage in mV
        if (_adc_current[num].unit == ADC_UNIT_1) {
            voltage = static_cast<float>(esp_adc_cal_raw_to_voltage(adc_reading, &_adc1Characteristics));
        } else if (_adc_current[num].unit == ADC_UNIT_2) {
            voltage = static_cast<float>(esp_adc_cal_raw_to_voltage(adc_reading, &_adc2Characteristics));
        } else {
            ESP_LOGE(TAG, "Invalid ADC channel");
        }

        voltage /= 1000;                                                  // mV to V
        float sense_current = voltage / DOUT_SENSOR_RESISTOR_SENSE_VALUE; // I = U/R
        float current       = 0.0f;

        if (voltage < DOUT_SENSOR_VOLTAGE_BELOW_1A_mV) {
            current = sense_current * DOUT_SENSOR_COEFF_BELOW_1A;
        } else if (voltage < DOUT_SENSOR_VOLTAGE_BELOW_1_5A_mV) {
            current = sense_current * DOUT_SENSOR_COEFF_BELOW_1_5A;
        } else if (voltage < DOUT_SENSOR_VOLTAGE_BELOW_2A_mV) {
            current = sense_current * DOUT_SENSOR_COEFF_BELOW_2A;
        } else {
            current = sense_current * DOUT_SENSOR_COEFF_ABOVE_2A;
        }

        return current;
    } else {
        ESP_LOGE(TAG, "Invalid DOUT_%d", num + 1);
        return 0.0f;
    }
}
#endif

/**
 * @brief Every 500ms check if there is a power error on DOUT or
 * if output is in error: desactivate for 5 secondes then retry
 */
#if defined(OI_CORE)
void DigitalOutputs::_controlTask(void *pvParameters)
{
    uint8_t *state = (uint8_t *)calloc(_nb, sizeof(uint8_t));

    while (1) {
        /* Checking if DOUT is in overcurrent */
        for (int i = 0; i < _nb; i++) {
            // If error happened
            if (ioex_get_level(*_ioex, _ioex_current[i]) == 1) {
                ESP_LOGE(TAG, "Current on DOUT_%u is too high", i + 1);
                ioex_set_level(*_ioex, _ioex_num[i], IOEX_LOW);
                state[i] = 1;
            } else if (state[i] == 10) { // Retry after 10 loops
                state[i] = 0;
                // Set output at user choice (do not set HIGH if user setted this pin LOW during error)
                xSemaphoreTake(_mutex, portMAX_DELAY);
                ioex_set_level(*_ioex, _ioex_num[i], (ioex_level_t)_level[i]);
                xSemaphoreGive(_mutex);
            } else if (state[i] != 0) { // increase error counter to reach 10
                state[i]++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    free(state);
}
#else
void DigitalOutputs::_controlTask(void *pvParameters)
{
    uint8_t *state           = (uint8_t *)calloc(_nb, sizeof(uint8_t));
    float currentSum         = 0.0f;
    float current            = 0.0f;
    int currentSumState      = 0;
    bool overcurrentDetected = false;

    while (1) {
        /* Reset currentSum */
        currentSum = 0;
        /* Checking if individual DOUT is in overcurrent (> 4A) */
        for (uint8_t i = 0; i < _nb; i++) {
            // Read current
            current = _adcReadCurrent((DOut_Num_t)i);
            currentSum += current;
            // If error happened
            if (current > _overcurrentThreshold) {
                ESP_LOGE(TAG, "Current on DOUT_%u is too high: %.2fA", i + 1, current);
                gpio_set_level(_gpio_num[i], 0);
                state[i] = 1;
                overcurrentDetected = true;
            } else if (state[i] == 10) { // Retry after 10 loops
                state[i] = 0;
                // Set output at user choice (do not set HIGH if user setted this pin LOW duringverror)
                xSemaphoreTake(_mutex, portMAX_DELAY);
                gpio_set_level(_gpio_num[i], _level[i]);
                xSemaphoreGive(_mutex);
            } else if (state[i] != 0) { // increase error counter to reach 10
                state[i]++;
            }
        }

        if (currentSum > _overcurrentThresholdSum) {
            currentSumState = (currentSumState + 1) % 1000;
            overcurrentDetected = true;
        } else {
            currentSumState--;
        }

        /* If overcurrent detected */
        if (overcurrentDetected) {
            /* Call user callback if set */
            if (_overcurrentCallback != NULL) {
                _overcurrentCallback(NULL);
            }
            overcurrentDetected = false;
        }

        // Total current is above 8A for more than a minute
        if (currentSumState == 120) {
            ESP_LOGE(TAG, "Total current is too high: %.2fA", currentSum);
            // Set all DOUT to 0;
            for (uint8_t i = 0; i < DigitalOutputs::_nb; i++) {
                gpio_set_level(DigitalOutputs::_gpio_num[i], 0);
            }
        } else if (currentSumState == -1) { // When off for two minute, reactivate outputs
            // Set all DOUT to wanted value;
            for (uint8_t i = 0; i < DigitalOutputs::_nb; i++) {
                xSemaphoreTake(_mutex, portMAX_DELAY);
                gpio_set_level(DigitalOutputs::_gpio_num[i], DigitalOutputs::_level[i]);
                xSemaphoreGive(_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    free(state);
}
#endif