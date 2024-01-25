#pragma once

template <typename Type>
void Var<Type>::setWriteCallback(void (*Callback)(Var<Type> *variable, void *data), void *data)
{
    this->_write_callback = Callback;
    this->_callback_data = data;
}

template <typename Type>
Var_Type Var<Type>::getType(void)
{
    return (this->_type);
}

template <typename Type>
Type Var<Type>::get(void)
{
    return (this->_value);
}

template <typename Type>
void Var<Type>::setDelta(Type delta)
{
    if (this->_type == Var_Type_String || this->_type == Var_Type_Bool) {
        ESP_LOGW("Cloud", "Var::setDelta(): Cannot set delta for string and boolean variables");
        return;
    }
    this->_delta_value = delta;
}

template <typename Type>
Var<Type>::Var(const char *name, bool is_writable): VarMaster(name, is_writable)
{
    this->_is_writable = is_writable;
    this->setWriteCallback(NULL, NULL);
    this->setType();
    this->_state_changed = true;
    this->_value = 0;
    memset(&this->_delta_value, 0, sizeof(Type));
    memset(&this->_delta_temp, 0, sizeof(Type));
}

template <typename Type>
bool Var<Type>::deltaCheck(void)
{
    if (this->_type == Var_Type_String)
        return (true);
    if (this->_type == Var_Type_Bool)
        return (this->_delta_temp != this->_delta_value);
    return (this->_delta_temp >= this->_delta_value);
}

template <typename Type>
void Var<Type>::resetDeltaTemp(void)
{
    this->_delta_temp = 0;
}

template <typename Type>
void Var<Type>::triggerCallback(void)
{
    if (this->_write_callback != NULL)
        this->_write_callback(this, this->_callback_data);
}

template<typename Type>
unsigned short Var<Type>::getDataByteSize(void)
{
    return (sizeof(time_t) + sizeof(Type) + 1);
}

template<typename Type>
unsigned char * Var<Type>::getDataAsBytes(void)
{
    unsigned char *buffer = NULL;
    time_t timestamp = time(NULL);
    const short data_size = sizeof(time_t) + sizeof(Type) + 1;

    buffer = (unsigned char *) malloc(data_size);
    buffer[0] = sizeof(time_t);
    if (buffer == NULL)
        return (NULL);
    memcpy(buffer + 1, &timestamp, sizeof(time_t));
    memcpy(buffer + sizeof(time_t) + 1, (const void *) &this->_value, sizeof(Type));
    return (buffer);
}