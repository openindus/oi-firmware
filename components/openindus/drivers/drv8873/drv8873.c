#include "drv8873.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "DRV8873_SPI";

// Global configuration instance (defined here, declared in .h)
drv8873_spi_config_t *drv8873_global_config = NULL;

#define HEADER_BYTE 0b10000000
#define READ_ADDRESS_BYTE (1 << 6)

esp_err_t drv8873_spi_init(const drv8873_spi_config_t *config) {
    esp_err_t ret;

    // Validate configuration
    if (!config || config->device_count < 1 || config->device_count > 63) {
        ESP_LOGE(TAG, "Invalid daisy chain configuration: device_count must be 1-63");
        return ESP_ERR_INVALID_ARG;
    }

    // Configure the SPI device (DRV8873)
    const spi_device_interface_config_t dev_config = {
        .clock_speed_hz = 1 * 1000 * 1000,  // 1 MHz (adjustable as needed)
        .mode = 1,                          // SPI Mode 1 (CPOL=0, CPHA=1)
        .spics_io_num = config->nSCS_pin,   // Chip Select pin
        .queue_size = 10,                   // Queue size
        .command_bits = 0,                  // No command phase
        .address_bits = 0,                  // No address phase
        .dummy_bits = 0,                    // No dummy bits
        .flags = SPI_DEVICE_NO_DUMMY,
        .cs_ena_pretrans = 1                // add 1 period between CS and first clock beat
    };

    ret = spi_bus_add_device(SPI2_HOST, &dev_config, &config->spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    // Store the config globally for future use
    drv8873_global_config = config;

    // TODO: Remove debug 
    // get reg 0 of device 0
    uint8_t reg_value;
    drv8873_spi_read_register(0, &reg_value, 0);
    ESP_LOGD(TAG, "Read register 0 of device 0: %02X", reg_value);

    ESP_LOGI(TAG, "SPI daisy chain initialized with %d devices", config->device_count);
    return ESP_OK;
}

esp_err_t drv8873_spi_read_register(drv8873_register_t reg_address, uint8_t *reg_value, int device_index) {

    const int device_count = drv8873_global_config->device_count;
    // Validate parameters
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (device_index < 0 || device_index >= device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d (valid range: 0-%d)", 
                 device_index, drv8873_global_config->device_count - 1);
        return ESP_ERR_INVALID_ARG;
    }

    // Calculate total transaction length: 
    // 2 bytes for headers, plus device_count * (1 address + 1 data)
    const int total_bytes = (2 + (2 * device_count));
    
    // Create full transaction buffer: 16 bits per device
    uint8_t *tx_buffer = (uint8_t*)calloc(total_bytes, sizeof(uint8_t));
    uint8_t *rx_buffer = (uint8_t*)calloc(total_bytes, sizeof(uint8_t));

    if (!tx_buffer || !rx_buffer) {
        ESP_LOGE(TAG, "Failed to allocate memory for SPI transaction");
        free(tx_buffer);
        free(rx_buffer);
        return ESP_ERR_NO_MEM;
    }

    // first header byte MSBs are 10, and other bits is the device count.
    tx_buffer[0] = HEADER_BYTE | (device_count & 0b00111111);
    // second header byte MSBs are 10. then, if we clear faults, 
    // and last 4 bits can be integrity bits.
    tx_buffer[1] = HEADER_BYTE | 0b00010101;

    tx_buffer[2] = READ_ADDRESS_BYTE;// init read
    tx_buffer[3] = READ_ADDRESS_BYTE;// init read
    tx_buffer[4] = READ_ADDRESS_BYTE;// init read
    tx_buffer[5] = READ_ADDRESS_BYTE;// init read

    // then, for each device, address bytes. starts by the last device in chain.
    int index_of_address_for_device = 2 + device_count - 1 - device_index;
    // Create command: Read bit (1<<6) + register address (bits 5-1)
    uint8_t command = READ_ADDRESS_BYTE | ((reg_address << 1) & 0b00111110);
    tx_buffer[index_of_address_for_device] = command;

    // then, for each device, data bytes. starts by the last device in chain.
    int index_of_data_for_device = 2 + (2 * device_count) - 1 - device_index;

    // Configure SPI transaction
    spi_transaction_t trans = {
        .length = total_bytes * 8, // Total bits to transmit
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer,
    };

    esp_err_t ret = spi_device_transmit(drv8873_global_config->spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read register 0x%02X from device %d: %s", 
                 reg_address, device_index, esp_err_to_name(ret));
        free(tx_buffer);
        free(rx_buffer);
        return ret;
    }

    // Extract the response for the target device (last device in chain responds first)
    *reg_value = rx_buffer[index_of_data_for_device];

    // header validation
    if(tx_buffer[0] != rx_buffer[device_count]){
        ESP_LOGE(TAG, "Header byte 1 mismatch: sent %02X, received %02X", tx_buffer[0], rx_buffer[device_count]);
        ret = ESP_ERR_INVALID_RESPONSE;
    }
    if (tx_buffer[1] != rx_buffer[device_count + 1]){
        ESP_LOGE(TAG, "Header byte 2 mismatch: sent %02X, received %02X", tx_buffer[1], rx_buffer[device_count + 1]);
        ret = ESP_ERR_INVALID_RESPONSE;
    }
