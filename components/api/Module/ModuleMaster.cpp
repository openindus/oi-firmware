/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleMaster.cpp
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ModuleMaster.h"
#include "ModuleControl.h"
#include "ModulePinout.h"

static const char MODULE_TAG[] = "Module";

std::map<std::pair<Event_t,uint16_t>, EventCallback_t> ModuleMaster::_event;
std::vector<uint16_t> ModuleMaster::_ids;

void ModuleMaster::init(void)
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
        .gpioModeSync = GPIO_MODE_INPUT_OUTPUT,
        .gpioNumPower = MODULE_PIN_CMD_MOSFET_ALIM,
    };
    BusIO::init(&config);
    BusIO::writeSync(0);

    ESP_LOGI(MODULE_TAG, "Create bus task");
    xTaskCreate(_busTask, "Bus task", 4096, NULL, 1, NULL);
}

bool ModuleMaster::autoId(void)
{
    ESP_LOGI(MODULE_TAG, "Auto ID");
    
    _ids.clear();

    BusRs::Frame_t frame;
    frame.command = MODULE_AUTO_ID;
    frame.identifier = 0;
    frame.broadcast = true;
    frame.direction = 1;
    frame.ack = false;
    frame.length = 0;
    BusRs::write(&frame);

    /* Wait */
    vTaskDelay(1000/portTICK_PERIOD_MS);

    if (ModuleControl::_instances.size() == _ids.size()) {
        sort(_ids.begin(), _ids.end(), std::greater<uint16_t>());
        for (int i=0; i<_ids.size(); i++) {
            ModuleControl::setId(ModuleControl::_instances[i], _ids[i]);
        }
        frame.command = MODULE_SET_STATUS;
        BusRs::write(&frame);
        return true;
    } else {
        ESP_LOGE(MODULE_TAG, "Number of instantiated modules: %d",  ModuleControl::_instances.size());
        ESP_LOGE(MODULE_TAG, "Number of IDs received: %d",  _ids.size());
        return false;
    }
}

void autoTest(void)
{
    /** @todo: auto test when starting the module */
}

void ModuleMaster::program(uint32_t num)
{
    UsbConsole::end(); // Do not perform in the task
    xTaskCreate(&_programmingTask, "Module programming task", 4096, (void*)num, 1, NULL);
    MODULE_PROGRAMMING();
}

bool ModuleMaster::ping(uint32_t num) 
{
    BusRs::Frame_t frame;
    frame.command = MODULE_PING;
    frame.identifier = 0;
    frame.broadcast = true;
    frame.direction = 1;
    frame.ack = true;
    frame.length = 4;
    frame.data = (uint8_t*)&num; // Serial number
    return (BusRs::requestFrom(&frame, pdMS_TO_TICKS(10)) == 0);
}

void ModuleMaster::onEvent(Event_t event, uint16_t id, EventCallback_t callback)
{
    _event.insert({
        std::make_pair(event, id),
        callback
    });
}

