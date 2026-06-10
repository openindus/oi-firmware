/**
 * @file MotorDcPidCtrl.cpp
 * @brief MotorDcPidCtrl class implementation with PID position control for DC motors
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcPidCtrl.h"

#include <cmath>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char* TAG = "MotorDcPidCtrl";

#define PID_TASK_PERIOD_MS  10      /*!< Control loop period in ms (100 Hz) */
#define PID_DEAD_BAND       1.0f    /*!< Duty cycle threshold below which the motor brakes (%) */

/* Static member definitions */
std::vector<Encoder*>                   MotorDcPidCtrl::_motorEncoders;
std::vector<pid_ctrl_block_handle_f_t>  MotorDcPidCtrl::_pidHandles;
std::vector<float>                      MotorDcPidCtrl::_targetPositions;
std::vector<bool>                       MotorDcPidCtrl::_controlActive;
TaskHandle_t                            MotorDcPidCtrl::_pidTask = nullptr;

int MotorDcPidCtrl::init(std::vector<MotorDC_PinConfig_t> motorsConfig,
    gpio_num_t faultPin, const pid_ctrl_config_f_t *pidConfig)
{
    int err = MotorDc::init(motorsConfig, faultPin);
    if (err != 0) {
        ESP_LOGE(TAG, "MotorDc init failed (%d)", err);
        return err;
    }

    size_t count = motorsConfig.size();
    _motorEncoders.resize(count, nullptr);
    _targetPositions.resize(count, 0.0f);
    _controlActive.resize(count, false);
    _pidHandles.resize(count, nullptr);

    pid_ctrl_config_f_t defaultConfig = {
        .init_param = {
            .kp          = 1.0f,
            .ki          = 0.0f,
            .kd          = 0.0f,
            .max_output  =  100.0f,
            .min_output  = -100.0f,
            .max_integral =  0.0f,
            .min_integral =  0.0f,
            .cal_type    = PID_CAL_TYPE_POSITIONAL,
        }
    };

    const pid_ctrl_config_f_t *cfg = (pidConfig != nullptr) ? pidConfig : &defaultConfig;

    for (size_t i = 0; i < count; i++) {
        esp_err_t ret = pid_new_control_block_f(cfg, &_pidHandles[i]);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PID block for motor %d: %s",
                     (int)i+1, esp_err_to_name(ret));
            err = -1;
        }
    }

    xTaskCreate(_pidControlTask, "pid_ctrl", 4096, nullptr, 5, &_pidTask);

    return err;
}

