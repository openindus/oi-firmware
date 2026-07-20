#include "OpenIndus.h"
#include "Arduino.h"
#include "WiFi.h"
#include "credentials.h"

Core core;

/* Platform credentials (from your OpenIndus cloud platform) */
#define PLATFORM_UUID   "ddd3707a-5bc8-49a3-8063-453c1ce624e8"
#define PLATFORM_TOKEN  "268w_4sBMpjppTwwrOAJvye1mGbH02SZRVIcYfPDm_U"
#define PROJECT_ID      2
#define PROJECT_VERSION "1.0.0"

/* The cloud handles provisioning (device uuid/token) automatically and
 * persists the credentials in NVS. */
OICloud cloud(PLATFORM_UUID, PLATFORM_TOKEN, PROJECT_ID, PROJECT_VERSION);

/* Cloud variables: each maps to an MQTT topic. */
IntVariable  counter("counter", 0, UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH, 0, 0);
BoolVariable buttonOn("buttonon", false, UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH, 1000, 10000);
FloatVariable setpoint("setpoint", 0.0f, UpdateMethod::ASYNCHRONOUS, UpdateType::SUBSCRIBE);
IntVariable  heapSize("heapsize", 0, UpdateMethod::SYNCHRONOUS, UpdateType::PUBLISH, 2000);
IntVariable  rssi("rssi", 0, UpdateMethod::SYNCHRONOUS, UpdateType::PUBLISH, 2000);

int i = 0;

void setup(void)
{
    printf("Hello OpenIndus!\n");
    /* 1. Bring up network connectivity */
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true); // re-associate automatically if the AP link drops
    WiFi.begin(ssid, password);
    printf("Connecting to WiFi \"%s\"\n", ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    printf("\nWiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());


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
    cloud.addVariable(&heapSize);
    cloud.begin();
}

void loop(void)
{
    /* Just update the values; the cloud task publishes them according to
     * each variable's refresh policy. */
    i++;
    rssi.setValue(WiFi.RSSI());
    counter.setValue(i);
    buttonOn.setValue((i % 2) == 0);
    heapSize.setValue((int)ESP.getFreeHeap());
    delay(1000);
}
