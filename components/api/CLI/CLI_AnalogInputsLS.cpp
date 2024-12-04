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
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _rtdReadCmdArgs;

static struct {
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _tcReadCmdArgs;

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
    // - [X] TASK make this a single array instead of the whole pinout that is half unused

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

static int _rtdReadCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_rtdReadCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _rtdReadCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _rtdReadCmdArgs.sensorIndex->ival[0];

    // get RTD value in Ohm at Index position
    float rRTD = 0.0;
    rRTD = AnalogInputsLS::sensors[sensorIndex]->readResistor();
    // print value
    printf("[%d] : %3.4f Ohms\n", sensorIndex, rRTD);

    return 0;
}

static int _tcReadVCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_tcReadCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _tcReadCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _tcReadCmdArgs.sensorIndex->ival[0];

    // get TC value in Volt at Index position
    float rTCv = 0.0;
    rTCv = AnalogInputsLS::sensors[sensorIndex]->readMillivolts();
    // print value
    printf("[%d] : %.4f Millivolts\n", sensorIndex, rTCv);

    return 0;
}

static int _tcReadCCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_tcReadCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _tcReadCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _tcReadCmdArgs.sensorIndex->ival[0];

    // get TC value in °C at Index position
    float rTCc = 0.0;
    rTCc = AnalogInputsLS::sensors[sensorIndex]->readTemperature();
    // print value
    printf("[%d] : %.4f degC\n", sensorIndex, rTCc);

    return 0;
}

static int _registerRtdReadCmd(void)
{
    _rtdReadCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _rtdReadCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "rtd-read",
        .help = "Commands for reading RTD value in Ohms",
        .hint = NULL,
        .func = &_rtdReadCmdHandler,
        .argtable = &_rtdReadCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

static int _registerTcReadVCmd(void)
{
    _tcReadCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _tcReadCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "tcv-read",
        .help = "Commands for reading Thermocouple (type K) value in Volts",
        .hint = NULL,
        .func = &_tcReadVCmdHandler,
        .argtable = &_tcReadCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

static int _registerTcReadCCmd(void)
{
    _tcReadCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _tcReadCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "tcc-read",
        .help = "Commands for reading Thermocouple (type K) value in °C",
        .hint = NULL,
        .func = &_tcReadCCmdHandler,
        .argtable = &_tcReadCmdArgs
    };
    return esp_console_cmd_register(&cmd);
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

static struct {
    struct arg_int *sensorIndex;
    struct arg_end *end;
} _rawReadCmdArgs;

static int _rawReadCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_rawReadCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _rawReadCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _rawReadCmdArgs.sensorIndex->ival[0];
    Sensor *sensor = AnalogInputsLS::sensors[sensorIndex];
    AnalogInputsLS::getHighSideMux()->route(INPUT_SENSOR_5V, 0);
    AnalogInputsLS::getLowSideMux()->route(4, OUTPUT_GND);
    // Read
    int16_t raw_value = sensor->read();
    // AnalogInputsLS::getHighSideMux()->route(INPUT_OPEN_HS, 4);
    // AnalogInputsLS::getLowSideMux()->route(4, OUTPUT_OPEN_LS);
    // Display
    printf("[%d] : %hd\n", sensorIndex, raw_value);
    return 0;
}

static int _registerRawReadCmd(void)
{
    _rawReadCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _rawReadCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "raw-read",
        .help = "Command for getting a raw sensor value",
        .hint = NULL,
        .func = &_rawReadCmdHandler,
        .argtable = &_rawReadCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

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

// Register all CLI commands
int CLI::_registerAnalogInputsLSCmd(void)
{
    int ret = 0;
    ret |= _registerMuxRoute();
    ret |= _registerSensorSetParameterCmd();
    ret |= _registerRawReadCmd();
    ret |= _registerRtdReadCmd();
    ret |= _registerTcReadVCmd();
    ret |= _registerTcReadCCmd();
    ret |= _registerAddSensorCmd();
    return ret;
}

#endif
