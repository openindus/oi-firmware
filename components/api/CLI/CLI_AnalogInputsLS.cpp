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
    struct arg_int *inputM;
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
    std::vector<AIn_Num_t> inputNum;

    if (_AddSensorCmdArgs.inputP->count == 1) {
        inputNum.push_back((AIn_Num_t)_AddSensorCmdArgs.inputP->ival[0]);
    }
    if (_AddSensorCmdArgs.inputN->count == 1) {
        inputNum.push_back((AIn_Num_t)_AddSensorCmdArgs.inputN->ival[0]);
    }
    //for 3 wires RTD, get 3rd wire pinout info
    if (_AddSensorCmdArgs.inputM->count == 1) {
        inputNum.push_back((AIn_Num_t)_AddSensorCmdArgs.inputM->ival[0]);
    }
    
    // add sensor (on a list)
    AnalogInputsLS::addSensor((Sensor_Type_e) sensorType, inputNum);
    
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
    rRTD = AnalogInputsLS::rtd[sensorIndex].readResistor();
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
    rTCv = AnalogInputsLS::tc[sensorIndex].readMillivolts();
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
    rTCc = AnalogInputsLS::tc[sensorIndex].readTemperature();
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
    _AddSensorCmdArgs.inputM = arg_int0(NULL, NULL, "<inputM>", "3rd wire RTD ADC input (AINn)");
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
    struct arg_int *ref;
    struct arg_end *end;
} _rawSetRefCmdArgs;

static int _rawSetRefCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_rawSetRefCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _rawSetRefCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _rawSetRefCmdArgs.sensorIndex->ival[0];
    int reference_int = _rawSetRefCmdArgs.ref->ival[0];

    RawSensor *sensor = &AnalogInputsLS::raw[sensorIndex];
    switch (reference_int) {
        case 0:
            sensor->setReference(REFERENCE_EXCITATION);
            break;
        case 1:
            sensor->setReference(REFERENCE_IDAC_1);
            break;
        case 2:
            sensor->setReference(REFERENCE_INTERNAL_2_5V);
            break;
        default:
            (void) printf("Error: The reference %d does not exist.", reference_int);
            return 1;
    };
    return 0;
}

static int _registerRawSetRefCmd(void)
{
    _rawSetRefCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _rawSetRefCmdArgs.ref = arg_int1(NULL, NULL, "<ref>", "Ref (0, 1, 2) = (excitation, idac1, internal)");
    _rawSetRefCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "raw-set-ref",
        .help = "Command for setting a raw sensor reference",
        .hint = NULL,
        .func = &_rawSetRefCmdHandler,
        .argtable = &_rawSetRefCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_int *sensorIndex;
    struct arg_int *gain;
    struct arg_end *end;
} _rawSetGainCmdArgs;

static int8_t _get_max_on_bit(int number)
{
    int8_t max_bit = -1;

    for (int i = 0; i < (sizeof(int) * 8); i++) {
        if ((1 << i) & number) {
            max_bit = i;
        }
    }
    return max_bit;
}

static bool _is_power_2(int number)
{
    uint8_t nbr_bits = 0;

    for (int i = 0; i < (sizeof(int) * 8); i++) {
        if ((1 << i) & number) {
            nbr_bits++;
        }
    }
    return nbr_bits == 1; // if only 1 bit is up, the number is a power of 2
}

static int _rawSetGainCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_rawSetGainCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _rawSetGainCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _rawSetGainCmdArgs.sensorIndex->ival[0];
    int gain_int = _rawSetGainCmdArgs.gain->ival[0];

    RawSensor *sensor = &AnalogInputsLS::raw[sensorIndex];
    if (gain_int > 128 || !_is_power_2(gain_int)) {
        printf("The number %d is not a valid gain (1, 2, 4, 8, 16, 32, 64, 128)\n", gain_int);
        return 1;
    }
    sensor->setGain((Sensor_Gain_e) _get_max_on_bit(gain_int));
    // if 1 -> 0, 2 -> 1, 4 -> 2, 8 -> 3, 16 -> 4, 32 -> 5, 64 -> 6, 128 -> 7
    return 0;
}

static int _registerRawSetGainCmd(void)
{
    _rawSetGainCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _rawSetGainCmdArgs.gain = arg_int1(NULL, NULL, "<gain>", "Gain (1, 2, 4, 8, 16, 32, 64, 128)");
    _rawSetGainCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "raw-set-gain",
        .help = "Command for setting a raw sensor gain",
        .hint = NULL,
        .func = &_rawSetGainCmdHandler,
        .argtable = &_rawSetGainCmdArgs
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
    RawSensor *sensor = &AnalogInputsLS::raw[sensorIndex];
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
    struct arg_int *active;
    struct arg_end *end;
} _rawSetBiasCmdArgs;

static int _rawSetBiasCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_rawSetBiasCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _rawSetBiasCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _rawSetBiasCmdArgs.sensorIndex->ival[0];
    int active = _rawSetBiasCmdArgs.active->ival[0];
    RawSensor *sensor = &AnalogInputsLS::raw[sensorIndex];
    // set
    sensor->setBiasActive(active);
    return 0;
}

static int _registerRawSetBiasCmd(void)
{
    _rawSetBiasCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _rawSetBiasCmdArgs.active = arg_int1(NULL, NULL, "<active>", "Bias active status");
    _rawSetBiasCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "raw-set-bias",
        .help = "Command for setting the bias activation",
        .hint = NULL,
        .func = &_rawSetBiasCmdHandler,
        .argtable = &_rawSetBiasCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_int *sensorIndex;
    struct arg_int *excitation;
    struct arg_end *end;
} _rawSetExcitationCmdArgs;

static int _rawSetExcitationCmdHandler(int argc, char **argv)
{
    // check if argument are missing
    int err = arg_parse(argc, argv, (void **)&_rawSetExcitationCmdArgs);
    if (err != 0) {
        arg_print_errors(stderr, _rawSetExcitationCmdArgs.end, argv[0]);
        return -1;
    }

    // get arguments
    int sensorIndex = _rawSetExcitationCmdArgs.sensorIndex->ival[0];
    int excitation = _rawSetExcitationCmdArgs.excitation->ival[0];
    RawSensor *sensor = &AnalogInputsLS::raw[sensorIndex];
    // set
    sensor->setExcitation((Sensor_Excitation_e) excitation);
    return 0;
}

static int _registerRawSetExcitationCmd(void)
{
    _rawSetExcitationCmdArgs.sensorIndex = arg_int1(NULL, NULL, "<sensorIndex>", "Sensor index");
    _rawSetExcitationCmdArgs.excitation = arg_int1(NULL, NULL, "<excitation>", "Excitation level");
    _rawSetExcitationCmdArgs.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "raw-set-excitation",
        .help = "Command for setting the excitation",
        .hint = NULL,
        .func = &_rawSetExcitationCmdHandler,
        .argtable = &_rawSetExcitationCmdArgs
    };
    return esp_console_cmd_register(&cmd);
}

// Register all CLI commands
int CLI::_registerAnalogInputsLSCmd(void)
{
    int ret = 0;
    ret |= _registerMuxRoute();
    ret |= _registerRawSetRefCmd();
    ret |= _registerRawSetGainCmd();
    ret |= _registerRawSetBiasCmd();
    ret |= _registerRawSetExcitationCmd();
    ret |= _registerRawReadCmd();
    ret |= _registerRtdReadCmd();
    ret |= _registerTcReadVCmd();
    ret |= _registerTcReadCCmd();
    ret |= _registerAddSensorCmd();
    return ret;
}

#endif
