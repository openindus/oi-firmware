/**************************************************************************//**
  * @file    powerstep01.c 
  * @author  IPC Rennes
  * @version V1.6.0
  * @date    June 4th, 2018
  * @brief   Powerstep01 motor driver (Microstepping controller with power MOSFETs)
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

#include "powerSTEP01/powerSTEP01.h"

/** @addtogroup BSP
  * @{
  */   
   
/** @addtogroup POWERSTEP01
  * @{
  */   

/* Private constants ---------------------------------------------------------*/

/** @addtogroup Powerstep01_Private_Constants
  * @{
  */
/// Error while initialising the SPI
#define POWERSTEP01_ERROR_0   (0xB000)
/// Error: Bad SPI transaction
#define POWERSTEP01_ERROR_1   (0xB001)

/**
  * @}
  */ 

/* Private variables ---------------------------------------------------------*/

/** @addtogroup Powerstep01_Private_Variables
  * @{
  */
  
/// Function pointer to busy interrupt call back
void (*busyInterruptCallback)(void);
/// Function pointer to flag interrupt call back
void (*flagInterruptCallback)(void);
/// Function pointer to error handler call back
void (*errorHandlerCallback)(uint16_t);
static volatile uint8_t numberOfDevices;
static uint8_t spiTxBursts[POWERSTEP01_CMD_ARG_MAX_NB_BYTES][MAX_NUMBER_OF_DEVICES];
static uint8_t spiRxBursts[POWERSTEP01_CMD_ARG_MAX_NB_BYTES][MAX_NUMBER_OF_DEVICES];
static volatile bool spiPreemtionByIsr = FALSE;
static volatile bool isrFlag = FALSE;
static uint16_t powerstep01DriverInstance = 0;

/**
  * @}
  */ 

/* Private function prototypes -----------------------------------------------*/

/** @defgroup Powerstep01_Private_Functions Private Functions
  * @{
  */  
int32_t Powerstep01_ConvertPosition(uint32_t abs_position_reg);
void Powerstep01_ErrorHandler(uint16_t error);
void Powerstep01_SendCommand(uint8_t deviceId, uint8_t command, uint32_t value);
void Powerstep01_SetRegisterToPredefinedValues(uint8_t deviceId);
void Powerstep01_SetDeviceParamsToGivenValues(uint8_t deviceId, powerstep01_Init_u_t *initPrm);
void Powerstep01_WriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte);

/**
 * @brief To and from register parameter conversion functions
 */
///Convert the float formatted acceleration or deceleration into respectively an ACC or DEC register value
uint16_t Powerstep01_AccDec_Steps_s2_to_RegVal(float steps_s2);
///Convert the ACC or DEC register value into step/s^2
float Powerstep01_AccDec_RegVal_to_Steps_s2(uint32_t regVal);
///Convert BEMF compensation slope to values for ST_SLP, FN_SLP_ACC or FN_SLP_DEC register
uint8_t Powerstep01_BEMFslope_Perc_to_RegVal(float percentage);
///Convert values from ST_SLP, FN_SLP_ACC or FN_SLP_DEC register to BEMF compensation slope percentage
float Powerstep01_BEMFslope_RegVal_to_Perc(uint32_t regVal);
///Convert the float formatted speed into a FS_SPD register value
uint16_t Powerstep01_FSSpd_Steps_s_to_RegVal(float steps_s);
///Convert the FS_SPD register value into step/s
float Powerstep01_FSSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted speed into a INT_SPEED register value
uint16_t Powerstep01_IntSpd_Steps_s_to_RegVal(float steps_s);
///Convert the INT_SPEED register value into step/s
float Powerstep01_IntSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted thermal compensation factor into a K_THEM register value
uint8_t Powerstep01_KTherm_Comp_to_RegVal(float compFactor);
///Convert the K_THERM register value into a float formatted thermal compensation factor
float Powerstep01_KTherm_RegVal_to_Comp(uint32_t regVal);
///Convert voltage in percentage to values for KVAL_RUN, KVAL_HOLD, KVAL_ACC or KVAL_DEC register
uint8_t Powerstep01_Kval_Perc_to_RegVal(float percentage);
///Convert values from KVAL_RUN, KVAL_HOLD, KVAL_ACC or KVAL_DEC register to percentage
float Powerstep01_Kval_RegVal_to_Perc(uint32_t regVal);
///Convert the float formatted speed into a MAX_SPEED register value
uint16_t Powerstep01_MaxSpd_Steps_s_to_RegVal(float steps_s);
///Convert the MAX_SPEED register value into step/s
float Powerstep01_MaxSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted speed into a MIN_SPEED register value
uint16_t Powerstep01_MinSpd_Steps_s_to_RegVal(float steps_s);
///Convert the MIN_SPEED register value into step/s
float Powerstep01_MinSpd_RegVal_to_Steps_s(uint32_t regVal);
///Convert the float formatted speed into a SPEED register value
uint32_t Powerstep01_Speed_Steps_s_to_RegVal(float steps_s);
///Convert the SPEED register value into step/s
float Powerstep01_Speed_RegVal_to_Steps_s(uint32_t regVal);
///Converts STALL or OCD Threshold voltage in mV to values for STALL_TH or OCD_TH register
uint8_t Powerstep01_StallOcd_Th_to_RegVal(float mV);
///Convert values from STALL_TH or OCD_TH register to mV
float Powerstep01_StallOcd_RegVal_to_Th(uint32_t regVal);
///Convert voltage in mV to values for TVAL_RUN, TVAL_HOLD, TVAL_ACC or TVAL_DEC register
uint8_t Powerstep01_Tval_RefVoltage_to_RegVal(float voltage_mV);
///Convert values from TVAL_RUN, TVAL_HOLD, TVAL_ACC or TVAL_DEC register to mV
float Powerstep01_Tval_RegVal_to_RefVoltage(uint32_t regVal);
///Convert time in us to values for TON_MIN register
uint8_t Powerstep01_Tmin_Time_to_RegVal(float tmin_us);
///Convert values for TON_MIN or TOFF_MIN register to time in us
float Powerstep01_Tmin_RegVal_to_Time(uint32_t regVal);

/**
  * @}
  */ 

/** @defgroup Powerstep01_Exported_Variables Exported Variables
  * @{
  */

/// powerSTEP01 motor driver functions pointer structure
motorDrv_t   powerstep01Drv= 
{
  Powerstep01_Init,                    //void (*Init)(void*);
  Powerstep01_ReadId,                  //uint16_t (*ReadID)(void);
  Powerstep01_AttachErrorHandler,      //void (*AttachErrorHandler)(void (*callback)(uint16_t));
  Powerstep01_AttachFlagInterrupt,     //void (*AttachFlagInterrupt)(void (*callback)(void));
  Powerstep01_AttachBusyInterrupt,     //void (*AttachBusyInterrupt)(void (*callback)(void));
  Powerstep01_FlagInterruptHandler,    //void (*FlagInterruptHandler)(void);
  0,                                   //uint16_t (*GetAcceleration)(uint8_t);
  0,                                   //uint16_t (*GetCurrentSpeed)(uint8_t);
  0,                                   //uint16_t (*GetDeceleration)(uint8_t);
  0,                                   //motorState_t(*GetDeviceState)(uint8_t);
  Powerstep01_GetFwVersion,            //uint32_t (*GetFwVersion)(void);
  Powerstep01_GetMark,                 //int32_t (*GetMark)(uint8_t);
  0,                                   //uint16_t (*GetMaxSpeed)(uint8_t);
  0,                                   //uint16_t (*GetMinSpeed)(uint8_t);
  Powerstep01_GetPosition,             //int32_t (*GetPosition)(uint8_t);
  Powerstep01_CmdGoHome,               //void (*GoHome)(uint8_t);
  Powerstep01_CmdGoMark,               //void (*GoMark)(uint8_t);
  Powerstep01_CmdGoTo,                 //void (*GoTo)(uint8_t, int32_t);
  Powerstep01_CmdHardStop,             //void (*HardStop)(uint8_t);
  Powerstep01_CmdMove,                 //void (*Move)(uint8_t, motorDir_t, uint32_t );
  0,                                   //void (*ResetAllDevices)(void);
  0,                                   //void (*Run)(uint8_t, motorDir_t);
  0,                                   //bool(*SetAcceleration)(uint8_t ,uint16_t );
  0,                                   //bool(*SetDeceleration)(uint8_t , uint16_t );
  Powerstep01_SetHome,                 //void (*SetHome)(uint8_t, int32_t);
  Powerstep01_SetMark,                 //void (*SetMark)(uint8_t, int32_t);
  0,                                   //bool (*SetMaxSpeed)(uint8_t, uint16_t );
  0,                                   //bool (*SetMinSpeed)(uint8_t, uint16_t );
  0,                                   //bool (*SoftStop)(uint8_t);
  0,                                   //void (*StepClockHandler)(uint8_t deviceId);
  Powerstep01_WaitWhileActive,         //void (*WaitWhileActive)(uint8_t);
  0,                                   //void (*CmdDisable)(uint8_t);
  0,                                   //void (*CmdEnable)(uint8_t);
  Powerstep01_CmdGetParam,             //uint32_t (*CmdGetParam)(uint8_t, uint32_t);
  Powerstep01_CmdGetStatus,            //uint16_t (*CmdGetStatus)(uint8_t);
  Powerstep01_CmdNop,                  //void (*CmdNop)(uint8_t);
  Powerstep01_CmdSetParam,             //void (*CmdSetParam)(uint8_t, uint32_t, uint32_t);
  Powerstep01_ReadStatusRegister,      //uint16_t (*ReadStatusRegister)(uint8_t);
  Powerstep01_ReleaseReset,            //void (*ReleaseReset)(uint8_t);
  Powerstep01_Reset,                   //void (*Reset)(uint8_t);
  Powerstep01_SelectStepMode,          //bool (*SelectStepMode)(uint8_t deviceId, motorStepMode_t);
  0,                                   //void (*SetDirection)(uint8_t, motorDir_t);
  Powerstep01_CmdGoToDir,              //void (*CmdGoToDir)(uint8_t, motorDir_t, int32_t);
  Powerstep01_CheckBusyHw,             //uint8_t (*CheckBusyHw)(void);
  Powerstep01_CheckStatusHw,           //uint8_t (*CheckStatusHw)(void);
  Powerstep01_CmdGoUntil,              //void (*CmdGoUntil)(uint8_t, motorAction_t, motorDir_t, uint32_t);
  Powerstep01_CmdHardHiZ,              //void (*CmdHardHiZ)(uint8_t);
  Powerstep01_CmdReleaseSw,            //void (*CmdReleaseSw)(uint8_t, motorAction_t, motorDir_t);
  Powerstep01_CmdResetDevice,          //void (*CmdResetDevice)(uint8_t);
  Powerstep01_CmdResetPos,             //void (*CmdResetPos)(uint8_t);
  Powerstep01_CmdRun,                  //void (*CmdRun)(uint8_t, motorDir_t, uint32_t);
  Powerstep01_CmdSoftHiZ,              //void (*CmdSoftHiZ)(uint8_t);
  Powerstep01_CmdStepClock,            //void (*CmdStepClock)(uint8_t, motorDir_t);
  Powerstep01_FetchAndClearAllStatus,  //void (*FetchAndClearAllStatus)(void);  
  Powerstep01_GetFetchedStatus,        //uint16_t (*GetFetchedStatus)(uint8_t);
  Powerstep01_GetNbDevices,            //uint8_t (*GetNbDevices)(void);
  Powerstep01_IsDeviceBusy,            //bool (*IsDeviceBusy)(uint8_t);
  Powerstep01_SendQueuedCommands,      //void (*SendQueuedCommands)(void);
  Powerstep01_QueueCommands,           //void (*QueueCommands)(uint8_t, uint8_t, int32_t);
  Powerstep01_WaitForAllDevicesNotBusy,//void (*WaitForAllDevicesNotBusy)(void);
  Powerstep01_ErrorHandler,            //void (*ErrorHandler)(uint16_t);
  Powerstep01_BusyInterruptHandler,    //void (*BusyInterruptHandler)(void);
  Powerstep01_CmdSoftStop,             //void (*CmdSoftStop)(uint8_t);
  Powerstep01_StartStepClock,          //void (*StartStepClock)(uint16_t);
  Powerstep01_StopStepClock,           //void (*StopStepClock)(void);
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
  Powerstep01_SetNbDevices,            //bool (*SetNbDevices)(uint8_t);
  Powerstep01_SetAnalogValue,            //bool (*SetAnalogValue)(uint8_t, uint32_t, float);
  Powerstep01_GetAnalogValue             //float (*GetAnalogValue)(uint8_t, uint32_t);
};

