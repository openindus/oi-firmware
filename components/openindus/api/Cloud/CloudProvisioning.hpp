/**
 * @file CloudProvisioning.hpp
 * @brief Device provisioning against the OI-Cloud REST API (create + status).
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Device credentials returned by the platform on device creation.
 */
struct DeviceCredentials {
    std::string uuid;
    std::string token;
};

/**
 * @brief Device authorization status returned by the status endpoint (spec 3).
 */
struct DeviceStatus {
    bool pending;
    bool accepted;
};

/**
 * @brief REST provisioning helpers (spec section 3).
 *
 * All calls use HTTPS with the ESP-IDF certificate bundle for server verification.
 */
namespace CloudProvisioning {

/**
 * @brief POST /api/v1/platform/{platformUuid}/device
 *
 * Body: {"name","project_id","platform_token"}
 *
 * @param host Cloud host (e.g. "cloud.openindus.com")
 * @param platformUuid Platform UUID
 * @param platformToken Platform token
 * @param projectId Project id
 * @param deviceName Device name (MAC address)
 * @param[out] out Filled with uuid+token on HTTP 201
 * @return HTTP status code (201 created, 409 already created), or -1 on transport error
 */
int createDevice(const std::string& host, const std::string& platformUuid,
                 const std::string& platformToken, int projectId,
                 const std::string& deviceName, DeviceCredentials& out);

/**
 * @brief GET /api/v1/plateform/{platformUuid}/device/{deviceUuid}/status
 *
 * Header: Authorization: Bearer {token}
 *
 * @param host Cloud host
 * @param platformUuid Platform UUID
 * @param deviceUuid Device UUID
 * @param token Device token
 * @param[out] out Filled with pending/accepted on HTTP 200
 * @return HTTP status code (200 ok, 401 device not recognized), or -1 on transport error
 */
int getStatus(const std::string& host, const std::string& platformUuid,
              const std::string& deviceUuid, const std::string& token,
              DeviceStatus& out);

} // namespace CloudProvisioning
