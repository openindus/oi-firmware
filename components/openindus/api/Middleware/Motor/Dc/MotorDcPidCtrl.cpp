/**
 * @file MotorDcPidCtrl.cpp
 * @brief MotorDcPidCtrl class implementation with PID position control for DC motors
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcPidCtrl.h"
#include "pid_ctrl.h"
#include "esp_log.h"
#include <vector>
#include <cmath>

static const char* TAG = "MotorDcPidCtrl";

int MotorDcPidCtrl::init(std::vector<MotorDC_PinConfig_t> motorsConfig, 
    gpio_num_t faultPin, const pid_ctrl_config_f_t *pidConfig)
{

}

void MotorDcPidCtrl::attachEncoder(MotorNum_t motor, Encoder* encoder)
{
    if (motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    _motorEncoders[motor] = encoder;
    ESP_LOGI(TAG, "Encoder attached to motor %d", motor);
}

void MotorDcPidCtrl::detachEncoder(MotorNum_t motor)
{
    if (motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    _motorEncoders[motor] = nullptr;
    ESP_LOGI(TAG, "Encoder detached from motor %d", motor);
}
