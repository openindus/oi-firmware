#include "ads866x.h"

static const char ADS866x_TAG[] = "Ads866x";

static uint8_t s_ads866x_mode = ADS866X_MODE_IDLE;
static ads866x_config_t* s_config = NULL;
static float s_ads866x_voltage_reference = ADS8664_VOLTAGE_REFERENCE;
static spi_device_handle_t s_spi_handler = NULL;

static bool s_spi_initialized = false;
static bool s_gpio_initialized = false;
static bool s_device_configured = false;
static bool* adc_channels_PD = NULL;

static int ads866x_gpio_init(void)
{
    int err = 0;

    assert(s_config != NULL);

    if (s_device_configured == true) {
        gpio_config_t cfg;
            
        ESP_LOGI(ADS866x_TAG, "Initialize Ads866x GPIOs");

        /* Configure Rst pin */
        cfg.pin_bit_mask = (1ULL << s_config->pin_rst);
        cfg.mode = GPIO_MODE_OUTPUT;
        cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        cfg.intr_type = GPIO_INTR_DISABLE;

        err |= gpio_config(&cfg);

        /* Configure Alarm pin */
        cfg.pin_bit_mask = (1ULL << s_config->pin_alarm);
        cfg.mode = GPIO_MODE_INPUT;
        cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        cfg.intr_type = GPIO_INTR_POSEDGE;

        err |= gpio_config(&cfg);

        // Set Rst pin to HIGH level
        err |= gpio_set_level(s_config->pin_rst, 1);

        s_gpio_initialized = true;
    } else {
        ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
        return -1;
    }

    return err;
}

static int ads866x_spi_init(void)
{
    int err = 0;

    if (s_spi_initialized == false) {
        ESP_LOGI(ADS866x_TAG, "Initialize the Ads866x device on the SPI bus");

        if (s_device_configured == true) {
            assert(s_config != NULL);

            spi_device_interface_config_t device_config = {
                .command_bits = 0,
                .address_bits = 0,
                .dummy_bits = 0,
                .mode = 1,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 1,
                .cs_ena_posttrans = 0,
                .clock_speed_hz = s_config->spi_freq,
                .input_delay_ns = 0,
                .spics_io_num = s_config->spi_pin_cs,
                .flags = 0,
                .queue_size = 1,
                .pre_cb = NULL,
                .post_cb = NULL
            };

            err = spi_bus_add_device(s_config->spi_host, &device_config, &s_spi_handler);

            s_spi_initialized = true;
        } else {
            ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
            return -1;
        }
    }

    return err;
}

int ads866x_init(ads866x_config_t *config)
{
    int ret = 0;

    ESP_LOGI(ADS866x_TAG, "Configure Ads866x device");

    s_config = config;
    s_device_configured = true;

    adc_channels_PD = (bool*)calloc(s_config->adc_channel_nb, sizeof(bool));

    ads866x_gpio_init();
    ads866x_spi_init();
    ads866x_set_channels_power_down(0xFF);  //Power down all channels for power consumption optimization

    return ret;
}

uint16_t ads866x_analog_read(uint8_t channel)
{
    uint16_t res = 0;

    if (s_device_configured) {
        if (channel < s_config->adc_channel_nb) {
            ads866x_manual_channel_select(channel);
            res = ads866x_noOp();

            // Puts result in 12 bits format - Cf datasheet ADS8664 Block Table 4 - Page 56
            res = res >> 4;
        } else {
            ESP_LOGE(ADS866x_TAG, "Invalid channel index. Must be < %d", s_config->adc_channel_nb);            
        }
    } else {
        ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
    }
    return res;
}

float ads866x_get_voltage_reference(void)
{
    return s_ads866x_voltage_reference;
}

void ads866x_set_voltage_reference(float ref)
{
    s_ads866x_voltage_reference = ref;
}

uint16_t ads866x_noOp(void)
{
    return ads866x_spi_write_command_register(ADS866X_CMD_NO_OP);
}

uint16_t ads866x_reset(void)
{
    ESP_LOGI(ADS866x_TAG, "Reset ads866x.");
    return ads866x_spi_write_command_register(ADS866X_CMD_RST);
}

uint16_t ad866x_standby(void)
{
    return ads866x_spi_write_command_register(ADS866X_CMD_STDBY);
}

uint16_t ads866x_power_down(void)
{
    return ads866x_spi_write_command_register(ADS866X_CMD_PWR_DN);
}

uint16_t ads866x_auto_reset(void)
{
    return ads866x_spi_write_command_register(ADS866X_CMD_AUTO_RST);
}

