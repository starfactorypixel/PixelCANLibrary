#pragma once

#include <stdint.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionProactiveInterfaces.h"
#include "CANFunctionReactiveInterfaces.h"

class CANObjectBase : public CANObjectInterface
{
private:
    can_object_id_t _id = CAN_SYSTEM_ID_BROADCAST;

    CANFunctionReactiveInterface *_reactive_functions = nullptr;
    CANFunctionProactiveInterface *_proactive_functions = nullptr;

public:
    CANObjectBase() = delete;

    CANObjectBase(can_object_id_t object_id) : _id(object_id) {};

    virtual ~CANObjectBase() = default;

    virtual CANFunctionReactiveInterface *RegisterReactiveFunction(CANFunctionReactiveInterface *reactive_function) override
    {
        if (reactive_function == nullptr)
            return nullptr;

        if (_reactive_functions != nullptr)
            return _reactive_functions->SetNextFunction(reactive_function);

        _reactive_functions = reactive_function;
        return _reactive_functions;
    };

    virtual CANFunctionProactiveInterface *RegisterProactiveFunction(CANFunctionProactiveInterface *proactive_function) override
    {
        if (proactive_function == nullptr)
            return nullptr;

        if (_proactive_functions != nullptr)
            return _proactive_functions->SetNextFunction(proactive_function);

        _proactive_functions = proactive_function;
        return _proactive_functions;
    };

    virtual can_result_t Process(uint32_t time, can_frame_t &outgoing_can_frame) override
    {
        if (_proactive_functions == nullptr)
            return can_result_t::CAN_RESULT_IGNORE;

        return _proactive_functions->Process(*this, time, outgoing_can_frame);
    };

    virtual can_result_t InputCanFrame(const can_frame_t &incoming_can_frame, can_frame_t &outgoing_can_frame) override
    {
        // TODO: надо разобраться с очередью отправки
        if (incoming_can_frame.object_id != CAN_SYSTEM_ID_BROADCAST &&
            incoming_can_frame.object_id != this->GetId())
            return can_result_t::CAN_RESULT_IGNORE;

        if (_reactive_functions == nullptr)
            return can_result_t::CAN_RESULT_IGNORE;

        return _reactive_functions->InputCanFrame(*this, incoming_can_frame, outgoing_can_frame);
    };

    virtual can_object_id_t GetId() override
    {
        return _id;
    };
};
