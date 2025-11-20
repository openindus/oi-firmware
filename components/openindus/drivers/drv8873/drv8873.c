#include "drv8873.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "DRV8873_SPI";

// Global configuration instance (defined here, declared in .h)
drv8873_spi_config_t *drv8873_global_config = NULL;

esp_err_t drv8873_spi_init(drv8873_spi_config_t *config) {
    esp_err_t ret;

    // Configure the SPI device (DRV8873)
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = 1 * 1000 * 1000,  // 1 MHz (adjustable as needed)
        .mode = 0,                          // SPI Mode 0 (CPOL=0, CPHA=0)
        .spics_io_num = config->nSCS_pin,   // Chip Select pin
        .queue_size = 7,                    // Queue size
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

    uint16_t command = (1 << 14) | (reg_address << 9);  // Bit 14=1 (Read), bits 13-9=Register address
    spi_transaction_t trans = {
        .length = 16 * 2,  // 16 bits for command + 16 bits for response
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

    uint16_t command = (0 << 14) | (reg_address << 9) | reg_value;  // Bit 14=0 (Write), bits 13-9=Register address, bits 8-0=Data

    spi_transaction_t trans = {
        .length = 16,  // 16 bits for command
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

    // Read IC1 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC1_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update MODE field (bits 1-0)
    reg_value = (reg_value & 0xFFFC) | (mode & 0x0003);

    // Write updated register
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

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update ITRIP_LVL field (bits 3-2)
    reg_value = (reg_value & 0xFFC3) | ((itrip_level & 0x0003) << 2);

    // Write updated register
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

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update DIS_ITRIP field (bits 1-0)
    reg_value = (reg_value & 0xFFFC) | (disable_flags & 0x0003);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Current regulation for OUT1/OUT2: 0x%02X", disable_flags);
    return ESP_OK;
}

esp_err_t drv8873_set_ocp_mode(drv8873_ocp_mode_t ocp_mode) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update OCP_MODE field (bits 1-0)
    reg_value = (reg_value & 0xFFFC) | (ocp_mode & 0x0003);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "OCP mode set to 0x%02X", ocp_mode);
    return ESP_OK;
}

esp_err_t drv8873_set_tsd_mode(int auto_recovery) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update TSD_MODE bit (bit 6)
    if (auto_recovery) {
        reg_value |= (1 << 6);
    } else {
        reg_value &= ~(1 << 6);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "TSD mode set to %s", auto_recovery ? "auto-recovery" : "latched");
    return ESP_OK;
}

esp_err_t drv8873_set_itrip_rep(int report) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update ITRIP_REP bit (bit 7)
    if (report) {
        reg_value |= (1 << 7);
    } else {
        reg_value &= ~(1 << 7);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "ITRIP report %s", report ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_otw_rep(int report) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update OTW_REP bit (bit 5)
    if (report) {
        reg_value |= (1 << 5);
    } else {
        reg_value &= ~(1 << 5);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "OTW report %s", report ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_dis_cpuv(int disable) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update DIS_CPUV bit (bit 4)
    if (disable) {
        reg_value |= (1 << 4);
    } else {
        reg_value &= ~(1 << 4);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "CPUV %s", disable ? "disabled" : "enabled");
    return ESP_OK;
}

esp_err_t drv8873_set_ocp_tretry(int retry_time) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update OCP_TRETRY field (bits 3-2)
    reg_value = (reg_value & 0xFFF3) | ((retry_time & 0x0003) << 2);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "OCP retry time set to %dms", retry_time * 500);
    return ESP_OK;
}

esp_err_t drv8873_set_en_olp(int enable) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update EN_OLP bit (bit 6)
    if (enable) {
        reg_value |= (1 << 6);
    } else {
        reg_value &= ~(1 << 6);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "OLP diagnostic %s", enable ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_olp_delay(int delay) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update OLP_DLY bit (bit 5)
    if (delay) {
        reg_value |= (1 << 5);
    } else {
        reg_value &= ~(1 << 5);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "OLP delay set to %s", delay ? "1.2ms" : "300us");
    return ESP_OK;
}

esp_err_t drv8873_set_en_ola(int enable) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update EN_OLA bit (bit 4)
    if (enable) {
        reg_value |= (1 << 4);
    } else {
        reg_value &= ~(1 << 4);
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "OLA diagnostic %s", enable ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_clear_fault(void) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC3 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC3_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Write 1 to CLR_FLT bit (bit 7) to clear faults
    reg_value |= (1 << 7);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC3_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Faults cleared via CLR_FLT");
    return ESP_OK;
}

