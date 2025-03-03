/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file sc16is750.c
 * @brief Functions for SC16IS750 SPI to UART bridge
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "sc16is750.h"

static const char SC16IS750_TAG[] = "sc16is750";

#define SC16IS750_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                      \
    {                                                                                       \
        if (a)                                                                              \
        {                                                                                   \
            ESP_LOGE(SC16IS750_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                                  \
        }                                                                                   \
    } while (0)

//Private variables
IRAM_ATTR static void SC16IS750_isr_handler(void* arg);
static void SC16IS750_task(void* arg);
TaskHandle_t intrTaskHandle;
SemaphoreHandle_t xSemaphoreQueue;
SemaphoreHandle_t xSemaphoreSpi;

static SC16IS750_rxQueue_t rx_queue; // do not remove static

static SC16IS750_DeviceConfig_t _deviceConfig;
static spi_device_handle_t _spiHandler = NULL;
static bool _spiInitialized = false;
// Save config settings
SC16IS750_SerialConfig_t _config;

esp_err_t SC16IS750_create(SC16IS750_DeviceConfig_t *config)
{
    assert(config != NULL);
    _deviceConfig = *config;    

    ESP_LOGV(SC16IS750_TAG, "Device configured");

    gpio_config_t intr_config;
    intr_config.intr_type = GPIO_INTR_NEGEDGE;
    intr_config.mode = GPIO_MODE_INPUT;
    intr_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    intr_config.pull_up_en = GPIO_PULLUP_DISABLE;

    intr_config.pin_bit_mask = (1ULL<<(_deviceConfig.spi_pin_int));

    ESP_ERROR_CHECK(gpio_config(&intr_config));
    gpio_isr_handler_add(_deviceConfig.spi_pin_int, SC16IS750_isr_handler, NULL);
    gpio_intr_enable(_deviceConfig.spi_pin_int);

    ESP_LOGV(SC16IS750_TAG, "RS User Interrupt pin configured");

    rx_queue.head = 0;
    rx_queue.tail = 0;
    rx_queue.count = 0;

    ESP_LOGV(SC16IS750_TAG, "Internal receive & send buffer are initialised");

    spi_device_interface_config_t devConfig = {
        .command_bits = 0,
        .address_bits = 8,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = _deviceConfig.spi_freq,
        .input_delay_ns = 0,
        .spics_io_num = _deviceConfig.spi_pin_cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    SC16IS750_CHECK(spi_bus_add_device(_deviceConfig.spi_host, &devConfig, &_spiHandler), "error while adding device to spi bus", err);
    _spiInitialized = true;
    ESP_LOGV(SC16IS750_TAG, "SPI configured");

    xTaskCreate(SC16IS750_task, "SC16IS750_task", 4096, NULL, 10, &intrTaskHandle);
    xSemaphoreQueue = xSemaphoreCreateMutex();
    xSemaphoreSpi = xSemaphoreCreateBinary();
    ESP_LOGV(SC16IS750_TAG, "Internal Tasks created");

    return ESP_OK;

    err:
        return ESP_FAIL;
}

esp_err_t SC16IS750_delete()
{   
    SC16IS750_CHECK(spi_bus_remove_device(_spiHandler), "error while removing device to spi bus", err);
    _spiInitialized = false;

    gpio_isr_handler_remove(_deviceConfig.spi_pin_int);
    gpio_intr_disable(_deviceConfig.spi_pin_int);

    vTaskDelete(intrTaskHandle);

    ESP_LOGV(SC16IS750_TAG, "Device deinit successful");

    return ESP_OK;
    err:
        return ESP_FAIL;
}

int queue_push(uint8_t element)
{
    xSemaphoreTake(xSemaphoreQueue, portTICK_PERIOD_MS);
    if((rx_queue.count == SC16IS750_RX_QUEUE_SIZE))
    {
        ESP_LOGV(SC16IS750_TAG, "RX internal queue is full");
        return -1;
    }

    rx_queue.e[rx_queue.tail] = element;
    rx_queue.tail = (rx_queue.tail + 1) % SC16IS750_RX_QUEUE_SIZE;
    rx_queue.count++;
    xSemaphoreGive(xSemaphoreQueue);
    return 0;
}

int queue_pop(uint8_t * element)
{
    xSemaphoreTake(xSemaphoreQueue, portTICK_PERIOD_MS);
    if(!(rx_queue.count == SC16IS750_RX_QUEUE_SIZE) && (rx_queue.head == rx_queue.tail)) //queue is empty ?
    {
        ESP_LOGV(SC16IS750_TAG, "RX internal queue is empty");
        return -1;
    }
    ESP_LOGV(SC16IS750_TAG, "RX internal queue number of chars before reading: %d", rx_queue.count);
    *element = rx_queue.e[rx_queue.head];
    rx_queue.head = (rx_queue.head + 1) % SC16IS750_RX_QUEUE_SIZE;
    rx_queue.count--;
    ESP_LOGV(SC16IS750_TAG, "RX internal queue number of chars after reading: %d", rx_queue.count);
    xSemaphoreGive(xSemaphoreQueue);
    return 0;
}

int queue_size()
{
    int temp;
    xSemaphoreTake(xSemaphoreQueue, portTICK_PERIOD_MS);
    temp = rx_queue.count;
    xSemaphoreGive(xSemaphoreQueue);
    return temp;
}

int SC16IS750_queue_available()
{
    int size = queue_size();
    return size;
}

int SC16IS750_queue_read()
{
    uint8_t tmp;
    queue_pop(&tmp);
    return tmp;
}


int SC16IS750_queue_read_bytes(uint8_t *data, int len)
{
    int avail = queue_size();
    if(len < avail)
    {
        avail = len;
    }

    uint8_t tmp[avail];
    for(int i = 0; i < avail; i++)
    {
        queue_pop(&tmp[i]);
    }
    
    memcpy(data, &tmp[0], avail*sizeof(uint8_t));
    return 0;
}

void SC16IS750_flush() 
{
    // FCR is Write Only, use saved _config

    // reset TXFIFO, reset RXFIFO, non FIFO mode  
    ESP_LOGV(SC16IS750_TAG, "Flush TX and RX FIFO");
    SC16IS750_writeRegister(FCR, FCR_TX_FIFO_RST | FCR_RX_FIFO_RST);

    if (_config.fifoenable)
    {
        // enable FIFO mode and set FIFO control values  
        SC16IS750_writeRegister(FCR, _config.fifoformat | FCR_ENABLE_FIFO);
    }

    else
    {
        // disable FIFO mode and set FIFO control values  
        SC16IS750_writeRegister(FCR, _config.fifoformat);
    }
}

uint8_t SC16IS750_readable() 
{
    uint8_t data_reg;
    SC16IS750_readRegister(LSR, &data_reg);
    if(data_reg & LSR_DR) // Data in Receiver Bit, at least one character waiting
    { 
        ESP_LOGV(SC16IS750_TAG, "There is at least one byte in RX FIFO");
        return 1;
    }
    else 
    {
        ESP_LOGV(SC16IS750_TAG, "RX FIFO is empty");
        return 0; 
    }
}

uint8_t SC16IS750_readableCount() 
{
    uint8_t data_reg;
    SC16IS750_readRegister(RXLVL, &data_reg);
    ESP_LOGV(SC16IS750_TAG, "Number of chars in RX FIFO : %d", data_reg);
    return data_reg;
}

uint8_t SC16IS750_writable() 
{
    uint8_t data_reg;
    SC16IS750_readRegister(LSR, &data_reg);
    if (data_reg & LSR_THRE) // THR Empty, space for at least one character
    { 
        ESP_LOGV(SC16IS750_TAG, "There is space for at least one byte in TX FIFO");
        return 1;
    }
    else 
    {
        ESP_LOGV(SC16IS750_TAG, "No space in TX FIFO");
        return 0;  
    }
}

uint8_t SC16IS750_writableCount() 
{
    uint8_t data_reg;
    SC16IS750_readRegister(TXLVL, &data_reg);
    ESP_LOGV(SC16IS750_TAG, "Number of space in TX FIFO : %d", data_reg);
    return data_reg;  // TX Level
}

uint8_t SC16IS750_getc() 
{
    ESP_LOGV(SC16IS750_TAG, "Read char from device SPI/UART");
    uint8_t data_reg;
    if (!SC16IS750_readable()) 
    {
        return -1;
    }
    SC16IS750_readRegister(RHR, &data_reg);

    return data_reg;
}

void SC16IS750_putc(int value) 
{
    ESP_LOGV(SC16IS750_TAG, "Write char to device SPI/UART");
    uint8_t data_reg =0;    
    while ( data_reg == 0) 
    {
        // Wait for space in TX buffer
        SC16IS750_readRegister(TXLVL, &data_reg);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    SC16IS750_writeRegister(THR, value);
}

void SC16IS750_writeBytes(uint8_t *data, int len)
{   
    ESP_LOGV(SC16IS750_TAG, "Write bytes to device SPI/UART");
    uint8_t data_reg=0;

    // Write blocks of BULK_BLOCK_LEN  
    while (len > 0) 
    {
        ESP_LOGV(SC16IS750_TAG, "LEN remaining to send : %d \n", len);
        if(len < SC16IS750_FIFO_TX)
        {
            while(data_reg < len) 
            {
                // Wait for space in TX buffer
                SC16IS750_readRegister(TXLVL, &data_reg);
                vTaskDelay(pdMS_TO_TICKS(1));
            }

            // Write a block of BULK_BLOCK_LEN bytes
            // optimized
            SC16IS750_hal_write(SC16IS750_WRITE_CMD(THR), data, len);  

            len  -= len;
            data += len;
            data_reg=0;
        }

        else
        {
            while(data_reg < SC16IS750_FIFO_TX) 
            {
                // Wait for space in TX buffer
                SC16IS750_readRegister(TXLVL, &data_reg);
                vTaskDelay(pdMS_TO_TICKS(1));
            }

            // Write a block of BULK_BLOCK_LEN bytes
            // optimized
            SC16IS750_hal_write(SC16IS750_WRITE_CMD(THR), data, SC16IS750_FIFO_TX);    
                
            len  -= SC16IS750_FIFO_TX;
            data += SC16IS750_FIFO_TX;
            data_reg=0;
        }
    }
}

void SC16IS750_hal_write(uint8_t reg, uint8_t *frame, int len)
{
    if (_spiInitialized == true)
    {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = reg,
            .length = 8*len,
            .rxlength = 8,
            .user = NULL,
            .tx_buffer = NULL,
            .rx_buffer = NULL
        };

        trans.tx_buffer = &frame[0];
        xSemaphoreTake(xSemaphoreSpi, portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));
        xSemaphoreGive(xSemaphoreSpi);
    }

    else
    {
        ESP_LOGE(SC16IS750_TAG, "Configure SPI before transaction");
    }
}

void SC16IS750_hal_read(uint8_t reg, uint8_t *receive_frame, int len)
{
    uint8_t temp[len];
    if (_spiInitialized == true)
    {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = reg,
            .length = 8*(len),
            .rxlength = 8*(len),
            .user = NULL,
            .tx_buffer = NULL,
            .rx_buffer = NULL
        };

        trans.tx_buffer = &temp[0];
        trans.rx_buffer = &receive_frame[0];
        xSemaphoreTake(xSemaphoreSpi, portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));
        xSemaphoreGive(xSemaphoreSpi);
    }

    else
    {
        ESP_LOGE(SC16IS750_TAG, "Configure SPI before transaction");
    }
    
}

