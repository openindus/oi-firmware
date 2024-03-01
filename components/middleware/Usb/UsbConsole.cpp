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

#if defined(OI_CORE)
#define PROMPT_STR "Core"
#elif defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
#define PROMPT_STR "Discrete"
#elif defined(OI_STEPPER) || defined(OI_STEPPER_VE)
#define PROMPT_STR "Stepper"
#elif defined(OI_MIXED)
#define PROMPT_STR "Mixed"
#else
#define PROMPT_STR ""
#endif

static const char TAG[] = "UsbConsole";

esp_console_repl_t* UsbConsole::_repl = NULL;

/* By default do not keep console after init */
bool UsbConsole::keepFlag = false;
bool UsbConsole::continueListenTask = true;
TaskHandle_t UsbConsole::listenTaskHandle = NULL;

void UsbConsole::listen()
{
    xTaskCreate(_listenTask, "Listen console task", 2048, NULL, 1, &listenTaskHandle);
}

bool UsbConsole::begin(bool force)
{
    /* stop listen task */
    continueListenTask = false;
    while(eTaskGetState(listenTaskHandle) == eReady) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* If force and keep flag are false, do not start console */
    if (!force && !keepFlag) {
        return false;
    }

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

    return true;
}

void UsbConsole::end(void)
{
    ESP_ERROR_CHECK(_repl->del(_repl));
}

void UsbConsole::_listenTask(void *pvParameters)
{
    char buff[10];
    int c;
    int i = 0;

    while(continueListenTask) {
        vTaskDelay(10);
        c = fgetc(stdin);
        if(c != EOF) {
            buff[i] = (char) c;
            if(i == 6) {
                buff[i+1] = '\0';
                if(strcmp("console", buff) ==  0) {
                    keepFlag = true;
                    ESP_LOGI(TAG, "Activating console");
                    break;
                } else {
                    i = 0;
                }
            }
            i++;
        }
    }
    vTaskDelete(NULL);
}