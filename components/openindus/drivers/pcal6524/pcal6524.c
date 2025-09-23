/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ioexpander.c
 * @brief Functions to the ioexpander pcal6524
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "pcal6524.h"

static const char IOEX_TAG[] = "pcal6524";

#define IOEX_CHECK(a, str, goto_tag, ...)                                                   \
    do                                                                                      \
    {                                                                                       \
        if (a)                                                                              \
        {                                                                                   \
            ESP_LOGE(IOEX_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__);      \
            goto goto_tag;                                                                  \
        }                                                                                   \
    } while (0)

#define ACK_CHECK_EN                    (0x01)  /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                   (0x00)  /*!< I2C master will not check ack from slave */
#define ACK_VAL                         (0x00)  /*!< I2C ack value */
#define NACK_VAL                        (0x01)  /*!< I2C nack value */

#define IOEX_INTERRUPT_DEFAULT_PRIORITY (10)

#define INPUT_PORT_0                    (0x00)
#define INPUT_PORT_1                    (0x01)
#define INPUT_PORT_2                    (0x02)

#define OUTPUT_PORT_0                   (0x04)
#define OUTPUT_PORT_1                   (0x05)
#define OUTPUT_PORT_2                   (0x06)

#define CONFIGURATION_PORT_0            (0x0C)
#define CONFIGURATION_PORT_1            (0x0D)
#define CONFIGURATION_PORT_2            (0x0E)

#define PULL_ENABLE_PORT_0              (0x4C)
#define PULL_ENABLE_PORT_1              (0x4D)
#define PULL_ENABLE_PORT_2              (0x4E)

#define PULL_MODE_PORT_0                (0x50)
#define PULL_MODE_PORT_1                (0x51)
#define PULL_MODE_PORT_2                (0x52)

#define INTERRUPT_MASK_PORT_0           (0x54)
#define INTERRUPT_MASK_PORT_1           (0x55)
#define INTERRUPT_MASK_PORT_2           (0x56)

#define INTERRUPT_STATUS_PORT_0         (0x58)
#define INTERRUPT_STATUS_PORT_1         (0x59)
#define INTERRUPT_STATUS_PORT_2         (0x5A)

#define INTERRUPT_EDGE_PORT_0A          (0x60)
#define INTERRUPT_EDGE_PORT_0B          (0x61)
#define INTERRUPT_EDGE_PORT_1A          (0x62)
#define INTERRUPT_EDGE_PORT_1B          (0x63)
#define INTERRUPT_EDGE_PORT_2A          (0x64)
#define INTERRUPT_EDGE_PORT_2B          (0x65)

#define INTERRUPT_CLEAR_PORT_0          (0x68)
#define INTERRUPT_CLEAR_PORT_1          (0x69)
#define INTERRUPT_CLEAR_PORT_2          (0x6A)


static void ioex_isr_handler(void* arg);
static void ioex_task_interrupt_handler(void* arg);

esp_err_t i2c_write(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t data);
esp_err_t i2c_read(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t *data);
uint8_t ioex_num_to_num(ioex_num_t ioex_num);

esp_err_t add_to_list(ioex_device_t *io, ioex_interrupt_element_t *interrupt_element);
uint8_t find_list_position_from_priority(ioex_device_t *io, uint8_t priority);
esp_err_t remove_from_list(ioex_device_t *io, ioex_num_t ioex_num);
int find_list_position_from_ioex_num(ioex_device_t *io, ioex_num_t ioex_num);

/* -------------------------------------------------------------------------- */
/* -------------------------- Driver's functions ---------------------------- */
/* -------------------------------------------------------------------------- */

