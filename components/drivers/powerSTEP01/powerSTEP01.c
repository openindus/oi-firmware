#include "powerSTEP01/powerSTEP01.h"


/* Private variables */
static bool _deviceConfigured = false;
static bool _spiInitialized = false;
static bool _pwmInitialized = false;
static bool _gpioInitialized[NUMBER_OF_DEVICES] = {false, false, false};

static spi_device_handle_t _spiHandler = NULL;
Powerstep01_DeviceConfig_t *_deviceConfig = NULL;

/* NVS Memory */
const char *OI_NVM_KEY_POWERSTEP01_ACC[] = {"ACC1","ACC2"};
const char *OI_NVM_KEY_POWERSTEP01_DEC[] = {"DEC1","DEC2"};
const char *OI_NVM_KEY_POWERSTEP01_MAX_SPEED[] = {"MAXSPD1","MAXSPD2"};
const char *OI_NVM_KEY_POWERSTEP01_MIN_SPEED[] = {"MINSP1","MINSPD2"};
const char *OI_NVM_KEY_POWERSTEP01_OCD_TH[] = {"OCD1","OCD2"};
const char *OI_NVM_KEY_POWERSTEP01_FS_SPD[] = {"FSSPD1","FSSPD2"};
const char *OI_NVM_KEY_POWERSTEP01_STEP_MODE[] = {"STEP1","STEP2"};
const char *OI_NVM_KEY_POWERSTEP01_ALARM_EN[] = {"ALARM1","ALARM2"};
const char *OI_NVM_KEY_POWERSTEP01_GATECFG1[] = {"GTCFG11","GTCFG12"};
const char *OI_NVM_KEY_POWERSTEP01_GATECFG2[] = {"GTCFG21","GTCFG22"};
const char *OI_NVM_KEY_POWERSTEP01_CONFIG[] = {"CONFIG1","CONFIG2"};

const char *OI_NVM_KEY_POWERSTEP01_KVAL_HOLD[] = {"KVALH1","KVALH2"};
const char *OI_NVM_KEY_POWERSTEP01_KVAL_RUN[] = {"KVALR1","KVALR2"};
const char *OI_NVM_KEY_POWERSTEP01_KVAL_ACC[] = {"KVALA1","KVALA2"};
const char *OI_NVM_KEY_POWERSTEP01_KVAL_DEC[] = {"KVALD1","KVALD2"};
const char *OI_NVM_KEY_POWERSTEP01_INT_SPD[] = {"INTSPD1","INTSPD2"};
const char *OI_NVM_KEY_POWERSTEP01_ST_SLP[] = {"STSLP1","STSLP2"};
const char *OI_NVM_KEY_POWERSTEP01_FN_SLP_ACC[] = {"FNACC1","FNACC2"};
const char *OI_NVM_KEY_POWERSTEP01_FN_SLP_DEC[] = {"FNDEC1","FNDEC2"};
const char *OI_NVM_KEY_POWERSTEP01_K_THERM[] = {"K_THERM1","K_THERM2"};
const char *OI_NVM_KEY_POWERSTEP01_STALL_TH[] = {"STALL1","STALL2"};


/* STM32Cube drivers components fonctions used to control powerSTEP01 */

/**
 * @brief This function provides an accurate delay in milliseconds
 * @param[in] delay  time length in milliseconds
 * @retval None
 */
