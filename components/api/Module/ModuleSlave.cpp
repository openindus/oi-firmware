/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleSlave.cpp
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ModuleSlave.h"
#include "ModulePinout.h"

static const char MODULE_TAG[] = "Module";

uint16_t ModuleSlave::_id;
std::map<uint8_t, std::function<void(std::vector<uint8_t>&)>> ModuleSlave::_ctrlCallbacks;

void ModuleSlave::init(void)
{
    ESP_LOGI(MODULE_TAG, "Bus init");
    /* Bus RS/CAN */
    BusRS::begin(MODULE_RS_NUM_PORT, MODULE_PIN_RS_UART_TX, MODULE_PIN_RS_UART_RX);
    BusCAN::begin(MODULE_PIN_CAN_TX, MODULE_PIN_CAN_RX);

    /* Bus IO */
    BusIO::Config_t config = {
        .adcChannelId = MODULE_OI_ID_ADC_CHANNEL,
        .adcWidthId = MODULE_OI_ID_ADC_WIDTH,
        .gpioNumSync = MODULE_PIN_OI_GPIO,
        .gpioModeSync = GPIO_MODE_INPUT,
        .gpioNumPower = MODULE_PIN_CMD_MOSFET_ALIM,
    };
    BusIO::init(&config);

    /* Board ID is represented by the 10 most significants bits of the adc reading (12 bits) */
    _id = (uint16_t) (BusIO::readId()>>2);
    ESP_LOGI(MODULE_TAG, "Bus Id: %d", _id);

    /* Bus task */
    ESP_LOGI(MODULE_TAG, "Create bus task");
    xTaskCreate(_busTask, "Bus task", 4096, NULL, 1, NULL);
}

/**
 * @brief Send an event on the CAN bus
 * 
 * @param msgBytes 
 */
void ModuleSlave::sendEvent(std::vector<uint8_t> msgBytes)
{
    BusCAN::Frame_t frame;
    frame.cmd = CMD_EVENT;
    std::copy(msgBytes.begin(), msgBytes.end(), frame.data);
    uint8_t length = msgBytes.size();
    BusCAN::write(&frame, _id, length);
}

void ModuleSlave::_busTask(void *pvParameters) 
{
    BusRS::Frame_t frame;
    frame.length = 1024;
    frame.data = (uint8_t*)malloc(frame.length);
    while (1) {
        if (BusRS::read(&frame, portMAX_DELAY) < 0) {
            ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        } else {
            switch (frame.cmd)
            {
            case CMD_RESTART:
            {
                ModuleStandalone::restart();
                break;
            }
            case CMD_PING:
            {
                int num; // Serial number
                memcpy(&num, frame.data, 4);
                if (num ==  ModuleStandalone::getSerialNum()) {
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
                int sn = ModuleStandalone::getSerialNum();
                memcpy(frame.data, &sn, sizeof(int));
                if (BusCAN::write(&discoverFrame, _id) == -1)
                    ModuleStandalone::ledBlink(LED_RED, 1000); // Error
                break;
            }
            case CMD_GET_BOARD_INFO:
            {
                if (frame.id == _id) {
                    Module_Info_t board_info;
                    ModuleStandalone::getBoardType(board_info.efuse.board_type);
                    board_info.efuse.serial_number = ModuleStandalone::getSerialNum();
                    ModuleStandalone::getHardwareVersion(board_info.efuse.hardware_version);
                    ModuleStandalone::getSoftwareVersion(board_info.software_version);
                    memcpy(frame.data, &board_info, sizeof(Module_Info_t));
                    frame.length = sizeof(Module_Info_t);
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
                    ModuleStandalone::ledBlink(LED_WHITE, 1000); // Programming mode
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
            case CMD_CONTROL:
            {
                std::vector<uint8_t> msg;
                msg.assign(frame.data, frame.data + frame.length);

                if (frame.id == _id) {

                    if (_ctrlCallbacks.find(frame.data[0]) != _ctrlCallbacks.end()) {
                        for (auto it=_ctrlCallbacks.begin(); it!=_ctrlCallbacks.end(); it++) {
                            if (it->first == frame.data[0]) {
                                (*it).second(msg);
                                break;
                            }
                        }
                    } else {
                        ESP_LOGW(MODULE_TAG, "CTRL Request does not exist: 0x%02x", frame.data[0]);
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
                        ModuleStandalone::ledOn(color);
                    } else if (state == LED_OFF) {
                        ModuleStandalone::ledOff();
                    } else if (state == LED_BLINK) {
                        ModuleStandalone::ledBlink(color, period);
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

void ModuleSlave::_heartbeatTask(void *pvParameters)
{
    /** @todo Send heartbeat periodically */
}
