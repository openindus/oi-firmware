#include "ads1x4s08.h"
#include "cd74hc4052.h"


// Set ADC_RESOLUTION between 16 and 24 bits
#define ADC_RESOLUTION  16
#define DATA_FORMAT     (ADC_RESOLUTION - 16) 

spi_device_handle_t adc_handle;

static const char ADC_TAG[] = "ads114s08";

ads1x4s08_map_register_t ads1x4s08_mapreg;

QueueHandle_t isrQueue = NULL;

ads1x4s08_data_t data;

/**
 * @brief Configures SPI parameters and initialize ADC
 * @retval None
*/
void ADS1x4S08_Hal_spi_init(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk, gpio_num_t cs){
    
    spi_bus_config_t spibuscfg = {
    .mosi_io_num        = mosi,
    .miso_io_num        = miso,
    .sclk_io_num        = sclk,
    .quadwp_io_num      = -1,
    .quadhd_io_num      = -1,
    .data4_io_num       = -1, 
    .data5_io_num       = -1,
    .data6_io_num       = -1,
    .data7_io_num       = -1,
    .max_transfer_sz    = 32,
    .flags              = 0,
    .intr_flags         = 0
    };

    spi_device_interface_config_t spidevcfg = {
    .command_bits       = 0,
    .address_bits       = 0,
    .dummy_bits         = 0,
    .mode               = ADS1X4S08_SPI_MODE_1,
    .duty_cycle_pos     = 0,
    .cs_ena_pretrans    = 0,
    .cs_ena_posttrans   = 0,
    .clock_speed_hz     = 1000000,
    .input_delay_ns     = 0,
    .spics_io_num       = -1,
    .flags              = 0,
    .queue_size         = 1,
    .pre_cb             = NULL,
    .post_cb            = NULL
    };

    esp_err_t err_init = spi_bus_initialize(HOST_ID, &spibuscfg, SPI_DMA_CH_AUTO);
    if(err_init != ESP_OK)
        ESP_LOGE(ADC_TAG, "The SPI bus is not initialized");
    ESP_ERROR_CHECK(err_init);
    esp_err_t err_bus = spi_bus_add_device(HOST_ID, &spidevcfg, &adc_handle);
    if(err_init != ESP_OK)
        ESP_LOGE(ADC_TAG, "The ADC device is not initialized");
    ESP_ERROR_CHECK(err_bus);
}

/**
 * @brief Configures transaction parameters and transmit data on SPI bus
 * @param[in] data_to_send Byte to send
 * @retval ADC data
*/
uint8_t ADS1x4S08_Hal_spi_transfer(uint8_t data_to_send){
    uint8_t buffer;

    spi_transaction_t transfer = {
    .flags      = 0,
    .cmd        = 0,
    .addr       = 0,
    .length     = 8,
    .rxlength   = 8,
    .user       = NULL,
    .tx_buffer  = &data_to_send,
    .rx_buffer  = &buffer
    };

    esp_err_t err_trans = spi_device_transmit(adc_handle, &transfer);
    if(err_trans != ESP_OK)
        ESP_LOGE(ADC_TAG, "Bad configuration of the SPI transaction parameters");
    ESP_ERROR_CHECK(err_trans);

    return buffer;
}

/**
 * @brief Reads an ADC register
 * @param[in] reg_addr Byte of the register address
 * @param[out] reg_data Byte of the register data
 * @retval None
*/
void ADS1x4S08_get_register(ads1x4s08_reg_addr_t reg_addr, uint8_t *reg_data){
    ADS1x4S08_Hal_spi_transfer(0x20 + reg_addr);
    ADS1x4S08_Hal_spi_transfer(0x00);
    uint8_t register_data = ADS1x4S08_Hal_spi_transfer(0);
    memcpy(reg_data, &register_data, 1);
}

/**
 * @brief Writes an ADC register
 * @param[in] reg_addr Byte of the register address
 * @param[inout] reg_data Byte of the register data
 * @retval None
*/
void ADS1x4S08_set_register(ads1x4s08_reg_addr_t reg_addr, uint8_t *reg_data){
    ADS1x4S08_Hal_spi_transfer(0x40 + reg_addr);
    ADS1x4S08_Hal_spi_transfer(0x00);
    ADS1x4S08_Hal_spi_transfer(*reg_data);
}