void SC16IS750_writeRegister(SC16IS750_Register_t reg, uint8_t data)
{
    ESP_LOGV(SC16IS750_TAG, "Write SPI byte to the device");;
    SC16IS750_hal_write(SC16IS750_WRITE_CMD(reg), &data, 1);

}

void SC16IS750_readRegister(SC16IS750_Register_t reg, uint8_t *data)
{
    ESP_LOGV(SC16IS750_TAG, "Read device SPI byte");
    SC16IS750_hal_read(SC16IS750_READ_CMD(reg), data, 1);
}

void SC16IS750_readDataBlock(uint8_t *data, int len)
{
    ESP_LOGV(SC16IS750_TAG, "Read block to device in SPI");
    SC16IS750_hal_read(SC16IS750_READ_CMD(THR), data, len);
}

void SC16IS750_swReset() 
{
    SC16IS750_writeRegister(IOCTRL, IOC_SW_RST); 
    ESP_LOGV(SC16IS750_TAG, "Software reset");    
}


void SC16IS750_ioGetState(uint8_t *bits) 
{
    SC16IS750_readRegister(IOSTATE, bits);
    ESP_LOGV(SC16IS750_TAG, "Get level of output pins");
}


void SC16IS750_ioSetState(uint8_t bits) 
{  
    SC16IS750_writeRegister(IOSTATE, bits);
    ESP_LOGV(SC16IS750_TAG, "Set level of outputs");
}

