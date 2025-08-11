#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"

/// @brief CANFunctionTimerCheckerInterface is an interface for checking the timer type of a CAN object.
///        It provides a method to get the timer type based on the current state of the CAN object.
///        The timer type can be NORMAL, WARNING, or CRITICAL.
///        The GetTimerType method is a public method that calls the protected method of derived classes
///        and returns the timer type. It is the only method that should be called from outside the class.
class CANFunctionTimerCheckerInterface
{
private:
    /// @brief Determines the most critical timer type of two timer types.
    ///        This method is used to combine two timer types into one.
    /// @param value1 First timer type
    /// @param value2 Second timer type
    /// @return Timer type based on the two timer type values
    timer_type_t _GetCumulativeTimerType(timer_type_t value1, timer_type_t value2)
    {
        if (value1 == timer_type_t::CAN_TIMER_TYPE_CRITICAL || value2 == timer_type_t::CAN_TIMER_TYPE_CRITICAL)
            return timer_type_t::CAN_TIMER_TYPE_CRITICAL;
        else if (value1 == timer_type_t::CAN_TIMER_TYPE_WARNING || value2 == timer_type_t::CAN_TIMER_TYPE_WARNING)
            return timer_type_t::CAN_TIMER_TYPE_WARNING;
        else
            return timer_type_t::CAN_TIMER_TYPE_NORMAL;
    };

public:
    virtual ~CANFunctionTimerCheckerInterface() = default;

    /// @brief Public method to get the timer type based on the values of CAN object.
    /// @param can_object CAN object to check the values of
    /// @return Timer type based on the values of CAN object
    virtual timer_type_t GetTimerType(CANObjectInterface &can_object) = 0;
};
