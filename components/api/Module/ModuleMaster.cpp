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

#if defined(MODULE_MASTER)

#include "Module.h"

static const char TAG[] = "Module";

std::map<uint16_t, std::pair<uint16_t, uint32_t>, std::greater<uint16_t>> ModuleMaster::_ids;

int ModuleMaster::init(void)
{
    int err = 0;

    ESP_LOGI(TAG, "Bus init");

    /* Bus RS/CAN */
    err |= BusRS::begin(MODULE_RS_NUM_PORT, MODULE_PIN_RS_UART_TX, MODULE_PIN_RS_UART_RX);
    err |= BusCAN::begin(MODULE_PIN_CAN_TX, MODULE_PIN_CAN_RX);

    /* Bus IO */
    BusIO::Config_t config = {
        .adcChannelId = MODULE_OI_ID_ADC_CHANNEL,
        .adcWidthId = MODULE_OI_ID_ADC_WIDTH,
        .gpioNumSync = MODULE_PIN_OI_GPIO,
        .gpioModeSync = GPIO_MODE_INPUT_OUTPUT,
        .gpioNumPower = MODULE_PIN_CMD_MOSFET_ALIM,
    };
    err |= BusIO::init(&config);
    BusIO::writeSync(0);

    ESP_LOGI(TAG, "Create bus task");
    xTaskCreate(_busTask, "Bus task", 4096, NULL, 1, NULL);

    return err;
}

