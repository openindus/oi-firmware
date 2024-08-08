/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControllerMaster.cpp
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "ControllerMaster.h"

static const char TAG[] = "ControllerMaster";

Controller_State_t ControllerMaster::_state = STATE_IDLE;
TaskHandle_t ControllerMaster::_taskHandle = NULL;
std::map<uint16_t, std::pair<uint16_t, uint32_t>, std::greater<uint16_t>> ControllerMaster::_ids;
std::map<std::pair<uint8_t, uint16_t>, std::function<void(uint8_t)>> ControllerMaster::_eventCallbacks;
std::vector<Controller*> ControllerMaster::_instances;

int ControllerMaster::init(void)
{
    int err = 0;

    BusIO::writeSync(0);

    ESP_LOGI(TAG, "Create bus task");
    xTaskCreate(_busTask, "Bus task", 4096, NULL, 1, &_taskHandle);

    _state = STATE_RUNNING;

    return err;
}

void ControllerMaster::start(void)
{
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle);
    }
    _state = STATE_RUNNING;
}

void ControllerMaster::stop(void)
{
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
    }
    _state = STATE_IDLE;
}

int ControllerMaster::getStatus(void)
{
    return (int)_state;
}

bool ControllerMaster::autoId(void)
{
    ESP_LOGI(TAG, "Auto ID");

    /* Check if IDs are in bus order or by Serial Number */
    int num_id_auto = 0;
    int num_id_sn = 0;

    for (int i=0; i<_instances.size(); i++) {
        if (_instances[i]->getSN() == 0) {
            num_id_auto++;
        } else {
            num_id_sn++;
        }
    }

    if ((num_id_auto > 0) && (num_id_sn > 0)) {
        ESP_LOGE(TAG, "Modules must be initialized using the same constructor. You cannot initialize some module without SN and some modules with SN.");
        ESP_LOGE(TAG, "Number of module with SN:%i", num_id_sn);
        ESP_LOGE(TAG, "Number of module without SN:%i", num_id_auto);
        return false;
    }

    /* Initialize module with auto id (bus order) */
    if (num_id_auto) {
        
        _ids.clear();

        BusRS::Frame_t frame;
        frame.cmd = CMD_DISCOVER;
        frame.id = 0;
        frame.dir = 1;
        frame.ack = false;
        frame.length = 0;
        BusRS::write(&frame);

        Led::on(LED_YELLOW);

        /* Wait */
        vTaskDelay(200/portTICK_PERIOD_MS);
    
        if (_instances.size() == _ids.size()) {
            std::map<uint16_t, std::pair<uint16_t, uint32_t>>::iterator it = _ids.begin();
            for (int i=0; i<_instances.size(); i++) {
                // First, check if board type is good
                if (BoardUtils::areTypeCompatible(it->second.first, _instances[i]->getType())) {
                    // If OK, set ID and SN
                    _instances[i]->setId(it->first);
                    _instances[i]->setSN(it->second.second);
                    ++it;
                    _instances[i]->ledOn(LED_YELLOW);
                    vTaskDelay(50/portTICK_PERIOD_MS);
                } else {
                    char name1[16];
                    char name2[16];
                    ESP_LOGE(TAG, "Type of module %i is incorrect: you declared an %s and module detected is an %s", \
                                        i+1, \
                                        BoardUtils::typeToName(_instances[i]->getType(), name1), \
                                        BoardUtils::typeToName(it->second.first, name2));
                    ESP_LOGE(TAG, "Check that the order of module in your main.cpp file correspond with the order of modules on the rail");
                    return false;
                }
            }
        } else {
            ESP_LOGE(TAG, "Number of instantiated modules: %d",  _instances.size());
            ESP_LOGE(TAG, "Number of IDs received: %d",  _ids.size());
            return false;
        }
    }

    /* Initialize module with SN */
    else {
        for (int i=0; i<_instances.size(); i++) {
            uint16_t current_id = ControllerMaster::_getIdFromSerialNumAndType(_instances[i]->getType(), _instances[i]->getSN());
            if (current_id != 0) {
                _instances[i]->setId(current_id);
                _instances[i]->ledOn(LED_YELLOW);
                vTaskDelay(50/portTICK_PERIOD_MS);
            } else {
                ESP_LOGE(TAG, "Cannot instantiate module with SN:%i",  _instances[i]->getSN());
                return false;
            }
        }
    }

    vTaskDelay(50/portTICK_PERIOD_MS);

    /* Success, broadcast message to set all led green */
    for (int i=0; i<_instances.size(); i++) {
        _instances[i]->ledBlink(LED_GREEN, 1000);
    }
    return true;
}

