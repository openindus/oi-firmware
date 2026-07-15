/**
 * @file Cloud.cpp
 * @brief Implementation file for the Cloud API component with MQTT support.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Cloud.hpp"
#include "MQTTManager.hpp"
#include "CloudProvisioning.hpp"
#include "Board.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "nvs.h"
#include "cJSON.h"

// The Cloud library is only available on master and standalone modules
// (a slave has no network stack of its own and is driven over the bus).
#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_STANDALONE)

#if defined(CONFIG_MODULE_MASTER)
#include "Master.h"
#endif

static const char* TAG = "Cloud";

static const char* NVS_NAMESPACE = "oi_cloud";
static const char* NVS_KEY_UUID = "dev_uuid";
static const char* NVS_KEY_TOKEN = "dev_token";
static const char* NVS_KEY_PLATFORM = "plat_uuid";

// Timing (ms)
static const uint32_t PROVISION_RETRY_MS = 5000;
static const uint32_t STATUS_POLL_MS = 5000;
static const uint32_t SERVICE_PERIOD_MS = 100;
static const uint32_t ALREADY_CREATED_WAIT_MS = 10000; // 409: alert + wait before retry
static const uint32_t CONNECT_TIMEOUT_MS = 30000;      // wait for MQTT_EVENT_CONNECTED
static const uint32_t RECONNECT_GRACE_MS = 30000;      // tolerate a mid-session drop
static const uint32_t RECONNECT_WAIT_MS = 5000;        // "Attendre 5s" before START

Cloud::Cloud(const char* platformUuid, const char* platformToken, int projectId)
    : _projectId(projectId)
    , _haveCredentials(false)
    , _state(CloudState::IDLE)
    , _taskHandle(nullptr)
    , _varLog(nullptr)
    , _varVersion(nullptr)
    , _varRestart(nullptr)
    , _varOta(nullptr)
    , _varModules(nullptr)
{
    if (platformUuid) {
        _platformUuid = platformUuid;
    }
    if (platformToken) {
        _platformToken = platformToken;
    }

    // Device name = system MAC address (used as provisioning "name")
    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        char mac_str[18];
        snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        _deviceName = mac_str;
    } else {
        ESP_LOGE(TAG, "Failed to get system MAC address");
        _deviceName = "00:00:00:00:00:00";
    }

    ESP_LOGI(TAG, "Cloud instance created (device name: %s)", _deviceName.c_str());
}

Cloud::~Cloud() {
    end();
    ESP_LOGI(TAG, "Cloud instance destroyed");
}

void Cloud::useDeviceCredentials(const char* uuid, const char* token) {
    if (uuid && token) {
        _deviceUuid = uuid;
        _deviceToken = token;
        _haveCredentials = true;
        ESP_LOGI(TAG, "Using provided device credentials");
    }
}

bool Cloud::begin() {
    return begin("oicloud.openindus.com");
}

bool Cloud::begin(const char* host) {
    if (!host) {
        ESP_LOGE(TAG, "Host is required");
        return false;
    }
    if (_taskHandle) {
        ESP_LOGW(TAG, "Cloud already started");
        return true;
    }

    _host = host;
    _setupDefaultVariables();

    BaseType_t ret = xTaskCreate(_task, "Cloud task", 8192, this, 5, &_taskHandle);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create Cloud task");
        _taskHandle = nullptr;
        return false;
    }
    ESP_LOGI(TAG, "Cloud task started (host: %s)", _host.c_str());
    return true;
}

void Cloud::end(void) {
    if (_taskHandle) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }
    MQTTManager::getInstance()->disconnect();
    _state = CloudState::IDLE;

    // Free owned default variables and drop them from the serviced list
    for (auto* v : _defaultVariables) {
        _variables.erase(std::remove(_variables.begin(), _variables.end(), v), _variables.end());
        delete v;
    }
    _defaultVariables.clear();
    _varLog = _varVersion = _varOta = _varModules = nullptr;
    _varRestart = nullptr;
}

CloudState Cloud::getState(void) const {
    return _state;
}

bool Cloud::isConnected(void) const {
    return _state == CloudState::CONNECTED && MQTTManager::getInstance()->isConnected();
}

/* ------------------------------------------------------------------------- */
/* Variable registration                                                     */
/* ------------------------------------------------------------------------- */

void Cloud::_addVariable(ICloudVariable* var) {
    if (!var) {
        return;
    }
    _variables.push_back(var);
    ESP_LOGI(TAG, "Registered variable: %s", var->getName().c_str());
}

void Cloud::unregisterVariable(const std::string& name) {
    for (auto it = _variables.begin(); it != _variables.end(); ++it) {
        if ((*it)->getName() == name) {
            _variables.erase(it);
            ESP_LOGI(TAG, "Unregistered variable: %s", name.c_str());
            return;
        }
    }
}