ioex_device_t *ioex_create(i2c_port_t i2c_port, uint8_t i2c_address, bool use_interrupt, gpio_num_t interrupt_pin)
{
    ioex_device_t *io = NULL;

    ESP_LOGV(IOEX_TAG, "configuring ioexpander pcal6524...");

    IOEX_CHECK(i2c_address == 0, "address can't be null", err);

    // Creating instance of ioexpander
    io = (ioex_device_t *)malloc(sizeof(ioex_device_t));
    IOEX_CHECK(io == NULL, "request memory for ioexpander pcal6524 failed", err);

    io->address = i2c_address;
    io->i2c_port = i2c_port;

    // reset io
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x00 << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x06, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(io->i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (use_interrupt)
    {
        // Init task
        TaskHandle_t taskHandle;
        xTaskCreate(ioex_task_interrupt_handler, "interrupt_task", 4096, (void*)io, 10, &taskHandle);
        io->interrupt_handle = taskHandle;
        io->interrupt_pin = interrupt_pin;

        //  Init gpio interrupt
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
        io_conf.pin_bit_mask = (1ULL<<interrupt_pin);
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&io_conf);

        // Hook isr handler for specific gpio pin
        gpio_isr_handler_add(interrupt_pin, ioex_isr_handler, (void*)io);

        // Init ISR linked list
        io->interrupt_list = malloc(sizeof(ioex_interrupt_list_t));
        IOEX_CHECK(io->interrupt_list == NULL, "request memory for ioexpander pcal6524 interrupt list failed", err);
        io->interrupt_list->count = 0;
        io->interrupt_list->first = NULL;
    }
    else
    {
        io->interrupt_handle = NULL;
        io->interrupt_list = NULL;
    }

    return io;

err:
    return NULL;
}

esp_err_t ioex_delete(ioex_device_t *io)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);
    ESP_LOGV(IOEX_TAG, "deleting ioexpander pcal6524...");
    if (io->interrupt_handle != NULL)
    {
        gpio_isr_handler_remove(io->interrupt_pin);
        vTaskDelete(io->interrupt_handle);

        for (int i = 0; i < IOEX_NUM_MAX; i++)
        {
            remove_from_list(io, (ioex_num_t)i);
        }

    }
    free(io);
    io = NULL;

    return ESP_OK;

err:
    return ESP_FAIL;
}

esp_err_t ioex_config(ioex_device_t *io, const ioex_config_t *pIOEXConfig)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);
    IOEX_CHECK(pIOEXConfig == NULL, "instance of ioexpander cannot be null", err);

    uint8_t pins = 0;

    /* PORT0 */
    if (pIOEXConfig->pin_bit_mask & 0x000000FF)
    {
        pins = (uint8_t)(pIOEXConfig->pin_bit_mask);
        ioex_set_direction_multiple(io, IOEX_PORT0, pins, pIOEXConfig->mode);
        ioex_set_pull_mode_multiple(io, IOEX_PORT0, pins, pIOEXConfig->pull_mode);
        ioex_set_interrupt_type_multiple(io, IOEX_PORT0, pins, pIOEXConfig->interrupt_type);
    }

    /* PORT1 */
    if (pIOEXConfig->pin_bit_mask & 0x0000FF00)
    {
        pins = (uint8_t)(pIOEXConfig->pin_bit_mask >> 8);
        ioex_set_direction_multiple(io, IOEX_PORT1, pins, pIOEXConfig->mode);
        ioex_set_pull_mode_multiple(io, IOEX_PORT1, pins, pIOEXConfig->pull_mode);
        ioex_set_interrupt_type_multiple(io, IOEX_PORT1, pins, pIOEXConfig->interrupt_type);
    }

    /* PORT2 */
    if (pIOEXConfig->pin_bit_mask & 0x00FF0000)
    {
        pins = (uint8_t)(pIOEXConfig->pin_bit_mask >> 16);
        ioex_set_direction_multiple(io, IOEX_PORT2, pins, pIOEXConfig->mode);
        ioex_set_pull_mode_multiple(io, IOEX_PORT2, pins, pIOEXConfig->pull_mode);
        ioex_set_interrupt_type_multiple(io, IOEX_PORT2, pins, pIOEXConfig->interrupt_type);
    }

    /* ERROR */
    if (pIOEXConfig->pin_bit_mask & 0xFF000000)
    {
        ESP_LOGE(IOEX_TAG, "invalid pin detected in pin_bit_mask");
        goto err;
    }

    return ESP_OK;

