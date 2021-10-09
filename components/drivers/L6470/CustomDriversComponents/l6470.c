/**************************************************************************//**
  * @file    l6470.c 
  * @author  IPC Rennes
  * @version V1.6.0
  * @date    June 4th, 2018
  * @brief   L6470 motor driver (Microstepping controller with power MOSFETs)
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

#include "L6470/L6470.h"
#include "l6470.h"

/** @addtogroup BSP
  * @{
  */   
   
/** @addtogroup L6470
  * @{
  */   

/* Private constants ---------------------------------------------------------*/

/** @addtogroup L6470_Private_Constants
  * @{
  */
/// Error while initialising the SPI
#define L6470_ERROR_0   (0xB000)
/// Error: Bad SPI transaction
#define L6470_ERROR_1   (0xB001)

/**
  * @}
  */ 

/* Private variables ---------------------------------------------------------*/

/** @addtogroup L6470_Private_Variables
  * @{
  */
  
/// Function pointer to busy interrupt call back
void (*busyInterruptCallback)(void);
/// Function pointer to flag interrupt call back
void (*flagInterruptCallback)(void);
/// Function pointer to error handler call back
void (*errorHandlerCallback)(uint16_t);
static volatile uint8_t numberOfDevices;
static uint8_t spiTxBursts[L6470_CMD_ARG_MAX_NB_BYTES][MAX_NUMBER_OF_DEVICES];
static uint8_t spiRxBursts[L6470_CMD_ARG_MAX_NB_BYTES][MAX_NUMBER_OF_DEVICES];
static volatile bool spiPreemtionByIsr = FALSE;
static volatile bool isrFlag = FALSE;
static uint16_t l6470DriverInstance = 0;

/**
  * @}
  */ 

/* Private function prototypes -----------------------------------------------*/

/** @defgroup L6470_Private_Functions Private Functions
  * @{
  */  
int32_t L6470_ConvertPosition(uint32_t abs_position_reg);
void L6470_ErrorHandler(uint16_t error);
void L6470_SendCommand(uint8_t deviceId, uint8_t command, uint32_t value);
void L6470_SetRegisterToPredefinedValues(uint8_t deviceId);
void L6470_SetDeviceParamsToGivenValues(uint8_t deviceId, l6470_Init_u_t *initPrm);
void L6470_WriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte);

/**
 * @brief To and from register parameter conversion functions
 */
///Convert the float formatted acceleration or deceleration into respectively an ACC or DEC register value
uint16_t L6470_AccDec_Steps_s2_to_RegVal(float steps_s2);
///Convert the ACC or DEC register value into step/s^2
float L6470_AccDec_RegVal_to_Steps_s2(uint32_t regVal);
///Convert BEMF compensation slope to values for ST_SLP, FN_SLP_ACC or FN_SLP_DEC register
uint8_t L6470_BEMFslope_Perc_to_RegVal(float percentage);
///Convert values from ST_SLP, FN_SLP_ACC or FN_SLP_DEC register to BEMF compensation slope percentage
float L6470_BEMFslope_RegVal_to_Perc(uint32_t regVal);
///Convert the float formatted speed into a FS_SPD register value
uint16_t L6470_FSSpd_Steps_s_to_RegVal(float steps_s);
///Convert the FS_SPD register value into step/s
float L6470_FSSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted speed into a INT_SPEED register value
uint16_t L6470_IntSpd_Steps_s_to_RegVal(float steps_s);
///Convert the INT_SPEED register value into step/s
float L6470_IntSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted thermal compensation factor into a K_THEM register value
uint8_t L6470_KTherm_Comp_to_RegVal(float compFactor);
///Convert the K_THERM register value into a float formatted thermal compensation factor
float L6470_KTherm_RegVal_to_Comp(uint32_t regVal);
///Convert voltage in percentage to values for KVAL_RUN, KVAL_HOLD, KVAL_ACC or KVAL_DEC register
uint8_t L6470_Kval_Perc_to_RegVal(float percentage);
///Convert values from KVAL_RUN, KVAL_HOLD, KVAL_ACC or KVAL_DEC register to percentage
float L6470_Kval_RegVal_to_Perc(uint32_t regVal);
///Convert the float formatted speed into a MAX_SPEED register value
uint16_t L6470_MaxSpd_Steps_s_to_RegVal(float steps_s);
///Convert the MAX_SPEED register value into step/s
float L6470_MaxSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted speed into a MIN_SPEED register value
uint16_t L6470_MinSpd_Steps_s_to_RegVal(float steps_s);
///Convert the MIN_SPEED register value into step/s
float L6470_MinSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted speed into a SPEED register value
uint32_t L6470_Speed_Steps_s_to_RegVal(float steps_s);
///Convert the SPEED register value into step/s
float L6470_Speed_RegVal_to_Steps_s(uint32_t regVal);
///Converts STALL or OCD Threshold voltage in mV to values for STALL_TH or OCD_TH register
uint8_t L6470_StallOcd_Th_to_RegVal(float mV);
///Convert values from STALL_TH or OCD_TH register to mV
float L6470_StallOcd_RegVal_to_Th(uint32_t regVal);
///Convert voltage in mV to values for TVAL_RUN, TVAL_HOLD, TVAL_ACC or TVAL_DEC register
uint8_t L6470_Tval_RefVoltage_to_RegVal(float voltage_mV);
///Convert values from TVAL_RUN, TVAL_HOLD, TVAL_ACC or TVAL_DEC register to mV
float L6470_Tval_RegVal_to_RefVoltage(uint32_t regVal);
///Convert time in us to values for TON_MIN register
uint8_t L6470_Tmin_Time_to_RegVal(float tmin_us);
///Convert values for TON_MIN or TOFF_MIN register to time in us
float L6470_Tmin_RegVal_to_Time(uint32_t regVal);

/**
  * @}
  */ 

/** @defgroup L6470_Exported_Variables Exported Variables
  * @{
  */

