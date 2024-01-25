#include "PS01_Cmd.h"
#include "PS01_Hal.h"

static uint8_t spiTxBursts[POWERSTEP01_CMD_ARG_MAX_NB_BYTES][POWERSTEP01_NUMBER_OF_DEVICES];
static uint8_t spiRxBursts[POWERSTEP01_CMD_ARG_MAX_NB_BYTES][POWERSTEP01_NUMBER_OF_DEVICES];
static volatile bool spiPreemtionByIsr = 0;
static volatile bool isrFlag = 0;

/******************************************************//**
 * @brief  Sends a command to a given device Id via the SPI
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] param Command to send (all Powerstep01 commmands 
 * except POWERSTEP01_SET_PARAM, POWERSTEP01_GET_PARAM, 
 * POWERSTEP01_GET_STATUS)
 * @param[in] value arguments to send on 32 bits
 * @retval None
 **********************************************************/
void PS01_SendCommand(uint8_t deviceId, uint8_t param, uint32_t value)
{
	if (POWERSTEP01_NUMBER_OF_DEVICES > deviceId)
	{
		uint32_t loop;
		uint8_t maxArgumentNbBytes = 0;
		uint8_t spiIndex = POWERSTEP01_NUMBER_OF_DEVICES - deviceId - 1;
		bool itDisable = 0; 
		
		do
		{
		spiPreemtionByIsr = 0;
		if (itDisable)
		{
			/* re-enable PS01_Hal_EnableIrq if disable in previous iteration */
			PS01_Hal_EnableIrq();
			itDisable = 0;
		}    
		for (loop = 0; loop < POWERSTEP01_NUMBER_OF_DEVICES; loop++)
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
			spiTxBursts[0][spiIndex] = param;
			spiTxBursts[1][spiIndex] = (uint8_t)(value >> 16);
			spiTxBursts[2][spiIndex] = (uint8_t)(value >> 8);
			spiTxBursts[3][spiIndex] = (uint8_t)(value);
			maxArgumentNbBytes = 3;
			break;
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
		PS01_Hal_DisableIrq();
		itDisable = 1;
		} while (spiPreemtionByIsr); // check pre-emption by ISR
		for (loop = POWERSTEP01_CMD_ARG_MAX_NB_BYTES - 1 - maxArgumentNbBytes; 
			loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES; 
			loop++)
		{
		PS01_Hal_SpiWriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
		}
		/* re-enable PS01_Hal_EnableIrq after SPI transfers*/
		PS01_Hal_EnableIrq();
	}
}

/******************************************************//**
 * @brief Issues PowerStep01 Get Param command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] param PowerStep01 register address
 * @retval Register value - 1 to 3 bytes (depends on register)
 *********************************************************/
uint32_t PS01_Cmd_GetParam(uint8_t deviceId, uint32_t param)
{
	uint32_t spiRxData = 0;

	if (POWERSTEP01_NUMBER_OF_DEVICES > deviceId)
	{
		uint32_t loop;
		uint8_t maxArgumentNbBytes = 0;
		uint8_t spiIndex = POWERSTEP01_NUMBER_OF_DEVICES - deviceId - 1;
		bool itDisable = 0;
		
		do
		{
		spiPreemtionByIsr = 0;
		if (itDisable)
		{
			/* re-enable PS01_Hal_EnableIrq if disable in previous iteration */
			PS01_Hal_EnableIrq();
			itDisable = 0;
		}
		for (loop = 0; loop < POWERSTEP01_NUMBER_OF_DEVICES; loop++)
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
		PS01_Hal_DisableIrq();
		itDisable = 1;
		} while (spiPreemtionByIsr); // check pre-emption by ISR
		for (loop = POWERSTEP01_CMD_ARG_MAX_NB_BYTES-1-maxArgumentNbBytes;
			loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES;
			loop++)
		{
		PS01_Hal_SpiWriteBytes(&spiTxBursts[loop][0],
								&spiRxBursts[loop][0]);
		}
		spiRxData = ((uint32_t)spiRxBursts[1][spiIndex] << 16)|
					(spiRxBursts[2][spiIndex] << 8) |
					(spiRxBursts[3][spiIndex]);    
		/* re-enable PS01_Hal_EnableIrq after SPI transfers*/
		PS01_Hal_EnableIrq();
	}

	return (spiRxData);
}

