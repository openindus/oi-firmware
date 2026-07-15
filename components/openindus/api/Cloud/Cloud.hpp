/**
 * @file Cloud.hpp
 * @brief Header file for the Cloud API component.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

// The Cloud library is only available on master and standalone modules
// (a slave has no network stack of its own and is driven over the bus).
#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_STANDALONE)

#include "CloudVariable.hpp"

// Include CloudVariable types
using BoolVariable = CloudVariable<bool>;
using IntVariable = CloudVariable<int>;
using FloatVariable = CloudVariable<float>;
using StringVariable = CloudVariable<std::string>;

/**
 * @brief Cloud connection / provisioning state.
 */
enum class CloudState {
    IDLE,               // Not started
    LOADING_CREDS,      // Reading credentials from NVS
    PROVISION_CREATE,   // Registering the device (POST /device)
    PROVISION_PENDING,  // Waiting for user acceptance (GET /status)
    CONNECTING,         // Connecting to the MQTT broker
    CONNECTED,          // Connected and operational
    RECONNECTING,       // Link lost, attempting to re-establish the connection
    ERR_REJECTED,       // Device rejected by the user
    ERR_ALREADY_CREATED // Device already exists but credentials are lost
};

/**
 * @brief Cloud API class
 *
 * Handles device provisioning (spec section 3), credential persistence in NVS,
 * MQTT-over-WebSocket transport and typed cloud variables. All provisioning,
 * connection and variable servicing runs in a background task so user code
 * never blocks: construct, addVariable(), begin(host), then setValue()/getValue().
 */
class Cloud {
private:
    // Platform identity
    std::string _platformUuid;
    std::string _platformToken;
    int _projectId;

    // Device identity
    std::string _deviceName;  // MAC address, used as provisioning "name"
    std::string _deviceUuid;
    std::string _deviceToken;
    bool _haveCredentials;    // Credentials provided/loaded, skip provisioning

    // Runtime
    std::string _host;
    volatile CloudState _state;
    TaskHandle_t _taskHandle;

    // Registered variables (user + default), non-owning except default vars
    std::vector<ICloudVariable*> _variables;
    std::vector<ICloudVariable*> _defaultVariables; // owned, freed in end()

    // Default variables (spec 2.2)
    StringVariable* _varLog;
    StringVariable* _varVersion;
    BoolVariable* _varRestart;
    StringVariable* _varOta;
    StringVariable* _varModules;

    // Result of the provisioning subgraph (START in the state machine).
    enum class ProvisionResult {
        ACCEPTED, // Device registered and accepted, ready to connect MQTT
        REJECTED  // Device rejected by the user, terminal
    };

    // Internal helpers
    static void _task(void* arg);
    void _run(void);
    ProvisionResult _ensureProvisioned(void); // START subgraph: register / check access
    bool _connectMqtt(uint32_t timeoutMs);    // "Lancer la connexion mqtt"
    void _subscribeAll(void);                 // "Abonnement aux topics"
    void _serviceLoop(void);                  // SERVE: publish until the link is lost
    bool _loadCredentials(void);
    void _saveCredentials(void);
    void _clearCredentials(void);
    void _setupDefaultVariables(void);
    std::string _topicFor(const ICloudVariable* var) const;
    void _subscribeVariable(ICloudVariable* var);
    void _addVariable(ICloudVariable* var);

public:
    /**
     * @brief Constructor for Cloud API (auto-provisioning, preferred).
     * @param platformUuid Platform UUID
     * @param platformToken Platform authentication token
     * @param projectId Project id the device belongs to
     */
    Cloud(const char* platformUuid, const char* platformToken, int projectId);
    ~Cloud();

    /**
     * @brief Provide device credentials directly, bypassing provisioning/NVS.
     * @param uuid Device UUID
     * @param token Device token
     */
    void useDeviceCredentials(const char* uuid, const char* token);

    /**
     * @brief Start the cloud client. Launches the background task and returns
     *        immediately; provisioning and connection happen asynchronously.
     * @param host Cloud host, e.g. "cloud.openindus.com"
     * @return true if the task was started
     */
    bool begin(const char* host);
    bool begin();

    /**
     * @brief Stop the cloud client and disconnect.
     */
    void end(void);

    /**
     * @brief Get the current connection/provisioning state.
     */
    CloudState getState(void) const;

    /**
     * @brief Checks if the device is connected to the cloud.
     * @return true if connected, false otherwise.
     */
    bool isConnected(void) const;

    /**
     * @brief Register a CloudVariable with the Cloud system.
     * @tparam T CloudVariable type
     * @param variable Pointer to a CloudVariable
     */
    template <typename T>
    void addVariable(CloudVariable<T>* variable) { _addVariable(variable); }

    /**
     * @brief Unregister a variable by name.
     * @param name Variable name to unregister
     */
    void unregisterVariable(const std::string& name);

    /**
     * @brief Publish a log message (default "log" variable).
     */
    void log(const std::string& message);

    const std::string& getPlatformToken(void) const { return _platformToken; }
    const std::string& getDeviceUuid(void) const { return _deviceUuid; }
};

#endif // CONFIG_MODULE_MASTER || CONFIG_MODULE_STANDALONE
