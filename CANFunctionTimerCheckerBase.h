#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionTimerCheckerInterface.h"

/*****************************************************************************************\
\*****************************************************************************************/
template <typename T>
class CANFunctionTimerCheckerBase : public CANFunctionTimerCheckerInterface
{
protected:
    /// @brief Determines the timer type based on the value of the data field.
    /// @param value Value of the data field
    /// @return Timer type based on the value of the data field
    virtual timer_type_t _GetValueType(T value) = 0;

    /// @brief Determines the timer type based on the previous timer type and the new one.
    /// @param prev_value Previous timer type
    /// @param new_value New timer type
    /// @return Timer type based on the previous and new values
    timer_type_t _GetCumulativeType(timer_type_t prev_value, timer_type_t new_value)
    {
        if (prev_value == timer_type_t::CAN_TIMER_TYPE_CRITICAL || new_value == timer_type_t::CAN_TIMER_TYPE_CRITICAL)
            return timer_type_t::CAN_TIMER_TYPE_CRITICAL;
        else if (prev_value == timer_type_t::CAN_TIMER_TYPE_WARNING || new_value == timer_type_t::CAN_TIMER_TYPE_WARNING)
            return timer_type_t::CAN_TIMER_TYPE_WARNING;
        else
            return timer_type_t::CAN_TIMER_TYPE_NORMAL;
    };

public:
    virtual ~CANFunctionTimerCheckerBase() = default;

    /// @brief Public method to get the timer type based on the values of CAN object.
    /// @param can_object CAN object to check the values of
    /// @return Timer type based on the values of CAN object
    timer_type_t GetTimerType(CANObjectInterface &can_object) final
    {
        if (can_object.GetOneDataFieldSize() != sizeof(T))
            return timer_type_t::CAN_TIMER_TYPE_NONE;

        timer_type_t timer_type = timer_type_t::CAN_TIMER_TYPE_NORMAL;
        T *value = static_cast<T *>(can_object.GetValuePtr(0));
        for (size_t i = 0; i < can_object.GetDataFieldCount(); i++)
        {
            timer_type = _GetCumulativeType(timer_type, _GetValueType(value[i]));
        }

        return timer_type;
    };
};
