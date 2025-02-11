/**
 * @file DigitalOutputsExp.h
 * @brief Digital output expander
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "DigitalOutputs.h"
#include "pcal6524.h"

class DigitalOutputsExp
{
protected:
    static int init(ioex_device_t **ioex, const ioex_num_t *ioex_num,
                    const ioex_num_t *ioex_current, int num);

public:
    /**
     * @brief Set an output at high or low level.
     *
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    static void digitalWrite(DOut_Num_t num, bool level);

    template <typename T1, typename T2> 
    static inline void digitalWrite(T1 num, T2 level) {
        digitalWrite((DOut_Num_t)num, (bool)level);
    }

    /**
     * @brief Toggle a digital output
     *
     * @param num DOUT to toggle
     **/
    static void toggleOutput(DOut_Num_t num);

    /**
     * @brief Get the overcurrent status of a digital output
     *
     * @param num DOUT to get
     * @return 1 if overcurrent, 0 if not
     **/
    static int outputIsOvercurrent(DOut_Num_t num);

private:
    static uint8_t _nb;

    static ioex_num_t *_ioex_num;
    static ioex_num_t *_ioex_current;
    static ioex_device_t **_ioex;
    static uint8_t *_level;
    static SemaphoreHandle_t _mutex;

    static void _controlTask(void *pvParameters);

    static int _registerCLI(void);
};