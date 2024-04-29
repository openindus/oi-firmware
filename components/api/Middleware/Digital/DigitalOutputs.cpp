/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputs.c
 * @brief Functions for DOUT
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalOutputs.h"

static const char DOUT_TAG[] = "DigitalOutputs";
static SemaphoreHandle_t _mutex;

DigitalOutputs::DigitalOutputs(const gpio_num_t *gpio, const AdcNumChannel_t *adc, int num) 
{
    _type = DIGITAL_OUTPUT_GPIO;

    /* Save number of DOUT */
    _num = num;
    
    /* Init memory and copy gpio numbers in _gpio_num table */
    _gpio_num = (gpio_num_t*) calloc(num, sizeof(gpio_num_t));
    memcpy(_gpio_num, gpio, num * sizeof(gpio_num_t));
    
    /* Init memory and copy adc channels in _adc_current table */
    _adc_current = (AdcNumChannel_t*) calloc(num, sizeof(AdcNumChannel_t));
    memcpy(_adc_current, adc, num * sizeof(AdcNumChannel_t));

    /* Init memory of _doutLevel */
    _doutLevel = (uint8_t*) calloc(num, sizeof(uint8_t));
}

DigitalOutputs::DigitalOutputs(const gpio_num_t *gpio, const adc1_channel_t *adc, int num) 
{
    _type = DIGITAL_OUTPUT_GPIO;

    /* Save number of DOUT */
    _num = num;
    
    /* Init memory and copy gpio numbers in _gpio_num table */
    _gpio_num = (gpio_num_t*) calloc(num, sizeof(gpio_num_t));
    memcpy(_gpio_num, gpio, num * sizeof(gpio_num_t));
    
    /* Init memory and copy adc channels in _adc_current table */
    _adc_current = (AdcNumChannel_t*) calloc(num, sizeof(AdcNumChannel_t));
    for (int i = 0; i < num; i++) {
        _adc_current[i].adc_num = ADC_UNIT_1;
        _adc_current[i].channel = (adc_channel_t)adc[i];
    }

    /* Init memory of _doutLevel */
    _doutLevel = (uint8_t*) calloc(num, sizeof(uint8_t));
}

DigitalOutputs::DigitalOutputs(ioex_device_t **ioex, const ioex_num_t *ioex_num, const ioex_num_t *ioex_current, int num)
{
    _type = DIGITAL_OUTPUT_IOEX;

    /* Save number of DOUT */
    _num = num;

    /* Save pointer to ioex */
    _ioex = ioex;
    
    /* Init memory and copy gpio numbers in _gpio_num table */
    _ioex_num = (ioex_num_t*) calloc(num, sizeof(ioex_num_t));
    memcpy(_ioex_num, ioex_num, num * sizeof(ioex_num_t));
    
    /* Init memory and copy adc channels in _adc_current table */
    _ioex_current = (ioex_num_t*) calloc(num, sizeof(ioex_num_t));
    memcpy(_ioex_current, ioex_current, num * sizeof(ioex_num_t));

    /* Init memory of _doutLevel */
    _doutLevel = (uint8_t*) calloc(num, sizeof(uint8_t));
}

DigitalOutputs::~DigitalOutputs()
{
    if (_type == DIGITAL_OUTPUT_GPIO) {
        free(_gpio_num);
        free(_adc_current);
    } else { // DIGITAL_OUTPUT_IOEX
        free(_ioex_num);
        free(_ioex_current);
    }
    free(_doutLevel);
}