/**
  * @}
  */ 

/** @defgroup Powerstep01_Library_Functions Library Functions
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
void Powerstep01_AttachErrorHandler(void (*callback)(uint16_t))
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
void Powerstep01_AttachFlagInterrupt(void (*callback)(void))
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
void Powerstep01_AttachBusyInterrupt(void (*callback)(void))
{
  busyInterruptCallback = (void (*)())callback;
}

/******************************************************//**
 * @brief Read id
 * @retval Id of the powerSTEP01 Driver Instance
 **********************************************************/
uint16_t Powerstep01_ReadId(void)
{
  return(powerstep01DriverInstance);
}

/******************************************************//**
 * @brief Starts the Powerstep01 library
 * @param[in] pInit pointer to the initialization data
 * @retval None
 **********************************************************/
void Powerstep01_Init(void* pInit)
{ 
  /* Initialise the GPIOs of the just added device */
  Powerstep01_Board_GpioInit(powerstep01DriverInstance);
  
  if(Powerstep01_Board_SpiInit() != 0)
  {
    /* Initialization Error */
    Powerstep01_ErrorHandler(POWERSTEP01_ERROR_0);
  } 
  
  /* configure the step clock */
  Powerstep01_Board_StepClockInit();
  
  /* Standby-reset deactivation */
  Powerstep01_Board_ReleaseReset(powerstep01DriverInstance);
  
  /* Let a delay after reset */
  Powerstep01_Board_Delay(1);

  if (pInit == 0)
  {
    // Set all registers to their predefined values from powerstep01_target_config.h 
    Powerstep01_SetRegisterToPredefinedValues(powerstep01DriverInstance);
  }
  else
  {
    Powerstep01_SetDeviceParamsToGivenValues(powerstep01DriverInstance, pInit);
  }
  
  // Put the Powerstep01 in HiZ state
  Powerstep01_CmdHardHiZ(powerstep01DriverInstance);
  
  Powerstep01_FetchAndClearAllStatus();
  
  powerstep01DriverInstance++;
}

/******************************************************//**
 * @brief Checks if at least one Powerstep01 is busy by checking 
 * busy pin position. 
 * The busy pin is shared between all devices.
 * @retval One if at least one Powerstep01 is busy, otherwise zero
 **********************************************************/
uint8_t Powerstep01_CheckBusyHw(void)
{
	if(!(Powerstep01_Board_BUSY_PIN_GetState()))
  {
    return 0x01;
  }
	else 
  {
    return 0x00;
  }
}

/******************************************************//**
 * @brief Checks if at least one Powerstep01 has an alarm flag set
 * by reading flag pin position.
 * The flag pin is shared between all devices.
 * @retval One if at least one Powerstep01 has an alarm flag set ,
 * otherwise zero
 **********************************************************/
uint8_t Powerstep01_CheckStatusHw(void)
{
	if(!(Powerstep01_Board_FLAG_PIN_GetState()))
  {
    return 0x01;
  }
	else 
  {
    return 0x00;
  }
}

/******************************************************//**
 * @brief Issues PowerStep01 Get Param command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param PowerStep01 register address
 * @retval Register value - 1 to 3 bytes (depends on register)
 *********************************************************/
uint32_t Powerstep01_CmdGetParam(uint8_t deviceId, uint32_t param)
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
        /* re-enable Powerstep01_Board_EnableIrq if disable in previous iteration */
        Powerstep01_Board_EnableIrq();
        itDisable = FALSE;
      }
      for (loop = 0; loop < numberOfDevices; loop++)
      {
        spiTxBursts[0][loop] = POWERSTEP01_NOP;
        spiTxBursts[1][loop] = POWERSTEP01_NOP;
        spiTxBursts[2][loop] = POWERSTEP01_NOP;
        spiTxBursts[3][loop] = POWERSTEP01_NOP;
        spiRxBursts[0][loop] = 0;
        spiRxBursts[1][loop] = 0;
        spiRxBursts[2][loop] = 0;
        spiRxBursts[3][loop] = 0;    
      }
      switch (param)
      {
        case POWERSTEP01_ABS_POS: 
        case POWERSTEP01_MARK:
        case POWERSTEP01_SPEED:
          spiTxBursts[0][spiIndex] = ((uint8_t)POWERSTEP01_GET_PARAM )| (param);
          maxArgumentNbBytes = 3;
          break;
        case POWERSTEP01_EL_POS:
        case POWERSTEP01_ACC:
        case POWERSTEP01_DEC:
        case POWERSTEP01_MAX_SPEED:
        case POWERSTEP01_MIN_SPEED:
        case POWERSTEP01_FS_SPD:
        case POWERSTEP01_INT_SPD:
        case POWERSTEP01_CONFIG:
        case POWERSTEP01_GATECFG1:
        case POWERSTEP01_STATUS:
          spiTxBursts[1][spiIndex] = ((uint8_t)POWERSTEP01_GET_PARAM )| (param);
          maxArgumentNbBytes = 2;
          break;
      default:
          spiTxBursts[2][spiIndex] = ((uint8_t)POWERSTEP01_GET_PARAM )| (param);
          maxArgumentNbBytes = 1;
      }
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      Powerstep01_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR
    for (loop = POWERSTEP01_CMD_ARG_MAX_NB_BYTES-1-maxArgumentNbBytes;
         loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES;
         loop++)
    {
       Powerstep01_WriteBytes(&spiTxBursts[loop][0],
                             &spiRxBursts[loop][0]);
    }
    spiRxData = ((uint32_t)spiRxBursts[1][spiIndex] << 16)|
                 (spiRxBursts[2][spiIndex] << 8) |
                 (spiRxBursts[3][spiIndex]);    
    /* re-enable Powerstep01_Board_EnableIrq after SPI transfers*/
    Powerstep01_Board_EnableIrq();
  }

  return (spiRxData);
}

/******************************************************//**
 * @brief Issues PowerStep01 Get Status command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Status Register content
 *********************************************************/
