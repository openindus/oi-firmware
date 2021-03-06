#include "OpenIndus.h"

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "esp_task_wdt.h"


#if defined CONFIG_OI_CORE
OICore Core;
#elif (defined CONFIG_OI_DISCRETE) || (defined CONFIG_OI_DISCRETE_VERTICAL)
OIDiscrete Discrete;
#elif (defined CONFIG_OI_STEPPER) || (defined CONFIG_OI_STEPPER_VERTICAL)
OIStepper Stepper;
#elif defined CONFIG_OI_MIXED
OIMixed Mixed;
#elif (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP)
OIRelay Relay;
#endif

void attachFunctions(void);

TaskHandle_t mainTaskHandle = NULL;

void mainTask(void *pvParameters)
{
    setup();
    while(1)
    {
        loop();
    }
}

extern "C" void app_main()
{
#ifdef ARDUINO
    initArduino();
#endif

    System.setModule((OIModuleMaster*)OIModule::getInstance(0));

    for (int i=1; i<OIModule::getNbInstance(); i++)
    {
        System.setSubModule((OIModuleSlave*)OIModule::getInstance(i));
    }

    System.init();

#ifndef CONFIG_MODULE_STANDALONE
    attachFunctions();
#endif

    System.start();
    Console.start();

    xTaskCreate(mainTask, "mainTask", 8192, NULL, 1, &mainTaskHandle);
}