// logd full buffers
    if(total_bytes == 10){
        // TX:   HDR1  HDR2  A4   A3   A2   A1   D4   D3   D2   D1
        // RX:   S4    S3    S2   S1   HDR1 HDR2  R4   R3   R2   R1
        ESP_LOGD(TAG, "TX: HDR1 HDR2 A4 A3 A2 A1 D4 D3 D2 D1");
        ESP_LOGD(TAG, "TX:  %02X   %02X  %02X %02X %02X %02X %02X %02X %02X %02X", 
                tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3], tx_buffer[4],
                tx_buffer[5], tx_buffer[6], tx_buffer[7], tx_buffer[8], tx_buffer[9]);
        ESP_LOGD(TAG, "RX: S4 S3 S2 S1 HDR1 HDR2 R4 R3 R2 R1");
        ESP_LOGD(TAG, "RX: %02X %02X %02X %02X  %02X   %02X  %02X %02X %02X %02X", 
                rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4],
                rx_buffer[5], rx_buffer[6], rx_buffer[7], rx_buffer[8], rx_buffer[9]);
        ESP_LOGD(TAG, "SPI Read Reg 0x%02X from Device %d: RX: %02X", reg_address, device_index, *reg_value);
    }

    free(tx_buffer);
    free(rx_buffer);
    return ret;
}

esp_err_t drv8873_spi_write_register(drv8873_register_t reg_address, uint8_t reg_value, int device_index) {
    // Validate parameters
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d (valid range: 0-%d)", 
                 device_index, drv8873_global_config->device_count - 1);
        return ESP_ERR_INVALID_ARG;
    }

    // temporary return to debug.
    ESP_LOGW(TAG, "Writing disabled for debug purposes.");
    return ESP_OK;

    // Calculate total transaction length: 
    // 2 bytes for headers, plus device_count * (1 address + 1 data)
    int total_bytes = 2 + 2 * drv8873_global_config->device_count;
    
    // Create full transaction buffer: bytes
    uint8_t *tx_buffer = (uint8_t*)calloc(total_bytes, sizeof(uint8_t));

    if (!tx_buffer) {
        ESP_LOGE(TAG, "Failed to allocate memory for SPI transaction");
        return ESP_ERR_NO_MEM;
    }

    // first header byte MSBs are 10, and other bits is the device count.
    tx_buffer[0] = HEADER_BYTE | (drv8873_global_config->device_count & 0b00111111);
    // second header byte MSBs are 10. then, if we clear faults, and last 4 bits are don't care.
    tx_buffer[1] = HEADER_BYTE;

    // then, for each device, address bytes. starts by the last device in chain.
    int index_of_address_for_device = 2 + drv8873_global_config->device_count - 1 - device_index;
    // Create command: Write bit (0<<6) + register address (bits 5-1)
    uint8_t command = (0 << 6) | ((reg_address << 1) & 0b00111110);
    tx_buffer[index_of_address_for_device] = command;

    // then, for each device, data bytes. starts by the last device in chain.
    int index_of_data_for_device = 2 + (2 * drv8873_global_config->device_count) - 1 - device_index;
    tx_buffer[index_of_data_for_device] = reg_value;

    // Configure SPI transaction
    spi_transaction_t trans = {
        .length = total_bytes * 8, // Total bits to transmit
        .tx_buffer = tx_buffer,
        .rx_buffer = NULL,              // No need to receive data for write
    };

    // logd full buffers
    if(total_bytes == 10){
    ESP_LOGD(TAG, "TX: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", 
             tx_buffer[0], tx_buffer[1], tx_buffer[2], tx_buffer[3], tx_buffer[4],
             tx_buffer[5], tx_buffer[6], tx_buffer[7], tx_buffer[8], tx_buffer[9]);
    }

    esp_err_t ret = spi_device_transmit(drv8873_global_config->spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register 0x%02X to device %d: %s", 
                 reg_address, device_index, esp_err_to_name(ret));
    }

    free(tx_buffer);
    return ret;
}