uint16_t Powerstep01_CmdGetStatus(uint8_t deviceId)
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
        /* re-enable Powerstep01_Board_EnableIrq if disable in previous iteration */
        Powerstep01_Board_EnableIrq();
        itDisable = FALSE;
      }    
      for (loop = 0; loop < numberOfDevices; loop++)
      {
         spiTxBursts[0][loop] = POWERSTEP01_NOP;
         spiTxBursts[1][loop] = POWERSTEP01_NOP;
         spiTxBursts[2][loop] = POWERSTEP01_NOP;
         spiTxBursts[3][loop] = POWERSTEP01_NOP;
         spiRxBursts[0][loop] = 0;
         spiRxBursts[1][loop] = 0;
         spiRxBursts[2][loop] = 0;
         spiRxBursts[3][loop] = 0;       
      }
      spiTxBursts[0][spiIndex] = POWERSTEP01_GET_STATUS;
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      Powerstep01_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR  
    for (loop = 0; loop < POWERSTEP01_CMD_ARG_NB_BYTES_GET_STATUS + POWERSTEP01_RSP_NB_BYTES_GET_STATUS; loop++)
    {
       Powerstep01_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
    }
    status = (spiRxBursts[1][spiIndex] << 8) | (spiRxBursts[2][spiIndex]);
    /* re-enable Powerstep01_Board_EnableIrq after SPI transfers*/
    Powerstep01_Board_EnableIrq();    
  }
  return (status);
}

/******************************************************//**
 * @brief Issues PowerStep01 Go Home command (Shortest path to zero position)
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void Powerstep01_CmdGoHome(uint8_t deviceId)
{
   Powerstep01_SendCommand(deviceId, POWERSTEP01_GO_HOME, 0);   
}

/******************************************************//**
 * @brief Issues PowerStep01 Go Mark command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void Powerstep01_CmdGoMark(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_GO_MARK, 0);     
}

/******************************************************//**
 * @brief Issues PowerStep01 Go To command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] abs_pos absolute position in steps in agreement
 * with the step mode where requested to move
 * @retval None
 *********************************************************/
void Powerstep01_CmdGoTo(uint8_t deviceId, int32_t abs_pos)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_GO_TO, abs_pos);  
}

/******************************************************//**
 * @brief Issues PowerStep01 Go To Dir command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction movement direction
 * @param[in] abs_pos absolute position in steps in agreement
 * with the step mode where requested to move
 * @retval None
 *********************************************************/
void Powerstep01_CmdGoToDir(uint8_t deviceId, 
                            motorDir_t direction,
                            int32_t abs_pos)
{
  Powerstep01_SendCommand(deviceId, 
                          (uint8_t)POWERSTEP01_GO_TO_DIR | 
                          (uint8_t)direction, abs_pos);  
}

/******************************************************//**
 * @brief Issues PowerStep01 Go Until command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] action ACTION_RESET or ACTION_COPY
 * @param[in] direction movement direction
 * @param[in] speed in 2^-28 step/tick
 * @retval None
 *********************************************************/
void Powerstep01_CmdGoUntil(uint8_t deviceId, 
                           motorAction_t action, 
                           motorDir_t direction, 
                           uint32_t speed)
{
  Powerstep01_SendCommand(deviceId,
                          (uint8_t)POWERSTEP01_GO_UNTIL | (uint8_t)action | (uint8_t)direction,
                          speed); 
}

/******************************************************//**
 * @brief Issues PowerStep01 Hard HiZ command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The HardHiZ command immediately disables the power bridges
 * (high impedance state) and raises the HiZ flag. 
 * When the motor is stopped, a HardHiZ command forces the bridges 
 * to enter high impedance state.
 * This command can be given anytime and is immediately executed.
 *********************************************************/
void Powerstep01_CmdHardHiZ(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_HARD_HIZ, 0);    
}

/******************************************************//**
 * @brief Issues PowerStep01 Hard Stop command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The HardStop command causes an immediate motor stop with
 * infinite deceleration.
 * When the motor is in high impedance state, a HardStop command
 * forces the bridges to exit high impedance state; no motion is performed.
 * This command can be given anytime and is immediately executed.
 * This command keeps the BUSY flag low until the motor is stopped.
 *********************************************************/
void Powerstep01_CmdHardStop(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_HARD_STOP, 0);         
}

/******************************************************//**
 * @brief Issues PowerStep01 Move command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction
 * @param[in] n_step number of steps
 * @retval None
 *********************************************************/
void Powerstep01_CmdMove(uint8_t deviceId, 
                         motorDir_t direction, 
                         uint32_t n_step)
{
  Powerstep01_SendCommand(deviceId, 
                          (uint8_t)POWERSTEP01_MOVE | 
                          (uint8_t)direction, n_step);  
}

/******************************************************//**
 * @brief Issues PowerStep01 NOP command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void Powerstep01_CmdNop(uint8_t deviceId)
{
  /* Send NOP operation code to PowerStep01 */
  Powerstep01_SendCommand(deviceId, POWERSTEP01_NOP, 0);
}

/******************************************************//**
 * @brief Issues PowerStep01 Release SW command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] action type of action to undertake when the SW
 * input is forced high
 * @param[in] direction movement direction
 * @retval None
 *********************************************************/
void Powerstep01_CmdReleaseSw(uint8_t deviceId, 
                             motorAction_t action, 
                             motorDir_t direction)
{
   Powerstep01_SendCommand(deviceId, 
                           (uint8_t)POWERSTEP01_RELEASE_SW | 
                           (uint8_t)action | 
                           (uint8_t)direction, 
                           0); 
}

/******************************************************//**
 * @brief Issues PowerStep01 Reset Device command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void Powerstep01_CmdResetDevice(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_RESET_DEVICE, 0);         
}

/******************************************************//**
 * @brief Issues PowerStep01 Reset Pos command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void Powerstep01_CmdResetPos(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_RESET_POS, 0);       
}

/******************************************************//**
 * @brief Issues PowerStep01 Run command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction (FORWARD, BACKWARD)
 * @param[in] speed in 2^-28 step/tick
 * @retval None
 *********************************************************/
void Powerstep01_CmdRun(uint8_t deviceId, 
                       motorDir_t direction,
                       uint32_t speed)
{
  Powerstep01_SendCommand(deviceId, 
                          (uint8_t)POWERSTEP01_RUN|(uint8_t)direction,
                          speed);
}

/******************************************************//**
 * @brief Issues the SetParam command to the PowerStep01 of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param Register adress (POWERSTEP01_ABS_POS, POWERSTEP01_MARK,...)
 * @param[in] value Value to set in the register
 * @retval None
 *********************************************************/
void Powerstep01_CmdSetParam(uint8_t deviceId, uint32_t param, uint32_t value)
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
        /* re-enable Powerstep01_Board_EnableIrq if disable in previous iteration */
        Powerstep01_Board_EnableIrq();
        itDisable = FALSE;
      }  
      for (loop = 0;loop < numberOfDevices; loop++)
      {
        spiTxBursts[0][loop] = POWERSTEP01_NOP;
        spiTxBursts[1][loop] = POWERSTEP01_NOP;
        spiTxBursts[2][loop] = POWERSTEP01_NOP;
        spiTxBursts[3][loop] = POWERSTEP01_NOP;
      }
      switch (param)
      {
        case POWERSTEP01_ABS_POS: ;
        case POWERSTEP01_MARK:
          spiTxBursts[0][spiIndex] = ((uint8_t)POWERSTEP01_SET_PARAM )| (param);
          spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
          spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
          maxArgumentNbBytes = 3;
          break;
        case POWERSTEP01_EL_POS:
        case POWERSTEP01_ACC:
        case POWERSTEP01_DEC:
        case POWERSTEP01_MAX_SPEED:
        case POWERSTEP01_MIN_SPEED:
        case POWERSTEP01_FS_SPD:
        case POWERSTEP01_INT_SPD:
        case POWERSTEP01_CONFIG:
        case POWERSTEP01_GATECFG1:
          spiTxBursts[1][spiIndex] = ((uint8_t)POWERSTEP01_SET_PARAM )| (param);
          spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
          maxArgumentNbBytes = 2;
          break;
        default:
          spiTxBursts[2][spiIndex] = ((uint8_t)POWERSTEP01_SET_PARAM )| (param);
          maxArgumentNbBytes = 1;
      }
      spiTxBursts[3][spiIndex] = (uint8_t)(value);
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      Powerstep01_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR  
    /* SPI transfer */
    for (loop = POWERSTEP01_CMD_ARG_MAX_NB_BYTES - 1 - maxArgumentNbBytes;
         loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES;
         loop++)
    {
      Powerstep01_WriteBytes(&spiTxBursts[loop][0],&spiRxBursts[loop][0]);
    }
    /* re-enable Powerstep01_Board_EnableIrq after SPI transfers*/
    Powerstep01_Board_EnableIrq();
  }
}

/******************************************************//**
 * @brief Issues PowerStep01 Soft HiZ command
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
void Powerstep01_CmdSoftHiZ(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_SOFT_HIZ, 0);           
}

/******************************************************//**
 * @brief Issues PowerStep01 Soft Stop command
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
void Powerstep01_CmdSoftStop(uint8_t deviceId)
{
  Powerstep01_SendCommand(deviceId, POWERSTEP01_SOFT_STOP, 0);         
}

/******************************************************//**
 * @brief Issues PowerStep01 Step Clock command
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction (FORWARD, BACKWARD)
 * @retval None
 *********************************************************/
void Powerstep01_CmdStepClock(uint8_t deviceId,
                             motorDir_t direction)
{
  Powerstep01_SendCommand(deviceId, 
                          (uint8_t)POWERSTEP01_STEP_CLOCK | (uint8_t)direction,
                          0);  
}

/******************************************************//**
 * @brief Fetch and clear status flags of all devices 
 * by issuing a GET_STATUS command simultaneously  
 * to all devices.
 * Then, the fetched status of each device can be retrieved
 * by using the Powerstep01_GetFetchedStatus function
 * provided there is no other calls to functions which 
 * use the SPI in between.
 * @retval None
 *********************************************************/
