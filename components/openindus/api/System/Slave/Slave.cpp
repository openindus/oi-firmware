/**
 * @file Slave.cpp
 * @brief Slave class implementation
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Slave.h"

#if defined(CONFIG_MODULE_SLAVE)

static const char TAG[] = "Slave";

uint16_t Slave::_id;
State_e Slave::_state = STATE_IDLE;
TaskHandle_t Slave::_busTaskHandle = NULL;
std::map<uint8_t, std::function<void(std::vector<uint8_t>&)>> Slave::_callbacks;
std::list<std::function<void(void)>> Slave::_resetCallbacks;
std::map<uint8_t, std::function<void(std::vector<uint8_t> &)>> Slave::_eventCallbacks;
std::vector<EventCallbackConfig_s> Slave::_eventCallbackConfigs;

int Slave::init(void)
{
    int err = 0;

    /* Board ID is represented by the 10 most significants bits of the adc reading (12 bits) */
    _id = (uint16_t) (BusIO::readId()>>2);
    ESP_LOGI(TAG, "Bus Id: %d", _id);

    /* Bus task */
    ESP_LOGI(TAG, "Create BusRS task");
    xTaskCreate(_busRsTask, "BusRS task", 4096, NULL, 1, &_busTaskHandle);

    ESP_LOGI(TAG, "Create BusCAN task");
    xTaskCreate(_busCanTask, "BusCAN task", 4096, NULL, 1, &_busTaskHandle);

    _state = STATE_RUNNING;

    _registerCLI();

    return err;
}

void Slave::start(void)
{
    if (_busTaskHandle != NULL) {
        vTaskResume(_busTaskHandle);
    }
    _state = STATE_RUNNING;
}

void Slave::stop(void)
{
    if (_busTaskHandle != NULL) {
        vTaskSuspend(_busTaskHandle);
    }
    _state = STATE_IDLE;
}

int Slave::getStatus(void)
{
    return (int)_state;
}

/**
 * @brief Send an event on the CAN bus
 * 
 * @param msgBytes 
 */
void Slave::sendEvent(std::vector<uint8_t> msgBytes)
{
    BusCAN::Frame_t frame;
    frame.cmd = CMD_SEND_EVENT;
    std::copy(msgBytes.begin(), msgBytes.end(), frame.args);
    uint8_t size = msgBytes.size() + 1;
    BusCAN::write(&frame, _id, size);
}

/**
 * @brief Send an error on the CAN bus
 * 
 * @param errorCode 
 */
void Slave::sendError(uint8_t errorCode)
{
    BusCAN::Frame_t frame;
    frame.cmd = CMD_SEND_ERROR;
    frame.args[0] = errorCode;
    BusCAN::write(&frame, _id, 1);
}

void Slave::_busRsTask(void *pvParameters) 
{
    BusRS::Frame_t frame;
    frame.length = 1024;
    frame.data = (uint8_t*)malloc(frame.length);
    while (1) {
        if (BusRS::read(&frame, portMAX_DELAY) < 0) {
            Led::blink(LED_RED, 1000); // Error
        } else {
            switch (frame.cmd)
            {
            case CMD_RESTART:
            {
                ESP_LOGI(TAG, "Restart");

                Board::restart();
                break;
            }
            case CMD_PING:
            {
                ESP_LOGI(TAG, "Ping");

                uint16_t type; // Board type
                uint32_t num; // Serial number
                memcpy(&type, frame.data, sizeof(uint16_t));
                memcpy(&num, &frame.data[2], sizeof(uint32_t));
                if (num ==  Board::getSerialNum() && type == Board::getBoardType()) {
                    frame.dir = 0;
                    frame.ack = false;
                    frame.id = _id; // return our id --> the master can get the id from serialNumber
                    BusRS::write(&frame);
                }
                break;
            }
            case CMD_DISCOVER_SLAVES:
            {
                ESP_LOGI(TAG, "Discover slaves");

                BusCAN::Frame_t discoverFrame;
                discoverFrame.cmd = CMD_DISCOVER_SLAVES;
                uint16_t type = Board::getBoardType();
                uint32_t sn = Board::getSerialNum();
                memcpy(discoverFrame.args, &type, sizeof(uint16_t));
                memcpy(&discoverFrame.args[2], &sn, sizeof(uint32_t));
                if (BusCAN::write(&discoverFrame, _id, sizeof(uint16_t)+sizeof(uint32_t)+1) == -1)
                    Led::blink(LED_RED, 1000); // Error
                break;
            }
            case CMD_GET_BOARD_INFO:
            {
                ESP_LOGI(TAG, "Get board info");

                if (frame.id == _id) {
                    Board_Info_t board_info;
                    board_info.efuse.board_type = Board::getBoardType();
                    board_info.efuse.hardware_variant = Board::getHardwareVariant();
                    board_info.efuse.serial_number = Board::getSerialNum();
                    Board::getSoftwareVersion(board_info.software_version);
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
                    Led::blink(LED_WHITE, 1000); // Programming mode
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
            case CMD_RUN_CALLBACK:
            {
                if (frame.id == _id) {
                    std::vector<uint8_t> msg;
                    msg.assign(frame.data, frame.data + frame.length);
                    auto it = _callbacks.find(frame.data[0]);
                    if (it != _callbacks.end()) {
                        (*it).second(msg);
                    } else {
                        frame.error = 1;
                        ESP_LOGW(TAG, "Callback does not exist: %d", frame.data[0]);
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
            case CMD_LED_CTRL:
            {
                LedState_t state;
                LedColor_t color;
                uint32_t period;

                if (frame.id == _id) {
                    state = (LedState_t)frame.data[0];
                    color = (LedColor_t)frame.data[1];
                    memcpy(&period, &frame.data[2], sizeof(uint32_t));
                    if (state == LED_ON) {
                        Led::on(color);
                    } else if (state == LED_OFF) {
                        Led::off();
                    } else if (state == LED_BLINK) {
                        Led::blink(color, period);
                    } else if (state == LED_SYNC) {
                        Led::sync();
                    } else {
                        // Invalid state
                    }
                }
                break;
            }
            case CMD_RESET:
            {
                ESP_LOGI(TAG, "Reset module");

                for (const auto& resetCallback : _resetCallbacks) {
                    resetCallback();
                }
                break;
            }
            case CMD_REGISTER_EVENT_CALLBACK:
            {
                if (frame.id == _id) {
                    ESP_LOGI(TAG, "Config event callback");

                    EventCallbackConfig_s eventConfig;
                    eventConfig.moduleId = (frame.data[1] << 8) | frame.data[0];
                    eventConfig.eventId = frame.data[2];
                    eventConfig.eventArg = frame.data[3];
                    eventConfig.callbackId = frame.data[4];
                    eventConfig.callbackArgs = std::vector<uint8_t>(frame.data + 5, frame.data + frame.length);
                    _eventCallbackConfigs.push_back(eventConfig); // Store the event callback config
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

void Slave::_busCanTask(void *pvParameters) 
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
                    for (auto it = _eventCallbackConfigs.begin(); it != _eventCallbackConfigs.end(); ++it) {
                        if ((it->moduleId == id) && 
                            (it->eventId == frame.args[0]) &&
                            (it->eventArg == frame.args[1])) {
                                auto callbackIt = _eventCallbacks.find(it->callbackId);
                                if (callbackIt != _eventCallbacks.end()) {
                                    callbackIt->second(it->callbackArgs);
                                }
                        }
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }
}

void Slave::_heartbeatTask(void *pvParameters)
{
    /** @todo Send heartbeat periodically */
}

#endif