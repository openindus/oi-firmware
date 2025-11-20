/**
 * @file spi_master.h
 * @brief Driver ESP-IDF pour le contrôle du DRV8873 via SPI.
 * @details Ce driver permet de lire/écrire les registres du DRV8873.
 */
#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Adresses des registres du DRV8873
typedef enum {
    DRV8873_REG_FAULT_STATUS = 0x00,  // FAULT Status Register (Read-only)
    DRV8873_REG_DIAG_STATUS = 0x01,   // DIAG Status Register (Read-only)
    DRV8873_REG_IC1_CONTROL = 0x02,   // IC1 Control Register (R/W)
    DRV8873_REG_IC2_CONTROL = 0x03,   // IC2 Control Register (R/W)
    DRV8873_REG_IC3_CONTROL = 0x04,   // IC3 Control Register (R/W)
    DRV8873_REG_IC4_CONTROL = 0x05,   // IC4 Control Register (R/W)
} drv8873_register_t;

// Modes de contrôle du DRV8873
typedef enum {
    DRV8873_MODE_PH_EN = 0x00,        // Mode Phase/Enable
    DRV8873_MODE_PWM = 0x01,          // Mode PWM
    DRV8873_MODE_INDEPENDENT = 0x02,  // Mode demi-pont indépendant
    DRV8873_MODE_DISABLED = 0x03,     // Mode désactivé (Hi-Z)
} drv8873_mode_t;

// Modes de protection contre les surintensités (OCP)
typedef enum {
    DRV8873_OCP_MODE_LATCHED = 0x00,      // Verrouillage en cas de surintensité
    DRV8873_OCP_MODE_AUTO_RETRY = 0x01,   // Réessai automatique
    DRV8873_OCP_MODE_REPORT = 0x02,       // Signalement uniquement
    DRV8873_OCP_MODE_DISABLED = 0x03,     // Désactivé
} drv8873_ocp_mode_t;

// Niveaux de seuil de courant (ITRIP)
typedef enum {
    DRV8873_ITRIP_LEVEL_4A = 0x00,        // 4A
    DRV8873_ITRIP_LEVEL_5_4A = 0x01,      // 5.4A
    DRV8873_ITRIP_LEVEL_6_5A = 0x02,      // 6.5A
    DRV8873_ITRIP_LEVEL_7A = 0x03,        // 7A
} drv8873_itrip_level_t;

// Options de désactivation de la régulation de courant
typedef enum {
    DRV8873_CURR_REG_ENABLED = 0x00,      // Régulation activée pour OUT1 et OUT2
    DRV8873_CURR_REG_DISABLE_OUT1 = 0x01, // Régulation désactivée pour OUT1
    DRV8873_CURR_REG_DISABLE_OUT2 = 0x02, // Régulation désactivée pour OUT2
    DRV8873_CURR_REG_DISABLE_BOTH = 0x03, // Régulation désactivée pour OUT1 et OUT2
} drv8873_current_regulation_t;

// Masques pour les bits du registre FAULT Status
typedef enum {
    DRV8873_FAULT_FAULT_MASK = 0x40,      // Défaut global (bit 6)
    DRV8873_FAULT_OTW_MASK = 0x20,        // Overtemperature Warning (bit 5)
    DRV8873_FAULT_UVLO_MASK = 0x10,       // Undervoltage Lockout (bit 4)
    DRV8873_FAULT_CPUV_MASK = 0x08,       // Charge Pump Undervoltage (bit 3)
    DRV8873_FAULT_OCP_MASK = 0x04,        // Overcurrent Protection (bit 2)
    DRV8873_FAULT_TSD_MASK = 0x02,        // Thermal Shutdown (bit 1)
    DRV8873_FAULT_OLD_MASK = 0x01,        // Open Load Detection (bit 0)
} drv8873_fault_mask_t;

// Masques pour les bits du registre DIAG Status
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

// Structure pour la configuration SPI
typedef struct {
    spi_device_handle_t spi_handle;  // Handle SPI pour le DRV8873
    int nSCS_pin;                    // Broche CS (Chip Select)
} drv8873_spi_config_t;

// Instance de configuration globale
extern drv8873_spi_config_t *drv8873_global_config;

// Initialise le bus SPI pour le DRV8873
esp_err_t drv8873_spi_init(drv8873_spi_config_t *config);

// Lit un registre du DRV8873
esp_err_t drv8873_spi_read_register(drv8873_register_t reg_address, uint16_t *reg_value);

// Écrit dans un registre du DRV8873
esp_err_t drv8873_spi_write_register(drv8873_register_t reg_address, uint16_t reg_value);

// Configure le mode de contrôle du DRV8873
esp_err_t drv8873_set_mode(drv8873_mode_t mode);

// Configure le seuil de courant (ITRIP_LVL)
esp_err_t drv8873_set_current_limit(drv8873_itrip_level_t itrip_level);

// Active/désactive la régulation de courant pour OUT1/OUT2
esp_err_t drv8873_set_current_regulation(drv8873_current_regulation_t disable_flags);

// Lit le statut des défauts (FAULT, OCP, TSD, etc.)
esp_err_t drv8873_get_fault_status(uint16_t *fault_status);

#ifdef __cplusplus
}
#endif
