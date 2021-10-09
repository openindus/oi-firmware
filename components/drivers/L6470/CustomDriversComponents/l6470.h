/**************************************************************************//**
  * @file    powerstep01.h 
  * @author  IPC Rennes
  * @version V1.6.0
  * @date    June 4th, 2018
  * @brief   Header for L6470 motor driver (Microstepping controller with power MOSFETs)
  * @note    (C) COPYRIGHT 2016 STMicroelectronics
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _L6470_H_INCLUDED
#define _L6470_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "L6470/CustomDriversComponents/l6470_target_config.h"
#include "powerSTEP01/STM32DriversComponents/motor.h"
#include "stdint.h"
   
/** @addtogroup BSP
  * @{
  */
   
/** @addtogroup L6470
  * @{
  */

/* Exported Constants --------------------------------------------------------*/

/** @addtogroup L6470_Exported_Constants
  * @{
  */

/// Current FW major version
#define L6470_FW_MAJOR_VERSION (uint8_t)(1)
/// Current FW minor version
#define L6470_FW_MINOR_VERSION (uint8_t)(5)
/// Current FW patch version
#define L6470_FW_PATCH_VERSION (uint8_t)(0)
/// Current FW version
#define L6470_FW_VERSION (uint32_t)((L6470_FW_MAJOR_VERSION<<16)|\
                                          (L6470_FW_MINOR_VERSION<<8)|\
                                          (L6470_FW_PATCH_VERSION))
    
/// L6470 max number of bytes of command & arguments to set a parameter
#define L6470_CMD_ARG_MAX_NB_BYTES              (4)

/// L6470 command + argument bytes number for NOP command
#define L6470_CMD_ARG_NB_BYTES_NOP              (1)
/// L6470 command + argument bytes number for RUN command
#define L6470_CMD_ARG_NB_BYTES_RUN              (4)
/// L6470 command + argument bytes number for STEP_CLOCK command
#define L6470_CMD_ARG_NB_BYTES_STEP_CLOCK       (1)
/// L6470 command + argument bytes number for MOVE command
#define L6470_CMD_ARG_NB_BYTES_MOVE             (4)
/// L6470 command + argument bytes number for GO_TO command
#define L6470_CMD_ARG_NB_BYTES_GO_TO            (4)
/// L6470 command + argument bytes number for GO_TO_DIR command
#define L6470_CMD_ARG_NB_BYTES_GO_TO_DIR        (4)
/// L6470 command + argument bytes number for GO_UNTIL command
#define L6470_CMD_ARG_NB_BYTES_GO_UNTIL         (4)
/// L6470 command + argument bytes number for RELEASE_SW command
#define L6470_CMD_ARG_NB_BYTES_RELEASE_SW       (1)
/// L6470 command + argument bytes number for GO_HOME command
#define L6470_CMD_ARG_NB_BYTES_GO_HOME          (1)
/// L6470 command + argument bytes number for GO_MARK command
#define L6470_CMD_ARG_NB_BYTES_GO_MARK          (1)
/// L6470 command + argument bytes number for RESET_POS command
#define L6470_CMD_ARG_NB_BYTES_RESET_POS        (1)
/// L6470 command + argument bytes number for RESET_DEVICE command
#define L6470_CMD_ARG_NB_BYTES_RESET_DEVICE     (1)
/// L6470 command + argument bytes number for NOP command
#define L6470_CMD_ARG_NB_BYTES_SOFT_STOP        (1)
/// L6470 command + argument bytes number for HARD_STOP command
#define L6470_CMD_ARG_NB_BYTES_HARD_STOP        (1)
/// L6470 command + argument bytes number for SOFT_HIZ command
#define L6470_CMD_ARG_NB_BYTES_SOFT_HIZ         (1)
/// L6470 command + argument bytes number for ARD_HIZ command
#define L6470_CMD_ARG_NB_BYTES_HARD_HIZ         (1)
/// L6470 command + argument bytes number for GET_STATUS command
#define L6470_CMD_ARG_NB_BYTES_GET_STATUS       (1)

/// L6470 response bytes number 
#define L6470_RSP_NB_BYTES_GET_STATUS           (2)    

/// Daisy chain command mask
#define DAISY_CHAIN_COMMAND_MASK (0xFA)

/// powerSTEP01 max absolute position
#define L6470_MAX_POSITION (int32_t)(0x001FFFFF)

/// powerSTEP01 min absolute position
#define L6470_MIN_POSITION (int32_t)(0xFFE00000)
    
