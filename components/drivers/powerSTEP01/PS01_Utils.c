#include "PS01_Utils.h"

/**********************************************************
 * @brief Convert the float formatted acceleration or
 * deceleration into respectively an ACCELERATION or DECELERATION register value
 * @param[in] steps_s2 the acceleration or deceleration as
 * steps/s^2, range 14.55 to 59590 steps/s^2
 * @retval The acceleration or deceleration as steps/tick^2
 **********************************************************/
uint16_t PS01_AccDec_Steps_s2_to_RegVal(float steps_s2)
{
  return ((uint16_t)(((float)(steps_s2)*0.068719476736f)+0.5f));
}

/**********************************************************
 * @brief Convert the ACCELERATION or DECELERATION register value into step/s^2
 * @param[in] regVal The ACCELERATION or DECELERATION register value
 * @retval The speed as steps/s
 **********************************************************/
float PS01_AccDec_RegVal_to_Steps_s2(uint32_t regVal)
{
  return (((float)(regVal))*14.5519152283f);
}

/**********************************************************
 * @brief Convert BEMF compensation slope to values for ST_SLP,
 * FN_SLP_ACC or FN_SLP_DEC register
 * @param[in] percentage BEMF compensation slope percentage,
 * range 0 to 0.4% (0.004) s/step
 * @retval value for ST_SLP, FN_SLP_ACC or FN_SLP_DEC register
 **********************************************************/
uint8_t PS01_BEMFslope_Perc_to_RegVal(float percentage)
{
  return ((uint8_t)(((float)(percentage)*637.5f)+0.5f));
}

/**********************************************************
 * @brief Convert values from ST_SLP, FN_SLP_ACC or
 * FN_SLP_DEC register to BEMF compensation slope percentage
 * @param[in] regVal The ST_SLP, FN_SLP_ACC or FN_SLP_DEC
 * register value
 * @retval BEMF compensation slope percentage
 **********************************************************/
float PS01_BEMFslope_RegVal_to_Perc(uint32_t regVal)
{
  return (((float)(regVal))*0.00156862745098f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a FS_SPD 
 * register value
 * @param[in] steps_s the speed as steps/s, range 15.25 to 15610 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
uint16_t PS01_FSSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)((float)(steps_s)*0.065536f));
}

/**********************************************************
 * @brief Convert the FS_SPD register value into step/s
 * @param[in] regVal The FS_SPD register value
 * @retval The full Step speed as steps/s
 **********************************************************/
float PS01_FSSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)regVal+0.999f)*15.258789f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a INT_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 976.5 steps/s
 * @retval The intersect speed as steps/tick
 **********************************************************/
uint16_t PS01_IntSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*16.777216f)+0.5f));
}

/**********************************************************
 * @brief Convert the INT_SPEED register value into step/s
 * @param[in] regVal The INT_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
float PS01_IntSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.0596045f);
}

/**********************************************************
 * @brief Convert the float formatted thermal compensation
 * factor into a K_THEM register value
 * @param[in] compFactor the float formatted thermal 
 * compensation factor, range 1 to 1.46875
 * @retval value for K_THERM register
 **********************************************************/
uint8_t PS01_KTherm_Comp_to_RegVal(float compFactor)
{
  return ((uint8_t)((((float)(compFactor)-1.0f)*32.0f)+0.5f));
}

/**********************************************************
 * @brief Convert the K_THERM register value into a float 
 * formatted thermal compensation factor
 * @param[in] regVal The K_THERM register value
 * @retval The float formatted thermal compensation factor
 **********************************************************/
float PS01_KTherm_RegVal_to_Comp(uint32_t regVal)
{
  return (((float)(regVal))*0.03125f+1);
}
/**********************************************************
 * @brief Convert voltage in percentage to values for KVAL_RUN,
 * KVAL_HOLD, KVAL_ACC or KVAL_DEC register
 * @param[in] percentage percentage of the power supply voltage
 * applied to the motor windings, range 0.4% to 99.6%
 * @retval value for KVAL_RUN, KVAL_HOLD, KVAL_ACC or
 * KVAL_DEC register
 * @note The voltage applied is sinusoidal
 **********************************************************/
uint8_t PS01_Kval_Perc_to_RegVal(float percentage)
{
  return ((uint8_t)(((float)(percentage)*2.56f)+0.5f));
}

/**********************************************************
 * @brief Convert values from KVAL_RUN, KVAL_HOLD, KVAL_ACC
 * or KVAL_DEC register to percentage
 * @param[in] regVal The KVAL_RUN, KVAL_HOLD, KVAL_ACC
 * or KVAL_DEC register value
 * @retval percentage of the power supply voltage applied to
 * the motor windings
 * @note The voltage applied is sinusoidal
 **********************************************************/