void Powerstep01_Board_Delay(uint32_t delay)
{
    ESP_LOGV(PS01_TAG, "Delay: %ums", delay);
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

/**
 * @brief This function enable the interruptions
 * @retval None
 */
void Powerstep01_Board_EnableIrq(void)
{
    ESP_LOGV(PS01_TAG, "Enable Irq");
    //TODO
}

/**
 * @brief This function disable the interruptions
 * @retval None
 */
void Powerstep01_Board_DisableIrq(void)
{
    ESP_LOGV(PS01_TAG, "Disable Irq");
    //TODO
}

/**
 * @brief  Initializes the GPIOs used by the powerSTEP01s
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void Powerstep01_Board_GpioInit(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Initializes the GPIOs");

    assert(_deviceConfig != NULL);

    if (deviceId < _deviceConfig->num_of_devices)
    {
        if (_deviceConfigured == true)
        {
            gpio_config_t cfg;
            
            ESP_LOGI(PS01_TAG, "Initializes the device%u's GPIOs", deviceId+1);
            ESP_LOGI(PS01_TAG, "SW: io%d | STBY/RST: io%d | BUSY/SYNC: io%d | FLAG: io%d", 
                _deviceConfig->pin_sw[deviceId], _deviceConfig->pin_stby_rst[deviceId], 
                _deviceConfig->pin_busy_sync, _deviceConfig->pin_flag);

            /* SW, STBY/RST */
            cfg.pin_bit_mask = ((1ULL << _deviceConfig->pin_sw[deviceId]) |
                                (1ULL << _deviceConfig->pin_stby_rst[deviceId]));
            cfg.mode = GPIO_MODE_OUTPUT;
            cfg.pull_up_en = GPIO_PULLUP_DISABLE;
            cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
            cfg.intr_type = GPIO_INTR_DISABLE;

            ESP_ERROR_CHECK(gpio_config(&cfg));

            /* BUSY/SYNC */
            if (deviceId == DEVICE1)
            {                
                cfg.pin_bit_mask = (1ULL << _deviceConfig->pin_busy_sync);
                cfg.mode = GPIO_MODE_INPUT;
                cfg.pull_up_en = GPIO_PULLUP_ENABLE;
                cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
                cfg.intr_type = GPIO_INTR_POSEDGE;

                ESP_ERROR_CHECK(gpio_config(&cfg));
            }

            gpio_isr_handler_add(_deviceConfig->pin_busy_sync, (gpio_isr_t)Powerstep01_BusyInterruptHandler, NULL);

            /* Flag */
            if (deviceId == DEVICE1)
            {                
                cfg.pin_bit_mask = (1ULL << _deviceConfig->pin_flag);
                cfg.mode = GPIO_MODE_INPUT;
                cfg.pull_up_en = GPIO_PULLUP_ENABLE;
                cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
                cfg.intr_type = GPIO_INTR_NEGEDGE;

                ESP_ERROR_CHECK(gpio_config(&cfg));
            }

            gpio_isr_handler_add(_deviceConfig->pin_flag, (gpio_isr_t)Powerstep01_FlagInterruptHandler, NULL);

            _gpioInitialized[deviceId] = true;
        }
        else
        {
            ESP_LOGE(PS01_TAG, "Device is not configured !!!");
        }
    }
}

/**
 * @brief Initialises the step clock by setting
 * corresponding GPIO, Timer, Pwm, ...
 * @retval None
 */