/// powerSTEP01 error base number
#define L6470_ERROR_BASE              (0xB000)

/// powerSTEP01 acceleration and deceleration max value
#define L6470_ACC_DEC_MAX_VALUE       (float)(59590)
/// powerSTEP01 max speed max value
#define L6470_MAX_SPEED_MAX_VALUE     (float)(15610)
/// powerSTEP01 min speed max value
#define L6470_MIN_SPEED_MAX_VALUE     (float)(976.3)
/// powerSTEP01 full step speed max value
#define L6470_FS_SPD_MAX_VALUE        (float)(15625)
/// powerSTEP01 intersect speed max value
#define L6470_INT_SPD_MAX_VALUE       (float)(976.5)
/// powerSTEP01 thermal compensation max value
#define L6470_K_THERM_MAX_VALUE       (float)(1.46875)
/// powerSTEP01 thermal compensation min value
#define L6470_K_THERM_MIN_VALUE       (float)(1)
/// powerSTEP01 thermal compensation max value
#define L6470_STALL_OCD_TH_MAX_VALUE  (float)(1000)
/// powerSTEP01 thermal compensation max value
#define L6470_K_THERM_MAX_VALUE       (float)(1.46875)
/// powerSTEP01 voltage amplitude regulation max value
#define L6470_KVAL_MAX_VALUE          (float)(255/256)
/// powerSTEP01 BEMF compensation curve slope max value
#define L6470_SLP_MAX_VALUE           (float)(0.4)
/// powerSTEP01 torque regulation DAC reference voltage max value
#define L6470_TVAL_MAX_VALUE          (float)(1000)
/// powerSTEP01 minimum off and on time max value
#define L6470_TOFF_TON_MIN_MAX_VALUE  (float)(64)
///Shift of the low speed optimization bit in MIN_SPEED register
#define L6470_LSPD_OPT_SHIFT            (12)
///Shift of F_PWM_INT field in CONFIG register for voltage mode
#define L6470_CONFIG_PWM_DIV_SHIFT      (13)
///Shift of F_PWM_DEC field in CONFIG register for voltage mode
#define L6470_CONFIG_PWM_MUL_SHIFT      (10)
///Shift of MOT_STATUS field in STATUS register
#define L6470_STATUS_MOT_STATUS_SHIFT   (5)
    
/**
  * @}
  */

/* Exported Types  -------------------------------------------------------*/

/** @defgroup L6470_Exported_Types L6470 Exported Types
  * @{
  */ 
    
/** @defgroup Error_Types Error Types
  * @{
  */
/// Errors
typedef enum {
  L6470_ERROR_SET_HOME         = L6470_ERROR_BASE,      /// Error while setting home position
  L6470_ERROR_SET_MAX_SPEED    = L6470_ERROR_BASE + 1,  /// Error while setting max speed
  L6470_ERROR_SET_MIN_SPEED    = L6470_ERROR_BASE + 2,  /// Error while setting min speed
  L6470_ERROR_SET_ACCELERATION = L6470_ERROR_BASE + 3,  /// Error while setting acceleration
  L6470_ERROR_SET_DECELERATION = L6470_ERROR_BASE + 4,  /// Error while setting decelaration
  L6470_ERROR_MCU_OSC_CONFIG   = L6470_ERROR_BASE + 5,  /// Error while configuring mcu oscillator
  L6470_ERROR_MCU_CLOCK_CONFIG = L6470_ERROR_BASE + 6,  /// Error while configuring mcu clock
  L6470_ERROR_POSITION         = L6470_ERROR_BASE + 7,  /// Unexpected current position (wrong number of steps)
  L6470_ERROR_SPEED            = L6470_ERROR_BASE + 8,  /// Unexpected current speed
  L6470_ERROR_INIT             = L6470_ERROR_BASE + 9,  /// Unexpected number of devices or unexpected value for predefined parameter
  L6470_ERROR_SET_DIRECTION    = L6470_ERROR_BASE + 10, /// Error while setting direction
  L6470_ERROR_SET_STEP_MODE    = L6470_ERROR_BASE + 11, /// Attempt to set an unsupported step mode
  L6470_ERROR_APPLY_SPEED      = L6470_ERROR_BASE + 12, /// Error while applying speed
  L6470_ERROR_SET_TORQUE       = L6470_ERROR_BASE + 13, /// Error
  L6470_ERROR_REGISTER         = L6470_ERROR_BASE + 14  /// Error while writing or reading a register
}l6470_errorTypes_t;
/**
  * @}
  */    