void MotorDcPidCtrl::_pidControlTask(void* arg)
{
    while (1) {
        for (size_t i = 0; i < _motorEncoders.size(); i++) {
            if (!_controlActive[i] || _motorEncoders[i] == nullptr || _pidHandles[i] == nullptr) {
                continue;
            }

            float currentPos = (float)_motorEncoders[i]->getPulses();
            float error      = _targetPositions[i] - currentPos;

            // printf(">Target:%.1f\n>Current:%.1f\n>Error:%.1f\n",
            //        _targetPositions[i], currentPos, error);

            float output = 0.0f;
            pid_compute_f(_pidHandles[i], error, &output);

            // printf(">PID Output: %.1f\n", output);

            if (fabsf(output) < PID_DEAD_BAND) {
                MotorDc::brake((MotorNum_t)i);
            } else if (output > 0.0f) {
                MotorDc::run((MotorNum_t)i, FORWARD, output);
            } else {
                MotorDc::run((MotorNum_t)i, REVERSE, -output);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(PID_TASK_PERIOD_MS));
    }
}

void MotorDcPidCtrl::attachEncoder(MotorNum_t motor, Encoder* encoder)
{
    if ((size_t)motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", (int)motor+1);
        return;
    }

    _motorEncoders[motor] = encoder;
    ESP_LOGI(TAG, "Encoder attached to motor %d", (int)motor+1);
}

void MotorDcPidCtrl::detachEncoder(MotorNum_t motor)
{
    if ((size_t)motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", (int)motor+1);
        return;
    }

    _controlActive[motor] = false;
    _motorEncoders[motor] = nullptr;
    ESP_LOGI(TAG, "Encoder detached from motor %d", (int)motor+1);
}

void MotorDcPidCtrl::moveTo(MotorNum_t motor, float position)
{
    if ((size_t)motor >= _targetPositions.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", (int)motor+1);
        return;
    }
    if (_motorEncoders[motor] == nullptr) {
        ESP_LOGE(TAG, "No encoder attached to motor %d", (int)motor+1);
        return;
    }

    pid_reset_ctrl_block_f(_pidHandles[motor]);
    _targetPositions[motor] = position;
    _controlActive[motor]   = true;

    ESP_LOGI(TAG, "Motor %d: moving to %.1f pulses", (int)motor+1, position);
}

void MotorDcPidCtrl::stop(MotorNum_t motor)
{
    if ((size_t)motor >= _controlActive.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", (int)motor+1);
        return;
    }

    _controlActive[motor] = false;
    MotorDc::brake(motor);

    ESP_LOGI(TAG, "Motor %d: position control stopped", (int)motor+1);
}

float MotorDcPidCtrl::getPosition(MotorNum_t motor)
{
    if ((size_t)motor >= _motorEncoders.size() || _motorEncoders[motor] == nullptr) {
        ESP_LOGE(TAG, "No encoder attached to motor %d", (int)motor+1);
        return 0.0f;
    }

    return (float)_motorEncoders[motor]->getPulses();
}

void MotorDcPidCtrl::setPidParams(MotorNum_t motor, const pid_ctrl_parameter_f_t* params)
{
    if ((size_t)motor >= _pidHandles.size() || _pidHandles[motor] == nullptr) {
        ESP_LOGE(TAG, "PID not initialized for motor %d", (int)motor+1);
        return;
    }

    esp_err_t ret = pid_update_parameters_f(_pidHandles[motor], params);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update PID params for motor %d: %s",
                 (int)motor+1, esp_err_to_name(ret));
    }
}

struct HomingCtx_t {
    MotorNum_t        motor;
    Encoder*          encoder;
    SemaphoreHandle_t sem;
};

void MotorDcPidCtrl::homing(HomingType_t type, DIn_Num_t dinNum, MotorNum_t motor,
    float dutyCycle, bool invertLogic, uint32_t timeoutMs)
{
    if ((size_t)motor >= _motorEncoders.size() || _motorEncoders[motor] == nullptr) {
        ESP_LOGE(TAG, "No encoder attached to motor %d — call attachEncoder() first", (int)motor+1);
        return;
    }

    SemaphoreHandle_t homingDone = xSemaphoreCreateBinary();
    if (homingDone == nullptr) {
        ESP_LOGE(TAG, "Failed to create homing semaphore");
        return;
    }

    HomingCtx_t* ctx = new HomingCtx_t{motor, _motorEncoders[motor], homingDone};

    DigitalInputs* din = new DigitalInputs();
    din->attachInterrupt(dinNum, [](void* arg) {
        HomingCtx_t* c = static_cast<HomingCtx_t*>(arg);
        MotorDc::brake(c->motor);
        c->encoder->reset();
        xSemaphoreGive(c->sem);
    }, CHANGE_MODE, ctx);

    int sensorState = din->digitalRead(dinNum);
    MotorDirection_t dir = (!invertLogic) ?
        ((sensorState == 1) ? FORWARD : REVERSE) :
        ((sensorState == 1) ? REVERSE : FORWARD);

    MotorDc::run(motor, dir, dutyCycle);

    ESP_LOGI(TAG, "Homing motor %d on DIN %d (invertLogic=%d)",
             (int)motor+1, (int)dinNum+1, (int)invertLogic);

    if (xSemaphoreTake(homingDone, pdMS_TO_TICKS(timeoutMs)) != pdPASS) {
        ESP_LOGW(TAG, "Homing timeout for motor %d", (int)motor+1);
        MotorDc::brake(motor);
    } else {
        ESP_LOGI(TAG, "Homing complete for motor %d", (int)motor+1);
    }

    din->detachInterrupt(dinNum);
    vSemaphoreDelete(homingDone);
    delete ctx;
}

