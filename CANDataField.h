#pragma once

#include <stdint.h>

/******************************************************************************************
 *
 ******************************************************************************************/
class CANDataFieldInterface
{
    using validator_callback_t = data_field_attention_state_t (*)(void *data, uint8_t data_length);

public:
    virtual ~CANDataFieldInterface() = default;

    /// @brief Registers validator for DataField. Validator checks for alarm state of DataField
    /// @param validator Pointer to validator callback function
    virtual void RegisterValidatorCallback(validator_callback_t validator) = 0;

    /// @brief Returns attention state of the data field. Validator will not be called. Validator will be called by Process() method.
    /// @return Returns current attention state of the data field.
    virtual data_field_attention_state_t GetDataFieldAttentionState() = 0;

    /// @brief Process data field data (perform validator call, updates attention state etc)
    virtual void Process() = 0;

    /// @brief  Write data field value to the specified data buffer.
    ///         копируем данные из _data_pointer в data
    ///         подразумевается, что используем этот метод для формирования данных CAN-фрейма
    /// @param data Pointer to the data buffer for writing.
    /// @param max_data_length Maximum data size, which can be received by data buffer.
    /// @return 'true' if data writing was successful, 'false' if not.
    virtual bool WriteData(uint8_t *data, uint8_t max_data_length) = 0;
};

/******************************************************************************************
 *
 ******************************************************************************************/
template <typename T>
class CANDataField : public CANDataFieldInterface
{
public:
    CANDataField() = delete;

    CANDataField(T &data) : _data_pointer(&data){};

    virtual ~CANDataField() = default;

    /// @brief Registers validator for DataField. Validator checks for alarm state of DataField
    /// @param validator Pointer to validator callback function
    virtual void RegisterValidatorCallback(validator_callback_t validator) override{};

    /// @brief Returns attention state of the data field. Validator will not be called. Validator will be called by Process() method.
    /// @return Returns current attention state of the data field.
    virtual data_field_attention_state_t GetDataFieldAttentionState() override
    {
        return _attention_state;
    };

    /// @brief Process data field data (perform validator call, updates attention state etc)
    virtual void Process() override
    {
        if (_validator == nullptr || _data_pointer == nullptr)
            return;

        _attention_state = _validator(_data_pointer, sizeof(T));
    };

    /// @brief Write data field value to the specified data buffer.
    /// @param data Pointer to the data buffer for writing.
    /// @param max_data_length Maximum data size, which can be received by data buffer.
    /// @return 'true' if data writing was successful, 'false' if not.
    virtual bool WriteData(uint8_t *data, uint8_t max_data_length)
    {
        return true;
    };

private:
    T *_data_pointer = nullptr;
    validator_callback_t _validator = nullptr;
    data_field_attention_state_t _attention_state = DF_ATTENTION_STATE_NONE;
};