bool ModuleMaster::autoId(void)
{
    ESP_LOGI(TAG, "Auto ID");

    /* Check if IDs are in bus order or by Serial Number */
    int num_id_auto = 0;
    int num_id_sn = 0;

    for (int i=0; i<ModuleControl::_instances.size(); i++) {
        if (ModuleControl::_instances[i]->getSN() == 0) {
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

        ModuleStandalone::ledOn(LED_YELLOW);

        /* Wait */
        vTaskDelay(200/portTICK_PERIOD_MS);
    
        if (ModuleControl::_instances.size() == _ids.size()) {
            std::map<uint16_t, std::pair<uint16_t, uint32_t>>::iterator it = _ids.begin();
            for (int i=0; i<ModuleControl::_instances.size(); i++) {
                // First, check if board type is good
                if (it->second.first == ModuleControl::_instances[i]->getType()) {
                    // If OK, set ID and SN
                    ModuleControl::_instances[i]->setId(it->first);
                    ModuleControl::_instances[i]->setSN(it->second.second);
                    ++it;
                    ModuleControl::_instances[i]->ledOn(LED_YELLOW);
                    vTaskDelay(50/portTICK_PERIOD_MS);
                } else {
                    char name[16];
                    ESP_LOGE(TAG, "Type of module %i is incorrect: you have instantiate an %s and module detected is an %s", \
                                        i+1, \
                                        ModuleUtils::typeToName(ModuleControl::_instances[i]->getType(), name), \
                                        ModuleUtils::typeToName(it->second.first, name));
                    ESP_LOGE(TAG, "Check that the order of module in your main.cpp file correspond with the order of modules on the rail");
                    return false;
                }
            }
        } else {
            ESP_LOGE(TAG, "Number of instantiated modules: %d",  ModuleControl::_instances.size());
            ESP_LOGE(TAG, "Number of IDs received: %d",  _ids.size());
            return false;
        }
    }

    /* Initialize module with SN */
    else {
        for (int i=0; i<ModuleControl::_instances.size(); i++) {
            uint16_t current_id = ModuleMaster::_getIdFromSerialNumAndType(ModuleControl::_instances[i]->getType(), ModuleControl::_instances[i]->getSN());
            if (current_id != 0) {
                ModuleControl::_instances[i]->setId(current_id);
                ModuleControl::_instances[i]->ledOn(LED_YELLOW);
                vTaskDelay(50/portTICK_PERIOD_MS);
            } else {
                ESP_LOGE(TAG, "Cannot instantiate module with SN:%i",  ModuleControl::_instances[i]->getSN());
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

void ModuleMaster::program(uint16_t type, uint32_t sn)
{
    UsbConsole::end(); // Do not perform in the task
    uint16_t id = _getIdFromSerialNumAndType(type, sn);
    xTaskCreate(_programmingTask, "Module programming task", 4096, (void*)&id, 1, NULL);
    ModuleStandalone::ledBlink(LED_WHITE, 1000); // Programming mode
}

bool ModuleMaster::ping(uint16_t type, uint32_t sn) 
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_PING;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = sizeof(type)+sizeof(sn);
    frame.data = (uint8_t*)malloc(sizeof(type)+sizeof(sn));
    memcpy(frame.data, &type, sizeof(type)); // Type 
    memcpy(&frame.data[sizeof(type)], &sn, sizeof(sn)); // Serial number
    BusRS::write(&frame, pdMS_TO_TICKS(10));
    return (BusRS::read(&frame, pdMS_TO_TICKS(10)) == 0);
}

void ModuleMaster::getBoardInfo(uint16_t type, uint32_t sn, Module_Info_t* board_info)
{
    uint16_t id = _getIdFromSerialNumAndType(type, sn);

    if (id == 0) {
        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        return;
    }

    BusRS::Frame_t frame;
    frame.cmd = CMD_GET_BOARD_INFO;
    frame.id = id;
    frame.dir = 1;
    frame.ack = true;
    frame.length = 0;
    frame.data = (uint8_t*)malloc(sizeof(Module_Info_t));
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    BusRS::read(&frame, pdMS_TO_TICKS(100));
    memcpy(board_info, frame.data, sizeof(Module_Info_t));
    free(frame.data);
    return;
}

std::map<uint16_t,std::pair<uint16_t, uint32_t>,std::greater<uint16_t>> ModuleMaster::discoverSlaves(void)
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

uint16_t ModuleMaster::_getIdFromSerialNumAndType(uint16_t type, uint32_t sn)
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
    memcpy(&frame.data[sizeof(type)], &sn, sizeof(sn)); // Serial number
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    BusRS::read(&frame, pdMS_TO_TICKS(100));
    id = frame.id;
    free(frame.data);
    return id;
}

void ModuleMaster::_busTask(void *pvParameters) 
{
    BusCAN::Frame_t frame;
    uint16_t id;
    uint8_t length;

    while (1) {
        if (BusCAN::read(&frame, &id, &length) != -1) { 
            switch (frame.cmd)
            {
                case CMD_EVENT:
                {
                    auto it = ModuleControl::_eventCallbacks.find(std::make_pair(frame.data[0], id));
                    if (it != ModuleControl::_eventCallbacks.end()) {
                        if (it->second != NULL) {
                            it->second(frame.data[1]);
                        }
                    } else {
                        ESP_LOGW(TAG, "Command does not exist: command: 0x%02x, id: %d", frame.data[0], id);
                    }
                    break;
                }
                case CMD_DISCOVER:
                {
                    uint16_t* type = reinterpret_cast<uint16_t*>(&frame.data[0]);
                    uint32_t* sn = reinterpret_cast<uint32_t*>(&frame.data[2]);
                    _ids.insert(std::pair<uint16_t, std::pair<uint16_t, uint32_t>>(id, std::pair<uint16_t, uint32_t>(*type, *sn)));
                    break;
                }
                default:
                {
                    ESP_LOGW(TAG, "Receive undefined command");
                    break;
                }
            }
        } else {
            ModuleStandalone::ledBlink(LED_RED, 1000); // Error
        }
    }
}

void ModuleMaster::_programmingTask(void *pvParameters)
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
                    frame.cmd = CMD_FLASH_LOADER_WRITE;
                    frame.id = id;
                    frame.dir = 1;
                    frame.ack = true;
                    frame.length = 1024;
                    memcpy(frame.data, &packet.data[sequence*1024+16], 1024); // data
                    BusRS::write(&frame, pdMS_TO_TICKS(100));
                    if (BusRS::read(&frame, pdMS_TO_TICKS(100)) < 0) {
                        ModuleStandalone::ledBlink(LED_RED, 1000); // Error
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
                frame.cmd = CMD_READ_REGISTER;
                frame.id = id;
                frame.dir = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, packet.data, 4); // addr
                BusRS::write(&frame, pdMS_TO_TICKS(200));
                if (BusRS::read(&frame, pdMS_TO_TICKS(200)) < 0) {
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
                frame.cmd = CMD_FLASH_LOADER_CHECK;
                frame.id = id;
                frame.dir = 1;
                frame.ack = true;
                frame.length = 4;
                memcpy(frame.data, &packet.data[4], 4); // flash size
                BusRS::write(&frame, pdMS_TO_TICKS(500));
                if (BusRS::read(&frame, pdMS_TO_TICKS(3000)) < 0) {
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