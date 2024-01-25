#pragma once

#include "esp_log.h"

/// @brief Variable type enum
enum Var_Type {
    Var_Type_Bool = 1,
    Var_Type_Int32 = 4,
    Var_Type_Double = 8,
    Var_Type_String = -1
};

/// @brief Variable Update Mode
enum Var_Mode {
    /// @brief The variable will be updated if the value changes OR if the delay is reached
    Var_Mode_Any,
    /// @brief The variable will be updated only if the value changes
    Var_Mode_ValueChange,
    /// @brief The variable will be updated only if the delay is reached
    Var_Mode_Delay,
    /// @brief The variable will be updated only if the value changes AND if the delay is reached
    Var_Mode_Both
};

/// @brief Variable Base Class
class VarMaster {
    public:
        /// @brief Set update mode
        /// @param mode Var_Mode_Any / Var_Mode_Delay / Var_Mode_ValueChange / Var_Mode_Both
        void setUpdateMode(Var_Mode mode);

        /// @brief Set the refresh delay
        /// @param value seconds (default and minimum 1)
        /// @return 1 on error / 0 on success
        int setRefreshDelay(unsigned int value);

        /// @brief Stop or resume variable update
        /// @param enable True / False
        void setWatchdog(bool enable);

        /// @brief Get the variable's name
        /// @return Name
        const char *getName(void);

    protected:
        VarMaster(const char *name, bool _is_writable);
        char *_name;
        bool _is_writable;
        Var_Type _type;
        VarMaster *_prev;
        VarMaster *_next;
        Var_Mode _mode;
        float _timer;
        unsigned int _refresh_delay;
        bool _state_changed;
        bool _watch_value;
        char *_receive_topic;
        char *_publish_topic;
        void *_callback_data;
        void setTopic(const char *_UUID);
        bool canUpdateValue(float elapsed_time);
        virtual ~VarMaster();
        virtual unsigned char * getDataAsBytes(void)=0;
        virtual unsigned short getDataByteSize(void)=0;
        virtual void triggerCallback(void)=0;
        virtual void updateData(char *data)=0;
        virtual bool deltaCheck(void)=0;
        virtual void resetDeltaTemp(void)=0;
    friend class Cloud;
} typedef VarAny;

/// @brief Variable Class
/// @tparam Type Variable Type
template <typename Type>
class Var: public VarMaster {
    public:
        Var(const char *name, bool is_writable);
        ~Var() override;
        void updateData(char *data) override;

        /// @brief Set update mode
        /// @param mode Var_Mode_Any / Var_Mode_Delay / Var_Mode_ValueChange / Var_Mode_Both
        void setUpdateMode(Var_Mode mode);

        /// @brief Set the refresh delay
        /// @param value in seconds (default and minimum 1)
        /// @return 1 on error / 0 on success
        int setRefreshDelay(unsigned int value);

        /// @brief Stop or resume variable update
        /// @param enable True / False
        void setWatchdog(bool enable);

        /// @brief Get the name of the variable
        /// @return Name
        const char *getName(void);

        /// @brief Get the type of the variable
        /// @return Type
        Var_Type getType(void);

        /// @brief Set variable's value
        /// @param value int32_t value
        void set(int32_t value);
        /// @brief Set variable's value
        /// @param value double value
        void set(double value);
        /// @brief Set variable's value
        /// @param value boolean value
        void set(bool value);
        /// @brief Set variable's value
        /// @param value const char *
        void set(const char *value);

        /// @brief Get the variable's value
        /// @return Value
        Type get(void);

        /// @brief Set value update trigger delta
        /// @param delta Delta
        void setDelta(Type delta);

        /// @brief Set the callback that will be triggered when the variable is updated by the server
        /// @param Callback Callback function
        /// @param data User data to be passed as an arugument (can be NULL)
        void setWriteCallback(void (*Callback)(Var<Type> *variable, void *data), void *data);

        /// @brief Add operator
        Type operator+=(Type value);
        /// @brief Substract operator
        Type operator-=(Type value);
        /// @brief Multiply operator
        Type operator*=(Type value);
        /// @brief Divide operator
        Type operator/=(Type value);
        /// @brief Modulus operator
        Type operator%=(Type value);
        /// @brief Decrement operator
        Type operator--();
        /// @brief Assignment operator
        Type operator=(Type value);
        /// @brief Assignment operator
        Type operator=(Var<Type> &variable);
        /// @brief Equality operator
        bool operator==(Type value);
        /// @brief Inequality operator
        bool operator!=(Type value);
        /// @brief Logical AND operator
        bool operator&&(Type value);
        /// @brief Logical OR operator
        bool operator||(Type value);
        /// @brief Logical NOT operator
        bool operator!();
        operator Type() { return this->get(); }
        Var<Type>* operator()() { return this; };

    private:
        Type _value;
        Type _delta_value;
        Type _delta_temp;
        void (*_write_callback)(Var<Type> *variable, void *data);

        /// @brief Internal use only.
        void setType(void);

        /// @brief Calls the variable's write callback if it exists.
        void triggerCallback(void) override;

        /// @brief Get the variable data as bytes with a timestamp:
        /// - The first byte represents the size in bytes of the time_t structure
        /// - The sizeof(time_t) next bytes represent the current timestamp
        /// - The rest of the bytes represents the raw variable data.
        /// @return Bytes
        unsigned char * getDataAsBytes(void) override;

        /// @brief Return the total number of bytes that getDataAsBytes will return.
        /// @return Number of bytes
        unsigned short getDataByteSize(void) override;

        /// @brief Check if the delta between two value is reached.
        bool deltaCheck(void) override;

        /// @brief Resets the internal delta buffer.
        void resetDeltaTemp(void) override;
};

#include "../src/Var_basics.tpp"
#include "../src/Var_operator.tpp"