/// L6470 motor driver functions pointer structure
motorDrv_t   l6470Drv= 
{
  L6470_Init,                    //void (*Init)(void*);
  L6470_ReadId,                  //uint16_t (*ReadID)(void);
  L6470_AttachErrorHandler,      //void (*AttachErrorHandler)(void (*callback)(uint16_t));
  L6470_AttachFlagInterrupt,     //void (*AttachFlagInterrupt)(void (*callback)(void));
  L6470_AttachBusyInterrupt,     //void (*AttachBusyInterrupt)(void (*callback)(void));
  L6470_FlagInterruptHandler,    //void (*FlagInterruptHandler)(void);
  0,                                   //uint16_t (*GetAcceleration)(uint8_t);
  0,                                   //uint16_t (*GetCurrentSpeed)(uint8_t);
  0,                                   //uint16_t (*GetDeceleration)(uint8_t);
  0,                                   //motorState_t(*GetDeviceState)(uint8_t);
  L6470_GetFwVersion,            //uint32_t (*GetFwVersion)(void);
  L6470_GetMark,                 //int32_t (*GetMark)(uint8_t);
  0,                                   //uint16_t (*GetMaxSpeed)(uint8_t);
  0,                                   //uint16_t (*GetMinSpeed)(uint8_t);
  L6470_GetPosition,             //int32_t (*GetPosition)(uint8_t);
  L6470_CmdGoHome,               //void (*GoHome)(uint8_t);
  L6470_CmdGoMark,               //void (*GoMark)(uint8_t);
  L6470_CmdGoTo,                 //void (*GoTo)(uint8_t, int32_t);
  L6470_CmdHardStop,             //void (*HardStop)(uint8_t);
  L6470_CmdMove,                 //void (*Move)(uint8_t, motorDir_t, uint32_t );
  0,                                   //void (*ResetAllDevices)(void);
  0,                                   //void (*Run)(uint8_t, motorDir_t);
  0,                                   //bool(*SetAcceleration)(uint8_t ,uint16_t );
  0,                                   //bool(*SetDeceleration)(uint8_t , uint16_t );
  L6470_SetHome,                 //void (*SetHome)(uint8_t, int32_t);
  L6470_SetMark,                 //void (*SetMark)(uint8_t, int32_t);
  0,                                   //bool (*SetMaxSpeed)(uint8_t, uint16_t );
  0,                                   //bool (*SetMinSpeed)(uint8_t, uint16_t );
  0,                                   //bool (*SoftStop)(uint8_t);
  0,                                   //void (*StepClockHandler)(uint8_t deviceId);
  L6470_WaitWhileActive,         //void (*WaitWhileActive)(uint8_t);
  0,                                   //void (*CmdDisable)(uint8_t);
  0,                                   //void (*CmdEnable)(uint8_t);
  L6470_CmdGetParam,             //uint32_t (*CmdGetParam)(uint8_t, uint32_t);
  L6470_CmdGetStatus,            //uint16_t (*CmdGetStatus)(uint8_t);
  L6470_CmdNop,                  //void (*CmdNop)(uint8_t);
  L6470_CmdSetParam,             //void (*CmdSetParam)(uint8_t, uint32_t, uint32_t);
  L6470_ReadStatusRegister,      //uint16_t (*ReadStatusRegister)(uint8_t);
  L6470_ReleaseReset,            //void (*ReleaseReset)(uint8_t);
  L6470_Reset,                   //void (*Reset)(uint8_t);
  L6470_SelectStepMode,          //bool (*SelectStepMode)(uint8_t deviceId, motorStepMode_t);
  0,                                   //void (*SetDirection)(uint8_t, motorDir_t);
  L6470_CmdGoToDir,              //void (*CmdGoToDir)(uint8_t, motorDir_t, int32_t);
  L6470_CheckBusyHw,             //uint8_t (*CheckBusyHw)(void);
  L6470_CheckStatusHw,           //uint8_t (*CheckStatusHw)(void);
  L6470_CmdGoUntil,              //void (*CmdGoUntil)(uint8_t, motorAction_t, motorDir_t, uint32_t);
  L6470_CmdHardHiZ,              //void (*CmdHardHiZ)(uint8_t);
  L6470_CmdReleaseSw,            //void (*CmdReleaseSw)(uint8_t, motorAction_t, motorDir_t);
  L6470_CmdResetDevice,          //void (*CmdResetDevice)(uint8_t);
  L6470_CmdResetPos,             //void (*CmdResetPos)(uint8_t);
  L6470_CmdRun,                  //void (*CmdRun)(uint8_t, motorDir_t, uint32_t);
  L6470_CmdSoftHiZ,              //void (*CmdSoftHiZ)(uint8_t);
  L6470_CmdStepClock,            //void (*CmdStepClock)(uint8_t, motorDir_t);
  L6470_FetchAndClearAllStatus,  //void (*FetchAndClearAllStatus)(void);  
  L6470_GetFetchedStatus,        //uint16_t (*GetFetchedStatus)(uint8_t);
  L6470_GetNbDevices,            //uint8_t (*GetNbDevices)(void);
  L6470_IsDeviceBusy,            //bool (*IsDeviceBusy)(uint8_t);
  L6470_SendQueuedCommands,      //void (*SendQueuedCommands)(void);
  L6470_QueueCommands,           //void (*QueueCommands)(uint8_t, uint8_t, int32_t);
  L6470_WaitForAllDevicesNotBusy,//void (*WaitForAllDevicesNotBusy)(void);
  L6470_ErrorHandler,            //void (*ErrorHandler)(uint16_t);
  L6470_BusyInterruptHandler,    //void (*BusyInterruptHandler)(void);
  L6470_CmdSoftStop,             //void (*CmdSoftStop)(uint8_t);
  L6470_StartStepClock,          //void (*StartStepClock)(uint16_t);
  L6470_StopStepClock,           //void (*StopStepClock)(void);
  0,                                   //void (*SetDualFullBridgeConfig)(uint8_t);
  0,                                   //uint32_t (*GetBridgeInputPwmFreq)(uint8_t);
  0,                                   //void (*SetBridgeInputPwmFreq)(uint8_t, uint32_t);
  0,                                   //void (*SetStopMode)(uint8_t, motorStopMode_t);
  0,                                   //motorStopMode_t (*GetStopMode)(uint8_t);
  0,                                   //void (*SetDecayMode)(uint8_t, motorDecayMode_t);
  0,                                   //motorDecayMode_t (*GetDecayMode)(uint8_t);
  0,                                   //motorStepMode_t (*GetStepMode)(uint8_t);
  0,                                   //motorDir_t (*GetDirection)(uint8_t);
  0,                                   //void (*ExitDeviceFromReset)(uint8_t);
  0,                                   //void (*SetTorque)(uint8_t, motorTorqueMode_t, uint8_t);
  0,                                   //uint8_t (*GetTorque)(uint8_t, motorTorqueMode_t);
  0,                                   //void (*SetRefFreq)(uint8_t, uint32_t);
  0,                                   //uint32_t (*GetRefFreq)(uint8_t);
  0,                                   //void (*SetRefDc)(uint8_t, uint8_t);
  0,                                   //uint8_t (*GetRefDc)(uint8_t);
  L6470_SetNbDevices,            //bool (*SetNbDevices)(uint8_t);
  L6470_SetAnalogValue,            //bool (*SetAnalogValue)(uint8_t, uint32_t, float);
  L6470_GetAnalogValue             //float (*GetAnalogValue)(uint8_t, uint32_t);
};

/**
  * @}
  */ 

/** @defgroup L6470_Library_Functions Library Functions
  * @{
  */   

/******************************************************//**
 * @brief  Attaches a user callback to the error Handler.
 * The call back will be then called each time the library 
 * detects an error
 * @param[in] callback Name of the callback to attach 
 * to the error Hanlder
 * @retval None
 **********************************************************/
void L6470_AttachErrorHandler(void (*callback)(uint16_t))
{
  errorHandlerCallback = (void (*)(uint16_t))callback;
}

/******************************************************//**
 * @brief  Attaches a user callback to the flag Interrupt
 * The call back will be then called each time the status 
 * flag pin will be pulled down due to the occurrence of 
 * a programmed alarms ( OCD, thermal pre-warning or 
 * shutdown, UVLO, wrong command, non-performable command)
 * @param[in] callback Name of the callback to attach 
 * to the Flag Interrupt
 * @retval None
 **********************************************************/
void L6470_AttachFlagInterrupt(void (*callback)(void))
{
  flagInterruptCallback = (void (*)())callback;
}

/******************************************************//**
 * @brief  Attaches a user callback to the busy Interrupt
 * The call back will be then called each time the busy 
 * pin is set or reset 
 * @param[in] callback Name of the callback to attach 
 * to the Busy Interrupt
 * @retval None
 **********************************************************/
void L6470_AttachBusyInterrupt(void (*callback)(void))
{
  busyInterruptCallback = (void (*)())callback;
}

/******************************************************//**
 * @brief Read id
 * @retval Id of the L6470 Driver Instance
 **********************************************************/
uint16_t L6470_ReadId(void)
{
  return(l6470DriverInstance);
}

/******************************************************//**
 * @brief Starts the L6470 library
 * @param[in] pInit pointer to the initialization data
 * @retval None
 **********************************************************/
void L6470_Init(void* pInit)
{ 
  /* Initialise the GPIOs of the just added device */
  L6470_Board_GpioInit(l6470DriverInstance);
  
  if(L6470_Board_SpiInit() != 0)
  {
    /* Initialization Error */
    L6470_ErrorHandler(L6470_ERROR_0);
  } 
  
  /* configure the step clock */
  L6470_Board_StepClockInit();
  
  /* Standby-reset deactivation */
  L6470_Board_ReleaseReset(l6470DriverInstance);
  
  /* Let a delay after reset */
  L6470_Board_Delay(1);

  // if (pInit == 0)
  // {
  //   // Set all registers to their predefined values from l6470_target_config.h 
  //   L6470_SetRegisterToPredefinedValues(l6470DriverInstance);
  // }
  // else
  // {
  //   L6470_SetDeviceParamsToGivenValues(l6470DriverInstance, pInit);
  // }
  
  // // Put the L6470 in HiZ state
  // L6470_CmdHardHiZ(l6470DriverInstance);
  
  L6470_FetchAndClearAllStatus();
  
  l6470DriverInstance++;
}

/******************************************************//**
 * @brief Checks if at least one L6470 is busy by checking 
 * busy pin position. 
 * The busy pin is shared between all devices.
 * @retval One if at least one L6470 is busy, otherwise zero
 **********************************************************/
uint8_t L6470_CheckBusyHw(void)
{
	if(!(L6470_Board_BUSY_PIN_GetState()))
  {
    return 0x01;
  }
	else 
  {
    return 0x00;
  }
}

/******************************************************//**
 * @brief Checks if at least one L6470 has an alarm flag set
 * by reading flag pin position.
 * The flag pin is shared between all devices.
 * @retval One if at least one L6470 has an alarm flag set ,
 * otherwise zero
 **********************************************************/
uint8_t L6470_CheckStatusHw(void)
{
	if(!(L6470_Board_FLAG_PIN_GetState()))
  {
    return 0x01;
  }
	else 
  {
    return 0x00;
  }
}

/******************************************************//**
 * @brief Issues L6470 Get Param command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param L6470 register address
 * @retval Register value - 1 to 3 bytes (depends on register)
 *********************************************************/
