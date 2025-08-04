#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionTimerCheckerInterface.h"
#include "CANFunctionTimerCheckerBase.h"

/*****************************************************************************************\
 *
 * Timer Checker: High is Critical
 *
 * This class checks the timer type based on the value of the data field.
 * The timer type is determined as follows:
 * - If the value is greater than or equal to the critical boundary, the timer type is CRITICAL.
 * - If the value is greater than or equal to the warning boundary, the timer type is WARNING.
 * - Otherwise, the timer type is NORMAL.
 *
\*****************************************************************************************/
template <typename T>
class CANFunctionTimerChecker_HighIsCritical : public CANFunctionTimerCheckerBase<T>
{
private:
    T _warning_boundary;
    T _critical_boundary;

protected:
    virtual timer_type_t _GetValueType(T value) override
    {
        if (value >= _critical_boundary)
            return timer_type_t::CAN_TIMER_TYPE_CRITICAL;
        else if (value >= _warning_boundary)
            return timer_type_t::CAN_TIMER_TYPE_WARNING;

        return timer_type_t::CAN_TIMER_TYPE_NORMAL;
    };

public:
    CANFunctionTimerChecker_HighIsCritical() = default;
    CANFunctionTimerChecker_HighIsCritical(T warning_lowest_value, T critical_lowest_value)
        : _warning_boundary(warning_lowest_value), _critical_boundary(critical_lowest_value) {
          };

    CANFunctionTimerChecker_HighIsCritical &SetWarningBoundary(T warning_lowest_value)
    {
        _warning_boundary = warning_lowest_value;
        return *this;
    };

    CANFunctionTimerChecker_HighIsCritical &SetCriticalBoundary(T critical_lowest_value)
    {
        _critical_boundary = critical_lowest_value;
        return *this;
    };
};

/*****************************************************************************************\
 *
 * Timer Checker: Low is Critical
 *
 * This class checks the timer type based on the value of the data field.
 * The timer type is determined as follows:
 * - If the value is lower than or equal to the critical boundary, the timer type is CRITICAL.
 * - If the value is lower than or equal to the warning boundary, the timer type is WARNING.
 * - Otherwise, the timer type is NORMAL.
 *
\*****************************************************************************************/
template <typename T>
class CANFunctionTimerChecker_LowIsCritical : public CANFunctionTimerCheckerBase<T>
{
private:
    T _warning_boundary;
    T _critical_boundary;

protected:
    virtual timer_type_t _GetValueType(T value) override
    {
        if (value <= _critical_boundary)
            return timer_type_t::CAN_TIMER_TYPE_CRITICAL;
        else if (value <= _warning_boundary)
            return timer_type_t::CAN_TIMER_TYPE_WARNING;

        return timer_type_t::CAN_TIMER_TYPE_NORMAL;
    };

public:
    CANFunctionTimerChecker_LowIsCritical() = default;
    CANFunctionTimerChecker_LowIsCritical(T critical_highest_value, T warning_highest_value)
        : _warning_boundary(warning_highest_value), _critical_boundary(critical_highest_value) {
          };

    CANFunctionTimerChecker_LowIsCritical &SetWarningBoundary(T warning_highest_value)
    {
        _warning_boundary = warning_highest_value;
        return *this;
    };

    CANFunctionTimerChecker_LowIsCritical &SetCriticalBoundary(T critical_highest_value)
    {
        _critical_boundary = critical_highest_value;
        return *this;
    };
};

/*****************************************************************************************\
 * 
 * Timer Checker: One Normal, All Critical
 * 
 * This class checks the timer type based on the value of the data field.
 * The timer type is determined as follows:
 * - If the value is equal to the normal value, the timer type is NORMAL.
 * - Otherwise, the timer type is CRITICAL.
 *
\*****************************************************************************************/
template <typename T>
class CANFunctionTimerChecker_OneNormalAllCritical : public CANFunctionTimerCheckerBase<T>
{
private:
    T _normal_value;

protected:
    virtual timer_type_t _GetValueType(T value) override
    {
        if (value == _normal_value)
            return timer_type_t::CAN_TIMER_TYPE_NORMAL;

        return timer_type_t::CAN_TIMER_TYPE_CRITICAL;
    };

public:
    CANFunctionTimerChecker_OneNormalAllCritical() = default;
    CANFunctionTimerChecker_OneNormalAllCritical(T normal_value) : _normal_value(normal_value) {};

    CANFunctionTimerChecker_OneNormalAllCritical &SetNormalValue(T normal_value)
    {
        _normal_value = normal_value;
        return *this;
    };
};