std::string Cloud::_topicFor(const ICloudVariable* var) const {
    char prefix = var->getTypePrefix();
    std::string type = (prefix == 'd') ? "def" : std::string(1, prefix);
    return "device/" + _deviceUuid + "/" + type + "/" + var->getName();
}

void Cloud::_subscribeVariable(ICloudVariable* var) {
    if (!var->isSubscriber()) {
        return;
    }
    std::string topic = _topicFor(var);
    MQTTManager::getInstance()->subscribe(topic, [var](const std::string& payload) {
        var->applyPayload(payload);
    });
}

/* ------------------------------------------------------------------------- */
/* Default variables (spec 2.2)                                              */
/* ------------------------------------------------------------------------- */

void Cloud::_setupDefaultVariables(void) {
    if (!_defaultVariables.empty()) {
        return; // already set up
    }

    _varLog = new StringVariable("log", "", UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH);
    _varVersion = new StringVariable("version", "", UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH);
    _varModules = new StringVariable("modules", "", UpdateMethod::ASYNCHRONOUS,UpdateType::PUBLISH);
    _varRestart = new BoolVariable("restart", false, UpdateMethod::ASYNCHRONOUS, UpdateType::SUBSCRIBE);
    _varOta = new StringVariable("ota", "", UpdateMethod::ASYNCHRONOUS, UpdateType::SUBSCRIBE);

    ICloudVariable* defaults[] = {_varLog, _varVersion, _varModules, _varRestart, _varOta};
    for (auto* v : defaults) {
        v->setTypePrefix('d'); // "def" topic type
        _defaultVariables.push_back(v);
        _variables.push_back(v);
    }

    // restart -> reboot the module
    _varRestart->onReceive([](const bool& value) {
        if (value) {
            ESP_LOGW(TAG, "Restart command received, rebooting");
            esp_restart();
        }
    });

    // ota -> parse {version,url} and log (no download in this iteration)
    _varOta->onReceive([](const std::string& payload) {
        cJSON* root = cJSON_Parse(payload.c_str());
        if (root) {
            cJSON* version = cJSON_GetObjectItem(root, "version");
            cJSON* url = cJSON_GetObjectItem(root, "url");
            ESP_LOGI(TAG, "OTA request received - version: %s, url: %s",
                     cJSON_IsString(version) ? version->valuestring : "?",
                     cJSON_IsString(url) ? url->valuestring : "?");
            cJSON_Delete(root);
        } else {
            ESP_LOGW(TAG, "OTA payload is not valid JSON: %s", payload.c_str());
        }
    });

    // Publish the firmware version
    char version[32] = {0};
    Board::getSoftwareVersion(version);
    _varVersion->setValue(std::string(version));

    // Publish the connected modules as a JSON array. Each entry holds the module
    // serial number, its position (bus id) and its software version. The value is
    // set once here so the ASYNCHRONOUS variable is published on connection.
    cJSON* modulesArray = cJSON_CreateArray();

#if defined(CONFIG_MODULE_MASTER)
    // Master: enumerate the modules on the rail and read each one's board info.
    auto slaves = Master::discoverSlaves();
    for (const auto& slave : slaves) {
        uint16_t id = slave.first;             // bus id = position on the rail
        uint16_t boardType = slave.second.first;
        uint32_t boardSN = slave.second.second;

        Board_Info_t info = {};
        Master::getBoardInfo(boardType, boardSN, &info);

        cJSON* module = cJSON_CreateObject();
        cJSON_AddNumberToObject(module, "serial_number", boardSN);
        cJSON_AddNumberToObject(module, "position", id);
        cJSON_AddStringToObject(module, "version", info.software_version);
        cJSON_AddItemToArray(modulesArray, module);
    }
#else
    // Standalone: no bus, just report the device's own information.
    cJSON* module = cJSON_CreateObject();
    cJSON_AddNumberToObject(module, "serial_number", Board::getSerialNum());
    cJSON_AddNumberToObject(module, "position", 0);
    cJSON_AddStringToObject(module, "version", version);
    cJSON_AddItemToArray(modulesArray, module);
#endif

    char* modulesStr = cJSON_PrintUnformatted(modulesArray);
    if (modulesStr) {
        _varModules->setValue(std::string(modulesStr));
        cJSON_free(modulesStr);
    }
    cJSON_Delete(modulesArray);
}

void Cloud::log(const std::string& message) {
    if (_varLog) {
        _varLog->setValue(message);
    }
}

/* ------------------------------------------------------------------------- */
/* NVS credential persistence                                                */
/* ------------------------------------------------------------------------- */

