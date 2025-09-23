/**
 * @file RSCLI.h
 * @brief RS485/232 command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <cstring>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "RS.h"

class RSCLI
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
        struct arg_int *config;
        struct arg_str *mode;
        struct arg_end *end;
    } rsBeginArgs;

    static struct {
        struct arg_int *size;
        struct arg_end *end;
    } rsReadBufferArgs;

    static struct {
        struct arg_int *byte;
        struct arg_end *end;
    } rsWriteByteArgs;

    static struct {
        struct arg_str *data;
        struct arg_end *end;
    } rsWriteBufferArgs;

    // Command functions
    static int rsBeginFunc(int argc, char **argv);
    static int rsEndFunc(int argc, char **argv);
    static int rsAvailableFunc(int argc, char **argv);
    static int rsAvailableForWriteFunc(int argc, char **argv);
    static int rsReadFunc(int argc, char **argv);
    static int rsReadBufferFunc(int argc, char **argv);
    static int rsWriteByteFunc(int argc, char **argv);
    static int rsWriteBufferFunc(int argc, char **argv);

private:
    static RS* _rsInstance;
};