/**
 * @brief Initialize register variables from the default value set in the ADC after power on
 * @retval None
*/
void ADS1x4S08_get_default_register(void){
    ADS1x4S08_get_register(ADS1X4S08_REG_ID, &ads1x4s08_mapreg.ads1x4s08_device_id);

    ADS1x4S08_get_register(ADS1X4S08_REG_STATUS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_status);

    ADS1x4S08_get_register(ADS1X4S08_REG_INPMUX, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_inpmux);

    ADS1x4S08_get_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);

    ADS1x4S08_get_register(ADS1X4S08_REG_DATARATE, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_datarate);
    
    ADS1x4S08_get_register(ADS1X4S08_REG_REF, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_ref);

    ADS1x4S08_get_register(ADS1X4S08_REG_IDACMAG  , (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_idacmag);

    ADS1x4S08_get_register(ADS1X4S08_REG_IDACMUX, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_idacmux);

    ADS1x4S08_get_register(ADS1X4S08_REG_VBIAS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_vbias);

    ADS1x4S08_get_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);

    ADS1x4S08_get_register(ADS1X4S08_REG_OFCAL0, &ads1x4s08_mapreg.ads1x4s08_ofcal0);

    ADS1x4S08_get_register(ADS1X4S08_REG_OFCAL1, &ads1x4s08_mapreg.ads1x4s08_ofcal1);

    ADS1x4S08_get_register(ADS1X4S08_REG_FSCAL0 , &ads1x4s08_mapreg.ads1x4s08_fscal0);

    ADS1x4S08_get_register(ADS1X4S08_REG_FSCAL1, &ads1x4s08_mapreg.ads1x4s08_fscal1);

    ADS1x4S08_get_register(ADS1X4S08_REG_GPIODAT, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_gpiodat);

    ADS1x4S08_get_register(ADS1X4S08_REG_GPIOCON, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_gpiocon);
}

/**
 * @brief Set the reference voltage of ADC
 * @param[in] ref Type of reference voltage (ext/int)
 * @param[in] state State of the internal reference voltage (on/off)
 * @retval None
*/
void ADS1x4S08_set_reference_voltage(ads1x4s08_ref_input_t ref, ads1x4s08_ref_voltage_config_t state){
    ads1x4s08_mapreg.ads1x4s08_ref.ads1x4s08_refcon = state;
    ads1x4s08_mapreg.ads1x4s08_ref.ads1x4s08_refsel = ref;
    ADS1x4S08_set_register(ADS1X4S08_REG_REF, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_ref);
}

/**
 * @brief Set datarate of ADC
 * @param[in] datarate Datarate of ADC
 * @retval None
*/
void ADS1x4S08_set_datarate(ads1x4s08_datarate_t datarate){
    ads1x4s08_mapreg.ads1x4s08_datarate.dr = datarate;
    ADS1x4S08_set_register(ADS1X4S08_REG_DATARATE, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_datarate);
}

/**
 * @brief Set gain of ADS1X4S08_PGA integrated in ADC
 * @param[in] gain ADS1X4S08_PGA gain
 * @retval None
*/
void ADS1x4S08_set_gain(ads1x4s08_pga_gain_t gain){
    if(gain == ADS1X4S08_GAIN_1)
        ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_DISABLE;
    else
        ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
    ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain = gain;
    ADS1x4S08_set_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);
}

/**
 * @brief Detect ADS1X4S08_PGA saturation and if a saturation occures, return true, else false
 * @retval Boolean value
*/
bool ADS1x4S08_pga_saturation(){
    int N;
    N = (data.ads1x4s08_data1 << 8) + data.ads1x4s08_data2;
    #if DATA_FORMAT
        N = (N << 8) + data.data3;
    #endif
    if((N == 32767) | (N == 32768)){
        ESP_LOGE(ADC_TAG, "The ADS1X4S08_REG_PGA limits are reached ! A gain calibration is required...");
        return true;
    }
    else
        return false;
}

