/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ledonewire.c
 * @brief Functions to control a onewire led
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ledonewire/ledonewire.h"

static const char LEDONEWIRE_TAG[] = "ledonewire";

#define LEDONEWIRE_CHECK(a, str, goto_tag, ret_value, ...)                                   \
    do                                                                                       \
    {                                                                                        \
        if (!(a))                                                                            \
        {                                                                                    \
            ESP_LOGE(LEDONEWIRE_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ledonewire = ret_value;                                                          \
            goto goto_tag;                                                                   \
        }                                                                                    \
    } while (0)

#define LEDONEWIRE_T0H_NS (300)
#define LEDONEWIRE_T0L_NS (900)
#define LEDONEWIRE_T1H_NS (900)
#define LEDONEWIRE_T1L_NS (300)
#define LEDONEWIRE_RESET_US (80)

static uint32_t ledonewire_t0h_ticks = 0;
static uint32_t ledonewire_t1h_ticks = 0;
static uint32_t ledonewire_t0l_ticks = 0;
static uint32_t ledonewire_t1l_ticks = 0;

static ledonewire_control_t ledControl;
static TaskHandle_t ledTaskHandle;
static ledonewire_t* led;

/**
 * @brief Led task used for blinking
 * 
 */
static void ledTask(void *pvParameters);

void ledonewire_install(gpio_num_t gpio_num)
{
    if (ledControl.installed == false)
    {
        led = malloc(sizeof(ledonewire_t));
        ledonewire_config_t led_config = LEDONEWIRE_DEFAULT_CONFIG(gpio_num);
        led = ledonewire_create(&led_config);
        ledControl.color = LED_BLUE;
        ledControl.period = 1000;
        ledControl.blink = true;
        ledControl.blinkStatus = true;
        ledControl.installed = true;
        ledControl.intensity = 255;
        ESP_LOGV(LEDONEWIRE_TAG, "creating led task");
        xTaskCreate(ledTask, "led task", 1024, NULL, 10, &ledTaskHandle);
    }
}

void ledonewire_remove()
{
    if (ledControl.installed == true)
    {
        ledonewire_delete(led);
        ledControl.installed = false;
    }
}

void ledonewire_on(LedColor_t color)
{
    if (ledControl.installed == true)
    {
        ESP_LOGV(LEDONEWIRE_TAG, "led on color:0x%02x", color);
        ledControl.color = color;
        ledControl.blink = false;
        xTaskNotify(ledTaskHandle, 0, eNoAction);
    }
}

void ledonewire_off(void)
{
    if (ledControl.installed == true)
    {
        ESP_LOGV(LEDONEWIRE_TAG, "led off color");
        ledControl.color = LED_NONE;
        ledControl.blink = false;
        xTaskNotify(ledTaskHandle, 0, eNoAction);
    }
}

void ledonewire_blink(LedColor_t color, uint32_t period)
{
    if (ledControl.installed == true)
    {
        ESP_LOGV(LEDONEWIRE_TAG, "led blink color:0x%02x | period:%u", color, period);
        ledControl.color = color;
        ledControl.period = period;
        ledControl.blink = true;
        xTaskNotify(ledTaskHandle, 0, eNoAction);
    }
}

void ledTask(void *pvParameters)
{
    uint8_t red, green, blue;

    while(1)
    {
        xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS((ledControl.period)/2));

        if (ledControl.installed)
        {
            red = (ledControl.color & LED_RED_MASK) ? ledControl.intensity : 0;
            green = (ledControl.color & LED_GREEN_MASK) ? ledControl.intensity : 0;
            blue = (ledControl.color & LED_BLUE_MASK) ? ledControl.intensity : 0;

            if (ledControl.blink)
            {
                if (ledControl.blinkStatus)
                {
                    ledonewire_set_color(led, red, green, blue);
                    ledControl.blinkStatus = false;
                }
                else
                {
                    ledonewire_set_color(led, 0, 0, 0);
                    ledControl.blinkStatus = true;
                }
            }
            else
            {
                ledonewire_set_color(led, red, green, blue);
            }
        }
    }
}