err:
    return ESP_ERR_INVALID_ARG;
}

esp_err_t ioex_set_level(ioex_device_t *io, ioex_num_t ioex_num, ioex_level_t level)
{
    uint8_t pins, port;

    if (ioex_num < IOEX_NUM_10)
    {
        pins = (uint8_t)(1U<<ioex_num);
        port = IOEX_PORT0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 8));
        port = IOEX_PORT1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 16));
        port = IOEX_PORT2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "invalid io num");
        return ESP_ERR_INVALID_ARG;
    }
    return ioex_set_level_multiple(io, port, pins, level);
}

esp_err_t ioex_set_level_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_level_t level)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t reg, data = 0;

    if (port == IOEX_PORT0)
    {
        reg = OUTPUT_PORT_0;
    }
    else if (port == IOEX_PORT1)
    {
        reg = OUTPUT_PORT_1;
    }
    else if (port == IOEX_PORT2)
    {
        reg = OUTPUT_PORT_2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto err;
    }

    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg, &data), "error while reading ioexpander register", err);

    if (level == IOEX_HIGH)
    {
        data |= pins;
    }
    else
    {
        data &= ~pins;
    }

    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg, data), "error while writing ioexpander register", err);

    return ESP_OK;

err :
    return ESP_FAIL;
}

int ioex_get_level(ioex_device_t *io, ioex_num_t ioex_num)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t reg, bit, data = 0;

    if (ioex_num < IOEX_NUM_10)
    {
        bit = (uint8_t)(1U<<ioex_num);
        reg = INPUT_PORT_0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        bit =  (uint8_t)(1U<<(ioex_num - 8));
        reg = INPUT_PORT_1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        bit =  (uint8_t)(1U<<(ioex_num - 16));
        reg = INPUT_PORT_2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto err;
    }

    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg, &data), "error while reading ioexpander register", err);

    if ((data & bit) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }

err:
    return -1;
}

esp_err_t ioex_set_direction(ioex_device_t *io, ioex_num_t ioex_num, ioex_mode_t mode)
{
    uint8_t pins, port;

    if (ioex_num < IOEX_NUM_10)
    {
        pins = (uint8_t)(1U<<ioex_num);
        port = IOEX_PORT0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 8));
        port = IOEX_PORT1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 16));
        port = IOEX_PORT2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "invalid io num");
        return ESP_ERR_INVALID_ARG;
    }
    return ioex_set_direction_multiple(io, port, pins, mode);
}

esp_err_t ioex_set_direction_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_mode_t mode)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t reg, data = 0;

    if (port == IOEX_PORT0)
    {
        reg = CONFIGURATION_PORT_0;
    }
    else if (port == IOEX_PORT1)
    {
        reg = CONFIGURATION_PORT_1;
    }
    else if (port == IOEX_PORT2)
    {
        reg = CONFIGURATION_PORT_2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto err;
    }

    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg, &data), "error while reading ioexpander register", err);

    if (mode == IOEX_INPUT)
    {
        data |= pins;
    }
    else
    {
        data &= ~pins;
    }

    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg, data), "error while writing ioexpander register", err);

    return ESP_OK;

err:
    return ESP_FAIL;
}

esp_err_t ioex_set_pull_mode(ioex_device_t *io, ioex_num_t ioex_num, ioex_pull_mode_t pull_mode)
{
    uint8_t pins, port;

    if (ioex_num < IOEX_NUM_10)
    {
        pins = (uint8_t)(1U<<ioex_num);
        port = IOEX_PORT0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 8));
        port = IOEX_PORT1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 16));
        port = IOEX_PORT2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "invalid io num");
        return ESP_ERR_INVALID_ARG;
    }
    return ioex_set_pull_mode_multiple(io, port, pins, pull_mode);
}

