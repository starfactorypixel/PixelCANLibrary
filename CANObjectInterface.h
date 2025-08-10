#pragma once

#include "CAN_common.h"

class CANFunctionReactiveInterface;
class CANFunctionProactiveInterface;

/// @brief Base interface for CAN objects
class CANObjectInterface
{
public:
    virtual ~CANObjectInterface() = default;

    virtual CANFunctionReactiveInterface *RegisterReactiveFunction(CANFunctionReactiveInterface *reactive_function) = 0;
    virtual CANFunctionProactiveInterface *RegisterProactiveFunction(CANFunctionProactiveInterface *proactive_function) = 0;
    virtual can_result_t Process(uint32_t time, can_frame_t &outgoing_can_frame) = 0;
    virtual can_result_t InputCanFrame(const can_frame_t &incoming_can_frame, can_frame_t &outgoing_can_frame) = 0;
    virtual can_object_id_t GetId() = 0;
    virtual uint8_t GetDataFieldCount() = 0;
    virtual uint8_t GetOneDataFieldSize() = 0;
    virtual void SetValue(uint8_t index, void *value, uint8_t value_size) = 0;
    virtual void *GetValuePtr(uint8_t index) = 0;
};
