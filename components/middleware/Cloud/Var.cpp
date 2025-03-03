#include "string.h"
#include "esp_log.h"
#include "Cloud.hpp"
#include "Cloud_Private.hpp"

/// @brief VarMaster constructor
/// @param name Variable name
/// @param is_writable Enables control from server
VarMaster::VarMaster(const char *name, bool is_writable)
{
    if (name == NULL) {
        ESP_LOGE(Cloud_LogTag, "VarMaster::VarMaster(): Arugument name can't be NULL");
        abort();
    }
    this->_name = strdup(name);
    if (this->_name == NULL) {
        ESP_LOGE(Cloud_LogTag, "VarMaster::VarMaster(): Variable name allocation failed");
        abort();
    }
    this->_is_writable = is_writable;
    this->_prev = NULL;
    this->_next = NULL;
    this->_receive_topic = NULL;
    this->_publish_topic = NULL;
    this->_state_changed = false;
    this->_timer = 0.0f;
    this->_refresh_delay = 1;
    this->_watch_value = true;
    this->_callback_data = NULL;
    this->_mode = Var_Mode_ValueChange;
}

/// @brief VarMaster Destructor
VarMaster::~VarMaster()
{
    if (this->_name != NULL)
        free(_name);
    if (this->_receive_topic != NULL)
        free(this->_receive_topic);
    if (this->_publish_topic != NULL)
        free(this->_publish_topic);
}

const char *VarMaster::getName(void)
{
    return (this->_name);
}

/// @brief Sets variable publish topic
/// @param UUID Device ID
void VarMaster::setTopic(const char *UUID)
{
    unsigned short topic_size = 0;

    if (this->_receive_topic != NULL)
        free(this->_receive_topic);
    topic_size = strlen(UUID) + strlen(this->_name) + 10;
    this->_receive_topic = (char *) malloc(sizeof(char) * topic_size);
    memset(this->_receive_topic, 0, topic_size);
    strcpy(this->_receive_topic, UUID);
    strcat(this->_receive_topic, this->_name);
    strcat(this->_receive_topic, Mqtt_Topic_Receive_Suffix);
    if (this->_publish_topic != NULL)
        free(this->_publish_topic);
    topic_size = strlen(UUID) + strlen(this->_name) + 10;
    this->_publish_topic = (char *) malloc(sizeof(char) * topic_size);
    memset(this->_publish_topic, 0, topic_size);
    strcpy(this->_publish_topic, UUID);
    strcat(this->_publish_topic, this->_name);
    strcat(this->_publish_topic, Mqtt_Topic_Send_Suffix);
}

int VarMaster::setRefreshDelay(unsigned int value)
{
    if (value < 1) {
        ESP_LOGE(Cloud_LogTag, "VarMaster::setRefreshDelay() : Not able to set value equal to 0");
        return (1);
    } else {
        this->_refresh_delay = value;
        return (0);
    }
}

void VarMaster::setWatchdog(bool enable)
{
    this->_watch_value = enable;
}

void VarMaster::setUpdateMode(Var_Mode mode)
{
    this->_mode = mode;
}

/// @brief Checks if the variable needs to be published
/// @param elapsed_time Elapsed time since the last call
/// @return true or false
bool VarMaster::canUpdateValue(float elapsed_time)
{
    if (this->_watch_value == false)
        return (false);
    this->_timer += elapsed_time;
    switch (this->_mode) {
    case Var_Mode_Both:
        if ((this->_state_changed == true && this->deltaCheck() == true) &&
            this->_timer >= this->_refresh_delay)
            return (true);
        break;
    case Var_Mode_Delay:
        if (this->_timer >= this->_refresh_delay)
            return (true);
        break;
    case Var_Mode_ValueChange:
        if (this->_state_changed == true && this->deltaCheck() == true)
            return (true);
        break;
    case Var_Mode_Any:
        if ((this->_state_changed == true && this->deltaCheck() == true) ||
                this->_timer >= this->_refresh_delay)
                return (true);
            break;
    default:
        break;
    }
    return (false);
}

template <>
Var<int32_t>::~Var(void) { return; }

template <>
Var<bool>::~Var(void) { return; }

template <>
Var<double>::~Var(void){ return; }

template <>
Var<char *>::~Var(void)
{
    if (this->_value != NULL)
        free(this->_value);
}

template <>
void Var<int32_t>::set(int32_t value)
{
    if (this->_value != value) {
        this->_state_changed = true;
        this->_delta_temp += abs(value - this->get());
    }
    this->_value = value;
}

template <>
void Var<bool>::set(bool value)
{
    if (this->_value != value) {
        this->_state_changed = true;
        this->_delta_temp = 1;
    }
    this->_value = value;
}

template <>
void Var<double>::set(double value)
{
    if (this->_value != value) {
        this->_state_changed = true;
        this->_delta_temp += abs(value - this->get());
    }
    this->_value = value;
}

template <>
void Var<char *>::set(const char *value)
{
    if (value == NULL)
        return;
    if (this->_value != NULL) {
        if (strcmp(this->_value, value) != 0)
            this->_state_changed = true;
        free(this->_value);
    } else {
        this->_state_changed = true;
    }
    this->_value = strdup(value);
}


template <>
void Var<int32_t>::setType(void)
{
    this->_type = Var_Type_Int32;
}

template <>
void Var<bool>::setType(void)
{
    this->_type = Var_Type_Bool;
}

template <>
void Var<double>::setType(void)
{
    this->_type = Var_Type_Double;
}

template <>
void Var<char *>::setType(void)
{
    this->_type = Var_Type_String;
}

template<>
unsigned short Var<char *>::getDataByteSize(void)
{
    if (this->_value == NULL)
        return (sizeof(time_t) + 1);
    return (sizeof(time_t) + strlen(this->_value) + 1);
}

template<>
unsigned char * Var<char *>::getDataAsBytes(void)
{
    unsigned char *buffer = NULL;
    time_t timestamp = time(NULL);
    if (this->_value == NULL)
        return (NULL);
    const short string_length = strlen(this->_value);

    buffer = (unsigned char *) malloc(sizeof(time_t) + string_length + 1);
    buffer[0] = sizeof(time_t);
    if (buffer == NULL)
        return (NULL);
    memcpy(buffer + 1, &timestamp, sizeof(time_t));
    memcpy(buffer + sizeof(time_t) + 1, (const void *) this->_value, string_length);
    return (buffer);
}

template <>
void Var<int32_t>::updateData(char *data)
{
    this->set((int32_t) strtol(data, NULL, 10));
}

template <>
void Var<bool>::updateData(char *data)
{
    this->set((strtol(data, NULL, 10) == 0) ? false : true);
}

template <>
void Var<double>::updateData(char *data)
{
    this->set(strtod(data, NULL));
}

template <>
void Var<char *>::updateData(char *data)
{
    this->set(data);
}
