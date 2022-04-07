/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIStepper.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "OIStepper.h"

#if (defined CONFIG_OI_STEPPER) || (defined CONFIG_OI_STEPPER_VERTICAL)

#define BUSY_INTERRUPT_EVENT    (1 << 0)
#define ERROR_HANDLER_EVENT     (1 << 1)
#define FLAG_INTERRUPT_EVENT    (1 << 2)
#define ETOR1_INTERRUPT_EVENT   (1 << 3)
#define ETOR2_INTERRUPT_EVENT   (1 << 4)
#define ETOR3_INTERRUPT_EVENT   (1 << 5)
#define ETOR4_INTERRUPT_EVENT   (1 << 6)

TaskHandle_t OIStepper::_taskHandle = NULL;
EventGroupHandle_t OIStepper::_eventGroupHandle = NULL;

int OIStepper::_limitSwitchToMotor[4] = {-1, -1, -1, -1};
bool OIStepper::_limitSwitchToNotify[4] = {false, false, false, false};
bool OIStepper::_no_logic = true;  /* set switch logic to NO */

#ifdef CONFIG_OI_STEPPER

static const char OI_STEPPER_TAG[] = "OIStepper";

const gpio_num_t OIStepper::_etor[OISTEPPER_NB_ETORS] = { 
    OISTEPPER_GPIO_PIN_ETOR1,
    OISTEPPER_GPIO_PIN_ETOR2,
    OISTEPPER_GPIO_PIN_ETOR3,
    OISTEPPER_GPIO_PIN_ETOR4,
};
#elif defined(CONFIG_OI_STEPPER_VERTICAL)

static const char OI_STEPPER_TAG[] = "OIStepperVE";

const gpio_num_t OIStepper::_etor[OISTEPPER_NB_ETORS] = { 
    OISTEPPER_GPIO_PIN_ETOR1,
    OISTEPPER_GPIO_PIN_ETOR2,
    OISTEPPER_GPIO_PIN_ETOR3,
    OISTEPPER_GPIO_PIN_ETOR4,
    OISTEPPER_GPIO_PIN_ETOR5,
    OISTEPPER_GPIO_PIN_ETOR6,
};
#endif

#ifdef CONFIG_L6470
L6470_DeviceConfig_t OIStepper::device_conf;
#else
Powerstep01_DeviceConfig_t OIStepper::device_conf;
#endif

void OIStepper::init()
{
    OIModuleMaster::init();

    /* Init GPIO service */
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    /* Hardware init */
    ESP_LOGI(OI_STEPPER_TAG, "Hardware initialization");

    /* ETOR */
    gpio_config_t etor_conf = {
        .pin_bit_mask = ((1ULL<<OISTEPPER_GPIO_PIN_ETOR1) |
                        (1ULL<<OISTEPPER_GPIO_PIN_ETOR2) |
                        (1ULL<<OISTEPPER_GPIO_PIN_ETOR3) |
                        (1ULL<<OISTEPPER_GPIO_PIN_ETOR4)),

        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&etor_conf));

    /* Initialize powerstep01 devices */
    device_conf = {
        .spi_host = OISTEPPER_SPI_HOST,
        .spi_freq = OISTEPPER_SPI_FREQ,
        .spi_pin_mosi = OISTEPPER_SPI_PIN_MOSI,
        .spi_pin_miso = OISTEPPER_SPI_PIN_MISO,
        .spi_pin_clk = OISTEPPER_SPI_PIN_CLK,
        .spi_pin_cs = OISTEPPER_SPI_PIN_CS,
        .pin_busy_sync = OISTEPPER_GPIO_PIN_BUSY_SYNC,
        .pin_flag = OISTEPPER_GPIO_PIN_FLAG,
        .num_of_devices = OISTEPPER_NUMBER_OF_DEVICES,
#if OISTEPPER_NUMBER_OF_DEVICES == 1
        .pin_sw = {OISTEPPER_GPIO_PIN_D1_SW},
        .pin_stby_rst = {OISTEPPER_GPIO_PIN_D1_STBY_RST},
#else
        .pin_sw = {OISTEPPER_GPIO_PIN_D1_SW, OISTEPPER_GPIO_PIN_D2_SW},
        .pin_stby_rst = {OISTEPPER_GPIO_PIN_D1_STBY_RST, OISTEPPER_GPIO_PIN_D2_STBY_RST},
#endif
        .pwm_pin_stck = OISTEPPER_PWM_PIN_STCK,
        .pwm_timer = OISTEPPER_PWM_TIMER,
        .pwm_mode = OISTEPPER_PWM_MODE,
        .pwm_channel = OISTEPPER_PWM_CHANNEL,
    };

    /* Configure Device */