uint32_t L6470_CmdGetParam(uint8_t deviceId, uint32_t param)
{
  uint32_t spiRxData = 0;

  if (numberOfDevices > deviceId)
  {
    uint32_t loop;
    uint8_t maxArgumentNbBytes = 0;
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    bool itDisable = FALSE;
    
    do
    {
      spiPreemtionByIsr = FALSE;
      if (itDisable)
      {
        /* re-enable L6470_Board_EnableIrq if disable in previous iteration */
        L6470_Board_EnableIrq();
        itDisable = FALSE;
      }
      for (loop = 0; loop < numberOfDevices; loop++)
      {
        spiTxBursts[0][loop] = L6470_NOP;
        spiTxBursts[1][loop] = L6470_NOP;
        spiTxBursts[2][loop] = L6470_NOP;
        spiTxBursts[3][loop] = L6470_NOP;
        spiRxBursts[0][loop] = 0;
        spiRxBursts[1][loop] = 0;
        spiRxBursts[2][loop] = 0;
        spiRxBursts[3][loop] = 0;    
      }
      switch (param)
      {
        case L6470_ABS_POS: 
        case L6470_MARK:
        case L6470_SPEED:
          spiTxBursts[0][spiIndex] = ((uint8_t)L6470_GET_PARAM )| (param);
          maxArgumentNbBytes = 3;
          break;
        case L6470_EL_POS:
        case L6470_ACC:
        case L6470_DEC:
        case L6470_MAX_SPEED:
        case L6470_MIN_SPEED:
        case L6470_FS_SPD:
        case L6470_INT_SPD:
        case L6470_CONFIG:
        // case L6470_GATECFG1:
        case L6470_STATUS:
          spiTxBursts[1][spiIndex] = ((uint8_t)L6470_GET_PARAM )| (param);
          maxArgumentNbBytes = 2;
          break;
      default:
          spiTxBursts[2][spiIndex] = ((uint8_t)L6470_GET_PARAM )| (param);
          maxArgumentNbBytes = 1;
      }
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      L6470_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR
    for (loop = L6470_CMD_ARG_MAX_NB_BYTES-1-maxArgumentNbBytes;
         loop < L6470_CMD_ARG_MAX_NB_BYTES;
         loop++)
    {
       L6470_WriteBytes(&spiTxBursts[loop][0],
                             &spiRxBursts[loop][0]);
    }
    spiRxData = ((uint32_t)spiRxBursts[1][spiIndex] << 16)|
                 (spiRxBursts[2][spiIndex] << 8) |
                 (spiRxBursts[3][spiIndex]);    
    /* re-enable L6470_Board_EnableIrq after SPI transfers*/
    L6470_Board_EnableIrq();
  }
  return (spiRxData);
}

/******************************************************//**
 * @brief Issues L6470 Get Status command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Status Register content
 *********************************************************/
uint16_t L6470_CmdGetStatus(uint8_t deviceId)
{
  uint16_t status = 0;
  if (numberOfDevices > deviceId)
  {
    uint32_t loop;
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    bool itDisable = FALSE; 
 
    do
    {
      spiPreemtionByIsr = FALSE;
      if (itDisable)
      {
        /* re-enable L6470_Board_EnableIrq if disable in previous iteration */
        L6470_Board_EnableIrq();
        itDisable = FALSE;
      }    
      for (loop = 0; loop < numberOfDevices; loop++)
      {
         spiTxBursts[0][loop] = L6470_NOP;
         spiTxBursts[1][loop] = L6470_NOP;
         spiTxBursts[2][loop] = L6470_NOP;
         spiTxBursts[3][loop] = L6470_NOP;
         spiRxBursts[0][loop] = 0;
         spiRxBursts[1][loop] = 0;
         spiRxBursts[2][loop] = 0;
         spiRxBursts[3][loop] = 0;       
      }
      spiTxBursts[0][spiIndex] = L6470_GET_STATUS;
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      L6470_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR  
    for (loop = 0; loop < L6470_CMD_ARG_NB_BYTES_GET_STATUS + L6470_RSP_NB_BYTES_GET_STATUS; loop++)
    {
       L6470_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
    }
    status = (spiRxBursts[1][spiIndex] << 8) | (spiRxBursts[2][spiIndex]);
    /* re-enable L6470_Board_EnableIrq after SPI transfers*/
    L6470_Board_EnableIrq();    
  }
  return (status);
}

/******************************************************//**
 * @brief Issues L6470 Go Home command (Shortest path to zero position)
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void L6470_CmdGoHome(uint8_t deviceId)
{
   L6470_SendCommand(deviceId, L6470_GO_HOME, 0);   
}

/******************************************************//**
 * @brief Issues L6470 Go Mark command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void L6470_CmdGoMark(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_GO_MARK, 0);     
}

/******************************************************//**
 * @brief Issues L6470 Go To command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] abs_pos absolute position in steps in agreement
 * with the step mode where requested to move
 * @retval None
 *********************************************************/
void L6470_CmdGoTo(uint8_t deviceId, int32_t abs_pos)
{
  L6470_SendCommand(deviceId, L6470_GO_TO, abs_pos);  
}

/******************************************************//**
 * @brief Issues L6470 Go To Dir command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction movement direction
 * @param[in] abs_pos absolute position in steps in agreement
 * with the step mode where requested to move
 * @retval None
 *********************************************************/
void L6470_CmdGoToDir(uint8_t deviceId, 
                            motorDir_t direction,
                            int32_t abs_pos)
{
  L6470_SendCommand(deviceId, 
                          (uint8_t)L6470_GO_TO_DIR | 
                          (uint8_t)direction, abs_pos);  
}

/******************************************************//**
 * @brief Issues L6470 Go Until command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] action ACTION_RESET or ACTION_COPY
 * @param[in] direction movement direction
 * @param[in] speed in 2^-28 step/tick
 * @retval None
 *********************************************************/
void L6470_CmdGoUntil(uint8_t deviceId, 
                           motorAction_t action, 
                           motorDir_t direction, 
                           uint32_t speed)
{
  L6470_SendCommand(deviceId,
                          (uint8_t)L6470_GO_UNTIL | (uint8_t)action | (uint8_t)direction,
                          speed); 
}

/******************************************************//**
 * @brief Issues L6470 Hard HiZ command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The HardHiZ command immediately disables the power bridges
 * (high impedance state) and raises the HiZ flag. 
 * When the motor is stopped, a HardHiZ command forces the bridges 
 * to enter high impedance state.
 * This command can be given anytime and is immediately executed.
 *********************************************************/
void L6470_CmdHardHiZ(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_HARD_HIZ, 0);    
}

/******************************************************//**
 * @brief Issues L6470 Hard Stop command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The HardStop command causes an immediate motor stop with
 * infinite deceleration.
 * When the motor is in high impedance state, a HardStop command
 * forces the bridges to exit high impedance state; no motion is performed.
 * This command can be given anytime and is immediately executed.
 * This command keeps the BUSY flag low until the motor is stopped.
 *********************************************************/
void L6470_CmdHardStop(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_HARD_STOP, 0);         
}

/******************************************************//**
 * @brief Issues L6470 Move command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction
 * @param[in] n_step number of steps
 * @retval None
 *********************************************************/
void L6470_CmdMove(uint8_t deviceId, 
                         motorDir_t direction, 
                         uint32_t n_step)
{
  L6470_SendCommand(deviceId, 
                          (uint8_t)L6470_MOVE | 
                          (uint8_t)direction, n_step);  
}

/******************************************************//**
 * @brief Issues L6470 NOP command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void L6470_CmdNop(uint8_t deviceId)
{
  /* Send NOP operation code to L6470 */
  L6470_SendCommand(deviceId, L6470_NOP, 0);
}

/******************************************************//**
 * @brief Issues L6470 Release SW command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] action type of action to undertake when the SW
 * input is forced high
 * @param[in] direction movement direction
 * @retval None
 *********************************************************/
void L6470_CmdReleaseSw(uint8_t deviceId, 
                             motorAction_t action, 
                             motorDir_t direction)
{
   L6470_SendCommand(deviceId, 
                           (uint8_t)L6470_RELEASE_SW | 
                           (uint8_t)action | 
                           (uint8_t)direction, 
                           0); 
}

/******************************************************//**
 * @brief Issues L6470 Reset Device command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void L6470_CmdResetDevice(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_RESET_DEVICE, 0);         
}

/******************************************************//**
 * @brief Issues L6470 Reset Pos command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void L6470_CmdResetPos(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_RESET_POS, 0);       
}

/******************************************************//**
 * @brief Issues L6470 Run command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction (FORWARD, BACKWARD)
 * @param[in] speed in 2^-28 step/tick
 * @retval None
 *********************************************************/
void L6470_CmdRun(uint8_t deviceId, 
                       motorDir_t direction,
                       uint32_t speed)
{
  L6470_SendCommand(deviceId, 
                          (uint8_t)L6470_RUN|(uint8_t)direction,
                          speed);
}

/******************************************************//**
 * @brief Issues the SetParam command to the L6470 of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param Register adress (L6470_ABS_POS, L6470_MARK,...)
 * @param[in] value Value to set in the register
 * @retval None
 *********************************************************/
