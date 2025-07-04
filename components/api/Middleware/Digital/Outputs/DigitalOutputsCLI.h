/**
 * @file DigitalOutputsCLI.h
 * @brief Digital Outputs CLI class
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <cstring>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "DigitalOutputs.h"
#include "DigitalOutputsInterface.h"
#if defined(MODULE_MASTER)
#include "DigitalOutputsCmd.h"
#include "ModuleControl.h"
#endif

class DigitalOutputsCLI
{
public:
    /**
     * @brief Initialize and register all CLI commands
     * 
     * @return int 0 on success, error code on failure
     */
    static int init(void);

protected:

    // Command argument structures
    static struct {
        struct arg_int *dout;
        struct arg_int *level;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } digitalWriteArgs;

    static struct {
        struct arg_int *dout;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } toggleOutputArgs;

    static struct {
        struct arg_int *dout;
        struct arg_str *mode;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } outputModeArgs;

    static struct {
        struct arg_int *dout;
        struct arg_int *freq;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } setPWMFrequencyArgs;

    static struct {
        struct arg_int *dout;
        struct arg_dbl *duty;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } setPWMDutyCycleArgs;

    static struct {
        struct arg_int *dout;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } getOutputCurrentArgs;

    static struct {
        struct arg_int *dout;
#if defined(MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } outputIsOvercurrentArgs;

    // Command functions
    static int digitalWriteFunc(int argc, char **argv);
    static int toggleOutputFunc(int argc, char **argv);
    static int outputModeFunc(int argc, char **argv);
    static int setPWMFrequencyFunc(int argc, char **argv);
    static int setPWMDutyCycleFunc(int argc, char **argv);
    static int getOutputCurrentFunc(int argc, char **argv);
    static int outputIsOvercurrentFunc(int argc, char **argv);

    // Static instance
    static DigitalOutputsInterface* _digitalOutputsInstance;
};
