/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01_hal.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "PS01_Global.h"
#include "PS01_Register.h"

#if defined(OI_STEPPER)
#define POWERSTEP01_NUMBER_OF_DEVICES 2
#else
#define POWERSTEP01_NUMBER_OF_DEVICES 1
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    DEVICE1 = 0,
    DEVICE2 = 1,
    NUMBER_OF_DEVICES,
} PS01_Device_t;

typedef struct {
    int num_of_devices;
    spi_host_device_t spi_host;
    int spi_freq;
    int spi_pin_mosi;
    int spi_pin_miso;
    int spi_pin_clk;
    int spi_pin_cs;
    gpio_num_t pin_busy_sync[NUMBER_OF_DEVICES];
    gpio_num_t pin_flag[NUMBER_OF_DEVICES];
    gpio_num_t pin_sw[NUMBER_OF_DEVICES];
    gpio_num_t pin_stby_rst[NUMBER_OF_DEVICES];
    gpio_num_t pwm_pin_stck[NUMBER_OF_DEVICES];
    ledc_timer_t pwm_timer[NUMBER_OF_DEVICES];
    ledc_channel_t pwm_channel[NUMBER_OF_DEVICES];
} PS01_Hal_Config_t;

void PS01_Hal_SetConfig(PS01_Hal_Config_t *config);                                 // Config devices

void PS01_Hal_Delay(uint32_t delay);                                                // Delay of the requested number of milliseconds
void PS01_Hal_EnableIrq(void);                                                      // Disable Irq
void PS01_Hal_DisableIrq(void);                                                     // Enable Irq
void PS01_Hal_GpioInit(uint8_t deviceId);                                           // Initialise GPIOs used for a powerSTEP01 device
void PS01_Hal_StepClockInit(uint8_t deviceId);                                      // Initialise the step clock for a powerSTEP01 device
void PS01_Hal_StartStepClock(uint8_t deviceId, uint16_t newFreq);                   // Start the step clock by using the given frequency
void PS01_Hal_StopStepClock(uint8_t deviceId);                                      // Stop the PWM uses for the step clock
void PS01_Hal_ReleaseReset(uint8_t deviceId);                                       // Reset the powerSTEP01 reset pin 
void PS01_Hal_Reset(uint8_t deviceId);                                              // Set the powerSTEP01 reset pin

uint8_t PS01_Hal_SpiInit(void);                                                     // Initialise the SPI used for powerSTEP01s
uint8_t PS01_Hal_SpiWriteBytes(uint8_t *pByteToTransmit, uint8_t *pReceivedByte);   // Write bytes to the powerSTEP01s via SPI            

void PS01_Hal_SetSwitchLevel(uint8_t deviceId, uint32_t level);                     // Set switch level
uint8_t PS01_Hal_GetBusyLevel(uint8_t deviceId);                                    // Returns the BUSY pin state
uint8_t PS01_Hal_GetFlagLevel(uint8_t deviceId);                                    // Returns the FLAG pin state

void PS01_Hal_Init(uint8_t deviceId);

void PS01_Hal_AttachBusyInterrupt(void(*callback)(uint8_t));
void PS01_Hal_DetachBusyInterrupt(void);

void PS01_Hal_AttachFlagInterrupt(void(*callback)(uint8_t, uint16_t));
void PS01_Hal_DetachFlagInterrupt(void);

#ifdef __cplusplus
}
#endif