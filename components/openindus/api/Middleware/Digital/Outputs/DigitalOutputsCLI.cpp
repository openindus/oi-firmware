/**
 * @file DigitalOutputsCLI.cpp
 * @brief Command line interface - Digital Outputs
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsCLI.h"
#include "Utils.h"

static const char* TAG = "DigitalOutputsCLI";

// Static member definitions
decltype(DigitalOutputsCLI::digitalWriteArgs) DigitalOutputsCLI::digitalWriteArgs;
decltype(DigitalOutputsCLI::toggleOutputArgs) DigitalOutputsCLI::toggleOutputArgs;
decltype(DigitalOutputsCLI::outputModeArgs) DigitalOutputsCLI::outputModeArgs;
decltype(DigitalOutputsCLI::setPWMFrequencyArgs) DigitalOutputsCLI::setPWMFrequencyArgs;
decltype(DigitalOutputsCLI::setPWMDutyCycleArgs) DigitalOutputsCLI::setPWMDutyCycleArgs;
decltype(DigitalOutputsCLI::getOutputCurrentArgs) DigitalOutputsCLI::getOutputCurrentArgs;
decltype(DigitalOutputsCLI::outputIsOvercurrentArgs) DigitalOutputsCLI::outputIsOvercurrentArgs;

DigitalOutputsInterface* DigitalOutputsCLI::_digitalOutputsInstance = nullptr;

// Macro to create DigitalOutputs instance
#define CREATE_DIGITAL_OUTPUTS_INSTANCE(idArg) \
    do { \
        if (_digitalOutputsInstance != nullptr) { \
            delete _digitalOutputsInstance; \
            _digitalOutputsInstance = nullptr; \
        } \
        _digitalOutputsInstance = new DigitalOutputs(); \
    } while(0)

#if defined(CONFIG_MODULE_MASTER)
#undef CREATE_DIGITAL_OUTPUTS_INSTANCE
#define CREATE_DIGITAL_OUTPUTS_INSTANCE(idArg) \
    do { \
        if (_digitalOutputsInstance != nullptr) { \
            delete _digitalOutputsInstance; \
            _digitalOutputsInstance = nullptr; \
        } \
        if ((idArg) != nullptr && (idArg)->count > 0) { \
            uint16_t moduleId = (uint16_t)(idArg)->ival[0]; \
            _digitalOutputsInstance = new DigitalOutputsCmd(moduleId); \
        } else { \
            _digitalOutputsInstance = new DigitalOutputs(); \
        } \
    } while(0)
#endif

// Callback function for overcurrent testing
static void __attribute__((unused)) overcurrentCallback(void *arg)
{
    DOut_Num_t dout = (DOut_Num_t)(uintptr_t)arg;
    printf("Overcurrent detected on DOUT_%d\n", dout + 1);
}

int DigitalOutputsCLI::digitalWriteFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, digitalWriteArgs);

    DOut_Num_t dout = (DOut_Num_t)(digitalWriteArgs.dout->ival[0] - 1);
    bool level = digitalWriteArgs.level->ival[0] != 0;

    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 digitalWriteArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(digitalWriteArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    if (_digitalOutputsInstance != nullptr) {
        _digitalOutputsInstance->digitalWrite(dout, level);
        printf("DOUT_%d set to %s\n", digitalWriteArgs.dout->ival[0], level ? "HIGH" : "LOW");
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }
    
    return 0;
}

int DigitalOutputsCLI::toggleOutputFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, toggleOutputArgs);

    DOut_Num_t dout = (DOut_Num_t)(toggleOutputArgs.dout->ival[0] - 1);
    
    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 toggleOutputArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(toggleOutputArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    if (_digitalOutputsInstance != nullptr) {
        _digitalOutputsInstance->toggleOutput(dout);
        printf("DOUT_%d toggled\n", toggleOutputArgs.dout->ival[0]);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }

    return 0;
}

int DigitalOutputsCLI::outputModeFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, outputModeArgs);

    DOut_Num_t dout = (DOut_Num_t)(outputModeArgs.dout->ival[0] - 1);
    
    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 outputModeArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

    DOut_Mode_t mode = DOUT_MODE_DIGITAL; // Default mode
    
    if (outputModeArgs.mode->count > 0) {
        const char *mode_str = outputModeArgs.mode->sval[0];
        if (strcmp(mode_str, "digital") == 0) {
            mode = DOUT_MODE_DIGITAL;
        } else if (strcmp(mode_str, "pwm") == 0) {
            mode = DOUT_MODE_PWM;
        } else {
            ESP_LOGE(TAG, "Invalid output mode: %s. Must be 'digital' or 'pwm'", mode_str);
            return -1;
        }
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(outputModeArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    if (_digitalOutputsInstance != nullptr) {
        _digitalOutputsInstance->outputMode(dout, mode);
        printf("DOUT_%d mode set to %s\n", 
            outputModeArgs.dout->ival[0],
            mode == DOUT_MODE_DIGITAL ? "DIGITAL" : "PWM");
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }

    return 0;
}

int DigitalOutputsCLI::setPWMFrequencyFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setPWMFrequencyArgs);

    DOut_Num_t dout = (DOut_Num_t)(setPWMFrequencyArgs.dout->ival[0] - 1);
    uint32_t freq = (uint32_t)setPWMFrequencyArgs.freq->ival[0];
    
    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 setPWMFrequencyArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(setPWMFrequencyArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    if (_digitalOutputsInstance != nullptr) {
        _digitalOutputsInstance->setPWMFrequency(dout, freq);
        printf("DOUT_%d PWM frequency set to %lu Hz\n", 
            setPWMFrequencyArgs.dout->ival[0], freq);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }

    return 0;
}

int DigitalOutputsCLI::setPWMDutyCycleFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setPWMDutyCycleArgs);

    DOut_Num_t dout = (DOut_Num_t)(setPWMDutyCycleArgs.dout->ival[0] - 1);
    float duty = (float)setPWMDutyCycleArgs.duty->dval[0];
    
    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 setPWMDutyCycleArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

    if (duty < 0.0f || duty > 100.0f) {
        ESP_LOGE(TAG, "Invalid duty cycle: %.2f. Must be between 0.0 and 100.0", duty);
        return -1;
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(setPWMDutyCycleArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    if (_digitalOutputsInstance != nullptr) {
        _digitalOutputsInstance->setPWMDutyCycle(dout, duty);
        printf("DOUT_%d PWM duty cycle set to %.2f%%\n", 
            setPWMDutyCycleArgs.dout->ival[0], duty);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }

    return 0;
}

int DigitalOutputsCLI::getOutputCurrentFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, getOutputCurrentArgs);

    DOut_Num_t dout = (DOut_Num_t)(getOutputCurrentArgs.dout->ival[0] - 1);
    
    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 getOutputCurrentArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(getOutputCurrentArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    float current = -1.0f;
    if (_digitalOutputsInstance != nullptr) {
        current = _digitalOutputsInstance->getOutputCurrent(dout);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }
    printf("%.3f\n", current);

    return 0;
}

int DigitalOutputsCLI::outputIsOvercurrentFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, outputIsOvercurrentArgs);

    DOut_Num_t dout = (DOut_Num_t)(outputIsOvercurrentArgs.dout->ival[0] - 1);
    
    if (dout >= DOUT_MAX) {
        ESP_LOGE(TAG, "Invalid DOUT number: %d. Must be between 1 and %d", 
                 outputIsOvercurrentArgs.dout->ival[0], DOUT_MAX);
        return -1;
    }

#if defined(CONFIG_MODULE_MASTER)
    CREATE_DIGITAL_OUTPUTS_INSTANCE(outputIsOvercurrentArgs.id);
#else
    CREATE_DIGITAL_OUTPUTS_INSTANCE(NULL);
#endif

    int overcurrent = -1;
    if (_digitalOutputsInstance != nullptr) {
        overcurrent = _digitalOutputsInstance->outputIsOvercurrent(dout);
    } else {
        ESP_LOGE(TAG, "Failed to create instance");
        return -1;
    }
    printf("%d\n", overcurrent);

    return 0;
}

int DigitalOutputsCLI::init(void)
{
    int err = 0;

    // Register digital write command
    digitalWriteArgs.dout = arg_int1(NULL, NULL, "<dout>", "Digital output number (1-8)");
    digitalWriteArgs.level = arg_int1(NULL, NULL, "<level>", "Output level (0=LOW, 1=HIGH)");
    // digitalWriteArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
    // digitalWriteArgs.level = arg_int1("l", "level", "<level>", "Output level (0=LOW, 1=HIGH)");
#if defined(CONFIG_MODULE_MASTER)
    digitalWriteArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    digitalWriteArgs.end = arg_end(4);
#else
    digitalWriteArgs.end = arg_end(3);
#endif

    const esp_console_cmd_t digitalWriteCmd = {
        .command = "digital-write",
        .help = "Set digital output level",
        .hint = NULL,
        .func = &DigitalOutputsCLI::digitalWriteFunc,
        .argtable = &digitalWriteArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&digitalWriteCmd);

    // Register toggle output command
    toggleOutputArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
#if defined(CONFIG_MODULE_MASTER)
    toggleOutputArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    toggleOutputArgs.end = arg_end(3);
#else
    toggleOutputArgs.end = arg_end(2);
#endif

    const esp_console_cmd_t toggleOutputCmd = {
        .command = "toggle-output",
        .help = "Toggle digital output",
        .hint = NULL,
        .func = &DigitalOutputsCLI::toggleOutputFunc,
        .argtable = &toggleOutputArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&toggleOutputCmd);

    // Register output mode command
    outputModeArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
    outputModeArgs.mode = arg_str1("m", "mode", "<mode>", "Output mode: digital, pwm");
#if defined(CONFIG_MODULE_MASTER)
    outputModeArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    outputModeArgs.end = arg_end(4);
#else
    outputModeArgs.end = arg_end(3);
#endif

    const esp_console_cmd_t outputModeCmd = {
        .command = "output-mode",
        .help = "Set output mode (digital/pwm)",
        .hint = NULL,
        .func = &DigitalOutputsCLI::outputModeFunc,
        .argtable = &outputModeArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&outputModeCmd);

    // Register PWM frequency command
    setPWMFrequencyArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
    setPWMFrequencyArgs.freq = arg_int1("f", "freq", "<freq>", "PWM frequency in Hz (50-1000)");
#if defined(CONFIG_MODULE_MASTER)
    setPWMFrequencyArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    setPWMFrequencyArgs.end = arg_end(4);
#else
    setPWMFrequencyArgs.end = arg_end(3);
#endif

    const esp_console_cmd_t setPWMFrequencyCmd = {
        .command = "set-pwm-frequency",
        .help = "Set PWM frequency",
        .hint = NULL,
        .func = &DigitalOutputsCLI::setPWMFrequencyFunc,
        .argtable = &setPWMFrequencyArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&setPWMFrequencyCmd);

    // Register PWM duty cycle command
    setPWMDutyCycleArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
    setPWMDutyCycleArgs.duty = arg_dbl1("c", "duty", "<duty>", "PWM duty cycle in % (0.0-100.0)");
#if defined(CONFIG_MODULE_MASTER)
    setPWMDutyCycleArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    setPWMDutyCycleArgs.end = arg_end(4);
#else
    setPWMDutyCycleArgs.end = arg_end(3);
#endif

    const esp_console_cmd_t setPWMDutyCycleCmd = {
        .command = "set-pwm-duty-cycle",
        .help = "Set PWM duty cycle",
        .hint = NULL,
        .func = &DigitalOutputsCLI::setPWMDutyCycleFunc,
        .argtable = &setPWMDutyCycleArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&setPWMDutyCycleCmd);

    // Register get output current command
    getOutputCurrentArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
#if defined(CONFIG_MODULE_MASTER)
    getOutputCurrentArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    getOutputCurrentArgs.end = arg_end(3);
#else
    getOutputCurrentArgs.end = arg_end(2);
#endif

    const esp_console_cmd_t getOutputCurrentCmd = {
        .command = "get-output-current",
        .help = "Get output current in Amperes",
        .hint = NULL,
        .func = &DigitalOutputsCLI::getOutputCurrentFunc,
        .argtable = &getOutputCurrentArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&getOutputCurrentCmd);

    // Register output is overcurrent command
    outputIsOvercurrentArgs.dout = arg_int1("d", "dout", "<dout>", "Digital output number (1-8)");
#if defined(CONFIG_MODULE_MASTER)
    outputIsOvercurrentArgs.id = arg_int0("i", "id", "<id>", "Module ID for remote operation");
    outputIsOvercurrentArgs.end = arg_end(3);
#else
    outputIsOvercurrentArgs.end = arg_end(2);
#endif

    const esp_console_cmd_t outputIsOvercurrentCmd = {
        .command = "output-is-overcurrent",
        .help = "Check if output is in overcurrent state",
        .hint = NULL,
        .func = &DigitalOutputsCLI::outputIsOvercurrentFunc,
        .argtable = &outputIsOvercurrentArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    err |= esp_console_cmd_register(&outputIsOvercurrentCmd);

    return err;
}
