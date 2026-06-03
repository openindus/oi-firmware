/**
 * @file MotorDcPidCtrl.h
 * @brief MotorDcPidCtrl class definition with PID position control for DC motors
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "MotorDc.h"
#include "pid_ctrl.h"
#include "Encoder.h"

/**
 * @class MotorDcPidCtrl
 * @brief DC Motor control class with PID position control
 */
class MotorDcPidCtrl : public MotorDc
{
protected:

    static int init(std::vector<MotorDC_PinConfig_t> motorsConfig, 
        gpio_num_t faultPin, const pid_ctrl_config_f_t *pidConfig = nullptr);

public:

    static void attachEncoder(MotorNum_t motor, Encoder* encoder);
    static void detachEncoder(MotorNum_t motor);

private:

};