/**
 * @brief Auto calibration of the ADS1X4S08_PGA
 * @retval None
*/
void ADS1x4S08_gain_calibration(){
    ads1x4s08_pga_gain_t gain;
    bool erreur = false, gain_max = false;

    gain = (ads1x4s08_pga_gain_t)ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain;
    ADS1x4S08_set_gain(gain);

    if(ADS1x4S08_pga_saturation())
    {
        do
        {
            if (gain == ADS1X4S08_GAIN_1)
            {
                ESP_LOGE(ADC_TAG, "Please check the voltage range of the sensor, it might be out of limits !");
                erreur = true;
                break;
            }
            ESP_LOGI(ADC_TAG, "Gain calibration in progress...");
            gain--;
            ADS1x4S08_set_gain(gain);
        } while (ADS1x4S08_pga_saturation());
        if(!erreur & (gain == ADS1X4S08_GAIN_1))
        {
            ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_DISABLE;
            ADS1x4S08_set_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);
        }
    }
    else
    {
        do
        {
            if(gain == ADS1X4S08_GAIN_128)
            {
                gain_max = true;
                break;
            }
            ESP_LOGI(ADC_TAG, "Gain calibration in progress...");
            gain++;
            ADS1x4S08_set_gain(gain);
        } while (!ADS1x4S08_pga_saturation());
        if(!gain_max)
        {
            gain--;
            ADS1x4S08_set_gain(gain);
        }
    }
}

/**
 * @brief Enables or not status and crc in data format
 * @param[in] status Status register of ADC
 * @param[in] crc CRC value to detect transmission errors
 * @retval None
*/
void ADS1x4S08_set_data_format(ads1x4s08_enable_t status, ads1x4s08_enable_t crc){
    if(((crc | status) != ADS1X4S08_ENABLE) & ((crc | status) != ADS1X4S08_DISABLE))
    {
        ESP_LOGE(ADC_TAG, "Wrong value for crc or sendstat, please set ENABLE or ADS1X4S08_DISABLE");
        ESP_ERROR_CHECK(ESP_ERR_INVALID_ARG);
    }
    else
    {
        ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_sendstat = status;
        ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_crc = crc;
        ADS1x4S08_set_register(ADS1X4S08_REG_SYS, (uint8_t *)&ads1x4s08_mapreg.ads1x4s08_sys);
    }
}

/**
 * @brief Start data conversion with the ADC
 * @retval None
*/
void ADS1x4S08_start_conversion(void){
    ads1x4s08_mapreg.ads1x4s08_datarate.mode = ADS1X4S08_CONTINUOUS_MODE;
    ADS1x4S08_set_register(ADS1X4S08_REG_DATARATE, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_datarate);
    ADS1x4S08_Hal_spi_transfer(START);
    ADS1x4S08_Hal_spi_transfer(SFOCAL);         // Self offset calibration
    ESP_LOGI(ADC_TAG, "Conversion started...");
}

/**
 * @brief Get conversion data from the ADC
 * @retval None
*/
void ADS1x4S08_get_conversion_data(void){
    int isr;
    ADS1x4S08_get_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);
    xQueueReceive(isrQueue, &isr,  portMAX_DELAY);       // Delay the data retrieval for the next falling edge of /DRDY to prevent from data corruption with previous other SPI commands
    if(xQueueReceive(isrQueue, &isr, portMAX_DELAY)){
        ADS1x4S08_Hal_spi_transfer(RDATA);
        if(ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_sendstat == ADS1X4S08_ENABLE)
            data.ads1x4s08_status = ADS1x4S08_Hal_spi_transfer(0); // write 0 in register status
        data.ads1x4s08_data1 = ADS1x4S08_Hal_spi_transfer(0); // write 0 in register data 1 
        data.ads1x4s08_data2 = ADS1x4S08_Hal_spi_transfer(0); // write 0 in register data 2 
        #if DATA_FORMAT
            data.data3 = ADS1x4S08_Hal_spi_transfer(0); // write 0 in register data 3
        #endif
        if(ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_crc == ADS1X4S08_ENABLE)
            data.ads1x4s08_crc = ADS1x4S08_Hal_spi_transfer(0); // write 0 in register CRC
        ESP_LOGV(ADC_TAG, "Data retrieved ! \n");
    }
    else
        ESP_LOGE(ADC_TAG, "No data retrieved !\n");

}