/// masks for ABS_POS register of L6470
typedef enum {
  L6470_ABS_POS_VALUE_MASK        = ((uint32_t)0x003FFFFF),
  L6470_ABS_POS_SIGN_BIT_MASK     = ((uint32_t)0x00200000)
} l6470_AbsPosMasks_t;

/// masks for EL_POS register of L6470
typedef enum {
  L6470_ELPOS_STEP_MASK       = ((uint16_t)0x180),
  L6470_ELPOS_MICROSTEP_MASK  = ((uint16_t)0x07F)
} l6470_ElPosMasks_t;

/// masks for MIN_SPEED register of L6470
typedef enum {
  L6470_LSPD_OPT        = ((uint16_t)((0x1)<<L6470_LSPD_OPT_SHIFT)),
  L6470_MIN_SPEED_MASK  = ((uint16_t)0x0FFF)
} l6470_MinSpeedMasks_t;

/// Low speed optimization (MIN_SPEED register of L6470)
typedef enum {
  L6470_LSPD_OPT_OFF    = ((uint16_t)0x0000),
  L6470_LSPD_OPT_ON     = ((uint16_t)L6470_LSPD_OPT)
} l6470_LspdOpt_t;

/// masks for FS_SPD register of L6470
typedef enum {
  // L6470_BOOST_MODE   = ((uint16_t)((0x1)<<L6470_BOOST_MODE_SHIFT)),
  L6470_FS_SPD_MASK  = ((uint16_t)0x03FF)
} l6470_FsSpdMasks_t;

typedef enum {
  L6470_OCD_TH_375mA    = ((uint8_t)0x00),
  L6470_OCD_TH_750mA    = ((uint8_t)0x01),
  L6470_OCD_TH_1125mA   = ((uint8_t)0x02),
  L6470_OCD_TH_1500mA   = ((uint8_t)0x03),
  L6470_OCD_TH_1875mA   = ((uint8_t)0x04),
  L6470_OCD_TH_2250mA   = ((uint8_t)0x05),
  L6470_OCD_TH_2625mA   = ((uint8_t)0x06),
  L6470_OCD_TH_3000mA   = ((uint8_t)0x07),
  L6470_OCD_TH_3375mA   = ((uint8_t)0x08),
  L6470_OCD_TH_3750mA   = ((uint8_t)0x09),
  L6470_OCD_TH_4125mA   = ((uint8_t)0x0A),
  L6470_OCD_TH_4500mA   = ((uint8_t)0x0B),
  L6470_OCD_TH_4875mA   = ((uint8_t)0x0C),
  L6470_OCD_TH_5250mA   = ((uint8_t)0x0D),
  L6470_OCD_TH_5625mA   = ((uint8_t)0x0E),
  L6470_OCD_TH_6000mA   = ((uint8_t)0x0F),
} l6470_OcdTh_t;

/// masks for STEP_MODE register of L6470
typedef enum {
  L6470_STEP_MODE_STEP_SEL    = ((uint8_t)0x07),
  L6470_STEP_MODE_CM_VM       = ((uint8_t)0x08),
  L6470_STEP_MODE_SYNC_SEL    = ((uint8_t)0x70),
  L6470_STEP_MODE_SYNC_EN     = ((uint8_t)0x80)
} l6470_StepModeMasks_t;

/// Voltage or Current mode selection (CM_VM field of STEP_MODE register of L6470)
typedef enum {
  L6470_CM_VM_VOLTAGE    = ((uint8_t)0x00),
  L6470_CM_VM_CURRENT    = ((uint8_t)0x08)
} l6470_CmVm_t;

/// Stepping options (field STEP_SEL of STEP_MODE register of L6470)
typedef enum {
  L6470_STEP_SEL_1      = ((uint8_t)0x00),
  L6470_STEP_SEL_1_2    = ((uint8_t)0x01),
  L6470_STEP_SEL_1_4    = ((uint8_t)0x02),
  L6470_STEP_SEL_1_8    = ((uint8_t)0x03),
  L6470_STEP_SEL_1_16   = ((uint8_t)0x04),
  L6470_STEP_SEL_1_32   = ((uint8_t)0x05),
  L6470_STEP_SEL_1_64   = ((uint8_t)0x06),
  L6470_STEP_SEL_1_128  = ((uint8_t)0x07)
} l6470_StepSel_t;

/// L6470 Sync Output frequency enabling bitw
#define L6470_SYNC_EN   ((0x1) << 7)

