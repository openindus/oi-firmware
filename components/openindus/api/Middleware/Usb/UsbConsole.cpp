/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file UsbConsole.cpp
 * @brief OpenIndus USB Console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "UsbConsole.h"

#if defined(CONFIG_OI_CORE)
#define PROMPT_STR "Core"
#elif defined(CONFIG_OI_DISCRETE) || defined(CONFIG_OI_DISCRETE_VE)
#define PROMPT_STR "Discrete"
#elif defined(CONFIG_OI_STEPPER) || defined(CONFIG_OI_STEPPER_VE)
#define PROMPT_STR "Stepper"
#elif defined(CONFIG_OI_DC)
#define PROMPT_STR "Dc"
#elif defined(CONFIG_OI_MIXED)
#define PROMPT_STR "Mixed"
#elif defined(OI_ANALOG_LS)
#define PROMPT_STR "AnalogLS"
#else
#define PROMPT_STR ""
#endif

static const char TAG[] = "UsbConsole";

esp_console_repl_t* UsbConsole::_repl = NULL;

bool UsbConsole::checkUserActivation(uint32_t timeoutMs)
{
    int64_t startTime = esp_timer_get_time();
    char buff[10];
    int c;
    int i = 0;

    while((esp_timer_get_time() - startTime) < (timeoutMs * 1000)) { // Convert ms to µs
        c = fgetc(stdin);
        if(c != EOF) {
            buff[i] = (char) c;
            if(i == 6) {
                buff[i+1] = '\0';
                if(strcmp("console", buff) == 0) {
                    ESP_LOGI(TAG, "Activating console");
                    return true;
                } else {
                    i = 0;
                }
            }
            i++;
        }
    }
    return false;
}

void UsbConsole::begin(void)
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 1024;
    // repl_config.history_save_path = HIDOUTY_PATH;

    esp_console_register_help_command();

    printf("\n ==================================================\n");
    printf(" |                                                |\n");
    printf(" |       Welcome to the OpenIndus System          |\n");
    printf(" |                                                |\n");
    printf(" ==================================================\n\n");

    esp_console_dev_uart_config_t hw_config = {
        .channel = UART_NUM_0,
        .baud_rate = 115200,
        .tx_gpio_num = -1,
        .rx_gpio_num = -1,
    };
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &_repl));
    ESP_ERROR_CHECK(esp_console_start_repl(_repl));
}

void UsbConsole::end(void)
{
    ESP_ERROR_CHECK(_repl->del(_repl));
}