/******************************************************//**
 * @brief Issues PowerStep01 Get Status command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval Status Register content
 *********************************************************/
uint16_t PS01_Cmd_GetStatus(uint8_t deviceId)
{
	uint16_t status = 0;
	if (POWERSTEP01_NUMBER_OF_DEVICES > deviceId)
	{
		uint32_t loop;
		uint8_t spiIndex = POWERSTEP01_NUMBER_OF_DEVICES - deviceId - 1;
		bool itDisable = 0; 
	
		do
		{
		spiPreemtionByIsr = 0;
		if (itDisable)
		{
			/* re-enable PS01_Hal_EnableIrq if disable in previous iteration */
			PS01_Hal_EnableIrq();
			itDisable = 0;
		}    
		for (loop = 0; loop < POWERSTEP01_NUMBER_OF_DEVICES; loop++)
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
		PS01_Hal_DisableIrq();
		itDisable = 1;
		} while (spiPreemtionByIsr); // check pre-emption by ISR  
		for (loop = 0; loop < POWERSTEP01_CMD_ARG_NB_BYTES_GET_STATUS + POWERSTEP01_RSP_NB_BYTES_GET_STATUS; loop++)
		{
		PS01_Hal_SpiWriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
		}
		status = (spiRxBursts[1][spiIndex] << 8) | (spiRxBursts[2][spiIndex]);
		/* re-enable PS01_Hal_EnableIrq after SPI transfers*/
		PS01_Hal_EnableIrq();    
	}
	return (status);
}

/******************************************************//**
 * @brief Issues PowerStep01 Go Home command (Shortest path to zero position)
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void PS01_Cmd_GoHome(uint8_t deviceId)
{
   	PS01_SendCommand(deviceId, POWERSTEP01_GO_HOME, 0);   
}

/******************************************************//**
 * @brief Issues PowerStep01 Go Mark command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void PS01_Cmd_GoMark(uint8_t deviceId)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_GO_MARK, 0);     
}

/******************************************************//**
 * @brief Issues PowerStep01 Go To command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] abs_pos absolute position in steps in agreement
 * with the step mode where requested to move
 * @retval None
 *********************************************************/
void PS01_Cmd_GoTo(uint8_t deviceId, int32_t abs_pos)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_GO_TO, abs_pos);  
}

/******************************************************//**
 * @brief Issues PowerStep01 Go To Dir command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] direction movement direction
 * @param[in] abs_pos absolute position in steps in agreement
 * with the step mode where requested to move
 * @retval None
 *********************************************************/
void PS01_Cmd_GoToDir(uint8_t deviceId, 
                            motorDir_t direction,
                            int32_t abs_pos)
{
	PS01_SendCommand(deviceId, 
							(uint8_t)POWERSTEP01_GO_TO_DIR | 
							(uint8_t)direction, abs_pos);  
}

/******************************************************//**
 * @brief Issues PowerStep01 Go Until command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] action ACTION_RESET or ACTION_COPY
 * @param[in] direction movement direction
 * @param[in] speed in 2^-28 step/tick
 * @retval None
 *********************************************************/
void PS01_Cmd_GoUntil(uint8_t deviceId, 
                           motorAction_t action, 
                           motorDir_t direction, 
                           uint32_t speed)
{
	PS01_SendCommand(deviceId,
						(uint8_t)POWERSTEP01_GO_UNTIL | (uint8_t)action | (uint8_t)direction,
						speed); 
}

/******************************************************//**
 * @brief Issues PowerStep01 Hard HiZ command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The HardHiZ command immediately disables the power bridges
 * (high impedance state) and raises the HiZ flag. 
 * When the motor is stopped, a HardHiZ command forces the bridges 
 * to enter high impedance state.
 * This command can be given anytime and is immediately executed.
 *********************************************************/
void PS01_Cmd_HardHiZ(uint8_t deviceId)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_HARD_HIZ, 0);    
}

