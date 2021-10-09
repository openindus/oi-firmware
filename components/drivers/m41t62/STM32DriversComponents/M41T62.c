/**
******************************************************************************
* @file    M41T62.c
* @author  HESA Application Team
* @version 1.0.0
* @date    07/09/2015
* @brief   M41T62 driver file

* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*
* <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>

******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "M41T62.h"

#ifdef  USE_FULL_ASSERT_M41T62
#include <stdio.h>
#endif


/** @addtogroup Environmental_Sensor
* @{
*/

/** @defgroup M41T62_DRIVER
* @brief M41T62 DRIVER
* @{
*/


/** @defgroup M41T62_Public_Functions
* @{
*/
uint8_t DecimalToBCD(uint8_t x)
{
  uint8_t d,u;
  
  d=x/10;
  u=x-d*10;
  return(u+(d<<4));
}

/**
* @brief  Get the M41T62 driver version.
* @param  This parameter is a @ref M41T62_DriverVersion_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_DriverVersion(M41T62_DriverVersion_st *Version)
{  
  Version->Major = M41T62_DriverVersion_Major;
  Version->Minor = M41T62_DriverVersion_Minor;
  Version->Point = M41T62_DriverVersion_Point;
  
  return M41T62_OK;
}

/**
* @brief  Set Stop bit.
* @param  status can be M41T62_STOP_BIT_HIGH: Setting this bit to a '1' will cause the oscillator to stop.
* @param  status can be M41T62_STOP_BIT_LOW: When reset to a '0' the oscillator restarts within one second (typical).
*         This parameter is a @ref HM41T62_STOP_BIT_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_STOP_Bit(M41T62_STOP_BIT_State_et status)
{
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_STOP_BIT_STATE(status));
  
  if(M41T62_ReadRegs(M41T62_SECONDSREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_STOP_MASK;
  tmp |= ((uint8_t)status)<<M41T62_STOP_BIT;
  
  if(M41T62_WriteRegs(M41T62_SECONDSREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Get Stop bit.
* @param  Pointer to the returned value with updated stop bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_STOP_Bit(M41T62_STOP_BIT_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_SECONDSREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_STOP_BIT_State_et)((tmp & M41T62_STOP_MASK) >> M41T62_STOP_BIT);
  
  return M41T62_OK;  
}

/**
* @brief  Set Alarm flag enable bit.
* @param  status can be M41T62_AFE_LOW.
* @param  status can be M41T62_AFE_HIGH: If AFE is set, the alarm condition activates the IRQ/OUT pin.
*         This parameter is a @ref M41T62_AFE_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_AFE_Bit(M41T62_AFE_State_et status)
{
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_AFE_State(status));
  
  if(M41T62_ReadRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_AFE_MASK;
  tmp |= ((uint8_t)status)<<M41T62_AFE_BIT;
  
  if(M41T62_WriteRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Get Alarm Flag Enable bit.
* @param  Pointer to the returned value with updated Alarm Flag Enable bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_AFE_Bit(M41T62_AFE_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_AFE_State_et)((tmp & M41T62_AFE_MASK) >> M41T62_AFE_BIT);
  
  return M41T62_OK;  
}

/**
* @brief  Get Alarm Flag bit (read only).
* @param  Pointer to the returned value with updated Alarm Flag Enable bit status.
*         When the clock information matches the alarm clock settings based on the match criteria defined by RPT5�RPT1, the AF is set.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_AF_Bit(M41T62_AF_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_AF_State_et)((tmp & M41T62_AF_MASK) >> M41T62_AF_BIT);
  
  return M41T62_OK;  
}

/**
* @brief  Set Oscillator fail bit.
* @param  status can be: M41T62_OF_BIT_HIGH or M41T62_OF_BIT_LOW
*         If the oscillator fail (OF) bit is internally set to a '1,'
*         this indicates that the oscillator has either stopped, or was stopped for some period of time
*         This parameter is a @ref M41T62_OF_BIT_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_OF_Bit(M41T62_OF_BIT_State_et status)
{
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_OF_BIT_State(status));
  
  if(M41T62_ReadRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_OF_MASK;
  tmp |= ((uint8_t)status)<<M41T62_OF_BIT;
  
  if(M41T62_WriteRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Get Oscillator Fail bit OF.
* @param  Pointer to the returned value with updated Alarm Flag Enable bit status.
*         If the OF bit cannot be written to '0' four (4) seconds after the initial power-up, 
*         the STOP bit (ST) should be written to a '1,' then immediately reset to '0.'
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_OF_Bit(M41T62_OF_BIT_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_OF_BIT_State_et)((tmp & M41T62_OF_MASK) >> M41T62_OF_BIT);
  
  return M41T62_OK;  
}

/**
* @brief  Set Square Wave Enable bit.
* @param  status can be M41T62_SQWE_LOW: the SQW pin is turned off
* @param  status can be M41T62_SQWE_HIGH: the SQW pin is turned on 
*         This parameter is a @ref M41T62_SQWE_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_SQWE_Bit(M41T62_SQWE_State_et status)
{
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_SQWE_State(status));
  
  if(M41T62_ReadRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_SQWE_MASK;
  tmp |= ((uint8_t)status)<<M41T62_SQWE_BIT;
  
  if(M41T62_WriteRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Get Square Wave Enable bit.
* @param  Pointer to the returned value with updated Square Wave Enable bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_SQWE_Bit(M41T62_SQWE_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_SQWE_State_et)((tmp & M41T62_SQWE_MASK) >> M41T62_SQWE_BIT);
  
  return M41T62_OK;  
}

/**
* @brief Get Watchdog Flag bit (read only).
*        If the processor does not reset the timer within the specified period,
*        the M41T62 sets the WDF (watchdog flag) and generates an interrupt on the IRQ pin 
* @param  Pointer to the returned value with updated Watchdog Flag bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_WDF_Bit(M41T62_WDF_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_WDF_State_et)((tmp & M41T62_WDF_MASK) >> M41T62_WDF_BIT);
  
  return M41T62_OK;  
}

/**
* @brief  Set OUT bit.
*         When the OFIE bit, AFE bit, and watchdog register are not set to generate an interrupt, 
*         the IRQ/OUT pin becomes an output driver that reflects the contents of OUT bit. 
* @param  status can be M41T62_OUT_BIT_HIG or M41T62_OUT_BIT_LOW.
*         This parameter is a @ref M41T62_OUT_BIT_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_OUT_Bit(M41T62_OUT_BIT_State_et status)
{
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_OUT_BIT_State(status));
  
  if(M41T62_ReadRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_OUT_BIT_MASK;
  tmp |= ((uint8_t)status)<<M41T62_OUT_BIT;
  
  if(M41T62_WriteRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Get OUT bit.
* @param  Pointer to the returned value with updated OUT bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_OUT_Bit(M41T62_OUT_BIT_State_et* status)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  *status = (M41T62_OUT_BIT_State_et)((tmp & M41T62_OUT_BIT_MASK) >> M41T62_OUT_BIT);
  
  return M41T62_OK;  
}

/**
* @brief  Time setting.
* @param  Set hours, minutes, seconds and 10ths/100ths of seconds
*         This parameter is a @ref M41T62_Time_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Setting_Time(M41T62_Time_st time)
{
    uint8_t tmp[4];

    M41T62_assert_param(IS_M41T62_TIME(time));

    time.Hours=DecimalToBCD(time.Hours);
    time.Minutes=DecimalToBCD(time.Minutes);
    time.Seconds=DecimalToBCD(time.Seconds);
    time.Seconds_10Ths=DecimalToBCD(time.Seconds_10Ths);

    if(M41T62_ReadRegs(M41T62_10TH_100THREG, 4, tmp))
    {
      return M41T62_ERROR;
    }
    tmp[0] &= (uint8_t)~((M41T62_SECONDS_10THS_MASK | M41T62_SECONDS_100THS_MASK));
    tmp[0] |=time.Seconds_10Ths;

    
    tmp[1] &= ~((M41T62_SECONDS_MASK | M41T62_10SECONDS_MASK));
    tmp[1] |=time.Seconds;

    tmp[2] &= ~((M41T62_MINUTES_MASK | M41T62_10MINUTES_MASK));
    tmp[2] |=time.Minutes;

    tmp[3] &=~((M41T62_HOURS_MASK | M41T62_10HOURS_MASK));
    tmp[3] |=time.Hours;

    if(M41T62_WriteRegs(M41T62_10TH_100THREG, 4, tmp))
      return M41T62_ERROR;

    return M41T62_OK;
}



/**
* @brief  Get the time.
* @param  Pointer to the returned value  M41T62_Time_st with updated time.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Getting_Time(M41T62_Time_st* time)
{
  uint8_t tmp[4];
  uint8_t d,u;
  
  if(M41T62_ReadRegs(M41T62_10TH_100THREG, 4, tmp))
    return M41T62_ERROR;
 
  d = (tmp[0] & M41T62_SECONDS_10THS_MASK)>>M41T62_10THS_SECONDS_BIT;
  u = (tmp[0] & M41T62_SECONDS_100THS_MASK);
  time->Seconds_10Ths=u+10*d;
 
  d = (tmp[1] & M41T62_10SECONDS_MASK)>>M41T62_10SECONDS_BIT;
  u = (tmp[1] & M41T62_SECONDS_MASK);
  time->Seconds=u+10*d;
  
  d = (tmp[2] & M41T62_10MINUTES_MASK)>>M41T62_10MINUTES_BIT;
  u = (tmp[2] & M41T62_MINUTES_MASK);
  time->Minutes=u+10*d;
  
  d = (tmp[3] & M41T62_10HOURS_MASK)>>M41T62_10HOURS_BIT;
  u = (tmp[3] & M41T62_HOURS_MASK);
  time->Hours=u+10*d;
  
  return M41T62_OK;  
}

/**
* @brief  Date setting.
* @param  Set DayWeek, DayMonth, Month, Year and Century
*         This parameter is a @ref M41T62_Date_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Setting_Date(M41T62_Date_st Date)
{
    uint8_t tmp[4];
    
    M41T62_assert_param(IS_M41T62_DAYWEEK(Date.DayWeek));
    M41T62_assert_param(IS_M41T62_DAYMONTH(Date.DayMonth));
    M41T62_assert_param(IS_M41T62_MONTH(Date.Month));
    M41T62_assert_param(IS_M41T62_YEAR(Date.Year));
    M41T62_assert_param(IS_M41T62_CENTURY(Date.Century));

    Date.DayMonth=DecimalToBCD(Date.DayMonth);
    Date.Month=DecimalToBCD(Date.Month);
    Date.Year=DecimalToBCD(Date.Year);
    
    if(M41T62_ReadRegs(M41T62_DAYWEEKREG, 4, tmp))
      return M41T62_ERROR;
    
    tmp[0] &= ~(M41T62_DAYWEEK_MASK);
    tmp[0] |=Date.DayWeek;
  
    tmp[1] &= ~(M41T62_DAYMONTH_MASK | M41T62_10DAYMONTH_MASK);
    tmp[1] |=Date.DayMonth;
    
    tmp[2] &= ~((M41T62_MONTH_MASK | M41T62_10MONTH_MASK | M41T62_CENTURY_MASK));
    tmp[2] |=(Date.Month | (Date.Century<<M41T62_CENTURY));
 
     
    tmp[3] &=(uint8_t)~((M41T62_YEAR_MASK | M41T62_10YEAR_MASK));
    tmp[3] |=Date.Year;
    
      
    if(M41T62_WriteRegs(M41T62_DAYWEEKREG, 4, tmp))
      return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Get the data.
* @param  Pointer to the returned value  M41T62_Date_st with updated date.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Getting_Date(M41T62_Date_st* Date)
{
  uint8_t tmp[4];
  uint8_t d,u;
  
    if(M41T62_ReadRegs(M41T62_DAYWEEKREG, 4, tmp))
      return M41T62_ERROR;
  
    (*Date).DayWeek= tmp[0]& M41T62_DAYWEEK_MASK;
    
    d = (tmp[1] &  M41T62_10DAYMONTH_MASK)>>M41T62_10DAYMONTH_BIT;
    u = (tmp[1] & M41T62_DAYMONTH_MASK);
    (*Date).DayMonth=u+10*d;
  
    d = (tmp[2] & M41T62_10MONTH_MASK)>>M41T62_10MONTH_BIT;
    u = (tmp[2] & M41T62_MONTH_MASK);
    (*Date).Month=u+10*d;
    
    d = (tmp[3] & M41T62_10YEAR_MASK)>>M41T62_10YEAR_BIT;
    u = (tmp[3] & M41T62_YEAR_MASK);
    (*Date).Year=u+10*d;
    
    /*Anto*/
      u = (tmp[2] & M41T62_CENTURY_MASK)>>M41T62_CENTURY;
    (*Date).Century=u;

 return M41T62_OK;  
}

