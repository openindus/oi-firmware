/**
 * @file CLI_Bus.cpp
 * @brief Command line interface - Bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI_Bus.h"
#include "Bus.h"

static const char TAG[] = "CLI_Bus";

/* --- CAN Write --- */

static struct {
    struct arg_int *id;
    struct arg_int *data;
    struct arg_end *end;
} CANWriteArgs;

static int CANWriteCmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &CANWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, CANWriteArgs.end, argv[0]);
        return 1;
    }

    int id = CANWriteArgs.id->ival[0];
    int data[8] = {0};
    int size = argc - 2;
    for (int i = 0; i < size; i++) {
        data[i] = CANWriteArgs.data->ival[i];
    }

    BusCAN::Frame_t frame;
    if (size > 0) {
        memcpy(&frame, data, size);
    }
    BusCAN::write(&frame, id, size);

    return 0;
}

static int registerCANWriteCmd(void)
{
    CANWriteArgs.id = arg_int1(NULL, "id", "ID", "identifier");
    CANWriteArgs.data = arg_intn(NULL, NULL, "<DATA>", 0, 8, "CAN data");
    CANWriteArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "can-write",
        .help = "Write data to the CAN bus",
        .hint = NULL,
        .func = &CANWriteCmd,
        .argtable = &CANWriteArgs
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- */

int CLI_Bus_registerCommands(void)
{
    int err = 0;
    err |= registerCANWriteCmd();
    return err;
}