/// SYNC_SEL options (STEP_MODE register of L6470)
typedef enum {
  L6470_SYNC_SEL_DISABLED   = ((uint8_t)0x00),
  L6470_SYNC_SEL_1_2        = ((uint8_t)(L6470_SYNC_EN|0x00)),
  L6470_SYNC_SEL_1          = ((uint8_t)(L6470_SYNC_EN|0x10)),
  L6470_SYNC_SEL_2          = ((uint8_t)(L6470_SYNC_EN|0x20)),
  L6470_SYNC_SEL_4          = ((uint8_t)(L6470_SYNC_EN|0x30)),
  L6470_SYNC_SEL_8          = ((uint8_t)(L6470_SYNC_EN|0x40)),
  L6470_SYNC_SEL_16         = ((uint8_t)(L6470_SYNC_EN|0x50)),
  L6470_SYNC_SEL_32         = ((uint8_t)(L6470_SYNC_EN|0x60)),
  L6470_SYNC_SEL_64         = ((uint8_t)(L6470_SYNC_EN|0x70))
} l6470_SyncSel_t;

/// Alarms conditions (ALARM_EN register of L6470)
typedef enum {
  L6470_ALARM_EN_OVERCURRENT        = ((uint8_t)0x01),
  L6470_ALARM_EN_THERMAL_SHUTDOWN   = ((uint8_t)0x02),
  L6470_ALARM_EN_THERMAL_WARNING    = ((uint8_t)0x04),
  L6470_ALARM_EN_UVLO               = ((uint8_t)0x08),
  L6470_ALARM_EN_STALL_DETECTION_A  = ((uint8_t)0x10),
  L6470_ALARM_EN_STALL_DETECTION_B  = ((uint8_t)0x20),
  L6470_ALARM_EN_SW_TURN_ON         = ((uint8_t)0x40),
  L6470_ALARM_EN_WRONG_NPERF_CMD    = ((uint8_t)0x80)
} l6470_AlarmEn_t;

/// Masks for CONFIG register of L6470 
typedef enum {
  L6470_CONFIG_OSC_SEL      = ((uint16_t)0x0007),
  L6470_CONFIG_EXT_CLK      = ((uint16_t)0x0008),
  L6470_CONFIG_SW_MODE      = ((uint16_t)0x0010),
  L6470_CONFIG_OC_SD        = ((uint16_t)0x0080),
  L6470_CONFIG_POW_SR       = ((uint16_t)0x0300),
  L6470_CONFIG_EN_VSCOMP    = ((uint16_t)0x0020),
  L6470_CONFIG_F_PWM_DEC    = ((uint16_t)0x1C00),
  L6470_CONFIG_F_PWM_INT    = ((uint16_t)0xE000)
} l6470_ConfigMasks_t;

/// Masks for CONFIG register of L6470  (specific for current mode)
#define L6470_CONFIG_EN_TQREG (L6470_CONFIG_EN_VSCOMP) 

/// Oscillator management (EXT_CLK and OSC_SEL fields of CONFIG register of L6470)
typedef enum {
  L6470_CONFIG_INT_16MHZ               = ((uint16_t)0x0000),
  L6470_CONFIG_INT_16MHZ_OSCOUT_2MHZ   = ((uint16_t)0x0008),
  L6470_CONFIG_INT_16MHZ_OSCOUT_4MHZ   = ((uint16_t)0x0009),
  L6470_CONFIG_INT_16MHZ_OSCOUT_8MHZ   = ((uint16_t)0x000A),
  L6470_CONFIG_INT_16MHZ_OSCOUT_16MHZ  = ((uint16_t)0x000B),
  L6470_CONFIG_EXT_8MHZ_XTAL_DRIVE     = ((uint16_t)0x0004),
  L6470_CONFIG_EXT_16MHZ_XTAL_DRIVE    = ((uint16_t)0x0005),
  L6470_CONFIG_EXT_24MHZ_XTAL_DRIVE    = ((uint16_t)0x0006),
  L6470_CONFIG_EXT_32MHZ_XTAL_DRIVE    = ((uint16_t)0x0007),
  L6470_CONFIG_EXT_8MHZ_OSCOUT_INVERT  = ((uint16_t)0x000C),
  L6470_CONFIG_EXT_16MHZ_OSCOUT_INVERT = ((uint16_t)0x000D),
  L6470_CONFIG_EXT_24MHZ_OSCOUT_INVERT = ((uint16_t)0x000E),
  L6470_CONFIG_EXT_32MHZ_OSCOUT_INVERT = ((uint16_t)0x000F)
} l6470_ConfigOscMgmt_t;