void L6470_CmdSetParam(uint8_t deviceId, uint32_t param, uint32_t value)
{

  if (numberOfDevices > deviceId)
  {
    uint32_t loop;
    uint8_t maxArgumentNbBytes = 0;
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    bool itDisable = FALSE;
    
    do
    {
      spiPreemtionByIsr = FALSE;
      if (itDisable)
      {
        /* re-enable L6470_Board_EnableIrq if disable in previous iteration */
        L6470_Board_EnableIrq();
        itDisable = FALSE;
      }  
      for (loop = 0;loop < numberOfDevices; loop++)
      {
        spiTxBursts[0][loop] = L6470_NOP;
        spiTxBursts[1][loop] = L6470_NOP;
        spiTxBursts[2][loop] = L6470_NOP;
        spiTxBursts[3][loop] = L6470_NOP;
      }
      switch (param)
      {
        case L6470_ABS_POS: ;
        case L6470_MARK:
          spiTxBursts[0][spiIndex] = ((uint8_t)L6470_SET_PARAM )| (param);
          spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
          spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
          maxArgumentNbBytes = 3;
          break;
        case L6470_EL_POS:
        case L6470_ACC:
        case L6470_DEC:
        case L6470_MAX_SPEED:
        case L6470_MIN_SPEED:
        case L6470_FS_SPD:
        case L6470_INT_SPD:
        case L6470_CONFIG:
          spiTxBursts[1][spiIndex] = ((uint8_t)L6470_SET_PARAM )| (param);
          spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
          maxArgumentNbBytes = 2;
          break;
        default:
          spiTxBursts[2][spiIndex] = ((uint8_t)L6470_SET_PARAM )| (param);
          maxArgumentNbBytes = 1;
      }
      spiTxBursts[3][spiIndex] = (uint8_t)(value);
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      L6470_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR  
    /* SPI transfer */
    for (loop = L6470_CMD_ARG_MAX_NB_BYTES - 1 - maxArgumentNbBytes;
         loop < L6470_CMD_ARG_MAX_NB_BYTES;
         loop++)
    {
      L6470_WriteBytes(&spiTxBursts[loop][0],&spiRxBursts[loop][0]);
    }
    /* re-enable L6470_Board_EnableIrq after SPI transfers*/
    L6470_Board_EnableIrq();
  }
}

/******************************************************//**
 * @brief Issues L6470 Soft HiZ command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The SoftHiZ command disables the power bridges
 * (high impedance state) after a deceleration to zero.
 * The deceleration value used is the one stored in the DEC register.
 * When bridges are disabled, the HiZ flag is raised.
 * When the motor is stopped, a SoftHiZ command forces the bridges
 * to enter high impedance state.
 * This command can be given anytime and is immediately executed.
 * This command keeps the BUSY flag low until the motor is stopped.
 *********************************************************/
void L6470_CmdSoftHiZ(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_SOFT_HIZ, 0);           
}

/******************************************************//**
 * @brief Issues L6470 Soft Stop command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The SoftStop command causes an immediate deceleration
 * to zero speed and a consequent motor stop.
 * The deceleration value used is the one stored in the DEC register.
 * When the motor is in high impedance state, a SoftStop
 * command forces the bridges to exit from high impedance state.
 * No motion is performed.
 * This command can be given anytime and is immediately executed.
 * This command keeps the BUSY flag low until the motor is stopped.
 *********************************************************/
void L6470_CmdSoftStop(uint8_t deviceId)
{
  L6470_SendCommand(deviceId, L6470_SOFT_STOP, 0);         
}

/******************************************************//**
 * @brief Issues L6470 Step Clock command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction (FORWARD, BACKWARD)
 * @retval None
 *********************************************************/
void L6470_CmdStepClock(uint8_t deviceId,
                             motorDir_t direction)
{
  L6470_SendCommand(deviceId, 
                          (uint8_t)L6470_STEP_CLOCK | (uint8_t)direction,
                          0);  
}

/******************************************************//**
 * @brief Fetch and clear status flags of all devices 
 * by issuing a GET_STATUS command simultaneously  
 * to all devices.
 * Then, the fetched status of each device can be retrieved
 * by using the L6470_GetFetchedStatus function
 * provided there is no other calls to functions which 
 * use the SPI in between.
 * @retval None
 *********************************************************/
void L6470_FetchAndClearAllStatus(void)
{
  uint8_t loop;

  for (loop = 0; loop < numberOfDevices; loop++)
  {
     spiTxBursts[0][loop] = L6470_GET_STATUS;
     spiTxBursts[1][loop] = L6470_NOP;
     spiTxBursts[2][loop] = L6470_NOP;
     spiTxBursts[3][loop] = L6470_NOP;
     spiRxBursts[0][loop] = 0;
     spiRxBursts[1][loop] = 0;
     spiRxBursts[2][loop] = 0;
     spiRxBursts[3][loop] = 0;
  }
  for (loop = 0; 
       loop < L6470_CMD_ARG_NB_BYTES_GET_STATUS + 
              L6470_RSP_NB_BYTES_GET_STATUS; 
       loop++)
  {
     L6470_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
  }
}

/******************************************************//**
 * @brief Get the value of the STATUS register which was 
 * fetched by using L6470_FetchAndClearAllStatus.
 * The fetched values are available  as long as there
 * no other calls to functions which use the SPI.
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Last fetched value of the STATUS register
 *********************************************************/
uint16_t L6470_GetFetchedStatus(uint8_t deviceId)
{
  uint16_t status = 0;
  if (numberOfDevices > deviceId)
  {
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    status = (spiRxBursts[1][spiIndex] << 8) | (spiRxBursts[2][spiIndex]);
  }
  return (status);
}

/******************************************************//**
 * @brief Return motor handle (pointer to the L6470 motor driver structure)
 * @retval Pointer to the motorDrv_t structure
 **********************************************************/
motorDrv_t* L6470_GetMotorHandle(void)
{
  return (&l6470Drv);
}
       
/******************************************************//**
 * @brief Returns the FW version of the library
 * @retval L6470_FW_VERSION
 **********************************************************/
uint32_t L6470_GetFwVersion(void)
{
  return (L6470_FW_VERSION);
}

/******************************************************//**
 * @brief  Returns the mark position  of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Mark register value converted in a 32b signed integer 
 **********************************************************/
int32_t L6470_GetMark(uint8_t deviceId)
{
  return L6470_ConvertPosition(L6470_CmdGetParam(deviceId, L6470_MARK));
}

/******************************************************//**
 * @brief Return the number of devices in the daisy chain 
 * @retval number of devices from 1 to MAX_NUMBER_OF_DEVICES
 *********************************************************/
uint8_t L6470_GetNbDevices(void)
{
    return (numberOfDevices);
}

/******************************************************//**
 * @brief Issues L6470 Get Parameter command and convert the result to
 * floating point if meaningfull
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param L6470 register address
 * @retval Register value - 1 to 3 bytes (depends on register)
 *********************************************************/
float L6470_GetAnalogValue(uint8_t deviceId, uint32_t param)
{
  bool voltageMode = ((L6470_CM_VM_CURRENT&L6470_CmdGetParam(deviceId,L6470_STEP_MODE))==0);
  uint32_t registerValue = L6470_CmdGetParam(deviceId, param);
  float value;
  switch (param)
  {
    case L6470_ABS_POS:
    case L6470_MARK:
      value = (float) L6470_ConvertPosition(registerValue);
      break;
    case L6470_ACC:
    case L6470_DEC:
      value = L6470_AccDec_RegVal_to_Steps_s2(registerValue);
      break;
    case L6470_SPEED:
      value = L6470_Speed_RegVal_to_Steps_s(registerValue);
      break;
    case L6470_MAX_SPEED:
      value = L6470_MaxSpd_RegVal_to_Steps_s(registerValue);
      break;
    case L6470_MIN_SPEED:
      registerValue &= L6470_MIN_SPEED_MASK;
      value = L6470_MinSpd_RegVal_to_Steps_s(registerValue);
      break;      
    case L6470_FS_SPD:
      registerValue &= L6470_FS_SPD_MASK;
      value = L6470_FSSpd_RegVal_to_Steps_s(registerValue);
      break;
    case L6470_INT_SPD:
      value = L6470_IntSpd_RegVal_to_Steps_s(registerValue);
      break;
    case L6470_K_THERM:
      value = L6470_KTherm_RegVal_to_Comp(registerValue);
      break;
    case L6470_OCD_TH:
    case L6470_STALL_TH:
      value = L6470_StallOcd_RegVal_to_Th(registerValue);
      break;
    case L6470_KVAL_HOLD:  //L6470_TVAL_HOLD
    case L6470_KVAL_RUN:   //L6470_TVAL_RUN
    case L6470_KVAL_ACC:   //L6470_TVAL_ACC
    case L6470_KVAL_DEC:   //L6470_TVAL_DEC
      if (voltageMode!=FALSE)  value = L6470_Kval_RegVal_to_Perc(registerValue);
      else value = L6470_Tval_RegVal_to_RefVoltage(registerValue);     
      break;
    case L6470_ST_SLP:
      // if (voltageMode==FALSE) 
      // {
      //   break;
      // }      
    case L6470_FN_SLP_ACC: //L6470_TON_MIN
    case L6470_FN_SLP_DEC: //L6470_TOFF_MIN
      if (voltageMode!=FALSE) value = L6470_BEMFslope_RegVal_to_Perc(registerValue);
      else value = L6470_Tmin_RegVal_to_Time(registerValue);
      break;
    default:
      value = (float) registerValue;
  }
  return value;
}

/******************************************************//**
 * @brief  Returns the ABS_POSITION of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval ABS_POSITION register value converted in a 32b signed integer
 **********************************************************/
int32_t L6470_GetPosition(uint8_t deviceId)
{
  return L6470_ConvertPosition(L6470_CmdGetParam(deviceId, L6470_ABS_POS));
}

/******************************************************//**
 * @brief Checks if the specified device is busy
 * by reading the Busy flag bit ot its status Register
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval true if device is busy, false zero
 *********************************************************/
bool L6470_IsDeviceBusy(uint8_t deviceId)
{
  if(!(L6470_ReadStatusRegister(deviceId) & L6470_STATUS_BUSY)) 
  {
    return TRUE;
  }
  else 
  {
    return FALSE;
  }
}
  
