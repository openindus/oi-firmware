#include "OpenIndus.h"
#include "Arduino.h"
#include "RelayHP.h"

#ifdef MODULE_MASTER

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
    /* Activate all */
    relay.digitalWrite(RELAY_1, HIGH);
    delay(200);
    relay.digitalWrite(RELAY_2, HIGH);
    delay(200);
    relay.digitalWrite(RELAY_3, HIGH);
    delay(200);
    relay.digitalWrite(RELAY_4, HIGH);
    delay(1000);

    /* Deactivate all */
    relay.digitalWrite(RELAY_1, LOW);
    delay(200);
    relay.digitalWrite(RELAY_2, LOW);
    delay(200);
    relay.digitalWrite(RELAY_3, LOW);
    delay(200);
    relay.digitalWrite(RELAY_4, LOW);

    test_oi_gpio(&relay);

    delay(1000);

}
#endif