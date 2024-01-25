#pragma once
#include "esp_mac.h"

struct OIMacAddr_s
{
    unsigned char base_mac_addr[6];
    unsigned char local_mac[6];
} typedef OIMacAddr_t;

OIMacAddr_t getMacAddr();
