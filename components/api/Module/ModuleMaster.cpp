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

std::map<std::pair<ModuleCmd_EventId_t,uint16_t>, ModuleCmd_EventCallback_t> ModuleMaster::_callback;
std::map<uint16_t,int,std::greater<uint16_t>> ModuleMaster::_ids;

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

    /* Check if IDs are in bus order or by Serial Number */
    int num_id_auto = 0;
    int num_id_sn = 0;

    for (int i=0; i<ModuleControl::_instances.size(); i++) {
        if (ModuleControl::getSN(ModuleControl::_instances[i]) == 0) {
            num_id_auto++;
        } else {
            num_id_sn++;
        }
    }

    if ((num_id_auto > 0) && (num_id_sn > 0)) {
        ESP_LOGE(MODULE_TAG, "Modules must be initialized using the same constructor. You cannot initialize some module without SN and some modules with SN.");
        ESP_LOGE(MODULE_TAG, "Number of module with SN:%i", num_id_sn);
        ESP_LOGE(MODULE_TAG, "Number of module without SN:%i", num_id_auto);
        return false;
    }

    /* Initialize module with auto id (bus order) */
    if (num_id_auto) {
        
        _ids.clear();

        BusRs::Frame_t frame;
        frame.command = CMD_DISCOVER;
        frame.id = 0;
        frame.dir = 1;
        frame.ack = false;
        frame.length = 0;
        BusRs::write(&frame);

        ModuleStandalone::ledOn(LED_YELLOW);

        /* Wait */
        vTaskDelay(200/portTICK_PERIOD_MS);
    
        if (ModuleControl::_instances.size() == _ids.size()) {
            std::map<uint16_t, int>::iterator it = _ids.begin();
            for (int i=0; i<ModuleControl::_instances.size(); i++) {
                ModuleControl::setId(ModuleControl::_instances[i], it->first);
                ModuleControl::setSN(ModuleControl::_instances[i], it->second);
                ++it;
                ModuleControl::_instances[i]->ledOn(LED_YELLOW);
                vTaskDelay(50/portTICK_PERIOD_MS);
            }
        } else {
            ESP_LOGE(MODULE_TAG, "Number of instantiated modules: %d",  ModuleControl::_instances.size());
            ESP_LOGE(MODULE_TAG, "Number of IDs received: %d",  _ids.size());
            return false;
        }
    }

    /* Initialize module with SN */
    else {
        for (int i=0; i<ModuleControl::_instances.size(); i++) {
            uint16_t current_id = ModuleMaster::getIdFromSN(ModuleControl::getSN(ModuleControl::_instances[i]));
            if (current_id != 0) {
                ModuleControl::setId(ModuleControl::_instances[i], current_id);
                ModuleControl::_instances[i]->ledOn(LED_YELLOW);
                vTaskDelay(50/portTICK_PERIOD_MS);
            } else {
                ESP_LOGE(MODULE_TAG, "Cannot instanciate module with SN:%i",  ModuleControl::getSN(ModuleControl::_instances[i]));
                return false;
            }
        }
    }

    vTaskDelay(50/portTICK_PERIOD_MS);

    /* Success, broadcast message to set all led green */
    for (int i=0; i<ModuleControl::_instances.size(); i++) {
        ModuleControl::_instances[i]->ledBlink(LED_GREEN, 1000);
    }
    return true;
}

void autoTest(void)
{
    /** @todo: auto test when starting the module */
}

void ModuleMaster::program(int num)
{
    UsbConsole::end(); // Do not perform in the task
    xTaskCreate(&_programmingTask, "Module programming task", 4096, (void*)num, 1, NULL);
    ModuleStandalone::ledBlink(LED_WHITE, 1000); // Programming mode
}

bool ModuleMaster::ping(int num) 
{
    BusRs::Frame_t frame;
    frame.command = CMD_PING;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 4;
    frame.data = (uint8_t*)&num; // Serial number
    BusRs::write(&frame, pdMS_TO_TICKS(10));
    return (BusRs::read(&frame, pdMS_TO_TICKS(10)) == 0);
}

void ModuleMaster::onEvent(ModuleCmd_EventId_t eventId, uint16_t id, ModuleCmd_EventCallback_t callback)
{
    _callback.insert({
        std::make_pair(eventId, id),
        callback
    });
}

void ModuleMaster::handleEvent(ModuleCmd_EventId_t eventId, uint16_t id, int num)
{
    if (_callback.find(std::make_pair(eventId, id)) != _callback.end()) {
        for (auto it=_callback.begin(); it!=_callback.end(); it++) {
            if ((it->first.first == eventId) && 
                (it->first.second == id)) {
                (*it).second(num);
            }
        }
    } else {
        ESP_LOGW(MODULE_TAG, "Event does not exist: event=0x%02x, id=%d", eventId, id);
    }
}

uint16_t ModuleMaster::getIdFromSN(int num)
{
    uint16_t id = 0;

    BusRs::Frame_t frame;
    frame.command = CMD_PING;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 4;
    frame.data = (uint8_t*)malloc(4);
    memcpy(frame.data, &num, 4); // Serial number
    BusRs::write(&frame, pdMS_TO_TICKS(100));
    BusRs::read(&frame, pdMS_TO_TICKS(100));
    id = frame.id;
    free(frame.data);
    return id;
}