/**
 * @brief Stop data conversion
 * @retval None
*/
void ADS1x4S08_stop_conversion(void){
    ADS1x4S08_Hal_spi_transfer(STOP);
    ESP_LOGI(ADC_TAG, "Data conversion stopped !");
}

/**
 * @brief Converts digital value from the ADC conversion into voltage value
 * @retval Voltage value
*/
float ADS1x4S08_get_analog_voltage(){
    float adc_voltage;
    uint32_t N;
    uint32_t bitmask;

    ADS1x4S08_get_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);
    float q = (2 * VREF / (float)pow(2, ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain)) / (float)pow(2, ADC_RESOLUTION);
    
    #if !DATA_FORMAT
        N = ((data.ads1x4s08_data1 << 8) + data.ads1x4s08_data2);
        bitmask = 0xffff;
    #elif DATA_FORMAT
        N = ((data.data1 << 16) + (data.data2 << 8) + data.data3);
        bitmask = 0xffffff;
    #endif
    if(N >= pow(2, ADC_RESOLUTION - 1)){
        N = ((~N) & bitmask) + 1;
        adc_voltage = -(float)N * q;
    }
    else
        adc_voltage = (float)N * q;

    return adc_voltage;
}

/**
 * @brief Guarantees that the initial CRC matches the calculated CRC with output data
 * @retval Boolean value to valid data
*/
bool ADS1x4S08_check_data(){
    uint32_t result = 0;
    uint32_t polynom;
    uint msb;

    ADS1x4S08_get_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);

    if(ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_crc == ADS1X4S08_ENABLE)
    {
        #if !DATAFORMAT
            msb = 23;
            if(ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_sendstat == ADS1X4S08_ENABLE)
                result = data.ads1x4s08_status << 24;
            result += (data.ads1x4s08_data1 << 16) + (data.ads1x4s08_data2 << 8);
        #elif DATAFORMAT
            i = 31;
            if(ads1x4s08_mapreg.sys.sendstat == ADS1X4S08_ENABLE)
                result = data.status << 32;
            result += (data.data1 << 24) + (data.data2 << 16) + (data.data3 << 8);
        #endif
        while(result >= 256)
        {
            polynom = 263;
            while((result >> msb) != 1)
                msb--;
            polynom <<= msb - 8;
            result ^= polynom;
        }
        if(result == data.ads1x4s08_crc)
        {
            return true;
        }
        else
        {
            ESP_LOGE(ADC_TAG, "Data transmission errors occured !");
            return false;
        }
    }
    else
    {
        ESP_LOGI(ADC_TAG, "CRC feature has not been enabled !");
        return true;
    }

}

/**
 * @brief Interruption on /DRDY pin from the ADC when data is ready to be retrieved
 *        The interruption is triggered on the negative edge
*/
void ADS1x4S08_ISR_data_ready(void *args){
    int isr = (int)args;
    xQueueSendFromISR(isrQueue, &isr, NULL);
}

/**
 * @brief Get the temperature from the internal sensor in the ADC
 * @retval None
*/
float ADS1x4S08_internal_temperature_sensor(void){
    float Va = 0.129, Ta = 25, alpha = 403e-6, V0, Vx, Tx;

    ADS1x4S08_set_gain(ADS1X4S08_GAIN_4);

    ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_sys_mon = ADS1X4S08_INTERNAL_TEMPERATURE_SENSOR;

    ADS1x4S08_set_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);

    V0 = Va - alpha * Ta;
    ADS1x4S08_get_conversion_data();
    Vx = ADS1x4S08_get_analog_voltage();
    Tx = (Vx - V0) / alpha;

    ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_sys_mon = ADS1X4S08_DISABLE;
    ADS1x4S08_set_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);

    return Tx;
}

