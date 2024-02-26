/*
    This is an example code for the OICloud library using 4G.
    About: https://openindus.com/
    Firmware Doc: https://openindus.com/oi-content/doc/index.html
*/

#include "Arduino.h"
#include "Netif_LTE.h"
#include "Cloud.hpp"

#define MOBILE_APN "TM"   // Change this according to your operator
#define SIM_PIN_CODE "0000"         // SIM card PIN code

#define Machine_UUID "your-uuid-here"
#define FirmwareVersion "Version 1.0"

Cloud cloud(Machine_UUID, FirmwareVersion);
VarInt Uptime("Uptime", false);
VarInt Downtime("Downtime", false);
VarString IP("IP", false);
VarBool Restart("Restart", true);
VarInt Memory("Memory Usage", false);

// Event network connected
static void network_connected(const Netif_t *netif)
{
    if (netif->got_ip == false)
        return;
    if (cloud.isInit() == false)
        cloud.init();
    if (cloud.isPaused() == true)
        cloud.resume();

    char buffer[16] = {'\0'};
    snprintf(buffer, 16, "%d.%d.%d.%d", (int) netif->ip[0], (int) netif->ip[1], (int) netif->ip[2], (int) netif->ip[3]);
    IP = buffer;
}

// Event network disconnected
static void network_disconnected(const Netif_t *netif)
{
    if (cloud.isPaused() == false)
        cloud.pause();
}

// Callback for the restart variable
void restart_clbk(VarBool *var, void *data)
{
    if (var->get() == true)
        esp_restart();
}

void setup()
{
    cloud.addVar(&Uptime);
    cloud.addVar(&IP);
    cloud.addVar(&Restart);
    cloud.addVar(&Memory);
    cloud.addVar(&Downtime);

    Restart.setWriteCallback(restart_clbk, NULL);
    lte_connect(MOBILE_APN, SIM_PIN_CODE, network_connected, network_disconnected, NULL);
}

void loop()
{
    if (cloud.isConnected() == true) {
        Uptime += 1;
    } else {
        Downtime += 1;
    }
    Memory = 320000 - esp_get_free_heap_size();
    delay(1000);
    cloud.update();
}
