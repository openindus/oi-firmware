/**
 * @file mcp25625.c
 * @brief MCP25625 Driver
 * @author Mani Gillier <mani.gillier@openindus.com>
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "mcp25625.h"
#include "OSAL.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "freertos/projdefs.h"
#include "mcp25625_reg.h"
#include <stdint.h>

static char const *const TAG = "MCP25625";

static uint8_t const MCP25625_COMMAND_BITS_NB = 8;
static uint8_t const MCP25625_ADDRESS_BITS_NB = 8;
static int const MCP25625_CLOCK_SPEED         = 4000000U; // Max 10MHz (Table 7-6)
static int const MCP25625_SPI_QUEUE_SIZE      = 16;

struct mcp25625_can mcp25625_can_instance = {
    .host = -1, .cs = -1, .intr = -1, .baudrate = 0, .extended_mode = false, .initialized = false};

TaskHandle_t mcp25625_can_task_handle = NULL;

#define MCP25625_COMMAND_WRITE ((uint8_t)0b00000010)
#define MCP25625_COMMAND_READ ((uint8_t)0b00000011)

[[maybe_unused]]
static esp_err_t reg_write(reg_addr_t reg, reg_value_t value)
{
    spi_transaction_t transaction = {.flags            = 0,
                                     .cmd              = MCP25625_COMMAND_WRITE,
                                     .addr             = reg,
                                     .length           = 8,
                                     .rxlength         = 0,
                                     .override_freq_hz = 0,
                                     .user             = NULL,
                                     .tx_buffer        = (uint8_t[]){value},
                                     .rx_buffer        = NULL};
    esp_err_t err = spi_device_polling_transmit(mcp25625_can_instance.handle, &transaction);
    if (err != ESP_OK) {
        LOGE(TAG, "Failed to write register %#0.2x", reg);
    }
    return err;
}

[[maybe_unused]]
static esp_err_t reg_read(reg_addr_t reg, reg_value_t *value)
{
    spi_transaction_t transaction = {.flags            = 0,
                                     .cmd              = MCP25625_COMMAND_READ,
                                     .addr             = reg,
                                     .length           = 0,
                                     .rxlength         = BYTESIZE,
                                     .override_freq_hz = 0,
                                     .user             = NULL,
                                     .tx_buffer        = NULL,
                                     .rx_buffer        = value};
    esp_err_t err = spi_device_polling_transmit(mcp25625_can_instance.handle, &transaction);
    if (err != ESP_OK) {
        LOGE(TAG, "Failed to read register %#0.2x", reg);
    }
    return err;
}

static char const *error_name(uint8_t mask)
{
    switch (mask) {
    case EFLG_RX1OVR_MASK:
        return "RX1OVR";
    case EFLG_RX0OVR_MASK:
        return "RX0OVR";
    case EFLG_TXBO_MASK:
        return "TXBO";
    case EFLG_TXEP_MASK:
        return "TXEP";
    case EFLG_RXEP_MASK:
        return "RXEP";
    case EFLG_TXWAR_MASK:
        return "TXWAR";
    case EFLG_RXWAR_MASK:
        return "RXWAR";
    case EFLG_EWARN_MASK:
        return "EWARN";
    default:
        return NULL;
    };
}

static char const *interrupt_name(uint8_t mask)
{
    switch (mask) {
    case CANINTF_MERRF_MASK:
        return "MERRF";
    case CANINTF_WAKIF_MASK:
        return "WAKIF";
    case CANINTF_ERRIF_MASK:
        return "ERRIF";
    case CANINTF_TX2IF_MASK:
        return "TX2IF";
    case CANINTF_TX1IF_MASK:
        return "TX1IF";
    case CANINTF_TX0IF_MASK:
        return "TX0IF";
    case CANINTF_RX1IF_MASK:
        return "RX1IF";
    case CANINTF_RX0IF_MASK:
        return "RX0IF";
    default:
        return NULL;
    };
}

#define LOG_BUF_SIZE 256
static void log_register(reg_value_t value, char const *const reg_name,
                         char const *(*get_name)(uint8_t mask))
{
    char buf[LOG_BUF_SIZE] = {0};
    uint16_t index         = 0;

    for (uint8_t bit_index = 0; bit_index < BYTESIZE; bit_index++) {
        char const *name = get_name(value & (0x01 << bit_index));
        if (name == NULL) {
            continue;
        }
        if (index) {
            index += snprintf(buf + index, sizeof(buf) - index - 1, " ");
        }
        index += snprintf(buf + index, sizeof(buf) - index - 1, "%s", name);
    }
    LOGI(TAG, "%s: %#0.2x [%s]", reg_name, value, buf);
}

esp_err_t reg_write_bitfield(reg_addr_t addr, reg_bitfield_t bitfield)
{

    esp_err_t err    = ESP_OK;
    uint8_t mask     = EXTRACT_BF_MASK(bitfield);
    uint8_t value    = EXTRACT_BF_VALUES(bitfield) & mask;
    reg_value_t read = 0x00;

    err = reg_read(addr, &read);
    if (err != ESP_OK) {
        goto end;
    }
    value |= read & (~mask);
    err = reg_write(addr, value);
end:
    return err;
}

struct message {
    uint16_t sid : 11;
    uint8_t srr : 1;
    uint8_t ide : 1;
    uint32_t eid : 29;
    uint8_t rtr : 1;
    uint8_t dlc : 4;
    uint8_t data[8];
};

static esp_err_t read_message()
{
    esp_err_t err = ESP_OK;

    uint8_t buffer[13] = {0};
    for (uint8_t index = 0; index < sizeof(buffer); index++) {
        err = reg_read(0x61 + index, &buffer[index]);
        if (err != ESP_OK) {
            goto err;
        }
    }
    struct message msg = {.sid = (buffer[0] << 3) | ((buffer[1] & 0xE0) >> 5),
                          .srr = (buffer[1] & 0x10) >> 4,
                          .ide = (buffer[1] & 0x08) >> 3,
                          .eid = (buffer[0] << 21) | ((buffer[1] & 0xE0) << 13) |
                                 ((buffer[1] & 0x03) << 16) | (buffer[2] << 8) | buffer[3],
                          .rtr  = ((buffer[4] & 0x40) >> 6),
                          .dlc  = buffer[4] & 0x0F,
                          .data = {buffer[5], buffer[6], buffer[7], buffer[8], buffer[9],
                                   buffer[10], buffer[11], buffer[12]}};
    LOGI(TAG,
         "Received message: SID: %#0.3x, EID: %#0.5x, IDE: %d, RTR: %d, DLC: %d, Data: %02x %02x "
         "%02x %02x %02x %02x %02x %02x",
         msg.sid, msg.eid, msg.ide, msg.rtr, msg.dlc, msg.data[0], msg.data[1], msg.data[2],
         msg.data[3], msg.data[4], msg.data[5], msg.data[6], msg.data[7]);
    return err;
err:
    LOGE(TAG, "Failed to read message");
    return err;
}

static esp_err_t manage_interrupt(reg_value_t mask)
{
    switch (mask) {
    case CANINTF_MERRF_MASK:
        LOGW(TAG, "CAN MESSAGE ERROR");
        return ESP_OK;
    case CANINTF_WAKIF_MASK:
        return ESP_OK;
    case CANINTF_ERRIF_MASK:
        LOGW(TAG, "CAN ERROR");
        uint8_t value = 0x00;
        reg_read(REG_EFLG, &value);
        log_register(value, "Errors", &error_name);
        return ESP_ERR_NOT_SUPPORTED;
    case CANINTF_TX2IF_MASK:
        return ESP_OK;
    case CANINTF_TX1IF_MASK:
        return ESP_OK;
    case CANINTF_TX0IF_MASK:
        LOGI(TAG, "TX buffer is now empty");
        return ESP_OK;
    case CANINTF_RX1IF_MASK:
        return ESP_OK;
    case CANINTF_RX0IF_MASK:
        LOGW(TAG, "RX buffer is full, you have an incomming message");
        esp_err_t err = read_message();
        return err;
    default:
        return ESP_ERR_INVALID_ARG;
    }
}

void mcp25625_can_task(void *args)
{
    (void)args;
    uint8_t value = 0x00;
    while (1) {
        // Wait for interrupt
        ulTaskNotifyTake(0, portMAX_DELAY);
        if (reg_read(REG_CANINTF, &value) != ESP_OK) {
            LOGE(TAG, "Failed to read interrupt register");
            continue;
        }
        log_register(value, "Interrupts", &interrupt_name);
        for (uint8_t bit_index = 0; bit_index < BYTESIZE; bit_index++) {
            if (!(value & (0x01 << bit_index))) {
                continue;
            }
            if (manage_interrupt(value & (0x01 << bit_index)) == ESP_OK) {
                value &= ~(0x01 << bit_index);
            }
        }
        // Clear treated interrupts
        reg_write(REG_CANINTF, value);
        if (value) {
            LOGW(TAG, "Failed to clear interrupts: %#0.2x", value);
        }
    }
    vTaskDelete(NULL);
}

IRAM_ATTR void mcp25625_isr(void *args)
{
    (void)args;
    xTaskNotifyGive(mcp25625_can_task_handle);
}

static esp_err_t mcp25625_init_isr(gpio_num_t intr)
{
    gpio_config_t gpio_interrupt_config = {.intr_type    = GPIO_INTR_NEGEDGE,
                                           .mode         = GPIO_MODE_INPUT,
                                           .pin_bit_mask = (1ULL << intr),
                                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                           .pull_up_en   = GPIO_PULLUP_DISABLE};
    esp_err_t err                       = gpio_config(&gpio_interrupt_config);
    if (err != ESP_OK) {
        LOGE(TAG, "Failed to init gpio config for interrupt: %d", err);
        goto end;
    }
    err = gpio_isr_handler_add(intr, mcp25625_isr, NULL);
end:
    return err;
}

esp_err_t mcp25625_can_init(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr)
{
    mcp25625_can_instance.host = host;
    mcp25625_can_instance.cs   = cs;
    mcp25625_can_instance.intr = intr;
    // Init spi device
    spi_device_interface_config_t device_config = {
        .command_bits     = MCP25625_COMMAND_BITS_NB,
        .address_bits     = MCP25625_ADDRESS_BITS_NB,
        .dummy_bits       = 0,
        .mode             = 0, // MCP25625 5.0 (0,0)
        .clock_source     = SPI_CLK_SRC_DEFAULT,
        .duty_cycle_pos   = 0,
        .cs_ena_pretrans  = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz   = MCP25625_CLOCK_SPEED,
        .input_delay_ns   = 0,
        .sample_point     = SPI_SAMPLING_POINT_PHASE_0, // default in esp-idf
        .spics_io_num     = cs,
        .flags            = SPI_DEVICE_HALFDUPLEX,
        .queue_size       = MCP25625_SPI_QUEUE_SIZE,
        .pre_cb           = NULL,
        .post_cb          = NULL};
    esp_err_t err = spi_bus_add_device(host, &device_config, &mcp25625_can_instance.handle);
    if (err != ESP_OK) {
        LOGE(TAG, "Failed to add the device to the spi bus : %d", err);
    }
    return err;
}

void mcp25625_can_begin(unsigned long baudrate, bool extended_mode)
{
    mcp25625_can_instance.baudrate      = baudrate;
    mcp25625_can_instance.extended_mode = extended_mode;

    BaseType_t ret =
        xTaskCreate(mcp25625_can_task, "mcp25625", 4096, NULL, 10, &mcp25625_can_task_handle);
    if (ret != pdPASS) {
        LOGE(TAG, "Failed to instantiate MCP25625 task");
        return;
    }
    esp_err_t err = mcp25625_init_isr(mcp25625_can_instance.intr);
    if (err != ESP_OK) {
        LOGE(TAG, "Failed to instantiate MCP25625 interrupt");
        return;
    }
    reg_bitfield_t cnf1 = BITFIELD_P(CNF1, SJW, 0b11) | BITFIELD_P(CNF1, BRP, 0);
    reg_bitfield_t cnf2 = BITFIELD(CNF2, BTLMODE, ON) | BITFIELD(CNF2, SAM, ONCE) |
                          BITFIELD_P(CNF2, PHSEG1, 3) | BITFIELD_P(CNF2, PRSEG, 1);
    reg_bitfield_t cnf3 = BITFIELD_P(CNF3, PHSEG2, 2);

    reg_write_bitfield(REG_CNF1, cnf1);
    reg_write_bitfield(REG_CNF2, cnf2);
    reg_write_bitfield(REG_CNF3, cnf3);
    reg_write(REG_CANINTE, CANINTE_ALL_ON);
    reg_write(0x60, 0x60); // RXB0CTRL - disable masks & filters
    reg_value_t value = 0x00;

    reg_bitfield_t canctrl = BITFIELD(CANCTRL, REQOP, NORMAL);
    reg_write_bitfield(REG_CANCTRL, canctrl);

    reg_read(0x0E, &value);

    LOGI(TAG, "MCP25625 Can ready : %#0.2x", value);
    mcp25625_can_instance.initialized = true;
}

void mcp25625_can_stop()
{
    gpio_isr_handler_remove(mcp25625_can_instance.intr);
    vTaskDelete(mcp25625_can_task_handle);
    mcp25625_can_instance.initialized = false;
}

void mcp25625_can_read()
{
    /*
    reg_value_t value = 0x00;
    reg_read(REG_CANINTF, &value);
    log_register(value, "Interrupts", &interrupt_name);
    */
}
