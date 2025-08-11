#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionInterface.h"

/// @brief CANFunctionProactiveInterface is an interface for proactive functions
///        It is used to process CAN frames proactively, without waiting for incoming frames.
///        It is used to implement periodic tasks or tasks that should be executed at a specific time
class CANFunctionProactiveInterface : public CANFunctionInterface
{
private:
    /// @brief Pointer to the next proactive function in the chain of responsibility
    ///        It is used to pass the CAN frame to the next function in the chain if the current function cannot handle it.
    ///        If this pointer is nullptr, it means that there are no more functions in the chain,
    ///        and the current function should handle the CAN frame
    /// @note This pointer is set by the SetNextFunction() method.
    CANFunctionProactiveInterface *_next_proactive_function = nullptr;

public:
    CANFunctionProactiveInterface() : CANFunctionInterface(), _next_proactive_function(nullptr) {};
    virtual ~CANFunctionProactiveInterface() = default;

    /// @brief Sets next proactive function in the chain of responsibility
    /// @param can_proactive_function - Pointer to the proactive function which should be added to the end of chain
    /// @return Pointer to the proactive function so man can link functions in a convenient way
    CANFunctionProactiveInterface *SetNextFunction(CANFunctionProactiveInterface *can_proactive_function)
    {
        if (this == can_proactive_function)
            return this;

        if (this->_next_proactive_function != nullptr)
            return this->_next_proactive_function->SetNextFunction(can_proactive_function);

        this->_next_proactive_function = can_proactive_function;
        return can_proactive_function;
    };

    /// @brief Processes all the functions in the chain of responsibility
    ///        It is called by the CANObjectInterface::Process() method.
    /// @param parent_can_object - CAN object which processes proactive functions
    /// @param time - Current time in milliseconds
    ///        It is used to implement periodic tasks or tasks that should be executed at a specific time
    /// @param outgoing_can_frame - Outgoing CAN frame
    ///        It is used to send data to the CAN bus
    /// @return can_result_t - Result of the processing
    ///         TODO: Do we actually need can_result now?
    virtual can_result_t Process(CANObjectInterface &parent_can_object, uint32_t time, can_frame_t &outgoing_can_frame)
    {
        if (this->_next_proactive_function == nullptr)
            return CAN_RESULT_IGNORE;

        return this->_next_proactive_function->Process(parent_can_object, time, outgoing_can_frame);
    };
};