esp_err_t drv8873_lock_registers(int lock) {
    uint16_t reg_value;
    esp_err_t ret;

    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Read IC3 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC3_CONTROL, &reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update LOCK field (bits 6-4)
    if (lock) {
        reg_value = (reg_value & 0x8FFF) | (0x03 << 4); // 011b = Locked
    } else {
        reg_value = (reg_value & 0x8FFF) | (0x04 << 4); // 100b = Unlocked
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC3_CONTROL, reg_value);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Registers %s", lock ? "locked" : "unlocked");
    return ESP_OK;
}

esp_err_t drv8873_get_fault_status(uint16_t *fault_status) {
    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = drv8873_spi_read_register(DRV8873_REG_FAULT_STATUS, fault_status);
    if (ret != ESP_OK) {
        return ret;
    }

    // Check if FAULT bit is set
    if (*fault_status & DRV8873_FAULT_FAULT_MASK) {
        uint16_t diag_status;
        ret = drv8873_spi_read_register(DRV8873_REG_DIAG_STATUS, &diag_status);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read DIAG register after FAULT detected");
            return ret;
        }

        // Analyze specific fault causes according to documentation
        if (*fault_status & DRV8873_FAULT_UVLO_MASK) {
            ESP_LOGE(TAG, "FAULT: Undervoltage Lockout (UVLO) - VM < 4.45V");
            return ret; // Stop further analysis when UVLO is detected
        }
        if (*fault_status & DRV8873_FAULT_CPUV_MASK) {
            ESP_LOGE(TAG, "FAULT: Charge Pump Undervoltage (CPUV) - VCP < VVM + 2.25V");
        }
        if (*fault_status & DRV8873_FAULT_OCP_MASK) {
            ESP_LOGE(TAG, "FAULT: Overcurrent Protection (OCP) - Current limit exceeded");
        }
        if (*fault_status & DRV8873_FAULT_TSD_MASK) {
            ESP_LOGE(TAG, "FAULT: Thermal Shutdown (TSD) - TJ > 165°C");
        }
        if (*fault_status & DRV8873_FAULT_OLD_MASK) {
            ESP_LOGE(TAG, "FAULT: Open-Load Detection (OLD) - No load detected");
        }
        if (*fault_status & DRV8873_FAULT_OTW_MASK) {
            ESP_LOGE(TAG, "FAULT: Overtemperature Warning (OTW) - TJ > 140°C");
        }

        // Analyze DIAG register details
        if (diag_status & DRV8873_DIAG_OL1_MASK) {
            ESP_LOGI(TAG, "DIAG: Open Load on Half-Bridge 1");
        }
        if (diag_status & DRV8873_DIAG_OL2_MASK) {
            ESP_LOGI(TAG, "DIAG: Open Load on Half-Bridge 2");
        }
        if (diag_status & DRV8873_DIAG_OCP_H1_MASK) {
            ESP_LOGE(TAG, "DIAG: Overcurrent on High-Side FET 1");
        }
        if (diag_status & DRV8873_DIAG_OCP_L1_MASK) {
            ESP_LOGE(TAG, "DIAG: Overcurrent on Low-Side FET 1");
        }
        if (diag_status & DRV8873_DIAG_OCP_H2_MASK) {
            ESP_LOGE(TAG, "DIAG: Overcurrent on High-Side FET 2");
        }
        if (diag_status & DRV8873_DIAG_OCP_L2_MASK) {
            ESP_LOGE(TAG, "DIAG: Overcurrent on Low-Side FET 2");
        }
        if (diag_status & DRV8873_DIAG_ITRIP1_MASK) {
            ESP_LOGW(TAG, "DIAG: Current Regulation Active on Half-Bridge 1");
        }
        if (diag_status & DRV8873_DIAG_ITRIP2_MASK) {
            ESP_LOGW(TAG, "DIAG: Current Regulation Active on Half-Bridge 2");
        }

        if (diag_status & (DRV8873_DIAG_OL1_MASK | DRV8873_DIAG_OL2_MASK)) {
            ESP_LOGI(TAG, "FAULT detected. DIAG status: 0x%04X", diag_status);
        } else {
            ESP_LOGE(TAG, "FAULT detected. DIAG status: 0x%04X", diag_status);
        }
    }

    return ret;
}

esp_err_t drv8873_get_diag_status(uint16_t *diag_status) {
    // Use global config only (no config parameter anymore)
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    return drv8873_spi_read_register(DRV8873_REG_DIAG_STATUS, diag_status);
}