esp_err_t drv8873_set_mode(drv8873_mode_t mode, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC1 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC1_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update MODE field (bits 1-0)
    reg_value = (reg_value & 0b11111100) | (mode & 0b00000011);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC1_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: Mode set to 0x%02X", device_index, mode);
    return ESP_OK;
}

esp_err_t drv8873_set_current_limit(drv8873_itrip_level_t itrip_level, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update ITRIP_LVL field (bits 3-2)
    reg_value = (reg_value & 0xC3) | ((itrip_level & 0x03) << 2);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: Current limit set to 0x%02X", device_index, itrip_level);
    return ESP_OK;
}

esp_err_t drv8873_set_current_regulation(drv8873_current_regulation_t disable_flags, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update DIS_ITRIP field (bits 1-0)
    reg_value = (reg_value & 0xFC) | (disable_flags & 0x03);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: Current regulation for OUT1/OUT2: 0x%02X", device_index, disable_flags);
    return ESP_OK;
}

esp_err_t drv8873_set_ocp_mode(drv8873_ocp_mode_t ocp_mode, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update OCP_MODE field (bits 1-0)
    reg_value = (reg_value & 0xFC) | (ocp_mode & 0x03);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: OCP mode set to 0x%02X", device_index, ocp_mode);
    return ESP_OK;
}

esp_err_t drv8873_set_tsd_mode(int auto_recovery, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: TSD mode set to %s", device_index, auto_recovery ? "auto-recovery" : "latched");
    return ESP_OK;
}

esp_err_t drv8873_set_itrip_rep(int report, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: ITRIP report %s", device_index, report ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_otw_rep(int report, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: OTW report %s", device_index, report ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_dis_cpuv(int disable, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: CPUV %s", device_index, disable ? "disabled" : "enabled");
    return ESP_OK;
}

esp_err_t drv8873_set_ocp_tretry(int retry_time, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC2 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC2_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update OCP_TRETRY field (bits 3-2)
    reg_value = (reg_value & 0xF3) | ((retry_time & 0x03) << 2);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC2_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: OCP retry time set to %dms", device_index, retry_time * 500);
    return ESP_OK;
}

esp_err_t drv8873_set_en_olp(int enable, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: OLP diagnostic %s", device_index, enable ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_olp_delay(int delay, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: OLP delay set to %s", device_index, delay ? "1.2ms" : "300us");
    return ESP_OK;
}

esp_err_t drv8873_set_en_ola(int enable, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value, device_index);
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
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: OLA diagnostic %s", device_index, enable ? "enabled" : "disabled");
    return ESP_OK;
}

esp_err_t drv8873_set_dis_itrip(drv8873_current_regulation_t disable_flags, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC4 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC4_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update DIS_ITRIP field (bits 1-0)
    reg_value = (reg_value & 0xFC) | (disable_flags & 0x03);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC4_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: ITRIP disabled for OUT1/OUT2: 0x%02X", device_index, disable_flags);
    return ESP_OK;
}

