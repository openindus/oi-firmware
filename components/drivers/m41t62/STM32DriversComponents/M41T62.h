/**
******************************************************************************
* @file    M41T62_Driver.h
* @author  HESA Application Team
* @version 1.0.0
* @date    07/09/2015
* @brief   M41T62 driver header file

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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __M41T62_DRIVER__H
#define __M41T62_DRIVER__H

#include <stdint.h>

// the user must include the proper file where HAL_ReadReg and HAL_WriteReg are implemented
#include "m41t62/m41t62.h"
/* Uncomment the line below to expanse the "assert_param" macro in the drivers code */
#define USE_FULL_ASSERT_M41T62  

/* Exported macro ------------------------------------------------------------*/
#define   M41T62_BIT(x) ((uint8_t)x)

#ifdef  USE_FULL_ASSERT_M41T62

/**
* @brief  The assert_param macro is used for function's parameters check.
* @param  expr: If expr is false, it calls assert_failed function which reports 
*         the name of the source file and the source line number of the call 
*         that failed. If expr is true, it returns no value.
* @retval None
*/
#define M41T62_assert_param(expr) ((expr) ? (void)0 : M41T62_assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void M41T62_assert_failed(uint8_t* file, uint32_t line);
#else
#define M41T62_assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT_M41T62 */

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup M41T62_DRIVER 
* @{
*/

/* Exported Types -------------------------------------------------------------*/
/** @defgroup M41T62_Exported_Types
* @{
*/

/**
* @brief  Error code type. 
*/
typedef enum {M41T62_OK = (uint8_t)0, M41T62_ERROR = !M41T62_OK} M41T62_Error_et;

  /**
  * @brief  Bit status type. 
  */
  typedef enum {M41T62_RESET = (uint8_t)0, M41T62_SET = !M41T62_RESET} M41T62_BitStatus_et; 
#define IS_M41T62_BitStatus(MODE) ((MODE == M41T62_RESET) || (MODE == M41T62_SET))

/**
* @brief M41T62 STOP bit. 
*/
typedef enum {M41T62_STOP_BIT_HIGH = (uint8_t)1, M41T62_STOP_BIT_LOW = !M41T62_STOP_BIT_HIGH} M41T62_STOP_BIT_State_et; 
#define IS_M41T62_STOP_BIT_STATE(MODE) (MODE == (M41T62_STOP_BIT_HIGH) || (MODE == M41T62_STOP_BIT_LOW))

/**
* @brief  M41T62 Oscillator fail bit. 
*/
typedef enum {M41T62_OF_BIT_HIGH = (uint8_t)1, M41T62_OF_BIT_LOW = !M41T62_OF_BIT_HIGH} M41T62_OF_BIT_State_et; 
#define IS_M41T62_OF_BIT_State(MODE) ((MODE == M41T62_OF_BIT_HIGH) || (MODE == M41T62_OF_BIT_LOW))

/**
* @brief  M41T62 Alarm Flag (read only). 
*/
typedef enum {M41T62_AF_LOW = (uint8_t)0, M41T62_AF_HIGH = !M41T62_AF_LOW} M41T62_AF_State_et; 
#define IS_M41T62_AF_State(MODE) ((MODE == M41T62_AF_LOW) || (MODE == M41T62_AF_HIGH))

/**
* @brief  M41T62 WatchDog Flag bit (read only). 
*/
typedef enum {M41T62_WDF_LOW = (uint8_t)0, M41T62_WDF_HIGH = !M41T62_WDF_LOW} M41T62_WDF_State_et; 
#define IS_M41T62_WDF_State(MODE) ((MODE == M41T62_WDF_LOW) || (MODE == M41T62_WDF_HIGH))

/**
* @brief  M41T62 Alarm flag enable. 
*/
typedef enum {M41T62_AFE_LOW = (uint8_t)0, M41T62_AFE_HIGH = !M41T62_AFE_LOW} M41T62_AFE_State_et; 
#define IS_M41T62_AFE_State(MODE) ((MODE == M41T62_AFE_LOW) || (MODE == M41T62_AFE_HIGH))

/**
* @brief  M41T62 Square Wave Enable bit.
*/
typedef enum {M41T62_SQWE_LOW = (uint8_t)0, M41T62_SQWE_HIGH = !M41T62_SQWE_LOW} M41T62_SQWE_State_et; 
#define IS_M41T62_SQWE_State(MODE) ((MODE == M41T62_SQWE_LOW) || (MODE == M41T62_SQWE_HIGH))

/**
* @brief  M41T62 OUT bit.
*/
typedef enum {M41T62_OUT_BIT_LOW = (uint8_t)0, M41T62_OUT_BIT_HIGH = !M41T62_OUT_BIT_LOW} M41T62_OUT_BIT_State_et; 
#define IS_M41T62_OUT_BIT_State(MODE) ((MODE == M41T62_OUT_BIT_LOW) || (MODE == M41T62_OUT_BIT_HIGH))

/**
* @brief M41T62  Sign Calibration status. 
*/
  typedef enum {M41T62_NEGATIVE_SIGN = (uint8_t)0, M41T62_POSITIVE_SIGN = !M41T62_NEGATIVE_SIGN} M41T62_SignCalibrationStatus_et; 
#define IS_M41T62_CALIBRATION_SIGN(MODE) ((MODE == M41T62_NEGATIVE_SIGN) || (MODE == M41T62_POSITIVE_SIGN))


/**
* @brief  M41T62 Driver Version Info structure definition. 
*/   
typedef struct {
  uint8_t   Major;
  uint8_t   Minor;
  uint8_t Point; 
}M41T62_DriverVersion_st; 

/**
* @brief  M41T62 Alarm repeat mode.
*/
typedef enum {
  M41T62_OnceperSecond = (uint8_t)31,
  M41T62_OnceperMinute = (uint8_t)30,
  M41T62_OnceperHour   = (uint8_t)28,
  M41T62_OnceperDay    = (uint8_t)24,
  M41T62_OnceperMonth  = (uint8_t)16,
  M41T62_OnceperYear   = (uint8_t)0
} M41T62_AlarmRepeatMode_et; 
#define IS_M41T62_AlarmRepeatMode(MODE) ((MODE == M41T62_OnceperSecond)||(MODE == M41T62_OnceperMinute)||(MODE == M41T62_OnceperHour)||(MODE == M41T62_OnceperDay)||(MODE == M41T62_OnceperMonth)||(MODE == M41T62_OnceperYear))


/**
* @brief  M41T62 Time structure definition. 
*/   
typedef struct
{
  uint8_t  Hours;          /*Hours 00-23*/
  uint8_t  Minutes;       /*Minutes 00-59 */
  uint8_t Seconds;        /*Seconds 00-59*/
  uint8_t Seconds_10Ths; /*Seconds 00-99*/
 }M41T62_Time_st;
#define IS_M41T62_TIME(TIME) ((00<= TIME.Hours) && (TIME.Hours<= 23)) && ((00<= TIME.Minutes) && (TIME.Minutes<= 59)) && ((00<= TIME.Seconds) && (TIME.Seconds<= 59)) && ((00<= TIME.Seconds_10Ths) &&(TIME.Seconds_10Ths<= 99))


/**
* @brief  M41T62 Date structure definition. 
*/   
typedef struct
{
  uint8_t      DayWeek;       /*Day of week  01-7*/
  uint8_t      DayMonth;      /*Day of month 01-31*/
  uint8_t      Month ;        /*Month  01-12*/
  uint8_t      Year;          /*Year 00-99*/
  uint8_t      Century;       /*Century 0-3*/
}M41T62_Date_st;

#define IS_M41T62_DAYWEEK(Date) ((01<= Date) && (Date<= 07))
#define IS_M41T62_DAYMONTH(Date) ((01<= Date) && (Date<= 31))
#define IS_M41T62_MONTH(Date) ((01<= Date) && (Date<= 12))
#define IS_M41T62_YEAR(Date) ((01<= Date) && (Date<= 99))
#define IS_M41T62_CENTURY(Date) ((00<= Date) && (Date<= 03))


/**
* @brief  M41T62 Alarm structure definition. 
*/   
typedef struct
{
  uint8_t      AlMonth;     /*Alarm month  01-12*/
  uint8_t      AlDate;      /*Day of month 01-31*/
  uint8_t      AlHour ;     /*Hour 00-23*/
  uint8_t      AlMinutes;   /*Minutes 00-59*/
  uint8_t      AlSeconds;   /*Seconds 00-59*/
}M41T62_Alarm_st;

#define IS_M41T62_ALMONTH(Alarm) ((00<= Alarm) && (Alarm<= 12))
#define IS_M41T62_ALDATE(Alarm) ((00<= Alarm) && (Alarm<= 31))
#define IS_M41T62_ALHOUR(Alarm) ((00<= Alarm) && (Alarm<= 23))
#define IS_M41T62_ALMINUTES(Alarm) ((00<= Alarm) && (Alarm<= 59))
#define IS_M41T62_ALSECONDS(Alarm) ((00<= Alarm) && (Alarm<= 59))
  

/**
* @brief  Resolution bit Configuration RB2-RB0.
*/
typedef enum {  
  
  M41T62_RB_00625_SEC  = (uint8_t)0x00,         /*!< Resolution: 0.0625 sec */
  M41T62_RB_025_SEC    = (uint8_t)0x01,         /*!< Resolution: 0.25 sec */
  M41T62_RB_1_SEC      = (uint8_t)0x02,        /*!< Resolution: 1 sec */
  M41T62_RB_4_SEC      = (uint8_t)0x03,         /*!< Resolution: 4 sec */
  M41T62_RB_1_MIN      = (uint8_t)0x80         /*!< Resolution: 1 min */
} M41T62_RB_et;

#define IS_M41T62_WATCHDOG_RESOLUTION(MODE) ((MODE == M41T62_RB_00625_SEC)||(MODE == M41T62_RB_025_SEC)||(MODE == M41T62_RB_1_SEC)||(MODE == M41T62_RB_4_SEC)||(MODE == M41T62_RB_1_MIN))
/**
* @brief  M41T62 Watchdog type def. definition. 
*/
typedef struct
{
  uint8_t      BM;     /*Binary multiplier bit*/
  M41T62_RB_et   RB;     /*Resolution bit*/
}M41T62_Watchdog_st;
#define IS_M41T62_WATCHDOG(Time) ((0<= Time.BM) && (Time.BM<=31))


/**
* @brief   Square Wave output frequency
*/
typedef enum {  

  M41T62_SquareWave_32KHz  = (uint8_t)0x10,         /*!<32.768 KHz*/
  M41T62_SquareWave_8KHz  = (uint8_t)0x20,         /*!< 8.192 KHz*/ 
  M41T62_SquareWave_4KHz  = (uint8_t)0x30,         /*!< 4.096 KHz*/
  M41T62_SquareWave_2KHz  = (uint8_t)0x40,         /*!< 2.048 KHz*/
  M41T62_SquareWave_1KHz  = (uint8_t)0x50,         /*!< 1.024 KHz*/
  M41T62_SquareWave_512Hz  =(uint8_t)0x60,         /*!< 512 Hz*/
  M41T62_SquareWave_256Hz  =(uint8_t)0x70,         /*!< 256 Hz*/
  M41T62_SquareWave_128Hz  =(uint8_t)0x80,         /*!< 128 Hz*/
  M41T62_SquareWave_64Hz  =(uint8_t)0x90,          /*!< 64 Hz*/
  M41T62_SquareWave_32Hz  =(uint8_t)0xA0,         /*!< 32 Hz*/
  M41T62_SquareWave_16Hz  =(uint8_t)0xB0,         /*!< 16 Hz*/
  M41T62_SquareWave_8Hz  =(uint8_t)0xC0,         /*!< 8 Hz*/
  M41T62_SquareWave_4Hz  =(uint8_t)0xD0,         /*!< 4 Hz*/
  M41T62_SquareWave_2Hz  =(uint8_t)0xE0,         /*!< 2 Hz*/
  M41T62_SquareWave_1Hz  =(uint8_t)0xF0         /*!< 1 Hz*/  
} M41T62_SquareWaveFrequency_et;

#define IS_M41T62_SquareWaveFrequency(MODE) ((MODE == M41T62_SquareWave_32KHz) || (MODE == M41T62_SquareWave_8KHz)|| (MODE == M41T62_SquareWave_4KHz) || (MODE == M41T62_SquareWave_2KHz) || \
                                               (MODE == M41T62_SquareWave_1KHz) || (MODE == M41T62_SquareWave_512Hz)|| (MODE == M41T62_SquareWave_256Hz) || (MODE == M41T62_SquareWave_128Hz) || \
                                               (MODE == M41T62_SquareWave_64Hz) || (MODE == M41T62_SquareWave_32Hz)|| (MODE == M41T62_SquareWave_16Hz) || (MODE == M41T62_SquareWave_8Hz) || \
                                               (MODE == M41T62_SquareWave_4Hz) || (MODE == M41T62_SquareWave_2Hz)|| (MODE == M41T62_SquareWave_1Hz))