void attachFunctions(void)
{
    CMD.add(OIMessage(CMD_SYSTEM, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        if ((msg.getConf()) == 0) {
            System.stop();
        }
        else if ((msg.getConf()) == 1) {
            System.start();
        }
        return 0;
    });

    CMD.add(OIMessage(CMD_DOWNLOAD_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        System.downloadMode(); 
        return 0;
    });

    CMD.add(OIMessage(CMD_SEND_OI_ID, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        System.setOiId(msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_PING, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        return 0xAA;
    });

    CMD.add(OIMessage(CMD_GET_TYPE, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        return Module.getType();
    });

    CMD.add(OIMessage(CMD_GET_TYPE_ALL, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        int type = Module.getType();
        BusTWAI.sendMessage(OIMessage(CMD_GET_TYPE_ALL, Module.getId(), 0, type), msg.getId());
        return type;
    });
    
    CMD.add(OIMessage(CMD_RESTART, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        Module.restart();
        return 0;
    });

    CMD.add(OIMessage(CMD_LED, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        if (msg.getConf(1) == LED_ACTION_ON)
        {
            Module.ledOn((LedColor_t)msg.getConf(0));
        }
        else if (msg.getConf(1) == LED_ACTION_OFF)
        {
            Module.ledOff();
        }
        else if (msg.getConf(1) == LED_ACTION_BLINK)
        {
            Module.ledBlink((LedColor_t)msg.getConf(0), msg.getData()); 
        }
        return 0;
    });

    CMD.add(OIMessage(CMD_INTERNAL_TEMP, Module.getId()), [](OIMessage msg) -> uint32_t
    {
        float temp = Module.getInternalTemp();
        uint32_t tempReturn = 0;
        memcpy(&tempReturn, &temp, sizeof(float));
        return tempReturn;
    });

    CMD.add(OIMessage(CMD_GET_ALL_OI_ID, Module.getId()), [](OIMessage msg) -> uint32_t
    {
        uint32_t oiId = Module.getOiId();
        BusTWAI.sendMessage(OIMessage(CMD_SEND_OI_ID, Module.getId(), 0, oiId), msg.getId());
        return oiId;
    });

    CMD.add(OIMessage(CMD_SET_AUTO_ID, Module.getId()), [](OIMessage msg) -> uint32_t {
        printf("OI-ID: %d \t Module Id: %d\n", msg.getData(), msg.getConf());
        if(msg.getData() == Module.getOiId()) 
        {
            Module.setHardwareId(msg.getConf());
        }
        return 0;
    });

#if defined CONFIG_OI_CORE

    CMD.remove(OIMessage(CMD_GET_TYPE_ALL, Module.getId()));

    CMD.add(OIMessage(CMD_GET_TYPE_ALL, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        printf("Board id=[%u], type=[%u]\n", msg.getId(), msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_DIGITAL_WRITE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.digitalWrite((Stor_t)msg.getConf(), (uint8_t)msg.getData());
        return 0;
    });
    
    CMD.add(OIMessage(CMD_DIGITAL_READ, Module.getId()),   [](OIMessage msg) -> uint32_t 
    { 
        int getValue = Module.digitalRead((Etor_t)msg.getConf());
        return getValue;
    });
    
    CMD.add(OIMessage(CMD_ANALOG_READ, Module.getId()),   [](OIMessage msg) -> uint32_t 
    { 
        float getValue = Module.analogRead((Eana_t)msg.getConf());
        uint32_t castedValue = reinterpret_cast<uint32_t &>(getValue);
        return castedValue;
    });
  
    CMD.add(OIMessage(CMD_ATTACH_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Etor_t etor = (Etor_t)msg.getConf();
        Module.attachInterrupt(etor, Module.handleInterrupt, &etor, (InterruptMode_t)msg.getData()); 
        return 0;
    });
    
    CMD.add(OIMessage(CMD_DETACH_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.detachInterrupt((Etor_t)msg.getConf(), (InterruptMode_t)msg.getData()); 
        return 0;
    });

#elif (defined CONFIG_OI_DISCRETE) || (defined CONFIG_OI_DISCRETE_VERTICAL)

    CMD.add(OIMessage(CMD_DIGITAL_WRITE, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        Module.digitalWrite((Stor_t)msg.getConf(), (uint8_t)msg.getData()); return 0;
    });

    CMD.add(OIMessage(CMD_ANALOG_WRITE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.analogWrite((Stor_t)msg.getConf(), (uint8_t)msg.getData()); return 0;
    });

    CMD.add(OIMessage(CMD_DIGITAL_READ, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return Module.digitalRead((Etor_t)msg.getConf());
    });

    CMD.add(OIMessage(CMD_ANALOG_READ, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        float getValue = Module.analogRead((Eana_t)msg.getConf(0), (Units_t)msg.getConf(1));
        uint32_t valueReturn = 0;
        memcpy(&valueReturn, &getValue, sizeof(float));
        return valueReturn;
    });

    CMD.add(OIMessage(CMD_STOR_SENSOR_READ, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        float getValue = Module.getStorCurrent((Stor_t)msg.getConf());
        uint32_t valueReturn = 0;
        memcpy(&valueReturn, &getValue, sizeof(float));
        return valueReturn;
    });

    CMD.add(OIMessage(CMD_ATTACH_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.attachInterrupt((Etor_t)msg.getConf(), Module.handleInterrupt, (InterruptMode_t)msg.getData()); 
        return 0;
    });

    CMD.add(OIMessage(CMD_DETACH_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.detachInterrupt((Etor_t)msg.getConf(), (InterruptMode_t)msg.getData()); 
        return 0;
    });

#elif (defined CONFIG_OI_STEPPER) || (defined CONFIG_OI_STEPPER_VERTICAL)

    CMD.add(OIMessage(CMD_GET_ETOR_LEVEL, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return Module.getEtorLevel(static_cast<Etor_t>(msg.getConf()));
    });

    CMD.add(OIMessage(CMD_SET_LOGIC_SWITCH, Module.getId()), [](OIMessage msg) -> uint32_t { 
        Module.setSwitchLogic(static_cast<bool>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_ATTACH_LIMIT_SWITCH, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.attachLimitSwitch(static_cast<Etor_t>(msg.getConf(1)), static_cast<Motor_t>(msg.getConf(0)), static_cast<bool>(msg.getData()));
        return 0;
    });

    CMD.add(OIMessage(CMD_DETACH_LIMIT_SWITCH, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.detachLimitSwitch(static_cast<Etor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_ATTACH_BUSY_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.attachBusyInterrupt([](void) {
            BusTWAI.sendMessage(OIMessage(CMD_BUSY_INTERRUPT, Module.getId()), BROADCAST_ID);
        }); 
        return 0;
    });

    CMD.add(OIMessage(CMD_ATTACH_ERROR_HANDLER, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        // Module.attachErrorHandler([](uint32_t) { 
        //     BusTWAI.sendMessage(OIMessage(CMD_ERROR_HANDLER, Module.getId()), BROADCAST_ID);
        // }); 
        return 0;
    });

    CMD.add(OIMessage(CMD_ATTACH_FLAG_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.attachFlagInterrupt([](void) { 
            BusTWAI.sendMessage(OIMessage(CMD_FLAG_INTERRUPT, Module.getId()), BROADCAST_ID);
        }); 
        return 0;
    });

    CMD.add(OIMessage(CMD_GET_STATUS, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return Module.getStatus(static_cast<Motor_t>(msg.getConf()));
    });

    CMD.add(OIMessage(CMD_GO_HOME, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.goHome(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_GO_MARK, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.goMark(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_GO_TO, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.goTo(static_cast<Motor_t>(msg.getConf()), 
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_GO_TO_DIR, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.goToDir(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorDir_t>((msg.getConf() & 0xFF00) >> 8), 
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_GO_UNTIL, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.goUntil(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorAction_t>((msg.getConf() & 0xF000) >> 12), 
            static_cast<motorDir_t>((msg.getConf() & 0x0F00) >> 8), 
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_HARD_HIZ, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.hardHiZ(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_HARD_STOP, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.hardStop(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_MOVE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.move(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorDir_t>((msg.getConf() & 0xFF00) >> 8), 
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_RELEASE_SW, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.releaseSw(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorAction_t>((msg.getConf() & 0xF000) >> 12), 
            static_cast<motorDir_t>((msg.getConf() & 0x0F00) >> 8));
        return 0;
    });

    CMD.add(OIMessage(CMD_RESET_DEVICE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.resetDevice(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_RESET_POS, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.resetPos(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_RUN, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.run(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorDir_t>((msg.getConf() & 0xFF00) >> 8), 
            static_cast<float>(msg.getData()));
        return 0;
    });

    CMD.add(OIMessage(CMD_SOFT_HIZ, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.softHiZ(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });


    CMD.add(OIMessage(CMD_SOFT_STOP, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.softStop(static_cast<Motor_t>(msg.getConf()));
        return 0;
    });

    CMD.add(OIMessage(CMD_STEP_CLOCK, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.stepClock(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorDir_t>((msg.getConf() & 0xFF00) >> 8), 
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_GET_MARK, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return Module.getMark(static_cast<Motor_t>(msg.getConf()));
    });

    CMD.add(OIMessage(CMD_GET_POSITION, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return Module.getPosition(static_cast<Motor_t>(msg.getConf()));
    });

    CMD.add(OIMessage(CMD_BUSY_CHECK, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return static_cast<uint32_t>(Module.busyCheck(static_cast<Motor_t>(msg.getConf())));
    });;

    CMD.add(OIMessage(CMD_SELECT_STEP_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return static_cast<uint32_t>(Module.selectStepMode(static_cast<Motor_t>(msg.getConf()), 
            static_cast<motorStepMode_t>((msg.getConf() & 0xFF00) >> 8)));
    });

    CMD.add(OIMessage(CMD_SET_HOME, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.setHome(static_cast<Motor_t>(msg.getConf()),
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_SET_MARK, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.setMark(static_cast<Motor_t>(msg.getConf()),
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_SET_PARAM, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.setParam(static_cast<Motor_t>(msg.getConf()), 
            ((msg.getConf() & 0xFF00) >> 8), 
            msg.getData());
        return 0;
    });

    CMD.add(OIMessage(CMD_GET_PARAM, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        return Module.getParam(static_cast<Motor_t>(msg.getConf()), 
            ((msg.getConf() & 0xFF00) >> 8));
    });

#elif defined CONFIG_OI_MIXED

    CMD.add(OIMessage(CMD_DIGITAL_WRITE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.digitalWrite((Stor_t)msg.getConf(), (uint8_t)msg.getData()); return 0;
    });

    CMD.add(OIMessage(CMD_DIGITAL_READ, Module.getId()),   [](OIMessage msg) -> uint32_t 
    {
        return Module.digitalRead((Etor_t)msg.getConf(0));
    });

    CMD.add(OIMessage(CMD_ANALOG_WRITE, Module.getId()),  [](OIMessage msg) -> uint32_t 
    { 
        Module.analogWrite((Stor_t)msg.getConf(), (uint8_t)msg.getData()); return 0;
    });

    CMD.add(OIMessage(CMD_ATTACH_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.attachInterrupt((Etor_t)msg.getConf(0), Module.handleInterrupt, (InterruptMode_t)msg.getData()); 
        return 0;
    });

    CMD.add(OIMessage(CMD_DETACH_INTERRUPT, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.detachInterrupt((Etor_t)msg.getConf(0), (InterruptMode_t)msg.getData()); 
        return 0;
    });
   
    CMD.add(OIMessage(CMD_STOR_SENSOR_READ, Module.getId()),   [](OIMessage msg) -> uint32_t 
    { 
        float getValue = Module.getStorCurrent((Stor_t)msg.getConf(0));
        uint32_t castedValue = reinterpret_cast<uint32_t &>(getValue);
        return castedValue;
    });

    CMD.add(OIMessage(CMD_VOLTAGE_WRITE_DAC, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        uint32_t data32 = msg.getData();
        float* data = reinterpret_cast<float*>(&data32);
        Module.voltageWrite((Sana_t)msg.getConf(0), *data, (Units_t)msg.getConf(1));
        return 0;
    });

    CMD.add(OIMessage(CMD_ANALOG_READ_ADC, Module.getId()),   [](OIMessage msg) -> uint32_t 
    { 
        float getValue = Module.analogRead((Eana_t)msg.getConf(0), (Units_t)msg.getConf(1));
        uint32_t castedValue = reinterpret_cast<uint32_t &>(getValue);
        return castedValue;
    });

    CMD.add(OIMessage(CMD_GET_ANALOG_ADC_REFERENCE, Module.getId()),   [](OIMessage msg) -> uint32_t 
    { 
        float getValue = Module.getAnalogReference((Units_t)msg.getConf(1));
        uint32_t castedValue = reinterpret_cast<uint32_t &>(getValue);
        return castedValue;
    });

    CMD.add(OIMessage(CMD_SET_ANALOG_ADC_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.analogReadMode((Eana_t)msg.getConf(0), (AdcMode_t)msg.getData(0));
        return 0;
    });

    CMD.add(OIMessage(CMD_SET_ANALOG_ADC_RESOLUTION, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.analogReadResolution((AdcRes_t)msg.getConf(0));
        return 0;
    });

    CMD.add(OIMessage(CMD_SET_ANALOG_DAC_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.setAnalogMode((Sana_t)msg.getConf(0), (DACAnalogMode_t)msg.getData(0));
        return 0;
    });

    CMD.add(OIMessage(CMD_GET_ANALOG_DAC_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        DACAnalogMode_t mode = Module.getAnalogMode((Sana_t)msg.getConf(0));
        return reinterpret_cast<uint32_t &>(mode);
    });

    CMD.add(OIMessage(CMD_SET_CURRENT_DAC_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        Module.setCurrentMode((Sana_t)msg.getConf(0), (DACCurrentMode_t)msg.getData(0));
        return 0;
    });

    CMD.add(OIMessage(CMD_GET_CURRENT_DAC_MODE, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        DACCurrentMode_t mode = Module.getCurrentMode((Sana_t)msg.getConf(0));
        return reinterpret_cast<uint32_t &>(mode);
    });

    CMD.add(OIMessage(CMD_CURRENT_WRITE_DAC, Module.getId()), [](OIMessage msg) -> uint32_t 
    { 
        uint32_t data32 = msg.getData();
        float* data = reinterpret_cast<float*>(&data32);
        Module.currentWrite((Sana_t)msg.getConf(0), *data, (Units_t)msg.getConf(1));
        return 0;
    });

#elif (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP)

    CMD.add(OIMessage(CMD_SET_RELAY, Module.getId()), [](OIMessage msg) -> uint32_t 
    {
        Module.setOutput((Relay_t)msg.getConf(), (uint8_t)msg.getData()); return 0;
    });

#endif
}