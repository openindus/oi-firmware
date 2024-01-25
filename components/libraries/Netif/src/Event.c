#include "Event_Private.h"
#include "esp_event.h"
#include "esp_log.h"

//Wrapper function (task was created)
static void call_connected(void *data)
{
    Netif_t *netif = (Netif_t *) data;
    ESP_LOGD("Netif", "Event connected");
    netif->eventConnected(netif);
    vTaskDelete(NULL);
}

//Wrapper function (task was created)
static void call_disconnected(void *data)
{
    Netif_t *netif = (Netif_t *) data;
    ESP_LOGD("Netif", "Event disconnected");
    netif->eventDisconnected(netif);
    vTaskDelete(NULL);
}

/// @brief Launches the connected callback in a task
/// @param netif Netif_t*
void Netif_task_Connected(Netif_t *netif)
{
    if (netif == NULL || netif->eventConnected == NULL)
        return;
    xTaskCreate(call_connected, "OINetif_connected", Task_StackDepth, netif, Task_Priority, NULL);
}

/// @brief Launches the disconnected callback in a task
/// @param netif Netif_t*
void Netif_task_Disconnected(Netif_t *netif)
{
    if (netif == NULL || netif->eventDisconnected == NULL)
        return;
    xTaskCreate(call_disconnected, "OINetif_disconnected", Task_StackDepth, netif, Task_Priority, NULL);
}