/******************************************************//**
 * @brief Put commands in queue before synchronous sending
 * done by calling L6470_SendQueuedCommands.
 * Any call to functions that use the SPI between the calls of 
 * L6470_QueueCommands and L6470_SendQueuedCommands 
 * will corrupt the queue.
 * A command for each device of the daisy chain must be 
 * specified before calling L6470_SendQueuedCommands.
 * @param[in] deviceId deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] command Command to queue (all L6470 commmands 
 * except L6470_SET_PARAM, L6470_GET_PARAM, 
 * L6470_GET_STATUS)
 * @param[in] value argument of the command to queue
 * @retval None
 *********************************************************/
void L6470_QueueCommands(uint8_t deviceId, uint8_t command, int32_t value)
{
  if (numberOfDevices > deviceId)
  {
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    
    switch (command & DAISY_CHAIN_COMMAND_MASK)
    {
      case L6470_RUN: ;
      case L6470_MOVE: ;
      case L6470_GO_TO: ;
      case L6470_GO_TO_DIR: ;
      case L6470_GO_UNTIL: ;
      case L6470_GO_UNTIL_ACT_CPY:
       spiTxBursts[0][spiIndex] = command;
       spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
       spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
       spiTxBursts[3][spiIndex] = (uint8_t)(value);
       break;
      default:
       spiTxBursts[0][spiIndex] = L6470_NOP;
       spiTxBursts[1][spiIndex] = L6470_NOP;
       spiTxBursts[2][spiIndex] = L6470_NOP;
       spiTxBursts[3][spiIndex] = command;
    }
  }
}

/******************************************************//**
 * @brief  Reads the Status Register value
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Status register value
 * @note The status register flags are not cleared 
 * at the difference with L6470_CmdGetStatus()
 **********************************************************/
uint16_t L6470_ReadStatusRegister(uint8_t deviceId)
{
  return (L6470_CmdGetParam(deviceId, L6470_STATUS));
}

/******************************************************//**
 * @brief  Releases the L6470 reset (pin set to High) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 **********************************************************/
void L6470_ReleaseReset(uint8_t deviceId)
{ 
  L6470_Board_ReleaseReset(deviceId); 
}

/******************************************************//**
 * @brief  Resets the L6470 (reset pin set to low) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 **********************************************************/
void L6470_Reset(uint8_t deviceId)
{
  L6470_Board_Reset(deviceId); 
}

/******************************************************//**
 * @brief  Set the stepping mode 
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] stepMode from full step to 1/128 microstep as specified in enum motorStepMode_t
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool L6470_SelectStepMode(uint8_t deviceId, motorStepMode_t stepMode)
{
  uint8_t stepModeRegister;
  l6470_StepSel_t l6470StepMode;

  switch (stepMode)
  {
    case STEP_MODE_FULL:
      l6470StepMode = L6470_STEP_SEL_1;
      break;
    case STEP_MODE_HALF:
      l6470StepMode = L6470_STEP_SEL_1_2;
      break;    
    case STEP_MODE_1_4:
      l6470StepMode = L6470_STEP_SEL_1_4;
      break;        
    case STEP_MODE_1_8:
      l6470StepMode = L6470_STEP_SEL_1_8;
      break;
    case STEP_MODE_1_16:
      l6470StepMode = L6470_STEP_SEL_1_16;
      break;        
    case STEP_MODE_1_32:
      l6470StepMode = L6470_STEP_SEL_1_32;
      break;
    case STEP_MODE_1_64:
      l6470StepMode = L6470_STEP_SEL_1_64;
      break;
    case STEP_MODE_1_128:
      l6470StepMode = L6470_STEP_SEL_1_128;
      break;
    default:
      return FALSE;     
  }
  
  /* Set the L6470 in HiZ state */
  L6470_CmdHardHiZ(deviceId);  
  
  /* Read Step mode register and clear STEP_SEL field */
  stepModeRegister = (uint8_t)(0xF8 & L6470_CmdGetParam(deviceId,L6470_STEP_MODE)) ;
  
  /* Apply new step mode */
  L6470_CmdSetParam(deviceId, L6470_STEP_MODE, stepModeRegister | (uint8_t)l6470StepMode);

  /* Reset abs pos register */
  L6470_CmdResetPos(deviceId);
  
  return TRUE;
}

/******************************************************//**
 * @brief Sends commands stored previously in the queue by 
 * L6470_QueueCommands
 * @retval None
 *********************************************************/
void L6470_SendQueuedCommands(void)
{
  uint8_t loop;
  
  for (loop = 0; 
       loop < L6470_CMD_ARG_MAX_NB_BYTES; 
       loop++)
  {
     L6470_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
  }
}

/******************************************************//**
 * @brief  Sets Home Position 
 * (ABS pos set to current pos minus new home position)
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] homePos new home position
 * @retval None
 **********************************************************/
void L6470_SetHome(uint8_t deviceId, int32_t homePos)
{
  uint32_t currentPos = L6470_CmdGetParam(deviceId, L6470_ABS_POS);
  L6470_CmdSetParam(deviceId, L6470_ABS_POS, currentPos- homePos);
}

/******************************************************//**
 * @brief  Sets Mark position 
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] markPos new mark position
 * @retval None
 **********************************************************/
void L6470_SetMark(uint8_t deviceId, int32_t markPos)
{
  L6470_CmdSetParam(deviceId, L6470_MARK, markPos);
}

/******************************************************//**
 * @brief  Sets the number of devices to be used 
 * @param[in] nbDevices (from 1 to MAX_NUMBER_OF_DEVICES)
 * @retval TRUE if successfull, FALSE if failure, attempt to set a number of 
 * devices greater than MAX_NUMBER_OF_DEVICES
 **********************************************************/