/// Oscillator management (EXT_CLK and OSC_SEL fields of CONFIG register of L6470)
typedef enum {
  L6470_CONFIG_SW_HARD_STOP = ((uint16_t)0x0000),
  L6470_CONFIG_SW_USER      = ((uint16_t)0x0010)
} l6470_ConfigSwMode_t;

/// Voltage supply compensation enabling for voltage mode (EN_VSCOMP field of CONFIG register of L6470)
typedef enum {
  L6470_CONFIG_VS_COMP_DISABLE  = ((uint16_t)0x0000),
  L6470_CONFIG_VS_COMP_ENABLE   = ((uint16_t)0x0020)
} l6470_ConfigEnVscomp_t;

/// Overcurrent shutdown (OC_SD field of CONFIG register of L6470)
typedef enum {
  L6470_CONFIG_OC_SD_DISABLE  = ((uint16_t)0x0000),
  L6470_CONFIG_OC_SD_ENABLE   = ((uint16_t)0x0080)
} l6470_ConfigOcSd_t;

/// Output slow rates values (POW_SR field of CONFIG register of L6470)
typedef enum {
    L6470_CONFIG_SLEW_RATE_320V_uS  = ((uint16_t)0x0000),
    L6470_CONFIG_SLEW_RATE_75V_uS   = ((uint16_t)0x0100),
    L6470_CONFIG_SLEW_RATE_110V_uS  = ((uint16_t)0x0200),
    L6470_CONFIG_SLEW_RATE_260V_uS  = ((uint16_t)0x0300)
} l6470_ConfigPowSR_t;

/// PWM frequency division factor (F_PWM_INT field of CONFIG register of L6470)
typedef enum {
  L6470_CONFIG_PWM_DIV_1    = (((uint16_t)0x00)<<L6470_CONFIG_PWM_DIV_SHIFT),
  L6470_CONFIG_PWM_DIV_2    = (((uint16_t)0x01)<<L6470_CONFIG_PWM_DIV_SHIFT),
  L6470_CONFIG_PWM_DIV_3    = (((uint16_t)0x02)<<L6470_CONFIG_PWM_DIV_SHIFT),
  L6470_CONFIG_PWM_DIV_4    = (((uint16_t)0x03)<<L6470_CONFIG_PWM_DIV_SHIFT),
  L6470_CONFIG_PWM_DIV_5    = (((uint16_t)0x04)<<L6470_CONFIG_PWM_DIV_SHIFT),
  L6470_CONFIG_PWM_DIV_6    = (((uint16_t)0x05)<<L6470_CONFIG_PWM_DIV_SHIFT),
  L6470_CONFIG_PWM_DIV_7    = (((uint16_t)0x06)<<L6470_CONFIG_PWM_DIV_SHIFT)
} l6470_ConfigFPwmInt_t;