void Powerstep01_Board_StepClockInit(void)
{
    ESP_LOGV(PS01_TAG, "Initialises the step clock");

    if (_pwmInitialized == false)
    {
        if (_deviceConfigured == true)
        {
            assert(_deviceConfig != NULL);

            ESP_LOGI(PS01_TAG, "Initializes the the step clock");
            
            ledc_timer_config_t pwm_timer = {
                .speed_mode         = _deviceConfig->pwm_mode,
                .duty_resolution    = LEDC_TIMER_13_BIT,
                .timer_num          = _deviceConfig->pwm_timer,
                .freq_hz            = 1000,
                .clk_cfg            = LEDC_AUTO_CLK,
            };

            /* Timer config */
            ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

            _pwmInitialized = true;
        }
        else
        {
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
void Powerstep01_Board_StartStepClock(uint16_t newFreq)
{
    ESP_LOGV(PS01_TAG, "Start the step clock");

    if (_pwmInitialized == true)
    {
        /* Set frequency */
        ESP_ERROR_CHECK(ledc_set_freq(_deviceConfig->pwm_mode, _deviceConfig->pwm_timer, newFreq));

        ledc_channel_config_t pwm_channel = {
            .gpio_num   = _deviceConfig->pwm_pin_stck,
            .speed_mode = _deviceConfig->pwm_mode,
            .channel    = _deviceConfig->pwm_channel,
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = _deviceConfig->pwm_timer,
            .duty       = 0x1000,
            .hpoint     = 0,
        };

        /* Channel config */
        ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));            
    }
    else
    {
        ESP_LOGE(PS01_TAG, "Step clock is not initialized !!!");
    }
}

/**
 * @brief Stops the PWM uses for the step clock
 * @retval None
 */
void Powerstep01_Board_StopStepClock(void)
{
    ESP_LOGV(PS01_TAG, "Stop the step clock");

    if (_pwmInitialized == true)
    {
        ESP_ERROR_CHECK(ledc_stop(_deviceConfig->pwm_mode, _deviceConfig->pwm_channel, 0));
    }
    else
    {
        ESP_LOGE(PS01_TAG, "Step clock is not initialized !!!");
    }
}

/**
 * @brief Releases the powerSTEP01 reset (pin set to High) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void Powerstep01_Board_ReleaseReset(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Releases the powerSTEP01 reset");

    if (_gpioInitialized[deviceId] == true)
    {
        assert(_deviceConfig != NULL);

        ESP_LOGI(PS01_TAG, "Releases device%u reset", deviceId);

        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig->pin_stby_rst[deviceId], 1));
    }
    else
    {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
}

/**
 * @brief Resets the powerSTEP01 (reset pin set to low) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void Powerstep01_Board_Reset(uint8_t deviceId)
{
    ESP_LOGV(PS01_TAG, "Resets the powerSTEP01");

    if (_gpioInitialized[deviceId] == true)
    {
        assert(_deviceConfig != NULL);

        ESP_LOGI(PS01_TAG, "Resets device%u", deviceId);

        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig->pin_stby_rst[deviceId], 0));
    }
    else
    {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
}

/**
 * @brief Initializes the SPI used by powerSTEP01
 * @retval HAL_OK if SPI transaction is OK, HAL_KO else
 */
uint8_t Powerstep01_Board_SpiInit(void)
{
    ESP_LOGV(PS01_TAG, "Initializes the SPI used by powerSTEP01");

    if (_spiInitialized == false)
    {
        if (_deviceConfigured == true)
        {
            assert(_deviceConfig != NULL);

            ESP_LOGI(PS01_TAG, "Initializes the bus SPI%u", _deviceConfig->spi_host+1);
            ESP_LOGI(PS01_TAG, "MOSI: io%d | MISO: io%d | CLK: io%d | CS: io%d", 
                _deviceConfig->spi_pin_mosi, _deviceConfig->spi_pin_miso, _deviceConfig->spi_pin_clk, _deviceConfig->spi_pin_cs);

            spi_bus_config_t busConfig = {
                .mosi_io_num = _deviceConfig->spi_pin_mosi,
                .miso_io_num = _deviceConfig->spi_pin_miso,
                .sclk_io_num = _deviceConfig->spi_pin_clk,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = 32,
                .flags = 0,
                .intr_flags = 0
            };

            ESP_ERROR_CHECK(spi_bus_initialize(_deviceConfig->spi_host, &busConfig, 1));

            spi_device_interface_config_t devConfig = {
                .command_bits = 0,
                .address_bits = 0,
                .dummy_bits = 0,
                .mode = 3,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 0,
                .cs_ena_posttrans = 0,
                .clock_speed_hz = _deviceConfig->spi_freq,
                .input_delay_ns = 0,
                .spics_io_num = _deviceConfig->spi_pin_cs,
                .flags = 0,
                .queue_size = 1,
                .pre_cb = NULL,
                .post_cb = NULL
            };

            ESP_ERROR_CHECK(spi_bus_add_device(_deviceConfig->spi_host, &devConfig, &_spiHandler));

            _spiInitialized = true;
        }
        else
        {
            ESP_LOGE(PS01_TAG, "Device is not configured !!!");
        }
    }
    return 0;
}

/**
 * @brief Write and read SPI byte to the powerSTEP01
 * @param[in] pByteToTransmit pointer to the byte to transmit
 * @param[in] pReceivedByte pointer to the received byte
 * @param[in] nbDevices Number of device in the SPI chain
 * @retval 0 if SPI transaction is OK, 0xFF else
 */
uint8_t Powerstep01_Board_SpiWriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte, uint8_t nbDevices)
{
    ESP_LOGV(PS01_TAG, "Write and read SPI byte to the powerSTEP01");

    if (_spiInitialized == true)
    {
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

        assert(_deviceConfig != NULL);
        
        if (nbDevices > _deviceConfig->num_of_devices)
        {
            ESP_LOGE(PS01_TAG, "Invalid input");
            return 0xFF; // Error
        }


        trans.length = (8 * nbDevices);
        trans.rxlength = (8 * nbDevices);
        trans.tx_buffer = pByteToTransmit;
        trans.rx_buffer = pReceivedByte;

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));
    }
    else
    {
        ESP_LOGE(PS01_TAG, "SPI is not initialized !!!");
    }
    return 0;
}

/**
 * @brief Returns the BUSY pin state.
 * @retval The BUSY pin value.
 */