void ModuleMaster::handleEvent(Event_t event, uint16_t id, int num)
{
    if (_event.find(std::make_pair(event, id)) != _event.end()) {
        for (auto it=_event.begin(); it!=_event.end(); it++) {
            if ((it->first.first == event) && 
                (it->first.second == id)) {
                (*it).second(num);
            }
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Event does no  t exist: event=0x%02x, id=%d", event, id);
    }
}

void ModuleMaster::_busTask(void *pvParameters) 
{
    BusCan::Frame_t frame;
    uint16_t id;
    while (1) {
        if (BusCan::read(&frame, &id, portMAX_DELAY) != -1) { 
            ESP_LOGD(MODULE_TAG, "Bus CAN read to %d | command(%d), type(%d), data(%d)\n", 
                id, frame.command, frame.type, frame.data);
            switch (frame.command)
            {
            case MODULE_EVENT:
                handleEvent((Event_t)frame.type, id, (int)frame.data);
                break;
            case MODULE_AUTO_ID:
                _ids.push_back((uint16_t)id);
                break;
            
            default:
                ESP_LOGW(MODULE_TAG, "Receive undefined command");
                break;
            }
        } else {
            MODULE_ERROR();
        }
    }
}

void ModuleMaster::_programmingTask(void *pvParameters)
{
    uint16_t id = 0;
    uint32_t num = (uint32_t)pvParameters;
    int sequence = 0;
    UsbSerialProtocol::Packet_t packet;
    packet.data = (uint8_t*)malloc(16400);
    BusRs::Frame_t frame;
    frame.data = (uint8_t*)malloc(1024);

    /* Get bus ID */
    frame.command = MODULE_BUS_ID;
    frame.identifier = 0;
    frame.broadcast = true;
    frame.direction = 1;
    frame.ack = true;
    frame.length = 4;
    memcpy(frame.data, &num, 4); // Serial number
    if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(100)) < 0) {
        MODULE_ERROR();
    } else {
        memcpy(&id, frame.data, 2);
        frame.identifier = id;
    }

    /* FlashLoader begin */
    frame.command = MODULE_FLASH_LOADER_BEGIN;
    frame.identifier = id;
    frame.broadcast = false;
    frame.direction = 1;
    frame.ack = true;
    frame.length = 0;
    if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(5000)) < 0) {
        MODULE_ERROR();
    }

    UsbSerialProtocol::begin();

    while (1) {
        if (UsbSerialProtocol::read(&packet) < 0) {
            MODULE_ERROR();
        } else {
            switch (packet.command)
            {
            case UsbSerialProtocol::FLASH_LOADER_DATA:
                sequence = 0;
                memcpy(&packet.size, &packet.data[0], 4); // data size
                for (int i=0; i<(packet.size/1024); i++) {
                    frame.command = MODULE_FLASH_LOADER_WRITE;
                    frame.identifier = id;
                    frame.broadcast = false;
                    frame.direction = 1;
                    frame.ack = true;
                    frame.length = 1024;
                    frame.seq_num = sequence;
                    memcpy(frame.data, &packet.data[sequence*1024+16], 1024); // data
                    if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(100)) < 0) {
                        MODULE_ERROR();
                    }
                    sequence++;
                }
                if ((packet.size%1024) > 0) {
                    frame.command = MODULE_FLASH_LOADER_WRITE;
                    frame.identifier = id;
                    frame.broadcast = false;
                    frame.direction = 1;
                    frame.ack = true;
                    frame.length = packet.size%1024;
                    frame.seq_num = sequence;
                    memcpy(frame.data, &packet.data[sequence*1024+16], packet.size%1024); // data
                    if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(100)) < 0) {
                        MODULE_ERROR();
                    }
                }                
                packet.direction = 1;
                packet.command = 0x03;
                packet.size = 4;
                packet.value = 0;
                memset(packet.data, 0x00, 4);
                UsbSerialProtocol::write(&packet);
                break;

            case UsbSerialProtocol::SYNC:
                packet.direction = 1;
                packet.size = 4;
                memset(packet.data, 0x00, 4);
                for (int i=0; i<8; i++)
                    UsbSerialProtocol::write(&packet);
                break;

            case UsbSerialProtocol::READ_REG:
                frame.command = MODULE_READ_REGISTER;
                frame.identifier = id;
                frame.broadcast = false;
                frame.direction = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, packet.data, 4); // addr
                if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(200)) < 0) {
                    MODULE_ERROR();
                } else {
                    packet.direction = 1;
                    packet.size = 4;
                    memcpy(&packet.value, frame.data, 4); // reg value
                    memset(packet.data, 0x00, 4);
                    UsbSerialProtocol::write(&packet);
                }
                break;

            case UsbSerialProtocol::SPI_FLASH_LOADER_MD5:
                frame.command = MODULE_FLASH_LOADER_CHECK;
                frame.identifier = id;
                frame.broadcast = false;
                frame.direction = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, &packet.data[4], 4); // flash size
                if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(500)) < 0) {
                    MODULE_ERROR();
                } else {
                    packet.direction = 1;
                    packet.size = 18;
                    memcpy(packet.data, frame.data, 16); // md5 sum
                    memset(&packet.data[16], 0x00, 2);
                    UsbSerialProtocol::write(&packet);
                }
                break;

            case UsbSerialProtocol::FLASH_LOADER_END:
                frame.command = MODULE_FLASH_LOADER_END;
                frame.identifier = id;
                frame.broadcast = false;
                frame.direction = 1;
                frame.ack = 0;
                frame.length = 0;
                BusRs::write(&frame);
                packet.direction = 1;
                packet.size = 4;
                memset(packet.data, 0x00, 4);
                UsbSerialProtocol::write(&packet);
                vTaskDelay(1000/portTICK_PERIOD_MS);
                esp_restart();
                break;
            
            default:
                packet.direction = 1;
                packet.size = 4;
                memset(packet.data, 0x00, 4);
                UsbSerialProtocol::write(&packet);
                break;
            }
        }
    }
    free(packet.data);
    packet.data = NULL;
    free(frame.data);
    frame.data = NULL;
    vTaskDelete(NULL);
}
