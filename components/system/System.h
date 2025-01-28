/**
 * @file System.h
 * @brief System main
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

class System
{
public:
    static int init(void);
    static void start(void);

private:
    static void _mainTask(void *pvParameters);
};