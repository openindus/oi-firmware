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
    int index = AnalogInputsLS::addSensor((Sensor_Type_e) sensorType, ainPins);
    
    printf("%d\n", index);
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
    int32_t parameter_value = _sensorSetParameterCmdArgs.parameter_value->ival[0];
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

/* SENSOR READ */

static struct
{
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _sensorReadCmdArgs;

static int _sensorReadCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorReadCmdArgs);
    if (err != 0)
    {
        arg_print_errors(stderr, _sensorReadCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _sensorReadCmdArgs.sensorIndex->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    // read
    sensor->read(true);
    return 0;
}

static int _registerSensorReadCmd(void)
{
    _sensorReadCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _sensorReadCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-read",
        .help = "Command for reading a sensor value (default function for each sensors)",
        .hint = NULL,
        .func = &_sensorReadCmdHandler,
        .argtable = &_sensorReadCmdArgs};
    return esp_console_cmd_register(&cmd);
}

/* SENSOR READ TENSION */

static struct {
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _sensorReadTensionCmdArgs;

static int _sensorReadTensionCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorReadTensionCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _sensorReadTensionCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _sensorReadTensionCmdArgs.sensorIndex->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    // read
    sensor->readMillivolts(true);
    return 0;
}

static int _registerSensorReadTensionCmd(void)
{
    _sensorReadTensionCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _sensorReadTensionCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-read-tension",
        .help = "Command for reading a sensor value (default function for each sensors)",
        .hint = NULL,
        .func = &_sensorReadTensionCmdHandler,
        .argtable = &_sensorReadTensionCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

/* SENSOR READ RESISTANCE */

static struct
{
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _sensorReadResistanceCmdArgs;

static int _sensorReadResistanceCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorReadResistanceCmdArgs);
    if (err != 0)
    {
        arg_print_errors(stderr, _sensorReadResistanceCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _sensorReadResistanceCmdArgs.sensorIndex->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    // read
    sensor->readResistor(true);
    return 0;
}

static int _registerSensorReadResistanceCmd(void)
{
    _sensorReadResistanceCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _sensorReadResistanceCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-read-resistance",
        .help = "Command for reading a sensor resistance value",
        .hint = NULL,
        .func = &_sensorReadResistanceCmdHandler,
        .argtable = &_sensorReadResistanceCmdArgs};
    return esp_console_cmd_register(&cmd);
}

/* SENSOR READ TEMPERATURE */

static struct
{
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _sensorReadTemperatureCmdArgs;

static int _sensorReadTemperatureCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorReadTemperatureCmdArgs);
    if (err != 0)
    {
        arg_print_errors(stderr, _sensorReadTemperatureCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _sensorReadTemperatureCmdArgs.sensorIndex->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    // read
    sensor->readTemperature(true);
    return 0;
}

static int _registerSensorReadTemperatureCmd(void)
{
    _sensorReadTemperatureCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _sensorReadTemperatureCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-read-temperature",
        .help = "Command for reading a sensor temperature value",
        .hint = NULL,
        .func = &_sensorReadTemperatureCmdHandler,
        .argtable = &_sensorReadTemperatureCmdArgs};
    return esp_console_cmd_register(&cmd);
}

/* SENSOR READ RAW VALUE */

static struct
{
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _sensorReadRawCmdArgs;

static int _sensorReadRawCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorReadRawCmdArgs);
    if (err != 0)
    {
        arg_print_errors(stderr, _sensorReadRawCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _sensorReadRawCmdArgs.sensorIndex->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    // read
    sensor->raw_read(0, 1, true);
    return 0;
}

static int _registerSensorReadRawCmd(void)
{
    _sensorReadRawCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _sensorReadRawCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-read-raw",
        .help = "Command for reading a sensor raw value",
        .hint = NULL,
        .func = &_sensorReadRawCmdHandler,
        .argtable = &_sensorReadRawCmdArgs};
    return esp_console_cmd_register(&cmd);
}

/* SENSOR LIST */

static struct
{
    struct arg_end *end;
} _sensorListCmdArgs;

static int _sensorListCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_sensorListCmdArgs);
    if (err != 0)
    {
        arg_print_errors(stderr, _sensorListCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    AnalogInputsLS::list_sensors();
    return 0;
}

static int _registerSensorListCmd(void)
{
    _sensorListCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "sensor-list",
        .help = "Command for listing all sensors",
        .hint = NULL,
        .func = &_sensorListCmdHandler,
        .argtable = &_sensorListCmdArgs};
    return esp_console_cmd_register(&cmd);
}


// Register all CLI commands
int CLI::_registerAnalogInputsLSCmd(void)
{
    int ret = 0;
    ret |= _registerMuxRoute();
    ret |= _registerSensorSetParameterCmd();
    ret |= _registerSensorReadCmd();
    ret |= _registerSensorReadRawCmd();
    ret |= _registerSensorReadTemperatureCmd();
    ret |= _registerSensorReadTensionCmd();
    ret |= _registerSensorReadResistanceCmd();
    ret |= _registerSensorListCmd();
    ret |= _registerAddSensorCmd();
    return ret;
}

#endif
