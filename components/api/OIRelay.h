/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIRelay.h
 * @brief
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "OIModule.h"
#include "driver/gpio.h"

#include "OIType.h"

#if defined CONFIG_OI_RELAY_LP

/***************************************************************************************************/
/*-------------------------------------- OI RELAY LP----------------------------------------*/
/***************************************************************************************************/

#define OI_RELAY_LP_PHASE_CMD1 GPIO_NUM_38
#define OI_RELAY_LP_PHASE_CMD2 GPIO_NUM_37
#define OI_RELAY_LP_PHASE_CMD3 GPIO_NUM_36
#define OI_RELAY_LP_PHASE_CMD4 GPIO_NUM_35
#define OI_RELAY_LP_PHASE_CMD5 GPIO_NUM_34
#define OI_RELAY_LP_PHASE_CMD6 GPIO_NUM_33

#else

/***************************************************************************************************/
/*-------------------------------------- OI RELAY HP----------------------------------------*/
/***************************************************************************************************/

#define OI_RELAY_HP_PHASE_CMD1 GPIO_NUM_13
#define OI_RELAY_HP_PHASE_CMD2 GPIO_NUM_12
#define OI_RELAY_HP_PHASE_CMD3 GPIO_NUM_11
#define OI_RELAY_HP_PHASE_CMD4 GPIO_NUM_10

#define OI_RELAY_HP_TEMPSENSOR_ALERT GPIO_NUM_34
#define OI_RELAY_HP_TEMPSENSOR_ADDR 0x49

#endif

#define OIRELAY_PIN_CMD_MOSFET GPIO_NUM_2

  /*********************************************************************/
  /*                    MAPPING OIRELAYLP CONNECTOR                   */
  /*********************************************************************/

  /*********************************************************************/
  /*| 9V-30V |       |       |       |       |        |       | PHASE |*/
  /*|  VIN   | RELAY6| RELAY5| RELAY4| RELAY3| RELAY2 | RELAY1|  IN   |*/
  /*|--------|-------|-------|-------|-------|--------|-------|-------|*/
/***|        |       |       |       |       |        |       |       |***/
/* *|  GND   | NEUTRE| NEUTRE| NEUTRE| NEUTRE| NEUTRE | NEUTRE| NEUTRE|* */
/*************************************************************************/

  /*************************************************************************************/
  /*                            MAPPING OIRELAYHP CONNECTOR                           */
  /*************************************************************************************/

  /*************************************************************************************/
  /*|        | PHASE |       |       |       |        |       |       |       |       |*/
  /*| NEUTRE |  IN   | NEUTRE| RELAY1| NEUTRE| RELAY2 | NEUTRE| RELAY3| NEUTRE| RELAY4|*/
  /*|--------|-------|-------|-------|-------|--------|-------|-------|-------|-------|*/

#if (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP)

class OIRelay : public OIModuleMaster
{

    public:

    OIRelay() : OIModuleMaster() {}

    virtual void init(void);

    /**
     * @brief Enable or disable relay
     * 
     */
    void setOutput(Relay_t relay, uint8_t on_off);

    private:
    void _initRelays(void);

};

#else

class OIRelay : public OIModuleSlave
{

    OIRelay(uint8_t id) : OIModuleSlave(id) {}

    inline void setOutput(Relay_t relay, uint8_t on_off) {
        setMessage(OIMessage(CMD_SET_RELAY, _senderId, (uint16_t) relay, (uint32_t) on_off));
    }

};

#endif // (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP)