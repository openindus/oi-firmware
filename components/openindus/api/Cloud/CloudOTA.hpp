/**
 * @file CloudOTA.hpp
 * @brief Over-the-air firmware update driven by the "ota" cloud variable.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

// The Cloud library is only available on master and standalone modules
// (a slave has no network stack of its own and is driven over the bus).
#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_STANDALONE)

/**
 * @brief Commands carried by the "ota" topic, in the "cmd" field of the JSON
 * payload {"cmd": <int>, "args": <string|int>}.
 *
 * - UPDATE   (cloud -> device): args is the firmware download URL (string).
 * - PROGRESS (device -> cloud): args is the number of bytes written (int).
 * - END      (device -> cloud): args is the error code (int), 0 == success.
 */
enum class CloudOtaCmd {
    UPDATE   = 0,
    PROGRESS = 1,
    END      = 2
};

/**
 * @brief OTA update handler.
 *
 * Singleton, because it owns a single global resource: the inactive OTA
 * partition. An UPDATE command spawns a dedicated task that streams the image
 * from the given URL straight into that partition (no RAM staging), reports its
 * progress on the same topic and reboots on success.
 */
class CloudOTA {
private:
    /** @brief Publisher installed by Cloud, emits on the "ota" topic. */
    std::function<void(const std::string&)> _publish;

    std::string _host;            // Cloud host, prefixed to a host-less URL
    std::string _token;           // Device token, sent as the Bearer credential
    std::string _url;             // Firmware URL of the update in progress
    volatile bool _inProgress;    // An update is running, refuse a new one
    TaskHandle_t _taskHandle;

    static CloudOTA* _instance;

    CloudOTA();

    static void _task(void* arg);
    void _run(void);

    /** @brief Publish {"cmd":cmd,"args":args} on the ota topic. */
    void _send(CloudOtaCmd cmd, int args);

public:
    /**
     * @brief Get the CloudOTA singleton instance.
     */
    static CloudOTA* getInstance(void);

    /**
     * @brief Install the publisher used to emit PROGRESS/END on the ota topic.
     * @param fn Publisher, or nullptr to mute the OTA (e.g. Cloud::end()).
     */
    void setPublisher(std::function<void(const std::string&)> fn);

    /**
     * @brief Set the cloud host used to resolve host-less firmware URLs.
     *
     * The UPDATE command carries a path only (e.g. "/firmware/xxx.bin"); it is
     * resolved against "https://" + host.
     *
     * @param host Cloud host, e.g. "cloud.openindus.com"
     */
    void setHost(const std::string& host);

    /**
     * @brief Set the device token sent as "Authorization: Bearer <token>".
     *
     * The firmware endpoint is authenticated like the rest of the REST API. The
     * token is only known once provisioning succeeded, so it is installed after
     * the credentials are available. It is dropped on a redirection leaving the
     * cloud host, so it never leaks to a third-party storage bucket.
     *
     * @param token Device token, or an empty string to send no Authorization
     */
    void setToken(const std::string& token);

    /**
     * @brief Handle an inbound "ota" payload.
     *
     * Safe to call from the MQTT event task: it only parses the payload and, for
     * an UPDATE command, spawns the download task. It never blocks and never
     * publishes.
     *
     * @param payload JSON payload of the ota topic
     */
    void handlePayload(const std::string& payload);

    /**
     * @brief Checks whether an update is currently running.
     * @return true if a download/flash is in progress
     */
    bool isInProgress(void) const;
};

#endif // CONFIG_MODULE_MASTER || CONFIG_MODULE_STANDALONE
