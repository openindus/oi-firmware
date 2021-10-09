#include "L6470/L6470.h"


/* Private variables */
static bool _deviceconfigured = false;
static bool _spiInitialized = false;
static bool _pwmInitialized = false;
static bool _gpioInitialized[NUMBER_OF_DEVICES] = {false, false, false};

static spi_device_handle_t _spiHandler = NULL;
L6470_DeviceConfig_t *_deviceconfig = NULL;

/* DEFAULT CONFIG */
#define L6470_CONF_DEFAULT_ACC          ((uint16_t)0x0043)
#define L6470_CONF_DEFAULT_DEC          ((uint16_t)0x0043)
#define L6470_CONF_DEFAULT_MAX_SPEED    ((uint16_t)0x0025)
#define L6470_CONF_DEFAULT_MIN_SPEED    ((uint16_t)0x0000)
#define L6470_CONF_DEFAULT_FS_SPD       ((uint16_t)0x0025)
#define L6470_CONF_DEFAULT_KVAL_HOLD    ((uint16_t)0x0029)
#define L6470_CONF_DEFAULT_KVAL_RUN     ((uint16_t)0x0000)
#define L6470_CONF_DEFAULT_KVAL_ACC     ((uint16_t)0x0000)
#define L6470_CONF_DEFAULT_KVAL_DEC     ((uint16_t)0x0000)
#define L6470_CONF_DEFAULT_INT_SPD      ((uint16_t)0x0408)
#define L6470_CONF_DEFAULT_ST_SLP       ((uint16_t)0x0019)
#define L6470_CONF_DEFAULT_FN_SLP_ACC   ((uint16_t)0x0029)
#define L6470_CONF_DEFAULT_FN_SLP_DEC   ((uint16_t)0x0029)
#define L6470_CONF_DEFAULT_K_THERM      ((uint16_t)0x0000)
#define L6470_CONF_DEFAULT_OCD_TH       ((uint16_t)0x0004)
#define L6470_CONF_DEFAULT_STALL_TH     ((uint16_t)0x0029)
#define L6470_CONF_DEFAULT_STEP_MODE    ((uint16_t)0x0004)
#define L6470_CONF_DEFAULT_ALARM_EN     ((uint16_t)0x00FF)
#define L6470_CONF_DEFAULT_CONFIG       ((uint16_t)0x0C88)

typedef struct 
{
    uint16_t acc;
    uint16_t dec;
    uint16_t max_speed;
    uint16_t min_speed;
    uint16_t fs_spd;
    uint16_t kval_hold;
    uint16_t kval_run;
    uint16_t kval_acc;
    uint16_t kval_dec;
    uint16_t int_speed;
    uint16_t st_slp;
    uint16_t fn_slp_acc;
    uint16_t fn_slp_dec;
    uint16_t k_therm;
    uint16_t ocd_th;
    uint16_t stall_th;
    uint16_t step_mode;
    uint16_t alarm_en;
    uint16_t config;
} L6470_Init_u_t;

/* NVS Memory */
const char *OI_NVM_KEY_L6470_ACC[] = {"ACC1","ACC2"};
const char *OI_NVM_KEY_L6470_DEC[] = {"DEC1","DEC2"};
const char *OI_NVM_KEY_L6470_MAX_SPEED[] = {"MAXSPD1","MAXSPD2"};
const char *OI_NVM_KEY_L6470_MIN_SPEED[] = {"MINSP1","MINSPD2"};
const char *OI_NVM_KEY_L6470_FS_SPD[] = {"FSSPD1","FSSPD2"};
const char *OI_NVM_KEY_L6470_KVAL_HOLD[] = {"KVALH1","KVALH2"};
const char *OI_NVM_KEY_L6470_KVAL_RUN[] = {"KVALR1","KVALR2"};
const char *OI_NVM_KEY_L6470_KVAL_ACC[] = {"KVALA1","KVALA2"};
const char *OI_NVM_KEY_L6470_KVAL_DEC[] = {"KVALD1","KVALD2"};
const char *OI_NVM_KEY_L6470_INT_SPD[] = {"INTSPD1","INTSPD2"};
const char *OI_NVM_KEY_L6470_ST_SLP[] = {"STSLP1","STSLP2"};
const char *OI_NVM_KEY_L6470_FN_SLP_ACC[] = {"FNACC1","FNACC2"};
const char *OI_NVM_KEY_L6470_FN_SLP_DEC[] = {"FNDEC1","FNDEC2"};
const char *OI_NVM_KEY_L6470_K_THERM[] = {"K_THERM1","K_THERM2"};
const char *OI_NVM_KEY_L6470_OCD_TH[] = {"OCD1","OCD2"};
const char *OI_NVM_KEY_L6470_STALL_TH[] = {"STALL1","STALL2"};
const char *OI_NVM_KEY_L6470_STEP_MODE[] = {"STEP1","STEP2"};
const char *OI_NVM_KEY_L6470_ALARM_EN[] = {"ALARM1","ALARM2"};
const char *OI_NVM_KEY_L6470_CONFIG[] = {"CONFIG1","CONFIG2"};