void DigitalOutputs::init()
{
    if (_type == DIGITAL_OUTPUT_GPIO) {
        /* Init DOUT */
        ESP_LOGI(DOUT_TAG, "Init DOUT");
        gpio_config_t doutConf = {
            .pin_bit_mask = 0,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        for (uint8_t i = 0; i < _num; i++) {
            doutConf.pin_bit_mask |= (1ULL <<_gpio_num[i]);
        }
        ESP_ERROR_CHECK(gpio_config(&doutConf));
        
        /* Init DOUT current */
        ESP_LOGI(DOUT_TAG, "Init DOUT current");
        ESP_ERROR_CHECK(adc1_config_width((adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT));
        for (uint8_t i = 0; i < _num; i++) {
            if (_adc_current[i].adc_num == ADC_UNIT_1) {
                ESP_ERROR_CHECK(adc1_config_channel_atten((adc1_channel_t)_adc_current[i].channel, ADC_ATTEN_DB_11));
            } else if (_adc_current[i].adc_num == ADC_UNIT_2) {
                ESP_ERROR_CHECK(adc2_config_channel_atten((adc2_channel_t)_adc_current[i].channel, ADC_ATTEN_DB_11));
            } else {
                ESP_LOGE(DOUT_TAG, "Invalid ADC channel");
            }
        }

        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, 1100, &_adc1Characteristics);
        for (uint8_t i = 0; i < _num; i++) {
            if (_adc_current[i].adc_num == ADC_UNIT_2) {
                esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, 1100, &_adc2Characteristics);
                break;
            }
        }
    } else { // DIGITAL_OUTPUT_IOEX
        /* Init DOUT */
        ESP_LOGI(DOUT_TAG, "Init DOUT");
        ioex_config_t doutConf = {
            .pin_bit_mask = 0,
            .mode = IOEX_OUTPUT,
            .pull_mode = IOEX_FLOATING,
            .interrupt_type = IOEX_INTERRUPT_DISABLE,
        };
        for (uint8_t i = 0; i < _num; i++) {
            doutConf.pin_bit_mask |= (1ULL <<_ioex_num[i]);
            // /!\ Set level before setting to output
            ESP_ERROR_CHECK(ioex_set_level(*_ioex, _ioex_num[i], IOEX_LOW));
        }
        ESP_ERROR_CHECK(ioex_config(*_ioex, &doutConf));

        ESP_LOGI(DOUT_TAG, "Init DOUT current");
        ioex_config_t doutSensorConf = {
            .pin_bit_mask = 0,
            .mode = IOEX_INPUT,
            .pull_mode = IOEX_FLOATING,
            .interrupt_type = IOEX_INTERRUPT_DISABLE,
        };
        for (uint8_t i = 0; i < _num; i++) {
            doutSensorConf.pin_bit_mask |= (1ULL <<_ioex_current[i]);
        }
        ESP_ERROR_CHECK(ioex_config(*_ioex, &doutSensorConf));
    }

    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    /* Create control task for overcurrent */
    ESP_LOGI(DOUT_TAG, "Create control task");
    xTaskCreate(_controlTask, "Control task", 4096, this, 1, NULL);
}

void DigitalOutputs::setLevel(DigitalOutputNum_t dout, uint8_t level)
{
    if (_type == DIGITAL_OUTPUT_GPIO) {
        gpio_set_level(_gpio_num[dout], level);
    } else { // DIGITAL_OUTPUT_IOEX
        ioex_set_level(*_ioex, _ioex_num[dout], (ioex_level_t) level);
    }
}

void DigitalOutputs::setAll(uint8_t level)
{
    /** @todo */
}

int DigitalOutputs::getLevel(DigitalOutputNum_t dout)
{
    if (_type == DIGITAL_OUTPUT_GPIO) {
        return gpio_get_level(_gpio_num[dout]);
    } else { // DIGITAL_OUTPUT_IOEX
        return ioex_get_level(*_ioex, _ioex_num[dout]);
    }
}

void DigitalOutputs::write(DigitalOutputNum_t dout, uint8_t level)
{
    if (dout < _num) {
        // Stor level 
        xSemaphoreTake(_mutex, portMAX_DELAY);
        _doutLevel[dout] = level;
        xSemaphoreGive(_mutex);
        // Set level
        setLevel(dout, level);
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", dout+1);
    }
}