bool Cloud::_loadCredentials(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }

    auto readStr = [&](const char* key, std::string& out) -> bool {
        size_t len = 0;
        if (nvs_get_str(handle, key, nullptr, &len) != ESP_OK || len == 0) {
            return false;
        }
        std::vector<char> buf(len);
        if (nvs_get_str(handle, key, buf.data(), &len) != ESP_OK) {
            return false;
        }
        out = buf.data();
        return true;
    };

    std::string uuid, token, plat;
    bool ok = readStr(NVS_KEY_UUID, uuid) &&
              readStr(NVS_KEY_TOKEN, token) &&
              readStr(NVS_KEY_PLATFORM, plat);
    nvs_close(handle);

    if (!ok) {
        return false;
    }
    if (plat != _platformUuid) {
        ESP_LOGW(TAG, "Stored platform UUID mismatch, re-provisioning required");
        return false;
    }

    _deviceUuid = uuid;
    _deviceToken = token;
    ESP_LOGI(TAG, "Loaded device credentials from NVS (uuid: %s)", _deviceUuid.c_str());
    return true;
}

void Cloud::_saveCredentials(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS to save credentials");
        return;
    }
    nvs_set_str(handle, NVS_KEY_UUID, _deviceUuid.c_str());
    nvs_set_str(handle, NVS_KEY_TOKEN, _deviceToken.c_str());
    nvs_set_str(handle, NVS_KEY_PLATFORM, _platformUuid.c_str());
    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG, "Saved device credentials to NVS");
}

void Cloud::_clearCredentials(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        nvs_erase_key(handle, NVS_KEY_UUID);
        nvs_erase_key(handle, NVS_KEY_TOKEN);
        nvs_erase_key(handle, NVS_KEY_PLATFORM);
        nvs_commit(handle);
        nvs_close(handle);
    } else {
        ESP_LOGW(TAG, "Failed to open NVS to clear credentials");
    }
    _deviceUuid.clear();
    _deviceToken.clear();
    _haveCredentials = false;
    ESP_LOGW(TAG, "Cleared device credentials");
}

/* ------------------------------------------------------------------------- */
/* Background task / state machine                                           */
/* ------------------------------------------------------------------------- */

void Cloud::_task(void* arg) {
    Cloud* self = static_cast<Cloud*>(arg);
    self->_run();
    // _run() only returns on a terminal error state; clear the handle so end()
    // does not delete an already-deleted task.
    self->_taskHandle = nullptr;
    vTaskDelete(nullptr);
}

void Cloud::_run(void) {
    // Load any persisted credentials before entering the provisioning loop.
    // Check state machine at 3-oiproduct\36-OICLOUD\02 - Architecture\recuperation_token
    if (!_haveCredentials) {
        _state = CloudState::LOADING_CREDS;
        if (_loadCredentials()) {
            _haveCredentials = true;
        }
    }

    // Top-level state machine (see the provisioning/reconnection flowchart).
    // Outer loop = START: (re-)provision on every escalation. Inner loop = the MQTT
    // session: connect, serve, and relaunch on a mid-session disconnect. Any connection
    // failure falls through to a wait and back to START, which re-validates via
    // getStatus.
    while (true) {
        if (_ensureProvisioned() == ProvisionResult::REJECTED) {
            return; // terminal: device rejected by the user
        }

        // "Accepté" -> connect, serve, and relaunch MQTT on a mid-session drop.
        while (_connectMqtt(CONNECT_TIMEOUT_MS)) {
            _subscribeAll();
            _state = CloudState::CONNECTED;
            ESP_LOGI(TAG, "Cloud connected");

            _serviceLoop(); // returns only when the link is lost past the grace period

            // "Déconnexion mqtt" -> tear down and loop back to relaunch the connection.
            MQTTManager::getInstance()->disconnect();
        }

        // "Erreur authentification" / connection failed -> wait, then back to START.
        MQTTManager::getInstance()->disconnect();
        _state = CloudState::RECONNECTING;
        ESP_LOGW(TAG, "MQTT connection failed, re-validating provisioning in %u ms",
                 (unsigned)RECONNECT_WAIT_MS);
        vTaskDelay(pdMS_TO_TICKS(RECONNECT_WAIT_MS));
    }
}

