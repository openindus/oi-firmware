#include "OpenIndus.h"
#include "Arduino.h"
#include "WiFi.h"
#include "credentials.h"

// >>> OI-GENERATED[credentials] - auto-managed, do not edit inside this block
/* Platform credentials (from your OpenIndus cloud platform) */
#define PLATFORM_UUID  "ddd3707a-5bc8-49a3-8063-453c1ce624e8"
#define PLATFORM_TOKEN "268w_4sBMpjppTwwrOAJvye1mGbH02SZRVIcYfPDm_U"
#define PROJECT_ID     10
/* PROJECT_VERSION must stay identical to the version configured for this
 * project on the OpenIndus web platform, otherwise OTA updates cannot be
 * handled correctly. */
#define PROJECT_VERSION "propre"

/* The cloud handles provisioning (device uuid/token) automatically and
 * persists the credentials in NVS. */
OICloud cloud(PLATFORM_UUID, PLATFORM_TOKEN, PROJECT_ID, PROJECT_VERSION);
// <<< OI-GENERATED[credentials]

Core core;


// >>> OI-GENERATED[variables] - auto-managed, do not edit inside this block
/* Cloud variables: each maps to an MQTT topic. */
// <<< OI-GENERATED[variables]

void setup(void)
{
    printf("Hello OpenIndus!\n");

    /* 1. Bring up network connectivity (WiFi) */
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);
    printf("Connecting to WiFi \"%s\"\n", ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

// >>> OI-GENERATED[onreceive] - new callbacks are added here; edit the bodies freely
    /* 2. React to values pushed from the cloud. Fill in these handlers.
     *    On regeneration your callbacks are kept as-is; only variables without
     *    a callback yet get a fresh stub added here. */
// <<< OI-GENERATED[onreceive]

    /* 3. Register the variables and start the cloud client.
     *    begin() returns immediately; provisioning and the MQTT connection
     *    run in a background task. */
// >>> OI-GENERATED[register] - auto-managed, do not edit inside this block
// <<< OI-GENERATED[register]
    cloud.begin();
}

void loop(void)
{
    // Add your loop logic here;
    delay(100);
}
