#include "PS01_Hal.h"
#include "PS01_Cmd.h"

static const char PS01_TAG[] = "powerSTEP01";

/* Private variables */
static bool _deviceConfigured = false;
static bool _spiInitialized = false;
static bool _pwmInitialized[NUMBER_OF_DEVICES] = {false, false};
static bool _gpioInitialized[NUMBER_OF_DEVICES] = {false, false};
static uint8_t _deviceId[NUMBER_OF_DEVICES] = {0, 1};

static SemaphoreHandle_t _mutex;
static spi_device_handle_t _spiHandler = NULL;
static PS01_Hal_Config_t _deviceConfig;

static xQueueHandle _flagEvent[NUMBER_OF_DEVICES];
static xQueueHandle _busyEvent[NUMBER_OF_DEVICES];

static void (*_busyCallback)(uint8_t) = NULL;

void IRAM_ATTR _flagIsr(void *arg)
{
    uint8_t deviceId = *(uint8_t*)arg;
    xQueueSendFromISR(_flagEvent[deviceId], NULL, NULL);
}

void _flagTask(void* arg)
{
    uint16_t status;
    uint8_t deviceId = *(uint8_t*)arg;

    while(1) {
        if(xQueueReceive(_flagEvent[deviceId], NULL, portMAX_DELAY)) {
            gpio_intr_disable(_deviceConfig.pin_flag[deviceId]);
            status = PS01_Cmd_GetStatus(deviceId);
            if (((status & POWERSTEP01_STATUS_CMD_ERROR) >> 7) == 1) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Command error", deviceId);
            }
            if (((status & POWERSTEP01_STATUS_UVLO) >> 9) == 0) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Undervoltage lockout (UVLO)", deviceId);
            }
            if (((status & POWERSTEP01_STATUS_UVLO_ADC) >> 10) == 0) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : VS undervoltage lockout (UVLO_ADC)", deviceId);
            }
            if (((status & POWERSTEP01_STATUS_TH_STATUS) >> 11) == 0b01) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Thermal warning", deviceId);
            }
            if (((((status & POWERSTEP01_STATUS_TH_STATUS) >> 11) == 0b10) ||
                ((status & POWERSTEP01_STATUS_TH_STATUS) >> 11) == 0b11)) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Thermal shutdown", deviceId);
            }
            if (((status & POWERSTEP01_STATUS_OCD) >> 13) == 0) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Overcurrent detection", deviceId);
            }
            if (((status & POWERSTEP01_STATUS_STALL_A) >> 14) == 0) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Stall detection (A)", deviceId);
            }
            if (((status & POWERSTEP01_STATUS_STALL_B) >> 15) == 0) {
                ESP_LOGW(PS01_TAG, "Flag event (%d) : Stall detection (B)", deviceId);
            }
            gpio_intr_enable(_deviceConfig.pin_flag[deviceId]);
        }
    }
}

void IRAM_ATTR _busyIsr(void* arg)
{
    uint8_t deviceId = *(uint8_t*)arg;
    xQueueSendFromISR(_busyEvent[deviceId], NULL, NULL);
}

void _busyTask(void* arg)
{
    uint8_t deviceId = *(uint8_t*)arg;

    while(1) {
        if(xQueueReceive(_busyEvent[deviceId], NULL, portMAX_DELAY)) {
            gpio_intr_disable(_deviceConfig.pin_busy_sync[deviceId]);
            if (_busyCallback != NULL) {
                _busyCallback(deviceId); 
            }
            gpio_intr_enable(_deviceConfig.pin_busy_sync[deviceId]);
        }
    }
}

/**
 * @brief Attach a busy interrupt for the given motor
 * 
 * @param deviceId 
 * @param callback 
 */
void PS01_Hal_AttachBusyInterrupt(void(*callback)(uint8_t))
{
    _busyCallback = callback;
}

/**
 * @brief Detach busy interrupt callback for the given motor
 * 
 * @param deviceId 
 */
void PS01_Hal_DetachBusyInterrupt(void)
{
    _busyCallback = NULL;
}


/**
 * @brief This function provides an accurate delay in milliseconds
 * @param[in] delay  time length in milliseconds
 * @retval None
 */
