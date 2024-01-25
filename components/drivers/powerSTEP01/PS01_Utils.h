/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01_utils.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "PS01_Global.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Convert the float formatted acceleration or deceleration into respectively an ACC or DEC register value */
uint16_t PS01_AccDec_Steps_s2_to_RegVal(float steps_s2);

/* Convert the ACC or DEC register value into step/s^2 */
float PS01_AccDec_RegVal_to_Steps_s2(uint32_t regVal);

/* Convert BEMF compensation slope to values for ST_SLP, FN_SLP_ACC or FN_SLP_DEC register */
uint8_t PS01_BEMFslope_Perc_to_RegVal(float percentage);

/* Convert values from ST_SLP, FN_SLP_ACC or FN_SLP_DEC register to BEMF compensation slope percentage */
float PS01_BEMFslope_RegVal_to_Perc(uint32_t regVal);

/* Convert the float formatted speed into a FS_SPD register value */
uint16_t PS01_FSSpd_Steps_s_to_RegVal(float steps_s);

/* Convert the FS_SPD register value into step/s */
float PS01_FSSpd_RegVal_to_Steps_s(uint32_t regVal);

/* Convert the float formatted speed into a INT_SPEED register value */
uint16_t PS01_IntSpd_Steps_s_to_RegVal(float steps_s);

/* Convert the INT_SPEED register value into step/s */
float PS01_IntSpd_RegVal_to_Steps_s(uint32_t regVal);

/* Convert the float formatted thermal compensation factor into a K_THEM register value */
uint8_t PS01_KTherm_Comp_to_RegVal(float compFactor);

/* Convert the K_THERM register value into a float formatted thermal compensation factor */
float PS01_KTherm_RegVal_to_Comp(uint32_t regVal);

/* Convert voltage in percentage to values for KVAL_RUN, KVAL_HOLD, KVAL_ACC or KVAL_DEC register */
uint8_t PS01_Kval_Perc_to_RegVal(float percentage);

/* Convert values from KVAL_RUN, KVAL_HOLD, KVAL_ACC or KVAL_DEC register to percentage */
float PS01_Kval_RegVal_to_Perc(uint32_t regVal);

/* Convert the float formatted speed into a MAX_SPEED register value */
uint16_t PS01_MaxSpd_Steps_s_to_RegVal(float steps_s);

/* Convert the MAX_SPEED register value into step/s */
float PS01_MaxSpd_RegVal_to_Steps_s(uint32_t regVal);

/* Convert the float formatted speed into a MIN_SPEED register value */
uint16_t PS01_MinSpd_Steps_s_to_RegVal(float steps_s);

/* Convert the MIN_SPEED register value into step/s */
float PS01_MinSpd_RegVal_to_Steps_s(uint32_t regVal);

/* Convert the float formatted speed into a SPEED register value */
uint32_t PS01_Speed_Steps_s_to_RegVal(float steps_s);

/* Convert the SPEED register value into step/s */
float PS01_Speed_RegVal_to_Steps_s(uint32_t regVal);

/* Converts STALL or OCD Threshold voltage in mV to values for STALL_TH or OCD_TH register */
uint8_t PS01_StallOcd_Th_to_RegVal(float mV);

/* Convert values from STALL_TH or OCD_TH register to mV */
float PS01_StallOcd_RegVal_to_Th(uint32_t regVal);

/* Convert voltage in mV to values for TVAL_RUN, TVAL_HOLD, TVAL_ACC or TVAL_DEC register */
uint8_t PS01_Tval_RefVoltage_to_RegVal(float voltage_mV);

/* Convert values from TVAL_RUN, TVAL_HOLD, TVAL_ACC or TVAL_DEC register to mV */
float PS01_Tval_RegVal_to_RefVoltage(uint32_t regVal);

/* Convert time in us to values for TON_MIN register */
uint8_t PS01_Tmin_Time_to_RegVal(float tmin_us);

/* Convert values for TON_MIN or TOFF_MIN register to time in us */
float PS01_Tmin_RegVal_to_Time(uint32_t regVal);

uint8_t PS01_TFast_Time_to_RegVal(float t_fast_us);

float PS01_TFast_RegVal_to_Time(uint8_t regVal);

#ifdef __cplusplus
}
#endif