#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"

/*****************************************************************************************\
 *
 * CANFunctionTimerCheckerInterface
 *
 * This is an interface for checking the timer type of a CAN object.
 * It provides a method to get the timer type based on the current state of the CAN object.
 * The timer type can be NORMAL, WARNING, or CRITICAL.
 *
 * The GetTimerType method is a public method that calls the ProtectedTimerTypeChecker method
 * and returns the timer type. It is the only method that should be called from outside the class.
 *
\*****************************************************************************************/
class CANFunctionTimerCheckerInterface
{
public:
    virtual ~CANFunctionTimerCheckerInterface() = default;

    /// @brief Public method to get the timer type based on the values of CAN object.
    /// @param can_object CAN object to check the values of
    /// @return Timer type based on the values of CAN object
    virtual timer_type_t GetTimerType(CANObjectInterface &can_object) = 0;
};