/* Private function prototypes */
void L6470_SetDeviceParams(uint8_t deviceId, L6470_Init_u_t initPrm);
L6470_Init_u_t L6470_GetMotorConfigFromNVS(uint8_t deviceId);
uint16_t L6470_GetParamFromNVS(nvs_handle_t handle, const char *key, uint16_t defaultParam);

/* STM32Cube drivers components fonctions used to control powerSTEP01 */

/**
 * @brief This function provides an accurate delay in milliseconds
 * @param[in] delay  time length in milliseconds
 * @retval None
 */
void L6470_Board_Delay(uint32_t delay)
{
    ESP_LOGV(L6470_TAG, "Delay: %ums", delay);
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

/**
 * @brief This function enable the interruptions
 * @retval None
 */
void L6470_Board_EnableIrq(void)
{
    ESP_LOGV(L6470_TAG, "Enable Irq");
    //TODO
}

/**
 * @brief This function disable the interruptions
 * @retval None
 */
void L6470_Board_DisableIrq(void)
{
    ESP_LOGV(L6470_TAG, "Disable Irq");
    //TODO
}

/**
 * @brief  Initializes the GPIOs used by the powerSTEP01s
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void L6470_Board_GpioInit(uint8_t deviceId)
{
    ESP_LOGV(L6470_TAG, "Initializes the GPIOs");

    assert(_deviceconfig != NULL);

    if (deviceId < _deviceconfig->num_of_devices)
    {
        if (_deviceconfigured == true)
        {
            gpio_config_t cfg;
            
            ESP_LOGI(L6470_TAG, "Initializes the device%u's GPIOs", deviceId+1);
            ESP_LOGI(L6470_TAG, "SW: io%d | STBY/RST: io%d | BUSY/SYNC: io%d | FLAG: io%d", 
                _deviceconfig->pin_sw[deviceId], _deviceconfig->pin_stby_rst[deviceId], 
                _deviceconfig->pin_busy_sync, _deviceconfig->pin_flag);

            /* SW, STBY/RST */
            cfg.pin_bit_mask = ((1ULL << _deviceconfig->pin_sw[deviceId]) |
                                (1ULL << _deviceconfig->pin_stby_rst[deviceId]));
            cfg.mode = GPIO_MODE_OUTPUT;
            cfg.pull_up_en = GPIO_PULLUP_DISABLE;
            cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
            cfg.intr_type = GPIO_INTR_DISABLE;

            ESP_ERROR_CHECK(gpio_config(&cfg));

            /* BUSY/SYNC */
            if (deviceId == DEVICE1)
            {                
                cfg.pin_bit_mask = (1ULL << _deviceconfig->pin_busy_sync);
                cfg.mode = GPIO_MODE_INPUT;
                cfg.pull_up_en = GPIO_PULLUP_ENABLE;
                cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
                cfg.intr_type = GPIO_INTR_POSEDGE;

                ESP_ERROR_CHECK(gpio_config(&cfg));
            }

            gpio_isr_handler_add(_deviceconfig->pin_busy_sync, (gpio_isr_t)L6470_BusyInterruptHandler, NULL);

            /* Flag */
            if (deviceId == DEVICE1)
            {                
                cfg.pin_bit_mask = (1ULL << _deviceconfig->pin_flag);
                cfg.mode = GPIO_MODE_INPUT;
                cfg.pull_up_en = GPIO_PULLUP_ENABLE;
                cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
                cfg.intr_type = GPIO_INTR_NEGEDGE;

                ESP_ERROR_CHECK(gpio_config(&cfg));
            }

            gpio_isr_handler_add(_deviceconfig->pin_flag, (gpio_isr_t)L6470_FlagInterruptHandler, NULL);


            _gpioInitialized[deviceId] = true;
        }
        else
        {
            ESP_LOGE(L6470_TAG, "Device is not configured !!!");
        }
    }
}

