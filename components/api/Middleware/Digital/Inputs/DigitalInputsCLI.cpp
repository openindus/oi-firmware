/**
 * @file DigitalInputsCLI.cpp
 * @brief Digital Inputs command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputsCLI.h"
#include "Utils.h"

static const char* TAG = "DigitalInputsCLI";

// Static member definitions
decltype(DigitalInputsCLI::digitalReadArgs) DigitalInputsCLI::digitalReadArgs;
decltype(DigitalInputsCLI::attachInterruptArgs) DigitalInputsCLI::attachInterruptArgs;
decltype(DigitalInputsCLI::detachInterruptArgs) DigitalInputsCLI::detachInterruptArgs;

DigitalInputsInterface* DigitalInputsCLI::_digitalInputsInstance = nullptr;

// Macro to create DigitalInputs instance
#define CREATE_DIGITAL_INPUTS_INSTANCE(idArg) \
    do { \
        if (_digitalInputsInstance != nullptr) { \
            delete _digitalInputsInstance; \
            _digitalInputsInstance = nullptr; \
        } \
        _digitalInputsInstance = new DigitalInputs(); \
    } while(0)

#if defined(MODULE_MASTER)
#undef CREATE_DIGITAL_INPUTS_INSTANCE
#define CREATE_DIGITAL_INPUTS_INSTANCE(idArg) \
    do { \
        if (_digitalInputsInstance != nullptr) { \
            delete _digitalInputsInstance; \
            _digitalInputsInstance = nullptr; \
        } \
        if ((idArg) != nullptr && (idArg)->count > 0) { \
            uint16_t moduleId = (uint16_t)(idArg)->ival[0]; \
            _digitalInputsInstance = new DigitalInputsCmd(moduleId); \
        } else { \
            _digitalInputsInstance = new DigitalInputs(); \
        } \
    } while(0)
#endif

// Array of callback functions
static IsrCallback_t interruptCallbacks[] = {
    [] (void *arg) { printf("Interrupt triggered on DIN_1\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_2\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_3\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_4\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_5\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_6\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_7\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_8\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_9\n"); },
    [] (void *arg) { printf("Interrupt triggered on DIN_10\n"); }
};

int DigitalInputsCLI::digitalReadFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, digitalReadArgs);

    // Parse the digital input number
    DIn_Num_t din = (DIn_Num_t)(digitalReadArgs.din->ival[0] - 1);
    if (din >= DIN_MAX) {
        ESP_LOGE(TAG, "Invalid DIN number: %d. Must be between 1 and %d", digitalReadArgs.din->ival[0], DIN_MAX);
        return -1;
    }

#if defined(MODULE_MASTER)
    CREATE_DIGITAL_INPUTS_INSTANCE(digitalReadArgs.id);
#else
    CREATE_DIGITAL_INPUTS_INSTANCE(NULL);
#endif

    int value = -1;
    if (_digitalInputsInstance != nullptr) {
        value = _digitalInputsInstance->digitalRead(din);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }
    printf("%d\n", value);
    
    return 0;
}

int DigitalInputsCLI::attachInterruptFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, attachInterruptArgs);

    // Parse the digital input number
    DIn_Num_t din = (DIn_Num_t)(attachInterruptArgs.din->ival[0] - 1);
    if (din >= DIN_MAX) {
        ESP_LOGE(TAG, "Invalid DIN number: %d. Must be between 1 and %d", attachInterruptArgs.din->ival[0], DIN_MAX);
        return -1;
    }

    // Parse the interrupt mode
    InterruptMode_t mode = CHANGE_MODE; // Default mode
    if (attachInterruptArgs.mode->count > 0) {
        const char *mode_str = attachInterruptArgs.mode->sval[0];
        if (strcmp(mode_str, "rising") == 0) {
            mode = RISING_MODE;
        } else if (strcmp(mode_str, "falling") == 0) {
            mode = FALLING_MODE;
        } else if (strcmp(mode_str, "change") == 0) {
            mode = CHANGE_MODE;
        } else {
            ESP_LOGE(TAG, "Invalid interrupt mode: %s. Must be 'rising', 'falling', or 'change'", mode_str);
            return -1;
        }
    }

#if defined(MODULE_MASTER)
    CREATE_DIGITAL_INPUTS_INSTANCE(attachInterruptArgs.id);
#else
    CREATE_DIGITAL_INPUTS_INSTANCE(NULL);
#endif

    if (_digitalInputsInstance != nullptr) {
        _digitalInputsInstance->attachInterrupt(din, interruptCallbacks[din], mode, NULL);
        printf("Interrupt attached to DIN_%d with mode %s\n", 
            attachInterruptArgs.din->ival[0],
            mode == RISING_MODE ? "RISING" : 
            mode == FALLING_MODE ? "FALLING" : "CHANGE");
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }

    return 0;
}

int DigitalInputsCLI::detachInterruptFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, detachInterruptArgs);

    // Parse the digital input number
    DIn_Num_t din = (DIn_Num_t)(detachInterruptArgs.din->ival[0] - 1);
    if (din >= DIN_MAX) {
        ESP_LOGE(TAG, "Invalid DIN number: %d. Must be between 1 and %d", detachInterruptArgs.din->ival[0], DIN_MAX);
        return -1;
    }

#if defined(MODULE_MASTER)
    CREATE_DIGITAL_INPUTS_INSTANCE(detachInterruptArgs.id);
#else
    CREATE_DIGITAL_INPUTS_INSTANCE(NULL);
#endif

    if (_digitalInputsInstance != nullptr) {
        _digitalInputsInstance->detachInterrupt(din);
        printf("Interrupt detached from DIN_%d\n", detachInterruptArgs.din->ival[0]);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }

    return 0;
}

int DigitalInputsCLI::init(void)
{
    int err = 0;

    // Register digital read command
    digitalReadArgs.din = arg_int1(NULL, NULL, "<din>", "Digital input number (1-10)");
    // digitalReadArgs.din = arg_int1("d", "din", "<din>", "Digital input number (1-10)");
#if defined(MODULE_MASTER)
    digitalReadArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    digitalReadArgs.end = arg_end(3);
#else
    digitalReadArgs.end = arg_end(2);
#endif

    const esp_console_cmd_t digitalReadCmd = {
        .command = "digital-read",
        .help = "Read digital input value",
        .hint = NULL,
        .func = &DigitalInputsCLI::digitalReadFunc,
        .argtable = &digitalReadArgs
    };
    err |= esp_console_cmd_register(&digitalReadCmd);

    // Register attach interrupt command
    attachInterruptArgs.din = arg_int1("d", "din", "<din>", "Digital input number (1-10)");
    attachInterruptArgs.mode = arg_str0("m", "mode", "<mode>", "Interrupt mode: rising, falling, change (default: change)");
#if defined(MODULE_MASTER)
    attachInterruptArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    attachInterruptArgs.end = arg_end(4);
#else
    attachInterruptArgs.end = arg_end(3);
#endif

    const esp_console_cmd_t attachInterruptCmd = {
        .command = "attach-interrupt",
        .help = "Attach interrupt to digital input",
        .hint = NULL,
        .func = &DigitalInputsCLI::attachInterruptFunc,
        .argtable = &attachInterruptArgs
    };
    err |= esp_console_cmd_register(&attachInterruptCmd);

    // Register detach interrupt command
    detachInterruptArgs.din = arg_int1("d", "din", "<din>", "Digital input number (1-10)");
#if defined(MODULE_MASTER)
    detachInterruptArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    detachInterruptArgs.end = arg_end(3);
#else
    detachInterruptArgs.end = arg_end(2);
#endif

    const esp_console_cmd_t detachInterruptCmd = {
        .command = "detach-interrupt",
        .help = "Detach interrupt from digital input",
        .hint = NULL,
        .func = &DigitalInputsCLI::detachInterruptFunc,
        .argtable = &detachInterruptArgs
    };
    err |= esp_console_cmd_register(&detachInterruptCmd);

    return err;
}
