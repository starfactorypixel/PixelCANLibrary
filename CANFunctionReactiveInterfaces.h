#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"

/*****************************************************************************************\

 These functions respond to incoming CAN-frames
 
\*****************************************************************************************/
class CANFunctionReactiveInterface
{
private:
    CANFunctionReactiveInterface *_next_reactive_function = nullptr;

public:
    CANFunctionReactiveInterface() : _next_reactive_function(nullptr) {};
    virtual ~CANFunctionReactiveInterface() = default;

    /// @brief Sets next function in the chain of responsibility
    /// @param can_reactive_function Pointer to the reactive function which should be added to the end of chain
    /// @return Pointer to the reactive function so man can link functions in a convenient way
    CANFunctionReactiveInterface *SetNextFunction(CANFunctionReactiveInterface *can_reactive_function)
    {
        if (this == can_reactive_function)
            return this;
            
        if (this->_next_reactive_function != nullptr)
            return this->_next_reactive_function->SetNextFunction(can_reactive_function);

        this->_next_reactive_function = can_reactive_function;
        return can_reactive_function;
    };

    /// @brief Processes the CAN frame
    /// @param parent_can_object CAN object which received a CAN frame
    /// @param incoming_can_frame Incoming CAN frame
    /// @param outgoing_can_frame Outgoing CAN frame
    /// @return TODO: Do we actually need can_result now?
    virtual can_result_t InputCanFrame(CANObjectInterface &parent_can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame)
    {
        if (this->_next_reactive_function == nullptr)
        {
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION);
            outgoing_can_frame.object_id = parent_can_object.GetId();
            return CAN_RESULT_CAN_FRAME;
        }

        return this->_next_reactive_function->InputCanFrame(parent_can_object, incoming_can_frame, outgoing_can_frame);
    };
};
