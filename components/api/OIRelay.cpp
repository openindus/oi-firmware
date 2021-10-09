#include "OIRelay.h"

#if (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP)

static const char OI_RELAY_TAG[] = "OIRelay";

#if (defined CONFIG_OI_RELAY_LP)
const gpio_num_t relayLpTable[6] = {OI_RELAY_LP_PHASE_CMD1, OI_RELAY_LP_PHASE_CMD2, OI_RELAY_LP_PHASE_CMD3, OI_RELAY_LP_PHASE_CMD4, OI_RELAY_LP_PHASE_CMD5,OI_RELAY_LP_PHASE_CMD6};
#else
const gpio_num_t relayHpTable[4] = {OI_RELAY_HP_PHASE_CMD1, OI_RELAY_HP_PHASE_CMD2, OI_RELAY_HP_PHASE_CMD3, OI_RELAY_HP_PHASE_CMD4};
#endif

void OIRelay::init()
{
    #ifdef CONFIG_OI_RELAY_HP
    _type = OI_RELAY_HP;
    #else
    _type = OI_RELAY_LP;
    #endif

    OIModuleMaster::init();

    /* Init of ana pins */
    ESP_LOGI(OI_RELAY_TAG, "initializing RELAYS");
    _initRelays();

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<OIRELAY_PIN_CMD_MOSFET);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_set_level(OIRELAY_PIN_CMD_MOSFET, LOW_LEVEL);
    gpio_install_isr_service(0);
}

void OIRelay::setOutput(Relay_t relay, uint8_t on_off) 
{
    #if defined CONFIG_OI_RELAY_LP
        gpio_set_level(relayLpTable[relay], on_off);
    #else
        gpio_set_level(relayHpTable[relay], on_off);
    #endif
}

/************************************************************************************************************/
/*----------------------------------------- PRIVATE FUCNTIONS ----------------------------------------------*/
/************************************************************************************************************/

void OIRelay::_initRelays(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = 0;

    #if defined CONFIG_OI_RELAY_LP
    uint8_t relay_nb = RELAY6;
    #else 
    uint8_t relay_nb = RELAY4;
    #endif

    for(int i = 0; i <= relay_nb; i++)
    {
        #if defined CONFIG_OI_RELAY_LP
        io_conf.pin_bit_mask |= (1ULL<<relayLpTable[i]);
        #else
        io_conf.pin_bit_mask |= (1ULL<<relayHpTable[i]);
        #endif
    }

    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

}

#endif /* (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP) */ 