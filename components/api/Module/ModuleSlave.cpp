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
std::map<Request_t, RequestCallback_t> ModuleSlave::_request;

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

    _id = BusIO::readId();
    ESP_LOGI(MODULE_TAG, "Bus Id: %d", _id);

    /* Bus task */
    ESP_LOGI(MODULE_TAG, "Create bus task");
    xTaskCreate(_busTask, "Bus task", 4096, NULL, 1, NULL);
}

void ModuleSlave::event(Event_t event, int num)
{
    BusCan::Frame_t frame;
    frame.command = MODULE_EVENT;
    frame.type = event;
    frame.data = num;
    BusCan::write(&frame, _id);
}

void ModuleSlave::onRequest(Request_t request, RequestCallback_t callback)
{
    _request.insert({request, callback});
}

uint32_t ModuleSlave::handleRequest(RequestMsg_t msg)
{
    if (_request.find((Request_t)msg.request) != _request.end()) {
        for (auto it=_request.begin(); it!=_request.end(); it++) {
            if (it->first == msg.request) {
                return (*it).second(msg);
            }
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Request does not exist: request=0x%02x", msg.request);
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
            MODULE_ERROR();
        } else {
            switch (frame.command)
            {
            case MODULE_RESTART:
            {
                ModuleStandalone::restart();
                break;
            }
            case MODULE_PING:
            {
                uint32_t num; // Serial number
                memcpy(&num, frame.data, 4);
                if (num ==  ModuleStandalone::getSerialNum()) {
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    BusRs::write(&frame);
                }
                break;
            }
            case MODULE_AUTO_ID:
            {
                BusCan::Frame_t autoIdFrame;
                autoIdFrame.command = MODULE_AUTO_ID;
                if (BusCan::write(&autoIdFrame, _id) == -1)
                    MODULE_ERROR();
                break;
            }            
            case MODULE_FLASH_LOADER_BEGIN:
            {
                if (frame.identifier == _id) {
                    MODULE_PROGRAMMING();
                    FlashLoader::begin();
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.length = 0;
                    BusRs::write(&frame);
                }
                break;
            }
            case MODULE_FLASH_LOADER_WRITE:
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
            case MODULE_FLASH_LOADER_CHECK:
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
            case MODULE_FLASH_LOADER_END:
            {
                if (frame.identifier == _id) {
                    FlashLoader::end();
                }
                break;
            }
            case MODULE_READ_REGISTER:
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
            case MODULE_SET_STATUS:
            {
                /** @todo: set status */
                MODULE_PAIRED();
                break;
            }
            case MODULE_REQUEST:
            {
                RequestMsg_t msg;
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
            case MODULE_BUS_ID:
            {
                uint16_t id = _id;
                uint32_t num; // Serial number
                memcpy(&num, frame.data, sizeof(num));
                if (num ==  ModuleStandalone::getSerialNum()) {
                    frame.broadcast = false;
                    frame.direction = 0;
                    frame.ack = false;
                    frame.length = sizeof(id);
                    memcpy(frame.data, &id, sizeof(id));
                    BusRs::write(&frame);
                }
                break;
            }
            case MODULE_LED_STATE:
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