bool L6470_SetNbDevices(uint8_t nbDevices)
{
  if (nbDevices <= MAX_NUMBER_OF_DEVICES)
  {
    numberOfDevices = nbDevices;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/******************************************************//**
 * @brief Issues the SetParam command to the L6470 of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param Register adress (L6470_ABS_POS, L6470_MARK,...)
 * @param[in] value Floating point value to convert and set into the register
 * @retval TRUE if param and value are valid, FALSE otherwise
 *********************************************************/
bool L6470_SetAnalogValue(uint8_t deviceId, uint32_t param, float value)
{
  uint32_t registerValue;
  bool result = TRUE;
  bool voltageMode = ((L6470_CM_VM_CURRENT&L6470_CmdGetParam(deviceId,L6470_STEP_MODE))==0);
  if ((value < 0)&&((param != L6470_ABS_POS)&&(param != L6470_MARK)))
  {
    result = FALSE;
  }
  switch (param)
  {
    case L6470_EL_POS:
      if ((value > (L6470_ELPOS_STEP_MASK|L6470_ELPOS_MICROSTEP_MASK))||
          ((value!=0)&&(value < (1<<(7-(L6470_STEP_MODE_STEP_SEL&L6470_CmdGetParam(0,L6470_STEP_MODE))))))) result = FALSE;
      else registerValue = ((uint32_t) value)&(L6470_ELPOS_STEP_MASK|L6470_ELPOS_MICROSTEP_MASK);
      break;        
    case L6470_ABS_POS:
    case L6470_MARK:
      if (value < 0)
      {
        value=-value;
        if (((uint32_t)value)<=(L6470_MAX_POSITION+1))
          registerValue = (L6470_ABS_POS_VALUE_MASK+1-(uint32_t)value)&L6470_ABS_POS_VALUE_MASK;
        else result = FALSE;  
      }
      else
      {
        if (((uint32_t)value)<=L6470_MAX_POSITION)
          registerValue = ((uint32_t) value)&L6470_ABS_POS_VALUE_MASK;
        else result = FALSE; 
      }
      break;
    case L6470_ACC:
    case L6470_DEC:
      if (value > L6470_ACC_DEC_MAX_VALUE) result = FALSE;
      else registerValue = L6470_AccDec_Steps_s2_to_RegVal(value);
      break;
    case L6470_MAX_SPEED:
      if (value > L6470_MAX_SPEED_MAX_VALUE) result = FALSE;
      else registerValue = L6470_MaxSpd_Steps_s_to_RegVal(value);
      break;
    case L6470_MIN_SPEED:
      if (value > L6470_MIN_SPEED_MAX_VALUE) result = FALSE;
      else registerValue = (L6470_LSPD_OPT&L6470_CmdGetParam(deviceId, param))|L6470_MinSpd_Steps_s_to_RegVal(value);
      break;      
    case L6470_FS_SPD:
      if (value > L6470_FS_SPD_MAX_VALUE) result = FALSE;
      else registerValue = L6470_FSSpd_Steps_s_to_RegVal(value);
      break;
    case L6470_INT_SPD:
      if (value > L6470_INT_SPD_MAX_VALUE) result = FALSE;
      else registerValue = L6470_IntSpd_Steps_s_to_RegVal(value);
      break;
    case L6470_K_THERM:
      if ((value < L6470_K_THERM_MIN_VALUE)||
          (value > L6470_K_THERM_MAX_VALUE)) result = FALSE;
      else registerValue = L6470_KTherm_Comp_to_RegVal(value);
      break;
    case L6470_OCD_TH:
    case L6470_STALL_TH:
      if (value > L6470_STALL_OCD_TH_MAX_VALUE) result = FALSE;
      else registerValue = L6470_StallOcd_Th_to_RegVal(value);
      break;
    case L6470_KVAL_HOLD:  //L6470_TVAL_HOLD
    case L6470_KVAL_RUN:   //L6470_TVAL_RUN
    case L6470_KVAL_ACC:   //L6470_TVAL_ACC
    case L6470_KVAL_DEC:   //L6470_TVAL_DEC
      if (voltageMode==FALSE)
      {
        if (value > L6470_TVAL_MAX_VALUE) result = FALSE;
        else registerValue = L6470_Tval_RefVoltage_to_RegVal(value);
      }
      else
      {
        if (value > L6470_KVAL_MAX_VALUE) result = FALSE;
        else registerValue = L6470_Kval_Perc_to_RegVal(value);
      }
      break;
    case L6470_ST_SLP:
      // if (voltageMode==FALSE) 
      // {
      //   result = FALSE;
      //   break;
      // }
    case L6470_FN_SLP_ACC: //L6470_TON_MIN
    case L6470_FN_SLP_DEC: //L6470_TOFF_MIN
      if (voltageMode==FALSE)
      {
        if (value>L6470_TOFF_TON_MIN_MAX_VALUE) result = FALSE;
        else registerValue = L6470_Tmin_Time_to_RegVal(value);
      }
      else
      {
        if (value > L6470_SLP_MAX_VALUE) result = FALSE;
        else registerValue = L6470_BEMFslope_Perc_to_RegVal(value);
      }
      break;
    default:
      result = FALSE;
  }
  if (result!=FALSE)
  {
    L6470_CmdSetParam(deviceId, param, registerValue);
  }
  return result;
}

/******************************************************//**
 * @brief  Start the step clock by using the given frequency
 * @param[in] newFreq in Hz of the step clock
 * @retval None
 * @note The frequency is directly the current speed of the device
 **********************************************************/
void L6470_StartStepClock(uint16_t newFreq)
{
  L6470_Board_StartStepClock(newFreq);
}

/******************************************************//**
 * @brief  Stops the PWM uses for the step clock
 * @retval None
 **********************************************************/
void L6470_StopStepClock(void)
{
  L6470_Board_StopStepClock();
}

/******************************************************//**
 * @brief  Locks until all devices become not busy
 * @retval None
 **********************************************************/
void L6470_WaitForAllDevicesNotBusy(void)
{
  bool busy = TRUE;
  uint8_t loop;
	
  /* Wait while at least one is active */
  while (busy)
  {
    busy = FALSE;
    for (loop = 0; loop < numberOfDevices; loop++)
    {
      busy  |= L6470_IsDeviceBusy(loop);
    }   
  }
}

/******************************************************//**
 * @brief  Locks until the device becomes not busy
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 **********************************************************/
void L6470_WaitWhileActive(uint8_t deviceId)
 {
	/* Wait while motor is running */
	while (L6470_IsDeviceBusy(deviceId) != 0);
}

/**
  * @}
  */

/** @addtogroup L6470_Private_Functions
  * @{
  */
/******************************************************//**
 * @brief  Converts the ABS_POSITION register value to a 32b signed integer
 * @param[in] abs_position_reg value of the ABS_POSITION register
 * @retval operation_result 32b signed integer corresponding to the absolute position 
 **********************************************************/
int32_t L6470_ConvertPosition(uint32_t abs_position_reg)
{
	int32_t operation_result;

  if (abs_position_reg & L6470_ABS_POS_SIGN_BIT_MASK) 
  {
		/* Negative register value */
		abs_position_reg = ~abs_position_reg;
		abs_position_reg += 1;

		operation_result = (int32_t) (abs_position_reg & L6470_ABS_POS_VALUE_MASK);
		operation_result = -operation_result;
  } 
  else 
  {
		operation_result = (int32_t) abs_position_reg;
	}
	return operation_result;
}

/******************************************************//**
 * @brief  Handlers of the busy interrupt which calls the user callback (if defined)
 * @retval None
 **********************************************************/
void L6470_BusyInterruptHandler(void)
{
  if (busyInterruptCallback != 0)
  {
    /* Set isr flag */
    isrFlag = TRUE;
    
    busyInterruptCallback();
    
    /* Reset isr flag */
    isrFlag = FALSE;   
  }
}

/******************************************************//**
 * @brief Error handler which calls the user callback (if defined)
 * @param[in] error Number of the error
 * @retval None
 **********************************************************/
void L6470_ErrorHandler(uint16_t error)
{
  if (errorHandlerCallback != 0)
  {
    errorHandlerCallback(error);
  }
  else   
  {
    while(1)
    {
      /* Infinite loop */
    }
  }
}

/******************************************************//**
 * @brief  Handlers of the flag interrupt which calls the user callback (if defined)
 * @retval None
 **********************************************************/
void L6470_FlagInterruptHandler(void)
{
  if (flagInterruptCallback != 0)
  {
    /* Set isr flag */
    isrFlag = TRUE;
    
    flagInterruptCallback();
    
    /* Reset isr flag */
    isrFlag = FALSE;   
  }
}
                          
/******************************************************//**
 * @brief  Sends a command to a given device Id via the SPI
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param Command to send (all L6470 commmands 
 * except L6470_SET_PARAM, L6470_GET_PARAM, 
 * L6470_GET_STATUS)
 * @param[in] value arguments to send on 32 bits
 * @retval None
 **********************************************************/
void L6470_SendCommand(uint8_t deviceId, uint8_t param, uint32_t value)
{
  if (numberOfDevices > deviceId)
  {
    uint32_t loop;
    uint8_t maxArgumentNbBytes = 0;
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    bool itDisable = FALSE; 
    
    do
    {
      spiPreemtionByIsr = FALSE;
      if (itDisable)
      {
        /* re-enable L6470_Board_EnableIrq if disable in previous iteration */
        L6470_Board_EnableIrq();
        itDisable = FALSE;
      }    
      for (loop = 0; loop < numberOfDevices; loop++)
      {
          spiTxBursts[0][loop] = L6470_NOP;
          spiTxBursts[1][loop] = L6470_NOP;
          spiTxBursts[2][loop] = L6470_NOP;
          spiTxBursts[3][loop] = L6470_NOP;   
      }
      switch (param & DAISY_CHAIN_COMMAND_MASK)
      {
        case L6470_GO_TO:
        case L6470_GO_TO_DIR:
          value = value & L6470_ABS_POS_VALUE_MASK;
        case L6470_RUN:
        case L6470_MOVE:
        case L6470_GO_UNTIL:
        case L6470_GO_UNTIL_ACT_CPY:
          spiTxBursts[0][spiIndex] = param;
          spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
          spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
          spiTxBursts[3][spiIndex] = (uint8_t)(value);
          maxArgumentNbBytes = 3;
          break;
      default:
          spiTxBursts[0][spiIndex] = L6470_NOP;
          spiTxBursts[1][spiIndex] = L6470_NOP;
          spiTxBursts[2][spiIndex] = L6470_NOP;
          spiTxBursts[3][spiIndex] = param;
      }
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      L6470_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR
    for (loop = L6470_CMD_ARG_MAX_NB_BYTES - 1 - maxArgumentNbBytes; 
         loop < L6470_CMD_ARG_MAX_NB_BYTES; 
         loop++)
    {
       L6470_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
    }
    /* re-enable L6470_Board_EnableIrq after SPI transfers*/
    L6470_Board_EnableIrq();
  }
}

/******************************************************//**
 * @brief  Sets the registers of the L6470 to their predefined values 
 * from l6470_target_config.h
 * @param[in] deviceId (from 0 to 2)
 * @retval None
 **********************************************************/
void L6470_SetRegisterToPredefinedValues(uint8_t deviceId)
{  
  L6470_CmdSetParam(deviceId, L6470_ABS_POS, 0);
  L6470_CmdSetParam(deviceId, L6470_EL_POS, 0);
  L6470_CmdSetParam(deviceId, L6470_MARK, 0);
  
  switch (deviceId)
  {
    case 0:
      L6470_CmdSetParam(deviceId, L6470_ACC,
        L6470_AccDec_Steps_s2_to_RegVal(
          L6470_CONF_PARAM_ACC_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_DEC,
        L6470_AccDec_Steps_s2_to_RegVal(
          L6470_CONF_PARAM_DEC_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_MAX_SPEED,
        L6470_MaxSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_MAX_SPEED_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_MIN_SPEED,
        L6470_CONF_PARAM_LSPD_BIT_DEVICE_0|
        L6470_MinSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_MIN_SPEED_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_FS_SPD,
        L6470_FSSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_FS_SPD_DEVICE_0)); 
      L6470_CmdSetParam(deviceId, L6470_OCD_TH,
        (uint8_t)L6470_CONF_PARAM_OCD_TH_DEVICE_0);
      L6470_CmdSetParam(deviceId, L6470_STEP_MODE,
        (uint8_t)L6470_CONF_PARAM_SYNC_MODE_DEVICE_0 |
        (uint8_t)L6470_CONF_PARAM_CM_VM_DEVICE_0|
        (uint8_t)L6470_CONF_PARAM_STEP_MODE_DEVICE_0);
      L6470_CmdSetParam(deviceId, L6470_ALARM_EN,
        L6470_CONF_PARAM_ALARM_EN_DEVICE_0);
      L6470_CmdSetParam(deviceId, L6470_INT_SPD,
        L6470_IntSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_INT_SPD_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_K_THERM,
        L6470_KTherm_Comp_to_RegVal(
          L6470_CONF_PARAM_K_THERM_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_STALL_TH,
        L6470_StallOcd_Th_to_RegVal(
          L6470_CONF_PARAM_STALL_TH_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_KVAL_HOLD,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_HOLD_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_KVAL_RUN,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_RUN_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_KVAL_ACC,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_ACC_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_KVAL_DEC,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_DEC_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_ST_SLP,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_ST_SLP_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_FN_SLP_ACC,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_FN_SLP_ACC_DEVICE_0));  
      L6470_CmdSetParam(deviceId, L6470_FN_SLP_DEC,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_FN_SLP_DEC_DEVICE_0));
      L6470_CmdSetParam(deviceId, L6470_CONFIG,
        (uint16_t)L6470_CONF_PARAM_CLOCK_SETTING_DEVICE_0 | 
        (uint16_t)L6470_CONF_PARAM_SW_MODE_DEVICE_0       | 
        (uint16_t)L6470_CONF_PARAM_VS_COMP_DEVICE_0       | 
        (uint16_t)L6470_CONF_PARAM_OC_SD_DEVICE_0         |
        (uint16_t)L6470_CONF_PARAM_POW_SR_DEVICE_0        | 
        (uint16_t)L6470_CONF_PARAM_PWM_DIV_DEVICE_0       | 
        (uint16_t)L6470_CONF_PARAM_PWM_MUL_DEVICE_0);
      break;

#if (MAX_NUMBER_OF_DEVICES > 1)
   case 1:
      L6470_CmdSetParam(deviceId, L6470_ACC,
        L6470_AccDec_Steps_s2_to_RegVal(
          L6470_CONF_PARAM_ACC_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_DEC,
        L6470_AccDec_Steps_s2_to_RegVal(
          L6470_CONF_PARAM_DEC_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_MAX_SPEED,
        L6470_MaxSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_MAX_SPEED_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_MIN_SPEED,
        L6470_CONF_PARAM_LSPD_BIT_DEVICE_1|
        L6470_MinSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_MIN_SPEED_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_FS_SPD,
        L6470_FSSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_FS_SPD_DEVICE_1)); 
      L6470_CmdSetParam(deviceId, L6470_OCD_TH,
        (uint8_t)L6470_CONF_PARAM_OCD_TH_DEVICE_1);
      L6470_CmdSetParam(deviceId, L6470_STEP_MODE,
        (uint8_t)L6470_CONF_PARAM_SYNC_MODE_DEVICE_1 |
        (uint8_t)L6470_CONF_PARAM_CM_VM_DEVICE_1|
        (uint8_t)L6470_CONF_PARAM_STEP_MODE_DEVICE_1);
      L6470_CmdSetParam(deviceId, L6470_ALARM_EN,
        L6470_CONF_PARAM_ALARM_EN_DEVICE_1);
      L6470_CmdSetParam(deviceId, L6470_INT_SPD,
        L6470_IntSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_INT_SPD_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_K_THERM,
        L6470_KTherm_Comp_to_RegVal(
          L6470_CONF_PARAM_K_THERM_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_STALL_TH,
        L6470_StallOcd_Th_to_RegVal(
          L6470_CONF_PARAM_STALL_TH_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_KVAL_HOLD,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_HOLD_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_KVAL_RUN,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_RUN_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_KVAL_ACC,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_ACC_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_KVAL_DEC,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_DEC_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_ST_SLP,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_ST_SLP_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_FN_SLP_ACC,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_FN_SLP_ACC_DEVICE_1));  
      L6470_CmdSetParam(deviceId, L6470_FN_SLP_DEC,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_FN_SLP_DEC_DEVICE_1));
      L6470_CmdSetParam(deviceId, L6470_CONFIG,
        (uint16_t)L6470_CONF_PARAM_CLOCK_SETTING_DEVICE_1 | 
        (uint16_t)L6470_CONF_PARAM_SW_MODE_DEVICE_1       | 
        (uint16_t)L6470_CONF_PARAM_VS_COMP_DEVICE_1       | 
        (uint16_t)L6470_CONF_PARAM_OC_SD_DEVICE_1         | 
        (uint16_t)L6470_CONF_PARAM_POW_SR_DEVICE_1        | 
        (uint16_t)L6470_CONF_PARAM_PWM_DIV_DEVICE_1       | 
        (uint16_t)L6470_CONF_PARAM_PWM_MUL_DEVICE_1);
      break;     

#endif
#if (MAX_NUMBER_OF_DEVICES > 2)
   case 2:
      L6470_CmdSetParam(deviceId, L6470_ACC,
        L6470_AccDec_Steps_s2_to_RegVal(
          L6470_CONF_PARAM_ACC_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_DEC,
        L6470_AccDec_Steps_s2_to_RegVal(
          L6470_CONF_PARAM_DEC_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_MAX_SPEED,
        L6470_MaxSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_MAX_SPEED_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_MIN_SPEED,
        L6470_CONF_PARAM_LSPD_BIT_DEVICE_2|
        L6470_MinSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_MIN_SPEED_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_FS_SPD,
        L6470_FSSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_FS_SPD_DEVICE_2)); 
      L6470_CmdSetParam(deviceId, L6470_OCD_TH,
        (uint8_t)L6470_CONF_PARAM_OCD_TH_DEVICE_2);
      L6470_CmdSetParam(deviceId, L6470_STEP_MODE,
        (uint8_t)L6470_CONF_PARAM_SYNC_MODE_DEVICE_2 |
        (uint8_t)L6470_CONF_PARAM_CM_VM_DEVICE_2|
        (uint8_t)L6470_CONF_PARAM_STEP_MODE_DEVICE_2);
      L6470_CmdSetParam(deviceId, L6470_ALARM_EN,
        L6470_CONF_PARAM_ALARM_EN_DEVICE_2);
      L6470_CmdSetParam(deviceId, L6470_INT_SPD,
        L6470_IntSpd_Steps_s_to_RegVal(
          L6470_CONF_PARAM_INT_SPD_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_K_THERM,
        L6470_KTherm_Comp_to_RegVal(
          L6470_CONF_PARAM_K_THERM_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_STALL_TH,
        L6470_StallOcd_Th_to_RegVal(
          L6470_CONF_PARAM_STALL_TH_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_KVAL_HOLD,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_HOLD_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_KVAL_RUN,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_RUN_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_KVAL_ACC,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_ACC_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_KVAL_DEC,
        L6470_Kval_Perc_to_RegVal(
          L6470_CONF_PARAM_KVAL_DEC_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_ST_SLP,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_ST_SLP_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_FN_SLP_ACC,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_FN_SLP_ACC_DEVICE_2));  
      L6470_CmdSetParam(deviceId, L6470_FN_SLP_DEC,
        L6470_BEMFslope_Perc_to_RegVal(
          L6470_CONF_PARAM_FN_SLP_DEC_DEVICE_2));
      L6470_CmdSetParam(deviceId, L6470_CONFIG,
        (uint16_t)L6470_CONF_PARAM_CLOCK_SETTING_DEVICE_2 | 
        (uint16_t)L6470_CONF_PARAM_SW_MODE_DEVICE_2       | 
        (uint16_t)L6470_CONF_PARAM_VS_COMP_DEVICE_2       | 
        (uint16_t)L6470_CONF_PARAM_OC_SD_DEVICE_2         |
        (uint16_t)L6470_CONF_PARAM_POW_SR_DEVICE_2        |
        (uint16_t)L6470_CONF_PARAM_PWM_DIV_DEVICE_2       | 
        (uint16_t)L6470_CONF_PARAM_PWM_MUL_DEVICE_2);
      break;
#endif
    default: ;
  }
}

/******************************************************//**
 * @brief  Set the parameters of the device to values of initPrm structure
 * @param[in] deviceId (from 0 to 2)
 * @param initPrm structure containing values to initialize the device 
 * parameters
 * @retval None
 **********************************************************/
void L6470_SetDeviceParamsToGivenValues(uint8_t deviceId,
                                              l6470_Init_u_t *initPrm)
{
  L6470_CmdSetParam(deviceId, L6470_ABS_POS, 0);
  L6470_CmdSetParam(deviceId, L6470_EL_POS, 0);
  L6470_CmdSetParam(deviceId, L6470_MARK, 0);
  L6470_CmdSetParam(deviceId, L6470_ACC,
    L6470_AccDec_Steps_s2_to_RegVal(initPrm->vm.cp.acceleration));
  L6470_CmdSetParam(deviceId, L6470_DEC,
    L6470_AccDec_Steps_s2_to_RegVal(initPrm->vm.cp.deceleration));
  L6470_CmdSetParam(deviceId, L6470_MAX_SPEED,
    L6470_MaxSpd_Steps_s_to_RegVal(initPrm->vm.cp.maxSpeed));
  L6470_CmdSetParam(deviceId, L6470_MIN_SPEED,
    initPrm->vm.cp.lowSpeedOptimization|
    L6470_MinSpd_Steps_s_to_RegVal(initPrm->vm.cp.minSpeed));
  L6470_CmdSetParam(deviceId, L6470_FS_SPD,
    L6470_FSSpd_Steps_s_to_RegVal(initPrm->vm.cp.fullStepSpeed));
  L6470_CmdSetParam(deviceId, L6470_OCD_TH,
    (uint8_t)initPrm->vm.cp.ocdThreshold);
  L6470_CmdSetParam(deviceId, L6470_STEP_MODE,
    (uint8_t)initPrm->vm.cp.syncClockSelection|
    (uint8_t)initPrm->vm.cp.cmVmSelection|
    (uint8_t)(uint8_t)initPrm->vm.cp.stepMode);
  L6470_CmdSetParam(deviceId, L6470_ALARM_EN,
    initPrm->vm.cp.alarmsSelection);
  L6470_CmdSetParam(deviceId, L6470_INT_SPD,
    L6470_IntSpd_Steps_s_to_RegVal(
      initPrm->vm.intersectSpeed));
  L6470_CmdSetParam(deviceId, L6470_K_THERM,
    L6470_KTherm_Comp_to_RegVal(
      initPrm->vm.thermalCompensationFactor)); 
  L6470_CmdSetParam(deviceId, L6470_STALL_TH,
    L6470_StallOcd_Th_to_RegVal(
      initPrm->vm.stallThreshold));
  L6470_CmdSetParam(deviceId, L6470_KVAL_HOLD,
    L6470_Kval_Perc_to_RegVal(
      initPrm->vm.kvalHold));
  L6470_CmdSetParam(deviceId, L6470_KVAL_RUN,
    L6470_Kval_Perc_to_RegVal(
      initPrm->vm.kvalRun));
  L6470_CmdSetParam(deviceId, L6470_KVAL_ACC,
    L6470_Kval_Perc_to_RegVal(
      initPrm->vm.kvalAcc));
  L6470_CmdSetParam(deviceId, L6470_KVAL_DEC,
    L6470_Kval_Perc_to_RegVal(
      initPrm->vm.kvalDec));
  L6470_CmdSetParam(deviceId, L6470_ST_SLP,
    L6470_BEMFslope_Perc_to_RegVal(
      initPrm->vm.startSlope));
  L6470_CmdSetParam(deviceId, L6470_FN_SLP_ACC,
    L6470_BEMFslope_Perc_to_RegVal(
      initPrm->vm.accelerationFinalSlope));
  L6470_CmdSetParam(deviceId, L6470_FN_SLP_DEC,
    L6470_BEMFslope_Perc_to_RegVal(
      initPrm->vm.decelerationFinalSlope));
  L6470_CmdSetParam(deviceId, L6470_CONFIG,
    (uint16_t)initPrm->vm.oscClkSel| 
    (uint16_t)initPrm->vm.swMode | 
    (uint16_t)initPrm->vm.enVsComp| 
    (uint16_t)initPrm->vm.ocSd|
    (uint16_t)initPrm->vm.slewRate|
    (uint16_t)initPrm->vm.fPwmInt| 
    (uint16_t)initPrm->vm.fPwmDec);
}

/******************************************************//**
 * @brief Write and receive a byte via SPI
 * @param[in] pByteToTransmit pointer to the byte to transmit
 * @param[in] pReceivedByte pointer to the received byte
 * @retval None
 *********************************************************/
void L6470_WriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte)
{
  if (L6470_Board_SpiWriteBytes(pByteToTransmit, pReceivedByte, numberOfDevices) != 0)
  {
    L6470_ErrorHandler(L6470_ERROR_1);
  }
  
  if (isrFlag)
  {
    spiPreemtionByIsr = TRUE;
  }
}                     

/**********************************************************
 * @brief Convert the float formatted acceleration or
 * deceleration into respectively an ACC or DEC register value
 * @param[in] steps_s2 the acceleration or deceleration as
 * steps/s^2, range 14.55 to 59590 steps/s^2
 * @retval The acceleration or deceleration as steps/tick^2
 **********************************************************/
inline uint16_t L6470_AccDec_Steps_s2_to_RegVal(float steps_s2)
{
  return ((uint16_t)(((float)(steps_s2)*0.068719476736f)+0.5f));
}

/**********************************************************
 * @brief Convert the ACC or DEC register value into step/s^2
 * @param[in] regVal The ACC or DEC register value
 * @retval The speed as steps/s
 **********************************************************/
inline float L6470_AccDec_RegVal_to_Steps_s2(uint32_t regVal)
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
inline uint8_t L6470_BEMFslope_Perc_to_RegVal(float percentage)
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
inline float L6470_BEMFslope_RegVal_to_Perc(uint32_t regVal)
{
  return (((float)(regVal))*0.00156862745098f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a FS_SPD 
 * register value
 * @param[in] steps_s the speed as steps/s, range 15.25 to 15610 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint16_t L6470_FSSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)((float)(steps_s)*0.065536f));
}

/**********************************************************
 * @brief Convert the FS_SPD register value into step/s
 * @param[in] regVal The FS_SPD register value
 * @retval The full Step speed as steps/s
 **********************************************************/
inline float L6470_FSSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)regVal+0.999f)*15.258789f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a INT_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 976.5 steps/s
 * @retval The intersect speed as steps/tick
 **********************************************************/
inline uint16_t L6470_IntSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*16.777216f)+0.5f));
}

/**********************************************************
 * @brief Convert the INT_SPEED register value into step/s
 * @param[in] regVal The INT_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float L6470_IntSpd_RegVal_to_Steps_s(uint32_t regVal)
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
inline uint8_t L6470_KTherm_Comp_to_RegVal(float compFactor)
{
  return ((uint8_t)((((float)(compFactor)-1.0f)*32.0f)+0.5f));
}

/**********************************************************
 * @brief Convert the K_THERM register value into a float 
 * formatted thermal compensation factor
 * @param[in] regVal The K_THERM register value
 * @retval The float formatted thermal compensation factor
 **********************************************************/
inline float L6470_KTherm_RegVal_to_Comp(uint32_t regVal)
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
inline uint8_t L6470_Kval_Perc_to_RegVal(float percentage)
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
inline float L6470_Kval_RegVal_to_Perc(uint32_t regVal)
{
  return (((float)(regVal))*0.390625f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a MAX_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 15.25 to 15610 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint16_t L6470_MaxSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*0.065536f)+0.5f));
}

