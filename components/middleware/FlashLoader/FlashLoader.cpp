/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file FlashLoaderOta.cpp
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "FlashLoader.h"

static const char FLASH_LOADER_TAG[] = "FlashLoader";

/* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
static esp_ota_handle_t update_handle = 0;
const esp_partition_t *update_partition = NULL;

void FlashLoader::begin(void)
{
    esp_err_t err; 

    ESP_LOGI(FLASH_LOADER_TAG, "FlashLoader begin");

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running) {
        ESP_LOGW(FLASH_LOADER_TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x",
                 configured->address, running->address);
        ESP_LOGW(FLASH_LOADER_TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    ESP_LOGI(FLASH_LOADER_TAG, "Running partition type %d subtype %d (offset 0x%08x)",
             running->type, running->subtype, running->address);

    update_partition = esp_ota_get_next_update_partition(NULL);
    assert(update_partition != NULL);
    ESP_LOGI(FLASH_LOADER_TAG, "Writing to partition subtype %d at offset 0x%08x",
             update_partition->subtype, update_partition->address);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(FLASH_LOADER_TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
        esp_ota_abort(update_handle);
    }
    ESP_LOGI(FLASH_LOADER_TAG, "esp_ota_begin succeeded");
}

void FlashLoader::write(uint8_t* data, size_t length) 
{
    esp_err_t err = esp_ota_write(update_handle, (const void *)data, length);
    if (err != ESP_OK) {
        ESP_LOGE(FLASH_LOADER_TAG, "esp_ota_write failed (%s)", esp_err_to_name(err));
        return;
    }
}

void FlashLoader::check(uint8_t md5Sum[16], size_t progSize)
{
    esp_err_t err;
    uint8_t* buffer = NULL;
    uint32_t addr = 0;
    MD5Context context;

    ESP_LOGI(FLASH_LOADER_TAG, "FlashLoader check");

    buffer = (uint8_t*)malloc(128);
    assert(update_partition != NULL);

    MD5Init(&context);
    while (progSize > 0) {
        uint32_t n = progSize;
        if (n > 128) 
            n = 128;
        err = esp_partition_read(update_partition, addr, buffer, n);
        if (err != ESP_OK) {
            ESP_LOGE(FLASH_LOADER_TAG, "esp_partition_read failed (%s)", esp_err_to_name(err));
        }
        MD5Update(&context, buffer, n);
        addr += n;
        progSize -= n;
    }
    MD5Final(md5Sum, &context);
}

void FlashLoader::end(void)
{
    esp_err_t err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            ESP_LOGE(FLASH_LOADER_TAG, "Image validation failed, image is corrupted");
        } else {
            ESP_LOGE(FLASH_LOADER_TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
        }
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(FLASH_LOADER_TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
    }
    ESP_LOGI(FLASH_LOADER_TAG, "Prepare to restart system!");
    esp_restart();
}