/**
 * @brief Initialises the step clock by setting
 * corresponding GPIO, Timer, Pwm, ...
 * @retval None
 */
void L6470_Board_StepClockInit(void)
{
    ESP_LOGV(L6470_TAG, "Initialises the step clock");

    if (_pwmInitialized == false)
    {
        if (_deviceconfigured == true)
        {
            assert(_deviceconfig != NULL);

            ESP_LOGI(L6470_TAG, "Initializes the the step clock");
            
            ledc_timer_config_t pwm_timer = {
                .speed_mode         = _deviceconfig->pwm_mode,
                .duty_resolution    = LEDC_TIMER_13_BIT,
                .timer_num          = _deviceconfig->pwm_timer,
                .freq_hz            = 1000,
                .clk_cfg            = LEDC_AUTO_CLK,
            };

            /* Timer config */
            ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));

            _pwmInitialized = true;
        }
        else
        {
            ESP_LOGE(L6470_TAG, "Device is not configured !!!");
        }
    }
}

/**
 * @brief Start the step clock by using the given frequency
 * @param[in] newFreq in Hz of the step clock
 * @retval None
 * @note The frequency is directly the current speed of the device
 */
void L6470_Board_StartStepClock(uint16_t newFreq)
{
    ESP_LOGV(L6470_TAG, "Start the step clock");

    if (_pwmInitialized == true)
    {
        /* Set frequency */
        ESP_ERROR_CHECK(ledc_set_freq(_deviceconfig->pwm_mode, _deviceconfig->pwm_timer, newFreq));

        ledc_channel_config_t pwm_channel = {
            .gpio_num   = _deviceconfig->pwm_pin_stck,
            .speed_mode = _deviceconfig->pwm_mode,
            .channel    = _deviceconfig->pwm_channel,
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = _deviceconfig->pwm_timer,
            .duty       = 0x1000,
            .hpoint     = 0,
        };

        /* Channel config */
        ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));            
    }
    else
    {
        ESP_LOGE(L6470_TAG, "Step clock is not initialized !!!");
    }
}

/**
 * @brief Stops the PWM uses for the step clock
 * @retval None
 */
void L6470_Board_StopStepClock(void)
{
    ESP_LOGV(L6470_TAG, "Stop the step clock");

    if (_pwmInitialized == true)
    {
        ESP_ERROR_CHECK(ledc_stop(_deviceconfig->pwm_mode, _deviceconfig->pwm_channel, 0));
    }
    else
    {
        ESP_LOGE(L6470_TAG, "Step clock is not initialized !!!");
    }
}

/**
 * @brief Releases the powerSTEP01 reset (pin set to High) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void L6470_Board_ReleaseReset(uint8_t deviceId)
{
    ESP_LOGV(L6470_TAG, "Releases the powerSTEP01 reset");

    if (_gpioInitialized[deviceId] == true)
    {
        assert(_deviceconfig != NULL);

        ESP_LOGI(L6470_TAG, "Releases device%u reset", deviceId);

        ESP_ERROR_CHECK(gpio_set_level(_deviceconfig->pin_stby_rst[deviceId], 1));
    }
    else
    {
        ESP_LOGE(L6470_TAG, "GPIOs are not initialized !!!");
    }
}

/**
 * @brief Resets the powerSTEP01 (reset pin set to low) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
 * @retval None
 */
void L6470_Board_Reset(uint8_t deviceId)
{
    ESP_LOGV(L6470_TAG, "Resets the powerSTEP01");

    if (_gpioInitialized[deviceId] == true)
    {
        assert(_deviceconfig != NULL);

        ESP_LOGI(L6470_TAG, "Resets device%u", deviceId);

        ESP_ERROR_CHECK(gpio_set_level(_deviceconfig->pin_stby_rst[deviceId], 0));
    }
    else
    {
        ESP_LOGE(L6470_TAG, "GPIOs are not initialized !!!");
    }
}

/**
 * @brief Initializes the SPI used by powerSTEP01
 * @retval HAL_OK if SPI transaction is OK, HAL_KO else
 */