uint16_t ads866x_manual_channel_select(uint8_t channel)
{
    uint16_t ret = 0;
    uint8_t cmd = 0;

    if (!s_device_configured) {
        return -1;
    }

    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }

    if (adc_channels_PD[channel] == true) { 
        uint8_t channels_PD = ads866x_get_channels_power_down();
        ads866x_set_channels_power_down(channels_PD & (0 << channel));      //Power up the channel
    }

    cmd = ADS866X_CMD_MAN_CH_0 + (channel * 4);
    ret = ads866x_spi_write_command_register(cmd);

    return ret;
}

void ads866x_set_channels_sequence(uint8_t channels_on)
{
    ads866x_spi_write_register(AUTO_SEQ_EN, channels_on);
}

void ads866x_set_channels_power_down(uint8_t channels_off)
{
    for (size_t i = 0; i < s_config->adc_channel_nb; i++) {
        adc_channels_PD[i] = (bool)((channels_off >> i) & 1);
    }
    
    ads866x_spi_write_register(CH_PWR_DN, channels_off);
}

void ads866x_set_channel_SPD(uint8_t channels)
{
    ads866x_set_channels_sequence(channels);
    ads866x_set_channels_power_down((uint8_t)~channels);
}

uint8_t ads866x_get_channels_sequence(void)
{
    return ads866x_spi_read_program_register(AUTO_SEQ_EN);
}

uint8_t ads866x_get_channels_power_down(void)
{
    return ads866x_spi_read_program_register(CH_PWR_DN);
}


/************* Feature Select Register *****************/

uint8_t ads866x_get_device_id(void)
{
    return (ads866x_get_feature_select() >> 6);
}

void ads866x_set_device_id(uint8_t id)
{
    uint8_t reg = 0;
    reg = ads866x_get_feature_select();
    reg = (reg & 0b00010111) | ((id & 0b11) << 6);
    ads866x_spi_write_register(FT_SEL, reg);
}

bool ads866x_get_alarm(void)
{
    return (ads866x_get_feature_select() >> 4) & 1;
}

void ads866x_set_alarm(bool alarm)
{
    uint8_t reg = 0;
    reg = ads866x_get_feature_select();
    reg = (reg & 0b11000111) | ((alarm == true) << 4);
    ads866x_spi_write_register(FT_SEL, reg);
}

uint8_t ads866x_get_sdo(void)
{
    return (ads866x_get_feature_select() & 0b111);
}

void ads866x_set_sdo(uint8_t sdo)
{
    uint8_t reg = 0;
    reg = ads866x_get_feature_select();
    reg = (reg & 0b11010000) | (sdo & 0b111);
    ads866x_spi_write_register(FT_SEL, reg);
}

uint8_t ads866x_get_feature_select(void)
{
    return ads866x_spi_read_program_register(FT_SEL);
}

uint8_t ads866x_get_channel_voltage_range(uint8_t channel)
{
    uint8_t reg = 0;
    uint8_t ads866x_range = 0;
    uint8_t range = 0;

    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = RG_CH_0 + channel;
    ads866x_range = ads866x_spi_read_program_register(reg);
    
    switch (ads866x_range) {
        case ADS866X_VOLTAGE_RANGE_0:
        case ADS866X_VOLTAGE_RANGE_1:
        case ADS866X_VOLTAGE_RANGE_2:
        case ADS866X_VOLTAGE_RANGE_3:
        case ADS866X_VOLTAGE_RANGE_5:
        case ADS866X_VOLTAGE_RANGE_6:
        case ADS866X_VOLTAGE_RANGE_7:
            range = ads866x_range;
            break;
        case ADS866X_VOLTAGE_RANGE_4:
            range = 4;
            break;
        case ADS866X_VOLTAGE_RANGE_8:
            range = 8;
            break;
        default:
            ESP_LOGE(ADS866x_TAG, "Invalid range");
            break;
    }

    return range;
}

void ads866x_set_channel_voltage_range(uint8_t channel, uint8_t range)
{
    uint8_t reg = 0;
    uint8_t ads866x_range = 0;
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG, "Invalid channel number");
    }

    switch (range) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
            ads866x_range = range;
            break;
        case 4:
            ads866x_range = ADS866X_VOLTAGE_RANGE_4;
            break;
        case 8:
            ads866x_range = ADS866X_VOLTAGE_RANGE_8;
            break;
        default:
            ESP_LOGE(ADS866x_TAG, "Invalid range");
            break;
    }
       
    reg = RG_CH_0 + channel;
    ads866x_spi_write_register(reg, (ads866x_range & 0x0F));
}