/**********************************************************
 * @brief Convert the MAX_SPEED register value into step/s
 * @param[in] regVal The MAX_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float L6470_MaxSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*15.258789f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a MIN_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 976.3 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint16_t L6470_MinSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*4.194304f)+0.5f));
}

/**********************************************************
 * @brief Convert the MIN_SPEED register value into step/s
 * @param[in] regVal The MIN_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float L6470_MinSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.238418579f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 15625 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint32_t L6470_Speed_Steps_s_to_RegVal(float steps_s)
{
  return ((uint32_t)(((float)(steps_s)*67.108864f)+0.5f));
}

/**********************************************************
 * @brief Convert the SPEED register value into step/s
 * @param[in] regVal The SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float L6470_Speed_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.01490116119f);
}

/**********************************************************
 * @brief Convert STALL or OCD Threshold voltage in mV to 
 * values for STALL_TH or OCD_TH register
 * @param[in] mV voltage in mV, range 31.25mV to 1000mV
 * @retval value for STALL_TH or OCD_TH register
 **********************************************************/
inline uint8_t L6470_StallOcd_Th_to_RegVal(float mV)
{
  return ((uint8_t)((((float)(mV)-31.25f)*0.032f)+0.5f));
}

/**********************************************************
 * @brief Convert values from STALL_TH or OCD_TH register 
 * to mV
 * @param[in] regVal The STALL_TH or OCD_TH register value
 * @retval voltage in mV
 **********************************************************/
inline float L6470_StallOcd_RegVal_to_Th(uint32_t regVal)
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
inline uint8_t L6470_Tval_RefVoltage_to_RegVal(float voltage_mV)
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
inline float L6470_Tval_RegVal_to_RefVoltage(uint32_t regVal)
{
  return (((float)(regVal+1))*7.8125f);
}

/**********************************************************
 * @brief Convert time in us to values for TON_MIN register
 * @param[in] tmin_us time in us, range 0.5us to 64us
 * @retval value for TON_MIN register
 **********************************************************/
inline uint8_t L6470_Tmin_Time_to_RegVal(float tmin_us)
{
  return ((uint8_t)((((float)(tmin_us)-0.5f)*2.0f)+0.5f));
}

/**********************************************************
 * @brief Convert values for TON_MIN or TOFF_MIN register to time in us
 * @param[in] regVal The TON_MIN or TOFF_MIN register value
 * @retval time in us
 **********************************************************/
inline float L6470_Tmin_RegVal_to_Time(uint32_t regVal)
{
  return (((float)(regVal+1))*0.5f);
}


/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