#ifdef CONFIG_L6470
    L6470_DeviceConfig(&device_conf);
    L6470_SetNbDevices(OISTEPPER_NUMBER_OF_DEVICES);
    ESP_LOGI(OI_STEPPER_TAG, "Number of L6470 device(s): %d", L6470_GetNbDevices());
#else
    Powerstep01_DeviceConfig(&device_conf);
    Powerstep01_SetNbDevices(OISTEPPER_NUMBER_OF_DEVICES);
    ESP_LOGI(OI_STEPPER_TAG, "Number of powerstep01 device(s): %d", Powerstep01_GetNbDevices());
#endif

    /* Configure motors */
#ifdef CONFIG_OI_STEPPER
    _type = OI_STEPPER;
#ifdef CONFIG_L6470
    L6470_InitMotor(DEVICE1); // Init device 1
    L6470_SetSwitchLevel(DEVICE1, HIGH_LEVEL);
    L6470_InitMotor(DEVICE2); // Init device 2
    L6470_SetSwitchLevel(DEVICE2, HIGH_LEVEL);
#else
    Powerstep01_InitDevice(DEVICE1);
    Powerstep01_InitDevice(DEVICE2);
    Powerstep01_InitNVSParameters(DEVICE1);
    Powerstep01_InitNVSParameters(DEVICE2);
    Powerstep01_SetSwitchLevel(DEVICE1, HIGH_LEVEL);
    Powerstep01_SetSwitchLevel(DEVICE2, HIGH_LEVEL);
#endif
    #elif defined(CONFIG_OI_STEPPER_VERTICAL)
    _type = OI_STEPPERVE;
    Powerstep01_InitDevice(DEVICE1); // Init device 1
    Powerstep01_InitNVSParameters(DEVICE1);
    Powerstep01_SetSwitchLevel(DEVICE1, HIGH_LEVEL);
#endif

    /* Check configuration */
#ifdef CONFIG_L6470
    if (L6470_ReadId() != OISTEPPER_NUMBER_OF_DEVICES) {
        ESP_LOGE(OI_STEPPER_TAG, "Incorrect number of powerstep01 instance");  
    }
#else
    if (Powerstep01_ReadId() != OISTEPPER_NUMBER_OF_DEVICES) {
        ESP_LOGE(OI_STEPPER_TAG, "Incorrect number of powerstep01 instance");  
    }
#endif

    /* create task and event group to manage interrupts */
    _eventGroupHandle = xEventGroupCreate();
    xTaskCreate(_handleEvent, "handle events", 2048, this, 5, NULL);
}

void OIStepper::setSwitchLogic(bool no_logic)
{
    _no_logic = no_logic;
}

int OIStepper::getEtorLevel(Etor_t etor) 
{
    int ret = -1;

    if (etor < OISTEPPER_NB_ETORS)
        ret = gpio_get_level(_etor[etor]);

    return ret;
}

void OIStepper::attachEtorInterrupt(Etor_t etor, void (*callback)(void)) 
{
    if (etor < OISTEPPER_NB_ETORS)
        gpio_isr_handler_add(_etor[etor], (gpio_isr_t)callback, NULL);
}

void OIStepper::detachEtorInterrupt(Etor_t etor) {
    if (etor < OISTEPPER_NB_ETORS)
        gpio_isr_handler_remove(_etor[etor]);
}

