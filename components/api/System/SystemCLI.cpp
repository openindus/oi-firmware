/**
 * @file SystemCLI.cpp
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "SystemCLI.h"

/* --- restart --- */

static int restartCmd(int argc, char **argv)
{
    esp_restart();
    return 0;
}

int SystemCLI::_registerRestartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restartCmd,
        .argtable = NULL
    };
    return esp_console_cmd_register(&cmd);
}

/* --- log --- */

static struct {
    struct arg_str *level;
    struct arg_str *tag;
    struct arg_end *end;
} logArgs;

static int logCmd(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &logArgs);
    if (err != 0) {
        arg_print_errors(stderr, logArgs.end, argv[0]);
        return -1;
    }

    if (logArgs.tag->count == 0) {
        logArgs.tag->sval[0] = "*";
    } 

    printf("Setting log %s to %s\n", logArgs.tag->sval[0], logArgs.level->sval[0]);

    if(strcmp(logArgs.level->sval[0], "NONE") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_NONE);
    } else if (strcmp(logArgs.level->sval[0], "ERROR") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_ERROR);
    } else if (strcmp(logArgs.level->sval[0], "WARN") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_WARN);
    } else if (strcmp(logArgs.level->sval[0], "INFO") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_INFO);
    } else if (strcmp(logArgs.level->sval[0], "DEBUG") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_DEBUG);
    } else if (strcmp(logArgs.level->sval[0], "VERBOSE") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_VERBOSE);
    } else {
        arg_print_errors(stderr, logArgs.end, argv[0]);
        return -2;
    }

    return 0;
}

int SystemCLI::_registerLogCmd(void)
{
    logArgs.level = arg_str1(NULL, NULL, "<LEVEL>", "[NONE, ERROR, WARN, INFO, DEBUG, VERBOSE]");
    logArgs.tag = arg_str0(NULL, NULL, "<TAG>", "specific tag");
    logArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "log",
        .help = "set the log level",
        .hint = NULL,
        .func = &logCmd,
        .argtable = &logArgs
    };
    return esp_console_cmd_register(&cmd);
}