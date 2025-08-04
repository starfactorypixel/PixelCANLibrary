#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"

/*****************************************************************************************\

 These functions work independently of incoming CAN-frames
 
\*****************************************************************************************/
class CANFunctionProactiveInterface
{
private:
    CANFunctionProactiveInterface *_next_proactive_function = nullptr;

public:
    CANFunctionProactiveInterface() : _next_proactive_function(nullptr) {};
    virtual ~CANFunctionProactiveInterface() = default;

    CANFunctionProactiveInterface *SetNextFunction(CANFunctionProactiveInterface *can_proactive_function)
    {
        if (this == can_proactive_function)
            return this;
            
        if (this->_next_proactive_function != nullptr)
            return this->_next_proactive_function->SetNextFunction(can_proactive_function);

        this->_next_proactive_function = can_proactive_function;
        return can_proactive_function;
    };

    virtual can_result_t Process(CANObjectInterface &parent_can_object, uint32_t time, can_frame_t &outgoing_can_frame)
    {
        if (this->_next_proactive_function == nullptr)
            return CAN_RESULT_IGNORE;

        return this->_next_proactive_function->Process(parent_can_object, time, outgoing_can_frame);
    };
};