/**
 * @brief Conver RGB data to RMT format.
 *
 * @note For ledonewire, R,G,B each contains 256 different choices (i.e. uint8_t)
 *
 * @param[in] src: source data, to converted to RMT format
 * @param[in] dest: place where to store the convert result
 * @param[in] src_size: size of source data
 * @param[in] wanted_num: number of RMT items that want to get
 * @param[out] translated_size: number of source data that got converted
 * @param[out] item_num: number of RMT items which are converted from source data
 */
static void IRAM_ATTR ledonewire_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size,
                                             size_t wanted_num, size_t *translated_size, size_t *item_num)
{
    if (src == NULL || dest == NULL)
    {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{ledonewire_t0h_ticks, 1, ledonewire_t0l_ticks, 0}}}; //Logical 0
    const rmt_item32_t bit1 = {{{ledonewire_t1h_ticks, 1, ledonewire_t1l_ticks, 0}}}; //Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pdest = dest;
    while (size < src_size && num < wanted_num)
    {
        for (int i = 0; i < 8; i++)
        {
            // MSB first
            if (*psrc & (1 << (7 - i)))
            {
                pdest->val = bit1.val;
            }
            else
            {
                pdest->val = bit0.val;
            }
            num++;
            pdest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}

ledonewire_t *ledonewire_create(const ledonewire_config_t *config)
{
    ledonewire_t *ledonewire = NULL;

    ESP_LOGV(LEDONEWIRE_TAG, "installing driver...");

    LEDONEWIRE_CHECK(config, "configuration can't be null", err, NULL);

    // Installing RMT driver
    rmt_config_t rmt_param = RMT_DEFAULT_CONFIG_TX(config->gpio_num, config->channel);

    // set counter clock to 40MHz
    rmt_param.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&rmt_param));
    ESP_ERROR_CHECK(rmt_driver_install(config->channel, 0, 0));

    uint32_t counter_clk_hz = 0;
    rmt_get_counter_clock(config->channel, &counter_clk_hz);
    // ns -> ticks
    float ratio = (float)counter_clk_hz / 1e9;

    ledonewire_t0h_ticks = (uint32_t)(ratio * LEDONEWIRE_T0H_NS);
    ledonewire_t0l_ticks = (uint32_t)(ratio * LEDONEWIRE_T0L_NS);
    ledonewire_t1h_ticks = (uint32_t)(ratio * LEDONEWIRE_T1H_NS);
    ledonewire_t1l_ticks = (uint32_t)(ratio * LEDONEWIRE_T1L_NS);

    // Creating instance of led
    ledonewire = (ledonewire_t *)malloc(sizeof(ledonewire_t));
    LEDONEWIRE_CHECK(ledonewire, "request memory for ledonewire failed", err, NULL);

    // set ledonewire to rmt adapter
    rmt_translator_init(config->channel, ledonewire_rmt_adapter);

    ledonewire->rmt_channel = config->channel; // set channel
    // set default color
    ledonewire->buffer[0] = 0;
    ledonewire->buffer[1] = 0;
    ledonewire->buffer[2] = 0;

    ESP_LOGV(LEDONEWIRE_TAG, "driver was successfully installed");
    return ledonewire;

err:
    return ledonewire;
}

esp_err_t ledonewire_delete(ledonewire_t *ledonewire)
{
    esp_err_t err = ESP_OK;
    LEDONEWIRE_CHECK(ledonewire, "ledonewire instance can't be null", err, NULL);
    ESP_LOGV(LEDONEWIRE_TAG, "removing driver...");
    err = rmt_driver_uninstall(ledonewire->rmt_channel);
    free(ledonewire);
    return err;

err:
    return ESP_OK;
}

esp_err_t ledonewire_set_color(ledonewire_t *ledonewire, uint8_t red, uint8_t green, uint8_t blue)
{
    LEDONEWIRE_CHECK(ledonewire, "ledonewire instance can't be null", err, NULL);
    // In three order of GRB
    ledonewire->buffer[0] = green;
    ledonewire->buffer[1] = red;
    ledonewire->buffer[2] = blue;
    rmt_write_sample(ledonewire->rmt_channel, ledonewire->buffer, 3, true);
    return rmt_wait_tx_done(ledonewire->rmt_channel, pdMS_TO_TICKS(5));

err:
    return ESP_OK;
}