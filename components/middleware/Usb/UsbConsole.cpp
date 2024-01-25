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

#if defined CONFIG_CORE
#define PROMPT_STR "Core"
#elif defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)
#define PROMPT_STR "Discrete"
#elif defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)
#define PROMPT_STR "Stepper"
#elif defined(CONFIG_MIXED)
#define PROMPT_STR "Mixed"
#else
#define PROMPT_STR ""
#endif

esp_console_repl_t* UsbConsole::_repl = NULL;

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
        .channel = CONFIG_ESP_CONSOLE_UART_NUM,
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