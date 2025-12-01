/**
 * @file DigitalInputsCLI.h
 * @brief Digital Inputs command line interface
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <cstring>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "DigitalInputs.h"
#if defined(CONFIG_MODULE_MASTER)
#include "DigitalInputsCmd.h"
#include "ModuleControl.h"
#endif

class DigitalInputsCLI
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
        struct arg_int *din;
#if defined(CONFIG_MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } digitalReadArgs;

    static struct {
        struct arg_int *din;
        struct arg_str *mode;
#if defined(CONFIG_MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } attachInterruptArgs;

    static struct {
        struct arg_int *din;
#if defined(CONFIG_MODULE_MASTER)
        struct arg_int *id;
#endif
        struct arg_end *end;
    } detachInterruptArgs;

    // Command functions
    static int digitalReadFunc(int argc, char **argv);
    static int attachInterruptFunc(int argc, char **argv);
    static int detachInterruptFunc(int argc, char **argv);

private:
    static DigitalInputsInterface* _digitalInputsInstance;
};
