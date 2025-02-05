/**
 * @file Controller.h
 * @brief Controller interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Bus.h"
#include "Led.h"

/* CAN */
#define CMD_EVENT                   (uint8_t) 0x07
// #define CMD_HEARTBEAT               (uint8_t) 0x10

/* RS */
#define CMD_NOP                     (uint8_t) 0x00
#define CMD_RESTART                 (uint8_t) 0x01
#define CMD_PING                    (uint8_t) 0x02
#define CMD_SET_LED                 (uint8_t) 0x03
#define CMD_DISCOVER                (uint8_t) 0x04
#define CMD_GET_BOARD_INFO          (uint8_t) 0x05
#define CMD_REQUEST                 (uint8_t) 0x06
#define CMD_FLASH_LOADER_BEGIN      (uint8_t) 0x08
#define CMD_FLASH_LOADER_WRITE      (uint8_t) 0x09
#define CMD_FLASH_LOADER_CHECK      (uint8_t) 0x0A
#define CMD_FLASH_LOADER_END        (uint8_t) 0x0B 
#define CMD_READ_REGISTER           (uint8_t) 0x0C
// #define CMD_WRITE_REGISTER          (uint8_t) 0x0D
// #define CMD_READ_NVS                (uint8_t) 0x0E
// #define CMD_WRITE_NVS               (uint8_t) 0x0F
// #define CMD_AUTO_TEST               (uint8_t) 0x11

typedef enum {
    STATE_UNDEFINED = (int)-1,
    STATE_IDLE      = (int)0,
    STATE_RUNNING   = (int)1,
    STATE_ERROR     = (int)2
} Controller_State_t;

#if defined(MODULE_MASTER)

class Controller
{
public:
    Controller(uint16_t type, uint32_t sn);

    template <typename... Args> int sendCmd(const uint8_t cmd, Args... args);
    int sendCmd(const uint8_t cmd, std::vector<uint8_t> &msgBytes, bool ackNeeded = true);

    template <typename... Args> int sendRequest(const uint8_t cmd, Args... args);
    int sendRequest(std::vector<uint8_t> &msgBytes, bool ackNeeded = true);

    inline void setId(uint16_t id) { _id = id; }
    inline uint16_t getId(void) { return _id; }
    inline void setSN(uint32_t sn) { _sn = sn; }
    inline uint32_t getSN(void) { return _sn; }
    inline uint16_t getType(void) { return _type; }

    inline void ledOn(LedColor_t color) { _setLed(LED_ON, color, 0); };
    inline void ledOff(void) { _setLed(LED_OFF, LED_NONE, 0); };
    inline void ledBlink(LedColor_t color, uint32_t period) { _setLed(LED_BLINK, color, period); };

    void restart(void);

private:
    uint16_t _id;   // Board id
    uint16_t _type; // Board
    uint32_t _sn;   // Serial number

    void _setLed(LedState_t state, LedColor_t color = LED_NONE, uint32_t period = 0);
};

#endif