uint32_t Powerstep01_Board_BUSY_PIN_GetState(void)
{
    uint32_t ret = 0;

    ESP_LOGV(PS01_TAG, "Returns the BUSY pin state");

    if (_gpioInitialized[DEVICE1] == true)
    {
        assert(_deviceConfig != NULL);

        ret = gpio_get_level(_deviceConfig->pin_busy_sync);
        // ESP_ERROR_CHECK(ret);

        ESP_LOGV(PS01_TAG, "BUSY/Sync pin state : %d", ret);
    }
    else
    {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
    return ret;
}

/**
 * @brief Returns the FLAG pin state.
 * @retval The FLAG pin value.
 */
uint32_t Powerstep01_Board_FLAG_PIN_GetState(void)
{
    uint32_t ret = 0;

    ESP_LOGV(PS01_TAG, "Returns the FLAG pin state");

    if (_gpioInitialized[DEVICE1] == true)
    {
        assert(_deviceConfig != NULL);
        
        ret = gpio_get_level(_deviceConfig->pin_flag);
        // ESP_ERROR_CHECK(ret);

        ESP_LOGV(PS01_TAG, "BUSY/Sync pin state : %d", ret);
    }
    else
    {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
    return ret;
}

/* OpenIndus drivers fonctions */

void Powerstep01_DeviceConfig(Powerstep01_DeviceConfig_t *config)
{
    ESP_LOGI(PS01_TAG, "Configure powerSTEP01 devices");

    _deviceConfig = config;
    _deviceConfigured = true;
}

void Powerstep01_SetSwitchLevel(uint8_t deviceId, uint32_t level)
{
    ESP_LOGV(PS01_TAG, "Set switch level");

    if (_gpioInitialized[deviceId] == true)
    {
        assert(_deviceConfig != NULL);

        ESP_LOGD(PS01_TAG, "Set switch level device%u: %d", deviceId, level);

        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig->pin_sw[deviceId], level));
    }
    else
    {
        ESP_LOGE(PS01_TAG, "GPIOs are not initialized !!!");
    }
}

uint16_t Powerstep01_GetParamFromNVS(nvs_handle_t handle, const char *key, uint16_t defaultParam)
{
    uint16_t param;

    if (nvs_get_u16(handle, key, (uint16_t*)&(param)) != ESP_OK)
    {
        param = defaultParam;
        ESP_ERROR_CHECK(nvs_set_u16(handle, key, param));
        ESP_ERROR_CHECK(nvs_commit(handle));
    }
    ESP_LOGD(PS01_TAG, "%u", (uint16_t)param);
    return param;
}