void PS01_Hal_Delay(uint32_t delay)
{
    ESP_LOGV(PS01_TAG, "Delay: %ums", delay);
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

/**
 * @brief This function enable the interruptions
 * @retval None
 */
void PS01_Hal_EnableIrq(void)
{
    ESP_LOGV(PS01_TAG, "Enable Irq");
    //TODO
}

/**
 * @brief This function disable the interruptions
 * @retval None
 */
void PS01_Hal_DisableIrq(void)
{
    ESP_LOGV(PS01_TAG, "Disable Irq");
    //TODO
}

/**
 * @brief  Initializes the GPIOs used by the powerSTEP01s
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void PS01_Hal_GpioInit(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Initializes the GPIOs");

    if (deviceId < _deviceConfig.num_of_devices) {
        if (_deviceConfigured == true) {
            gpio_config_t cfg;
            
            ESP_LOGI(PS01_TAG, "Initializes the device%u's GPIOs", deviceId+1);
            ESP_LOGI(PS01_TAG, "SW: io%d | STBY/RST: io%d | BUSY/SYNC: io%d | FLAG: io%d", 
                _deviceConfig.pin_sw[deviceId], _deviceConfig.pin_stby_rst[deviceId], 
                _deviceConfig.pin_busy_sync[deviceId], _deviceConfig.pin_flag[deviceId]);

            /* SW, STBY/RST */
            cfg.pin_bit_mask = ((1ULL << _deviceConfig.pin_sw[deviceId]) |
                                (1ULL << _deviceConfig.pin_stby_rst[deviceId]));
            cfg.mode = GPIO_MODE_OUTPUT;
            cfg.pull_up_en = GPIO_PULLUP_DISABLE;
            cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
            cfg.intr_type = GPIO_INTR_DISABLE;

            ESP_ERROR_CHECK(gpio_config(&cfg));

            /* BUSY/SYNC */      
            cfg.pin_bit_mask = (1ULL << _deviceConfig.pin_busy_sync[deviceId]);
            cfg.mode = GPIO_MODE_INPUT;
            cfg.pull_up_en = GPIO_PULLUP_ENABLE;
            cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
            cfg.intr_type = GPIO_INTR_POSEDGE;

            ESP_ERROR_CHECK(gpio_config(&cfg));

            /* Flag */             
            cfg.pin_bit_mask = (1ULL << _deviceConfig.pin_flag[deviceId]);
            cfg.mode = GPIO_MODE_INPUT;
            cfg.pull_up_en = GPIO_PULLUP_ENABLE;
            cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
            cfg.intr_type = GPIO_INTR_NEGEDGE;

            ESP_ERROR_CHECK(gpio_config(&cfg));

            _gpioInitialized[deviceId] = true;

        } else {
            ESP_LOGE(PS01_TAG, "Device is not configured !!!");
        }
    }
}

/**
 * @brief Initialises the step clock by setting
 * corresponding GPIO, Timer, Pwm, ...
 * @retval None
 */
void PS01_Hal_StepClockInit(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Initialises the step clock");

    if (_pwmInitialized[deviceId] == false) {
        if (_deviceConfigured == true) {
            ESP_LOGI(PS01_TAG, "Initializes the step clock");
            
            ledc_timer_config_t pwm_timer = {
                .speed_mode         = LEDC_LOW_SPEED_MODE,
                .duty_resolution    = LEDC_TIMER_8_BIT,
                .timer_num          = _deviceConfig.pwm_timer[deviceId],
                .freq_hz            = 10000,
                .clk_cfg            = LEDC_AUTO_CLK,
            };

            /* Timer config */
            ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

            _pwmInitialized[deviceId] = true;

        } else {
            ESP_LOGE(PS01_TAG, "Device is not configured !!!");
        }
    }
}

/**
 * @brief Start the step clock by using the given frequency
 * @param[in] newFreq in Hz of the step clock
 * @retval None
 * @note The frequency is directly the current speed of the device
 */