void DigitalOutputs::toggle(DigitalOutputNum_t dout)
{
    int level;
    if (dout < _num) {
        // Read level
        level = (getLevel(dout) == 1 ? 0 : 1);
         // Stor level 
        xSemaphoreTake(_mutex, portMAX_DELAY);
        _doutLevel[dout] = level;
        xSemaphoreGive(_mutex);
        // Write level
        this->write(dout, level);
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", dout+1);
    }
}

void blink(DigitalOutputNum_t dout)
{
    /** @todo */
}

void DigitalOutputs::modePWM(DigitalOutputNum_t dout, uint32_t freq, ledc_timer_bit_t bit)
{
    if (dout < DOUT_MAX) {
        ledc_timer_config_t ledcTimer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = bit,
            .timer_num = LEDC_TIMER_1,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledcTimer));

        ledc_channel_config_t ledcChannel = {
            .gpio_num           = _gpio_num[dout],
            .speed_mode         = LEDC_LOW_SPEED_MODE,
            .channel            = (ledc_channel_t)(LEDC_CHANNEL_0 + dout),
            .intr_type          = LEDC_INTR_DISABLE,
            .timer_sel          = LEDC_TIMER_1,
            .duty               = 0,
            .hpoint             = 0,
            .flags {
                .output_invert  = 0
            }
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledcChannel));
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", dout+1);
    }
}

void DigitalOutputs::setDutyCycle(DigitalOutputNum_t dout, uint32_t duty)
{
    if (dout < _num) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", dout+1);
    }
}

float DigitalOutputs::digitalGetCurrent(DigitalOutputNum_t dout)
{
    if (dout < _num) {   
        if (_type == DIGITAL_OUTPUT_GPIO) {
            int current_reading = 0;
            int adc_reading = 0;
            float voltage = 0.0f;

            for (int i = 0; i < DOUT_SENSOR_ADC_NO_OF_SAMPLES; i++) {
                if (_adc_current[dout].adc_num == ADC_UNIT_1) {
                    current_reading = adc1_get_raw((adc1_channel_t)_adc_current[dout].channel);
                } else if (_adc_current[dout].adc_num == ADC_UNIT_2) {
                    adc2_get_raw((adc2_channel_t)_adc_current[dout].channel, (adc_bits_width_t)ADC_WIDTH_BIT_DEFAULT, &current_reading);                    
                } else {
                    current_reading = 0;
                    ESP_LOGE(DOUT_TAG, "Invalid ADC channel");
                }
                adc_reading += current_reading;
            }

            adc_reading /= DOUT_SENSOR_ADC_NO_OF_SAMPLES;

            // Convert adc_reading to voltage in mV
            if (_adc_current[dout].adc_num == ADC_UNIT_1) {
                voltage = static_cast<float> (esp_adc_cal_raw_to_voltage(adc_reading, &_adc1Characteristics));
            } else if (_adc_current[dout].adc_num == ADC_UNIT_2) {
                voltage = static_cast<float> (esp_adc_cal_raw_to_voltage(adc_reading, &_adc2Characteristics));
            } else {
                ESP_LOGE(DOUT_TAG, "Invalid ADC channel");
            }

            voltage /= 1000; // mV to V
            float sense_current = voltage / DOUT_SENSOR_RESISTOR_SENSE_VALUE; // I = U/R
            float current = 0.0f;

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
            ESP_LOGE(DOUT_TAG, "this function is not available on this device. For current sensor with digital reading, call 'int digitalGetOverCurrentStatus(DigitalOutputNum_t)' function");
            return 0.0f;
        }
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", dout+1);
        return 0.0f;
    }
}

int DigitalOutputs::digitalGetOverCurrentStatus(DigitalOutputNum_t dout)
{
    if (dout < _num) {
        if (_type == DIGITAL_OUTPUT_GPIO) {
            ESP_LOGW(DOUT_TAG, "For current sensor with adc reading, call 'digitalGetCurrent' function");
            return (digitalGetCurrent(dout) > 4.0f)?1:0;
        } else { // DIGITAL_OUTPUT_IOEX
            return ioex_get_level(*_ioex, _ioex_current[dout]);
        }
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", dout+1);
        return 0.0f;
    }
}

