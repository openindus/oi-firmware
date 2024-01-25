#include "Mac_Private.h"
#include "esp_check.h"

/// @brief Returns the esp mac address
/// @return OIMacAddr_t
OIMacAddr_t getMacAddr()
{
    OIMacAddr_t addr;

    ESP_ERROR_CHECK(esp_efuse_mac_get_default((uint8_t *) addr.base_mac_addr));
    esp_derive_local_mac((uint8_t *) addr.local_mac, (uint8_t *) addr.base_mac_addr);
    return (addr);
}
