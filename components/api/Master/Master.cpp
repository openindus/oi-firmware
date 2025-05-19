/**
 * @file Master.cpp
 * @brief Master class implementation
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "Master.h"
#include "UsbConsole.h"
#include "UsbSerial.h"
#include "OSAL.h"

static const char TAG[] = "Master";

State_e Master::_state = STATE_IDLE;
TaskHandle_t Master::_busTaskHandle = NULL;
TaskHandle_t Master::_ledSyncTaskHandle = NULL;
SemaphoreHandle_t Master::_callbackMutex = NULL;

std::vector<ModuleControl*> Master::_modules;
std::map<uint16_t, Master::SlaveInfo, std::greater<uint16_t>> Master::_slaveInfos;
std::map<Master::EventKey, Master::EventCallback> Master::_eventCallbacks;
std::function<void(int)> Master::_errorCallback = NULL;

int Master::init(void)
{
    int err = 0;

    BusIO::writeSync(0);

    /* Initialize mutex to call function */
    _callbackMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_callbackMutex);

    ESP_LOGI(TAG, "Create BusCAN task");
    xTaskCreate(_busCanTask, "BusCAN task", 4096, NULL, 1, &_busTaskHandle);
    
    ESP_LOGI(TAG, "Create LED synchronization task");
    xTaskCreate(_ledSyncTask, "LED Sync task", 2048, NULL, 1, &_ledSyncTaskHandle);

    _state = STATE_RUNNING;

    /* CLI */
    _registerCLI();

    return err;
}

void Master::start(void)
{
    if (_busTaskHandle != NULL) {
        vTaskResume(_busTaskHandle);
    }
    _state = STATE_RUNNING;
}

void Master::stop(void)
{
    if (_busTaskHandle != NULL) {
        vTaskSuspend(_busTaskHandle);
    }
    _state = STATE_IDLE;
}

int Master::getStatus(void)
{
    return (int)_state;
}

bool Master::autoId(void)
{
    ESP_LOGI(TAG, "Auto ID");

    /* Check if IDs are in bus order or by Serial Number */
    int numIdAuto = 0;
    int numIdSN = 0;

    for (int i=0; i<_modules.size(); i++) {
        if (_modules[i]->getSN() == 0) {
            numIdAuto++;
        } else {
            numIdSN++;
        }
    }

    if ((numIdAuto > 0) && (numIdSN > 0)) {
        ESP_LOGE(TAG, "Modules must be initialized using the same constructor. \
            You cannot initialize some module without SN and some modules with SN.");
        ESP_LOGE(TAG, "Number of module with SN:%i", numIdSN);
        ESP_LOGE(TAG, "Number of module without SN:%i", numIdAuto);
        return false;
    }

    /* Initialize module with auto id (bus order) */
    if (numIdAuto) {
        _slaveInfos.clear();

        BusRS::Frame_t frame;
        frame.cmd = CMD_DISCOVER_SLAVES;
        frame.id = 0;
        frame.dir = 1;
        frame.ack = false;
        frame.length = 0;
        BusRS::write(&frame);

        Led::on(LED_YELLOW);

        /* Wait */
        delay(200);
    
        if (_modules.size() == _slaveInfos.size()) {
            std::map<uint16_t, SlaveInfo>::iterator it = _slaveInfos.begin();
            for (int i=0; i<_modules.size(); i++) {
                // First, check if board type is good
                if (BoardUtils::areTypeCompatible(it->second.first, _modules[i]->getType())) {
                    // If OK, set ID and SN
                    _modules[i]->setId(it->first);
                    _modules[i]->setSN(it->second.second);
                    ++it;
                    _modules[i]->ledOn(LED_YELLOW);
                    delay(50);
                } else {
                    char name1[16];
                    char name2[16];
                    ESP_LOGE(TAG, "Type of module %i is incorrect: you declared an %s and module detected is an %s", \
                        i+1, \
                        BoardUtils::typeToName(_modules[i]->getType(), name1), \
                        BoardUtils::typeToName(it->second.first, name2));
                    ESP_LOGE(TAG, "Check that the order of module in your main.cpp file correspond with the order of modules on the rail");
                    return false;
                }
            }
        } else {
            ESP_LOGE(TAG, "Number of instantiated modules: %d",  _modules.size());
            ESP_LOGE(TAG, "Number of IDs received: %d",  _slaveInfos.size());
            return false;
        }
    }

    /* Initialize module with SN */
    else {
        for (int i=0; i<_modules.size(); i++) {
            uint16_t id = Master::getSlaveId(_modules[i]->getType(), _modules[i]->getSN());
            if (id != 0) {
                _modules[i]->setId(id);
                _modules[i]->ledOn(LED_YELLOW);
                delay(50);
            } else {
                ESP_LOGE(TAG, "Cannot instantiate module with SN:%i",  _modules[i]->getSN());
                return false;
            }
        }
    }

    delay(50);

    /* Success, broadcast message to set all led green */
    for (int i=0; i<_modules.size(); i++) {
        _modules[i]->ledBlink(LED_GREEN, 1000);
    }
    return true;
}

