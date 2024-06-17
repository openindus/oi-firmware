#include "OpenIndus.h"
#include "Arduino.h"
#include "RelayHP.h"

static const char TAG[] = "Main";

OICore core;
OIRelayHP relay;
static int test_oi_gpio(Controller *slave)
{
    int ret = 0;
    // test OI_GPIO  
    core.writeSync(0);
    int8_t state = slave->readSync();
    if (state ==0){
        ESP_LOGI(TAG, "OI_GPIO state: %d", state); 
    }else{
        ESP_LOGE(TAG, "OI_GPIO state: %d", state); 
        ret = -1;
    }
    core.writeSync(1);
    state = slave->readSync();
    if (state ==1){
        ESP_LOGI(TAG, "OI_GPIO state: %d", state);
    }else{
        ESP_LOGE(TAG, "OI_GPIO state: %d", state); 
        ret = -1;
    }

    ESP_LOGW(TAG, "OI_ID: %d", slave->getId());

    return ret;
}

void setup(void)
{
}

void loop(void)
{
    test_oi_gpio(&relay);
    delay(1000);
}