/**
* @brief  Alarm setting.
* @param  The alarm can be configured to go off at a prescribed time on a specific month, date, hour, minute, or second,
*         or repeat every year, month, day, hour, minute, or second
*         This parameter is a @ref M41T62_Alarm_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Alarm_Setting(M41T62_Alarm_st Alarm)
{
  uint8_t tmp[5];
     
    M41T62_assert_param(IS_M41T62_ALMONTH(Alarm.AlMonth));
    M41T62_assert_param(IS_M41T62_ALDATE(Alarm.AlDate));
    M41T62_assert_param(IS_M41T62_ALHOUR(Alarm.AlHour));
    M41T62_assert_param(IS_M41T62_ALMINUTES(Alarm.AlMinutes));
    M41T62_assert_param(IS_M41T62_ALSECONDS(Alarm.AlSeconds));
    
    Alarm.AlMonth=DecimalToBCD(Alarm.AlMonth);
    Alarm.AlDate=DecimalToBCD(Alarm.AlDate);
    Alarm.AlHour=DecimalToBCD(Alarm.AlHour);
    Alarm.AlMinutes=DecimalToBCD(Alarm.AlMinutes);
    Alarm.AlSeconds=DecimalToBCD(Alarm.AlSeconds);
    
    if(M41T62_ReadRegs(M41T62_AL_MONTHREG, 5, tmp))
      return M41T62_ERROR;
    
    tmp[0] &= ~(M41T62_ALMONTH10_MASK | M41T62_ALMONTH_MASK);
    tmp[0] |=Alarm.AlMonth;
  
    tmp[1] &= ~(M41T62_ALDATE10_MASK | M41T62_ALDATE_MASK);
    tmp[1] |=Alarm.AlDate;
    
    tmp[2] &= ~(M41T62_ALHOUR10_MASK | M41T62_ALHOUR_MASK);
    tmp[2] |=Alarm.AlHour;
     
    tmp[3] &= ~(M41T62_ALMINUTES10_MASK | M41T62_ALMINUTES_MASK);
    tmp[3] |=Alarm.AlMinutes;
    
    tmp[4] &= ~(M41T62_ALSECONDS10_MASK | M41T62_ALSECONDS_MASK);
    tmp[4] |=Alarm.AlSeconds;
      
    if(M41T62_WriteRegs(M41T62_AL_MONTHREG, 5, tmp))
      return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Alarm repeat mode setting.
* @param  Alarm repeat mode: Once per second, once per minute, once per hour, once per day, once per month, once per year.
*         RTC devices provide an alarm which can be set either for a given time and day, or to repeat at a certain day in every month, 
*         or at a certain hour in every day, or at a certain minute of every hour, or at a certain second of every minute
*         This parameter is a @ref M41T62_Alarm_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Alarm_Repeat_Mode_Setting(M41T62_AlarmRepeatMode_et AlRpMd)
{
   uint8_t tmp[4];
      
    M41T62_assert_param(IS_M41T62_AlarmRepeatMode(AlRpMd));
    if(M41T62_ReadRegs(M41T62_AL_DATEREG, 4, tmp))
      return M41T62_ERROR;
    
    tmp[0]&= ~(M41T62_RPT4_MASK|M41T62_RPT5_MASK);
    tmp[0]|=((AlRpMd<<3)&(M41T62_RPT4_MASK|M41T62_RPT5_MASK));
    
    tmp[1] &= ~(M41T62_RPT3_MASK);
    tmp[1]|=((AlRpMd<<5)&(M41T62_RPT3_MASK));
       
    tmp[2] &= ~(M41T62_RPT2_MASK);
    tmp[2]|=((AlRpMd<<6)&(M41T62_RPT2_MASK));
         
    tmp[3] &= ~(M41T62_RPT1_MASK);
    tmp[3]|=((AlRpMd<<7)&(M41T62_RPT1_MASK));
        
    if(M41T62_WriteRegs(M41T62_AL_DATEREG, 4, tmp))
      return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Alarm disable.
*         To disable the alarm, write '0' to the alarm date register and to RPT5�RPT1
* @param  void
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Disable_Alarm(void)
{
 uint8_t tmp[4];
    
    if(M41T62_ReadRegs(M41T62_AL_DATEREG, 4, tmp))
      return M41T62_ERROR;
    
   /*To disable the alarm, write '0' to the alarm date register and to RPT5�RPT1*/
    for(uint8_t i=0;i<4;i++)
      tmp[i]= 0;
  
 
    if(M41T62_WriteRegs(M41T62_AL_DATEREG, 4, tmp))
      return M41T62_ERROR;
    
    /* Read action to move the address point from the falgs registers*/
    if(M41T62_ReadRegs(M41T62_AL_DATEREG, 1, tmp))
      return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Watchdog timer setting.
