/**
 * @file CLI_Led.cpp
 * @brief Command line interface - Led
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"
#include "Led.h"

/* --- led --- */

static struct {
    struct arg_str *state;
    struct arg_int *color;
    struct arg_end *end;
} ledArgs;

static int ledCmd(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &ledArgs);
    if (err != 0) {
        arg_print_errors(stderr, ledArgs.end, argv[0]);
        return -1;
    }

    if(strcmp(ledArgs.state->sval[0], "on") == 0) {
        if (ledArgs.color->count > 0) {
            Led::on((LedColor_t)ledArgs.color->ival[0]);
        } else {
            Led::on(LED_BLUE);
        }
    } else if (strcmp(ledArgs.state->sval[0], "off") == 0) {
       Led::off();
    } else if (strcmp(ledArgs.state->sval[0], "blink") == 0) {
        if (ledArgs.color->count > 0) {
            Led::blink((LedColor_t)ledArgs.color->ival[0], 1000);
        }
    } else {
        arg_print_errors(stderr, ledArgs.end, argv[0]);
        return -2;
    }
    return 0;
}

int CLI::_registerLedCmd(void)
{
    ledArgs.state = arg_str1(NULL, NULL, "<STATE>", "[on, off, blink]");
    ledArgs.color = arg_int0(NULL, NULL, "color", "[0: None, 1: red, 2: green, 3: yellow, 4: blue, 5: purple, 6: cyan, 7; white]");
    ledArgs.end = arg_end(6);

    const esp_console_cmd_t cmd = {
        .command = "led",
        .help = "Change led state",
        .hint = NULL,
        .func = &ledCmd,
        .argtable = &ledArgs
    };
    return esp_console_cmd_register(&cmd);
}