void Powerstep01_FetchAndClearAllStatus(void)
{
  uint8_t loop;

  for (loop = 0; loop < numberOfDevices; loop++)
  {
     spiTxBursts[0][loop] = POWERSTEP01_GET_STATUS;
     spiTxBursts[1][loop] = POWERSTEP01_NOP;
     spiTxBursts[2][loop] = POWERSTEP01_NOP;
     spiTxBursts[3][loop] = POWERSTEP01_NOP;
     spiRxBursts[0][loop] = 0;
     spiRxBursts[1][loop] = 0;
     spiRxBursts[2][loop] = 0;
     spiRxBursts[3][loop] = 0;
  }
  for (loop = 0; 
       loop < POWERSTEP01_CMD_ARG_NB_BYTES_GET_STATUS + 
              POWERSTEP01_RSP_NB_BYTES_GET_STATUS; 
       loop++)
  {
     Powerstep01_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
  }
}

/******************************************************//**
 * @brief Get the value of the STATUS register which was 
 * fetched by using Powerstep01_FetchAndClearAllStatus.
 * The fetched values are available  as long as there
 * no other calls to functions which use the SPI.
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Last fetched value of the STATUS register
 *********************************************************/
uint16_t Powerstep01_GetFetchedStatus(uint8_t deviceId)
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
 * @brief Return motor handle (pointer to the powerSTEP01 motor driver structure)
 * @retval Pointer to the motorDrv_t structure
 **********************************************************/
motorDrv_t* Powerstep01_GetMotorHandle(void)
{
  return (&powerstep01Drv);
}
       
/******************************************************//**
 * @brief Returns the FW version of the library
 * @retval POWERSTEP01_FW_VERSION
 **********************************************************/
uint32_t Powerstep01_GetFwVersion(void)
{
  return (POWERSTEP01_FW_VERSION);
}

/******************************************************//**
 * @brief  Returns the mark position  of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Mark register value converted in a 32b signed integer 
 **********************************************************/
int32_t Powerstep01_GetMark(uint8_t deviceId)
{
  return Powerstep01_ConvertPosition(Powerstep01_CmdGetParam(deviceId, POWERSTEP01_MARK));
}

/******************************************************//**
 * @brief Return the number of devices in the daisy chain 
 * @retval number of devices from 1 to MAX_NUMBER_OF_DEVICES
 *********************************************************/
uint8_t Powerstep01_GetNbDevices(void)
{
    return (numberOfDevices);
}

/******************************************************//**
 * @brief Issues PowerStep01 Get Parameter command and convert the result to
 * floating point if meaningfull
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param PowerStep01 register address
 * @retval Register value - 1 to 3 bytes (depends on register)
 *********************************************************/
float Powerstep01_GetAnalogValue(uint8_t deviceId, uint32_t param)
{
  bool voltageMode = ((POWERSTEP01_CM_VM_CURRENT&Powerstep01_CmdGetParam(deviceId,POWERSTEP01_STEP_MODE))==0);
  uint32_t registerValue = Powerstep01_CmdGetParam(deviceId, param);
  float value;
  switch (param)
  {
    case POWERSTEP01_ABS_POS:
    case POWERSTEP01_MARK:
      value = (float) Powerstep01_ConvertPosition(registerValue);
      break;
    case POWERSTEP01_ACC:
    case POWERSTEP01_DEC:
      value = Powerstep01_AccDec_RegVal_to_Steps_s2(registerValue);
      break;
    case POWERSTEP01_SPEED:
      value = Powerstep01_Speed_RegVal_to_Steps_s(registerValue);
      break;
    case POWERSTEP01_MAX_SPEED:
      value = Powerstep01_MaxSpd_RegVal_to_Steps_s(registerValue);
      break;
    case POWERSTEP01_MIN_SPEED:
      registerValue &= POWERSTEP01_MIN_SPEED_MASK;
      value = Powerstep01_MinSpd_RegVal_to_Steps_s(registerValue);
      break;      
    case POWERSTEP01_FS_SPD:
      registerValue &= POWERSTEP01_FS_SPD_MASK;
      value = Powerstep01_FSSpd_RegVal_to_Steps_s(registerValue);
      break;
    case POWERSTEP01_INT_SPD:
      value = Powerstep01_IntSpd_RegVal_to_Steps_s(registerValue);
      break;
    case POWERSTEP01_K_THERM:
      value = Powerstep01_KTherm_RegVal_to_Comp(registerValue);
      break;
    case POWERSTEP01_OCD_TH:
    case POWERSTEP01_STALL_TH:
      value = Powerstep01_StallOcd_RegVal_to_Th(registerValue);
      break;
    case POWERSTEP01_KVAL_HOLD:  //POWERSTEP01_TVAL_HOLD
    case POWERSTEP01_KVAL_RUN:   //POWERSTEP01_TVAL_RUN
    case POWERSTEP01_KVAL_ACC:   //POWERSTEP01_TVAL_ACC
    case POWERSTEP01_KVAL_DEC:   //POWERSTEP01_TVAL_DEC
      if (voltageMode!=FALSE)  value = Powerstep01_Kval_RegVal_to_Perc(registerValue);
      else value = Powerstep01_Tval_RegVal_to_RefVoltage(registerValue);     
      break;
    case POWERSTEP01_ST_SLP:
      if (voltageMode==FALSE) 
      {
        break;
      }      
    case POWERSTEP01_FN_SLP_ACC: //POWERSTEP01_TON_MIN
    case POWERSTEP01_FN_SLP_DEC: //POWERSTEP01_TOFF_MIN
      if (voltageMode!=FALSE) value = Powerstep01_BEMFslope_RegVal_to_Perc(registerValue);
      else value = Powerstep01_Tmin_RegVal_to_Time(registerValue);
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
int32_t Powerstep01_GetPosition(uint8_t deviceId)
{
  return Powerstep01_ConvertPosition(Powerstep01_CmdGetParam(deviceId, POWERSTEP01_ABS_POS));
}

/******************************************************//**
 * @brief Checks if the specified device is busy
 * by reading the Busy flag bit ot its status Register
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval true if device is busy, false zero
 *********************************************************/
bool Powerstep01_IsDeviceBusy(uint8_t deviceId)
{
  if(!(Powerstep01_ReadStatusRegister(deviceId) & POWERSTEP01_STATUS_BUSY)) 
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
 * done by calling Powerstep01_SendQueuedCommands.
 * Any call to functions that use the SPI between the calls of 
 * Powerstep01_QueueCommands and Powerstep01_SendQueuedCommands 
 * will corrupt the queue.
 * A command for each device of the daisy chain must be 
 * specified before calling Powerstep01_SendQueuedCommands.
 * @param[in] deviceId deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] command Command to queue (all Powerstep01 commmands 
 * except POWERSTEP01_SET_PARAM, POWERSTEP01_GET_PARAM, 
 * POWERSTEP01_GET_STATUS)
 * @param[in] value argument of the command to queue
 * @retval None
 *********************************************************/
void Powerstep01_QueueCommands(uint8_t deviceId, uint8_t command, int32_t value)
{
  if (numberOfDevices > deviceId)
  {
    uint8_t spiIndex = numberOfDevices - deviceId - 1;
    
    switch (command & DAISY_CHAIN_COMMAND_MASK)
    {
      case POWERSTEP01_RUN: ;
      case POWERSTEP01_MOVE: ;
      case POWERSTEP01_GO_TO: ;
      case POWERSTEP01_GO_TO_DIR: ;
      case POWERSTEP01_GO_UNTIL: ;
      case POWERSTEP01_GO_UNTIL_ACT_CPY:
       spiTxBursts[0][spiIndex] = command;
       spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
       spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
       spiTxBursts[3][spiIndex] = (uint8_t)(value);
       break;
      default:
       spiTxBursts[0][spiIndex] = POWERSTEP01_NOP;
       spiTxBursts[1][spiIndex] = POWERSTEP01_NOP;
       spiTxBursts[2][spiIndex] = POWERSTEP01_NOP;
       spiTxBursts[3][spiIndex] = command;
    }
  }
}

/******************************************************//**
 * @brief  Reads the Status Register value
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Status register value
 * @note The status register flags are not cleared 
 * at the difference with Powerstep01_CmdGetStatus()
 **********************************************************/
uint16_t Powerstep01_ReadStatusRegister(uint8_t deviceId)
{
  return (Powerstep01_CmdGetParam(deviceId, POWERSTEP01_STATUS));
}

/******************************************************//**
 * @brief  Releases the Powerstep01 reset (pin set to High) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 **********************************************************/
void Powerstep01_ReleaseReset(uint8_t deviceId)
{ 
  Powerstep01_Board_ReleaseReset(deviceId); 
}

/******************************************************//**
 * @brief  Resets the Powerstep01 (reset pin set to low) of all devices
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 **********************************************************/
void Powerstep01_Reset(uint8_t deviceId)
{
  Powerstep01_Board_Reset(deviceId); 
}

/******************************************************//**
 * @brief  Set the stepping mode 
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] stepMode from full step to 1/128 microstep as specified in enum motorStepMode_t
 * @retval TRUE if successfull, FALSE if failure
 **********************************************************/
bool Powerstep01_SelectStepMode(uint8_t deviceId, motorStepMode_t stepMode)
{
  uint8_t stepModeRegister;
  powerstep01_StepSel_t powerstep01StepMode;

  switch (stepMode)
  {
    case STEP_MODE_FULL:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1;
      break;
    case STEP_MODE_HALF:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_2;
      break;    
    case STEP_MODE_1_4:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_4;
      break;        
    case STEP_MODE_1_8:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_8;
      break;
    case STEP_MODE_1_16:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_16;
      break;        
    case STEP_MODE_1_32:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_32;
      break;
    case STEP_MODE_1_64:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_64;
      break;
    case STEP_MODE_1_128:
      powerstep01StepMode = POWERSTEP01_STEP_SEL_1_128;
      break;
    default:
      return FALSE;     
  }
  
  /* Set the powerstep01 in HiZ state */
  Powerstep01_CmdHardHiZ(deviceId);  
  
  /* Read Step mode register and clear STEP_SEL field */
  stepModeRegister = (uint8_t)(0xF8 & Powerstep01_CmdGetParam(deviceId,POWERSTEP01_STEP_MODE)) ;
  
  /* Apply new step mode */
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STEP_MODE, stepModeRegister | (uint8_t)powerstep01StepMode);

  /* Reset abs pos register */
  Powerstep01_CmdResetPos(deviceId);
  
  return TRUE;
}

/******************************************************//**
 * @brief Sends commands stored previously in the queue by 
 * Powerstep01_QueueCommands
 * @retval None
 *********************************************************/
void Powerstep01_SendQueuedCommands(void)
{
  uint8_t loop;
  
  for (loop = 0; 
       loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES; 
       loop++)
  {
     Powerstep01_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
  }
}

/******************************************************//**
 * @brief  Sets Home Position 
 * (ABS pos set to current pos minus new home position)
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] homePos new home position
 * @retval None
 **********************************************************/
void Powerstep01_SetHome(uint8_t deviceId, int32_t homePos)
{
  uint32_t currentPos = Powerstep01_CmdGetParam(deviceId, POWERSTEP01_ABS_POS);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ABS_POS, currentPos- homePos);
}

