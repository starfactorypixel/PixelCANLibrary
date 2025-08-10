#pragma once

#include <string.h> //memset
#include "CANObjectBase.h"

/// @brief Template class for CANObject.
///        It is a template class that allows to create CANObjects with a specified data type and
///        a specified number of data fields.
///        It inherits from CANObjectBase and provides methods to set and get values of the data
/// @tparam T - Data type of the data fields in the CANObject
///        It can be any type that fits into the CAN frame (size <= 7 bytes)
/// @tparam _item_count - Number of data fields in the CANObject
///        It must be greater than 0 and the total size of data fields must be less than or equal to 7 bytes
///        It is used to determine how many data fields are
template <typename T, uint8_t _item_count = 1>
class CANObject : public CANObjectBase
{
    static_assert(_item_count > 0);              // 0 data fields isn't allowed
    static_assert(_item_count * sizeof(T) <= 7); // static data size validation (to fit it into the CAN-frame)

private:
    T _data_fields[_item_count] = {0};

public:
    /// @brief Default constructor is forbidden.
    CANObject() = delete;

    CANObject(can_object_id_t id) : CANObjectBase(id)
    {
        memset(_data_fields, 0, _item_count * sizeof(T));
    };

    virtual ~CANObject() = default;

    /// @brief Returns the number of data fields in the CANObject
    ///        This method is used to determine how many data fields are available in the CANObject
    ///        It is used to iterate over the data fields in the CANObject.
    ///        The data fields are used to store the values of the CANObject.
    /// @return uint8_t - Number of data fields in the CANObject
    virtual uint8_t GetDataFieldCount() override
    {
        return _item_count;
    };

    /// @brief Returns the size of one data field in the CANObject
    /// @return uint8_t - Size of one data field in the CANObject 
    virtual uint8_t GetOneDataFieldSize() override
    {
        return sizeof(T);
    };

    /// @brief Sets the value of the data field at the specified index
    ///        The value will be copied to the data field.
    ///        The value size must be equal to the size of one data field.
    ///        If the value size is not equal to the size of one data field, the
    ///        value will not be set and the method will return.
    /// @param index - Index of the data field to set
    /// @param value - Pointer to the value to set
    /// @param value_size - Size of the value to set
    virtual void SetValue(uint8_t index, void *value, uint8_t value_size) override
    {
        if (value == nullptr || value_size != this->GetOneDataFieldSize())
            return;

        SetValue(index, *(T *)value);
    };

    /// @brief Sets the value of the data field at the specified index
    ///        The value will be copied to the data field.
    /// @param index - Index of the data field to set
    /// @param value - Value to set
    void SetValue(uint8_t index, T value)
    {
        if (index >= _item_count)
            return;

        _data_fields[index] = value;
    };

    /// @brief Returns a pointer to the value of the data field at the specified index
    ///        The pointer can be used to get the raw value of the data field.
    /// @param index - Index of the data field to get
    /// @return void* - Pointer to the value of the data field
    ///        If the index is out of bounds, the method will return nullptr.
    virtual void *GetValuePtr(uint8_t index) override
    {
        if (index >= _item_count)
            return nullptr;

        return (void *)&_data_fields[index];
    };

    /// @brief Returns the value of the data field at the specified index
    ///        The value will be returned as a copy.
    /// @param index - Index of the data field to get
    /// @return T - Value of the data field
    ///        If the index is out of bounds, the method will return 0.
    T GetValue(uint8_t index)
    {
        if (index >= _item_count)
            return (T)0;

        return _data_fields[index];
    }
};