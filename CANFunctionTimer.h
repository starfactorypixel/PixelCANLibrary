#pragma once

#include <string.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionProactiveInterfaces.h"
#include "CANFunctionTimerCheckerInterface.h"

/*****************************************************************************************\

 TIMER function: proactive function

\*****************************************************************************************/
// using timer_type_checker_t = timer_type_t (*)(CANObjectInterface &can_object);

class CANFunctionTimer : public CANFunctionProactiveInterface
{
private:
    uint32_t _last_call = 0;
    uint32_t _period_ms = 0;
    bool _is_enabled = false;

    timer_type_t _default_type = CAN_TIMER_TYPE_NONE;
    // timer_type_checker_t _type_checker = nullptr;
    CANFunctionTimerCheckerInterface *_type_checker = nullptr;

public:
    bool IsEnabled() { return _is_enabled && _period_ms > 0; };
    bool IsDisabled() { return !this->IsEnabled(); };

    CANFunctionTimer &Reset()
    {
        _period_ms = 0;
        _last_call = 0;
        _is_enabled = false;
        _default_type = CAN_TIMER_TYPE_NONE;
        _type_checker = nullptr; // TODO: точно ли так надо делать?
        return *this;
    };

    CANFunctionTimer &Enable()
    {
        _is_enabled = true;
        return *this;
    };

    CANFunctionTimer &Disable()
    {
        _is_enabled = false;
        return *this;
    };

    CANFunctionTimer &SetDefaultType(timer_type_t type)
    {
        _default_type = type;
        return *this;
    };

    timer_type_t GetDefaultType()
    {
        return _default_type;
    };

    /*
    CANFunctionTimer &SetTypeChecker(timer_type_checker_t type_checker)
    {
        _type_checker = type_checker;

        return *this;
    };
    */

    CANFunctionTimer &SetTypeChecker(CANFunctionTimerCheckerInterface *type_checker)
    {
        _type_checker = type_checker;

        return *this;
    };

    CANFunctionTimer &SetPeriod(uint32_t period_ms)
    {
        _period_ms = period_ms;
        return *this;
    };

    uint32_t GetPeriod()
    {
        return _period_ms;
    };

    virtual can_result_t Process(CANObjectInterface &can_object, uint32_t time, can_frame_t &outgoing_can_frame) override
    {
        if (this->IsDisabled() || time < _last_call || time - _last_call < _period_ms)
            return CANFunctionProactiveInterface::Process(can_object, time, outgoing_can_frame);

        timer_type_t current_timer_type = this->GetDefaultType();
        if (_type_checker != nullptr)
            // current_timer_type = _type_checker(can_object);
            current_timer_type = _type_checker->GetTimerType(can_object);

        if (CAN_TIMER_TYPE_NONE == current_timer_type)
            return CANFunctionProactiveInterface::Process(can_object, time, outgoing_can_frame);

        clear_can_frame_struct(outgoing_can_frame);
        outgoing_can_frame.initialized = true;
        outgoing_can_frame.object_id = can_object.GetId();
        outgoing_can_frame.function_id = (can_function_id_t)current_timer_type;
        uint8_t data_size = can_object.GetOneDataFieldSize() * can_object.GetDataFieldCount();
        if (data_size > CAN_FRAME_MAX_PAYLOAD)
            data_size = can_object.GetOneDataFieldSize();
        outgoing_can_frame.raw_data_length = sizeof(outgoing_can_frame.function_id) + data_size;
        memcpy(outgoing_can_frame.data, can_object.GetValuePtr(0), data_size);
        _last_call = time;

        return CAN_RESULT_CAN_FRAME;
    };
};
