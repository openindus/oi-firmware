/**
 * @file drv8873.h
 * @brief ESP-IDF driver for DRV8873 SPI control.
 * @details This driver provides full access to all DRV8873 registers as documented in the official datasheet.
 */
#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

// DRV8873 SPI Register Addresses (as per datasheet)
typedef enum {
    DRV8873_REG_FAULT_STATUS = 0x00,  // FAULT Status Register (Read-only)
    DRV8873_REG_DIAG_STATUS = 0x01,   // DIAG Status Register (Read-only)
    DRV8873_REG_IC1_CONTROL = 0x02,   // IC1 Control Register (R/W)
    DRV8873_REG_IC2_CONTROL = 0x03,   // IC2 Control Register (R/W)
    DRV8873_REG_IC3_CONTROL = 0x04,   // IC3 Control Register (R/W)
    DRV8873_REG_IC4_CONTROL = 0x05,   // IC4 Control Register (R/W)
} drv8873_register_t;

// DRV8873 Control Modes (IC1 Control Register, bits 1-0)
typedef enum {
    DRV8873_MODE_PH_EN = 0x00,        // Phase/Enable mode
    DRV8873_MODE_PWM = 0x01,          // PWM mode
    DRV8873_MODE_INDEPENDENT = 0x02,  // Independent half-bridge mode
    DRV8873_MODE_DISABLED = 0x03,     // Disabled (High-Z)
} drv8873_mode_t;

// OCP (Overcurrent Protection) Modes (IC2 Control Register, bits 1-0)
typedef enum {
    DRV8873_OCP_MODE_LATCHED = 0x00,      // Latched: H-bridge goes to Hi-Z, requires CLR_FLT or nSLEEP
    DRV8873_OCP_MODE_AUTO_RETRY = 0x01,   // Auto-retry: H-bridge goes to Hi-Z, auto-retries after OCP_TRETRY
    DRV8873_OCP_MODE_REPORT = 0x02,       // Report only: H-bridge remains active, only reports fault
    DRV8873_OCP_MODE_DISABLED = 0x03,     // Disabled: No OCP protection
} drv8873_ocp_mode_t;

// ITRIP Current Limit Levels (IC4 Control Register, bits 3-2)
typedef enum {
    DRV8873_ITRIP_LEVEL_4A = 0x00,        // 4.0 A
    DRV8873_ITRIP_LEVEL_5_4A = 0x01,      // 5.4 A
    DRV8873_ITRIP_LEVEL_6_5A = 0x02,      // 6.5 A
    DRV8873_ITRIP_LEVEL_7A = 0x03,        // 7.0 A
} drv8873_itrip_level_t;

// Current Regulation Disable Options (IC4 Control Register, bits 1-0)
typedef enum {
    DRV8873_CURR_REG_ENABLED = 0x00,      // Current regulation enabled for OUT1 and OUT2
    DRV8873_CURR_REG_DISABLE_OUT1 = 0x01, // Current regulation disabled for OUT1
    DRV8873_CURR_REG_DISABLE_OUT2 = 0x02, // Current regulation disabled for OUT2
    DRV8873_CURR_REG_DISABLE_BOTH = 0x03, // Current regulation disabled for OUT1 and OUT2
} drv8873_current_regulation_t;

// FAULT Status Register Bit Masks (Register 0x00)
typedef enum {
    DRV8873_FAULT_RSVD_MASK = 0x80,       // Reserved (bit 7)
    DRV8873_FAULT_FAULT_MASK = 0x40,      // Global fault (bit 6) - complements nFAULT pin
    DRV8873_FAULT_OTW_MASK = 0x20,        // Overtemperature Warning (bit 5)
    DRV8873_FAULT_UVLO_MASK = 0x10,       // Undervoltage Lockout (bit 4)
    DRV8873_FAULT_CPUV_MASK = 0x08,       // Charge Pump Undervoltage (bit 3)
    DRV8873_FAULT_OCP_MASK = 0x04,        // Overcurrent Protection (bit 2)
    DRV8873_FAULT_TSD_MASK = 0x02,        // Thermal Shutdown (bit 1)
    DRV8873_FAULT_OLD_MASK = 0x01,        // Open Load Detection (bit 0)
} drv8873_fault_mask_t;

// DIAG Status Register Bit Masks (Register 0x01)
typedef enum {
    DRV8873_DIAG_OL1_MASK = 0x80,         // Open Load on half-bridge 1 (bit 7)
    DRV8873_DIAG_OL2_MASK = 0x40,         // Open Load on half-bridge 2 (bit 6)
    DRV8873_DIAG_ITRIP1_MASK = 0x20,      // Current regulation status half-bridge 1 (bit 5)
    DRV8873_DIAG_ITRIP2_MASK = 0x10,      // Current regulation status half-bridge 2 (bit 4)
    DRV8873_DIAG_OCP_H1_MASK = 0x08,      // Overcurrent on high-side FET 1 (bit 3)
    DRV8873_DIAG_OCP_L1_MASK = 0x04,      // Overcurrent on low-side FET 1 (bit 2)
    DRV8873_DIAG_OCP_H2_MASK = 0x02,      // Overcurrent on high-side FET 2 (bit 1)
    DRV8873_DIAG_OCP_L2_MASK = 0x01,      // Overcurrent on low-side FET 2 (bit 0)
} drv8873_diag_mask_t;

