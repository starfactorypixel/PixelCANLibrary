#pragma once

#include "CanCommon.h"

/// @brief Interface for CAN objects
class CANObjectInterface
{
private:
    can_object_id_t _id = CAN_SYSTEM_ID_BROADCAST;
    can_frame_t *_rx_frame = nullptr;
    can_frame_t *_tx_frame = nullptr;

public:
    CANObjectInterface() = delete;
    CANObjectInterface(can_object_id_t object_id) : _id(object_id) {};
    virtual ~CANObjectInterface() = default;

    can_object_id_t getId() { return _id; };

    CANObjectInterface &setRxFrame(can_frame_t &can_frame)
    {
        _rx_frame = &can_frame;
        return *this;
    };

    CANObjectInterface &setTxFrame(can_frame_t &can_frame)
    {
        _tx_frame = &can_frame;
        return *this;
    };

    virtual uint8_t getDataFieldCount() = 0;
    virtual uint8_t getOneDataFieldSize() = 0;
    virtual void setValue(uint8_t index, void *value, uint8_t value_size) = 0;
    virtual void *getValuePtr(uint8_t index) = 0;

    virtual void process(uint32_t time) = 0;
};