#define IS_M41T62_CountsCalibration(Number) ((Number>=1) && (Number <=31))
/**
* @}
*/


/* Exported Constants ---------------------------------------------------------*/   
/** @defgroup M41T62_Exported_Constants
* @{
*/

/**
* @brief  M41T62 Registers map.
*/
#define M41T62_10TH_100THREG             (uint8_t)0x00
#define M41T62_SECONDSREG                (uint8_t)0x01
#define M41T62_MINUTESREG                (uint8_t)0x02
#define M41T62_HOURSREG                  (uint8_t)0x03
#define M41T62_DAYWEEKREG                (uint8_t)0x04
#define M41T62_DAYMONTHREG               (uint8_t)0x05
#define M41T62_CENTURY_MONTHREG          (uint8_t)0x06
#define M41T62_YEARREG                   (uint8_t)0x07
#define M41T62_CALIBRATIONREG            (uint8_t)0x08
#define M41T62_WATCHDOGREG               (uint8_t)0x09
#define M41T62_AL_MONTHREG               (uint8_t)0x0A
#define M41T62_AL_DATEREG                (uint8_t)0x0B
#define M41T62_AL_HOURREG                (uint8_t)0x0C
#define M41T62_AL_MINREG                 (uint8_t)0x0D
#define M41T62_AL_SECREG                 (uint8_t)0x0E
#define M41T62_FLAGSREG                  (uint8_t)0x0F

