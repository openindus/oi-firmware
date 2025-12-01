/**
 * @file CANCLI.cpp
 * @brief CAN bus command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CANCLI.h"
#include "Utils.h"

static const char* TAG = "CANCLI";

// Static member definitions
decltype(CANCLI::canBeginArgs) CANCLI::canBeginArgs;
decltype(CANCLI::canWriteArgs) CANCLI::canWriteArgs;
decltype(CANCLI::canSetStandardFilterArgs) CANCLI::canSetStandardFilterArgs;
decltype(CANCLI::canSetExtendedFilterArgs) CANCLI::canSetExtendedFilterArgs;

CAN* CANCLI::_canInstance = new CAN(SPI3_HOST, GPIO_NUM_11, GPIO_NUM_13);

int CANCLI::canBeginFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, canBeginArgs);

    unsigned long baudrate = 1000000; // Default to 1 Mbps
    bool extended_mode = false;

    // Parse baudrate if provided
    if (canBeginArgs.baudrate->count > 0) {
        baudrate = (unsigned long)canBeginArgs.baudrate->ival[0];
        
        // Validate baudrate (only standard CAN baudrates)
        if (baudrate != 50000 && baudrate != 100000 && baudrate != 125000 && 
            baudrate != 250000 && baudrate != 500000 && baudrate != 1000000) {
            ESP_LOGE(TAG, "Invalid baudrate: %lu. Use: 50000, 100000, 125000, 250000, 500000, or 1000000", baudrate);
            return -1;
        }
    }

    // Parse extended mode if provided
    if (canBeginArgs.extended->count > 0) {
        extended_mode = true;
    }

    if (_canInstance != nullptr) {
        _canInstance->begin(baudrate, extended_mode);
        printf("CAN initialized: baudrate=%lu, mode=%s\n", 
               baudrate, extended_mode ? "Extended" : "Standard");
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::canEndFunc(int argc, char **argv)
{
    if (_canInstance != nullptr) {
        _canInstance->end();
        printf("CAN communication ended\n");
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::canAvailableFunc(int argc, char **argv)
{
    if (_canInstance != nullptr) {
        int available = _canInstance->available();
        printf("%d\n", available);
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::canWriteFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, canWriteArgs);

    // Parse CAN ID
    uint32_t id = (uint32_t)canWriteArgs.id->ival[0];
    
    // Parse data string
    const char* dataStr = canWriteArgs.data->sval[0];
    size_t dataLen = strlen(dataStr);
    
    if (dataLen > 16) { // 8 bytes max, 2 chars per byte
        ESP_LOGE(TAG, "Data too long: %zu chars. Max 16 chars (8 bytes in hex)", dataLen);
        return -1;
    }

    // Parse extended mode
    bool extended = (canWriteArgs.extended->count > 0);
    
    // Parse RTR mode
    bool rtr = (canWriteArgs.rtr->count > 0);

    // Convert hex string to bytes
    CAN_Message_t msg;
    msg.id = id;
    msg.IDE = extended;
    msg.RTR = rtr;
    msg.size = 0;

    if (!rtr) { // Only parse data if not RTR frame
        // Parse hex string (format: "AABBCCDD" -> 0xAA, 0xBB, 0xCC, 0xDD)
        for (size_t i = 0; i < dataLen && i < 16; i += 2) {
            if (i + 1 < dataLen) {
                char hexByte[3] = {dataStr[i], dataStr[i+1], '\0'};
                unsigned int byte;
                if (sscanf(hexByte, "%02X", &byte) == 1) {
                    msg.msg[msg.size++] = (uint8_t)byte;
                } else {
                    ESP_LOGE(TAG, "Invalid hex data: %s", hexByte);
                    return -1;
                }
            }
        }
    }

    if (_canInstance != nullptr) {
        _canInstance->write(msg);
        printf("CAN message sent: ID=0x%lX, Extended=%s, RTR=%s, Size=%d\n", 
               msg.id, extended ? "Yes" : "No", rtr ? "Yes" : "No", msg.size);
        if (!rtr && msg.size > 0) {
            printf("Data: ");
            for (int i = 0; i < msg.size; i++) {
                printf("%02X ", msg.msg[i]);
            }
            printf("\n");
        }
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::canReadFunc(int argc, char **argv)
{
    if (_canInstance != nullptr) {
        if (_canInstance->available() > 0) {
            CAN_Message_t msg = _canInstance->read();
            printf("CAN message received: ID=0x%lX, Extended=%s, RTR=%s, Size=%d\n",
                   msg.id, msg.IDE ? "Yes" : "No", msg.RTR ? "Yes" : "No", msg.size);
            if (!msg.RTR && msg.size > 0) {
                printf("Data: ");
                for (int i = 0; i < msg.size; i++) {
                    printf("%02X ", msg.msg[i]);
                }
                printf("\n");
            }
        } else {
            printf("No CAN messages available\n");
        }
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::canSetStandardFilterFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, canSetStandardFilterArgs);

    uint16_t mask = (uint16_t)canSetStandardFilterArgs.mask->ival[0];
    uint16_t filter = (uint16_t)canSetStandardFilterArgs.filter->ival[0];

    if (_canInstance != nullptr) {
        _canInstance->setStandardFilter(mask, filter);
        printf("Standard CAN filter set: mask=0x%04X, filter=0x%04X\n", mask, filter);
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::canSetExtendedFilterFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, canSetExtendedFilterArgs);

    uint32_t mask = (uint32_t)canSetExtendedFilterArgs.mask->ival[0];
    uint32_t filter = (uint32_t)canSetExtendedFilterArgs.filter->ival[0];

    if (_canInstance != nullptr) {
        _canInstance->setExtendedFilter(mask, filter);
        printf("Extended CAN filter set: mask=0x%08lX, filter=0x%08lX\n", mask, filter);
    } else {
        ESP_LOGE(TAG, "Failed to create CAN instance");
        return -1;
    }

    return 0;
}

int CANCLI::init(void)
{
    int err = 0;

    // Register can-begin command
    canBeginArgs.baudrate = arg_int0("b", "baudrate", "<baudrate>", "CAN baudrate: 50000, 100000, 125000, 250000, 500000, 1000000 (default: 1000000)");
    canBeginArgs.extended = arg_lit0("e", "extended", "Use extended frame format (29-bit IDs)");
    canBeginArgs.end = arg_end(3);

    const esp_console_cmd_t canBeginCmd = {
        .command = "can-begin",
        .help = "Initialize CAN communication",
        .hint = NULL,
        .func = &CANCLI::canBeginFunc,
        .argtable = &canBeginArgs,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canBeginCmd);

    // Register can-end command
    const esp_console_cmd_t canEndCmd = {
        .command = "can-end",
        .help = "End CAN communication",
        .hint = NULL,
        .func = &CANCLI::canEndFunc,
        .argtable = NULL,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canEndCmd);

    // Register can-available command
    const esp_console_cmd_t canAvailableCmd = {
        .command = "can-available",
        .help = "Get number of CAN messages available for reading",
        .hint = NULL,
        .func = &CANCLI::canAvailableFunc,
        .argtable = NULL,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canAvailableCmd);

    // Register can-write command
    canWriteArgs.id = arg_int1("i", "id", "<id>", "CAN message ID (0x000-0x7FF for standard, 0x00000000-0x1FFFFFFF for extended)");
    canWriteArgs.data = arg_str0("d", "data", "<data>", "Data in hex format (e.g., \"AABBCCDD\" for 4 bytes)");
    canWriteArgs.extended = arg_lit0("e", "extended", "Use extended frame format (29-bit ID)");
    canWriteArgs.rtr = arg_lit0("r", "rtr", "Send Remote Transmission Request frame");
    canWriteArgs.end = arg_end(5);

    const esp_console_cmd_t canWriteCmd = {
        .command = "can-write",
        .help = "Send a CAN message",
        .hint = NULL,
        .func = &CANCLI::canWriteFunc,
        .argtable = &canWriteArgs,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canWriteCmd);

    // Register can-read command
    const esp_console_cmd_t canReadCmd = {
        .command = "can-read",
        .help = "Read a CAN message",
        .hint = NULL,
        .func = &CANCLI::canReadFunc,
        .argtable = NULL,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canReadCmd);

    // Register can-set-standard-filter command
    canSetStandardFilterArgs.mask = arg_int1("m", "mask", "<mask>", "Standard CAN mask (0x000-0x7FF)");
    canSetStandardFilterArgs.filter = arg_int1("f", "filter", "<filter>", "Standard CAN filter (0x000-0x7FF)");
    canSetStandardFilterArgs.end = arg_end(3);

    const esp_console_cmd_t canSetStandardFilterCmd = {
        .command = "can-set-standard-filter",
        .help = "Set standard CAN ID filter",
        .hint = NULL,
        .func = &CANCLI::canSetStandardFilterFunc,
        .argtable = &canSetStandardFilterArgs,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canSetStandardFilterCmd);

    // Register can-set-extended-filter command
    canSetExtendedFilterArgs.mask = arg_int1("m", "mask", "<mask>", "Extended CAN mask (0x00000000-0x1FFFFFFF)");
    canSetExtendedFilterArgs.filter = arg_int1("f", "filter", "<filter>", "Extended CAN filter (0x00000000-0x1FFFFFFF)");
    canSetExtendedFilterArgs.end = arg_end(3);

    const esp_console_cmd_t canSetExtendedFilterCmd = {
        .command = "can-set-extended-filter",
        .help = "Set extended CAN ID filter",
        .hint = NULL,
        .func = &CANCLI::canSetExtendedFilterFunc,
        .argtable = &canSetExtendedFilterArgs,
        .func_w_context = nullptr,
        .context = nullptr
    };
    err |= esp_console_cmd_register(&canSetExtendedFilterCmd);

    return err;
}