float PS01_Kval_RegVal_to_Perc(uint32_t regVal)
{
  return (((float)(regVal))*0.390625f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a MAX_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 15.25 to 15610 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
uint16_t PS01_MaxSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*0.065536f)+0.5f));
}

/**********************************************************
 * @brief Convert the MAX_SPEED register value into step/s
 * @param[in] regVal The MAX_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
float PS01_MaxSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*15.258789f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a MIN_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 976.3 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
uint16_t PS01_MinSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*4.194304f)+0.5f));
}

/**********************************************************
 * @brief Convert the MIN_SPEED register value into step/s
 * @param[in] regVal The MIN_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
float PS01_MinSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.238418579f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 15625 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
uint32_t PS01_Speed_Steps_s_to_RegVal(float steps_s)
{
  return ((uint32_t)(((float)(steps_s)*67.108864f)+0.5f));
}

/**********************************************************
 * @brief Convert the SPEED register value into step/s
 * @param[in] regVal The SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
float PS01_Speed_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.01490116119f);
}

/**********************************************************
 * @brief Convert STALL or OCD Threshold voltage in mV to 
 * values for STALL_TH or OCD_TH register
 * @param[in] mV voltage in mV, range 31.25mV to 1000mV
 * @retval value for STALL_TH or OCD_TH register
 **********************************************************/
uint8_t PS01_StallOcd_Th_to_RegVal(float mV)
{
  return ((uint8_t)((((float)(mV)-31.25f)*0.032f)+0.5f));
}

/**********************************************************
 * @brief Convert values from STALL_TH or OCD_TH register 
 * to mV
 * @param[in] regVal The STALL_TH or OCD_TH register value
 * @retval voltage in mV
 **********************************************************/
float PS01_StallOcd_RegVal_to_Th(uint32_t regVal)
{
  return (((float)(regVal+1))*31.25f);
}

/**********************************************************
 * @brief Convert voltage in mV to values for TVAL_RUN,
 * TVAL_HOLD, TVAL_ACC or TVAL_DEC register
 * @param[in] voltage_mV voltage in mV, range 7.8mV to 1000mV
 * @retval value for TVAL_RUN, TVAL_HOLD, TVAL_ACC or
 * TVAL_DEC register
 * @note The voltage corresponds to a peak output current
 * accross the external sense power resistor
 **********************************************************/
uint8_t PS01_Tval_RefVoltage_to_RegVal(float voltage_mV)
{
  return ((uint8_t)((((float)(voltage_mV)-7.8125f)*0.128f)+0.5f));
}

/**********************************************************
 * @brief Convert values from TVAL_RUN, TVAL_HOLD, TVAL_ACC
 * or TVAL_DEC register to mV
 * @param[in] regVal The TVAL_RUN, TVAL_HOLD, TVAL_ACC
 * or TVAL_DEC register value
 * @retval voltage in mV
 * @note The voltage corresponds to a peak output current
 * accross the external sense power resistor
 **********************************************************/
float PS01_Tval_RegVal_to_RefVoltage(uint32_t regVal)
{
  return (((float)(regVal+1))*7.8125f);
}

/**********************************************************
 * @brief Convert time in us to values for TON_MIN register
 * @param[in] tmin_us time in us, range 0.5us to 64us
 * @retval value for TON_MIN register
 **********************************************************/
uint8_t PS01_Tmin_Time_to_RegVal(float tmin_us)
{
  return ((uint8_t)((((float)(tmin_us)-0.5f)*2.0f)+0.5f));
}

/**********************************************************
 * @brief Convert values for TON_MIN or TOFF_MIN register to time in us
 * @param[in] regVal The TON_MIN or TOFF_MIN register value
 * @retval time in us
 **********************************************************/
float PS01_Tmin_RegVal_to_Time(uint32_t regVal)
{
  return (((float)(regVal+1))*0.5f);
}

/**********************************************************
 * @brief Convert time in us to values for T_FAST register
 * @param[in] t_fast_us time in us, range 2us to 32us
 * @retval value for tfast register
 **********************************************************/
uint8_t PS01_TFast_Time_to_RegVal(float t_fast_us)
{
  return ((uint8_t)(((float)(t_fast_us))/2.0f)-1);
}

/**********************************************************
 * @brief Convert values for Toff_fast and Fast_step register to time in us
 * @param[in] regVal The TOFF_FAST or FAST_STEP register value
 * @retval time in us
 **********************************************************/
float PS01_TFast_RegVal_to_Time(uint8_t regVal)
{
  return (((float)(regVal+1))*2);
}