/**
* @brief  Bitfield positioning.
*/
#define M41T62_WDF_BIT                 M41T62_BIT(7)
#define M41T62_AF_BIT                  M41T62_BIT(6)
#define M41T62_OF_BIT                  M41T62_BIT(2)
#define M41T62_AFE_BIT                 M41T62_BIT(7)
#define M41T62_SQWE_BIT                M41T62_BIT(6)
#define M41T62_STOP_BIT                M41T62_BIT(7)
#define M41T62_OUT_BIT                 M41T62_BIT(7)
#define M41T62_10SECONDS_BIT           M41T62_BIT(4)
#define M41T62_10THS_SECONDS_BIT       M41T62_BIT(4)
#define M41T62_10MINUTES_BIT           M41T62_BIT(4)
#define M41T62_10HOURS_BIT             M41T62_BIT(4)
#define M41T62_10DAYMONTH_BIT          M41T62_BIT(4)
#define M41T62_10MONTH_BIT             M41T62_BIT(4)
#define M41T62_10YEAR_BIT              M41T62_BIT(4)
#define M41T62_CENTURY                 M41T62_BIT(6)
#define M41T62_WATCHDOG_BM             M41T62_BIT(2)


/**
* @brief  Time position.
*/
#define M41T62_SECONDS_10THS_MASK     0xF0
#define M41T62_SECONDS_100THS_MASK    0x0F
#define M41T62_SECONDS_MASK           0x0F
#define M41T62_10SECONDS_MASK         0x70
#define M41T62_MINUTES_MASK           0x0F
#define M41T62_10MINUTES_MASK         0x70
#define M41T62_HOURS_MASK             0x0F
#define M41T62_10HOURS_MASK           0x30