void SC16IS750_ioSetDirection(uint8_t bits)
{
    SC16IS750_writeRegister(IODIR, bits);
    ESP_LOGV(SC16IS750_TAG, "Set direction of I/O pins");
}

bool SC16IS750_connected() 
{
    // Perform read/write test to check if UART is working
    uint8_t data_test;

    SC16IS750_writeRegister(SPR, 0xE9);
    SC16IS750_readRegister(SPR, &data_test);
    return (data_test == 0xE9);
}

void SC16IS750_baud(int baudrate) 
{
    unsigned long divisor = SC16IS750_BAUDRATE_DIVISOR(baudrate);
    uint8_t lcr_tmp;

    SC16IS750_readRegister(LCR, &lcr_tmp);                            // Read current LCR register
    SC16IS750_writeRegister(LCR, lcr_tmp | LCR_ENABLE_DIV);           // Enable Divisor registers
    SC16IS750_writeRegister(DLL, ( divisor       & 0xFF));            //   write divisor LSB
    SC16IS750_writeRegister(DLH, ((divisor >> 8) & 0xFF));            //   write divisor MSB
    SC16IS750_writeRegister(LCR, lcr_tmp);                            // Restore LCR register, activate regular RBR, THR and IER registers  
    ESP_LOGV(SC16IS750_TAG, "Baudrate is set to %d", baudrate);
}