/**
 * @brief Reset the ADC and get the default values from every register
 * @retval None
*/
void ADS1x4S08_reset(void){
    ADS1x4S08_Hal_spi_transfer(RESET);
    vTaskDelay(1);

    ADS1x4S08_Hal_spi_transfer(WAKEUP);

    ADS1x4S08_get_default_register();

    ADS1x4S08_set_reference_voltage(ADS1X4S08_INTERNAL_REF_2V5, ADS1X4S08_INTERNAL_REF_ALWAYS_ON);
    ADS1x4S08_set_datarate(ADS1X4S08_DR_20_SPS);

    ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_delay = ADS1X4S08_DELAY_256_TMOD;
    ADS1x4S08_set_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);

    ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_timeout = ADS1X4S08_ENABLE;
    ADS1x4S08_set_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);

    ads1x4s08_mapreg.ads1x4s08_ref.ads1x4s08_refp_buf = !ADS1X4S08_DISABLE;
    ADS1x4S08_set_register(ADS1X4S08_REG_REF, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_ref);
}

/**
 * @brief Initialization of the ADC driver
*/
void ADS1x4S08_init(gpio_num_t adc_start, gpio_num_t adc_drdy, gpio_num_t adc_reset, gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk, gpio_num_t cs){
    gpio_set_direction(adc_start, GPIO_MODE_OUTPUT);
    gpio_set_direction(adc_drdy, GPIO_MODE_INPUT);
    gpio_set_direction(adc_reset, GPIO_MODE_OUTPUT);
    gpio_set_direction(cs, GPIO_MODE_OUTPUT);

    gpio_set_level(adc_start, 0);   // When using START command of ADC
    
    gpio_set_level(adc_reset, 0);   // RESET pin  active low
    vTaskDelay(10);
    gpio_set_level(adc_reset, 1);   // RESET pin  active low
    
    gpio_set_level(cs, 0);          // To communicate with ADS1x4S08, always keep /CS low (works only if device is alone on is spi bus)

    // ISR definition
    gpio_set_intr_type(adc_drdy, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(adc_drdy, ADS1x4S08_ISR_data_ready, NULL);
    gpio_intr_enable(adc_drdy);

    isrQueue = xQueueCreate(1, sizeof(int));

    ADS1x4S08_Hal_spi_init(mosi, miso, sclk, cs);
    ESP_LOGI(ADC_TAG, "The SPI driver is installed");
    ADS1x4S08_get_default_register();
    printf("ads1x4s08_mapreg.ads1x4s08_device_id: %u", ads1x4s08_mapreg.ads1x4s08_device_id);
    if((ads1x4s08_mapreg.ads1x4s08_device_id & 0b111) == 0b100)
    {
        #if !DATA_FORMAT
            ESP_LOGI(ADC_TAG, "ADS1X4S08_REG_ID = ADS114S08");
        #elif DATA_FORMAT
            ESP_LOGI(ADC_TAG, "ADS1X4S08_REG_ID = ADS124S08");
        #endif
    }
    else
    {
        ESP_LOGE(ADC_TAG, "Wrong device !");
        ESP_ERROR_CHECK(1);
    }

    ADS1x4S08_set_reference_voltage(ADS1X4S08_INTERNAL_REF_2V5, ADS1X4S08_INTERNAL_REF_ALWAYS_ON);

    ADS1x4S08_set_datarate(ADS1X4S08_DR_5_SPS);

    ads1x4s08_mapreg.ads1x4s08_sys.ads1x4s08_timeout = ADS1X4S08_ENABLE;
    ADS1x4S08_set_register(ADS1X4S08_REG_SYS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_sys);

    ads1x4s08_mapreg.ads1x4s08_ref.ads1x4s08_refp_buf = !ADS1X4S08_DISABLE;
    ADS1x4S08_set_register(ADS1X4S08_REG_REF, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_ref);

    ads1x4s08_mapreg.ads1x4s08_idacmag.ads1x4s08_fl_rail_en = ADS1X4S08_ENABLE;
    ADS1x4S08_set_register(ADS1X4S08_REG_IDACMAG  , (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_idacmag);
}