/**
 * @file MotorDc.cpp
 * @brief MotorDc class implementation
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDc.h"
#include "ads866x.h"
#include "drv8873.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_MAX           (8191) // Duty cycle max. ((2 ** 13) - 1)
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

static const char* TAG = "MotorDc";

std::vector<MotorDC_PinConfig_t> MotorDc::_motorsConfig;
gpio_num_t MotorDc::_faultPin;
std::vector<MotorDirection_t> MotorDc::_directions;

int MotorDc::init(std::vector<MotorDC_PinConfig_t> motorsConfig, gpio_num_t faultPin)
{    
    int err = 0;

    // Save config
    _faultPin = faultPin;
    _motorsConfig = motorsConfig;
    _directions.resize(_motorsConfig.size(), FORWARD);

    // Configure fault pin
    gpio_config_t input_conf;
    input_conf.pin_bit_mask = (1ULL << _faultPin);
    input_conf.mode = GPIO_MODE_INPUT;
    input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    input_conf.intr_type = GPIO_INTR_DISABLE;
    err |= gpio_config(&input_conf);

    // Configure LEDC timer to generate PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK,
        .deconfigure      = false // Do not deconfigure the timer
    };
    err |= ledc_timer_config(&ledc_timer);

    /* Configure all outputs */
    for(auto motorConfig = _motorsConfig.begin(); motorConfig != _motorsConfig.end(); motorConfig++) {
        // Configure disable pin
        gpio_config_t output_conf;
        output_conf.pin_bit_mask = (1ULL << motorConfig->disable);
        output_conf.mode = GPIO_MODE_OUTPUT;
        output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        output_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        output_conf.intr_type = GPIO_INTR_DISABLE;
        err |= gpio_config(&output_conf);

        // Configure LEDC PWM channel
        ledc_channel_config_t ledc_channel = {
            .gpio_num       = motorConfig->in1.gpio, // Placeholder, will be updated below
            .speed_mode     = LEDC_MODE,
            .channel        = motorConfig->in1.channel, // Placeholder, will be updated below
            .intr_type      = LEDC_INTR_DISABLE,
            .timer_sel      = LEDC_TIMER,
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0,
            .sleep_mode     = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
            .flags = {
            .output_invert = 0
            }
        };

        // Config IN1
        ledc_channel.gpio_num = motorConfig->in1.gpio;
        ledc_channel.channel = motorConfig->in1.channel;
        err |= ledc_channel_config(&ledc_channel);

        // Config IN2
        ledc_channel.gpio_num = motorConfig->in2.gpio;
        ledc_channel.channel = motorConfig->in2.channel;
        err |= ledc_channel_config(&ledc_channel);
    }

    // Initialize ADC for current reading
    initADC();

    // Initialize H-Bridge driver (DRV8873)
    initHBridge();

    /* CLI */
    _registerCLI();

    return err;
}

void MotorDc::run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle)
{
    // Check duty cycle
    if (dutyCycle < 0) dutyCycle = 0;
    if (dutyCycle > 100) dutyCycle = 100;

    // Enable motor
    gpio_set_level(_motorsConfig.at(motor).disable, 0);

    if (direction == FORWARD) {
        // IN1
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel, LEDC_DUTY_MAX);
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel);
        // IN2
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel, 
            (uint32_t)(((100 - dutyCycle) * LEDC_DUTY_MAX) / 100));
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel);
    } else {
        // IN1
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel, 
            (uint32_t)(((100 - dutyCycle) * LEDC_DUTY_MAX) / 100));
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel);
        // IN2
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel, LEDC_DUTY_MAX);
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel);
    }
    _directions.at(motor) = direction;
}

void MotorDc::stop(MotorNum_t motor)
{
    // IN1
    ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel, 0);
    ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel);

    // IN2
    ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel, 0);
    ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel);

    // Disable motor
    gpio_set_level(_motorsConfig.at(motor).disable, 1);
}

