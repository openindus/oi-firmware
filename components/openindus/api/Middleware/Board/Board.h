/**
 * @file Board.h
 * @brief Board interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <string.h>

/* Struct can fill an eFusSe block of 32 msgBytes */
typedef struct __attribute__((__packed__)) {
    uint8_t board_type;
    uint8_t hardware_variant;
    uint32_t serial_number;
    uint8_t reserved_1[4];
    int64_t timestamp; // Unix timestamp
    uint8_t reserved_2[13];
    uint8_t checksum;
} Board_eFuse_Info_t;

typedef struct {
    Board_eFuse_Info_t efuse;
    char software_version[32];
} Board_Info_t;

class Board
{
public:

    static int init(void);

    static void restart(void);
    static float getTemperature(void);    
    static uint8_t getBoardType(void);
    static uint8_t getHardwareVariant(void);
    static uint32_t getSerialNum(void);
    static int64_t getTimestamp(void);
    static void getSoftwareVersion(char software_version[32]);
    static bool setBoardInfo(uint8_t board_type, uint8_t hardware_variant, uint32_t serial_num, int64_t timestamp);

    /**
     * @brief Check if a boot error occured
     * 
     * @return true 
     * @return false 
     */
    static bool checkBootError(void);

private:

    static uint8_t _calculate_eFuse_checksum(uint8_t* data);
    static bool _verify_eFuse_checksum(Board_eFuse_Info_t info);
    static int _registerCLI(void);

    #if SOC_TEMP_SENSOR_SUPPORTED
    static void* _temp_sensor; // temperature_sensor_handle_t
    #endif

};

enum Board_Type_e {
    TYPE_OI_CORE            = (uint8_t) 3,
    TYPE_OI_CORELITE        = (uint8_t) 4,
    TYPE_OI_DISCRETE        = (uint8_t) 6,
    TYPE_OI_DISCRETE_VE     = (uint8_t) 7,
    TYPE_OI_MIXED           = (uint8_t) 8,
    TYPE_OI_RELAY_LP        = (uint8_t) 9,
    TYPE_OI_RELAY_HP        = (uint8_t) 10,
    TYPE_OI_STEPPER         = (uint8_t) 11,
    TYPE_OI_STEPPER_VE      = (uint8_t) 12,
    TYPE_OI_ANALOG_LS       = (uint8_t) 13,
    TYPE_OI_BRUSHLESS       = (uint8_t) 18,
    TYPE_OI_DC              = (uint8_t) 21,
};

class BoardUtils
{
public:

    static inline char* typeToName(uint8_t type, char* name) {
        switch (type) {
            case TYPE_OI_CORE:          strcpy(name, "OICore"); break;
            case TYPE_OI_CORELITE:      strcpy(name, "OICoreLite"); break;
            case TYPE_OI_DISCRETE:      strcpy(name, "OIDiscrete"); break;
            case TYPE_OI_DISCRETE_VE:   strcpy(name, "OIDiscreteVE"); break;
            case TYPE_OI_MIXED:         strcpy(name, "OIMixed");    break;
            case TYPE_OI_RELAY_LP:      strcpy(name, "OIRelayLP"); break;
            case TYPE_OI_RELAY_HP:      strcpy(name, "OIRelayHP"); break;
            case TYPE_OI_STEPPER:       strcpy(name, "OIStepper"); break;
            case TYPE_OI_STEPPER_VE:    strcpy(name, "OIStepperVE"); break;
            case TYPE_OI_ANALOG_LS:     strcpy(name, "OIAnalogLS"); break;
            case TYPE_OI_BRUSHLESS:     strcpy(name, "OIBrushless"); break;
            case TYPE_OI_DC:            strcpy(name, "OIDc"); break;
            default:                    strcpy(name, "Unknown"); break;
        }
        return name;
    }

    static inline bool areTypeCompatible(uint8_t type1, uint8_t type2) {
        // Types are equals
        if (type1 == type2) return true;

        // OICore and OICoreLite
        if ((type1 == TYPE_OI_CORE && type2 == TYPE_OI_CORELITE) || (type1 == TYPE_OI_CORELITE && type2 == TYPE_OI_CORE)) return true;

        // OICore and OICoreLite
        if ((type1 == TYPE_OI_DISCRETE && type2 == TYPE_OI_DISCRETE_VE) || (type1 == TYPE_OI_DISCRETE_VE && type2 == TYPE_OI_DISCRETE)) return true;

        // OICore and OICoreLite
        if ((type1 == TYPE_OI_STEPPER && type2 == TYPE_OI_STEPPER_VE) || (type1 == TYPE_OI_STEPPER_VE && type2 == TYPE_OI_STEPPER)) return true;

        // ELSE
        return false;
    }
};