* @param  Bits BMB4-BMB0 store a binary multiplier (0-31) and the three bits RB2-RB0 select the resolution
*         The amount of time-out is then determined to be the multiplication of the five-bit multiplier value with the resolution.
*         This parameter is a @ref M41T62_Watchdog_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Watchdog_Setting(M41T62_Watchdog_st Time)
{
  uint8_t tmp;
    
    M41T62_assert_param(IS_M41T62_WATCHDOG(Time));
    M41T62_assert_param(IS_M41T62_WATCHDOG_RESOLUTION(Time.RB));
    
    if(M41T62_ReadRegs(M41T62_WATCHDOGREG, 1, &tmp))
      return M41T62_ERROR;
    
    tmp &= ~(M41T62_WATCHDOG_BM_MASK);
    tmp |=(Time.BM<<M41T62_WATCHDOG_BM);
           
    tmp &= ~(M41T62_WATCHDOG_RB_MASK); 
    tmp |=Time.RB ; 
                   
    
    if(M41T62_WriteRegs(M41T62_WATCHDOGREG, 1, &tmp))
      return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Watchdog timer reset.
* @param  void
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Watchdog_Reset(void)
{
  uint8_t tmp=0;
    
   if(M41T62_WriteRegs(M41T62_WATCHDOGREG, 1, &tmp))
      return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Set to 0 default bit.
* @param This parameter is a @ref void.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Setting_DefaultBit(void)
{
  uint8_t tmp=0;
  
  //Set to 0 D6 and D7 reg03h
  if(M41T62_ReadRegs(M41T62_HOURSREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &=M41T62_HOURSREG_MASK;
  
  if(M41T62_WriteRegs(M41T62_HOURSREG, 1, &tmp))
    return M41T62_ERROR;

  
 //Set to 0 D3  reg04h
  if(M41T62_ReadRegs(M41T62_DAYWEEKREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &=M41T62_DAYWEEKREG_MASK;
  
  if(M41T62_WriteRegs(M41T62_DAYWEEKREG, 1, &tmp))
    return M41T62_ERROR;
  
  
  //Set to 0 D6 and D7 reg05h
  if(M41T62_ReadRegs(M41T62_DAYMONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &=(M41T62_10DAYMONTH_MASK|M41T62_DAYMONTH_MASK);
  
  if(M41T62_WriteRegs(M41T62_DAYMONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
      
  //Set to 0 D5 reg0Ah
  if(M41T62_ReadRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &=M41T62_AL_MONTHREG_MASK;
  
  if(M41T62_WriteRegs(M41T62_AL_MONTHREG, 1, &tmp))
    return M41T62_ERROR;
  
    //Set to 0 D6 reg0Ch
  if(M41T62_ReadRegs(M41T62_AL_HOURREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &=M41T62_AL_HOURREG_MASK;
  
  if(M41T62_WriteRegs(M41T62_AL_HOURREG, 1, &tmp))
    return M41T62_ERROR;
  
   //Set to 0 D0,D1,D3,D4 and D5 in  reg0Fh
  if(M41T62_ReadRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &=M41T62_FLAGSREG_MAK;
  
  if(M41T62_WriteRegs(M41T62_FLAGSREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief   Set the square wave output frequency
*          RS3-RS0 bits located in 04h establish the square wave output frequency.           
*          Once the selection of the SQW frequency has been completed, the SQW/FT pin can be turned on and off under software control with the square
*          wave enable bit (SQWE) located in register 0Ah. 
* @param  This parameter is a @ref M41T62_SquareWaveFrequency_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_SquareWaveFrequency(M41T62_SquareWaveFrequency_et frequency)
{
    uint8_t tmp;
  
    M41T62_assert_param(IS_M41T62_SquareWaveFrequency(frequency));

    
    if(M41T62_ReadRegs(M41T62_DAYWEEKREG, 1, &tmp))
        return M41T62_ERROR; 

    tmp &= ~M41T62_SQW_FREQUENCY_MASK; 

    tmp |= ((uint8_t)frequency);
   
    if(M41T62_WriteRegs(M41T62_DAYWEEKREG, 1, &tmp))
    return M41T62_ERROR;
  
   
  return M41T62_OK;
}

/**
* @brief  Get the square wave output frequency
* @param  Pointer to the returned value with updated square wavw output frequency.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41ST87W_Get_SquareWaveFrequency(M41T62_SquareWaveFrequency_et * frequency)
{
  uint8_t tmp;
  
  if(M41T62_ReadRegs(M41T62_DAYWEEKREG, 1, &tmp))
        return M41T62_ERROR; 
  
  *frequency = (M41T62_SquareWaveFrequency_et)(tmp & M41T62_SQW_FREQUENCY_MASK);
  
  return M41T62_OK;
}

/**
* @brief  Set the sign bit S to define the Sign of Calibration
*         Adding counts speeds the clock up (S is set a '1'), subtracting counts slows the clock down (S is set a '0').
* @param  Sign of Calibration:'1' indicates positive calibration, '0' indicates negative calibration
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_SignForCalibration(M41T62_SignCalibrationStatus_et sign)
{
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_CALIBRATION_SIGN(sign));
  
  if(M41T62_ReadRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_SIGN_CALIBRATION_MASK;
  tmp |= ((uint8_t)sign);
  
  if(M41T62_WriteRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}

/**
* @brief  Set the number of times pulses which are blanked (subtracted, negative calibration) or split (added, positive calibration).
*         The calibration bits occupy the five lower order bits (D4-D0) in the control register (08h).
* @param  Number of times pulses to subtract or to add (These bits can be set to represent any value between 0 and 31 in binary form.)
* @retval  Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_NumberCountsForCalibration(uint8_t numbercounts)
{
  
  uint8_t tmp;
  
  M41T62_assert_param(IS_M41T62_CountsCalibration(numbercounts));
  
  if(M41T62_ReadRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  tmp &= ~M41T62_COUNT_CALIBRATION_MASK;
  tmp |= ((uint8_t)numbercounts);
  
  if(M41T62_WriteRegs(M41T62_CALIBRATIONREG, 1, &tmp))
    return M41T62_ERROR;
  
  return M41T62_OK;
}


#ifdef USE_FULL_ASSERT_M41T62
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void M41T62_assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number */
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
