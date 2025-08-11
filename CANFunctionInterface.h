#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"

/// @brief Base class for all CAN functions
///        It provides a common interface for all CAN functions.
class CANFunctionInterface
{
private:
    /// @brief Function ID of the CAN function
    can_function_id_t _function_id = can_function_id_t::CAN_FUNC_NONE;
public:
    CANFunctionInterface() : _function_id(can_function_id_t::CAN_FUNC_NONE) {};
    virtual ~CANFunctionInterface() = default;

    /// @brief Sets the function ID of the CAN function
    /// @param function_id  
    /// @return Reference to the current CANFunctionInterface object
    ///         This allows for method chaining.
    CANFunctionInterface &SetFunctionID(can_function_id_t function_id)
    {
        this->_function_id = function_id;
        return *this;
    }

    /// @brief Gets the function ID of the CAN function
    /// @return Function ID of the CAN function
    ///         This is used to identify the function in the CAN frame.
    can_function_id_t GetFunctionID() const
    {
        return this->_function_id;
    }
};