/******************************************************//**
 * @brief  Sets Mark position 
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] markPos new mark position
 * @retval None
 **********************************************************/
void Powerstep01_SetMark(uint8_t deviceId, int32_t markPos)
{
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MARK, markPos);
}

/******************************************************//**
 * @brief  Sets the number of devices to be used 
 * @param[in] nbDevices (from 1 to MAX_NUMBER_OF_DEVICES)
 * @retval TRUE if successfull, FALSE if failure, attempt to set a number of 
 * devices greater than MAX_NUMBER_OF_DEVICES
 **********************************************************/
bool Powerstep01_SetNbDevices(uint8_t nbDevices)
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
 * @brief Issues the SetParam command to the PowerStep01 of the specified device
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param Register adress (POWERSTEP01_ABS_POS, POWERSTEP01_MARK,...)
 * @param[in] value Floating point value to convert and set into the register
 * @retval TRUE if param and value are valid, FALSE otherwise
 *********************************************************/
bool Powerstep01_SetAnalogValue(uint8_t deviceId, uint32_t param, float value)
{
  uint32_t registerValue;
  bool result = TRUE;
  bool voltageMode = ((POWERSTEP01_CM_VM_CURRENT&Powerstep01_CmdGetParam(deviceId,POWERSTEP01_STEP_MODE))==0);
  if ((value < 0)&&((param != POWERSTEP01_ABS_POS)&&(param != POWERSTEP01_MARK)))
  {
    result = FALSE;
  }
  switch (param)
  {
    case POWERSTEP01_EL_POS:
      if ((value > (POWERSTEP01_ELPOS_STEP_MASK|POWERSTEP01_ELPOS_MICROSTEP_MASK))||
          ((value!=0)&&(value < (1<<(7-(POWERSTEP01_STEP_MODE_STEP_SEL&Powerstep01_CmdGetParam(0,POWERSTEP01_STEP_MODE))))))) result = FALSE;
      else registerValue = ((uint32_t) value)&(POWERSTEP01_ELPOS_STEP_MASK|POWERSTEP01_ELPOS_MICROSTEP_MASK);
      break;        
    case POWERSTEP01_ABS_POS:
    case POWERSTEP01_MARK:
      if (value < 0)
      {
        value=-value;
        if (((uint32_t)value)<=(POWERSTEP01_MAX_POSITION+1))
          registerValue = (POWERSTEP01_ABS_POS_VALUE_MASK+1-(uint32_t)value)&POWERSTEP01_ABS_POS_VALUE_MASK;
        else result = FALSE;  
      }
      else
      {
        if (((uint32_t)value)<=POWERSTEP01_MAX_POSITION)
          registerValue = ((uint32_t) value)&POWERSTEP01_ABS_POS_VALUE_MASK;
        else result = FALSE; 
      }
      break;
    case POWERSTEP01_ACC:
    case POWERSTEP01_DEC:
      if (value > POWERSTEP01_ACC_DEC_MAX_VALUE) result = FALSE;
      else registerValue = Powerstep01_AccDec_Steps_s2_to_RegVal(value);
      break;
    case POWERSTEP01_MAX_SPEED:
      if (value > POWERSTEP01_MAX_SPEED_MAX_VALUE) result = FALSE;
      else registerValue = Powerstep01_MaxSpd_Steps_s_to_RegVal(value);
      break;
    case POWERSTEP01_MIN_SPEED:
      if (value > POWERSTEP01_MIN_SPEED_MAX_VALUE) result = FALSE;
      else registerValue = (POWERSTEP01_LSPD_OPT&Powerstep01_CmdGetParam(deviceId, param))|Powerstep01_MinSpd_Steps_s_to_RegVal(value);
      break;      
    case POWERSTEP01_FS_SPD:
      if (value > POWERSTEP01_FS_SPD_MAX_VALUE) result = FALSE;
      else registerValue = (POWERSTEP01_BOOST_MODE&Powerstep01_CmdGetParam(deviceId, param))|Powerstep01_FSSpd_Steps_s_to_RegVal(value);
      break;
    case POWERSTEP01_INT_SPD:
      if (value > POWERSTEP01_INT_SPD_MAX_VALUE) result = FALSE;
      else registerValue = Powerstep01_IntSpd_Steps_s_to_RegVal(value);
      break;
    case POWERSTEP01_K_THERM:
      if ((value < POWERSTEP01_K_THERM_MIN_VALUE)||
          (value > POWERSTEP01_K_THERM_MAX_VALUE)) result = FALSE;
      else registerValue = Powerstep01_KTherm_Comp_to_RegVal(value);
      break;
    case POWERSTEP01_OCD_TH:
    case POWERSTEP01_STALL_TH:
      if (value > POWERSTEP01_STALL_OCD_TH_MAX_VALUE) result = FALSE;
      else registerValue = Powerstep01_StallOcd_Th_to_RegVal(value);
      break;
    case POWERSTEP01_KVAL_HOLD:  //POWERSTEP01_TVAL_HOLD
    case POWERSTEP01_KVAL_RUN:   //POWERSTEP01_TVAL_RUN
    case POWERSTEP01_KVAL_ACC:   //POWERSTEP01_TVAL_ACC
    case POWERSTEP01_KVAL_DEC:   //POWERSTEP01_TVAL_DEC
      if (voltageMode==FALSE)
      {
        if (value > POWERSTEP01_TVAL_MAX_VALUE) result = FALSE;
        else registerValue = Powerstep01_Tval_RefVoltage_to_RegVal(value);
      }
      else
      {
        if (value > POWERSTEP01_KVAL_MAX_VALUE) result = FALSE;
        else registerValue = Powerstep01_Kval_Perc_to_RegVal(value);
      }
      break;
    case POWERSTEP01_ST_SLP:
      if (voltageMode==FALSE) 
      {
        result = FALSE;
        break;
      }
    case POWERSTEP01_FN_SLP_ACC: //POWERSTEP01_TON_MIN
    case POWERSTEP01_FN_SLP_DEC: //POWERSTEP01_TOFF_MIN
      if (voltageMode==FALSE)
      {
        if (value>POWERSTEP01_TOFF_TON_MIN_MAX_VALUE) result = FALSE;
        else registerValue = Powerstep01_Tmin_Time_to_RegVal(value);
      }
      else
      {
        if (value > POWERSTEP01_SLP_MAX_VALUE) result = FALSE;
        else registerValue = Powerstep01_BEMFslope_Perc_to_RegVal(value);
      }
      break;
    default:
      result = FALSE;
  }
  if (result!=FALSE)
  {
    Powerstep01_CmdSetParam(deviceId, param, registerValue);
  }
  return result;
}

/******************************************************//**
 * @brief  Start the step clock by using the given frequency
 * @param[in] newFreq in Hz of the step clock
 * @retval None
 * @note The frequency is directly the current speed of the device
 **********************************************************/
void Powerstep01_StartStepClock(uint16_t newFreq)
{
  Powerstep01_Board_StartStepClock(newFreq);
}

/******************************************************//**
 * @brief  Stops the PWM uses for the step clock
 * @retval None
 **********************************************************/
void Powerstep01_StopStepClock(void)
{
  Powerstep01_Board_StopStepClock();
}

/******************************************************//**
 * @brief  Locks until all devices become not busy
 * @retval None
 **********************************************************/
void Powerstep01_WaitForAllDevicesNotBusy(void)
{
  bool busy = TRUE;
  uint8_t loop;
	
  /* Wait while at least one is active */
  while (busy)
  {
    busy = FALSE;
    for (loop = 0; loop < numberOfDevices; loop++)
    {
      busy  |= Powerstep01_IsDeviceBusy(loop);
    }   
  }
}

/******************************************************//**
 * @brief  Locks until the device becomes not busy
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval None
 **********************************************************/
void Powerstep01_WaitWhileActive(uint8_t deviceId)
 {
	/* Wait while motor is running */
	while (Powerstep01_IsDeviceBusy(deviceId) != 0);
}

/**
  * @}
  */

/** @addtogroup Powerstep01_Private_Functions
  * @{
  */
