#pragma once

#include "CorePinout.h"
#define ETH_SPI_MOSI_PIN CORE_PIN_SPI_MOSI
#define ETH_SPI_MISO_PIN CORE_PIN_SPI_MISO
#define ETH_SPI_SCLK_PIN CORE_PIN_SPI_SCK
#define ETH_SPI_CS_PIN CORE_PIN_ETHERNET_SPI_CS
#define ETH_SPI_HOST_ID CORE_SPI_HOST
#define ETH_SPI_CLOCK_SPEED CONFIG_NETIF_SPI_ETHERNET_SPEED
#define ETH_INTERRUPT_PIN 18
#define ETH_SPI_COMMAND_BITS 16
#define ETH_SPI_ADDRESS_BITS 8