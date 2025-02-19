#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_sntp.h"
#include "sdkconfig.h"
#include "Cloud_Private.hpp"

/// @brief Update the system time using NTP. This function
/// refreshes the system time using NTP frequently.
/// It must be run in a task. This function may not work in IDF 5.x.
/// @param data Task Data
void updateSytemTimeNTP(void *data)
{
    time_t t = 0;
    struct tm tm;

    ESP_LOGI(Cloud_LogTag, "Initializing SNTP");
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, CONFIG_CLOUD_NTP_HOSTNAME);
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    esp_sntp_init();
    while (true) {
        ESP_LOGI(Cloud_LogTag, "NTP: Syncing sytem time...");
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
            ESP_LOGD(Cloud_LogTag, "NTP: Waiting for system time to be synced...");
            vTaskDelay(15 * 1000 / portTICK_PERIOD_MS);
        }
        t = time(NULL);
        tm = *localtime(&t);
        ESP_LOGI(Cloud_LogTag, "NTP: Synced system time to: %d-%02d-%02d %02d:%02d:%02d UTC", tm.tm_year + 1900,
                    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        vTaskDelay(CONFIG_CLOUD_NTP_REFRESH_DELAY * 1000 / portTICK_PERIOD_MS);
        sntp_restart();
        ESP_LOGW(Cloud_LogTag, "NTP: Restarted SNTP service");
    }
}
