#pragma once

#include <string.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionProactiveInterfaces.h"
#include "CANFunctionTimerCheckerInterface.h"

// using timer_type_checker_t = timer_type_t (*)(CANObjectInterface &can_object);

/// @brief CANFunctionTimer is a proactive function that triggers at a specified interval.
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
    CANFunctionTimer() : CANFunctionProactiveInterface(), _last_call(0), _period_ms(0), _is_enabled(false), _default_type(CAN_TIMER_TYPE_NONE), _type_checker(nullptr) {};
    ~CANFunctionTimer() = default;

    /// @brief Checks if the timer is enabled.
    /// @return true if the timer is enabled and the period is greater than zero, false otherwise.
    bool IsEnabled() { return _is_enabled && _period_ms > 0; };

    /// @brief Checks if the timer is disabled.
    /// @note This method returns true if the timer is not enabled or the period is zero
    /// @return true if the timer is disabled, false otherwise.
    bool IsDisabled() { return !this->IsEnabled(); };

    /// @brief Resets the timer function to its initial state.
    ///        This method clears the period, last call time, and disables the timer.
    ///        It also resets the default timer type and the type checker.
    /// @return Reference to the current instance of CANFunctionTimer for method chaining.
    CANFunctionTimer &Reset()
    {
        _period_ms = 0;
        _last_call = 0;
        _is_enabled = false;
        _default_type = CAN_TIMER_TYPE_NONE;
        _type_checker = nullptr; // TODO: точно ли так надо делать?
        return *this;
    };

    /// @brief Enables the timer function.
    /// @return Reference to the current instance of CANFunctionTimer for method chaining.
    CANFunctionTimer &Enable()
    {
        _is_enabled = true;
        return *this;
    };

    /// @brief Disables the timer function.
    /// @return Reference to the current instance of CANFunctionTimer for method chaining.
    CANFunctionTimer &Disable()
    {
        _is_enabled = false;
        return *this;
    };

    /// @brief Sets the default timer type for the timer function.
    ///        This method allows you to specify the timer type that will be used when the timer function is triggered.
    /// @param type - The timer type to set as the default.
    /// @return Reference to the current instance of CANFunctionTimer for method chaining.
    CANFunctionTimer &SetDefaultType(timer_type_t type)
    {
        _default_type = type;
        return *this;
    };

    /// @brief Gets the default timer type for the timer function.
    ///        This method retrieves the timer type that was set as the default using SetDefaultType
    /// @return timer_type_t - The default timer type for the timer function.
    timer_type_t GetDefaultType()
    {
        return _default_type;
    };

    /// @brief Sets the type checker for the timer function.
    ///        This method allows you to specify a type checker.
    ///        The type checker is an object thet implements the CANFunctionTimerCheckerInterface and
    ///        is used to determine the timer type based on the values of the CAN object.
    /// @param type_checker - Pointer to the type checker that implements the CANFunctionTimerCheckerInterface.
    /// @return Reference to the current instance of CANFunctionTimer for method chaining.
    CANFunctionTimer &SetTypeChecker(CANFunctionTimerCheckerInterface *type_checker)
    {
        _type_checker = type_checker;

        return *this;
    };

    /// @brief Sets the period for the timer function.
    ///        This method allows you to specify the interval at which the timer function should be triggered
    /// @param period_ms - The period in milliseconds for the timer function.
    /// @return Reference to the current instance of CANFunctionTimer for method chaining.
    CANFunctionTimer &SetPeriod(uint32_t period_ms)
    {
        _period_ms = period_ms;
        return *this;
    };

    /// @brief Gets the period for the timer function.
    ///        This method retrieves the interval at which the timer function is set to be triggered.
    /// @return uint32_t - The period in milliseconds for the timer function.
    uint32_t GetPeriod()
    {
        return _period_ms;
    };

    /// @brief Processes the timer function.
    ///        This method checks if the timer is enabled and if the current time exceeds the last call time plus the period.
    ///        If so, it retrieves the timer type using the type checker (if set) or uses the default type.
    ///        It then prepares an outgoing CAN frame with the timer type and data from the CAN object.
    /// @param can_object - CAN object that the timer function is associated with.
    /// @param time - Current time in milliseconds.
    /// @param outgoing_can_frame - Outgoing CAN frame that will be filled with the timer data.
    /// @return can_result_t - The result of the processing.
    ///         TODO: Do we actually need can_result now?
    virtual can_result_t Process(CANObjectInterface &can_object, uint32_t time, can_frame_t &outgoing_can_frame) override
    {
        if (this->IsDisabled() || time < _last_call || time - _last_call < _period_ms)
            return CANFunctionProactiveInterface::Process(can_object, time, outgoing_can_frame);

        timer_type_t current_timer_type = this->GetDefaultType();
        if (_type_checker != nullptr)
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
