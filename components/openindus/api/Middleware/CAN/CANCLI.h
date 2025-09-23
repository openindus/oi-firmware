/**
 * @file CANCLI.h
 * @brief CAN bus command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <cstring>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "CAN.h"

class CANCLI
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
        struct arg_int *baudrate;
        struct arg_lit *extended;
        struct arg_end *end;
    } canBeginArgs;

    static struct {
        struct arg_int *id;
        struct arg_str *data;
        struct arg_lit *extended;
        struct arg_lit *rtr;
        struct arg_end *end;
    } canWriteArgs;

    static struct {
        struct arg_int *mask;
        struct arg_int *filter;
        struct arg_end *end;
    } canSetStandardFilterArgs;

    static struct {
        struct arg_int *mask;
        struct arg_int *filter;
        struct arg_end *end;
    } canSetExtendedFilterArgs;

    // Command functions
    static int canBeginFunc(int argc, char **argv);
    static int canEndFunc(int argc, char **argv);
    static int canAvailableFunc(int argc, char **argv);
    static int canWriteFunc(int argc, char **argv);
    static int canReadFunc(int argc, char **argv);
    static int canSetStandardFilterFunc(int argc, char **argv);
    static int canSetExtendedFilterFunc(int argc, char **argv);

private:
    static CAN* _canInstance;
};