void ads866x_set_global_voltage_range(uint8_t range)
{
    for (int i = 0; i < s_config->adc_channel_nb; i++) {
        ads866x_set_channel_voltage_range(i, range);
    }
}


/************** Alarm Registers ***********************/

uint8_t ads866x_get_alarm_overview()
{
    return ads866x_spi_read_program_register(ALARM_OVERVIEW);
}

uint8_t ads866x_get_first_tripped_flag()
{
    return ads866x_spi_read_program_register(ALARM_CH0_TRIPPED_FLAG);
}

uint8_t ads866x_get_second_tripped_flag()
{
    return ads866x_spi_read_program_register(ALARM_CH4_TRIPPED_FLAG);
}

uint16_t ads866x_get_all_tripped_flags()
{
    uint8_t MSB = ads866x_spi_read_program_register(ALARM_CH0_TRIPPED_FLAG);
    uint8_t LSB = ads866x_spi_read_program_register(ALARM_CH4_TRIPPED_FLAG);

    return (MSB << 8) | LSB;
}

uint8_t ads866x_get_first_active_flag()
{
    return ads866x_spi_read_program_register(ALARM_CH0_ACTIVE_FLAG);
}

uint8_t ads866x_get_second_active_flag()
{
    return ads866x_spi_read_program_register(ALARM_CH4_ACTIVE_FLAG);
}

uint16_t ads866x_get_all_active_flags()
{
    uint8_t MSB = ads866x_spi_read_program_register(ALARM_CH0_ACTIVE_FLAG);
    uint8_t LSB = ads866x_spi_read_program_register(ALARM_CH4_ACTIVE_FLAG);

    return (MSB << 8) | LSB;    
}

uint8_t ads866x_get_channel_hysteresis(uint8_t channel)
{
    uint8_t reg = 0;
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = CH0_HYST + (5*channel);
    return ads866x_spi_read_program_register(reg);
}

uint16_t ads866x_get_channel_low_threshold(uint8_t channel)
{
    uint8_t reg = 0;
    uint8_t MSB = 0;
    uint8_t LSB = 0;
    
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = CH0_LT_MSB + (5*channel);
    MSB = ads866x_spi_read_program_register(reg);
    LSB = ads866x_spi_read_program_register(reg+1);

    return (MSB << 8) | LSB;
}

uint16_t ads866x_get_channel_high_threshold(uint8_t channel)
{
    uint8_t reg = 0;
    uint8_t MSB = 0;
    uint8_t LSB = 0;
    
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = CH0_HT_MSB + (5*channel);
    MSB = ads866x_spi_read_program_register(reg);
    LSB = ads866x_spi_read_program_register(reg+1);

    return (MSB << 8) | LSB;
}

void ads866x_set_channel_hysteresis(uint8_t channel, uint8_t hysteresis)
{
    uint8_t reg = 0;
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = CH0_HYST + (5*channel);
    ads866x_spi_write_register(reg, (hysteresis & 0x0F) << 4);
}

void ads866x_set_channel_low_threshold(uint8_t channel, uint16_t threshold)
{
    uint8_t reg = 0;
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = CH0_LT_MSB + (5*channel);
    ads866x_spi_write_register(reg, (threshold & 0xFFF) >> 8);
    ads866x_spi_write_register(reg+1, (threshold & 0x0F) << 4);
}

void ads866x_set_channel_high_threshold(uint8_t channel, uint16_t threshold)
{
    uint8_t reg = 0;
    if (channel >= s_config->adc_channel_nb) {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = CH0_HT_MSB + (5*channel);
    ads866x_spi_write_register(reg, (threshold & 0xFFF) >> 8);
    ads866x_spi_write_register(reg+1, (threshold & 0x0F) << 4);    
}

uint8_t ads866x_get_command_readback(void)
{
    return ads866x_spi_read_program_register(CMD_READBACK);
}


/************** SPI Read/Write functions ***********************/

esp_err_t ads866x_spi_write_register(uint8_t reg, uint8_t value)
{
    uint8_t txBuffer[3] = {((reg << 1) | 0x01), value, 0x00};

    if (s_spi_initialized == true) {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 24,
            .rxlength = 0,
            .user = NULL,
            .tx_buffer = &txBuffer,
            .rx_buffer = NULL
        };

        ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi_handler, &trans));

        s_ads866x_mode = ADS866X_MODE_PROG;
    } else {
        ESP_LOGE(ADS866x_TAG, "SPI is not initialized !!!");
    }
    return 0;
}