esp_err_t ioex_set_pull_mode_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_pull_mode_t pull_mode)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t reg1, reg2, data = 0;

    if (port == IOEX_PORT0)
    {
        reg1 = PULL_ENABLE_PORT_0;
        reg2 = PULL_MODE_PORT_0;
    }
    else if (port == IOEX_PORT1)
    {
        reg1 = PULL_ENABLE_PORT_1;
        reg2 = PULL_MODE_PORT_1;
    }
    else if (port == IOEX_PORT2)
    {
        reg1 = PULL_ENABLE_PORT_2;
        reg2 = PULL_MODE_PORT_2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto err;
    }

    /* First activate or desactivate pull */
    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg1, &data), "error while reading ioexpander register", err);

    if (pull_mode == IOEX_PULLUP || pull_mode == IOEX_PULLDOWN)
    {
        data |= pins;
    }
    else
    {
        data &= ~pins;
    }

    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg1, data), "error while writing ioexpander register", err);

    /* Then configure pull up or pull down */
    if (pull_mode != IOEX_FLOATING)
    {
        IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg2, &data), "error while reading ioexpander register", err);

        if (pull_mode == IOEX_PULLUP)
        {
            data |= pins;
        }
        else 
        {
            data &= ~pins;
        }

        IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg2, data), "error while writing ioexpander register", err);
    }

    return ESP_OK;

err:
    return ESP_FAIL;
}

esp_err_t ioex_pullup_enable(ioex_device_t *io, ioex_num_t ioex_num)
{
    return ioex_set_pull_mode(io, ioex_num, IOEX_PULLUP);
}

esp_err_t ioex_pulldown_enable(ioex_device_t *io, ioex_num_t ioex_num)
{
    return ioex_set_pull_mode(io, ioex_num, IOEX_PULLDOWN);
}

esp_err_t ioex_pull_disable(ioex_device_t *io, ioex_num_t ioex_num)
{
    return ioex_set_pull_mode(io, ioex_num, IOEX_FLOATING);
}

esp_err_t ioex_set_interrupt_type(ioex_device_t *io, ioex_num_t ioex_num, ioex_interrupt_type_t interrupt_type)
{
    uint8_t pins, port;

    if (ioex_num < IOEX_NUM_10)
    {
        pins = (uint8_t)(1U<<ioex_num);
        port = IOEX_PORT0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 8));
        port = IOEX_PORT1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        pins =  (uint8_t)(1U<<(ioex_num - 16));
        port = IOEX_PORT2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "invalid io num");
        return ESP_ERR_INVALID_ARG;
    }
    return ioex_set_interrupt_type_multiple(io, port, pins, interrupt_type);
}