// SPI Configuration Structure
typedef struct {
    spi_device_handle_t spi_handle;  // SPI handle for DRV8873
    int nSCS_pin;                    // Chip Select (CS) pin
} drv8873_spi_config_t;

extern drv8873_spi_config_t *drv8873_global_config;

// Initialize SPI bus for DRV8873
esp_err_t drv8873_spi_init(drv8873_spi_config_t *config);

// Read a SPI register from DRV8873 (read-only)
esp_err_t drv8873_spi_read_register(drv8873_register_t reg_address, uint16_t *reg_value);

// Write a SPI register to DRV8873 (write)
esp_err_t drv8873_spi_write_register(drv8873_register_t reg_address, uint16_t reg_value);

/**
 * @brief Configure the DRV8873 control mode
 * @param mode Control mode to set (PH/EN, PWM, Independent, Disabled)
 * @details Writes to bits 1-0 of IC1_CONTROL register (0x02)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_mode(drv8873_mode_t mode);

/**
 * @brief Configure the ITRIP current limit threshold
 * @param itrip_level Current limit level to set (4A, 5.4A, 6.5A, 7A)
 * @details Writes to bits 3-2 of IC4_CONTROL register (0x05)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_current_limit(drv8873_itrip_level_t itrip_level);

/**
 * @brief Disable current regulation on half-bridges
 * @param disable_flags Flags to disable regulation (none, OUT1, OUT2, both)
 * @details Writes to bits 1-0 of IC4_CONTROL register (0x05)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_current_regulation(drv8873_current_regulation_t disable_flags);

/**
 * @brief Configure the Overcurrent Protection (OCP) mode
 * @param ocp_mode OCP mode to set (Latched, Auto-retry, Report only, Disabled)
 * @details Writes to bits 1-0 of IC2_CONTROL register (0x03)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_ocp_mode(drv8873_ocp_mode_t ocp_mode);

/**
 * @brief Configure Thermal Shutdown (TSD) behavior
 * @param auto_recovery 1 for auto-recovery, 0 for latched shutdown
 * @details Writes to bit 6 of IC2_CONTROL register (0x03)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_tsd_mode(int auto_recovery);

/**
 * @brief Enable/disable ITRIP event reporting on nFAULT
 * @param report 1 to enable reporting, 0 to disable
 * @details Writes to bit 7 of IC2_CONTROL register (0x03)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_itrip_rep(int report);

/**
 * @brief Enable/disable Overtemperature Warning (OTW) reporting on nFAULT
 * @param report 1 to enable reporting, 0 to disable
 * @details Writes to bit 5 of IC2_CONTROL register (0x03)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_otw_rep(int report);

/**
 * @brief Enable/disable Charge Pump Undervoltage (CPUV) detection
 * @param disable 1 to disable detection (H-bridge remains active), 0 to enable (H-bridge goes to Hi-Z)
 * @details Writes to bit 4 of IC2_CONTROL register (0x03)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_dis_cpuv(int disable);

/**
 * @brief Configure OCP auto-retry time
 * @param retry_time Retry time (0=0.5ms, 1=1ms, 2=2ms, 3=4ms)
 * @details Writes to bits 3-2 of IC2_CONTROL register (0x03)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_ocp_tretry(int retry_time);

/**
 * @brief Enable passive Open-Load detection (OLP)
 * @param enable 1 to enable, 0 to disable
 * @details Writes to bit 6 of IC4_CONTROL register (0x05)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_en_olp(int enable);

/**
 * @brief Configure OLP diagnostic delay
 * @param delay 1 for 1.2ms, 0 for 300µs
 * @details Writes to bit 5 of IC4_CONTROL register (0x05)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_olp_delay(int delay);

/**
 * @brief Enable active Open-Load detection (OLA)
 * @param enable 1 to enable, 0 to disable
 * @details Writes to bit 4 of IC4_CONTROL register (0x05)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_en_ola(int enable);

/**
 * @brief Disable current regulation (ITRIP) on half-bridges
 * @param disable_flags Flags to disable regulation (none, OUT1, OUT2, both)
 * @details Writes to bits 1-0 of IC4_CONTROL register (0x05)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_set_dis_itrip(drv8873_current_regulation_t disable_flags);

/**
 * @brief Read the FAULT Status Register
 * @param fault_status Pointer to receive register content
 * @details Reads register 0x00 and analyzes fault causes
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_get_fault_status(uint16_t *fault_status);

/**
 * @brief Read the DIAG Status Register
 * @param diag_status Pointer to receive register content
 * @details Reads register 0x01 containing diagnostic details
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_get_diag_status(uint16_t *diag_status);

/**
 * @brief Clear all faults by writing 1 to CLR_FLT bit
 * @details Writes 1 to bit 7 of IC3_CONTROL register (0x04), auto-resets
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_clear_fault(void);

/**
 * @brief Lock/unlock control registers
 * @param lock 1 to lock (011b), 0 to unlock (100b)
 * @details Writes to bits 6-4 of IC3_CONTROL register (0x04)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t drv8873_lock_registers(int lock);

#ifdef __cplusplus
}
#endif