/* START subgraph: ensure the device is registered and accepted by the platform. */
Cloud::ProvisionResult Cloud::_ensureProvisioned(void) {
    // Every branch loops back to START (this while condition) except acceptance
    // (returns ACCEPTED) and terminal rejection (returns REJECTED).
    while (true) {
        // START: is the device UUID & token registered?
        bool registered = !_deviceUuid.empty() && !_deviceToken.empty();

        if (!registered) {
            // NO -> createDevice (POST /device)
            _state = CloudState::PROVISION_CREATE;
            DeviceCredentials creds;
            int status = CloudProvisioning::createDevice(_host, _platformUuid, _platformToken,
                                                         _projectId, _deviceName, creds);
            if (status == 201) {
                // Save the information, then back to START
                _deviceUuid = creds.uuid;
                _deviceToken = creds.token;
                _haveCredentials = true;
                _saveCredentials();
            } else if (status == 409) {
                // Device already exists on the platform but our credentials are lost.
                // Alert, wait, then back to START (the user must erase the device).
                ESP_LOGE(TAG, "Device already created on the platform. "
                              "Please erase the device to re-provision. Waiting...");
                _state = CloudState::ERR_ALREADY_CREATED;
                vTaskDelay(pdMS_TO_TICKS(ALREADY_CREATED_WAIT_MS));
            } else {
                ESP_LOGW(TAG, "Device creation failed (status %d), retrying", status);
                vTaskDelay(pdMS_TO_TICKS(PROVISION_RETRY_MS));
            }
            continue; // back to START
        }

        // YES -> Check access (GET /status)
        _state = CloudState::PROVISION_PENDING;
        DeviceStatus st = {false, false};
        int status = CloudProvisioning::getStatus(_host, _platformUuid, _deviceUuid,
                                                  _deviceToken, st);
        if (status == 401) {
            // Device not recognized: clear memory, wait, back to START (-> createDevice)
            ESP_LOGW(TAG, "Device not recognized by the platform (401), clearing credentials");
            _clearCredentials();
            vTaskDelay(pdMS_TO_TICKS(RECONNECT_WAIT_MS));
            continue; // back to START
        } else if (status == 200) {
            if (st.accepted) {
                // pending false/true, accepted true -> accepted, launch MQTT
                _saveCredentials();
                _state = CloudState::CONNECTING;
                return ProvisionResult::ACCEPTED;
            } else if (!st.pending) {
                // pending false, accepted false -> device not accepted, stop the code
                ESP_LOGE(TAG, "Device was rejected by the user");
                _state = CloudState::ERR_REJECTED;
                return ProvisionResult::REJECTED;
            } else {
                // pending true, accepted false -> wait and retry (back to Check access)
                ESP_LOGI(TAG, "Device pending acceptance...");
                vTaskDelay(pdMS_TO_TICKS(STATUS_POLL_MS));
            }
        } else {
            ESP_LOGW(TAG, "Status request failed (status %d), retrying", status);
            vTaskDelay(pdMS_TO_TICKS(STATUS_POLL_MS));
        }
    }
}

/* "Lancer la connexion mqtt": (re)start the client and wait for the broker connection. */
bool Cloud::_connectMqtt(uint32_t timeoutMs) {
    std::string uri = "wss://" + _host + "/mqtt";
    MQTTManager* mqtt = MQTTManager::getInstance();
    if (!mqtt->init(uri.c_str(), _deviceUuid.c_str(), _deviceToken.c_str()) || !mqtt->connect()) {
        ESP_LOGE(TAG, "Failed to start MQTT client");
        return false;
    }

    _state = CloudState::CONNECTING;
    uint32_t start = cloudMillis();
    while (!mqtt->isConnected()) {
        if ((cloudMillis() - start) >= timeoutMs) {
            ESP_LOGW(TAG, "MQTT did not connect within %u ms", (unsigned)timeoutMs);
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(SERVICE_PERIOD_MS));
    }
    return true;
}

/* "Abonnement aux topics": register subscriptions for every serviced variable. */
void Cloud::_subscribeAll(void) {
    for (auto* var : _variables) {
        _subscribeVariable(var);
    }
}

/* SERVE: publish variables per their refresh policy until the link is lost past the
 * grace period. Short blips are healed transparently by esp-mqtt's auto-reconnect. */
void Cloud::_serviceLoop(void) {
    MQTTManager* mqtt = MQTTManager::getInstance();
    uint32_t downSince = 0; // 0 = currently connected

    while (true) {
        uint32_t now = cloudMillis();
        if (mqtt->isConnected()) {
            downSince = 0;
            _state = CloudState::CONNECTED;
            for (auto* var : _variables) {
                if (var->shouldPublish(now)) {
                    mqtt->publish(_topicFor(var), var->serialize());
                }
            }
        } else {
            // "Déconnexion mqtt": let esp-mqtt attempt to relaunch the connection, but
            // escalate to a full reconnection (back to START) if it never recovers.
            _state = CloudState::RECONNECTING;
            if (downSince == 0) {
                downSince = now;
            } else if ((now - downSince) >= RECONNECT_GRACE_MS) {
                ESP_LOGW(TAG, "MQTT down for more than %u ms, re-establishing connection",
                         (unsigned)RECONNECT_GRACE_MS);
                return;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(SERVICE_PERIOD_MS));
    }
}

#endif // CONFIG_MODULE_MASTER || CONFIG_MODULE_STANDALONE