/**
* @brief  Date position.
*/
#define M41T62_DAYWEEK_MASK           0x07
#define M41T62_DAYMONTH_MASK          0x0F
#define M41T62_10DAYMONTH_MASK        0x30
#define M41T62_MONTH_MASK             0x0F
#define M41T62_10MONTH_MASK           0x10
#define M41T62_YEAR_MASK              0x0F
#define M41T62_10YEAR_MASK            0xF0
#define M41T62_CENTURY_MASK           0xC0


/**
* @brief  Alarm bit position.
*/
#define M41T62_ALMONTH_MASK            0x0F
#define M41T62_ALMONTH10_MASK          0x10
#define M41T62_ALDATE_MASK             0x0F
#define M41T62_ALDATE10_MASK           0x30
#define M41T62_ALHOUR_MASK             0x0F
#define M41T62_ALHOUR10_MASK           0x3F
#define M41T62_ALMINUTES_MASK          0x0F
#define M41T62_ALMINUTES10_MASK        0x70
#define M41T62_ALSECONDS_MASK          0x0F
#define M41T62_ALSECONDS10_MASK        0x70


/**
* @brief  Default bit (0) position 
*/
#define M41T62_HOURSREG_MASK          0x3F
#define M41T62_DAYWEEKREG_MASK        0xF7
#define M41T62_AL_MONTHREG_MASK       0xDF
#define M41T62_AL_HOURREG_MASK        0xBF
#define M41T62_FLAGSREG_MAK           0xC4

