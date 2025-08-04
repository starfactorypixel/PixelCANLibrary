#pragma once

#include <string.h> //memset
#include "CANObjectBase.h"

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

    virtual uint8_t GetDataFieldCount() override
    {
        return _item_count;
    };

    virtual uint8_t GetOneDataFieldSize() override
    {
        return sizeof(T);
    };

    virtual void SetValue(uint8_t index, void *value, uint8_t value_size) override
    {
        if (value == nullptr || value_size != this->GetOneDataFieldSize())
            return;

        SetValue(index, *(T *)value);
    };

    void SetValue(uint8_t index, T value)
    {
        if (index >= _item_count)
            return;

        _data_fields[index] = value;
    };

    virtual void *GetValuePtr(uint8_t index) override
    {
        if (index >= _item_count)
            return nullptr;

        return (void *)&_data_fields[index];
    };

    T GetValue(uint8_t index)
    {
        if (index >= _item_count)
            return (T)0;

        return _data_fields[index];
    }
};