void DigitalOutputs::_controlTask(void *pvParameters)
{
    /* Every 500ms check if there is a power error on DOUT or
    If output is in error: desactivate for 5 secondes then retry */

    DigitalOutputs* dout = (DigitalOutputs*) pvParameters;
    uint8_t* dout_state;
    dout_state = (uint8_t*) calloc(dout->_num, sizeof(uint8_t));
    float currentSum = 0;
    float current;
    int currentSumState = 0;
    
    if (dout->_type == DIGITAL_OUTPUT_GPIO) {
        while(1) {
            /* Reset currentSum */
            currentSum = 0;
            /* Checking if individual DOUT is in overcurrent (> 4A) */
            for (uint8_t i = 0; i < dout->_num; i++) {
                // Read current
                current = dout->digitalGetCurrent((DigitalOutputNum_t) i);
                currentSum += current;
                // If error happened
                if (current > 4.0f) {
                    ESP_LOGE(DOUT_TAG, "Current on DOUT_%u is too high: %.2fA", i+1, current);
                    gpio_set_level(dout->_gpio_num[i], 0);
                    dout_state[i] = 1;
                } else if (dout_state[i] == 10) { // Retry after 10 loops
                    dout_state[i] = 0;
                    // Set output at user choice (do not set HIGH if user setted this pin LOW during error)
                    xSemaphoreTake(_mutex, portMAX_DELAY);
                    gpio_set_level(dout->_gpio_num[i] , dout->_doutLevel[i]);
                    xSemaphoreGive(_mutex);
                } else if (dout_state[i] != 0) { // increase error counter to reach 10
                    dout_state[i]++;
                }
            }

            if (currentSum > 8.0f) {
                currentSumState = (currentSumState+1)%1000;
            } else { 
                currentSumState--;
            }

            // Total current is above 8A for more than a minute
            if (currentSumState == 120) {
                ESP_LOGE(DOUT_TAG, "Total current is too high: %.2fA", currentSum);
                // Set all DOUT to 0;
                for (uint8_t i = 0; i < dout->_num; i++) {
                    gpio_set_level(dout->_gpio_num[i], 0);
                }
            } else if (currentSumState == -1) { // When off for two minute, reactivate outputs
                // Set all DOUT to wanted value;
                for (uint8_t i = 0; i < dout->_num; i++) {
                    xSemaphoreTake(_mutex, portMAX_DELAY);
                    gpio_set_level(dout->_gpio_num[i] , dout->_doutLevel[i]);
                    xSemaphoreGive(_mutex);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    } else  { // DIGITAL_OUTPUT_IOEX
        while(1) {
            /* Checking if DOUT is in overcurrent */
            for (int i = 0; i < dout->_num; i++) {
                // If error happened
                if (dout->digitalGetOverCurrentStatus((DigitalOutputNum_t)i) == 1) {
                    ESP_LOGE(DOUT_TAG, "Current on DOUT_%u is too high", i+1);
                    ioex_set_level(*(dout->_ioex), dout->_ioex_num[i], IOEX_LOW);
                    dout_state[i] = 1;
                } else if (dout_state[i] == 10) { // Retry after 10 loops
                    dout_state[i] = 0;
                    // Set output at user choice (do not set HIGH if user setted this pin LOW during error)
                    xSemaphoreTake(_mutex, portMAX_DELAY);
                    ioex_set_level(*(dout->_ioex), dout->_ioex_num[i], (ioex_level_t)dout->_doutLevel[i]);
                    xSemaphoreGive(_mutex);
                } else if (dout_state[i] != 0) { // increase error counter to reach 10
                    dout_state[i]++;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    free(dout_state);
}