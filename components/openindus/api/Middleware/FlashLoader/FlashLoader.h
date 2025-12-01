/**
 * @file FlashLoader.hpp
 * @brief Header file for the FlashLoader component.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <cstdint>
#include <cstddef>

class FlashLoader
{
public:

    static void begin(void);
    static void write(uint8_t* data, size_t length);
    static void check(uint8_t md5Sum[16], size_t progSize);
    static void end(void);
};
