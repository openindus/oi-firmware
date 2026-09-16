#include "OpenIndus.h"
#include "Arduino.h"

Core core;

/* Platform credentials (from your OpenIndus cloud platform) */
#define PLATFORM_UUID   "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define PLATFORM_TOKEN  "your-platform-token"
#define PROJECT_ID      1
#define PROJECT_VERSION "1.0.0"

/* The cloud handles provisioning (device uuid/token) automatically and
 * persists the credentials in NVS. */
OICloud cloud(PLATFORM_UUID, PLATFORM_TOKEN, PROJECT_ID, PROJECT_VERSION);

/* Cloud variables: each maps to an MQTT topic. */
IntVariable  counter("counter", 0, UpdateMethod::SYNCHRONOUS, UpdateType::PUBLISH, 1000);
BoolVariable buttonOn("buttonon", false, UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH, 1000, 10000);
FloatVariable setpoint("setpoint", 0.0f, UpdateMethod::ASYNCHRONOUS, UpdateType::SUBSCRIBE);

int i = 0;

void setup(void)
{
    printf("Hello OpenIndus!\n");

    /* 1. Bring up network connectivity (cellular / PPP) */
    core.modem = new Modem();
    core.modem->begin("TM"); // APN of your SIM provider
    core.modem->connect();

    /* 2. React to values pushed from the cloud */
    setpoint.onReceive([](const float &value) {
        printf("New setpoint received from cloud: %f\n", value);
    });

    /* 3. Register the variables and start the cloud client.
     *    begin() returns immediately; provisioning and the MQTT connection
     *    run in a background task. */
    cloud.addVariable(&counter);
    cloud.addVariable(&buttonOn);
    cloud.addVariable(&setpoint);
    cloud.begin();
}

void loop(void)
{
    /* Just update the values; the cloud task publishes them according to
     * each variable's refresh policy. */
    i++;
    counter.setValue(i);
    buttonOn.setValue((i % 2) == 0);

    delay(1000);
}
