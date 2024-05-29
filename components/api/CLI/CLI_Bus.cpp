/**
 * @file CLI_Bus.cpp
 * @brief Command line interface - Bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"
#include "Bus.h"

static const char TAG[] = "CLI_Bus";

#if !defined(MODULE_STANDALONE)

/* --- read-id --- */

static int readIdCmd(int argc, char **argv) 
{
    printf("%u\n", BusIO::readId());
    return 0;
}

int CLI::_registerReadIdCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "read-id",
        .help = "Read the bus ID of the device",
        .hint = NULL,
        .func = &readIdCmd,
        .argtable = NULL
    };
    return esp_console_cmd_register(&cmd);
}

/* --- write-sync --- */

static struct {
    struct arg_int *level;
    struct arg_end *end;
} writeSyncArgs;

static int writeSyncCmd(int argc, char **argv) 
{
    int err = arg_parse(argc, argv, (void **) &writeSyncArgs);
    if (err != 0) {
        arg_print_errors(stderr, writeSyncArgs.end, argv[0]);
        return -1;
    }

    BusIO::writeSync((uint8_t)writeSyncArgs.level->ival[0]);

    return 0;
}

int CLI::_registerWriteSyncCmd(void)
{
    writeSyncArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    writeSyncArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "write-sync",
        .help = "set sync pin level",
        .hint = NULL,
        .func = &writeSyncCmd,
        .argtable = &writeSyncArgs
    };
    return esp_console_cmd_register(&cmd);
}

/* --- read-sync --- */

static int readSyncCmd(int argc, char **argv) 
{
    printf("%u\n", BusIO::readSync());
    return 0;
}

int CLI::_registerReadSyncCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "read-sync",
        .help = "get sync pin level",
        .hint = NULL,
        .func = &readSyncCmd,
        .argtable = NULL
    };
    return esp_console_cmd_register(&cmd);
}

/* --- bus-power --- */

static struct {
    struct arg_str *state;
    struct arg_end *end;
} busPowerArgs;

static int busPowerCmd(int argc, char **argv) 
{
    int err = arg_parse(argc, argv, (void **) &busPowerArgs);
    if (err != 0) {
        arg_print_errors(stderr, busPowerArgs.end, argv[0]);
        return -1;
    }

    if(strcmp(busPowerArgs.state->sval[0], "on") == 0) {
        BusIO::powerOn();
    } else if (strcmp(busPowerArgs.state->sval[0], "off") == 0) {
        BusIO::powerOff();
    } else {
        arg_print_errors(stderr, busPowerArgs.end, argv[0]);
        return 2;
    }
    return 0;
}

int CLI::_registerBusPowerCmd(void)
{
    busPowerArgs.state = arg_str1(NULL, NULL, "<STATE>", "[on/off]");
    busPowerArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "bus-power",
        .help = "Bus power on/off",
        .hint = NULL,
        .func = &busPowerCmd,
        .argtable = &busPowerArgs
    };
    return esp_console_cmd_register(&cmd);
}

/* --- can-write --- */

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

int CLI::_registerCANWriteCmd(void)
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

/* --- can-read --- */

static int CANReadCmd(int argc, char **argv)
{
    uint16_t id;
    uint8_t dlc;
    BusCAN::Frame_t frame;

    if (BusCAN::read(&frame, &id, &dlc) == ESP_OK) {
        printf("ID: %d\n", id);
        printf("DLC: %d\n", dlc);
        printf("Data: ");
        for (int i = 0; i < dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    } else {
        fprintf(stderr, "Failed to read from CAN bus\n");
        return 1;
    }

    return 0;
}

int CLI::_registerCANReadCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "can-read",
        .help = "Read data from the CAN bus",
        .hint = NULL,
        .func = &CANReadCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- rs-write --- */

static struct {
    struct arg_int *cmd;
    struct arg_int *id;
    struct arg_int *dir;
    struct arg_int *ack;
    struct arg_int *data;
    struct arg_end *end;
} RSWriteArgs;

static int RSWriteCmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &RSWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, RSWriteArgs.end, argv[0]);
        return 1;
    }

    BusRS::Frame_t frame;

    frame.cmd = RSWriteArgs.cmd->ival[0];
    frame.id = RSWriteArgs.id->ival[0];
    frame.dir = RSWriteArgs.dir->ival[0];
    frame.ack = RSWriteArgs.ack->ival[0];

    int size = argc - 5;
    frame.length = size;
    frame.data = (uint8_t*) malloc(size * sizeof(uint8_t));
    for (int i = 0; i < size; i++) {
        frame.data[i] = RSWriteArgs.data->ival[i];
    }

    BusRS::write(&frame);

    free(frame.data);
    return 0;
}

int CLI::_registerRSWriteCmd(void)
{
    RSWriteArgs.cmd = arg_int1(NULL, "cmd", "CMD", "command");
    RSWriteArgs.id = arg_int1(NULL, "id", "ID", "identifier");
    RSWriteArgs.dir = arg_int1(NULL, "dir", "Direction", "direction (0 or 1)");
    RSWriteArgs.ack = arg_int1(NULL, "ack", "Acknowledge", "acknowledge flag (0 or 1)");
    RSWriteArgs.data = arg_intn(NULL, NULL, "<DATA>", 0, 255, "RS data");
    RSWriteArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "rs-write",
        .help = "Write data to the RS bus",
        .hint = NULL,
        .func = &RSWriteCmd,
        .argtable = &RSWriteArgs
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- rs-read --- */

static int RSReadCmd(int argc, char **argv)
{
    BusRS::Frame_t frame;

    if (BusRS::read(&frame) == ESP_OK) {
        printf("Sync: %02X\n", frame.sync);
        printf("Command: %02X\n", frame.cmd);
        printf("ID: %d\n", frame.id);
        printf("Direction: %d\n", frame.dir);
        printf("Acknowledge: %d\n", frame.ack);
        printf("Error: %d\n", frame.error);
        printf("Length: %d\n", frame.length);
        printf("Checksum: %02X\n", frame.checksum);
        printf("Data: ");
        for (int i = 0; i < frame.length; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    } else {
        fprintf(stderr, "Failed to read from RS bus\n");
        return 1;
    }

    return 0;
}

int CLI::_registerRSReadCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "rs-read",
        .help = "Read data from the RS bus",
        .hint = NULL,
        .func = &RSReadCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

#endif