/******************************************************//**
 * @brief  Converts the ABS_POSITION register value to a 32b signed integer
 * @param[in] abs_position_reg value of the ABS_POSITION register
 * @retval operation_result 32b signed integer corresponding to the absolute position 
 **********************************************************/
int32_t Powerstep01_ConvertPosition(uint32_t abs_position_reg)
{
	int32_t operation_result;

  if (abs_position_reg & POWERSTEP01_ABS_POS_SIGN_BIT_MASK) 
  {
		/* Negative register value */
		abs_position_reg = ~abs_position_reg;
		abs_position_reg += 1;

		operation_result = (int32_t) (abs_position_reg & POWERSTEP01_ABS_POS_VALUE_MASK);
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
void Powerstep01_BusyInterruptHandler(void)
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
void Powerstep01_ErrorHandler(uint16_t error)
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
void Powerstep01_FlagInterruptHandler(void)
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
 * @param[in] param Command to send (all Powerstep01 commmands 
 * except POWERSTEP01_SET_PARAM, POWERSTEP01_GET_PARAM, 
 * POWERSTEP01_GET_STATUS)
 * @param[in] value arguments to send on 32 bits
 * @retval None
 **********************************************************/
void Powerstep01_SendCommand(uint8_t deviceId, uint8_t param, uint32_t value)
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
        /* re-enable Powerstep01_Board_EnableIrq if disable in previous iteration */
        Powerstep01_Board_EnableIrq();
        itDisable = FALSE;
      }    
      for (loop = 0; loop < numberOfDevices; loop++)
      {
          spiTxBursts[0][loop] = POWERSTEP01_NOP;
          spiTxBursts[1][loop] = POWERSTEP01_NOP;
          spiTxBursts[2][loop] = POWERSTEP01_NOP;
          spiTxBursts[3][loop] = POWERSTEP01_NOP;   
      }
      switch (param & DAISY_CHAIN_COMMAND_MASK)
      {
        case POWERSTEP01_GO_TO:
        case POWERSTEP01_GO_TO_DIR:
          value = value & POWERSTEP01_ABS_POS_VALUE_MASK;
        case POWERSTEP01_RUN:
        case POWERSTEP01_MOVE:
        case POWERSTEP01_GO_UNTIL:
        case POWERSTEP01_GO_UNTIL_ACT_CPY:
          spiTxBursts[0][spiIndex] = param;
          spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
          spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
          spiTxBursts[3][spiIndex] = (uint8_t)(value);
          maxArgumentNbBytes = 3;
          break;
      default:
          spiTxBursts[0][spiIndex] = POWERSTEP01_NOP;
          spiTxBursts[1][spiIndex] = POWERSTEP01_NOP;
          spiTxBursts[2][spiIndex] = POWERSTEP01_NOP;
          spiTxBursts[3][spiIndex] = param;
      }
      /* Disable interruption before checking */
      /* pre-emption by ISR and SPI transfers*/
      Powerstep01_Board_DisableIrq();
      itDisable = TRUE;
    } while (spiPreemtionByIsr); // check pre-emption by ISR
    for (loop = POWERSTEP01_CMD_ARG_MAX_NB_BYTES - 1 - maxArgumentNbBytes; 
         loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES; 
         loop++)
    {
       Powerstep01_WriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
    }
    /* re-enable Powerstep01_Board_EnableIrq after SPI transfers*/
    Powerstep01_Board_EnableIrq();
  }
}

/******************************************************//**
 * @brief  Sets the registers of the Powerstep01 to their predefined values 
 * from powerstep01_target_config.h
 * @param[in] deviceId (from 0 to 2)
 * @retval None
 **********************************************************/
void Powerstep01_SetRegisterToPredefinedValues(uint8_t deviceId)
{  
  powerstep01_CmVm_t cmVm;
  
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ABS_POS, 0);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_EL_POS, 0);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MARK, 0);
  
  switch (deviceId)
  {
    case 0:
      cmVm = POWERSTEP01_CONF_PARAM_CM_VM_DEVICE_0;
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ACC,
        Powerstep01_AccDec_Steps_s2_to_RegVal(
          POWERSTEP01_CONF_PARAM_ACC_DEVICE_0));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_DEC,
        Powerstep01_AccDec_Steps_s2_to_RegVal(
          POWERSTEP01_CONF_PARAM_DEC_DEVICE_0));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MAX_SPEED,
        Powerstep01_MaxSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_MAX_SPEED_DEVICE_0));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MIN_SPEED,
        POWERSTEP01_CONF_PARAM_LSPD_BIT_DEVICE_0|
        Powerstep01_MinSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_MIN_SPEED_DEVICE_0));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FS_SPD,
        POWERSTEP01_CONF_PARAM_BOOST_MODE_DEVICE_0|
        Powerstep01_FSSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_FS_SPD_DEVICE_0)); 
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_OCD_TH,
        (uint8_t)POWERSTEP01_CONF_PARAM_OCD_TH_DEVICE_0);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STEP_MODE,
        (uint8_t)POWERSTEP01_CONF_PARAM_SYNC_MODE_DEVICE_0 |
        (uint8_t)POWERSTEP01_CONF_PARAM_CM_VM_DEVICE_0|
        (uint8_t)POWERSTEP01_CONF_PARAM_STEP_MODE_DEVICE_0);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ALARM_EN,
        POWERSTEP01_CONF_PARAM_ALARM_EN_DEVICE_0);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG1,
        (uint16_t)POWERSTEP01_CONF_PARAM_IGATE_DEVICE_0 | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TCC_DEVICE_0   | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TBOOST_DEVICE_0|
        (uint16_t)POWERSTEP01_CONF_PARAM_WD_EN_DEVICE_0);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG2,
        (uint16_t)POWERSTEP01_CONF_PARAM_TBLANK_DEVICE_0 | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TDT_DEVICE_0);
      // Voltage mode
      if (cmVm == POWERSTEP01_CM_VM_VOLTAGE)
      {
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_INT_SPD,
          Powerstep01_IntSpd_Steps_s_to_RegVal(
            POWERSTEP01_CONF_PARAM_INT_SPD_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_K_THERM,
          Powerstep01_KTherm_Comp_to_RegVal(
            POWERSTEP01_CONF_PARAM_K_THERM_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STALL_TH,
          Powerstep01_StallOcd_Th_to_RegVal(
            POWERSTEP01_CONF_PARAM_STALL_TH_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_HOLD,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_HOLD_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_RUN,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_RUN_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_ACC,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_ACC_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_DEC,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_DEC_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ST_SLP,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_ST_SLP_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_ACC,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_FN_SLP_ACC_DEVICE_0));  
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_DEC,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_FN_SLP_DEC_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)POWERSTEP01_CONF_PARAM_CLOCK_SETTING_DEVICE_0 | 
          (uint16_t)POWERSTEP01_CONF_PARAM_SW_MODE_DEVICE_0       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VS_COMP_DEVICE_0       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_OC_SD_DEVICE_0         | 
          (uint16_t)POWERSTEP01_CONF_PARAM_UVLOVAL_DEVICE_0       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VCCVAL_DEVICE_0        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_PWM_DIV_DEVICE_0       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_PWM_MUL_DEVICE_0);
      }
      else
      {
        // Current mode
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_HOLD,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_HOLD_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_RUN,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_RUN_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_ACC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_ACC_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_DEC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_DEC_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_T_FAST,
          (uint8_t)POWERSTEP01_CONF_PARAM_TOFF_FAST_DEVICE_0 |
          (uint8_t)POWERSTEP01_CONF_PARAM_FAST_STEP_DEVICE_0);
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TON_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            POWERSTEP01_CONF_PARAM_TON_MIN_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TOFF_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            POWERSTEP01_CONF_PARAM_TOFF_MIN_DEVICE_0));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)POWERSTEP01_CONF_PARAM_CLOCK_SETTING_DEVICE_0 | 
          (uint16_t)POWERSTEP01_CONF_PARAM_SW_MODE_DEVICE_0       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_TQ_REG_DEVICE_0        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_OC_SD_DEVICE_0         | 
          (uint16_t)POWERSTEP01_CONF_PARAM_UVLOVAL_DEVICE_0       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VCCVAL_DEVICE_0        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_TSW_DEVICE_0           |
          (uint16_t)POWERSTEP01_CONF_PARAM_PRED_DEVICE_0);          
      }
      break;
