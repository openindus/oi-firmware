#include "OISystem.h"

#include "esp_ota_ops.h"
#include "esp_partition.h"

static const char OI_SYSTEM_TAG[] = "OISystem";

TaskHandle_t OISystem::_busRsTaskHandle;
TaskHandle_t OISystem::_busCanTaskHandle;

void OISystem::init()
{
    /* initalize the bus */
    ESP_LOGI(OI_SYSTEM_TAG, "initalize the bus");
    BusRS.init();
    BusTWAI.init();

    /* initalize the  module */
    ESP_LOGI(OI_SYSTEM_TAG, "initalize the module 0x%02x", _module->getId());
    _module->init();
}

void OISystem::start()
{
    ESP_LOGI(OI_SYSTEM_TAG, "start the system");

    /* create task control */
    #ifndef DISABLE_RS_TASK
    xTaskCreate(_busRsTask, "bus task to receive rs message", 4096, this, 1, &_busRsTaskHandle);
    #endif
    #ifndef DISABLE_CAN_TASK
    xTaskCreate(_busCanTask, "bus task to receive can message", 4096, this, 1, &_busCanTaskHandle);
    #endif

    #if defined(CONFIG_OI_CORE) & defined(CONFIG_AUTO_ID)

    automaticId();
    
    for (auto it=_submodules.begin(); it!=_submodules.end(); ++it)
    {        
        it->second->ledBlink(LED_GREEN, 1000);
    }
    _module->ledBlink(LED_GREEN, 1000);

    #endif
}

void OISystem::stop(void)
{
    /* deinitalize the bus */
    ESP_LOGI(OI_SYSTEM_TAG, "stop the system");
    
    assert(_busRsTaskHandle != NULL);
    vTaskDelete(_busRsTaskHandle);
    assert(_busCanTaskHandle != NULL);
    vTaskDelete(_busCanTaskHandle);

    for (auto it=_submodules.begin(); it!=_submodules.end(); ++it)
    {        
        it->second->ledOn(LED_GREEN);
    }
    _module->ledOn(LED_GREEN);
}

void OISystem::status(void)
{
    /**
     * @todo afficher l'état du système
     * 
     */
}

void OISystem::setModule(OIModuleMaster* module)
{
    assert(module != NULL);
    _module = module;
}

OIModuleMaster* OISystem::getModule(void)
{
    return _module;
}

void OISystem::setSubModule(OIModuleSlave* module)
{
    uint8_t id = module->getDestId();

    assert(module != NULL);
    /* add submodule in the table */
    auto it = _submodules.insert(std::map<uint8_t, OIModuleSlave*>::value_type(id, module));
    if (!it.second)
    {
        ESP_LOGW(OI_SYSTEM_TAG, "submodule 0x%02x already exists",id);
    }
    else
    {
        ESP_LOGV(OI_SYSTEM_TAG, "submodule 0x%02x inserted", id);
    }
}

OIModuleSlave* OISystem::getSubModule(uint8_t id)
{
    /* search submodule in the table */
    if (_submodules.find(id) != _submodules.end())
    {
        return _submodules.at(id);
    }
    else
    {
        ESP_LOGW(OI_SYSTEM_TAG, "module 0x%02x not found in the table", id);
        return NULL;
    }
}

void OISystem::_busRsTask(void *pvParameters)
{
    OISystem* system = (OISystem*)pvParameters;
    assert(system != NULL);

    OIMessage msg;
    uint8_t id;
    uint32_t data;

    ESP_LOGV(OI_SYSTEM_TAG, "run rs task bus");
    
    while (1)
    {
        if (BusRS.receiveMessage(msg, id) != -1)
        {
            assert(system->getModule() != NULL);

            if (id == (system->getModule())->getId() || id == BROADCAST_ID)
            {
                if ((msg.getType() & MASK_SET) == TYPE_SET)
                {
                    ESP_LOGV(OI_SYSTEM_TAG, "received set message");

                    if (id != BROADCAST_ID)
                    {
                        BusRS.sendMessage(OIMessage(msg.getType(), (system->getModule())->getId(), msg.getConf()), msg.getId());
                    }
                    Fct.run(msg);
                }
                else if ((msg.getType() & MASK_GET) == TYPE_GET)
                {
                    ESP_LOGV(OI_SYSTEM_TAG, "received get message");

                    /* run the command */
                    data = Fct.run(msg);

                    if (id != BROADCAST_ID)
                    {
                        id = msg.getId();
                        msg.setId((system->getModule())->getId());
                        msg.setData(data);
                        BusRS.sendMessage(msg, id);
                    }
                }
                else if ((msg.getType() & MASK_SEND) == TYPE_SEND)
                {
                    ESP_LOGV(OI_SYSTEM_TAG, "received send message");
                    Fct.run(msg);
                }
            }
        }
    }
}

void OISystem::_busCanTask(void *pvParameters)
{
    OISystem* system = (OISystem*)pvParameters;
    assert(system != NULL);

    OIMessage msg;
    uint8_t id;

    ESP_LOGV(OI_SYSTEM_TAG, "run can task bus");
    
    while (1)
    {
        if (BusTWAI.receiveMessage(msg, id) != -1)
        { 
            if (id == (system->getModule())->getId() || id == BROADCAST_ID)
            {
                if ((msg.getType() & MASK_SEND) == TYPE_SEND)
                {
                    Fct.run(msg);
                }
                else
                {
                    ESP_LOGW(OI_SYSTEM_TAG, "unknow message");
                }
            }
        }
    }
}

void OISystem::downloadMode()
{
    // Set update partition for next boot
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    assert(update_partition != NULL);
    esp_ota_set_boot_partition(update_partition);
    esp_restart();
}

void OISystem::automaticId(void)
{
    /* Get all OI-ID */
    BusRS.sendMessage(OIMessage(CMD_GET_ALL_OI_ID, 0), 255);
    vTaskDelay(100/portTICK_PERIOD_MS);

    /* OI-ID sorting */
    sort(_oiIdList.begin(), _oiIdList.end());

    if (_oiIdList.size() == _submodules.size())
    {
        int i = _submodules.size();
        for (auto it=_submodules.begin(); it!=_submodules.end(); ++it)
        {    
            i--;
            BusRS.setMessage(OIMessage(CMD_SET_AUTO_ID, 0, (uint8_t)it->first, _oiIdList[i]), 255);
        }
    }
    else
    {
        ESP_LOGW(OI_SYSTEM_TAG, "the number of modules does not match");
    }

    /* Set IDs */
    _module->setHardwareId(0);
    for (auto it=_submodules.begin(); it!=_submodules.end(); ++it)
    {        
        it->second->setSenderId(_module->getHardwareId());
    }
}

OISystem System;