/**
 * @file StrainGauge.cpp
 * @brief Strain gauge (current or voltage excitation)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "StrainGauge.h"

static const char TAG[] = "StrainGauge";

float StrainGauge::read(void)
{
    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    switch (_excitation)
    {
    case EXCITATION_VOLTAGE:

        break;
    
    default:
        break;
    }

    return 0.0;
}