/**
* @brief  Watchdog bit position.
*/
#define M41T62_WATCHDOG_BM_MASK        0x7C
#define M41T62_WATCHDOG_RB_MASK        0x83

/**
* @brief  Flags bit position.
*/
#define M41T62_WDF_MASK               (uint8_t)0x80
#define M41T62_AF_MASK                (uint8_t)0x40
#define M41T62_OF_MASK                (uint8_t)0x04

/**
* @brief  STOP bit position.
*/
#define M41T62_STOP_MASK              (uint8_t)0x80

/**
* @brief  Output level bit position.
*/
#define M41T62_OUT_BIT_MASK           (uint8_t)0x80

/**
* @brief  Alarm repeat mode bit.
*/
#define M41T62_RPT1_MASK               (uint8_t)0x80
#define M41T62_RPT2_MASK               (uint8_t)0x80
#define M41T62_RPT3_MASK               (uint8_t)0x80
#define M41T62_RPT4_MASK               (uint8_t)0x80
#define M41T62_RPT5_MASK               (uint8_t)0x40
#define M41T62_AFE_MASK                (uint8_t)0x80

/**
* @brief Square Wave Output position
*/
#define M41T62_SQW_FREQUENCY_MASK     (uint8_t)0xF0
#define M41T62_SQWE_MASK              (uint8_t)0x40

/**
* @brief Calibration bit position
*/
#define M41T62_COUNT_CALIBRATION_MASK (uint8_t)0x1F
#define M41T62_SIGN_CALIBRATION_MASK  (uint8_t)0x20

/**
* @brief  M41T62 I2C slave address.
*/
#define M41T62_I2C_ADDRESS            (uint8_t)0xD0

/**
* @brief  Driver version.
*/
#define M41T62_DriverVersion_Major (uint8_t)1
#define M41T62_DriverVersion_Minor (uint8_t)0
#define M41T62_DriverVersion_Point (uint8_t)0


/**
* @}
*/

/* Exported Functions -------------------------------------------------------------*/
/** @defgroup M41T62_Exported_Functions
* @{
*/

