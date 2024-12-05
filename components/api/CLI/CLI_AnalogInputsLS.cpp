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

/* MUX ROUTE */

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

/* ADD SENSOR COMMAND */

static struct {
    struct arg_int *sensorType;
    struct arg_int *inputP;
    struct arg_int *inputN;
    struct arg_int *additionnalPin1;
    struct arg_int *additionnalPin2;
    struct arg_end *end;
} _AddSensorCmdArgs;

static int _addSensorCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_AddSensorCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _AddSensorCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorType = _AddSensorCmdArgs.sensorType->ival[0];
    std::array<AIn_Num_e, 4> ainPins = {AIN_NULL, AIN_NULL, AIN_NULL, AIN_NULL};

    ainPins[0] = (AIn_Num_e) _AddSensorCmdArgs.inputP->ival[0];
    ainPins[1] = (AIn_Num_e) _AddSensorCmdArgs.inputN->ival[0];
    if (_AddSensorCmdArgs.additionnalPin1->count == 1) {
        ainPins[2] = (AIn_Num_e) _AddSensorCmdArgs.additionnalPin1->ival[0];
    }
    if (_AddSensorCmdArgs.additionnalPin2->count == 1) {
        ainPins[3] = (AIn_Num_e) _AddSensorCmdArgs.additionnalPin2->ival[0];
    }

    // add sensor (on a list)
    AnalogInputsLS::addSensor((Sensor_Type_e) sensorType, ainPins);
    
    return 0;
}

static int _registerAddSensorCmd(void)
{
    _AddSensorCmdArgs.sensorType = arg_int1(NULL, NULL, "<sensorType>", "Sensor type");
    _AddSensorCmdArgs.inputP = arg_int1(NULL, NULL, "<inputP>", "Positive ADC input (AINp)");
    _AddSensorCmdArgs.inputN = arg_int1(NULL, NULL, "<inputN>", "Negative ADC input (AINn)");
    _AddSensorCmdArgs.additionnalPin1 = arg_int0(NULL, NULL, "<additionnalPin1>", "3rd wire RTD ADC input or excitation output (AINnp)");
    _AddSensorCmdArgs.additionnalPin2 = arg_int0(NULL, NULL, "<additionnalPin2>", "excitation output (AINnp)");
    _AddSensorCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "add-sensor",
        .help = "Commands for adding sensor",
        .hint = NULL,
        .func = &_addSensorCmdHandler,
        .argtable = &_AddSensorCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

/* SENSOR SET PARAMETER */

static struct {
    struct arg_int *sensorIndex;
    struct arg_int *parameter_index;
    struct arg_int *parameter_value;
    struct arg_end *end;
} _sensorSetParameterCmdArgs;

static int _sensorSetParameterCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorSetParameterCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _sensorSetParameterCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _sensorSetParameterCmdArgs.sensorIndex->ival[0];
    int parameter_index = _sensorSetParameterCmdArgs.parameter_index->ival[0];
    uint8_t parameter_value = _sensorSetParameterCmdArgs.parameter_value->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    // set
    sensor->setParameter((Sensor_Parameter_e) parameter_index, (Sensor_Parameter_Value_u) {.value=parameter_value});
    return 0;
}

static int _registerSensorSetParameterCmd(void)
{
    _sensorSetParameterCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _sensorSetParameterCmdArgs.parameter_index = arg_int1(NULL, NULL, "<parameter>", "Parameter index");
    _sensorSetParameterCmdArgs.parameter_value = arg_int1(NULL, NULL, "<value>", "Parameter value");
    _sensorSetParameterCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-set-parameter",
        .help = "Command for setting a parameter in a sensor",
        .hint = NULL,
        .func = &_sensorSetParameterCmdHandler,
        .argtable = &_sensorSetParameterCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

// - [ ] TASK add a sensor-read command that reads no matter what type the sensor is.
// - [ ] TASK add a sensor-read-tension command
// - [ ] TASK add a sensor-read-resistance command
// - [ ] TASK add a sensor-read-temperature command
// - [ ] TASK add a sensor-read-raw command
// - [X] TASK remove sensor-specific commands.

// Register all CLI commands
int CLI::_registerAnalogInputsLSCmd(void)
{
    int ret = 0;
    ret |= _registerMuxRoute();
    ret |= _registerSensorSetParameterCmd();
    ret |= _registerAddSensorCmd();
    return ret;
}

#endif