/******************************************************//**
 * @brief Issues PowerStep01 Hard Stop command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 * @note The HardStop command causes an immediate motor stop with
 * infinite deceleration.
 * When the motor is in high impedance state, a HardStop command
 * forces the bridges to exit high impedance state; no motion is performed.
 * This command can be given anytime and is immediately executed.
 * This command keeps the BUSY flag low until the motor is stopped.
 *********************************************************/
void PS01_Cmd_HardStop(uint8_t deviceId)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_HARD_STOP, 0);         
}

/******************************************************//**
 * @brief Issues PowerStep01 Move command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction
 * @param[in] n_step number of steps
 * @retval None
 *********************************************************/
void PS01_Cmd_Move(uint8_t deviceId, 
                         motorDir_t direction, 
                         uint32_t n_step)
{
	PS01_SendCommand(deviceId, 
							(uint8_t)POWERSTEP01_MOVE | 
							(uint8_t)direction, n_step);  
}

/******************************************************//**
 * @brief Issues PowerStep01 NOP command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void PS01_Cmd_Nop(uint8_t deviceId)
{
  	/* Send NOP operation code to PowerStep01 */
	PS01_SendCommand(deviceId, POWERSTEP01_NOP, 0);
}

/******************************************************//**
 * @brief Issues PowerStep01 Release SW command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] action type of action to undertake when the SW
 * input is forced high
 * @param[in] direction movement direction
 * @retval None
 *********************************************************/
void PS01_Cmd_ReleaseSw(uint8_t deviceId, 
                             motorAction_t action, 
                             motorDir_t direction)
{
	PS01_SendCommand(deviceId, 
							(uint8_t)POWERSTEP01_RELEASE_SW | 
							(uint8_t)action | 
							(uint8_t)direction, 
							0); 
}

/******************************************************//**
 * @brief Issues PowerStep01 Reset Device command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void PS01_Cmd_ResetDevice(uint8_t deviceId)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_RESET_DEVICE, 0);         
}

/******************************************************//**
 * @brief Issues PowerStep01 Reset Pos command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @retval None
 *********************************************************/
void PS01_Cmd_ResetPos(uint8_t deviceId)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_RESET_POS, 0);       
}

/******************************************************//**
 * @brief Issues PowerStep01 Run command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction (FORWARD, BACKWARD)
 * @param[in] speed in 2^-28 step/tick
 * @retval None
 *********************************************************/
void PS01_Cmd_Run(uint8_t deviceId, 
                       motorDir_t direction,
                       uint32_t speed)
{
	PS01_SendCommand(deviceId, 
							(uint8_t)POWERSTEP01_RUN|(uint8_t)direction,
							speed);
}

/******************************************************//**
 * @brief Issues the SetParam command to the PowerStep01 of the specified device
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] param Register adress (POWERSTEP01_ABS_POS, POWERSTEP01_MARK,...)
 * @param[in] value Value to set in the register
 * @retval None
 *********************************************************/
void PS01_Cmd_SetParam(uint8_t deviceId, uint32_t param, uint32_t value)
{
	if (POWERSTEP01_NUMBER_OF_DEVICES > deviceId)
	{
		uint32_t loop;
		uint8_t maxArgumentNbBytes = 0;
		uint8_t spiIndex = POWERSTEP01_NUMBER_OF_DEVICES - deviceId - 1;
		bool itDisable = 0;
		
		do
		{
		spiPreemtionByIsr = 0;
		if (itDisable)
		{
			/* re-enable PS01_Hal_EnableIrq if disable in previous iteration */
			PS01_Hal_EnableIrq();
			itDisable = 0;
		}  
		for (loop = 0;loop < POWERSTEP01_NUMBER_OF_DEVICES; loop++)
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
		PS01_Hal_DisableIrq();
		itDisable = 1;
		} while (spiPreemtionByIsr); // check pre-emption by ISR  
		/* SPI transfer */
		for (loop = POWERSTEP01_CMD_ARG_MAX_NB_BYTES - 1 - maxArgumentNbBytes;
			loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES;
			loop++)
		{
		PS01_Hal_SpiWriteBytes(&spiTxBursts[loop][0],&spiRxBursts[loop][0]);
		}
		/* re-enable PS01_Hal_EnableIrq after SPI transfers*/
		PS01_Hal_EnableIrq();
	}
}

