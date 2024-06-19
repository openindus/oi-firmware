/**
 * @file CLI_AnalogLS.cpp
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"
#include "AnalogLS.h"

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
        mux = new Multiplexer {
            {ANALOG_LS_MUX_PIN_HS1_A0, ANALOG_LS_MUX_PIN_HS1_A1, ANALOG_LS_MUX_PIN_HS1_A2},
            {ANALOG_LS_MUX_PIN_HS2_A0, ANALOG_LS_MUX_PIN_HS2_A1, ANALOG_LS_MUX_PIN_HS2_A2}
    };
    } else if (!strcmp(mode, "LS")) {
        mux = new Multiplexer(
            {ANALOG_LS_MUX_PIN_LS1_A0, ANALOG_LS_MUX_PIN_LS1_A1, ANALOG_LS_MUX_PIN_LS1_A2},
            {ANALOG_LS_MUX_PIN_LS2_A0, ANALOG_LS_MUX_PIN_LS2_A1, ANALOG_LS_MUX_PIN_LS2_A2}
        );
    } else {
        ESP_LOGE(TAG, "Mode must be 'HS' or 'LS'");
        return -1;    
    }

    if (mux->route(input, output) != 0) {
        return -1;
    }

    delete mux;
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

int CLI::_registerAnalogLSCmd(void)
{
    int ret = 0;
    ret |= _registerMuxRoute();
    return ret;
}

#endif