void PS01_Hal_StartStepClock(uint8_t deviceId, uint16_t newFreq)
{
    ESP_LOGV(PS01_TAG, "Start the step clock");

    if (_pwmInitialized[deviceId] == true) {
        /* Set frequency */
        ESP_ERROR_CHECK(ledc_set_freq(LEDC_LOW_SPEED_MODE, _deviceConfig.pwm_timer[deviceId], newFreq));

        ledc_channel_config_t pwm_channel = {
            .gpio_num   = _deviceConfig.pwm_pin_stck[deviceId],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel    = _deviceConfig.pwm_channel[deviceId],
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = _deviceConfig.pwm_timer[deviceId],
            .duty       = 127, // (((2^8)/2) - 1)
            .hpoint     = 0,
        };

        /* Channel config */
        ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));            
    } else {
        ESP_LOGE(PS01_TAG, "Step clock is not initialized !!!");
    }
}

/**
 * @brief Stops the PWM uses for the step clock
 * @retval None
 */
void PS01_Hal_StopStepClock(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Stop the step clock");

    if (_pwmInitialized[deviceId] == true) {
        ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, _deviceConfig.pwm_channel[deviceId], 0));
    } else {
        ESP_LOGE(PS01_TAG, "Step clock is not initialized !!!");
    }
}

/**
 * @brief Releases the powerSTEP01 reset (pin set to High) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void PS01_Hal_ReleaseReset(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Releases the powerSTEP01 reset");

    if (_gpioInitialized[deviceId] == true) {
        ESP_LOGI(PS01_TAG, "Releases device%u reset", deviceId);
        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig.pin_stby_rst[deviceId], 1));
    }
    else {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
}

/**
 * @brief Resets the powerSTEP01 (reset pin set to low) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void PS01_Hal_Reset(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Resets the powerSTEP01");

    if (_gpioInitialized[deviceId] == true) {
        ESP_LOGI(PS01_TAG, "Resets device%u", deviceId);
        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig.pin_stby_rst[deviceId], 0));
    } else {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
}

/**
 * @brief Initializes the SPI used by powerSTEP01
 * @retval HAL_OK if SPI transaction is OK, HAL_KO else
 */
uint8_t PS01_Hal_SpiInit(void)
{
    ESP_LOGV(PS01_TAG, "Initializes the SPI used by powerSTEP01");

    if (_spiInitialized == false) {
        if (_deviceConfigured == true) {
            ESP_LOGI(PS01_TAG, "Initializes the bus SPI%u", _deviceConfig.spi_host+1);
            ESP_LOGI(PS01_TAG, "MOSI: io%d | MISO: io%d | CLK: io%d | CS: io%d", 
                _deviceConfig.spi_pin_mosi, _deviceConfig.spi_pin_miso, _deviceConfig.spi_pin_clk, _deviceConfig.spi_pin_cs);

            spi_bus_config_t busConfig = {
                .mosi_io_num = _deviceConfig.spi_pin_mosi,
                .miso_io_num = _deviceConfig.spi_pin_miso,
                .sclk_io_num = _deviceConfig.spi_pin_clk,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = 32,
                .flags = 0,
                .intr_flags = 0
            };

            ESP_ERROR_CHECK(spi_bus_initialize(_deviceConfig.spi_host, &busConfig, SPI_DMA_CH_AUTO));

            spi_device_interface_config_t devConfig = {
                .command_bits = 0,
                .address_bits = 0,
                .dummy_bits = 0,
                .mode = 3,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 3,
                .cs_ena_posttrans = 3,
                .clock_speed_hz = _deviceConfig.spi_freq,
                .input_delay_ns = 0,
                .spics_io_num = _deviceConfig.spi_pin_cs,
                .flags = 0,
                .queue_size = 1,
                .pre_cb = NULL,
                .post_cb = NULL
            };

            ESP_ERROR_CHECK(spi_bus_add_device(_deviceConfig.spi_host, &devConfig, &_spiHandler));

            _mutex = xSemaphoreCreateMutex();
            xSemaphoreGive(_mutex);

            _spiInitialized = true;
        } else {
            ESP_LOGE(PS01_TAG, "Device is not configured !!!");
        }
    }
    return 0;
}

/**
 * @brief Write and read SPI byte to the powerSTEP01
 * @param[in] pByteToTransmit pointer to the byte to transmit
 * @param[in] pReceivedByte pointer to the received byte
 * @retval 0 if SPI transaction is OK, 0xFF else
 */