void SC16IS750_send_break() 
{
    // Wait for 1.5 frames before clearing the break condition
    // This will have different effects on our platforms, but should
    // ensure that we keep the break active for at least one frame.
    // We consider a full frame (1 start bit + 8 data bits bits + 
    // 1 parity bit + 2 stop bits = 12 bits) for computation.
    // One bit time (in us) = 1000000/_baud
    // Twelve bits: 12000000/baud delay
    // 1.5 frames: 18000000/baud delay
    SC16IS750_set_break(true);
    ets_delay_us((18000000/_config.baudrate));
    SC16IS750_set_break(false);    
}

void SC16IS750_set_break(bool enable) 
{ 
    if (enable) 
    {
        _config.dataformat |= LCR_BRK_ENA;      // Save dataformat         
    }
    else 
    {
        _config.dataformat &= ~LCR_BRK_ENA;     // Save dataformat           
    }    
    SC16IS750_writeRegister(LCR, _config.dataformat); // Set LCR register
}

void SC16IS750_set_flow_control(SC16IS750_Flow_t type) {

    uint8_t lcr_tmp; 
    uint8_t efr_tmp = 0x00;

    // We need to enable flow control to prevent overflow of buffers and
    // lose data when used with fast devices like the WiFly.

    switch (type) 
    {
        case Disabled : 
            ESP_LOGV(SC16IS750_TAG, "Hardware flow control is disabled");
            break;
        case RTS:       
            efr_tmp = EFR_ENABLE_RTS;
            ESP_LOGV(SC16IS750_TAG, "Hardware flow control use RTS only");
            break;     
        case CTS:       
            ESP_LOGV(SC16IS750_TAG, "Hardware flow control use CTS only");
            efr_tmp = EFR_ENABLE_CTS;                     
            break;     
        case RTSCTS:    
            ESP_LOGV(SC16IS750_TAG, "Hardware flow control use RTS and CTS");
            efr_tmp = EFR_ENABLE_RTS | EFR_ENABLE_CTS;
            break;

        default:             ;    
    }

    //Save flowcontrol mode and enable enhanced functions  
    _config.flowctrl = efr_tmp | EFR_ENABLE_ENHANCED_FUNCTIONS;

    SC16IS750_readRegister(LCR, &lcr_tmp);                       // save LRC register
    SC16IS750_writeRegister(LCR, LCR_ENABLE_ENHANCED_FUNCTIONS); // write magic number 0xBF to enable access to EFR register
    SC16IS750_writeRegister(EFR, _config.flowctrl);              // set flow and enable enhanced functions
    SC16IS750_writeRegister(LCR, lcr_tmp);                       // restore LCR register
    ESP_LOGV(SC16IS750_TAG, "Hardware flow control");

}  