/// PWM frequency multiplication factor (F_PWM_DEC field of CONFIG register of L6470)
typedef enum {
  L6470_CONFIG_PWM_MUL_0_625  = (((uint16_t)0x00)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_0_75   = (((uint16_t)0x01)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_0_875  = (((uint16_t)0x02)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_1      = (((uint16_t)0x03)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_1_25   = (((uint16_t)0x04)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_1_5    = (((uint16_t)0x05)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_1_75   = (((uint16_t)0x06)<<L6470_CONFIG_PWM_MUL_SHIFT),
  L6470_CONFIG_PWM_MUL_2      = (((uint16_t)0x07)<<L6470_CONFIG_PWM_MUL_SHIFT)
} l6470_ConfigFPwmDec_t;

/// Bit mask for STATUS Register of L6470
typedef enum {
  L6470_STATUS_HIZ          = (((uint16_t)0x0001)),
  L6470_STATUS_BUSY         = (((uint16_t)0x0002)),
  L6470_STATUS_SW_F         = (((uint16_t)0x0004)),
  L6470_STATUS_SW_EVN       = (((uint16_t)0x0008)),
  L6470_STATUS_DIR          = (((uint16_t)0x0010)),
  L6470_STATUS_MOT_STATUS   = (((uint16_t)0x0060)),
  L6470_STATUS_NOTPERF_CMD  = (((uint16_t)0x0080)),
  L6470_STATUS_WRONG_CMD    = (((uint16_t)0x0100)),
  L6470_STATUS_UVLO         = (((uint16_t)0x0200)),
  L6470_STATUS_TH_WARN      = (((uint16_t)0x0400)),
  L6470_STATUS_TH_SD        = (((uint16_t)0x0800)),
  L6470_STATUS_OCD          = (((uint16_t)0x1000)),
  L6470_STATUS_STALL_A      = (((uint16_t)0x2000)),
  L6470_STATUS_STALL_B      = (((uint16_t)0x4000)),
  L6470_STATUS_STCK_MOD     = (((uint16_t)0x8000))
} l6470_StatusMasks_t;

/// Motor state (MOT_STATUS filed of STATUS register of L6470)
typedef enum {
  L6470_STATUS_MOT_STATUS_STOPPED       = (((uint16_t)0x0000)<<L6470_STATUS_MOT_STATUS_SHIFT),
  L6470_STATUS_MOT_STATUS_ACCELERATION  = (((uint16_t)0x0001)<<L6470_STATUS_MOT_STATUS_SHIFT),
  L6470_STATUS_MOT_STATUS_DECELERATION  = (((uint16_t)0x0002)<<L6470_STATUS_MOT_STATUS_SHIFT),
  L6470_STATUS_MOT_STATUS_CONST_SPD     = (((uint16_t)0x0003)<<L6470_STATUS_MOT_STATUS_SHIFT)
} l6470_Status_t;

/// L6470 internal register addresses
typedef enum {
  L6470_ABS_POS     = ((uint8_t)0x01),
  L6470_EL_POS      = ((uint8_t)0x02),
  L6470_MARK        = ((uint8_t)0x03), 
  L6470_SPEED       = ((uint8_t)0x04),
  L6470_ACC         = ((uint8_t)0x05),
  L6470_DEC         = ((uint8_t)0x06),
  L6470_MAX_SPEED   = ((uint8_t)0x07),
  L6470_MIN_SPEED   = ((uint8_t)0x08),
  L6470_FS_SPD      = ((uint8_t)0x15),
  L6470_KVAL_HOLD   = ((uint8_t)0x09),
  L6470_KVAL_RUN    = ((uint8_t)0x0A),
  L6470_KVAL_ACC    = ((uint8_t)0x0B),
  L6470_KVAL_DEC    = ((uint8_t)0x0C),
  L6470_INT_SPD     = ((uint8_t)0x0D),
  L6470_ST_SLP      = ((uint8_t)0x0E),
  L6470_FN_SLP_ACC  = ((uint8_t)0x0F),
  L6470_FN_SLP_DEC  = ((uint8_t)0x10),
  L6470_K_THERM     = ((uint8_t)0x11),
  L6470_ADC_OUT     = ((uint8_t)0x12),
  L6470_OCD_TH      = ((uint8_t)0x13),
  L6470_STALL_TH    = ((uint8_t)0x14),
  L6470_STEP_MODE   = ((uint8_t)0x16),
  L6470_ALARM_EN    = ((uint8_t)0x17),
  L6470_CONFIG      = ((uint8_t)0x18),
  L6470_STATUS      = ((uint8_t)0x19)
} l6470_Registers_t;

/// L6470 application commands
typedef enum {
  L6470_NOP               = ((uint8_t)0x00),
  L6470_SET_PARAM         = ((uint8_t)0x00),
  L6470_GET_PARAM         = ((uint8_t)0x20),
  L6470_RUN               = ((uint8_t)0x50),
  L6470_STEP_CLOCK        = ((uint8_t)0x58),
  L6470_MOVE              = ((uint8_t)0x40),
  L6470_GO_TO             = ((uint8_t)0x60),
  L6470_GO_TO_DIR         = ((uint8_t)0x68),
  L6470_GO_UNTIL          = ((uint8_t)0x82),
  L6470_GO_UNTIL_ACT_CPY  = ((uint8_t)0x8A),
  L6470_RELEASE_SW        = ((uint8_t)0x92),
  L6470_GO_HOME           = ((uint8_t)0x70),
  L6470_GO_MARK           = ((uint8_t)0x78),
  L6470_RESET_POS         = ((uint8_t)0xD8),
  L6470_RESET_DEVICE      = ((uint8_t)0xC0),
  L6470_SOFT_STOP         = ((uint8_t)0xB0),
  L6470_HARD_STOP         = ((uint8_t)0xB8),
  L6470_SOFT_HIZ          = ((uint8_t)0xA0),
  L6470_HARD_HIZ          = ((uint8_t)0xA8),
  L6470_GET_STATUS        = ((uint8_t)0xD0),
  L6470_RESERVED_CMD1     = ((uint8_t)0xEB),
  L6470_RESERVED_CMD2     = ((uint8_t)0xF8)
} l6470_Commands_t;

/** @defgroup Motor_Driver_Initialization_Structure Motor Driver Initialization Structure
  * @{
  */
///Initialization parameters structure common to current and voltage modes
typedef struct
{
  ///Current or voltage mode selection
  l6470_CmVm_t cmVmSelection;
  ///Acceleration 
  float acceleration;
  ///Deceleration
  float deceleration;
  ///Maximum speed
  float maxSpeed;
  ///Minimum speed
  float minSpeed;
  ///Low speed optimization bit
  l6470_LspdOpt_t lowSpeedOptimization;
  ///Full step speed
  float fullStepSpeed;
  ///Over current detection threshold
  float ocdThreshold;
  ///Step mode
  motorStepMode_t stepMode;
  ///Sync clock selection
  l6470_SyncSel_t syncClockSelection;
  ///Alarm selection
  uint8_t alarmsSelection;
} l6470_commonParameters_t;

///Initialization parameters structure for voltage mode
typedef struct
{
  ///Parameters common to current and voltage modes
  l6470_commonParameters_t cp;
  ///Voltage amplitude regulation when the motor is stopped
  float kvalHold;
  ///Voltage amplitude regulation when the motor is running at constant speed
  float kvalRun;
  ///Voltage amplitude regulation during motor acceleration
  float kvalAcc;
  ///Voltage amplitude regulation during motor deceleration
  float kvalDec;
  ///Speed value at which the BEMF compensation curve changes slope  
  float intersectSpeed;
  ///BEMF compensation curve slope when speed is lower than intersect speed
  float startSlope;
  ///BEMF compensation curve slope when speed is greater than intersect speed during acceleration
  float accelerationFinalSlope;
  ///BEMF compensation curve slope when speed is greater than intersect speed during deceleration
  float decelerationFinalSlope;
  ///Winding resistance thermal drift compensation coefficient
  float thermalCompensationFactor;
  ///Stall detection threshold
  float stallThreshold;
  ///System clock source management
  l6470_ConfigOscMgmt_t oscClkSel;
  ///External switch to act as HardStop interrupt or not
  l6470_ConfigSwMode_t swMode;
  ///Motor supply voltage compensation enable bit
  l6470_ConfigEnVscomp_t enVsComp;
  ///Overcurrent event causes or not the bridges to turn-off
  l6470_ConfigOcSd_t ocSd;
  // POW_SR slew rate
  l6470_ConfigPowSR_t slewRate;
  ///Integer division factor of PWM frequency generation
  l6470_ConfigFPwmInt_t fPwmInt;
  ///Multiplication factor of PWM frequency generation
  l6470_ConfigFPwmDec_t fPwmDec;
} l6470_VoltageMode_Init_t;

///Initialization parameters union type definition
typedef union l6470_Init_u l6470_Init_u_t;
///Union of current and voltage modes initialization parameters structures
union l6470_Init_u
{
  ///Initialization parameters structure for voltage mode
  l6470_VoltageMode_Init_t vm;
};


/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup L6470_Library_Functions
  * @{
  */
void L6470_AttachBusyInterrupt(void (*callback)(void));  //Attach a user callback to the busy interrupt
void L6470_AttachErrorHandler(void (*callback)(uint16_t));
void L6470_AttachFlagInterrupt(void (*callback)(void));  //Attach a user callback to the flag Interrupt
uint16_t L6470_ReadId(void);   //Read Id to get driver instance
void L6470_Init(void* pInit);  //Start the L6470 library
uint8_t L6470_CheckBusyHw(void);
uint8_t L6470_CheckStatusHw(void);
uint16_t L6470_CmdGetStatus(uint8_t deviceId);
void L6470_CmdGoHome(uint8_t deviceId);
void L6470_CmdGoMark(uint8_t deviceId);
void L6470_CmdGoTo(uint8_t deviceId, int32_t abs_pos);
void L6470_CmdGoToDir(uint8_t deviceId, motorDir_t direction, int32_t abs_pos);
void L6470_CmdGoUntil(uint8_t deviceId, motorAction_t action, motorDir_t direction, uint32_t speed);
void L6470_CmdHardHiZ(uint8_t deviceId);
void L6470_CmdHardStop(uint8_t deviceId);
void L6470_CmdMove(uint8_t deviceId, motorDir_t direction, uint32_t n_step);
void L6470_CmdNop(uint8_t deviceId);
void L6470_CmdReleaseSw(uint8_t deviceId, motorAction_t action, motorDir_t direction);
void L6470_CmdResetDevice(uint8_t deviceId);
void L6470_CmdResetPos(uint8_t deviceId);
void L6470_CmdRun(uint8_t deviceId, motorDir_t direction, uint32_t speed);
void L6470_CmdSoftHiZ(uint8_t deviceId);
void L6470_CmdSoftStop(uint8_t deviceId);
void L6470_CmdStepClock(uint8_t deviceId, motorDir_t direction);
void L6470_FetchAndClearAllStatus(void);                 //Fetch Status register and clear flags
uint16_t L6470_GetFetchedStatus(uint8_t deviceId);       //Get the fetched value of the STATUS register 
motorDrv_t* L6470_GetMotorHandle(void);                  //Return handle of the motor driver handle
uint32_t L6470_GetFwVersion(void);                       //Return the FW version
int32_t L6470_GetMark(uint8_t deviceId);                 //Return the mark position 
uint8_t L6470_GetNbDevices(void);                        //Get the number of devices
int32_t L6470_GetPosition(uint8_t deviceId);             //Return the ABS_POSITION (32b signed)
bool L6470_IsDeviceBusy(uint8_t deviceId); 
void L6470_QueueCommands(uint8_t deviceId, uint8_t command, int32_t value);
uint16_t L6470_ReadStatusRegister(uint8_t deviceId);     //Reads the Status Register value
void L6470_ReleaseReset(uint8_t deviceId);               //Release the L6470 reset pin 
void L6470_Reset(uint8_t deviceId);                      //Set the L6470 reset pin 
bool L6470_SelectStepMode(uint8_t deviceId, motorStepMode_t stepMode);
void L6470_SendQueuedCommands(void);
void L6470_SetHome(uint8_t deviceId, int32_t homePos);
void L6470_SetMark(uint8_t deviceId, int32_t markPos);
bool L6470_SetNbDevices(uint8_t nbDevices);
void L6470_StartStepClock(uint16_t newFreq); //Start the step clock by using the given frequency
void L6470_StopStepClock(void);              //Stop the PWM uses for the step clock
void L6470_WaitForAllDevicesNotBusy(void);
void L6470_WaitWhileActive(uint8_t deviceId);

/* Handler functions */
void L6470_BusyInterruptHandler(void);
void L6470_FlagInterruptHandler(void);

/**
 * @brief Functions to get and set parameters using digital or analog values
 */
void L6470_CmdSetParam(uint8_t deviceId, uint32_t param, uint32_t value);
uint32_t L6470_CmdGetParam(uint8_t deviceId, uint32_t param);
bool L6470_SetAnalogValue(uint8_t deviceId, uint32_t param, float value);
float L6470_GetAnalogValue(uint8_t deviceId, uint32_t param);

/**
  * @}
  */
  
/** @defgroup MotorControl_Board_Linked_Functions MotorControl Board Linked Functions
  * @{
  */   
extern void L6470_Board_Delay(uint32_t delay);         //Delay of the requested number of milliseconds
extern void L6470_Board_EnableIrq(void);               //Disable Irq
extern void L6470_Board_DisableIrq(void);              //Enable Irq
extern void L6470_Board_GpioInit(uint8_t deviceId);    //Initialise GPIOs used for a powerSTEP01 device
extern void L6470_Board_StepClockInit(void);           //Initialise the step clock for a powerSTEP01 device
extern void L6470_Board_StartStepClock(uint16_t newFreq); //Start the step clock by using the given frequency
extern void L6470_Board_StopStepClock(void);              //Stop the PWM uses for the step clock
extern void L6470_Board_ReleaseReset(uint8_t deviceId);   //Reset the powerSTEP01 reset pin 
extern void L6470_Board_Reset(uint8_t deviceId);          //Set the powerSTEP01 reset pin
extern uint8_t L6470_Board_SpiInit(void);   //Initialise the SPI used for powerSTEP01s
extern uint8_t L6470_Board_SpiWriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte, uint8_t nbDevices); //Write bytes to the powerSTEP01s via SPI
extern uint32_t L6470_Board_BUSY_PIN_GetState(void); //Returns the BUSY pin state
extern uint32_t L6470_Board_FLAG_PIN_GetState(void); //Returns the FLAG pin state
/**
  * @}
  */

  /**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _L6470_H_INCLUDED */