void OIStepper::attachLimitSwitch(Etor_t etor, Motor_t motor, bool notify) 
{
    if (etor < OISTEPPER_NB_ETORS)
    {
        if (_limitSwitchToMotor[static_cast<uint8_t>(etor)] == -1)
        {   
            _limitSwitchToMotor[static_cast<uint8_t>(etor)] = motor;
            _limitSwitchToNotify[static_cast<uint8_t>(etor)] = notify;
            switch (etor) {
                case ETOR1 : 
                    attachEtorInterrupt(etor, [](void) {
                        xEventGroupSetBits(_eventGroupHandle, ETOR1_INTERRUPT_EVENT);
                    });
                    break;
                case ETOR2 : 
                    attachEtorInterrupt(etor, [](void) {
                        xEventGroupSetBits(_eventGroupHandle, ETOR2_INTERRUPT_EVENT);
                    });
                    break;
                case ETOR3 : 
                    attachEtorInterrupt(etor, [](void) {
                        xEventGroupSetBits(_eventGroupHandle, ETOR3_INTERRUPT_EVENT);
                    });
                    break;
                case ETOR4 : 
                    attachEtorInterrupt(etor, [](void) {
                        xEventGroupSetBits(_eventGroupHandle, ETOR4_INTERRUPT_EVENT);
                    });
                    break;
                default:
                    break;
            }
        }
    }
}

void OIStepper::detachLimitSwitch(Etor_t etor) 
{
    if (etor < OISTEPPER_NB_ETORS)
    {
        if (_limitSwitchToMotor[static_cast<uint8_t>(etor)] != -1)
        {
            L6470_SetSwitchLevel(_limitSwitchToMotor[etor], HIGH_LEVEL);
            _limitSwitchToMotor[static_cast<uint8_t>(etor)] = -1;
            _limitSwitchToNotify[static_cast<uint8_t>(etor)] = false;
            detachEtorInterrupt(etor);
        }
    }
}

void OIStepper::attachBusyInterrupt(void (*callback)(void)) 
{
#ifdef CONFIG_L6470
    L6470_AttachBusyInterrupt(callback);
#else
    Powerstep01_AttachBusyInterrupt(callback);
#endif
}

void OIStepper::attachErrorHandler(void (*callback)(uint16_t)) 
{
#ifdef CONFIG_L6470
    L6470_AttachErrorHandler(callback);
#else
    Powerstep01_AttachErrorHandler(callback);
#endif
}

void OIStepper::attachFlagInterrupt(void (*callback)(void)) 
{
#ifdef CONFIG_L6470
    L6470_AttachFlagInterrupt(callback);
#else
    Powerstep01_AttachFlagInterrupt(callback);
#endif
}

uint16_t OIStepper::getStatus(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    return L6470_CmdGetStatus(motor);
#else
    return Powerstep01_CmdGetStatus(motor);
#endif
}

void OIStepper::goHome(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdGoHome(motor);
#else
    Powerstep01_CmdGoHome(motor);
#endif
}

void OIStepper::goMark(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdGoMark(motor);
#else
    Powerstep01_CmdGoMark(motor);
#endif
}

void OIStepper::goTo(Motor_t motor, int32_t abs_pos) const 
{
#ifdef CONFIG_L6470
    L6470_CmdGoTo(motor, abs_pos);
#else
    Powerstep01_CmdGoTo(motor, abs_pos);
#endif
}

void OIStepper::goToDir(Motor_t motor, motorDir_t direction, int32_t abs_pos) const 
{
#ifdef CONFIG_L6470
    L6470_CmdGoToDir(motor, direction, abs_pos);
#else
    Powerstep01_CmdGoToDir(motor, direction, abs_pos);
#endif
}

void OIStepper::goUntil(Motor_t motor, motorAction_t action, motorDir_t direction, uint32_t speed) const 
{
#ifdef CONFIG_L6470
    L6470_CmdGoUntil(motor, action, direction, speed);
#else
    Powerstep01_CmdGoUntil(motor, action, direction, speed);
#endif
}

void OIStepper::hardHiZ(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdHardHiZ(motor);
#else
    Powerstep01_CmdHardHiZ(motor);
#endif
}

void OIStepper::hardStop(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdHardStop(motor);
#else
    Powerstep01_CmdHardStop(motor);
#endif
}