uint8_t L6470_Board_SpiInit(void)
{
    ESP_LOGV(L6470_TAG, "Initializes the SPI used by powerSTEP01");

    if (_spiInitialized == false)
    {
        if (_deviceconfigured == true)
        {
            assert(_deviceconfig != NULL);

            ESP_LOGI(L6470_TAG, "Initializes the bus SPI%u", _deviceconfig->spi_host+1);
            ESP_LOGI(L6470_TAG, "MOSI: io%d | MISO: io%d | CLK: io%d | CS: io%d", 
                _deviceconfig->spi_pin_mosi, _deviceconfig->spi_pin_miso, _deviceconfig->spi_pin_clk, _deviceconfig->spi_pin_cs);

            spi_bus_config_t busConfig = {
                .mosi_io_num = _deviceconfig->spi_pin_mosi,
                .miso_io_num = _deviceconfig->spi_pin_miso,
                .sclk_io_num = _deviceconfig->spi_pin_clk,
                .quadwp_io_num = -1,
                .quadhd_io_num = -1,
                .max_transfer_sz = 32,
                .flags = 0,
                .intr_flags = 0
            };

            ESP_ERROR_CHECK(spi_bus_initialize(_deviceconfig->spi_host, &busConfig, 1));

            spi_device_interface_config_t devConfig = {
                .command_bits = 0,
                .address_bits = 0,
                .dummy_bits = 0,
                .mode = 3,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 0,
                .cs_ena_posttrans = 0,
                .clock_speed_hz = _deviceconfig->spi_freq,
                .input_delay_ns = 0,
                .spics_io_num = _deviceconfig->spi_pin_cs,
                .flags = 0,
                .queue_size = 1,
                .pre_cb = NULL,
                .post_cb = NULL
            };

            ESP_ERROR_CHECK(spi_bus_add_device(_deviceconfig->spi_host, &devConfig, &_spiHandler));

            _spiInitialized = true;
        }
        else
        {
            ESP_LOGE(L6470_TAG, "Device is not configured !!!");
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
uint8_t L6470_Board_SpiWriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte, uint8_t nbDevices)
{
    ESP_LOGV(L6470_TAG, "Write and read SPI byte to the powerSTEP01");

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

        assert(_deviceconfig != NULL);
        
        if (nbDevices > _deviceconfig->num_of_devices)
        {
            ESP_LOGE(L6470_TAG, "Invalid input");
            return 0xFF; // Error
        }

        trans.length = (8 * nbDevices);
        trans.rxlength = (8 * nbDevices);
        trans.tx_buffer = pByteToTransmit;
        trans.rx_buffer = pReceivedByte;

        // printf("tx:%i  %i\n", pByteToTransmit[0], pByteToTransmit[1]);

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));

        // printf("rx:%i  %i\n", pReceivedByte[0], pReceivedByte[1]);

    }
    else
    {
        ESP_LOGE(L6470_TAG, "SPI is not initialized !!!");
    }
    return 0;
}

/**
 * @brief Returns the BUSY pin state.
 * @retval The BUSY pin value.
 */
uint32_t L6470_Board_BUSY_PIN_GetState(void)
{
    uint32_t ret = 0;

    ESP_LOGV(L6470_TAG, "Returns the BUSY pin state");

    if (_gpioInitialized[DEVICE1] == true)
    {
        assert(_deviceconfig != NULL);

        ret = gpio_get_level(_deviceconfig->pin_busy_sync);
        // ESP_ERROR_CHECK(ret);

        ESP_LOGV(L6470_TAG, "BUSY/Sync pin state : %d", ret);
    }
    else
    {
        ESP_LOGE(L6470_TAG, "GPIOs are not initialized !!!");
    }
    return ret;
}

/**
 * @brief Returns the FLAG pin state.
 * @retval The FLAG pin value.
 */
uint32_t L6470_Board_FLAG_PIN_GetState(void)
{
    uint32_t ret = 0;

    ESP_LOGV(L6470_TAG, "Returns the FLAG pin state");

    if (_gpioInitialized[DEVICE1] == true)
    {
        assert(_deviceconfig != NULL);
        
        ret = gpio_get_level(_deviceconfig->pin_flag);
        // ESP_ERROR_CHECK(ret);

        ESP_LOGV(L6470_TAG, "BUSY/Sync pin state : %d", ret);
    }
    else
    {
        ESP_LOGE(L6470_TAG, "GPIOs are not initialized !!!");
    }
    return ret;
}

/* OpenIndus drivers fonctions */

