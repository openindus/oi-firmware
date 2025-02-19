#pragma once

#include "mqtt_client.h"
#include "Var_Private.hpp"

/// @brief Variable Bool
using VarBool = Var<bool>;
/// @brief Variable String
using VarString = Var<char *>;
/// @brief Variable Double
using VarDouble = Var<double>;
/// @brief Variable Int
using VarInt = Var<int32_t>;

class Cloud
{
    public:
        /// @brief Cloud Constructor
        /// @param uuid Device UUID
        /// @param firmware_name Optionnal - Firmware version string
        /// @note The firmware version string should not exceed 15 characters.
        explicit Cloud(const char *uuid, const char *firmware_name = NULL);
        ~Cloud();
        /// @brief Start the cloud client
        /// @warning Make sure the network interface is
        /// initialized before calling this function.
        void init(void);

        /// @brief Update cloud events
        /// @note This function needs to be called frequently, otherwise no
        /// values will be sent to the cloud.
        /// @return true on success, false on error
        bool update(void);

        /// @brief Add a variable to the update list
        /// @param variable Variable to add
        /// @return true on success, false on error
        bool addVar(VarAny *variable);

        /// @brief Remove a variable from the update list
        /// @param variable_name Name of the variable
        /// @param destroy Destroy the variable object (if true the variable must not be used after this call)
        /// @return true on success, false on error
        bool removeVar(char const *variable_name, bool destroy);

        /// @brief Remove a variable from the update list
        /// @param variable Variable instance
        /// @param destroy Destroy the variable object (if true the variable must not be used after this call)
        /// @return true on success, false on error
        bool removeVar(VarAny *variable, bool destroy=false);

        /// @brief Pause cloud update and disconnect from the cloud server
        void pause(void);

        /// @brief Resume cloud update and reconnect to the cloud server
        void resume(void);

        /// @brief Check if the client is initialized
        bool isInit(void);

        /// @brief Check if the client is connected
        bool isConnected(void);

        /// @brief Check if the client is paused
        bool isPaused(void);

        static void mqtt_event_Handler(void *handler_args,esp_event_base_t base,
                                    int32_t event_id, void *event_data);

#if defined(OI_CORE)
        /// @brief (Only on Core) Enable cloud status monitoring via the device LED
        void monitorStatus(void);
#endif

    private:
        SemaphoreHandle_t _lock_mutex;
        bool _is_init;
        bool _is_paused;
        VarMaster *_var_list;
        esp_mqtt_client_config_t _mqtt_cfg;
        clock_t _last_update_time;
        esp_mqtt_client_handle_t _mqtt_client;
        TaskHandle_t *_auth_task;
        TaskHandle_t *_ntp_task;
        bool _monitor_status;
        char *_UUID;
        char _firmware_name[16];
        char _topic_firmware[64];
        bool _is_broker_connected;
        bool _is_authenticated;
        bool _cloudLockAcquire(unsigned int timeout_ms);
        void _forceUpdate(void);
        void _updateSubscriptions(void);
        void _setVarFromServer(char *topic, char *data, int data_len, int topic_len);
        void _publishVar(VarMaster *variable);
        void _cleanConfig(void);
        void _sendDeviceInfo(void);
        void _ota_update(void);
        friend void cloud_authenticate(Cloud *cloud);
        friend void task_authenticate(void *data);
        friend void update_access_token(Cloud *cloud);
};
