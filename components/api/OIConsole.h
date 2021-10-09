/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIConsole.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "OISystem.h"

class OIConsole
{
public:

    /**
     * @brief Start the OpenIndus console
     * 
     */
    void start(void);

    /**
     * @brief Stop the console
     * 
     */
    void stop(void);

private:

    /**
     * @brief restart the cpu
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _restart(int argc, char **argv);

    /**
     * @brief add restart command
     * 
     */
    static void _registerRestart(void);

    /**
     * @brief set board id
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _setId(int argc, char **argv);

    /**
     * @brief add set-id command
     * 
     */
    static void _registerSetId(void);

    /**
     * @brief get board id
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _getId(int argc, char **argv);

    /**
     * @brief add get-id command
     * 
     */
    static void _registerGetId(void);

    /**
     * @brief set log level
     *
     * @param argc
     * @param argv
     * @return int
     */
    static int _logLevel(int argc, char **argv);

    /**
     * @brief add log level command
     * 
     */
    static void _registerLogLevel(void);

    /**
     * @brief ping commnad
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _ping(int argc, char **argv);

    /**
     * @brief add ping command
     * 
     */
    static void _registerPing(void);

    /**
     * @brief download mode
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _downloadMode(int argc, char **argv);

    /**
     * @brief add download mode command
     * 
     */
    static void _registerDownloadMode(void);

    /**
     * @brief listen command
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _listen(int argc, char **argv);

    /**
     * @brief add listen command
     * 
     */
    static void _registerListen(void);

    /**
     * @brief get all type command
     * 
     * @param argc 
     * @param argv 
     * @return int 
     */
    static int _getAllTypes(int argc, char **argv);

    /**
     * @brief add get all types command
     * 
     */
    static void _registerGetAllTypes(void);

    /**
     * @brief command a module
     *
     * @param argc
     * @param argv
     * @return int
     */
    static int _cmd(int argc, char **argv);

    /**
     * @brief add command
     * 
     */
    static void _registerCmd(void);
};

extern OIConsole Console;