float MotorDc::getCurrent(MotorNum_t motor)
{
    ESP_LOGD(TAG, "Requested measure current for motor %d", motor);

    // Motor to ADC channel mapping table
    static const uint8_t motorToChannel[][2] = {{2, 3}, {0, 1}, {5, 4}, {6, 7}};

    if (motor < 0 || motor >= sizeof(motorToChannel)/sizeof(motorToChannel[0])) {
        ESP_LOGE(TAG, "Invalid motor number");
        return 0.0f;
    }

    // Check if _directions is properly initialized
    if (motor >= _directions.size()) {
        ESP_LOGW(TAG, "Motor %d direction not initialized, assuming FORWARD", motor);
        // Default to channel 0 (FORWARD)
        uint8_t channel = motorToChannel[motor][0];
        float sum = 0.0f;
        for (int j = 0; j < 1000; ++j) {
            float raw = ads866x_analog_read(channel);
            float voltage = ads866x_convert_raw_2_volt(raw, 6);
            sum += voltage;
        }
        float avg = sum / 1000.0f;
        float current = avg * 1100.0f / 430.0f;
        return current;
    }

    uint8_t channel = motorToChannel[motor][(_directions.at(motor) == FORWARD) ? 0 : 1];
    float sum = 0.0f;
    for (int j = 0; j < 1000; ++j) {
        float raw = ads866x_analog_read(channel);
        float voltage = ads866x_convert_raw_2_volt(raw, 6);
        sum += voltage;
    }
    float avg = sum / 1000.0f;

    // Convert voltage to current (I = U / R)
    float current = avg * 1100.0f / 430.0f; // k=1100, R=430Ohm

    ESP_LOGD(TAG, "Motor %d current: %.3f A", motor, current);

    return current;
}

void MotorDc::initSPI(void)
{
    static bool initialized = false;
    if(initialized){
        return;
    }

    /* Initialize the SPI bus */
    const spi_bus_config_t busConfig = {
        .mosi_io_num = GPIO_NUM_15,
        .miso_io_num = GPIO_NUM_11,
        .sclk_io_num = GPIO_NUM_13,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .data_io_default_level = false,
        .max_transfer_sz = 32,
        .flags = 0,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = 0
    };
    esp_err_t err = spi_bus_initialize(SPI2_HOST, &busConfig, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(err));
    } else if (err == ESP_OK) {
        initialized = true;
    }
}

void MotorDc::initADC(void)
{
    MotorDc::initSPI();

    esp_err_t err = ESP_OK;

    /* Initialize Analog Inputs */
    static ads866x_config_t adcSPIConfig = {
        .spi_host = SPI2_HOST,
        .spi_freq = SPI_MASTER_FREQ_8M,
        .spi_pin_cs = GPIO_NUM_1,
        .pin_rst = GPIO_NUM_17,
        .pin_alarm = GPIO_NUM_12,
        .adc_channel_nb = 8
    };
    err |= ads866x_init(&adcSPIConfig);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC: %d", err);
        return;
    }

    /* Configure analog inputs */
    for (size_t i = 0; i < 8; i++) {
        ads866x_set_channel_voltage_range(i, 6);
    }
}

void MotorDc::initHBridge(void)
{
    MotorDc::initSPI();

    esp_err_t err = ESP_OK;

    /* Configure the DRV8873 device */
    static drv8873_spi_config_t drv8873_cfg = {
        .spi_handle = NULL,
        .nSCS_pin = GPIO_NUM_48,  // Chip Select pin
    };

    // Initialize SPI bus and device
    err |= drv8873_spi_init(&drv8873_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize DRV8873 SPI: %d", err);
        return;
    }

    // Configure the H-Bridge in PWM mode
    err |= drv8873_set_mode(DRV8873_MODE_PWM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set DRV8873 mode: %d", err);
        return;
    }

    // Set current limit to 4 A (0x00)
    err |= drv8873_set_current_limit(DRV8873_ITRIP_LEVEL_4A);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set DRV8873 current limit: %d", err);
        return;
    }

    // Enable current regulation for both OUT1 and OUT2
    err |= drv8873_set_current_regulation(DRV8873_CURR_REG_ENABLED);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set DRV8873 current regulation: %d", err);
        return;
    }

    // Clear any existing faults
    err |= drv8873_clear_fault();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear DRV8873 faults: %d", err);
        return;
    }

    // Wait 10ms for stabilization
    vTaskDelay(pdMS_TO_TICKS(10));

    // Read and log fault status
    uint16_t fault_status;
    err |= drv8873_get_fault_status(&fault_status);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "DRV8873 Fault status: 0x%04X", fault_status);
    } else {
        ESP_LOGE(TAG, "Failed to read DRV8873 fault status: %d", err);
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "DRV8873 H-Bridge initialized successfully");
    }
}