esp_err_t ioex_set_interrupt_type_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_interrupt_type_t interrupt_type)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t reg1, reg2, data1 = 0, data2 = 0;

    if (port == IOEX_PORT0)
    {
        reg1 = INTERRUPT_EDGE_PORT_0A;
        reg2 = INTERRUPT_EDGE_PORT_0B;
    }
    else if (port == IOEX_PORT1)
    {
        reg1 = INTERRUPT_EDGE_PORT_1A;
        reg2 = INTERRUPT_EDGE_PORT_1B;
    }
    else if (port == IOEX_PORT2)
    {
        reg1 = INTERRUPT_EDGE_PORT_2A;
        reg2 = INTERRUPT_EDGE_PORT_2B;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto err;
    }

    /* First activate or desactivate pull */
    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg1, &data1), "error while reading ioexpander register", err);
    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg2, &data2), "error while reading ioexpander register", err);

    /*****************************************************************************************/
    /*   Bit 1  |  Bit 0   |                      Description                                */
    /*---------------------------------------------------------------------------------------*/
    /*     0    |    0     |     level-triggered interrupt                                   */
    /*---------------------------------------------------------------------------------------*/
    /*     0    |    1     |     positive-going (rising) edge triggered interrupt            */
    /*---------------------------------------------------------------------------------------*/
    /*     1    |    0     |     negative-going (falling) edge triggered interrupt           */
    /*---------------------------------------------------------------------------------------*/
    /*     1    |    1     |     any edge (positive or negative-going) triggered interrupt   */
    /*****************************************************************************************/

    if (interrupt_type == IOEX_INTERRUPT_DISABLE) /* we use level-trigger when interrrupt is disable */
    {
        if (pins & 0x01) { data1 &= ~(0b00000011);};
        if (pins & 0x02) { data1 &= ~(0b00001100);};
        if (pins & 0x04) { data1 &= ~(0b00110000);};
        if (pins & 0x08) { data1 &= ~(0b11000000);};
        if (pins & 0x10) { data2 &= ~(0b00000011);};
        if (pins & 0x20) { data2 &= ~(0b00001100);};
        if (pins & 0x40) { data2 &= ~(0b00110000);};
        if (pins & 0x80) { data2 &= ~(0b11000000);};
    }
    else if (interrupt_type == IOEX_INTERRUPT_POSEDGE)
    {
        if (pins & 0x01) { data1 &= ~(0b00000010); data1 |= (0b00000001);};
        if (pins & 0x02) { data1 &= ~(0b00001000); data1 |= (0b00000100);};
        if (pins & 0x04) { data1 &= ~(0b00100000); data1 |= (0b00010000);};
        if (pins & 0x08) { data1 &= ~(0b10000000); data1 |= (0b01000000);};
        if (pins & 0x10) { data2 &= ~(0b00000010); data2 |= (0b00000001);};
        if (pins & 0x20) { data2 &= ~(0b00001000); data2 |= (0b00000100);};
        if (pins & 0x40) { data2 &= ~(0b00100000); data2 |= (0b00010000);};
        if (pins & 0x80) { data2 &= ~(0b10000000); data2 |= (0b00100000);};
    }
    else if (interrupt_type == IOEX_INTERRUPT_NEGEDGE)
    {
        if (pins & 0x01) { data1 &= ~(0b00000001); data1 |= (0b00000010);};
        if (pins & 0x02) { data1 &= ~(0b00000100); data1 |= (0b00001000);};
        if (pins & 0x04) { data1 &= ~(0b00010000); data1 |= (0b00100000);};
        if (pins & 0x08) { data1 &= ~(0b01000000); data1 |= (0b10000000);};
        if (pins & 0x10) { data2 &= ~(0b00000001); data2 |= (0b00000010);};
        if (pins & 0x20) { data2 &= ~(0b00000100); data2 |= (0b00001000);};
        if (pins & 0x40) { data2 &= ~(0b00010000); data2 |= (0b00100000);};
        if (pins & 0x80) { data2 &= ~(0b01000000); data2 |= (0b10000000);};
    }
    else if (interrupt_type == IOEX_INTERRUPT_ANYEDGE)
    {
        if (pins & 0x01) { data1 |= (0b00000011);};
        if (pins & 0x02) { data1 |= (0b00001100);};
        if (pins & 0x04) { data1 |= (0b00110000);};
        if (pins & 0x08) { data1 |= (0b11000000);};
        if (pins & 0x10) { data2 |= (0b00000011);};
        if (pins & 0x20) { data2 |= (0b00001100);};
        if (pins & 0x40) { data2 |= (0b00110000);};
        if (pins & 0x80) { data2 |= (0b11000000);};
    }

    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg1, data1), "error while writing ioexpander register", err);
    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg2, data2), "error while writing ioexpander register", err);

    return ESP_OK;

err:
    return ESP_FAIL;
}

esp_err_t ioex_interrupt_enable(ioex_device_t *io, ioex_num_t ioex_num)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t bit, reg, data = 0;

    if (ioex_num < IOEX_NUM_10)
    {
        bit = (uint8_t)(1U<<ioex_num);
        reg = INTERRUPT_MASK_PORT_0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        bit =  (uint8_t)(1U<<(ioex_num - 8));
        reg = INTERRUPT_MASK_PORT_1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        bit =  (uint8_t)(1U<<(ioex_num - 16));
        reg = INTERRUPT_MASK_PORT_2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto errArg;
    }

    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg, &data), "error while reading ioexpander register", err);

    data &= ~bit;

    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg, data), "error while writing ioexpander register", err);

    return ESP_OK;