void autoTest(void)
{
    /** @todo: auto test when starting the module */
}

void ControllerMaster::program(uint16_t type, uint32_t sn)
{
    UsbConsole::end(); // Do not perform in the task
    uint16_t id = _getIdFromSerialNumAndType(type, sn);
    xTaskCreate(_programmingTask, "Module programming task", 4096, (void*)&id, 1, NULL);
    Led::blink(LED_WHITE, 1000); // Programming mode
}

bool ControllerMaster::ping(uint16_t type, uint32_t sn) 
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_PING;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = sizeof(type)+sizeof(sn);
    frame.data = (uint8_t*)malloc(sizeof(type)+sizeof(sn));
    memcpy(frame.data, &type, sizeof(type)); // Type 
    memcpy(&frame.data[2], &sn, sizeof(sn)); // Serial number
    BusRS::write(&frame, pdMS_TO_TICKS(10));
    return (BusRS::read(&frame, pdMS_TO_TICKS(10)) == 0);
}

void ControllerMaster::getBoardInfo(uint16_t type, uint32_t sn, Board_Info_t* board_info)
{
    uint16_t id = _getIdFromSerialNumAndType(type, sn);

    if (id == 0) {
        Led::blink(LED_RED, 1000); // Error
        return;
    }

    BusRS::Frame_t frame;
    frame.cmd = CMD_GET_BOARD_INFO;
    frame.id = id;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 0;
    frame.data = (uint8_t*)malloc(sizeof(Board_Info_t));
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    BusRS::read(&frame, pdMS_TO_TICKS(100));
    memcpy(board_info, frame.data, sizeof(Board_Info_t));
    free(frame.data);
    return;
}

std::map<uint16_t,std::pair<uint16_t, uint32_t>,std::greater<uint16_t>> ControllerMaster::discoverSlaves(void)
{
    // Delete previous id list
    _ids.clear();

    BusRS::Frame_t frame;
    frame.cmd = CMD_DISCOVER;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 0;
    BusRS::write(&frame);

    // Wait for slaves to answer
    vTaskDelay(pdMS_TO_TICKS(200));

    return _ids;
}

uint16_t ControllerMaster::_getIdFromSerialNumAndType(uint16_t type, uint32_t sn)
{
    uint16_t id = 0;

    BusRS::Frame_t frame;
    frame.cmd = CMD_PING;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = sizeof(type)+sizeof(sn);
    frame.data = (uint8_t*)malloc(sizeof(type)+sizeof(sn));
    memcpy(frame.data, &type, sizeof(type)); // Type 
    memcpy(&frame.data[2], &sn, sizeof(sn)); // Serial number
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    BusRS::read(&frame, pdMS_TO_TICKS(100));
    id = frame.id;
    free(frame.data);
    return id;
}