void ModuleMaster::getBoardInfo(int num, Module_Info_t* board_info)
{
    uint16_t id = 0;
    
    id = ModuleMaster::getIdFromSN(num);
    if (id == 0) {
        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        return;
    }

    BusRs::Frame_t frame;
    frame.command = CMD_GET_BOARD_INFO;
    frame.id = id;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 0;
    frame.data = (uint8_t*)malloc(sizeof(Module_Info_t));
    BusRs::write(&frame, pdMS_TO_TICKS(100));
    BusRs::read(&frame, pdMS_TO_TICKS(100));
    memcpy(board_info, frame.data, sizeof(Module_Info_t));
    free(frame.data);
    return;
}

std::map<uint16_t,int,std::greater<uint16_t>> ModuleMaster::discoverSlaves()
{
    // Delete previous id list
    _ids.clear();

    BusRs::Frame_t frame;
    frame.command = CMD_DISCOVER;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 0;
    BusRs::write(&frame);

    // Wait for slaves to answer
    vTaskDelay(pdMS_TO_TICKS(200));

    return _ids;
}

void ModuleMaster::_busTask(void *pvParameters) 
{
    BusCan::Frame_t frame;
    uint16_t id;
    while (1) {
        if (BusCan::read(&frame, &id, portMAX_DELAY) != -1) { 
            ESP_LOGD(MODULE_TAG, "Bus CAN read to %d | command(%d), data(%d)\n", 
                id, frame.command, frame.data);
            switch (frame.command)
            {
            case CMD_EVENT:
                handleEvent((ModuleCmd_EventId_t)frame.data_byte[0], id, (int)frame.data_byte[1]);
                break;
            case CMD_DISCOVER:
                _ids.insert(std::pair<int, int>(id, (int)frame.data));
                break;
            
            default:
                ESP_LOGW(MODULE_TAG, "Receive undefined command");
                break;
            }
        } else {
            ModuleStandalone::ledBlink(LED_RED, 1000); // Error
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
    id = getIdFromSN(num);

    /* FlashLoader begin */
    frame.command = CMD_FLASH_LOADER_BEGIN;
    frame.id = id;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 0;
    BusRs::write(&frame, pdMS_TO_TICKS(5000));
    if (BusRs::read(&frame, pdMS_TO_TICKS(5000)) < 0) {
        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        goto end;
    }

    UsbSerialProtocol::begin(115200);

    while (1) {
        if (UsbSerialProtocol::read(&packet) < 0) {
            ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        } else {
            switch (packet.command)
            {
            case UsbSerialProtocol::FLASH_LOADER_DATA:
                sequence = 0;
                memcpy(&packet.size, &packet.data[0], 4); // data size
                for (int i=0; i<(packet.size/1024); i++) {
                    frame.command = CMD_FLASH_LOADER_WRITE;
                    frame.id = id;
                    frame.dir = 1;
                    frame.ack = true;
                    frame.length = 1024;
                    memcpy(frame.data, &packet.data[sequence*1024+16], 1024); // data
                    BusRs::write(&frame, pdMS_TO_TICKS(100));
                    if (BusRs::read(&frame, pdMS_TO_TICKS(100)) < 0) {
                        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
                        goto end;
                    }
                    sequence++;
                }
                if ((packet.size%1024) > 0) {
                    frame.command = CMD_FLASH_LOADER_WRITE;
                    frame.id = id;
                    frame.dir = 1;
                    frame.ack = true;
                    frame.length = packet.size%1024;
                    memcpy(frame.data, &packet.data[sequence*1024+16], packet.size%1024); // data
                    BusRs::write(&frame, pdMS_TO_TICKS(100));
                    if (BusRs::read(&frame, pdMS_TO_TICKS(100)) < 0) {
                        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
                        goto end;
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
                frame.command = CMD_READ_REGISTER;
                frame.id = id;
                frame.dir = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, packet.data, 4); // addr
                BusRs::write(&frame, pdMS_TO_TICKS(200));
                if (BusRs::read(&frame, pdMS_TO_TICKS(200)) < 0) {
                    ModuleStandalone::ledBlink(LED_RED, 1000); // Error
                    goto end;
                } else {
                    packet.direction = 1;
                    packet.size = 4;
                    memcpy(&packet.value, frame.data, 4); // reg value
                    memset(packet.data, 0x00, 4);
                    UsbSerialProtocol::write(&packet);
                }
                break;

            case UsbSerialProtocol::SPI_FLASH_LOADER_MD5:
                frame.command = CMD_FLASH_LOADER_CHECK;
                frame.id = id;
                frame.dir = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, &packet.data[4], 4); // flash size
                BusRs::write(&frame, pdMS_TO_TICKS(500));
                if (BusRs::read(&frame, pdMS_TO_TICKS(3000)) < 0) {
                    ModuleStandalone::ledBlink(LED_RED, 1000); // Error
                    goto end;
                } else {
                    packet.direction = 1;
                    packet.size = 18;
                    memcpy(packet.data, frame.data, 16); // md5 sum
                    memset(&packet.data[16], 0x00, 2);
                    UsbSerialProtocol::write(&packet);
                }
                break;
            
            case UsbSerialProtocol::CHANGE_BAUDRATE:
                packet.direction = 1;
                packet.size = 4;
                memset(packet.data, 0x00, 4);
                UsbSerialProtocol::write(&packet);
                UsbSerialProtocol::setBaudrate(921600);
                break;

            case UsbSerialProtocol::FLASH_LOADER_END:
                frame.command = CMD_FLASH_LOADER_END;
                frame.id = id;
                frame.dir = 1;
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

end:
    free(packet.data);
    packet.data = NULL;
    free(frame.data);
    frame.data = NULL;
    vTaskDelete(NULL);
}