void Powerstep01_InitDevice(uint8_t deviceId)
{
    static powerstep01_Init_u_t param;

    /* Common parameters */
#ifdef CONFIG_VOLTAGE_MODE
    param.cm.cp.cmVmSelection = POWERSTEP01_CM_VM_VOLTAGE;
#else // CONFIG_CURRENT_MODE
    param.cm.cp.cmVmSelection = POWERSTEP01_CM_VM_CURRENT;
#endif // CONFIG_VOLTAGE_MODE
    param.cm.cp.acceleration = 2008.16;
    param.cm.cp.deceleration = 2008.16;
    param.cm.cp.maxSpeed = 991.82;
    param.cm.cp.minSpeed = 0;
    param.cm.cp.lowSpeedOptimization = POWERSTEP01_LSPD_OPT_OFF;
    param.cm.cp.fullStepSpeed = 595.09;
    param.cm.cp.boostMode = POWERSTEP01_BOOST_MODE_OFF;
    param.cm.cp.ocdThreshold = POWERSTEP01_OCD_TH_281_25mV;
    param.cm.cp.stepMode = STEP_MODE_1_16;
    param.cm.cp.syncClockSelection = POWERSTEP01_SYNC_SEL_DISABLED;
    param.cm.cp.alarmsSelection = POWERSTEP01_ALARM_EN_OVERCURRENT |
                            POWERSTEP01_ALARM_EN_THERMAL_SHUTDOWN |
                            POWERSTEP01_ALARM_EN_THERMAL_WARNING |
                            POWERSTEP01_ALARM_EN_UVLO |
                            POWERSTEP01_ALARM_EN_STALL_DETECTION |
                            POWERSTEP01_ALARM_EN_SW_TURN_ON |
                            POWERSTEP01_ALARM_EN_WRONG_NPERF_CMD;
    param.cm.cp.iGate = POWERSTEP01_IGATE_64mA;
    param.cm.cp.tBoost = POWERSTEP01_TBOOST_0ns;
    param.cm.cp.tcc = POWERSTEP01_TCC_500ns;
    param.cm.cp.wdEn = POWERSTEP01_WD_EN_DISABLE;
    param.cm.cp.tBlank = POWERSTEP01_TBLANK_375ns;
    param.cm.cp.tdt = POWERSTEP01_TDT_125ns;

    /* Current mode */
    param.cm.tvalHold = 328.12;
    param.cm.tvalRun = 328.12;
    param.cm.tvalAcc = 328.12;
    param.cm.tvalDec = 328.12;
    param.cm.toffFast = POWERSTEP01_TOFF_FAST_8us;
    param.cm.fastStep = POWERSTEP01_FAST_STEP_12us;
    param.cm.tonMin = 3.0;
    param.cm.toffMin = 21.0;
    param.cm.oscClkSel = POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_2MHZ;
    param.cm.swMode = POWERSTEP01_CONFIG_SW_HARD_STOP;
    param.cm.tqReg = POWERSTEP01_CONFIG_TQ_REG_TVAL_USED;
    param.cm.enVsComp = POWERSTEP01_CONFIG_VS_COMP_DISABLE;
    param.cm.ocSd = POWERSTEP01_CONFIG_OC_SD_DISABLE;
    param.cm.uvloVal = POWERSTEP01_CONFIG_UVLOVAL_LOW;
    param.cm.vccVal = POWERSTEP01_CONFIG_VCCVAL_15V;
    param.cm.tsw = POWERSTEP01_CONFIG_TSW_048us;
    param.cm.predEn = POWERSTEP01_CONFIG_PRED_DISABLE;

    /* Voltage mode */
    param.vm.kvalHold = 16.02;
    param.vm.kvalRun = 16.02;
    param.vm.kvalAcc = 16.02;
    param.vm.kvalDec = 16.02;
    param.vm.intersectSpeed = 61.512;
    param.vm.startSlope = 0.03815;
    param.vm.accelerationFinalSlope = 0.06256;
    param.vm.decelerationFinalSlope = 0.06256;
    param.vm.thermalCompensationFactor = 1;
    param.vm.stallThreshold = 531.25;
    param.vm.oscClkSel = POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_2MHZ;
    param.vm.swMode = POWERSTEP01_CONFIG_SW_HARD_STOP;
    param.vm.enVsComp = POWERSTEP01_CONFIG_VS_COMP_DISABLE;
    param.vm.ocSd = POWERSTEP01_CONFIG_OC_SD_DISABLE;
    param.vm.uvloVal = POWERSTEP01_CONFIG_UVLOVAL_LOW;
    param.vm.vccVal = POWERSTEP01_CONFIG_VCCVAL_15V;
    param.vm.fPwmInt = POWERSTEP01_CONFIG_PWM_DIV_2;
    param.vm.fPwmDec = POWERSTEP01_CONFIG_PWM_MUL_1;

    Powerstep01_Init(&param);

    ESP_LOGI(PS01_TAG, "MOTOR%u initialized", deviceId+1);
}

void Powerstep01_InitNVSParameters(uint8_t deviceId)
{
    /* Initialize NVS */
    ESP_ERROR_CHECK(nvs_flash_init_partition("nvs_oi"));

    /* Open NVS */
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("nvs_oi", "stepper", NVS_READWRITE, &handle));

    /* ABS POS */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ABS_POS, 0);

    /* EL POS */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_EL_POS, 0);

    /* MARK */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MARK, 0);

    /* ACC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ACC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_ACC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_ACC)));
    
    /* DEC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_DEC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_DEC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_DEC)));

    /* MAX SPEED */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MAX_SPEED, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_MAX_SPEED[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_MAX_SPEED)));

    /* MIN SPEED */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MIN_SPEED, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_MIN_SPEED[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_MIN_SPEED)));

    /* FS SPD */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FS_SPD, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_FS_SPD[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_FS_SPD)));

    /* OCD TH */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_OCD_TH, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_OCD_TH[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_OCD_TH)));

    /* STEP MODE */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STEP_MODE, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_STEP_MODE[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_STEP_MODE)));

    /* ALARM EN */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ALARM_EN, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_ALARM_EN[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_ALARM_EN)));

    /* GATECFG1 */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG1, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_GATECFG1[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_GATECFG1)));

    /* GATECFG2 */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG2, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_GATECFG2[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_GATECFG2)));