esp_err_t drv8873_get_fault_status(uint8_t *fault_status, int device_index) {
    // Validate parameters
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read FAULT_STATUS register (16-bit register, two 8-bit bytes)
    uint8_t fault_low, fault_high;
    esp_err_t ret = drv8873_spi_read_register(DRV8873_REG_FAULT_STATUS, &fault_low, device_index);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = drv8873_spi_read_register(DRV8873_REG_FAULT_STATUS + 1, &fault_high, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Combine bytes into 16-bit value (low byte first, then high byte)
    *fault_status = (fault_high << 8) | fault_low;

    // Check if FAULT bit is set
    if (*fault_status & DRV8873_FAULT_FAULT_MASK) {
        uint8_t diag_status;
        ret = drv8873_spi_read_register(DRV8873_REG_DIAG_STATUS, &diag_status, device_index);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read DIAG register after FAULT detected on device %d", device_index);
            return ret;
        }

        // Analyze specific fault causes according to documentation
        if (*fault_status & DRV8873_FAULT_UVLO_MASK) {
            ESP_LOGE(TAG, "Device %d: FAULT: Undervoltage Lockout (UVLO) - VM < 4.45V", device_index);
            return ret; // Stop further analysis when UVLO is detected
        }
        if (*fault_status & DRV8873_FAULT_CPUV_MASK) {
            ESP_LOGE(TAG, "Device %d: FAULT: Charge Pump Undervoltage (CPUV) - VCP < VVM + 2.25V", device_index);
        }
        if (*fault_status & DRV8873_FAULT_OCP_MASK) {
            ESP_LOGE(TAG, "Device %d: FAULT: Overcurrent Protection (OCP) - Current limit exceeded", device_index);
        }
        if (*fault_status & DRV8873_FAULT_TSD_MASK) {
            ESP_LOGE(TAG, "Device %d: FAULT: Thermal Shutdown (TSD) - TJ > 165°C", device_index);
        }
        if (*fault_status & DRV8873_FAULT_OLD_MASK) {
            ESP_LOGE(TAG, "Device %d: FAULT: Open-Load Detection (OLD) - No load detected", device_index);
        }
        if (*fault_status & DRV8873_FAULT_OTW_MASK) {
            ESP_LOGE(TAG, "Device %d: FAULT: Overtemperature Warning (OTW) - TJ > 140°C", device_index);
        }

        // Analyze DIAG register details
        if (diag_status & DRV8873_DIAG_OL1_MASK) {
            ESP_LOGI(TAG, "Device %d: DIAG: Open Load on Half-Bridge 1", device_index);
        }
        if (diag_status & DRV8873_DIAG_OL2_MASK) {
            ESP_LOGI(TAG, "Device %d: DIAG: Open Load on Half-Bridge 2", device_index);
        }
        if (diag_status & DRV8873_DIAG_OCP_H1_MASK) {
            ESP_LOGE(TAG, "Device %d: DIAG: Overcurrent on High-Side FET 1", device_index);
        }
        if (diag_status & DRV8873_DIAG_OCP_L1_MASK) {
            ESP_LOGE(TAG, "Device %d: DIAG: Overcurrent on Low-Side FET 1", device_index);
        }
        if (diag_status & DRV8873_DIAG_OCP_H2_MASK) {
            ESP_LOGE(TAG, "Device %d: DIAG: Overcurrent on High-Side FET 2", device_index);
        }
        if (diag_status & DRV8873_DIAG_OCP_L2_MASK) {
            ESP_LOGE(TAG, "Device %d: DIAG: Overcurrent on Low-Side FET 2", device_index);
        }
        if (diag_status & DRV8873_DIAG_ITRIP1_MASK) {
            ESP_LOGW(TAG, "Device %d: DIAG: Current Regulation Active on Half-Bridge 1", device_index);
        }
        if (diag_status & DRV8873_DIAG_ITRIP2_MASK) {
            ESP_LOGW(TAG, "Device %d: DIAG: Current Regulation Active on Half-Bridge 2", device_index);
        }

        if (diag_status & (DRV8873_DIAG_OL1_MASK | DRV8873_DIAG_OL2_MASK)) {
            ESP_LOGI(TAG, "Device %d: FAULT detected. DIAG status: 0x%04X (open-load detection is normal when outputs are disabled)", device_index, diag_status);
        } else {
            ESP_LOGE(TAG, "Device %d: FAULT detected. DIAG status: 0x%04X", device_index, diag_status);
        }
    }

    return ret;
}

esp_err_t drv8873_get_diag_status(uint8_t *diag_status, int device_index) {
    // Validate parameters
    if (!drv8873_global_config || !drv8873_global_config->spi_handle) {
        ESP_LOGE(TAG, "DRV8873 SPI not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read DIAG_STATUS register (16-bit register, two 8-bit bytes)
    uint8_t diag_low, diag_high;
    esp_err_t ret = drv8873_spi_read_register(DRV8873_REG_DIAG_STATUS, &diag_low, device_index);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = drv8873_spi_read_register(DRV8873_REG_DIAG_STATUS + 1, &diag_high, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Combine bytes into 16-bit value (low byte first, then high byte)
    *diag_status = (diag_high << 8) | diag_low;

    return ret;
}

esp_err_t drv8873_clear_fault(int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC3 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC3_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Write 1 to CLR_FLT bit (bit 7) to clear faults
    reg_value |= (1 << 7);

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC3_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: Faults cleared via CLR_FLT", device_index);
    return ESP_OK;
}

esp_err_t drv8873_lock_registers(int lock, int device_index) {
    uint8_t reg_value;
    esp_err_t ret;

    // Validate device index
    if (device_index < 0 || device_index >= drv8873_global_config->device_count) {
        ESP_LOGE(TAG, "Invalid device index: %d", device_index);
        return ESP_ERR_INVALID_ARG;
    }

    // Read IC3 Control register
    ret = drv8873_spi_read_register(DRV8873_REG_IC3_CONTROL, &reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    // Update LOCK field (bits 6-4)
    if (lock) {
        reg_value = (reg_value & 0x8F) | (0x03 << 4); // 011b = Locked
    } else {
        reg_value = (reg_value & 0x8F) | (0x04 << 4); // 100b = Unlocked
    }

    // Write updated register
    ret = drv8873_spi_write_register(DRV8873_REG_IC3_CONTROL, reg_value, device_index);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Device %d: Registers %s", device_index, lock ? "locked" : "unlocked");
    return ESP_OK;
}
