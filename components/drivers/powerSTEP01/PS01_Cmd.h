/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01_commands.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "PS01_Global.h"
#include "PS01_Register.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	BACKWARD_DIR	= (uint8_t)0,
	FORWARD_DIR 	= (uint8_t)1,
	UNKNOW_DIR 		= (uint8_t)0xFF
} motorDir_t;

typedef enum {
	ACTION_RESET	= (uint8_t)0x00,
	ACTION_COPY  	= (uint8_t)0x08
} motorAction_t;

void PS01_Cmd_Nop(uint8_t deviceId);
void PS01_Cmd_SetParam(uint8_t deviceId, uint32_t param, uint32_t value);
uint32_t PS01_Cmd_GetParam(uint8_t deviceId, uint32_t param);
void PS01_Cmd_Run(uint8_t deviceId, motorDir_t direction, uint32_t speed);
void PS01_Cmd_StepClock(uint8_t deviceId, motorDir_t direction);
void PS01_Cmd_Move(uint8_t deviceId, motorDir_t direction, uint32_t n_step);
void PS01_Cmd_GoTo(uint8_t deviceId, int32_t abs_pos);
void PS01_Cmd_GoToDir(uint8_t deviceId, motorDir_t direction, int32_t abs_pos);
void PS01_Cmd_GoUntil(uint8_t deviceId, motorAction_t action, motorDir_t direction, uint32_t speed);
void PS01_Cmd_ReleaseSw(uint8_t deviceId, motorAction_t action, motorDir_t direction);
void PS01_Cmd_GoHome(uint8_t deviceId);
void PS01_Cmd_GoMark(uint8_t deviceId);
void PS01_Cmd_ResetPos(uint8_t deviceId);
void PS01_Cmd_ResetDevice(uint8_t deviceId);
void PS01_Cmd_SoftStop(uint8_t deviceId);
void PS01_Cmd_HardStop(uint8_t deviceId);
void PS01_Cmd_SoftHiZ(uint8_t deviceId);
void PS01_Cmd_HardHiZ(uint8_t deviceId);
uint16_t PS01_Cmd_GetStatus(uint8_t deviceId);

/* Other functions */
void PS01_FetchAndClearAllStatus(void);
uint16_t PS01_GetFetchedStatus(uint8_t deviceId);
void PS01_QueueCommands(uint8_t deviceId, uint8_t command, int32_t value);
void PS01_SendQueuedCommands(void);
void PS01_InitCommands(void);


#ifdef __cplusplus
}
#endif