void autoTest(void)
{
    /** @todo: auto test when starting the module */
}

void Master::program(uint16_t boardType, uint32_t boardSN)
{
    UsbConsole::end(); // Do not perform in the task
    uint16_t id = getSlaveId(boardType, boardSN);
    xTaskCreate(_programmingTask, "Module programming task", 4096, (void*)&id, 1, NULL);
    Led::blink(LED_WHITE, 1000); // Programming mode
}

bool Master::ping(uint16_t boardType, uint32_t boardSN) 
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_PING;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = sizeof(boardType)+sizeof(boardSN);
    frame.data = (uint8_t*)malloc(sizeof(boardType)+sizeof(boardSN));
    memcpy(frame.data, &boardType, sizeof(boardType)); // Type 
    memcpy(&frame.data[2], &boardSN, sizeof(boardSN)); // Serial number
    BusRS::write(&frame, 10);
    return (BusRS::read(&frame, 10) == 0);
}

void Master::getBoardInfo(uint16_t boardType, uint32_t boardSN, Board_Info_t* info)
{
    uint16_t id = getSlaveId(boardType, boardSN);

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
    BusRS::write(&frame, 100);
    BusRS::read(&frame, 100);
    memcpy(info, frame.data, sizeof(Board_Info_t));
    free(frame.data);
    return;
}

std::map<uint16_t, Master::SlaveInfo, std::greater<uint16_t>> Master::discoverSlaves(void)
{
    // Delete previous id list
    _slaveInfos.clear();

    BusRS::Frame_t frame;
    frame.cmd = CMD_DISCOVER_SLAVES;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 0;
    BusRS::write(&frame);

    // Wait for slaves to answer
    delay(200);

    return _slaveInfos;
}

int Master::runCallback(const uint16_t slaveId, const uint8_t callbackId, std::vector<uint8_t> &args, bool ackNeeded)
{
    args.insert(args.begin(), callbackId);
    int ret = runCallback(slaveId, args, ackNeeded);
    if (!args.empty()) {
        args.erase(args.begin());
    }
    return ret;
}

int Master::runCallback(const uint16_t slaveId, std::vector<uint8_t> &msgBytes, bool ackNeeded)
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_RUN_CALLBACK;
    frame.id = slaveId;
    frame.dir = 1;
    frame.ack = ackNeeded;
    frame.length = msgBytes.size();
    frame.data = msgBytes.data();

    int err = 0;
    xSemaphoreTake(_callbackMutex, portMAX_DELAY);
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    if (ackNeeded) {
        err = BusRS::read(&frame, pdMS_TO_TICKS(100));
        if (err < 0) {
            goto error;
        } else {
            msgBytes.assign(frame.data, frame.data + frame.length);
            goto success;
        }
    }

success:
    xSemaphoreGive(_callbackMutex);
    return 0;

error:
    xSemaphoreGive(_callbackMutex);
    return -1;
}

void Master::ledCtrl(const uint16_t slaveId, const uint8_t state, const uint8_t color, const uint32_t period)
{
    std::vector<uint8_t> args = {state, color};
    args.insert(args.end(), (uint8_t *)&period, (uint8_t *)&period + sizeof(uint32_t));
    BusRS::Frame_t frame;
    frame.cmd = CMD_LED_CTRL;
    frame.id = slaveId;
    frame.dir = 1;
    frame.ack = false;
    frame.length = args.size();
    frame.data = args.data();
    BusRS::write(&frame, 100);
}

