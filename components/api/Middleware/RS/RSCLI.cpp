/**
 * @file RSCLI.cpp
 * @brief RS485/232 command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RSCLI.h"
#include "Utils.h"

static const char* TAG = "RSCLI";

// Static member definitions
decltype(RSCLI::rsBeginArgs) RSCLI::rsBeginArgs;
decltype(RSCLI::rsReadBufferArgs) RSCLI::rsReadBufferArgs;
decltype(RSCLI::rsWriteByteArgs) RSCLI::rsWriteByteArgs;
decltype(RSCLI::rsWriteBufferArgs) RSCLI::rsWriteBufferArgs;

RS* RSCLI::_rsInstance = new RS(SPI3_HOST, GPIO_NUM_12, GPIO_NUM_14);

int RSCLI::rsBeginFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, rsBeginArgs);

    unsigned long baudrate = 115200;
    uint16_t config = 0x001c;
    RS_Mode_e mode = RS_485;

    // Parse baudrate if provided
    if (rsBeginArgs.baudrate->count > 0) {
        baudrate = (unsigned long)rsBeginArgs.baudrate->ival[0];
    }

    // Parse config if provided
    if (rsBeginArgs.config->count > 0) {
        config = (uint16_t)rsBeginArgs.config->ival[0];
    }

    // Parse mode if provided
    if (rsBeginArgs.mode->count > 0) {
        if (strcmp(rsBeginArgs.mode->sval[0], "RS485") == 0 || strcmp(rsBeginArgs.mode->sval[0], "485") == 0) {
            mode = RS_485;
        } else if (strcmp(rsBeginArgs.mode->sval[0], "RS232") == 0 || strcmp(rsBeginArgs.mode->sval[0], "232") == 0) {
            mode = RS_232;
        } else {
            ESP_LOGE(TAG, "Invalid mode: %s. Use RS485 or RS232", rsBeginArgs.mode->sval[0]);
            return -1;
        }
    }

    if (_rsInstance != nullptr) {
        _rsInstance->begin(baudrate, config, mode);
        printf("RS initialized: baudrate=%lu, config=0x%04X, mode=%s\n", 
               baudrate, config, (mode == RS_485) ? "RS485" : "RS232");
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsEndFunc(int argc, char **argv)
{
    if (_rsInstance != nullptr) {
        _rsInstance->end();
        printf("RS communication ended\n");
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsAvailableFunc(int argc, char **argv)
{
    if (_rsInstance != nullptr) {
        int available = _rsInstance->available();
        printf("%d\n", available);
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsAvailableForWriteFunc(int argc, char **argv)
{
    if (_rsInstance != nullptr) {
        int availableForWrite = _rsInstance->availableForWrite();
        printf("%d\n", availableForWrite);
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsReadFunc(int argc, char **argv)
{
    if (_rsInstance != nullptr) {
        int data = _rsInstance->read();
        if (data >= 0) {
            printf("0x%02X (%d)\n", data, data);
        } else {
            printf("No data available\n");
        }
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsReadBufferFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, rsReadBufferArgs);

    size_t size = (size_t)rsReadBufferArgs.size->ival[0];
    if (size <= 0 || size > 1024) {
        ESP_LOGE(TAG, "Invalid buffer size: %zu. Must be between 1 and 1024", size);
        return -1;
    }

    if (_rsInstance != nullptr) {
        uint8_t* buffer = (uint8_t*)malloc(size);
        if (buffer == nullptr) {
            ESP_LOGE(TAG, "Failed to allocate buffer");
            return -1;
        }

        size_t bytesRead = _rsInstance->read(buffer, size);
        printf("Read %zu bytes: ", bytesRead);
        for (size_t i = 0; i < bytesRead; i++) {
            printf("0x%02X ", buffer[i]);
        }
        printf("\n");

        free(buffer);
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsWriteByteFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, rsWriteByteArgs);

    uint8_t byte = (uint8_t)rsWriteByteArgs.byte->ival[0];

    if (_rsInstance != nullptr) {
        size_t bytesWritten = _rsInstance->write(byte);
        printf("Wrote %zu byte: 0x%02X\n", bytesWritten, byte);
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::rsWriteBufferFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, rsWriteBufferArgs);

    const char* dataStr = rsWriteBufferArgs.data->sval[0];
    size_t len = strlen(dataStr);

    if (_rsInstance != nullptr) {
        size_t bytesWritten = _rsInstance->write((const uint8_t*)dataStr, len);
        printf("Wrote %zu bytes: %s\n", bytesWritten, dataStr);
    } else {
        ESP_LOGE(TAG, "Failed to create RS instance");
        return -1;
    }

    return 0;
}

int RSCLI::init(void)
{
    int err = 0;

    // Register rs-begin command
    rsBeginArgs.baudrate = arg_int0("b", "baudrate", "<baudrate>", "Baudrate (default: 115200)");
    rsBeginArgs.config = arg_int0("c", "config", "<config>", "Configuration (default: 0x001c)");
    rsBeginArgs.mode = arg_str0("m", "mode", "<mode>", "RS mode: RS485 or RS232 (default: RS485)");
    rsBeginArgs.end = arg_end(4);

    const esp_console_cmd_t rsBeginCmd = {
        .command = "rs-begin",
        .help = "Initialize RS communication",
        .hint = NULL,
        .func = &RSCLI::rsBeginFunc,
        .argtable = &rsBeginArgs
    };
    err |= esp_console_cmd_register(&rsBeginCmd);

    // Register rs-end command
    const esp_console_cmd_t rsEndCmd = {
        .command = "rs-end",
        .help = "End RS communication",
        .hint = NULL,
        .func = &RSCLI::rsEndFunc,
        .argtable = NULL
    };
    err |= esp_console_cmd_register(&rsEndCmd);

    // Register rs-available command
    const esp_console_cmd_t rsAvailableCmd = {
        .command = "rs-available",
        .help = "Get number of bytes available for reading",
        .hint = NULL,
        .func = &RSCLI::rsAvailableFunc,
        .argtable = NULL
    };
    err |= esp_console_cmd_register(&rsAvailableCmd);

    // Register rs-available-for-write command
    const esp_console_cmd_t rsAvailableForWriteCmd = {
        .command = "rs-available-for-write",
        .help = "Get number of bytes available for writing",
        .hint = NULL,
        .func = &RSCLI::rsAvailableForWriteFunc,
        .argtable = NULL
    };
    err |= esp_console_cmd_register(&rsAvailableForWriteCmd);

    // Register rs-read command
    const esp_console_cmd_t rsReadCmd = {
        .command = "rs-read",
        .help = "Read a single byte from RS",
        .hint = NULL,
        .func = &RSCLI::rsReadFunc,
        .argtable = NULL
    };
    err |= esp_console_cmd_register(&rsReadCmd);

    // Register rs-read-buffer command
    rsReadBufferArgs.size = arg_int1("s", "size", "<size>", "Number of bytes to read (1-1024)");
    rsReadBufferArgs.end = arg_end(2);

    const esp_console_cmd_t rsReadBufferCmd = {
        .command = "rs-read-buffer",
        .help = "Read multiple bytes from RS into buffer",
        .hint = NULL,
        .func = &RSCLI::rsReadBufferFunc,
        .argtable = &rsReadBufferArgs
    };
    err |= esp_console_cmd_register(&rsReadBufferCmd);

    // Register rs-write-byte command
    rsWriteByteArgs.byte = arg_int1("b", "byte", "<byte>", "Byte to write (0-255)");
    rsWriteByteArgs.end = arg_end(2);

    const esp_console_cmd_t rsWriteByteCmd = {
        .command = "rs-write-byte",
        .help = "Write a single byte to RS",
        .hint = NULL,
        .func = &RSCLI::rsWriteByteFunc,
        .argtable = &rsWriteByteArgs
    };
    err |= esp_console_cmd_register(&rsWriteByteCmd);

    // Register rs-write-buffer command
    rsWriteBufferArgs.data = arg_str1("d", "data", "<data>", "String data to write");
    rsWriteBufferArgs.end = arg_end(2);

    const esp_console_cmd_t rsWriteBufferCmd = {
        .command = "rs-write-buffer",
        .help = "Write string data to RS",
        .hint = NULL,
        .func = &RSCLI::rsWriteBufferFunc,
        .argtable = &rsWriteBufferArgs
    };
    err |= esp_console_cmd_register(&rsWriteBufferCmd);

    return err;
}
