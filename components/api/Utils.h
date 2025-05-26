/**
 * @file Utils.h
 * @brief Utils functions for the API
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Parse command line arguments and handle errors automatically
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @param args_struct The argument structure containing the parsed arguments and end marker
 * 
 * This macro simplifies the repetitive pattern of calling arg_parse(), checking for errors,
 * printing error messages, and returning on failure. If there are parsing errors, it will
 * print the errors to stderr and return 1 from the calling function.
 * 
 * Usage example:
 *   PARSE_ARGS_OR_RETURN(argc, argv, myArgs);
 */
#define PARSE_ARGS_OR_RETURN(argc, argv, args_struct) \
    do { \
        int nerrors = arg_parse(argc, argv, (void **)&args_struct); \
        if (nerrors != 0) { \
            arg_print_errors(stderr, args_struct.end, argv[0]); \
            return 1; \
        } \
    } while(0)
