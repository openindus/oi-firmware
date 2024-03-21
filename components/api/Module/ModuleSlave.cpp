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
std::map<ModuleCmd_RequestId_t, ModuleCmd_RequestCallback_t> ModuleSlave::_callback;

void ModuleSlave::init(void)
{
    ESP_LOGI(MODULE_TAG, "Bus init");
    /* Bus RS/CAN */
    BusRs::begin(MODULE_RS_NUM_PORT, MODULE_PIN_RS_UART_TX, MODULE_PIN_RS_UART_RX);
    BusCan::begin(MODULE_PIN_CAN_TX, MODULE_PIN_CAN_RX);

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

void ModuleSlave::event(ModuleCmd_EventId_t eventId, int num)
{
    BusCan::Frame_t frame;
    frame.command = CMD_EVENT;
    frame.data_byte[0] = (uint8_t)eventId;
    frame.data_byte[1] = (uint8_t)num;
    BusCan::write(&frame, _id);
}

void ModuleSlave::onRequest(ModuleCmd_RequestId_t requestId, ModuleCmd_RequestCallback_t callback)
{
    _callback.insert({requestId, callback});
}

uint32_t ModuleSlave::handleRequest(ModuleCmd_RequestMsg_t msg)
{
    if (_callback.find((ModuleCmd_RequestId_t)msg.id) != _callback.end()) {
        for (auto it=_callback.begin(); it!=_callback.end(); it++) {
            if (it->first == msg.id) {
                return (*it).second(msg);
            }
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Request does not exist: request=0x%02x", msg.id);
    }
    return 0;
}

void ModuleSlave::_busTask(void *pvParameters) 
{
    BusRs::Frame_t frame;
    frame.length = 1024;
    frame.data = (uint8_t*)malloc(frame.length);
    while (1) {
        if (BusRs::read(&frame, portMAX_DELAY) < 0) {
            ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        } else {
            switch (frame.command)
            {
            case CMD_RESTART:
            {
                ModuleStandalone::restart();
                break;
            }
            case CMD_PING:
            {
                uint32_t num; // Serial number
                memcpy(&num, frame.data, 4);
                if (num ==  ModuleStandalone::getSerialNum()) {
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.identifier = _id; // return our id --> the master can get the id from serialNumber
                    BusRs::write(&frame);
                }
                break;
            }
            case CMD_DISCOVER:
            {
                BusCan::Frame_t discoverFrame;
                discoverFrame.command = CMD_DISCOVER;
                discoverFrame.data = ModuleStandalone::getSerialNum();
                if (BusCan::write(&discoverFrame, _id) == -1)
                    ModuleStandalone::ledBlink(LED_RED, 1000); // Error
                break;
            }
            case CMD_GET_BOARD_INFO:
            {
                if (frame.identifier == _id) {
                    Module_Info_t board_info;
                    char software_version[32]; // version can be 32 bytes
                    ModuleStandalone::getBoardType(board_info.efuse.board_type);
                    board_info.efuse.serial_number = ModuleStandalone::getSerialNum();
                    ModuleStandalone::getHardwareVersion(board_info.efuse.hardware_version);
                    ModuleStandalone::getSoftwareVersion(board_info.software_version);
                    memcpy(frame.data, &board_info, sizeof(Module_Info_t));
                    frame.length = sizeof(Module_Info_t);
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.identifier = _id;
                    BusRs::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_BEGIN:
            {
                if (frame.identifier == _id) {
                    ModuleStandalone::ledBlink(LED_WHITE, 1000); // Programming mode
                    FlashLoader::begin();
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.length = 0;
                    BusRs::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_WRITE:
            {
                if (frame.identifier == _id) {
                    FlashLoader::write(frame.data, frame.length);
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.length = 0;
                    BusRs::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_CHECK:
            {
                if (frame.identifier == _id) {
                    uint8_t md5Sum[16];
                    size_t progSize;
                    memcpy(&progSize, frame.data, sizeof(progSize));
                    FlashLoader::check(md5Sum, progSize);
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.length = 16;
                    memcpy(frame.data, md5Sum, 16);
                    BusRs::write(&frame);
                }
                break;
            }
            case CMD_FLASH_LOADER_END:
            {
                if (frame.identifier == _id) {
                    FlashLoader::end();
                }
                break;
            }
            case CMD_READ_REGISTER:
            {
                if (frame.identifier == _id) {
                    uint32_t addr;
                    memcpy(&addr, frame.data, sizeof(addr));
                    volatile uint32_t* reg = (volatile  uint32_t*)addr;
                    uint32_t value = *reg;
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.length = sizeof(value);
                    memcpy(frame.data, &value, sizeof(value));
                    BusRs::write(&frame);
                }
                break;
            }
            case CMD_REQUEST:
            {
                ModuleCmd_RequestMsg_t msg;
                memcpy(msg.byte, frame.data, sizeof(msg.byte));
                if (frame.identifier == _id) {
                    msg.data = handleRequest(msg);
                    if (frame.ack == true) {
                        frame.broadcast = false;
                        frame.direction = 0;
                        frame.ack = false;
                        frame.length = sizeof(msg.byte);
                        memcpy(frame.data, msg.byte, frame.length);
                        BusRs::write(&frame);
                    }
                }
                break;
            }
            case CMD_LED_CTRL:
            {
                LedState_t state;
                LedColor_t color;
                uint32_t period;
                if (frame.identifier == _id) {
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