void ControllerMaster::_busTask(void *pvParameters) 
{
    BusCAN::Frame_t frame;
    uint16_t id;
    uint8_t size;

    while (1) {
        if (BusCAN::read(&frame, &id, &size) != -1) { 
            switch (frame.cmd)
            {
                case CMD_EVENT:
                {
                    auto it = _eventCallbacks.find(std::make_pair(frame.args[0], id));
                    if (it != _eventCallbacks.end()) {
                        if (it->second != NULL) {
                            it->second(frame.args[1]);
                        }
                    } else {
                        ESP_LOGW(TAG, "Command does not exist: command: 0x%02x, id: %d", frame.args[0], id);
                    }
                    break;
                }
                case CMD_DISCOVER:
                {
                    uint16_t* type = reinterpret_cast<uint16_t*>(&frame.args[0]);
                    uint32_t* sn = reinterpret_cast<uint32_t*>(&frame.args[2]);
                    _ids.insert(std::pair<uint16_t, std::pair<uint16_t, uint32_t>>(id, std::pair<uint16_t, uint32_t>(*type, *sn)));
                    char name[16];
                    ESP_LOGI(TAG, "Received id from %s\t SN:%i | ID:%i", BoardUtils::typeToName(*type, name), *sn, id);
                    break;
                }
                default:
                {
                    ESP_LOGW(TAG, "Receive undefined command");
                    break;
                }
            }
        } else {
            Led::blink(LED_RED, 1000); // Error
        }
    }
}

void ControllerMaster::_programmingTask(void *pvParameters)
{
    uint16_t id = *(uint16_t*)pvParameters;
    int sequence = 0;
    UsbSerialProtocol::Packet_t packet;
    packet.data = (uint8_t*)malloc(16400);
    BusRS::Frame_t frame;
    frame.data = (uint8_t*)malloc(1024);

    /* FlashLoader begin */
    frame.cmd = CMD_FLASH_LOADER_BEGIN;
    frame.id = id;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 0;
    BusRS::write(&frame, pdMS_TO_TICKS(5000));
    if (BusRS::read(&frame, pdMS_TO_TICKS(5000)) < 0) {
        Led::blink(LED_RED, 1000); // Error
        goto end;
    }

    UsbSerialProtocol::begin(115200);

    while (1) {
        if (UsbSerialProtocol::read(&packet) < 0) {
            Led::blink(LED_RED, 1000); // Error
        } else {
            switch (packet.command)
            {
            case UsbSerialProtocol::FLASH_LOADER_DATA:
                sequence = 0;
                memcpy(&packet.size, &packet.data[0], 4); // data size
                for (int i=0; i<(packet.size/1024); i++) {
                    frame.cmd = CMD_FLASH_LOADER_WRITE;
                    frame.id = id;
                    frame.dir = 1;
                    frame.ack = true;
                    frame.length = 1024;
                    memcpy(frame.data, &packet.data[sequence*1024+16], 1024); // data
                    BusRS::write(&frame, pdMS_TO_TICKS(100));
                    if (BusRS::read(&frame, pdMS_TO_TICKS(100)) < 0) {
                        Led::blink(LED_RED, 1000); // Error
                        goto end;
                    }
                    sequence++;
                }
                if ((packet.size%1024) > 0) {
                    frame.cmd = CMD_FLASH_LOADER_WRITE;
                    frame.id = id;
                    frame.dir = 1;
                    frame.ack = true;
                    frame.length = packet.size%1024;
                    memcpy(frame.data, &packet.data[sequence*1024+16], packet.size%1024); // data
                    BusRS::write(&frame, pdMS_TO_TICKS(100));
                    if (BusRS::read(&frame, pdMS_TO_TICKS(100)) < 0) {
                        Led::blink(LED_RED, 1000); // Error
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
                frame.cmd = CMD_READ_REGISTER;
                frame.id = id;
                frame.dir = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, packet.data, 4); // addr
                BusRS::write(&frame, pdMS_TO_TICKS(200));
                if (BusRS::read(&frame, pdMS_TO_TICKS(200)) < 0) {
                    Led::blink(LED_RED, 1000); // Error
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
                frame.cmd = CMD_FLASH_LOADER_CHECK;
                frame.id = id;
                frame.dir = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, &packet.data[4], 4); // flash size
                BusRS::write(&frame, pdMS_TO_TICKS(500));
                if (BusRS::read(&frame, pdMS_TO_TICKS(3000)) < 0) {
                    Led::blink(LED_RED, 1000); // Error
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
                frame.cmd = CMD_FLASH_LOADER_END;
                frame.id = id;
                frame.dir = 1;
                frame.ack = 0;
                frame.length = 0;
                BusRS::write(&frame);
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

#endif