void OIStepper::move(Motor_t motor, motorDir_t direction, uint32_t n_step) const 
{
#ifdef CONFIG_L6470
    L6470_CmdMove(motor, direction, n_step);
#else
    Powerstep01_CmdMove(motor, direction, n_step);
#endif
}

void OIStepper::releaseSw(Motor_t motor, motorAction_t action, motorDir_t direction) const 
{
#ifdef CONFIG_L6470
    L6470_CmdReleaseSw(motor, action, direction);
#else
    Powerstep01_CmdReleaseSw(motor, action, direction);
#endif
}

void OIStepper::resetDevice(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdResetDevice(motor);
#else
    Powerstep01_CmdResetDevice(motor);
#endif
}

void OIStepper::resetPos(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdResetPos(motor);
#else
    Powerstep01_CmdResetPos(motor);
#endif
}

void OIStepper::run(Motor_t motor, motorDir_t direction, uint32_t speed) const 
{
#ifdef CONFIG_L6470
    L6470_CmdRun(motor, direction, speed);
#else
    Powerstep01_CmdRun(motor, direction, speed);
#endif
}

void OIStepper::softHiZ(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdSoftHiZ(motor);
#else
    Powerstep01_CmdSoftHiZ(motor);
#endif
}

void OIStepper::softStop(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    L6470_CmdSoftStop(motor);
#else
    Powerstep01_CmdSoftStop(motor);
#endif
}

void OIStepper::stepClock(Motor_t motor, motorDir_t direction) const 
{
#ifdef CONFIG_L6470
    L6470_CmdStepClock(motor, direction);
#else
    Powerstep01_CmdStepClock(motor, direction);
#endif
}

int32_t OIStepper::getMark(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    return L6470_GetMark(motor);
#else
    return Powerstep01_GetMark(motor);
#endif
}

int32_t OIStepper::getPosition(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    return L6470_GetPosition(motor);
#else
    return Powerstep01_GetPosition(motor);
#endif
}

bool OIStepper::isDeviceBusy(Motor_t motor) const 
{
#ifdef CONFIG_L6470
    return L6470_IsDeviceBusy(motor);
#else
    return Powerstep01_IsDeviceBusy(motor);
#endif
}

bool OIStepper::selectStepMode(Motor_t motor, motorStepMode_t stepMode) const 
{
#ifdef CONFIG_L6470
    return L6470_SelectStepMode(motor, stepMode);
#else
    return Powerstep01_SelectStepMode(motor, stepMode);
#endif
}

void OIStepper::setHome(Motor_t motor, int32_t position) const 
{
#ifdef CONFIG_L6470
    L6470_SetHome(motor, position);
#else
    Powerstep01_SetHome(motor, position);
#endif
}

void OIStepper::setMark(Motor_t motor, int32_t position) const 
{
#ifdef CONFIG_L6470
    L6470_SetMark(motor, position);
#else
    Powerstep01_SetMark(motor, position);
#endif
}

void OIStepper::setParam(Motor_t motor, uint32_t param, uint32_t value) const 
{
#ifdef CONFIG_L6470
    L6470_SetMotorConfigToNVS(motor, param, (uint16_t)value);
    L6470_CmdSetParam(motor, param, value);
#else
    Powerstep01_SetMotorConfigToNVS(motor, param, (uint16_t)value);
    Powerstep01_CmdSetParam(motor, param, value);
#endif
}

uint32_t OIStepper::getParam(Motor_t motor, uint32_t param) const 
{
#ifdef CONFIG_L6470
    return L6470_CmdGetParam(motor, param);
#else
    return Powerstep01_CmdGetParam(motor, param);
#endif
}

float OIStepper::getSpeed(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_SPEED);
}

float OIStepper::getAcceleration(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_ACC);
}

float OIStepper::getDeceleration(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_DEC);
}

float OIStepper::getMaxSpeed(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_MAX_SPEED);
}

float OIStepper::getMinSpeed(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_MIN_SPEED);
}

float OIStepper::getFsSpeed(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_FS_SPD);
}