void L6470_DeviceConfig(L6470_DeviceConfig_t *config)
{
    ESP_LOGI(L6470_TAG, "Configure L6470 devices");

    _deviceconfig = config;
    _deviceconfigured = true;
}

void L6470_SetSwitchLevel(uint8_t deviceId, uint32_t level)
{
    ESP_LOGV(L6470_TAG, "Set switch level");

    if (_gpioInitialized[deviceId] == true)
    {
        assert(_deviceconfig != NULL);

        ESP_LOGD(L6470_TAG, "Set switch level device%u: %d", deviceId, level);

        ESP_ERROR_CHECK(gpio_set_level(_deviceconfig->pin_sw[deviceId], level));
    }
    else
    {
        ESP_LOGE(L6470_TAG, "GPIOs are not initialized !!!");
    }
}

void L6470_InitMotor(uint8_t deviceId)
{
    L6470_Init(NULL);

    L6470_CmdHardHiZ(deviceId);

    L6470_SetDeviceParams(deviceId, L6470_GetMotorConfigFromNVS(deviceId));

    L6470_FetchAndClearAllStatus();
}

void L6470_SetMotorConfigToNVS(uint8_t deviceId, uint8_t reg, uint16_t value)
{
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init_partition("nvs_oi"));

    // Open
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("nvs_oi", "stepper", NVS_READWRITE, &handle));

    switch(reg)
    {
        case L6470_ACC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_ACC[deviceId], value));
            break;
        case L6470_DEC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_DEC[deviceId], value));
            break;
        case L6470_MAX_SPEED:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_MAX_SPEED[deviceId], value));
            break;
        case L6470_MIN_SPEED:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_MIN_SPEED[deviceId], value));
            break;
        case L6470_FS_SPD:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_FS_SPD[deviceId], value));
            break;
        case L6470_KVAL_HOLD:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_KVAL_HOLD[deviceId], value));
            break;
        case L6470_KVAL_RUN:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_KVAL_RUN[deviceId], value));
            break;
        case L6470_KVAL_ACC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_KVAL_ACC[deviceId], value));
            break;
        case L6470_KVAL_DEC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_KVAL_DEC[deviceId], value));
            break;
        case L6470_INT_SPD:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_INT_SPD[deviceId], value));
            break;
        case L6470_ST_SLP:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_ST_SLP[deviceId], value));
            break;
        case L6470_FN_SLP_ACC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_FN_SLP_ACC[deviceId], value));
            break;
        case L6470_FN_SLP_DEC:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_FN_SLP_DEC[deviceId], value));
            break;
        case L6470_K_THERM:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_K_THERM[deviceId], value));
            break;
        case L6470_OCD_TH:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_OCD_TH[deviceId], value));
            break;
        case L6470_STALL_TH:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_STALL_TH[deviceId], value));
            break;
        case L6470_STEP_MODE:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_STEP_MODE[deviceId], value));
            break;
        case L6470_ALARM_EN:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_ALARM_EN[deviceId], value));
            break;
        case L6470_CONFIG:
            ESP_ERROR_CHECK(nvs_set_u16(handle, OI_NVM_KEY_L6470_CONFIG[deviceId],value));
            break;
        default:
            break;
    }

    // Commit
    ESP_ERROR_CHECK(nvs_commit(handle));

    // Close
    nvs_close(handle);
}

/* Private function */

