#include "drv8873.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "DRV8873_SPI";

// Global configuration instance (defined here, declared in .h)
drv8873_spi_config_t *drv8873_global_config = NULL;

esp_err_t drv8873_spi_init(drv8873_spi_config_t *config) {
    esp_err_t ret;

    // Configure le device SPI (DRV8873)
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = 1 * 1000 * 1000,  // 1 MHz (ajustable selon les besoins)
        .mode = 0,                          // SPI Mode 0 (CPOL=0, CPHA=0)
        .spics_io_num = config->nSCS_pin,   // Broche CS
        .queue_size = 7,                    // Taille de la file d'attente
    };

    ret = spi_bus_add_device(SPI2_HOST, &dev_config, &config->spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    // Store the config globally for future use
    drv8873_global_config = config;

    ESP_LOGI(TAG, "SPI initialized successfully");
    return ESP_OK;
}

esp_err_t drv8873_spi_read_register(drv8873_register_t reg_address, uint16_t *reg_value) {
    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t command = (1 << 14) | (reg_address << 9);  // Bit 14=1 (Read), bits 13-9=Adresse
    uint16_t dummy_data = 0x0000;  // Données fictives pour la lecture

    spi_transaction_t trans = {
        .length = 16 * 2,  // 16 bits pour la commande + 16 bits pour la réponse
        .tx_buffer = &command,
        .rx_buffer = reg_value,
    };

    esp_err_t ret = spi_device_transmit(drv8873_global_config->spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read register 0x%02X: %s", reg_address, esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t drv8873_spi_write_register(drv8873_register_t reg_address, uint16_t reg_value) {
    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t command = (0 << 14) | (reg_address << 9) | reg_value;  // Bit 14=0 (Write), bits 13-9=Adresse, bits 8-0=Données

    spi_transaction_t trans = {
        .length = 16,  // 16 bits pour la commande
        .tx_buffer = &command,
    };

    esp_err_t ret = spi_device_transmit(drv8873_global_config->spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register 0x%02X: %s", reg_address, esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t drv8873_set_mode(drv8873_mode_t mode) {
    uint16_t reg_value;
    esp_err_t ret;

    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Lit le registre IC1 Control
    ret = drv8873_spi_read_register(DRV8873_REG_IC1_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Met à jour le champ MODE (bits 1-0)
    reg_value = (reg_value & 0xFFFC) | (mode & 0x0003);

    // Écrit le registre mis à jour
    ret = drv8873_spi_write_register(DRV8873_REG_IC1_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Mode set to 0x%02X", mode);
    return ESP_OK;
}

esp_err_t drv8873_set_current_limit(drv8873_itrip_level_t itrip_level) {
    uint16_t reg_value;
    esp_err_t ret;

    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Lit le registre IC4 Control
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Met à jour le champ ITRIP_LVL (bits 3-2)
    reg_value = (reg_value & 0xFFC3) | ((itrip_level & 0x0003) << 2);

    // Écrit le registre mis à jour
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Current limit set to 0x%02X", itrip_level);
    return ESP_OK;
}

esp_err_t drv8873_set_current_regulation(drv8873_current_regulation_t disable_flags) {
    uint16_t reg_value;
    esp_err_t ret;

    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Lit le registre IC4 Control
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Met à jour le champ DIS_ITRIP (bits 1-0)
    reg_value = (reg_value & 0xFFFC) | (disable_flags & 0x0003);

    // Écrit le registre mis à jour
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Current regulation disabled for OUT1/OUT2: 0x%02X", disable_flags);
    return ESP_OK;
}

esp_err_t drv8873_get_fault_status(uint16_t *fault_status) {
    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    return drv8873_spi_read_register(DRV8873_REG_FAULT_STATUS, fault_status);
}