#if (MAX_NUMBER_OF_DEVICES > 1)
   case 1:
      cmVm = POWERSTEP01_CONF_PARAM_CM_VM_DEVICE_1;
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ACC,
        Powerstep01_AccDec_Steps_s2_to_RegVal(
          POWERSTEP01_CONF_PARAM_ACC_DEVICE_1));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_DEC,
        Powerstep01_AccDec_Steps_s2_to_RegVal(
          POWERSTEP01_CONF_PARAM_DEC_DEVICE_1));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MAX_SPEED,
        Powerstep01_MaxSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_MAX_SPEED_DEVICE_1));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MIN_SPEED,
        POWERSTEP01_CONF_PARAM_LSPD_BIT_DEVICE_1|
        Powerstep01_MinSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_MIN_SPEED_DEVICE_1));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FS_SPD,
        POWERSTEP01_CONF_PARAM_BOOST_MODE_DEVICE_1|
        Powerstep01_FSSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_FS_SPD_DEVICE_1)); 
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_OCD_TH,
        (uint8_t)POWERSTEP01_CONF_PARAM_OCD_TH_DEVICE_1);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STEP_MODE,
        (uint8_t)POWERSTEP01_CONF_PARAM_SYNC_MODE_DEVICE_1 |
        (uint8_t)POWERSTEP01_CONF_PARAM_CM_VM_DEVICE_1|
        (uint8_t)POWERSTEP01_CONF_PARAM_STEP_MODE_DEVICE_1);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ALARM_EN,
        POWERSTEP01_CONF_PARAM_ALARM_EN_DEVICE_1);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG1,
        (uint16_t)POWERSTEP01_CONF_PARAM_IGATE_DEVICE_1 | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TCC_DEVICE_1   | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TBOOST_DEVICE_1|
        (uint16_t)POWERSTEP01_CONF_PARAM_WD_EN_DEVICE_1);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG2,
        (uint16_t)POWERSTEP01_CONF_PARAM_TBLANK_DEVICE_1 | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TDT_DEVICE_1);
      // Voltage mode
      if (cmVm == POWERSTEP01_CM_VM_VOLTAGE)
      {
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_INT_SPD,
          Powerstep01_IntSpd_Steps_s_to_RegVal(
            POWERSTEP01_CONF_PARAM_INT_SPD_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_K_THERM,
          Powerstep01_KTherm_Comp_to_RegVal(
            POWERSTEP01_CONF_PARAM_K_THERM_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STALL_TH,
          Powerstep01_StallOcd_Th_to_RegVal(
            POWERSTEP01_CONF_PARAM_STALL_TH_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_HOLD,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_HOLD_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_RUN,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_RUN_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_ACC,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_ACC_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_DEC,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_DEC_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ST_SLP,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_ST_SLP_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_ACC,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_FN_SLP_ACC_DEVICE_1));  
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_DEC,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_FN_SLP_DEC_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)POWERSTEP01_CONF_PARAM_CLOCK_SETTING_DEVICE_1 | 
          (uint16_t)POWERSTEP01_CONF_PARAM_SW_MODE_DEVICE_1       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VS_COMP_DEVICE_1       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_OC_SD_DEVICE_1         | 
          (uint16_t)POWERSTEP01_CONF_PARAM_UVLOVAL_DEVICE_1       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VCCVAL_DEVICE_1        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_PWM_DIV_DEVICE_1       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_PWM_MUL_DEVICE_1);
      }
      else
      {
        // Current mode
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_HOLD,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_HOLD_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_RUN,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_RUN_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_ACC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_ACC_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_DEC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_DEC_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_T_FAST,
          (uint8_t)POWERSTEP01_CONF_PARAM_TOFF_FAST_DEVICE_1 |
          (uint8_t)POWERSTEP01_CONF_PARAM_FAST_STEP_DEVICE_1);
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TON_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            POWERSTEP01_CONF_PARAM_TON_MIN_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TOFF_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            POWERSTEP01_CONF_PARAM_TOFF_MIN_DEVICE_1));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)POWERSTEP01_CONF_PARAM_CLOCK_SETTING_DEVICE_1 | 
          (uint16_t)POWERSTEP01_CONF_PARAM_SW_MODE_DEVICE_1       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_TQ_REG_DEVICE_1        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_OC_SD_DEVICE_1         | 
          (uint16_t)POWERSTEP01_CONF_PARAM_UVLOVAL_DEVICE_1       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VCCVAL_DEVICE_1        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_TSW_DEVICE_1           |
          (uint16_t)POWERSTEP01_CONF_PARAM_PRED_DEVICE_1);          
      }
      break;     
#endif
#if (MAX_NUMBER_OF_DEVICES > 2)
   case 2:
      cmVm = POWERSTEP01_CONF_PARAM_CM_VM_DEVICE_2;
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ACC,
        Powerstep01_AccDec_Steps_s2_to_RegVal(
          POWERSTEP01_CONF_PARAM_ACC_DEVICE_2));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_DEC,
        Powerstep01_AccDec_Steps_s2_to_RegVal(
          POWERSTEP01_CONF_PARAM_DEC_DEVICE_2));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MAX_SPEED,
        Powerstep01_MaxSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_MAX_SPEED_DEVICE_2));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MIN_SPEED,
        POWERSTEP01_CONF_PARAM_LSPD_BIT_DEVICE_2|
        Powerstep01_MinSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_MIN_SPEED_DEVICE_2));
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FS_SPD,
        POWERSTEP01_CONF_PARAM_BOOST_MODE_DEVICE_2|
        Powerstep01_FSSpd_Steps_s_to_RegVal(
          POWERSTEP01_CONF_PARAM_FS_SPD_DEVICE_2)); 
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_OCD_TH,
        (uint8_t)POWERSTEP01_CONF_PARAM_OCD_TH_DEVICE_2);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STEP_MODE,
        (uint8_t)POWERSTEP01_CONF_PARAM_SYNC_MODE_DEVICE_2 |
        (uint8_t)POWERSTEP01_CONF_PARAM_CM_VM_DEVICE_2|
        (uint8_t)POWERSTEP01_CONF_PARAM_STEP_MODE_DEVICE_2);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ALARM_EN,
        POWERSTEP01_CONF_PARAM_ALARM_EN_DEVICE_2);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG1,
        (uint16_t)POWERSTEP01_CONF_PARAM_IGATE_DEVICE_2 | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TCC_DEVICE_2   | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TBOOST_DEVICE_2|
        (uint16_t)POWERSTEP01_CONF_PARAM_WD_EN_DEVICE_2);
      Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG2,
        (uint16_t)POWERSTEP01_CONF_PARAM_TBLANK_DEVICE_2 | 
        (uint16_t)POWERSTEP01_CONF_PARAM_TDT_DEVICE_2);
      // Voltage mode
      if (cmVm == POWERSTEP01_CM_VM_VOLTAGE)
      {
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_INT_SPD,
          Powerstep01_IntSpd_Steps_s_to_RegVal(
            POWERSTEP01_CONF_PARAM_INT_SPD_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_K_THERM,
          Powerstep01_KTherm_Comp_to_RegVal(
            POWERSTEP01_CONF_PARAM_K_THERM_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STALL_TH,
          Powerstep01_StallOcd_Th_to_RegVal(
            POWERSTEP01_CONF_PARAM_STALL_TH_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_HOLD,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_HOLD_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_RUN,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_RUN_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_ACC,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_ACC_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_DEC,
          Powerstep01_Kval_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_KVAL_DEC_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ST_SLP,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_ST_SLP_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_ACC,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_FN_SLP_ACC_DEVICE_2));  
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_DEC,
          Powerstep01_BEMFslope_Perc_to_RegVal(
            POWERSTEP01_CONF_PARAM_FN_SLP_DEC_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)POWERSTEP01_CONF_PARAM_CLOCK_SETTING_DEVICE_2 | 
          (uint16_t)POWERSTEP01_CONF_PARAM_SW_MODE_DEVICE_2       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VS_COMP_DEVICE_2       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_OC_SD_DEVICE_2         | 
          (uint16_t)POWERSTEP01_CONF_PARAM_UVLOVAL_DEVICE_2       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VCCVAL_DEVICE_2        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_PWM_DIV_DEVICE_2       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_PWM_MUL_DEVICE_2);
      }
      else
      {
        // Current mode
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_HOLD,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_HOLD_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_RUN,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_RUN_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_ACC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_ACC_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_DEC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            POWERSTEP01_CONF_PARAM_TVAL_DEC_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_T_FAST,
          (uint8_t)POWERSTEP01_CONF_PARAM_TOFF_FAST_DEVICE_2 |
          (uint8_t)POWERSTEP01_CONF_PARAM_FAST_STEP_DEVICE_2);
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TON_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            POWERSTEP01_CONF_PARAM_TON_MIN_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TOFF_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            POWERSTEP01_CONF_PARAM_TOFF_MIN_DEVICE_2));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)POWERSTEP01_CONF_PARAM_CLOCK_SETTING_DEVICE_2 | 
          (uint16_t)POWERSTEP01_CONF_PARAM_SW_MODE_DEVICE_2       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_TQ_REG_DEVICE_2        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_OC_SD_DEVICE_2         | 
          (uint16_t)POWERSTEP01_CONF_PARAM_UVLOVAL_DEVICE_2       | 
          (uint16_t)POWERSTEP01_CONF_PARAM_VCCVAL_DEVICE_2        | 
          (uint16_t)POWERSTEP01_CONF_PARAM_TSW_DEVICE_2           |
          (uint16_t)POWERSTEP01_CONF_PARAM_PRED_DEVICE_2);          
      }
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
void Powerstep01_SetDeviceParamsToGivenValues(uint8_t deviceId,
                                              powerstep01_Init_u_t *initPrm)
{
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ABS_POS, 0);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_EL_POS, 0);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MARK, 0);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ACC,
    Powerstep01_AccDec_Steps_s2_to_RegVal(initPrm->cm.cp.acceleration));
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_DEC,
    Powerstep01_AccDec_Steps_s2_to_RegVal(initPrm->cm.cp.deceleration));
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MAX_SPEED,
    Powerstep01_MaxSpd_Steps_s_to_RegVal(initPrm->cm.cp.maxSpeed));
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_MIN_SPEED,
    initPrm->cm.cp.lowSpeedOptimization|
    Powerstep01_MaxSpd_Steps_s_to_RegVal(initPrm->cm.cp.minSpeed));
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FS_SPD,
    initPrm->cm.cp.boostMode|
    Powerstep01_FSSpd_Steps_s_to_RegVal(initPrm->cm.cp.fullStepSpeed));
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_OCD_TH,
    Powerstep01_StallOcd_Th_to_RegVal(initPrm->cm.cp.ocdThreshold));
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STEP_MODE,
    (uint8_t)initPrm->cm.cp.syncClockSelection|
    (uint8_t)initPrm->cm.cp.cmVmSelection|
    (uint8_t)(uint8_t)initPrm->cm.cp.stepMode);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ALARM_EN,
    initPrm->cm.cp.alarmsSelection);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG1,
    (uint16_t)initPrm->cm.cp.iGate|
    (uint16_t)initPrm->cm.cp.tcc|
    (uint16_t)initPrm->cm.cp.tBoost|
    (uint16_t)initPrm->cm.cp.wdEn);
  Powerstep01_CmdSetParam(deviceId, POWERSTEP01_GATECFG2,
    (uint16_t)initPrm->cm.cp.tBlank|
    (uint16_t)initPrm->cm.cp.tdt);  
  if (initPrm->cm.cp.cmVmSelection == POWERSTEP01_CM_VM_VOLTAGE)
  {
    //Voltage mode
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_INT_SPD,
      Powerstep01_IntSpd_Steps_s_to_RegVal(
        initPrm->vm.intersectSpeed));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_K_THERM,
      Powerstep01_KTherm_Comp_to_RegVal(
        initPrm->vm.thermalCompensationFactor)); 
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_STALL_TH,
      Powerstep01_StallOcd_Th_to_RegVal(
        initPrm->vm.stallThreshold));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_HOLD,
      Powerstep01_Kval_Perc_to_RegVal(
        initPrm->vm.kvalHold));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_RUN,
      Powerstep01_Kval_Perc_to_RegVal(
        initPrm->vm.kvalRun));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_ACC,
      Powerstep01_Kval_Perc_to_RegVal(
        initPrm->vm.kvalAcc));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_KVAL_DEC,
      Powerstep01_Kval_Perc_to_RegVal(
        initPrm->vm.kvalDec));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_ST_SLP,
      Powerstep01_BEMFslope_Perc_to_RegVal(
        initPrm->vm.startSlope));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_ACC,
      Powerstep01_BEMFslope_Perc_to_RegVal(
        initPrm->vm.accelerationFinalSlope));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_FN_SLP_DEC,
      Powerstep01_BEMFslope_Perc_to_RegVal(
        initPrm->vm.decelerationFinalSlope));
    Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
      (uint16_t)initPrm->vm.oscClkSel| 
      (uint16_t)initPrm->vm.swMode | 
      (uint16_t)initPrm->vm.enVsComp| 
      (uint16_t)initPrm->vm.ocSd| 
      (uint16_t)initPrm->vm.uvloVal| 
      (uint16_t)initPrm->vm.vccVal| 
      (uint16_t)initPrm->vm.fPwmInt| 
      (uint16_t)initPrm->vm.fPwmDec);
  }
  else
  {
    // Current mode
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_HOLD,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            initPrm->cm.tvalHold));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_RUN,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            initPrm->cm.tvalRun));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_ACC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            initPrm->cm.tvalAcc));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TVAL_DEC,
          Powerstep01_Tval_RefVoltage_to_RegVal(
            initPrm->cm.tvalDec));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_T_FAST,
          (uint8_t)initPrm->cm.toffFast|
          (uint8_t)initPrm->cm.fastStep);
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TON_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            initPrm->cm.tonMin));
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_TOFF_MIN,
          Powerstep01_Tmin_Time_to_RegVal(
            initPrm->cm.toffMin));       
        Powerstep01_CmdSetParam(deviceId, POWERSTEP01_CONFIG,
          (uint16_t)initPrm->cm.oscClkSel| 
          (uint16_t)initPrm->cm.swMode| 
          (uint16_t)initPrm->cm.tqReg| 
          (uint16_t)initPrm->cm.ocSd| 
          (uint16_t)initPrm->cm.uvloVal| 
          (uint16_t)initPrm->cm.vccVal|
          (uint16_t)initPrm->cm.tsw|
          (uint16_t)initPrm->cm.predEn);
  }
}