uint8_t PS01_Hal_SpiWriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte)
{
    ESP_LOGV(PS01_TAG, "Write and read SPI byte to the powerSTEP01");

    if (_spiInitialized == true) {
        xSemaphoreTake(_mutex, portMAX_DELAY);

        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 8,
            .rxlength = 8,
            .user = NULL,
            .tx_buffer = NULL,
            .rx_buffer = NULL
        };

        trans.length = (8 * POWERSTEP01_NUMBER_OF_DEVICES);
        trans.rxlength = (8 * POWERSTEP01_NUMBER_OF_DEVICES);
        trans.tx_buffer = pByteToTransmit;
        trans.rx_buffer = pReceivedByte;

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));

        xSemaphoreGive(_mutex);
    }
    else {
        ESP_LOGE(PS01_TAG, "SPI is not initialized !!!");
    }
    return 0;
}

/**
 * @brief Configure powerSTEP01 devices
 * @param config SPI and GPIOs configuration
 * @return void
 */
void PS01_Hal_SetConfig(PS01_Hal_Config_t *config)
{
    ESP_LOGI(PS01_TAG, "Configure powerSTEP01 devices");
    memcpy(&_deviceConfig, config, sizeof(PS01_Hal_Config_t));
    _deviceConfigured = true;
}

/**
 * @brief Set switch level
 * 
 * @param deviceId 
 * @param level 
 */
void PS01_Hal_SetSwitchLevel(uint8_t deviceId, uint32_t level)
{
    ESP_LOGV(PS01_TAG, "Set switch level");

    if (_gpioInitialized[deviceId] == true) {
        ESP_LOGV(PS01_TAG, "Set switch level device%u: %d", deviceId, level);
        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig.pin_sw[deviceId], level));
    } else {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!!");
    }
}

/**
 * @brief Returns the BUSY pin state.
 * @retval The BUSY pin value.
 */
uint8_t PS01_Hal_GetBusyLevel(uint8_t deviceId)
{
    uint32_t ret = 0;
    ESP_LOGV(PS01_TAG, "Returns the BUSY pin state");

    if (_gpioInitialized[deviceId] == true) {
        ret = gpio_get_level(_deviceConfig.pin_busy_sync[deviceId]);
        ESP_LOGV(PS01_TAG, "BUSY/Sync pin state : %d", ret);
    } else {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
    return ret;
}

/**
 * @brief Returns the FLAG pin state.
 * @retval The FLAG pin value.
 */
uint8_t PS01_Hal_GetFlagLevel(uint8_t deviceId)
{
    uint32_t ret = 0;
    ESP_LOGV(PS01_TAG, "Returns the FLAG pin state");

    if (_gpioInitialized[deviceId] == true) {
        ret = gpio_get_level(_deviceConfig.pin_flag[deviceId]);
        // ESP_ERROR_CHECK(ret);
        ESP_LOGV(PS01_TAG, "BUSY/Sync pin state : %d", ret);
    } else {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
    return ret;
}

void PS01_Hal_Init(uint8_t deviceId)
{
	/* Initialise the GPIOs of the just added device */
	PS01_Hal_GpioInit(deviceId);
	PS01_Hal_SpiInit();

	/* configure the step clock */
	PS01_Hal_StepClockInit(deviceId);

	/* Standby-reset deactivation */
	PS01_Hal_ReleaseReset(deviceId);

    /* Reset the SW pin */
    PS01_Hal_SetSwitchLevel(deviceId, 0);

    /* Let a delay after reset */
	PS01_Hal_Delay(1);

    /* PowerSTEP01 Flag */
    _flagEvent[deviceId] = xQueueCreate(1, 0);
    char task_name[14];
    snprintf(task_name, 14, "Flag task %i", deviceId);        
    xTaskCreate(_flagTask, task_name, 4096, &deviceId, 5, NULL);
    gpio_isr_handler_add(_deviceConfig.pin_flag[deviceId], (gpio_isr_t)_flagIsr, &_deviceId[deviceId]);

    /* PowerSTEP01 Busy */
    _busyEvent[deviceId] = xQueueCreate(1, 0);
    snprintf(task_name, 14, "Busy task %i", deviceId);   
    xTaskCreate(_busyTask, task_name, 2048, &deviceId, 5, NULL);
    gpio_isr_handler_add(_deviceConfig.pin_busy_sync[deviceId], (gpio_isr_t)_busyIsr, &_deviceId[deviceId]);
}