void L6470_SetDeviceParams(uint8_t deviceId, L6470_Init_u_t initPrm)
{
  L6470_CmdSetParam(deviceId, L6470_ABS_POS, 0);
  L6470_CmdSetParam(deviceId, L6470_EL_POS, 0);
  L6470_CmdSetParam(deviceId, L6470_MARK, 0);
  L6470_CmdSetParam(deviceId, L6470_ACC, initPrm.acc);
  L6470_CmdSetParam(deviceId, L6470_DEC, initPrm.dec);
  L6470_CmdSetParam(deviceId, L6470_MAX_SPEED, initPrm.max_speed);
  L6470_CmdSetParam(deviceId, L6470_MIN_SPEED, initPrm.min_speed);
  L6470_CmdSetParam(deviceId, L6470_FS_SPD, initPrm.fs_spd);
  L6470_CmdSetParam(deviceId, L6470_KVAL_HOLD, initPrm.kval_hold);
  L6470_CmdSetParam(deviceId, L6470_KVAL_RUN, initPrm.kval_run);
  L6470_CmdSetParam(deviceId, L6470_KVAL_ACC, initPrm.kval_acc);
  L6470_CmdSetParam(deviceId, L6470_KVAL_DEC, initPrm.kval_dec);
  L6470_CmdSetParam(deviceId, L6470_INT_SPD, initPrm.int_speed);
  L6470_CmdSetParam(deviceId, L6470_ST_SLP, initPrm.st_slp);
  L6470_CmdSetParam(deviceId, L6470_FN_SLP_ACC, initPrm.fn_slp_acc);
  L6470_CmdSetParam(deviceId, L6470_FN_SLP_DEC, initPrm.fn_slp_dec);
  L6470_CmdSetParam(deviceId, L6470_K_THERM, initPrm.k_therm);
  L6470_CmdSetParam(deviceId, L6470_OCD_TH, initPrm.ocd_th);
  L6470_CmdSetParam(deviceId, L6470_STALL_TH, initPrm.stall_th);
  L6470_CmdSetParam(deviceId, L6470_STEP_MODE, initPrm.step_mode);
  L6470_CmdSetParam(deviceId, L6470_ALARM_EN, initPrm.alarm_en);
  L6470_CmdSetParam(deviceId, L6470_CONFIG, initPrm.config);
}

L6470_Init_u_t L6470_GetMotorConfigFromNVS(uint8_t deviceId)
{
    L6470_Init_u_t param;

    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init_partition("nvs_oi"));

    // Open
    nvs_handle_t handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("nvs_oi", "stepper", NVS_READWRITE, &handle));

    // GET ALL PARAM
    param.acc = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_ACC[deviceId], L6470_CONF_DEFAULT_ACC);
    param.dec = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_DEC[deviceId], L6470_CONF_DEFAULT_DEC);
    param.max_speed = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_MAX_SPEED[deviceId], L6470_CONF_DEFAULT_MAX_SPEED);
    param.min_speed = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_MIN_SPEED[deviceId], L6470_CONF_DEFAULT_MIN_SPEED);
    param.fs_spd = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_FS_SPD[deviceId], L6470_CONF_DEFAULT_FS_SPD);
    param.kval_hold = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_KVAL_HOLD[deviceId], L6470_CONF_DEFAULT_KVAL_HOLD);
    param.kval_run = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_KVAL_RUN[deviceId], L6470_CONF_DEFAULT_KVAL_RUN);
    param.kval_acc = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_KVAL_ACC[deviceId], L6470_CONF_DEFAULT_KVAL_ACC);
    param.kval_dec = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_KVAL_DEC[deviceId], L6470_CONF_DEFAULT_KVAL_DEC);
    param.int_speed = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_INT_SPD[deviceId], L6470_CONF_DEFAULT_INT_SPD);
    param.st_slp = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_ST_SLP[deviceId], L6470_CONF_DEFAULT_ST_SLP);
    param.fn_slp_acc = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_FN_SLP_ACC[deviceId], L6470_CONF_DEFAULT_FN_SLP_ACC);
    param.fn_slp_dec = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_FN_SLP_DEC[deviceId], L6470_CONF_DEFAULT_FN_SLP_DEC);
    param.k_therm = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_K_THERM[deviceId], L6470_CONF_DEFAULT_K_THERM);
    param.ocd_th = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_OCD_TH[deviceId], L6470_CONF_DEFAULT_OCD_TH);
    param.stall_th = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_STALL_TH[deviceId], L6470_CONF_DEFAULT_STALL_TH);
    param.step_mode = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_STEP_MODE[deviceId], L6470_CONF_DEFAULT_STEP_MODE);
    param.alarm_en = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_ALARM_EN[deviceId], L6470_CONF_DEFAULT_ALARM_EN);
    param.config = L6470_GetParamFromNVS(handle, OI_NVM_KEY_L6470_CONFIG[deviceId], L6470_CONF_DEFAULT_CONFIG);

    // Close
    nvs_close(handle);

    return param;
}

uint16_t L6470_GetParamFromNVS(nvs_handle_t handle, const char *key, uint16_t defaultParam)
{
    uint16_t param;

    if (nvs_get_u16(handle, key, (uint16_t*)&(param)) != ESP_OK)
    {
        param = defaultParam;
        ESP_ERROR_CHECK(nvs_set_u16(handle, key, param));
        ESP_ERROR_CHECK(nvs_commit(handle));
    }
    ESP_LOGD(L6470_TAG, "%u", (uint16_t)param);
    return param;
}