void SC16IS750_set_flow_triggers(int resume, int halt) 
{
    // sanity checks
    halt = halt & 0x0F;
    resume = resume & 0x0F;  
    if (halt <= resume) 
    {
        halt   = TCR_HALT_DEFAULT;
        resume = TCR_RESUME_DEFAULT;  
    }

    // Note: TCR accessible only when EFR[4]=1 and MCR[2]=1
    SC16IS750_writeRegister(TCR, (resume << 4) | halt);          // set TCR register
    ESP_LOGV(SC16IS750_TAG, "Flow triggers are set");

}

void SC16IS750_set_modem_control() 
{
    //Note MCR[7:4] and MCR[2] only accessible when EFR[4] is set
    if (SC16IS750_PRESCALER == SC16IS750_PRESCALER_1) 
    { // Default prescaler after reset
        SC16IS750_writeRegister(MCR, (MCR_PRESCALE_1 | MCR_ENABLE_TCR_TLR));
    }  
    else 
    { 
        SC16IS750_writeRegister(MCR, MCR_PRESCALE_4 | MCR_ENABLE_TCR_TLR);
    }
    ESP_LOGV(SC16IS750_TAG, "Modem Control is set");
}  

void SC16IS750_format(int bits, SC16IS750_Parity_t parity, int stop_bits)
{
    char lcr_tmp = 0x00;
    
    switch (bits) {
        case 5:     lcr_tmp |= LCR_BITS5; break;
        case 6:     lcr_tmp |= LCR_BITS6; break;
        case 7:     lcr_tmp |= LCR_BITS7; break;
        case 8:     lcr_tmp |= LCR_BITS8; break;
        default:    lcr_tmp |= LCR_BITS8;
    }
    
    switch (parity) {
        case None:      lcr_tmp |= LCR_NONE; break;
        case Odd:       lcr_tmp |= LCR_ODD; break;
        case Even:      lcr_tmp |= LCR_EVEN; break;
        case Forced1:   lcr_tmp |= LCR_FORCED1; break;
        case Forced0:   lcr_tmp |= LCR_FORCED0; break;                      
        default:        lcr_tmp |= LCR_NONE;     
    }
    
    switch (stop_bits) {
        case 1:     lcr_tmp |= LCR_BITS1; break;
        case 2:     lcr_tmp |= LCR_BITS2; break;
        default:    lcr_tmp |= LCR_BITS1;     
    }
    
    _config.dataformat = lcr_tmp;           // Save dataformat
    SC16IS750_writeRegister(LCR, lcr_tmp);  // Set LCR register, activate regular RBR, THR and IER registers  
    ESP_LOGV(SC16IS750_TAG, "Data format is set");    
}