void Master::restart(const uint16_t slaveId)
{
    std::vector<uint8_t> args;
    BusRS::Frame_t frame;
    frame.cmd = CMD_RESTART;
    frame.id = slaveId;
    frame.dir = 1;
    frame.ack = false;
    frame.length = args.size();
    frame.data = args.data();
    BusRS::write(&frame, 100);
}

void Master::resetModules(void)
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_RESET;
    frame.id = 0;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 0;
    frame.data = NULL;
    BusRS::write(&frame);
}

void Master::registerEventCallback(uint16_t slaveId, ModuleControl* module, 
    uint8_t eventId, uint8_t eventArg, 
    uint8_t callbackId, std::vector<uint8_t> callbackArgs)
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_REGISTER_EVENT_CALLBACK;
    frame.id = slaveId;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 5 + callbackArgs.size();
    frame.data = (uint8_t*)malloc(frame.length);
    frame.data[0] = module->getId() & 0xFF;
    frame.data[1] = (module->getId() >> 8) & 0xFF;
    frame.data[2] = eventId;
    frame.data[3] = eventArg;
    frame.data[4] = callbackId;
    memcpy(&frame.data[5], callbackArgs.data(), callbackArgs.size());
    BusRS::write(&frame, 100);
    free(frame.data);
}

uint16_t Master::getSlaveId(uint16_t boardType, uint32_t boardSN)
{
    uint16_t id = 0;

    BusRS::Frame_t frame;
    frame.cmd = CMD_PING; // CMD_GET_SLAVE_ID
    frame.id = 0;
    frame.dir = 1;
    frame.ack = true;
    frame.length = sizeof(boardType)+sizeof(boardSN);
    frame.data = (uint8_t*)malloc(sizeof(boardType)+sizeof(boardSN));
    memcpy(frame.data, &boardType, sizeof(boardType)); // Type 
    memcpy(&frame.data[2], &boardSN, sizeof(boardSN)); // Serial number
    BusRS::write(&frame, 100);
    BusRS::read(&frame, 100);
    id = frame.id;
    free(frame.data);
    return id;
}

void Master::_busCanTask(void *pvParameters) 
{
    BusCAN::Frame_t frame;
    uint16_t id;
    uint8_t size;

    while (1) {
        if (BusCAN::read(&frame, &id, &size) != -1) { 
            switch (frame.cmd)
            {
                case CMD_SEND_EVENT:
                {
                    auto it = _eventCallbacks.find(std::make_pair(frame.args[0], id));
                    if (it != _eventCallbacks.end()) {
                        if (it->second != NULL) {
                            it->second(frame.args);
                        }
                    } else {
                        ESP_LOGW(TAG, "Command does not exist: command: 0x%02x, id: %d", frame.args[0], id);
                    }
                    break;
                }
                case CMD_DISCOVER_SLAVES:
                {
                    uint16_t* type = reinterpret_cast<uint16_t*>(&frame.args[0]);
                    uint32_t* sn = reinterpret_cast<uint32_t*>(&frame.args[2]);
                    _slaveInfos.insert(std::pair<uint16_t, std::pair<uint16_t, uint32_t>>(id, std::pair<uint16_t, uint32_t>(*type, *sn)));
                    char name[16];
                    ESP_LOGI(TAG, "Received id from %s\t SN:%i | ID:%i", BoardUtils::typeToName(*type, name), *sn, id);
                    break;
                }
                case CMD_SEND_ERROR:
                {
                    uint8_t errorCode = frame.args[0];
                    if (_errorCallback) {
                        _errorCallback(errorCode);
                    }
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

void Master::_ledSyncTask(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(60 * 60 * 1000); // 1 hour in milliseconds
    
    ESP_LOGI(TAG, "LED sync task started");
    
    while (1) {
        // Wait for one hour
        vTaskDelay(xDelay);
        
        ESP_LOGI(TAG, "Resynchronizing LEDs");

        Led::sync(); // Resynchronize the LED state
        
        // Iterate through all modules and resynchronize their LEDs
        for (int i = 0; i < _modules.size(); i++) {
            _modules[i]->ledSync();
        }
    }
}

void Master::_programmingTask(void *pvParameters)
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