/******************************************************//**
 * @brief Issues PowerStep01 Soft HiZ command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
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
void PS01_Cmd_SoftHiZ(uint8_t deviceId)
{
  	PS01_SendCommand(deviceId, POWERSTEP01_SOFT_HIZ, 0);           
}

/******************************************************//**
 * @brief Issues PowerStep01 Soft Stop command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
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
void PS01_Cmd_SoftStop(uint8_t deviceId)
{
	PS01_SendCommand(deviceId, POWERSTEP01_SOFT_STOP, 0);         
}

/******************************************************//**
 * @brief Issues PowerStep01 Step Clock command
 * @param[in] deviceId (from 0 to MAX_POWERSTEP01_NUMBER_OF_DEVICES-1 )
 * @param[in] direction Movement direction (FORWARD, BACKWARD)
 * @retval None
 *********************************************************/
void PS01_Cmd_StepClock(uint8_t deviceId,
                             motorDir_t direction)
{
	PS01_SendCommand(deviceId, 
							(uint8_t)POWERSTEP01_STEP_CLOCK | (uint8_t)direction,
							0);  
}

/* Other functions */

/******************************************************//**
 * @brief Fetch and clear status flags of all devices 
 * by issuing a GET_STATUS command simultaneously  
 * to all devices.
 * Then, the fetched status of each device can be retrieved
 * by using the PS01_GetFetchedStatus function
 * provided there is no other calls to functions which 
 * use the SPI in between.
 * @retval None
 *********************************************************/
void PS01_FetchAndClearAllStatus(void)
{
	uint8_t loop;

	for (loop = 0; loop < POWERSTEP01_NUMBER_OF_DEVICES; loop++)
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
		PS01_Hal_SpiWriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
	}
}

/******************************************************//**
 * @brief Get the value of the STATUS register which was 
 * fetched by using PS01_FetchAndClearAllStatus.
 * The fetched values are available  as long as there
 * no other calls to functions which use the SPI.
 * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @retval Last fetched value of the STATUS register
 *********************************************************/
uint16_t PS01_GetFetchedStatus(uint8_t deviceId)
{
	uint16_t status = 0;
	if (POWERSTEP01_NUMBER_OF_DEVICES > deviceId)
	{
		uint8_t spiIndex = POWERSTEP01_NUMBER_OF_DEVICES - deviceId - 1;
		status = (spiRxBursts[1][spiIndex] << 8) | (spiRxBursts[2][spiIndex]);
	}
	return (status);
}

/******************************************************//**
 * @brief Put commands in queue before synchronous sending
 * done by calling PS01_SendQueuedCommands.
 * Any call to functions that use the SPI between the calls of 
 * PS01_QueueCommands and PS01_SendQueuedCommands 
 * will corrupt the queue.
 * A command for each device of the daisy chain must be 
 * specified before calling PS01_SendQueuedCommands.
 * @param[in] deviceId deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
 * @param[in] command Command to queue (all Powerstep01 commmands 
 * except POWERSTEP01_SET_PARAM, POWERSTEP01_GET_PARAM, 
 * POWERSTEP01_GET_STATUS)
 * @param[in] value argument of the command to queue
 * @retval None
 *********************************************************/
void PS01_QueueCommands(uint8_t deviceId, uint8_t command, int32_t value)
{
	if (POWERSTEP01_NUMBER_OF_DEVICES > deviceId)
	{
		uint8_t spiIndex = POWERSTEP01_NUMBER_OF_DEVICES - deviceId - 1;
		
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
 * @brief Sends commands stored previously in the queue by 
 * PS01_QueueCommands
 * @retval None
 *********************************************************/
void PS01_SendQueuedCommands(void)
{
	uint8_t loop;
	
	for (loop = 0; 
		loop < POWERSTEP01_CMD_ARG_MAX_NB_BYTES; 
		loop++)
	{
		PS01_Hal_SpiWriteBytes(&spiTxBursts[loop][0], &spiRxBursts[loop][0]);
	}
}