err:
    return ESP_FAIL;

errArg:
    return ESP_ERR_INVALID_ARG;
}

esp_err_t ioex_interrupt_disable(ioex_device_t *io, ioex_num_t ioex_num)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", err);

    uint8_t bit, reg, data = 0;

   if (ioex_num < IOEX_NUM_10)
    {
        bit = (uint8_t)(1U<<ioex_num);
        reg = INTERRUPT_MASK_PORT_0;
    }
    else if (ioex_num < IOEX_NUM_20)
    {
        bit =  (uint8_t)(1U<<(ioex_num - 8));
        reg = INTERRUPT_MASK_PORT_1;
    }
    else if (ioex_num < IOEX_NUM_MAX)
    {
        bit =  (uint8_t)(1U<<(ioex_num - 16));
        reg = INTERRUPT_MASK_PORT_2;
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "pin is not valid");
        goto errArg;
    }

    IOEX_CHECK(i2c_read(io->i2c_port, io->address, reg, &data), "error while reading ioexpander register", err);

    data |= bit;

    IOEX_CHECK(i2c_write(io->i2c_port, io->address, reg, data), "error while writing ioexpander register", err);


    return ESP_OK;

err:
    return ESP_FAIL;

errArg:
    return ESP_ERR_INVALID_ARG;
}

esp_err_t ioex_isr_handler_add(ioex_device_t *io, ioex_num_t ioex_num, ioex_isr_t isr_handler, void *args, uint8_t priority)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", errArg);

    IOEX_CHECK(isr_handler == NULL, "handler cannot be null", errArg);

    IOEX_CHECK(io->interrupt_list == NULL, "interrupt are not activated for ioexpander", err);

    // Create interrupt element
    ioex_interrupt_element_t *interrupt_element = malloc(sizeof(ioex_interrupt_element_t));
    IOEX_CHECK(interrupt_element == NULL, "error while allocating memory for interrupt element", err);

    interrupt_element->ioex_num = ioex_num;
    interrupt_element->priority = priority;
    interrupt_element->isr_handler = isr_handler;
    interrupt_element->args = args;
    interrupt_element->next = NULL;

    // add interrupt element to the list
    if (add_to_list(io, interrupt_element) == ESP_OK)
    {
        ESP_LOGV(IOEX_TAG, "handler added for IOEX_NUM_%u", ioex_num_to_num(ioex_num));
        io->interrupt_list->count++;
    }
    else 
    {
        free(interrupt_element);
        interrupt_element = NULL;
        ESP_LOGE(IOEX_TAG, "error while adding isr for IOEX_NUM_%u", ioex_num_to_num(ioex_num));
        goto err;
    }

    return ESP_OK;

err:
    return ESP_FAIL;

errArg:
    return ESP_ERR_INVALID_ARG;
}

esp_err_t ioex_isr_handler_remove(ioex_device_t *io, ioex_num_t ioex_num)
{
    IOEX_CHECK(io == NULL, "instance of ioexpander cannot be null", errArg);

    IOEX_CHECK(io->interrupt_list == NULL, "interrupt are not activated for ioexpander", err);

    if (remove_from_list(io, ioex_num) == ESP_OK)
    {
        io->interrupt_list->count--;
        ESP_LOGV(IOEX_TAG, "handler removed for IOEX_NUM_%u", ioex_num_to_num(ioex_num));
        ioex_interrupt_disable(io, ioex_num);
    }
    else
    {
        ESP_LOGE(IOEX_TAG, "error while removing isr for IOEX_NUM_%u", ioex_num_to_num(ioex_num));
        goto err;
    }

    return ESP_OK;

err:
    return ESP_FAIL;

errArg:
    return ESP_ERR_INVALID_ARG;
}