float OIStepper::getKValHold(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_KVAL_HOLD);
}

float OIStepper::getKValRun(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_KVAL_RUN);
}

float OIStepper::getKValAcceleration(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_KVAL_ACC);
}

float OIStepper::getKValDeceleration(Motor_t motor) const
{
    return Powerstep01_GetAnalogValue(motor, POWERSTEP01_KVAL_DEC);  
}

void OIStepper::setAcceleration(Motor_t motor, float acceleration) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_ACC, acceleration))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setDeceleration(Motor_t motor, float deceleration) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_DEC, deceleration))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setMaxSpeed(Motor_t motor, float speed) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_MAX_SPEED, speed))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setMinSpeed(Motor_t motor, float speed) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_MIN_SPEED, speed))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__); 
    }
}

void OIStepper::setFsSpeed(Motor_t motor, float speed) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_FS_SPD, speed))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setKValHold(Motor_t motor, float kval) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_KVAL_HOLD, kval))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setKValRun(Motor_t motor, float kval) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_KVAL_RUN, kval))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setKValAcceleration(Motor_t motor, float kval) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_KVAL_ACC, kval))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}

void OIStepper::setKValDeceleration(Motor_t motor, float kval) const
{
    if (!Powerstep01_SetAnalogValue(motor, POWERSTEP01_KVAL_DEC, kval))
    {
        ESP_LOGW(OI_STEPPER_TAG, "%s() error to set value", __FUNCTION__);
    }
}



/* Private */

void OIStepper::_handleEvent(void *pvParameters)
{
    OIStepper* stepper = (OIStepper*)pvParameters;
    EventBits_t eventBits;
    uint8_t index = 0xFF;
    assert(stepper != NULL);

    while(1)
    {
        eventBits = xEventGroupWaitBits(_eventGroupHandle, 
            (BUSY_INTERRUPT_EVENT | ERROR_HANDLER_EVENT | FLAG_INTERRUPT_EVENT | 
            ETOR1_INTERRUPT_EVENT | ETOR2_INTERRUPT_EVENT | ETOR3_INTERRUPT_EVENT | ETOR4_INTERRUPT_EVENT), 
            pdTRUE, pdFALSE, portMAX_DELAY
        );

        index = 0xFF;
        
        if (eventBits & ETOR1_INTERRUPT_EVENT) {
            index = static_cast<uint8_t>(ETOR1);
        }
        else if (eventBits & ETOR2_INTERRUPT_EVENT) {
            index = static_cast<uint8_t>(ETOR2);
        }
        else if (eventBits & ETOR3_INTERRUPT_EVENT) {
            index = static_cast<uint8_t>(ETOR3);
        }
        else if (eventBits & ETOR4_INTERRUPT_EVENT) {
            index = static_cast<uint8_t>(ETOR4);
        }

        if (index != 0xFF)
        {
            if(_limitSwitchToMotor[index] != -1)
            {
            #ifdef CONFIG_L6470
                if(_no_logic)
                {
                    L6470_SetSwitchLevel(_limitSwitchToMotor[index], gpio_get_level(_etor[index])?LOW_LEVEL:HIGH_LEVEL);
                }
                else
                {
                    L6470_SetSwitchLevel(_limitSwitchToMotor[index], gpio_get_level(_etor[index]));
                }
            #else
                if(_no_logic)
                {
                    Powerstep01_SetSwitchLevel(_limitSwitchToMotor[index], gpio_get_level(_etor[index])?LOW_LEVEL:HIGH_LEVEL); // SW logic is inverted
                }
                else 
                {
                    Powerstep01_SetSwitchLevel(_limitSwitchToMotor[index], gpio_get_level(_etor[index])); // SW logic is not inverted
                }
            #endif
            }
            if(_limitSwitchToNotify[index])
            {
                BusTWAI.sendMessage(OIMessage(CMD_ETOR_INTERRUPT, stepper->getId(), static_cast<uint16_t>(index)), BROADCAST_ID);
            }
        }
    }
}

#endif /* (defined CONFIG_OI_STEPPER) || (defined CONFIG_OI_STEPPER_VERTICAL) */