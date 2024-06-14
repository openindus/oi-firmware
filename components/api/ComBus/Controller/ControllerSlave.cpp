/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControllerSlave.cpp
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ControllerSlave.h"

#if defined(MODULE_SLAVE)

static const char TAG[] = "ControllerSlave";

uint16_t ControllerSlave::_id;
Controller_State_t ControllerSlave::_state = STATE_IDLE;
TaskHandle_t ControllerSlave::_taskHandle = NULL;
std::map<uint8_t, std::function<void(std::vector<uint8_t>&)>> ControllerSlave::_ctrlCallbacks;

int ControllerSlave::init(void)
{
    int err = 0;

    /* Board ID is represented by the 10 most significants bits of the adc reading (12 bits) */
    _id = (uint16_t) (BusIO::readId()>>2);
    ESP_LOGI(TAG, "Bus Id: %d", _id);

    /* Bus task */
    ESP_LOGI(TAG, "Create bus task");
    xTaskCreate(_busTask, "Bus task", 4096, NULL, 1, &_taskHandle);

    _state = STATE_RUNNING;

    return err;
}

void ControllerSlave::start(void)
{
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle);
    }
    _state = STATE_RUNNING;
}

void ControllerSlave::stop(void)
{
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
    }
    _state = STATE_IDLE;
}

int ControllerSlave::getStatus(void)
{
    return (int)_state;
}

/**
 * @brief Send an event on the CAN bus
 * 
 * @param msgBytes 
 */
void ControllerSlave::sendEvent(std::vector<uint8_t> msgBytes)
{
    BusCAN::Frame_t frame;
    frame.cmd = CMD_EVENT;
    std::copy(msgBytes.begin(), msgBytes.end(), frame.args);
    uint8_t size = msgBytes.size() + 1;
    BusCAN::write(&frame, _id, size);
}

void ControllerSlave::_busTask(void *pvParameters) 
{
    BusRS::Frame_t frame;
    frame.length = 1024;
    frame.data = (uint8_t*)malloc(frame.length);
    while (1) {
        if (BusRS::read(&frame, portMAX_DELAY) < 0) {
            Module::ledBlink(LED_RED, 1000); // Error
        } else {
            switch (frame.cmd)
            {
            case CMD_RESTART:
            {
                Module::restart();
                break;
            }
            case CMD_PING:
            {
                uint16_t type; // Board type
                uint32_t num; // Serial number
                memcpy(&type, frame.data, sizeof(uint16_t));
                memcpy(&num, &frame.data[2], sizeof(uint32_t));
                if (num ==  Module::getSerialNum() && type == Module::getBoardType()) {
                    frame.dir = 0;
                    frame.ack = false;
                    frame.id = _id; // return our id --> the master can get the id from serialNumber
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_DISCOVER:
            {
                BusCAN::Frame_t discoverFrame;
                discoverFrame.cmd = CMD_DISCOVER;
                uint16_t type = Module::getBoardType();
                uint32_t sn = Module::getSerialNum();
                memcpy(discoverFrame.args, &type, sizeof(uint16_t));
                memcpy(&discoverFrame.args[2], &sn, sizeof(uint32_t));
                if (BusCAN::write(&discoverFrame, _id, sizeof(uint16_t)+sizeof(uint32_t)+1) == -1)
                    Module::ledBlink(LED_RED, 1000); // Error
                break;
            }
            case CMD_GET_BOARD_INFO:
            {
                if (frame.id == _id) {
                    Board_Info_t board_info;
                    board_info.efuse.board_type = Module::getBoardType();
                    board_info.efuse.serial_number = Module::getSerialNum();
                    Module::getHardwareVersion(board_info.efuse.hardware_version);
                    Module::getSoftwareVersion(board_info.software_version);
                    memcpy(frame.data, &board_info, sizeof(Board_Info_t));
                    frame.length = sizeof(Board_Info_t);
                    frame.dir = 0;
                    frame.ack = false;
                    frame.id = _id;
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_BEGIN:
            {
                if (frame.id == _id) {
                    Module::ledBlink(LED_WHITE, 1000); // Programming mode
                    FlashLoader::begin();
                    frame.dir = 0;
                    frame.ack = false;
                    frame.length = 0;
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_WRITE:
            {
                if (frame.id == _id) {
                    FlashLoader::write(frame.data, frame.length);
                    frame.dir = 0;
                    frame.ack = false;
                    frame.length = 0;
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_CHECK:
            {
                if (frame.id == _id) {
                    uint8_t md5Sum[16];
                    size_t progSize;
                    memcpy(&progSize, frame.data, sizeof(progSize));
                    FlashLoader::check(md5Sum, progSize);
                    frame.dir = 0;
                    frame.ack = false;
                    frame.length = 16;
                    memcpy(frame.data, md5Sum, 16);
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_END:
            {
                if (frame.id == _id) {
                    FlashLoader::end();
                }
                break;
            }
            case CMD_READ_REGISTER:
            {
                if (frame.id == _id) {
                    uint32_t addr;
                    memcpy(&addr, frame.data, sizeof(addr));
                    volatile uint32_t* reg = (volatile  uint32_t*)addr;
                    uint32_t value = *reg;
                    frame.dir = 0;
                    frame.ack = false;
                    frame.length = sizeof(value);
                    memcpy(frame.data, &value, sizeof(value));
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_REQUEST:
            {
                if (frame.id == _id) {

                    std::vector<uint8_t> msg;
                    msg.assign(frame.data, frame.data + frame.length);
                    auto it = _ctrlCallbacks.find(frame.data[0]);
                    if (it != _ctrlCallbacks.end()) {
                        (*it).second(msg);
                    } else {
                        frame.error = 1;
                        ESP_LOGW(TAG, "CTRL Request does not exist: 0x%02x", frame.data[0]);
                    }

                    if (frame.ack == true) {
                        frame.dir = 0;
                        frame.ack = false;
                        frame.length = msg.size();
                        memcpy(frame.data, msg.data(), msg.size());
                        BusRS::write(&frame);
                    }
                }

                break;
            }
            case CMD_LED_STATUS:
            {
                LedState_t state;
                LedColor_t color;
                uint32_t period;
                if (frame.id == _id) {
                    state = (LedState_t)frame.data[0];
                    color = (LedColor_t)frame.data[1];
                    memcpy(&period, &frame.data[2], sizeof(uint32_t));
                    if (state == LED_ON) {
                        Module::ledOn(color);
                    } else if (state == LED_OFF) {
                        Module::ledOff();
                    } else if (state == LED_BLINK) {
                        Module::ledBlink(color, period);
                    } else {
                        // State error
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }
    free(frame.data);
    frame.data = NULL;
}

void ControllerSlave::_heartbeatTask(void *pvParameters)
{
    /** @todo Send heartbeat periodically */
}

#endif