#ifdef CONFIG_CURRENT_MODE

    /* TVAL HOLD */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_HOLD, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_TVAL_HOLD[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_TVAL_HOLD)));

    /* TVAL RUN */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_RUN, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_TVAL_RUN[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_TVAL_RUN)));

    /* TVAL ACC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_ACC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_TVAL_ACC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_TVAL_ACC)));

    /* TVAL DEC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_DEC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_TVAL_DEC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_TVAL_DEC)));

    /* T FAST */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_T_FAST, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_T_FAST[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_T_FAST)));

    /* TON MIN */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TON_MIN, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_TON_MIN[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_TON_MIN)));

    /* TOFF MIN */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TOFF_MIN, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_TOFF_MIN[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_TOFF_MIN)));

    /* CONFIG */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_CONFIG[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_CONFIG)));

#else // CONFIG_VOLTAGE_MODE

    /* INT SPD */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_INT_SPD, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_INT_SPD[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_INT_SPD)));

    /* K THERM */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_K_THERM, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_K_THERM[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_K_THERM)));

    /* STALL TH */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STALL_TH, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_STALL_TH[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_STALL_TH)));

    /* KVAL HOLD */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_HOLD, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_KVAL_HOLD[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_KVAL_HOLD)));

    /* KVAL RUN */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_RUN, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_KVAL_RUN[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_KVAL_RUN)));

    /* KVAL ACC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_ACC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_KVAL_ACC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_KVAL_ACC)));

    /* KVAL DEC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_DEC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_KVAL_DEC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_KVAL_DEC)));

    /* ST SLP */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ST_SLP, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_ST_SLP[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_ST_SLP)));

    /* FN SLP ACC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_ACC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_FN_SLP_ACC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_FN_SLP_ACC)));

    /* FN SLP DEC */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_DEC, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_FN_SLP_DEC[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_FN_SLP_DEC)));

    /* CONFIG */
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG, 
        Powerstep01_GetParamFromNVS(handle, OI_NVM_KEY_POWERSTEP01_CONFIG[deviceId], 
            Powerstep01_CmdGetParam(deviceId, POWERSTEP01_CONFIG)));

#endif // CONFIG_VOLTAGE_MODE

    /* Close NVS */
    nvs_close(handle);
}

void Powerstep01_SetMotorConfigToNVS(uint8_t deviceId, uint8_t reg, uint16_t value)
{
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init_partition("nvs_oi"));

    // Open
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("nvs_oi", "stepper", NVS_READWRITE, &handle));

    switch(reg)
    {
        case POWERSTEP01_ACC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_ACC[deviceId], value));
            break;
        case POWERSTEP01_DEC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_DEC[deviceId], value));
            break;
        case POWERSTEP01_MAX_SPEED:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_MAX_SPEED[deviceId], value));
            break;
        case POWERSTEP01_MIN_SPEED:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_MIN_SPEED[deviceId], value));
            break;
        case POWERSTEP01_FS_SPD:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_FS_SPD[deviceId], value));
            break;
        case POWERSTEP01_KVAL_HOLD:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_KVAL_HOLD[deviceId], value));
            break;
        case POWERSTEP01_KVAL_RUN:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_KVAL_RUN[deviceId], value));
            break;
        case POWERSTEP01_KVAL_ACC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_KVAL_ACC[deviceId], value));
            break;
        case POWERSTEP01_KVAL_DEC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_KVAL_DEC[deviceId], value));
            break;
        case POWERSTEP01_INT_SPD:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_INT_SPD[deviceId], value));
            break;
        case POWERSTEP01_ST_SLP:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_ST_SLP[deviceId], value));
            break;
        case POWERSTEP01_FN_SLP_ACC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_FN_SLP_ACC[deviceId], value));
            break;
        case POWERSTEP01_FN_SLP_DEC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_FN_SLP_DEC[deviceId], value));
            break;
        case POWERSTEP01_K_THERM:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_K_THERM[deviceId], value));
            break;
        case POWERSTEP01_OCD_TH:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_OCD_TH[deviceId], value));
            break;
        case POWERSTEP01_STALL_TH:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_STALL_TH[deviceId], value));
            break;
        case POWERSTEP01_STEP_MODE:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_STEP_MODE[deviceId], value));
            break;
        case POWERSTEP01_ALARM_EN:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_ALARM_EN[deviceId], value));
            break;
        case POWERSTEP01_GATECFG1:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_GATECFG1[deviceId], value));
            break;
        case POWERSTEP01_GATECFG2:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_GATECFG2[deviceId], value));
            break;
        case POWERSTEP01_CONFIG:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_POWERSTEP01_CONFIG[deviceId],value));
            break;
        default:
            break;
    }

    // Commit
    ESP_ERROR_CHECK(nvs_commit(handle));

    // Close
    nvs_close(handle);
}