/**
* @brief  Read M41T62 Registers
* @param  uint32_t RegAddr:       address of the first register to read
* @param  uint32_t NumByteToRead:   number of registers to read
* @param  uint8_t *Data:          pointer to the destination data buffer
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
// the user must redefine the proper M41T62_ReadRegs
#define M41T62_ReadRegs(RegAddr, NumByteToRead, Data)  HAL_ReadReg(M41T62_I2C_ADDRESS, RegAddr, NumByteToRead, Data)

/**
* @brief  Write M41T62 Registers
* @param  uint32_t RegAddr:      address of the register to write
* @param  uint8_t *Data:         pointer to the source data buffer
* @param  uint32_t NumByteToWrite:           Number of bytes to write
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
// the user must redefine the proper M41T62_WriteRegs
#define M41T62_WriteRegs(RegAddr, NumByteToWrite, Data)  HAL_WriteReg(M41T62_I2C_ADDRESS, RegAddr, NumByteToWrite, Data)


uint8_t DecimalToBCD(uint8_t);


/**
* @brief  Get the M41T62 driver version.
* @param  This parameter is a @ref M41T62_DriverVersion_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_DriverVersion(M41T62_DriverVersion_st *Version);

/**
* @brief  Set Stop bit.
* @param  status can be M41T62_STOP_BIT_HIGH: Setting this bit to a '1' will cause the oscillator to stop.
* @param  status can be M41T62_STOP_BIT_LOW: When reset to a '0' the oscillator restarts within one second (typical).
*         This parameter is a @ref HM41T62_STOP_BIT_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_STOP_Bit(M41T62_STOP_BIT_State_et);
/**
* @brief  Get Stop bit.
* @param  Pointer to the returned value with updated stop bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_STOP_Bit(M41T62_STOP_BIT_State_et*);

/**
* @brief  Set Alarm flag enable bit.
* @param  status can be M41T62_AFE_LOW.
* @param  status can be M41T62_AFE_HIGH: If AFE is set, the alarm condition activates the IRQ/OUT pin.
*         This parameter is a @ref M41T62_AFE_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_AFE_Bit(M41T62_AFE_State_et);

/**
* @brief  Get Alarm Flag Enable bit.
* @param  Pointer to the returned value with updated Alarm Flag Enable bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_AFE_Bit(M41T62_AFE_State_et*);

/**
* @brief  Get Alarm Flag bit (read only).
* @param  Pointer to the returned value with updated Alarm Flag Enable bit status.
*         When the clock information matches the alarm clock settings based on the match criteria defined by RPT5�RPT1, the AF is set.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_AF_Bit(M41T62_AF_State_et*); //read only


/**
* @brief  Set Oscillator fail bit.
* @param  status can be: M41T62_OF_BIT_HIGH or M41T62_OF_BIT_LOW
*         If the oscillator fail (OF) bit is internally set to a '1,'
*         this indicates that the oscillator has either stopped, or was stopped for some period of time
*         This parameter is a @ref M41T62_OF_BIT_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_OF_Bit(M41T62_OF_BIT_State_et status);

/**
* @brief  Get Oscillator Fail bit OF.
* @param  Pointer to the returned value with updated Alarm Flag Enable bit status.
*         If the OF bit cannot be written to '0' four (4) seconds after the initial power-up, 
*         the STOP bit (ST) should be written to a '1,' then immediately reset to '0.'
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_OF_Bit(M41T62_OF_BIT_State_et*);

/**
* @brief  Set Square Wave Enable bit.
* @param  status can be M41T62_SQWE_LOW: the SQW pin is turned off
* @param  status can be M41T62_SQWE_HIGH: the SQW pin is turned on 
*         This parameter is a @ref M41T62_SQWE_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_SQWE_Bit(M41T62_SQWE_State_et);

/**
* @brief  Get Square Wave Enable bit.
* @param  Pointer to the returned value with updated Square Wave Enable bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_SQWE_Bit(M41T62_SQWE_State_et*);

/**
* @brief Get Watchdog Flag bit (read only).
*        If the processor does not reset the timer within the specified period,
*        the M41T62 sets the WDF (watchdog flag) and generates an interrupt on the IRQ pin 
* @param  Pointer to the returned value with updated Watchdog Flag bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_WDF_Bit(M41T62_WDF_State_et*); //read only

/**
* @brief  Set OUT bit.
*         When the OFIE bit, AFE bit, and watchdog register are not set to generate an interrupt, 
*         the IRQ/OUT pin becomes an output driver that reflects the contents of OUT bit. 
* @param  status can be M41T62_OUT_BIT_HIG or M41T62_OUT_BIT_LOW.
*         This parameter is a @ref M41T62_OUT_BIT_State_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_OUT_Bit(M41T62_OUT_BIT_State_et);

/**
* @brief  Get OUT bit.
* @param  Pointer to the returned value with updated OUT bit status.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Get_OUT_Bit(M41T62_OUT_BIT_State_et*);

/**
* @brief  Time setting.
* @param  Set hours, minutes, seconds
*         This parameter is a @ref M41T62_Time_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Setting_Time(M41T62_Time_st);

/**
* @brief  Get the time.
* @param  Pointer to the returned value  M41T62_Time_st with updated time.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Getting_Time(M41T62_Time_st*);

/**
* @brief  Date setting.
* @param  Set DayWeek, DayMonth, Month, Year and Century
*         This parameter is a @ref M41T62_Date_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Setting_Date(M41T62_Date_st);


/**
* @brief  Get the data.
* @param  Pointer to the returned value  M41T62_Date_st with updated date.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Getting_Date(M41T62_Date_st*);
/**
* @brief  Alarm setting.
* @param  The alarm can be configured to go off at a prescribed time on a specific month, date, hour, minute, or second,
*         or repeat every year, month, day, hour, minute, or second
*         This parameter is a @ref M41T62_Alarm_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Alarm_Setting(M41T62_Alarm_st);

/**
* @brief  Alarm repeat mode setting.
* @param  Alarm repeat mode: Once per second, once per minute, once per hour, once per day, once per month, once per year.
*         RTC devices provide an alarm which can be set either for a given time and day, or to repeat at a certain day in every month, 
*         or at a certain hour in every day, or at a certain minute of every hour, or at a certain second of every minute
*         This parameter is a @ref M41T62_Alarm_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Alarm_Repeat_Mode_Setting(M41T62_AlarmRepeatMode_et);
/**
* @brief  Alarm disable.
*         To disable the alarm, write '0' to the alarm date register and to RPT5�RPT1
* @param  void
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Disable_Alarm(void);

/**
* @brief  Watchdog timer setting.
* @param  Bits BMB4-BMB0 store a binary multiplier (0-31) and the three bits RB2-RB0 select the resolution
*         The amount of time-out is then determined to be the multiplication of the five-bit multiplier value with the resolution.
*         This parameter is a @ref M41T62_Watchdog_st.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Watchdog_Setting(M41T62_Watchdog_st);

/**
* @brief  Watchdog timer reset.
* @param  void
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Watchdog_Reset(void);

/**
* @brief   Set the square wave output frequency
*          RS3-RS0 bits located in 04h establish the square wave output frequency.           
*          Once the selection of the SQW frequency has been completed, the SQW/FT pin can be turned on and off under software control with the square
*          wave enable bit (SQWE) located in register 0Ah. 
* @param  This parameter is a @ref M41T62_SquareWaveFrequency_et.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_SquareWaveFrequency(M41T62_SquareWaveFrequency_et frequency);

/**
* @brief  Get the square wave output frequency
* @param  Pointer to the returned value with updated square wavw output frequency.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41ST87W_Get_SquareWaveFrequency(M41T62_SquareWaveFrequency_et * frequency);

/**
* @brief  Set the sign bit S to define the Sign of Calibration
*         Adding counts speeds the clock up (S is set a '1'), subtracting counts slows the clock down (S is set a '0').
* @param  Sign of Calibration:'1' indicates positive calibration, '0' indicates negative calibration
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_SignForCalibration(M41T62_SignCalibrationStatus_et sign);

/**
* @brief  Set the number of times pulses which are blanked (subtracted, negative calibration) or split (added, positive calibration).
*         The calibration bits occupy the five lower order bits (D4-D0) in the control register (08h).
* @param  Number of times pulses to subtract or to add (These bits can be set to represent any value between 0 and 31 in binary form.)
* @retval  Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Set_NumberCountsForCalibration(uint8_t numbercounts);

/**
* @brief  Set to 0 default bit.
* @param This parameter is a @ref void.
* @retval Error code [M41T62_OK, M41T62_ERROR].
*/
M41T62_Error_et M41T62_Setting_DefaultBit(void);



#ifdef __cplusplus
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

#endif /* __M41T62_DRIVER__H */

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