/******************************************************//**
 * @brief Write and receive a byte via SPI
 * @param[in] pByteToTransmit pointer to the byte to transmit
 * @param[in] pReceivedByte pointer to the received byte
 * @retval None
 *********************************************************/
void Powerstep01_WriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte)
{
  if (Powerstep01_Board_SpiWriteBytes(pByteToTransmit, pReceivedByte, numberOfDevices) != 0)
  {
    Powerstep01_ErrorHandler(POWERSTEP01_ERROR_1);
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
inline uint16_t Powerstep01_AccDec_Steps_s2_to_RegVal(float steps_s2)
{
  return ((uint16_t)(((float)(steps_s2)*0.068719476736f)+0.5f));
}

/**********************************************************
 * @brief Convert the ACC or DEC register value into step/s^2
 * @param[in] regVal The ACC or DEC register value
 * @retval The speed as steps/s
 **********************************************************/
inline float Powerstep01_AccDec_RegVal_to_Steps_s2(uint32_t regVal)
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
inline uint8_t Powerstep01_BEMFslope_Perc_to_RegVal(float percentage)
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
inline float Powerstep01_BEMFslope_RegVal_to_Perc(uint32_t regVal)
{
  return (((float)(regVal))*0.00156862745098f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a FS_SPD 
 * register value
 * @param[in] steps_s the speed as steps/s, range 15.25 to 15610 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint16_t Powerstep01_FSSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)((float)(steps_s)*0.065536f));
}

/**********************************************************
 * @brief Convert the FS_SPD register value into step/s
 * @param[in] regVal The FS_SPD register value
 * @retval The full Step speed as steps/s
 **********************************************************/
inline float Powerstep01_FSSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)regVal+0.999f)*15.258789f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a INT_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 976.5 steps/s
 * @retval The intersect speed as steps/tick
 **********************************************************/
inline uint16_t Powerstep01_IntSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*16.777216f)+0.5f));
}

/**********************************************************
 * @brief Convert the INT_SPEED register value into step/s
 * @param[in] regVal The INT_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float Powerstep01_IntSpd_RegVal_to_Steps_s(uint32_t regVal)
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
inline uint8_t Powerstep01_KTherm_Comp_to_RegVal(float compFactor)
{
  return ((uint8_t)((((float)(compFactor)-1.0f)*32.0f)+0.5f));
}

/**********************************************************
 * @brief Convert the K_THERM register value into a float 
 * formatted thermal compensation factor
 * @param[in] regVal The K_THERM register value
 * @retval The float formatted thermal compensation factor
 **********************************************************/
inline float Powerstep01_KTherm_RegVal_to_Comp(uint32_t regVal)
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
inline uint8_t Powerstep01_Kval_Perc_to_RegVal(float percentage)
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
inline float Powerstep01_Kval_RegVal_to_Perc(uint32_t regVal)
{
  return (((float)(regVal))*0.390625f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a MAX_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 15.25 to 15610 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint16_t Powerstep01_MaxSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*0.065536f)+0.5f));
}

/**********************************************************
 * @brief Convert the MAX_SPEED register value into step/s
 * @param[in] regVal The MAX_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float Powerstep01_MaxSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*15.258789f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a MIN_SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 976.3 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint16_t Powerstep01_MinSpd_Steps_s_to_RegVal(float steps_s)
{
  return ((uint16_t)(((float)(steps_s)*4.194304f)+0.5f));
}

/**********************************************************
 * @brief Convert the MIN_SPEED register value into step/s
 * @param[in] regVal The MIN_SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float Powerstep01_MinSpd_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.238418579f);
}

/**********************************************************
 * @brief Convert the float formatted speed into a SPEED 
 * register value
 * @param[in] steps_s the speed as steps/s, range 0 to 15625 steps/s
 * @retval The speed as steps/tick
 **********************************************************/
inline uint32_t Powerstep01_Speed_Steps_s_to_RegVal(float steps_s)
{
  return ((uint32_t)(((float)(steps_s)*67.108864f)+0.5f));
}

/**********************************************************
 * @brief Convert the SPEED register value into step/s
 * @param[in] regVal The SPEED register value
 * @retval The speed as steps/s
 **********************************************************/
inline float Powerstep01_Speed_RegVal_to_Steps_s(uint32_t regVal)
{
  return (((float)(regVal))*0.01490116119f);
}

/**********************************************************
 * @brief Convert STALL or OCD Threshold voltage in mV to 
 * values for STALL_TH or OCD_TH register
 * @param[in] mV voltage in mV, range 31.25mV to 1000mV
 * @retval value for STALL_TH or OCD_TH register
 **********************************************************/
inline uint8_t Powerstep01_StallOcd_Th_to_RegVal(float mV)
{
  return ((uint8_t)((((float)(mV)-31.25f)*0.032f)+0.5f));
}

/**********************************************************
 * @brief Convert values from STALL_TH or OCD_TH register 
 * to mV
 * @param[in] regVal The STALL_TH or OCD_TH register value
 * @retval voltage in mV
 **********************************************************/
inline float Powerstep01_StallOcd_RegVal_to_Th(uint32_t regVal)
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
inline uint8_t Powerstep01_Tval_RefVoltage_to_RegVal(float voltage_mV)
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
inline float Powerstep01_Tval_RegVal_to_RefVoltage(uint32_t regVal)
{
  return (((float)(regVal+1))*7.8125f);
}

/**********************************************************
 * @brief Convert time in us to values for TON_MIN register
 * @param[in] tmin_us time in us, range 0.5us to 64us
 * @retval value for TON_MIN register
 **********************************************************/
inline uint8_t Powerstep01_Tmin_Time_to_RegVal(float tmin_us)
{
  return ((uint8_t)((((float)(tmin_us)-0.5f)*2.0f)+0.5f));
}

/**********************************************************
 * @brief Convert values for TON_MIN or TOFF_MIN register to time in us
 * @param[in] regVal The TON_MIN or TOFF_MIN register value
 * @retval time in us
 **********************************************************/
inline float Powerstep01_Tmin_RegVal_to_Time(uint32_t regVal)
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
