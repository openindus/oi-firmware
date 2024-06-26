#pragma once

#define MODEM_MODEL ESP_MODEM_DCE_SIM7000
#define MODEM_TX_GPIO 19
#define MODEM_RX_GPIO 20
#define MODEM_BAUD_RATE CONFIG_NETIF_BAUD_RATE_MODEM
#define MODEM_RTS_GPIO -1
#define MODEM_CTS_GPIO -1
#define MODEM_UART_PORT_NUM 2

#ifdef CONFIG_NETIF_ENABLE_NBIOT
#define MODEM_ENABLE_NB_IOT true
#else
#define MODEM_ENABLE_NB_IOT false
#endif