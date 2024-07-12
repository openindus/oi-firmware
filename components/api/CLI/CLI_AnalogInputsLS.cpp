/**
 * @file CLI_AnalogInputsLS.cpp
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"
#include "AnalogInputsLS.h"

#if defined(OI_ANALOG_LS)

static const char TAG[] = "CLI";

/* --- Multiplexer --- */

static struct {
    struct arg_str *mode;
    struct arg_int *input;
    struct arg_int *output;
    struct arg_end *end;
} _muxRouteArgs;

static int _muxRoute(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **)&_muxRouteArgs);
    if (err != 0) {
        arg_print_errors(stderr, _muxRouteArgs.end, argv[0]);
        return -1;
    }

    const char *mode = _muxRouteArgs.mode->sval[0];
    int input = _muxRouteArgs.input->ival[0];
    int output = _muxRouteArgs.output->ival[0];

    Multiplexer* mux;

    if (!strcmp(mode, "HS")) {
        mux = AnalogInputsLS::getHighSideMux();
    } else if (!strcmp(mode, "LS")) {
        mux = AnalogInputsLS::getLowSideMux();
    } else {
        ESP_LOGE(TAG, "Mode must be 'HS' or 'LS'");
        return -1;    
    }

    if (mux != NULL) {
        if (mux->route(input, output) != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

static int _registerMuxRoute(void)
{
    _muxRouteArgs.mode = arg_str1(NULL, NULL, "[HS, LS]", "HS = High Side, LS = Low Side");
    _muxRouteArgs.input = arg_int1(NULL, NULL, "<input>", "Input channel");
    _muxRouteArgs.output = arg_int1(NULL, NULL, "<output>", "Output channel");
    _muxRouteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "mux-route",
        .help = "Configure mux route",
        .hint = NULL,
        .func = &_muxRoute,
        .argtable = &_muxRouteArgs
    };
    return esp_console_cmd_register(&cmd);
}

/* --- ADS114S0X Read --- */

static struct {
    struct arg_int *inputP;
    struct arg_int *inputN;
    struct arg_lit *useVbias;
    struct arg_end *end;
} _adcReadCmdArgs;

static int _adcReadCmdHandler(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **)&_adcReadCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _adcReadCmdArgs.end, argv[0]);
        return -1;
    }

    int inputP = _adcReadCmdArgs.inputP->ival[0];
    int inputN = _adcReadCmdArgs.inputN->ival[0];
    bool useVbias = _adcReadCmdArgs.useVbias->count > 0;

    ADS114S0X* adc = AnalogInputsLS::getAdcDevice();
    if (adc != NULL) {
        std::vector<uint16_t> adcCode;
        adc->read(&adcCode, static_cast<ADC_Input_t>(inputP), static_cast<ADC_Input_t>(inputN), useVbias);
        for (int i = 0; i < adcCode.size(); i++) {
            printf("%d\n", adcCode[i]);
        }
    } else {
        return -1;
    }

    return 0;
}

static int _registerAdcReadCmd(void)
{
    _adcReadCmdArgs.inputP = arg_int1(NULL, NULL, "<inputP>", "Positive ADC input (AINp)");
    _adcReadCmdArgs.inputN = arg_int1(NULL, NULL, "<inputN>", "Negative ADC input (AINn)");
    _adcReadCmdArgs.useVbias = arg_lit0(NULL, "useVbias", "Use bias voltage");
    _adcReadCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "adc-read",
        .help = "Commands for read ADC code",
        .hint = NULL,
        .func = &_adcReadCmdHandler,
        .argtable = &_adcReadCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

/* --- ADS114S0X Config --- */

static struct {
    struct arg_int *gain;
    struct arg_int *reference;
    struct arg_lit *excitation;
    struct arg_end *end;
} _adcConfigCmdArgs;

static int _adcConfigCmdHandler(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **)&_adcConfigCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _adcConfigCmdArgs.end, argv[0]);
        return -1;
    }

    int gain = _adcConfigCmdArgs.gain->ival[0];
    int reference = _adcConfigCmdArgs.reference->ival[0];
    bool useExcitation = _adcConfigCmdArgs.excitation->count > 0;

    ADS114S0X* adc = AnalogInputsLS::getAdcDevice();
    if (adc != NULL) {
        if (adc->config(
                static_cast<ADS114S0X_Gain_e>(gain), 
                static_cast<ADS114S0X_Reference_e>(reference), 
                useExcitation) != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

static int _registerAdcConfigCmd(void)
{
    _adcConfigCmdArgs.gain = arg_int1(NULL, NULL, "<gain>", "ADC gain (1, 2, 4, 8, 16, 32, 64, 128)");
    _adcConfigCmdArgs.reference = arg_int1(NULL, NULL, "<reference>", "ADC reference (1 = External, 2 = Internal)");
    _adcConfigCmdArgs.excitation = arg_lit0(NULL, "excitation", "Use excitation");
    _adcConfigCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "adc-config",
        .help = "Configure ADC settings",
        .hint = NULL,
        .func = &_adcConfigCmdHandler,
        .argtable = &_adcConfigCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

// Register all CLI commands
int CLI::_registerAnalogInputsLSCmd(void)
{
    int ret = 0;
    ret |= _registerMuxRoute();
    ret |= _registerAdcReadCmd();
    ret |= _registerAdcConfigCmd();
    return ret;
}

#endif