void SC16IS750_set_fifo_control() 
{
    // Set default fifoformat 
    // FCR
    _config.fifoenable = true;  
    
    // Note FCR[5:4] (=TX_IRQ_LVL) only accessible when EFR[4] is set (enhanced functions enable)
    //  _config.fifoformat = FCR_RX_IRQ_8 | FCR_TX_IRQ_56;
    _config.fifoformat = FCR_RX_IRQ_8 | FCR_TX_IRQ_8;  //Default
    
    if (_config.fifoenable)
    {
        // enable FIFO mode and set FIFO control values  
        SC16IS750_writeRegister(FCR, _config.fifoformat | FCR_ENABLE_FIFO);
    }
        
    else
    {
        // disable FIFO mode and set FIFO control values  
        SC16IS750_writeRegister(FCR, _config.fifoformat);
    }

    // Set Trigger level register TLR for RX and TX interrupt generation
    // Note TLR only accessible when EFR[4] is set (enhanced functions enable) and MCR[2] is set
    //   TRL Trigger levels for RX and TX are 0..15, meaning 0-60 with a granularity of 4 chars    
    // When TLR for RX or TX are 'Zero' the corresponding values in FCR are used. The FCR settings
    // have less resolution (only 4 levels) so TLR is considered an enhanced function.
    SC16IS750_writeRegister(TLR, 0x00);                                     // Use FCR Levels
    //  this->writeRegister(TLR, (TLR_RX_DEFAULT << 4) | TLR_TX_DEFAULT);   // Use Default enhanced levels
    ESP_LOGV(SC16IS750_TAG, "FIFO Control is set");
 
} 

void SC16IS750_setAutoRts()
{
    SC16IS750_writeRegister(EFCR, EFCR_ENA_TX_RTS);
}

void SC16IS750_init(SC16IS750_SerialParams_t cfg) 
{
    // Initialise SC16IS750
    
    // Software reset, assuming there is no access to the HW Reset pin
    SC16IS750_swReset();
    
    // Set default baudrate (depends on prescaler) and save in _config
    // DLL/DLH  
    SC16IS750_baud(cfg.baudrate);
    
    // Set default dataformat and save in _config
    // LCR 
    SC16IS750_format(cfg.format_bits, cfg.parity, cfg.format_stop_bits);  

    // Set RTS trigger levels
    // Note TCR only accessible when EFR[4] is set (enhanced functions enable) and MCR[2] is set
    SC16IS750_set_flow_triggers(cfg.trigger_resume, cfg.trigger_halt);
    
    // Set dataflow mode and Enables enhanced functions
    SC16IS750_set_flow_control(cfg.flow_type); 
    
    // FIFO control, sets TX and RX IRQ trigger levels and enables FIFO and save in _config
    // Note FCR[5:4] only accessible when EFR[4] is set (enhanced functions enable)
    // FCR, TLR
    SC16IS750_set_fifo_control();
    SC16IS750_flush();
    
    // Modem control, sets prescaler, enable TCR and TLR
    // Note MCR[7:4] and MCR[2] only accessible when EFR[4] is set (enhanced functions enable)
    SC16IS750_set_modem_control();
    
    SC16IS750_writeRegister(IER, 0x01); //Enable RX interruption

    // Set default break condition and save in _config
    // LCR   
    //set_break();
    
    // The UART bridge should now be successfully initialised.
    
    // Test if UART bridge is present and initialised
    if(!SC16IS750_connected()) { 
        ESP_LOGE(SC16IS750_TAG, "Failed to initialise device");  
    } 
}

IRAM_ATTR static void SC16IS750_isr_handler(void* arg)
{
    BaseType_t high_task_wakeup = pdFALSE;
    vTaskNotifyGiveFromISR(intrTaskHandle, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static void SC16IS750_task(void* arg)
{
    int readable = 0;
    uint8_t intr_type;
    uint8_t temp_buff[readable];
    while (1)
    {
        // Wait for an interrupt
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        SC16IS750_readRegister(IIR, &intr_type);

        if ((intr_type & 0x37) == IIR_RX_DATA)
        {
            readable = SC16IS750_readableCount();
            if(readable > 0 )
            {
                ESP_LOGV(SC16IS750_TAG, "Readable data : %d", readable);
                if((rx_queue.count + readable < SC16IS750_RX_QUEUE_SIZE))
                {
                    SC16IS750_readDataBlock(&temp_buff[0], readable);
                    for(int i = 0; i < readable; i++)
                    {
                        queue_push(temp_buff[i]);
                    }
                }
                
                else
                {
                    ESP_LOGW(SC16IS750_TAG, "Internal RX buffer has not enought space to read values");
                }
            }
        }
    }
}