/*------------------------------------------------------------------------*/
/*---------------------- Interrupt functions -----------------------------*/
/*------------------------------------------------------------------------*/

static IRAM_ATTR void ioex_isr_handler(void* arg)
{
    ioex_device_t *io = (ioex_device_t*)arg;
    xTaskNotifyFromISR(io->interrupt_handle, 0, eNoAction, NULL);
}

static uint64_t ioex_get_activated_pin(ioex_device_t *io)
{
    i2c_cmd_handle_t cmd;
    uint8_t data[3] = {0};

    ESP_LOGV(IOEX_TAG, "READ: i2c_port:%#04x; device_address:%#04x; register:%#04x", io->i2c_port, io->address, INTERRUPT_STATUS_PORT_0);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (io->address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, INTERRUPT_STATUS_PORT_0, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (io->address << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(io->i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return data[2] << 16 | data[1] << 8 | data[0];
}

static void ioex_task_interrupt_handler(void* arg)
{
    ioex_device_t *io = (ioex_device_t*)arg;
    i2c_cmd_handle_t cmd;
    uint64_t pin;
    uint8_t dataout[] = {0xFF, 0xFF, 0xFF};
    while (1)
    {
        // Wait for an interrupt
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        // Check the source of the interrupt (we don't use the generic function because we read all register to improve performance)
        pin = ioex_get_activated_pin(io);
        ESP_LOGV(IOEX_TAG, "raw:0x%08llx", pin);

        // We loop to permit reading of interrupts that appen while we process others
        while(pin) {
            if (io->interrupt_list->first == NULL)
            {
                ESP_LOGW(IOEX_TAG, "unexpected interrupt from ioexpander");
                continue;
            }

            /* List is already in order of priority so we just have to iterate through it */
            ioex_interrupt_element_t *interrupt_element = io->interrupt_list->first;
            while (interrupt_element != NULL)
            {
                if ((1ULL<<interrupt_element->ioex_num) & pin)
                {
                    pin &= ~(1ULL<<interrupt_element->ioex_num);
                    /* Call isr for the gpio */
                    ESP_LOGV(IOEX_TAG, "Interrupt from IOEX_NUM_%u", ioex_num_to_num(interrupt_element->ioex_num));
                    interrupt_element->isr_handler(interrupt_element->args);
                    /* Clear interrupt */
                    i2c_write(io->i2c_port, io->address, INTERRUPT_CLEAR_PORT_0 + (interrupt_element->ioex_num / 8), (1U<<(interrupt_element->ioex_num-(8*(interrupt_element->ioex_num / 8)))));
                }
                interrupt_element = interrupt_element->next;
            }
            // Some interrupt where not handled
            if (pin)
            {
                ESP_LOGW(IOEX_TAG, "An interrupt appended but not handler was set for it");
                ESP_LOGV(IOEX_TAG, "Clearing all interrupts");
                ESP_LOGV(IOEX_TAG, "WRITE: i2c_port:%#04x; device_address:%#04x; register:%#04x; data:0xffffff", io->i2c_port, io->address, INTERRUPT_STATUS_PORT_0);
                // clear all pending interrupts
                cmd = i2c_cmd_link_create();
                i2c_master_start(cmd);
                i2c_master_write_byte(cmd, (io->address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
                i2c_master_write_byte(cmd, INTERRUPT_CLEAR_PORT_0, ACK_CHECK_EN);
                i2c_master_write(cmd, dataout, 3, ACK_CHECK_EN);
                i2c_master_stop(cmd);
                i2c_master_cmd_begin(io->i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
                i2c_cmd_link_delete(cmd);
            }
            pin = ioex_get_activated_pin(io);
        }
    }
}

/*------------------------------------------------------------------------*/
/*----------------------- Utility functions ------------------------------*/
/*------------------------------------------------------------------------*/

esp_err_t i2c_write(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t data)
{
    ESP_LOGV(IOEX_TAG, "WRITE: i2c_port:%#04x; device_address:%#04x; register:%#04x; data:%#04x", i2c_port, address, reg, data);

    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_read(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t *data)
{
    ESP_LOGV(IOEX_TAG, "READ: i2c_port:%#04x; device_address:%#04x; register:%#04x", i2c_port, address, reg);

    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    ESP_LOGV(IOEX_TAG, "raw:%#04x", *data);
    return ret;
}

uint8_t ioex_num_to_num(ioex_num_t ioex_num)
{
    return ioex_num + (ioex_num/8)*2;
}

/*------------------------------------------------------------------------*/
/*---------------------- Chained list functions --------------------------*/
/*------------------------------------------------------------------------*/

esp_err_t add_to_list(ioex_device_t *io, ioex_interrupt_element_t *interrupt_element)
{
    if (io->interrupt_list->first == NULL)
    {
        interrupt_element->next = NULL;
        io->interrupt_list->first = interrupt_element;
        return ESP_OK;
    }

    uint8_t position = find_list_position_from_priority(io, interrupt_element->priority);
    ESP_LOGV(IOEX_TAG, "position:%u", position);

    ioex_interrupt_element_t *current_element = io->interrupt_list->first;
    uint8_t count = 1;
    esp_err_t ret = ESP_OK;

    while (count < position)
    {
        if (current_element != NULL)
        {
            current_element = current_element->next;
            count++;
        }
        else
        {
            ret = ESP_FAIL;
        }
    }
    if (ret == ESP_OK)
    {
        interrupt_element->next = current_element->next;
        current_element->next = interrupt_element;
    }

    return ret;
}

uint8_t find_list_position_from_priority(ioex_device_t *io, uint8_t priority)
{
    uint8_t count = 0;
    if (io->interrupt_list->first == NULL)
    {
        return count;
    }
    count++;
    ioex_interrupt_element_t *interrupt_element = io->interrupt_list->first;
    while (interrupt_element->priority < priority)
    {
        count++;
        if (interrupt_element->next == NULL)
        {
            return count - 1;
        }
        else
        {
            interrupt_element = interrupt_element->next;
        }
    }
    return count - 1;
}

esp_err_t remove_from_list(ioex_device_t *io, ioex_num_t ioex_num)
{
    int position = find_list_position_from_ioex_num(io, ioex_num);
    ESP_LOGV(IOEX_TAG, "position:%i", position);

    ioex_interrupt_element_t *interrupt_element;

    if (position == -1)
    {
        return ESP_FAIL;
    }

    if (position == 0)
    {
        interrupt_element = io->interrupt_list->first;
        io->interrupt_list->first = interrupt_element->next;
        free(interrupt_element);
        interrupt_element = NULL;
        return ESP_OK;
    }
    
    ioex_interrupt_element_t *current_element = io->interrupt_list->first;
    uint8_t count = 0;
    esp_err_t ret = ESP_OK;

    while (count < position-1)
    {
        if (current_element != NULL)
        {
            current_element = current_element->next;
            count++;
        }
        else
        {
            // element not found
            ret = ESP_FAIL;
        }
    }
    if (ret == ESP_OK)
    {
        interrupt_element = current_element->next;
        current_element->next = interrupt_element->next;
        free(interrupt_element);
        interrupt_element = NULL;
    }

    return ret;
}

int find_list_position_from_ioex_num(ioex_device_t *io, ioex_num_t ioex_num)
{
    int count = 0;
    if (io->interrupt_list->first == NULL)
    {
        return -1;
    }

    ioex_interrupt_element_t *interrupt_element = io->interrupt_list->first;

    while(interrupt_element != NULL)
    {
        if (interrupt_element->ioex_num == ioex_num)
        {
            ESP_LOGV(IOEX_TAG, "find !!");
            break;
        }
        else
        {
            count++;
            interrupt_element = interrupt_element->next;
        }
    }

    if (interrupt_element == NULL)
    {
        count = -1;
    }

    return count;
}