uint8_t ads866x_spi_read_program_register(uint8_t reg)
{
    uint8_t txBuffer[3] = {((reg << 1) | 0x00), 0x00, 0x00};
    uint8_t rxBuffer[3];

    if (s_spi_initialized == true) {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 24,
            .rxlength = 24,
            .user = NULL,
            .tx_buffer = &txBuffer,
            .rx_buffer = &rxBuffer
        };

        ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi_handler, &trans));

        s_ads866x_mode = ADS866X_MODE_PROG;
    } else {
        ESP_LOGE(ADS866x_TAG, "SPI is not initialized !!!");
    }

    uint32_t res = rxBuffer[0] << 16 | rxBuffer[1] << 8 | rxBuffer[2];

    return (uint8_t)(res >> 1);    //Correction of the ESP reading which is left shifted by 1 bit
}

uint16_t ads866x_spi_write_command_register(uint8_t reg)
{
    uint8_t txBuffer[4] = {reg, 0x00, 0x00, 0x00};
    uint8_t rxBuffer[4];

    if (s_spi_initialized == true) {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 16,
            .rxlength = 16,
            .user = NULL,
            .tx_buffer = txBuffer,
            .rx_buffer = rxBuffer
        };

        if (s_ads866x_mode > ADS866X_MODE_PROG) {
            // only 16 bit if POWERDOWN or STDBY or RST or IDLE
            trans.length = 32;
        }

        ESP_ERROR_CHECK(spi_device_polling_transmit(s_spi_handler, &trans));
    } else {
        ESP_LOGE(ADS866x_TAG, "SPI is not initialized !!!");
    }

    if (s_ads866x_mode == ADS866X_MODE_POWER_DN) {
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }

    switch (reg) {
        case ADS866X_CMD_NO_OP:
            switch (s_ads866x_mode) {
                case ADS866X_MODE_RESET:    s_ads866x_mode = ADS866X_MODE_IDLE;
                    break;
                case ADS866X_MODE_PROG :    s_ads866x_mode = ADS866X_MODE_IDLE;
                    break;
                case ADS866X_MODE_AUTO_RST: s_ads866x_mode = ADS866X_MODE_AUTO;
                    break;
            }
            break;
        case ADS866X_CMD_STDBY:
            s_ads866x_mode = ADS866X_MODE_STANDBY;
            break;
        case ADS866X_CMD_PWR_DN:
            s_ads866x_mode = ADS866X_MODE_POWER_DN;
            break;
        case ADS866X_CMD_RST:
            s_ads866x_mode = ADS866X_MODE_RESET;
            break;
        case ADS866X_CMD_AUTO_RST:
            s_ads866x_mode = ADS866X_MODE_AUTO_RST;
            break;
        default:
            s_ads866x_mode = ADS866X_MODE_MANUAL;
            break;
    }

    uint32_t res = rxBuffer[0] << 24 | rxBuffer[1] << 16 | rxBuffer[2] << 8 | rxBuffer[3];

    return (uint16_t)(res >> 1);    //Correction of the ESP reading which is left shifted by 1 bit
}

float ads866x_convert_raw_2_volt(uint16_t raw, uint8_t range)
{
    float out_min, out_max;

    switch (range) {
        case 1:
        case 2:
        case 3:
        case 4:
            out_min = -1.25 * s_ads866x_voltage_reference / (float)pow(2, range-1);
            out_max = 1.25 * s_ads866x_voltage_reference / (float)pow(2, range-1);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            out_min = 0;
            out_max = 2.5 * s_ads866x_voltage_reference / (float)pow(2, range-5);
            break;

        default:
            out_min = -2.5 * s_ads866x_voltage_reference;
            out_max = 2.5 * s_ads866x_voltage_reference;
            break;
    }
    return (float)raw * (out_max - out_min) / (float)(1 << ADS866x_RESOLUTION_BITS) + out_min;
}

uint16_t ads866x_convert_volt_2_raw(float voltage, uint8_t range)
{
    float in_min, in_max;
    switch (range) {
        case 1:
        case 2:
        case 3:
        case 4:
            in_min = -1.25 * s_ads866x_voltage_reference / (float)pow(2, range-1);
            in_max = 1.25 * s_ads866x_voltage_reference / (float)pow(2, range-1);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            in_min = 0;
            in_max = 2.5 * s_ads866x_voltage_reference / (float)pow(2, range-5);
            break;

        default:
            in_min = -2.5 * s_ads866x_voltage_reference;
            in_max = 2.5 * s_ads866x_voltage_reference;
            break;
    }
    return (voltage-in_min) * (1 << ADS866x_RESOLUTION_BITS) / (in_max-in_min);    
}