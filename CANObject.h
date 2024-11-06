#pragma once

#include <stdint.h>
#include <string.h>
#include "CAN_common.h"

/******************************************************************************************
 *
 ******************************************************************************************/
class CANObjectInterface
{
public:
    virtual ~CANObjectInterface() = default;

    /// @brief Registers an external handler for events. It will be called when event occurs.
    /// @param event_handler Pointer to the event handler.
    /// @param error_delay_ms Delay for the error evends in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionEvent(event_handler_t event_handler, uint16_t error_delay_ms) = 0;

    /// @brief Registers an external handler for events. It will be called when event occurs.
    /// @param event_handler Pointer to the event handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionEvent(event_handler_t event_handler) = 0;

    /// @brief Sets the value of error events resending delay.
    /// @param delay_ms Delay for the error evends in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetErrorEventDelay(uint16_t delay_ms) = 0;

    /// @brief Sets the hardware dependent error code.
    /// @param error_code Error code to set.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetHardwareErrorCode(error_code_hardware_t error_code) = 0;

    /// @brief Checks whether the external event function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionEvent() = 0;

    /// @brief Registers an external handler for set commands. It will be called when set command comes.
    /// @param set_handler Pointer to the set command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionSet(set_handler_t set_handler) = 0;

    /// @brief Checks whether the external set function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionSet() = 0;

    /// @brief Register an external handler for set real-time commands. It will be called when set_realtime command comes.
    /// @param set_realtime_handler Pointer to the set real-time external handler.
    /// @param error_handler Pointer to the external error handler
    /// @param data_interval_ms The interval between frames in milliseconds.
    /// @param data_zero_point The data zero point.
    /// @param is_silent 'true' if the object is 'slave' and it is just listening the CAN bus. 'false' in case the object is 'master' and it is sending real-time data.
    /// @param frames_can_lost The number of frames which can be lost before the object generates an error.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionSetRealtime(set_realtime_handler_t set_realtime_handler, set_realtime_error_handler_t error_handler, uint16_t data_interval_ms,
                                                            void *data_zero_point, bool is_silent = true, uint8_t frames_can_lost = 3) = 0;

    /// @brief Register an external handler for set real-time commands. It will be called when set_realtime command comes.
    /// @param set_realtime_handler Pointer to the set real-time external handler.
    /// @param error_handler Pointer to the external error handler
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionSetRealtime(set_realtime_handler_t set_realtime_handler, set_realtime_error_handler_t error_handler) = 0;

    /// @brief Sets the interval between CAN frames in milliseconds for real-time data.
    /// @param data_interval_ms The interval in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetRealtimeDataInterval(uint16_t data_interval_ms) = 0;

    /// @brief Returns real-time data interval of the object.
    /// @return Real-time data interval.
    virtual uint16_t GetRealtimeDataInterval() = 0;

    /// @brief Sets zero point for real-time data.
    /// @param data_zero_point Pointer to the data zero point.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetRealtimeZeroPoint(void *data_zero_point) = 0;

    /// @brief Returns zero point of the real-time object.
    /// @return Pointer to the real-time zero point.
    virtual void *GetRealtimeZeroPoint() = 0;

    /// @brief Sets the number of CAN frames which can be lost before the silent object generates an error.
    /// @param frames_can_lost The number of CAN frames.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetRealtimeFramesCanLost(uint8_t frames_can_lost) = 0;

    /// @brief Returns a number of CAN framse that can be lost by the silent listener object before it falls into the error state.
    /// @return The number of CAN frames that can be lost.
    virtual uint8_t GetRealtimeFramesCanLost() = 0;

    /// @brief Checks whether the external set real-time function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionSetRealtime() = 0;

    /// @brief Checks the error state of silent real-time object
    /// @return 'true' if object is silent and it is in error state
    virtual bool HasRealtimeError() = 0;

    /// @brief Resets the error state of the silent object
    virtual void ResetRealtimeErrorState() = 0;

    /// @brief Checks whether the real-time function is stopped. The sender object is stoppet if current value is in zero-point. The silent listener object becomes stopped after receiving zero-point value.
    /// @return 'true' if real-time object is stopped.
    virtual bool DoesRealtimeStopped() = 0;

    /// @brief Returns last real-time CAN frame received or sended by the object.
    /// @return Last real-time CAN frame ID.
    virtual uint8_t GetRealtimeLastFrameId() = 0;

    /// @brief Registers an external handler for timer. It will be called when timer occurs.
    /// @param timer_handler Pointer to the timer handler.
    /// @param period_ms Timer's period in milliseconds.
    /// @param flood_mode 'true' for work in flood mode: timer will send frame every period regardless of actual data updates
    ///                   'false' for work in frame limit mode: timer will send frames every period when the data was changed; but not more often than actual data updates.
    ///                   Example #1: timer in the frame limit mode, period is 250 ms, data updates every 30 ms, frame will be sent every 250 ms.
    ///                   Example #2: timer in the frame limit mode, period is 250 ms, data updates every 800 ms, frame will be sent every 800 ms.
    ///                   Example #3: timer in the flood mode, period is 250 ms, data was updated once on boot, frame will be sent every 250 ms.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionTimer(timer_handler_t timer_handler, uint16_t period_ms, bool flood_mode = false) = 0;

    /// @brief Registers an external handler for timer. It will be called when timer occurs.
    /// @param timer_handler Pointer to the timer handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionTimer(timer_handler_t timer_handler) = 0;

    /// @brief Sets the value of timer's period.
    /// @param period_ms Timer's period in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetTimerPeriod(uint16_t period_ms) = 0;

    /// @brief Specify the mode of the timer (flood or frame limit)
    /// @param flood_mode 'true' for work in flood mode: timer will send frame every period regardless of actual data updates
    ///                   'false' for work in frame limit mode: timer will send frames every period when the data was changed; but not more often than actual data updates.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetTimerFloodMode(bool flood_mode) = 0;

    /// @brief Checks whether the external timer function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionTimer() = 0;

    /// @brief Registers an external handler for lock commands. It will be called when lock command comes.
    /// @param lock_handler Pointer to the lock command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionLock(lock_handler_t lock_handler) = 0;

    /// @brief Checks whether the external lock function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionLock() = 0;

    /// @brief Registers an external handler for request commands. It will be called when request command comes.
    /// @param request_handler Pointer to the request command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionRequest(request_handler_t request_handler) = 0;

    /// @brief Checks whether the external request function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionRequest() = 0;

    /// @brief Registers an external handler for toggle commands. It will be called when toggle command comes.
    /// @param toggle_handler Pointer to the toggle command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionToggle(toggle_handler_t toggle_handler) = 0;

    /// @brief Checks whether the external toggle function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionToggle() = 0;

    /// @brief Registers an external handler for action commands. It will be called when action command comes.
    /// @param action_handler Pointer to the action command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionAction(action_handler_t action_handler) = 0;

    /// @brief Checks whether the external action function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionAction() = 0;

    /// @brief Sets type of object.
    /// @param object_type type of the object ot set.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetObjectType(object_type_t object_type) = 0;

    /// @brief Performs CANObjects processing
    /// @param time Current time
    /// @param can_frame [OUT] CAN frame for storing the outgoing data
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of CANObject processing (should we send any CAN frames or not)
    virtual can_result_t Process(uint32_t time, can_frame_t &can_frame, can_error_t &error) = 0;

    /// @brief Process incoming CAN frame
    /// @param can_frame [OUT] CAN frame for processing
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of incoming can frame processing (should we send any CAN frames or not)
    virtual can_result_t InputCanFrame(can_frame_t &can_frame, can_error_t &error) = 0;

    /// @brief Fills CAN frame from the object with specified data
    /// @param can_frame [OUT] CAN frame for processing
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @param function_id [IN] CAN function ID
    /// @param data [IN] Frame data to send in CAN frame
    /// @param data_length [IN] Frame data length
    /// @return The result of incoming can frame processing (should we send any CAN frames or not)
    virtual can_result_t FillRawCanFrame(can_frame_t &can_frame, can_error_t &error, can_function_id_t function_id, uint8_t *data = nullptr, uint8_t data_length = 0) = 0;

    /// @brief Returns CANObject ID
    /// @return Returns CANObject ID
    virtual can_object_id_t GetId() = 0;

    /// @brief Returns the value of error events resending delay.
    /// @return Delay for the error evends in milliseconds.
    virtual uint16_t GetErrorEventDelay() = 0;

    /// @brief Returns the value of timer's period.
    /// @return Timer's period in milliseconds.
    virtual uint16_t GetTimerPeriod() = 0;

    /// @brief Return the timer's mode.
    /// @return 'true' if timer works in flood mode, 'false' if timer works in frame limit mode.
    virtual bool IsTimerInFloodMode() = 0;

    /// @brief Checks whether the data has been updated by SetValue() since the last frame was sent.
    /// @return 'true' if there is new data.
    virtual bool DoesTimerHaveNewData() = 0;

    /// @brief Returns the type of the object.
    /// @return Type code of the object.
    virtual object_type_t GetObjectType() = 0;

    /// @brief Checks if the object is the system one.
    /// @return 'true' if the object is the system one (not ordinary).
    virtual bool IsObjectTypeSystem() = 0;

    /// @brief Checks if the object is ordinary.
    /// @return 'true' it the object is ordinary.
    virtual bool IsObjectTypeOrdinary() = 0;

    /// @brief Checks if the object is silent.
    /// @return 'true' it the object is silent.
    virtual bool IsObjectTypeSilent() = 0;

    /// @brief Checks if the object type is unknown.
    /// @return 'true' if the object type is unknown.
    virtual bool IsObjectTypeUnknown() = 0;

    /// @brief Returns the current lock level of the object.
    /// @return Lock level code of the object.
    virtual lock_func_level_t GetLockLevel() = 0;

    /// @brief Returns number of data fields in the CANObject
    /// @return Returns number of data fields in the CANObject
    virtual uint8_t GetDataFieldCount() = 0;

    /// @brief Returns size of the CANObject's one data field item
    /// @return Returns size of the CANObject's one data field item
    virtual uint8_t GetOneDataFieldSize() = 0;

    /// @brief Universal setter for CANObject's data fields
    /// @param index Index of data field to set. If the index is out of range, nothing will be done.
    /// @param value Pointer to the variable with data. The size of data depends of CANObject.
    /// @param timer_type The type of value for timer. With this we can specify is value normal, in warning range or in critical range.
    /// @param event_type The type of value for event. With this we can specify whether an event and what kind of event it is.
    virtual void SetValue(uint8_t index, void *value,
                          timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                          event_type_t event_type = CAN_EVENT_TYPE_NONE) = 0;

    /// @brief Universal getter for CANObject's data fields
    /// @param index Index of data field to get value from. If the index is out of range, nullpointer will be returned.
    /// @return Pointer to the data field value. If the index is out of range, nullpointer will be returned.
    virtual void *GetValuePtr(uint8_t index) = 0;
};

/******************************************************************************************
 *
 ******************************************************************************************/
template <typename T, uint8_t _item_count = 1>
class CANObject : public CANObjectInterface
{
    static_assert(_item_count > 0);              // 0 data fields isn't allowed
    static_assert(_item_count * sizeof(T) <= 7); // static data size validation (to fit it into the can frame)
public:
    /// @brief Default constructor is forbidden.
    CANObject() = delete;

    /// @brief Constructor of the CANObject
    /// @param id ID of the object
    /// @param timer_period_ms Period of a timer function. If CAN_TIMER_DISABLED then timer is disabled
    /// @param error_period_ms Repeation delay of an error event. If CAN_ERROR_DISABLED then error event is disabled
    /// @param flood_mode 'true' for work in flood mode: timer will send frame every period regardless of actual data updates
    ///                   'false' for work in frame limit mode: timer will send frames every period when the data was changed; but not more often than actual data updates.
    ///                   Example #1: timer in the frame limit mode, period is 250 ms, data updates every 30 ms, frame will be sent every 250 ms.
    ///                   Example #2: timer in the frame limit mode, period is 250 ms, data updates every 800 ms, frame will be sent every 800 ms.
    ///                   Example #3: timer in the flood mode, period is 250 ms, data was updated once on boot, frame will be sent every 250 ms.
    /// @param object_type The type of the object. By default, it is set to CAN_OBJECT_TYPE_ORDINARY, which means that the object isn't a system one.
    CANObject(can_object_id_t id,
              uint16_t timer_period_ms = CAN_TIMER_DISABLED, uint16_t error_period_ms = CAN_ERROR_DISABLED,
              bool flood_mode = false, object_type_t object_type = CAN_OBJECT_TYPE_ORDINARY)
        : _id(id), _timer_period(timer_period_ms), _error_period(error_period_ms), _flood_mode(flood_mode), _object_type(object_type)
    {
        ClearDataFields();
    };

    virtual ~CANObject() = default;

    /// @brief Clears all data fields and related structures.
    void ClearDataFields()
    {
        memset(_data_fields, 0, _item_count * sizeof(T));
        memset(_states_of_data_fields, 0, _item_count * sizeof(T));
    }

    /// @brief Timer type checker with upper limits
    /// @param value Current object value
    /// @param max_norm Upper limit of the normal value range
    /// @param max_warn Upper limit of the warning value range
    /// @return Type of the timer for specified value
    static inline timer_type_t TernaryExDown(T value, T max_norm, T max_warn)
    {
        if (value <= max_norm)
            return CAN_TIMER_TYPE_NORMAL;
        if (value <= max_warn)
            return CAN_TIMER_TYPE_WARNING;
        return CAN_TIMER_TYPE_CRITICAL;
    }

    /// @brief Timer type checker with lower limits
    /// @param value Current object value
    /// @param min_norm Lower limit of the normal value range
    /// @param min_warn Lower limit of the warning value range
    /// @return Type of the timer for specified value
    static inline timer_type_t TernaryExUp(T value, T min_norm, T min_warn)
    {
        if (value >= min_norm)
            return CAN_TIMER_TYPE_NORMAL;
        if (value >= min_warn)
            return CAN_TIMER_TYPE_WARNING;
        return CAN_TIMER_TYPE_CRITICAL;
    }

    /// @brief Registers an external handler for events. It will be called when event occurs.
    /// @param event_handler Pointer to the event handler.
    /// @param error_delay_ms Delay for the error evends in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionEvent(event_handler_t event_handler, uint16_t error_delay_ms) override
    {
        RegisterFunctionEvent(event_handler);
        SetErrorEventDelay(error_delay_ms);

        return *this;
    };

    /// @brief Registers an external handler for events. It will be called when event occurs.
    /// @param event_handler Pointer to the event handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionEvent(event_handler_t event_handler) override
    {
        _event_handler = event_handler;

        return *this;
    };

    /// @brief Sets the value of error events resending delay.
    /// @param delay_ms Delay for the error evends in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetErrorEventDelay(uint16_t delay_ms) override
    {
        _error_period = delay_ms;

        return *this;
    };

    /// @brief Sets the hardware dependent error code.
    /// @param error_code Error code to set.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetHardwareErrorCode(error_code_hardware_t error_code) override
    {
        _error_code_hardware = error_code;

        return *this;
    }

    /// @brief Checks whether the external event function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionEvent() override
    {
        return _event_handler != nullptr;
    };

    /// @brief Registers an external handler for set commands. It will be called when set command comes.
    /// @param set_handler Pointer to the set command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionSet(set_handler_t set_handler) override
    {
        _set_handler = set_handler;

        return *this;
    };

    /// @brief Checks whether the external set function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionSet() override
    {
        return _set_handler != nullptr;
    };

    /// @brief Register an external handler for set realtime commands. It will be called when set_realtime command comes.
    /// @param set_realtime_handler Pointer to the set realtime external handler.
    /// @param error_handler Pointer to the external error handler.
    /// @param data_interval_ms The interval between frames in milliseconds.
    /// @param data_zero_point The data zero point.
    /// @param is_silent 'true' if the object is 'slave' and it is just listening the CAN bus. 'false' in case the object is 'master' and it is sending realtime data.
    /// @param frames_can_lost The number of frames which can be lost before the object generates an error.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionSetRealtime(set_realtime_handler_t set_realtime_handler, set_realtime_error_handler_t error_handler, uint16_t data_interval_ms,
                                                            void *data_zero_point, bool is_silent = true, uint8_t frames_can_lost = 3) override
    {
        RegisterFunctionSetRealtime(set_realtime_handler, error_handler);
        SetRealtimeDataInterval(data_interval_ms);
        SetRealtimeZeroPoint(data_zero_point);
        SetRealtimeFramesCanLost(frames_can_lost);
        if (is_silent)
        {
            SetObjectType(CAN_OBJECT_TYPE_SILENT);
            _realtime_stopped = true;
        }

        return *this;
    };

    /// @brief Register an external handler for set realtime commands. It will be called when set_realtime command comes.
    /// @param set_realtime_handler Pointer to the set realtime external handler.
    /// @param error_handler Pointer to the external error handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionSetRealtime(set_realtime_handler_t set_realtime_handler, set_realtime_error_handler_t error_handler) override
    {
        _set_realtime_handler = set_realtime_handler;
        _set_realtime_error_handler = error_handler;

        return *this;
    };

    /// @brief Sets the interval between CAN frames in milliseconds for realtime data.
    /// @param data_interval_ms The interval in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetRealtimeDataInterval(uint16_t data_interval_ms) override
    {
        _realtime_frame_interval = data_interval_ms;

        return *this;
    };

    /// @brief Returns real-time data interval of the object.
    /// @return Real-time data interval.
    virtual uint16_t GetRealtimeDataInterval() override
    {
        return _realtime_frame_interval;
    };

    /// @brief Sets zero point for real-time data.
    /// @param data_zero_point Pointer to the data zero point.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetRealtimeZeroPoint(void *data_zero_point) override
    {
        _realtime_zero_point = *(T *)data_zero_point;

        return *this;
    };

    /// @brief Returns zero point of the real-time object.
    /// @return Pointer to the real-time zero point.
    virtual void *GetRealtimeZeroPoint() override
    {
        return &_realtime_zero_point;
    };

    /// @brief Sets the number of CAN frames which can be lost before the object generates an error.
    /// @param frames_can_lost The number of CAN frames.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetRealtimeFramesCanLost(uint8_t frames_can_lost) override
    {
        _realtime_frames_can_lost = frames_can_lost;

        return *this;
    }

    /// @brief Returns a number of CAN framse that can be lost by the silent listener object before it falls into the error state.
    /// @return The number of CAN frames that can be lost.
    virtual uint8_t GetRealtimeFramesCanLost() override
    {
        return _realtime_frames_can_lost;
    };

    /// @brief Checks whether the external set realtime function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionSetRealtime() override
    {
        return _set_realtime_handler != nullptr && _set_realtime_error_handler != nullptr;
    };

    /// @brief Checks error state of silent real-time object
    /// @return 'true' if object is silent and it is in error state
    virtual bool HasRealtimeError() override
    {
        return /*IsObjectTypeSilent() &&*/ _realtime_has_error;
    };

    /// @brief Resets the error state of the silent object
    virtual void ResetRealtimeErrorState() override
    {
        _realtime_has_error = false;
        _realtime_silent_should_ignore_frame_id_once = true;
    };

    /// @brief Checks whether the real-time function is stopped. The sender object is stoppet if current value is in zero-point. The silent listener object becomes stopped after receiving zero-point value.
    /// @return 'true' if real-time object is stopped.
    virtual bool DoesRealtimeStopped() override
    {
        return _realtime_stopped;
    };

    /// @brief Returns last real-time CAN frame received or sended by the object.
    /// @return Last real-time CAN frame ID.
    virtual uint8_t GetRealtimeLastFrameId() override
    {
        return _realtime_frame_id;
    };

    /// @brief Registers an external handler for timer. It will be called when timer occurs.
    /// @param timer_handler Pointer to the timer handler.
    /// @param period_ms Timer's period in milliseconds.
    /// @param flood_mode 'true' for work in flood mode: timer will send frame every period regardless of actual data updates
    ///                   'false' for work in frame limit mode: timer will send frames every period when the data was changed; but not more often than actual data updates.
    ///                   Example #1: timer in the frame limit mode, period is 250 ms, data updates every 30 ms, frame will be sent every 250 ms.
    ///                   Example #2: timer in the frame limit mode, period is 250 ms, data updates every 800 ms, frame will be sent every 800 ms.
    ///                   Example #3: timer in the flood mode, period is 250 ms, data was updated once on boot, frame will be sent every 250 ms.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionTimer(timer_handler_t timer_handler, uint16_t period_ms, bool flood_mode = false) override
    {
        RegisterFunctionTimer(timer_handler);
        SetTimerPeriod(period_ms);
        SetTimerFloodMode(flood_mode);

        return *this;
    };

    /// @brief Registers an external handler for timer. It will be called when timer occurs.
    /// @param timer_handler Pointer to the timer handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionTimer(timer_handler_t timer_handler) override
    {
        _timer_handler = timer_handler;

        return *this;
    };

    /// @brief Sets the value of timer's period.
    /// @param period_ms Timer's period in milliseconds.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetTimerPeriod(uint16_t period_ms) override
    {
        _timer_period = period_ms;

        return *this;
    };

    /// @brief Specify the mode of the timer (flood or frame limit)
    /// @param flood_mode 'true' for work in flood mode: timer will send frame every period regardless of actual data updates
    ///                   'false' for work in frame limit mode: timer will send frames every period when the data was changed; but not more often than actual data updates.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetTimerFloodMode(bool flood_mode) override
    {
        _flood_mode = flood_mode;

        return *this;
    };

    /// @brief Checks whether the external timer function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionTimer() override
    {
        return _timer_handler != nullptr;
    };

    /// @brief Registers an external handler for lock commands. It will be called when lock command comes.
    /// @param lock_handler Pointer to the lock command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionLock(lock_handler_t lock_handler) override
    {
        _lock_handler = lock_handler;

        return *this;
    };

    /// @brief Checks whether the external lock function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionLock() override
    {
        return _lock_handler != nullptr;
    };

    /// @brief Registers an external handler for request commands. It will be called when request command comes.
    /// @param request_handler Pointer to the request command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionRequest(request_handler_t request_handler) override
    {
        _request_handler = request_handler;

        return *this;
    };

    /// @brief Checks whether the external request function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionRequest() override
    {
        return _request_handler != nullptr;
    };

    /// @brief Registers an external handler for toggle commands. It will be called when toggle command comes.
    /// @param toggle_handler Pointer to the toggle command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionToggle(toggle_handler_t toggle_handler) override
    {
        _toggle_handler = toggle_handler;

        return *this;
    };

    /// @brief Checks whether the external toggle function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionToggle() override
    {
        return _toggle_handler != nullptr;
    };

    /// @brief Registers an external handler for action commands. It will be called when action command comes.
    /// @param action_handler Pointer to the action command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionAction(action_handler_t action_handler) override
    {
        _action_handler = action_handler;

        return *this;
    };

    /// @brief Checks whether the external action function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionAction() override
    {
        return _action_handler != nullptr;
    };

    /// @brief Sets type of object.
    /// @param object_type type of the object ot set.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &SetObjectType(object_type_t object_type) override
    {
        _object_type = object_type;

        return *this;
    };

    /// @brief Performs processing of CANObjects
    /// @param time Current time
    /// @param can_frame [OUT] CAN frame for storing the outgoing data
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of CANObject processing (should we send any CAN frames or not)
    virtual can_result_t Process(uint32_t time, can_frame_t &can_frame, can_error_t &error) override
    {
        // Check data timeout for real-time silent (listener) objects
        if (IsObjectTypeSilent())
        {
            if (HasExternalFunctionSetRealtime() &&
                !DoesRealtimeStopped() &&
                _realtime_frame_interval > 0 &&
                time - _last_realtime_frame_time >= _realtime_frame_interval * _realtime_frames_can_lost)
            {
                _realtime_has_error = true;
                _set_realtime_error_handler(time - _last_realtime_frame_time);
                _realtime_stopped = true;
            }
            return CAN_RESULT_IGNORE; // all other functions are ignored for silent objects
        }

        timer_type_t max_timer_type = CAN_TIMER_TYPE_NONE;
        event_type_t max_event_type = CAN_EVENT_TYPE_NONE;
        for (uint8_t i = 0; i < _item_count; i++)
        {
            if ((_states_of_data_fields[i] & CAN_TIMER_TYPE_MASK) > max_timer_type)
                max_timer_type = (timer_type_t)(_states_of_data_fields[i] & (uint8_t)CAN_TIMER_TYPE_MASK);

            if ((_states_of_data_fields[i] & CAN_EVENT_TYPE_MASK) > max_event_type)
                max_event_type = (event_type_t)(_states_of_data_fields[i] & (uint8_t)CAN_EVENT_TYPE_MASK);
        }
        if (max_event_type > CAN_EVENT_TYPE_NONE &&
            max_event_type != CAN_EVENT_TYPE_ERROR &&
            _error_code_hardware > 0)
        {
            // clear hardware error code if all field's error levels are not CAN_EVENT_TYPE_ERROR
            _error_code_hardware = 0;
        }

        can_result_t handler_result = CAN_RESULT_IGNORE;

        clear_can_frame_struct(can_frame);
        if (_realtime_frame_interval > 0 &&
            !DoesRealtimeStopped() &&
            time - _last_realtime_frame_time >= _realtime_frame_interval)
        {
            // Automatic sending of real-time data by sender object
            handler_result = _PrepareRealtimeCanFrame(can_frame, error);
            if (handler_result == CAN_RESULT_CAN_FRAME)
            {
                _last_realtime_frame_time = time;
                if (_realtime_zero_point == GetValue(0))
                {
                    _realtime_stopped = true;
                }
            }
        }
        else if (max_event_type == CAN_EVENT_TYPE_NORMAL)
        {
            // CAN_EVENT_TYPE_NORMAL should be sent immediately, we don't need to check the time
            if (HasExternalFunctionEvent())
            {
                handler_result = _event_handler(can_frame, max_event_type, error);
            }
            else
            {
                handler_result = _PrepareEventCanFrame(max_event_type, can_frame, error);
            }

            // we need to flush the NORMAL event state of all data fields
            for (uint8_t i = 0; i < _item_count; i++)
            {
                if ((_states_of_data_fields[i] & CAN_EVENT_TYPE_MASK) == CAN_EVENT_TYPE_NORMAL)
                    _states_of_data_fields[i] = (_states_of_data_fields[i] & (uint8_t)CAN_TIMER_TYPE_MASK) | CAN_EVENT_TYPE_NONE;
            }
        }
        else if (max_event_type > CAN_EVENT_TYPE_NORMAL && _error_period != CAN_ERROR_DISABLED)
        {
            // error flood prevention
            if (time - _last_event_time >= _error_period)
            {
                if (HasExternalFunctionEvent())
                {
                    handler_result = _event_handler(can_frame, max_event_type, error);
                }
                else
                {
                    handler_result = _PrepareEventCanFrame(max_event_type, can_frame, error);
                }
                _last_event_time = time;
            }
        }
        else if (max_timer_type != CAN_TIMER_TYPE_NONE && _timer_period != CAN_TIMER_DISABLED && time - _last_timer_time >= _timer_period)
        {
            if (DoesTimerHaveNewData() || IsTimerInFloodMode())
            {
                if (HasExternalFunctionTimer())
                {
                    handler_result = _timer_handler(can_frame, max_timer_type, error);
                }
                else
                {
                    handler_result = _PrepareTimerCanFrame(max_timer_type, can_frame, error);
                }
                _last_timer_time = time;
                _has_new_data = false;
            }
        }

        return handler_result;
    };

    /// @brief Process incoming CAN frame
    /// @param can_frame CAN frame for processing
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    /// @param time Current time. If time not specified CAN bus listeners can't measure realtime data timeout.
    /// @return The result of incoming can frame processing (should we send any CAN frames or not)
    virtual can_result_t InputCanFrame(can_frame_t &can_frame, can_error_t &error) override
    {
        if (!can_frame.initialized)
        {
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_BAD_INCOMING_CAN_FRAME;
            error.function_id = CAN_FUNC_EVENT_ERROR;
            return CAN_RESULT_ERROR;
        }

        if (_IsLockedForFunction(can_frame.function_id))
        {
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_LOCKED;
            error.function_id = CAN_FUNC_EVENT_ERROR;
            return CAN_RESULT_ERROR;
        }

        can_result_t handler_result = CAN_RESULT_ERROR;

        switch (can_frame.function_id)
        {
        case CAN_FUNC_SET_IN:
            if (HasExternalFunctionSet())
            {
                handler_result = _set_handler(can_frame, error);
            }
            else
            {
                handler_result = CAN_RESULT_ERROR;
                can_frame.initialized = false;
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_SET_FUNCTION_IS_MISSING;
                error.function_id = CAN_FUNC_EVENT_ERROR;
            }
            break;

        case CAN_FUNC_TOGGLE_IN:
            if (HasExternalFunctionToggle())
            {
                if (can_frame.raw_data_length == 1)
                {
                    handler_result = _toggle_handler(can_frame, error);
                }
                else
                {
                    handler_result = CAN_RESULT_ERROR;
                    can_frame.initialized = false;
                    error.error_section = ERROR_SECTION_CAN_OBJECT;
                    error.error_code = ERROR_CODE_OBJECT_TOGGLE_COMMAND_FRAME_SHOULD_NOT_HAVE_DATA;
                    error.function_id = CAN_FUNC_EVENT_ERROR;
                }
            }
            else
            {
                handler_result = CAN_RESULT_ERROR;
                can_frame.initialized = false;
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_TOGGLE_FUNCTION_IS_MISSING;
                error.function_id = CAN_FUNC_EVENT_ERROR;
            }
            break;

        case CAN_FUNC_ACTION_IN:
            if (HasExternalFunctionAction())
            {
                if (can_frame.raw_data_length == 1)
                {
                    handler_result = _action_handler(can_frame, error);
                }
                else
                {
                    handler_result = CAN_RESULT_ERROR;
                    can_frame.initialized = false;
                    error.error_section = ERROR_SECTION_CAN_OBJECT;
                    error.error_code = ERROR_CODE_OBJECT_ACTION_COMMAND_FRAME_SHOULD_NOT_HAVE_DATA;
                    error.function_id = CAN_FUNC_EVENT_ERROR;
                }
            }
            else
            {
                handler_result = CAN_RESULT_ERROR;
                can_frame.initialized = false;
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_ACTION_FUNCTION_IS_MISSING;
                error.function_id = CAN_FUNC_EVENT_ERROR;
            }
            break;

        case CAN_FUNC_SET_REAL_TIME_IN:
            // By default sender objects shouldn't react to this type of frames, so default result is CAN_RESULT_IGNORE
            handler_result = CAN_RESULT_IGNORE;
            if (IsObjectTypeSilent() && HasExternalFunctionSetRealtime() && !HasRealtimeError())
            {
                if (can_frame.raw_data_length > 2 &&
                    (can_frame.data[0] == _realtime_frame_id + 1 || _realtime_silent_should_ignore_frame_id_once))
                {
                    _last_realtime_frame_time = can_frame.time_ms;
                    _realtime_silent_should_ignore_frame_id_once = false;
                    _realtime_frame_id = can_frame.data[0];
                    T data = *(T *)&can_frame.data[1];
                    SetValue(0, data);
                    handler_result = _set_realtime_handler(can_frame, error);
                    if (data == *(T*)GetRealtimeZeroPoint())
                    {
                        _realtime_stopped = true;
                    }
                }
            }
            break;

        case CAN_FUNC_LOCK_IN:
            if (can_frame.raw_data_length != 2)
            {
                handler_result = CAN_RESULT_ERROR;
                can_frame.initialized = false;
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_LOCK_COMMAND_FRAME_DATA_LENGTH_ERROR;
                error.function_id = CAN_FUNC_LOCK_OUT_ERR;
            }
            else if (!_IsItKnownLockLevel((lock_func_level_t)can_frame.data[0]))
            {
                handler_result = CAN_RESULT_ERROR;
                can_frame.initialized = false;
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_LOCK_LEVEL_IS_UNKNOWN;
                error.function_id = CAN_FUNC_LOCK_OUT_ERR;
            }
            else
            {
                // save lock level because can frame may be rewrited by handler
                lock_func_level_t specified_lock_level = (lock_func_level_t)can_frame.data[0];
                if (HasExternalFunctionLock())
                {
                    handler_result = _lock_handler(can_frame, error);
                }
                else
                {
                    handler_result = _PrepareRawCanFrame(can_frame, error, CAN_FUNC_LOCK_OUT_OK, &specified_lock_level, 1);
                }

                // if handler was successful then we need to save specified lock level
                if (handler_result == CAN_RESULT_CAN_FRAME)
                {
                    _lock_level = specified_lock_level;
                }
            }
            break;

        case CAN_FUNC_REQUEST_IN:
            if (HasExternalFunctionRequest())
            {
                handler_result = _request_handler(can_frame, error);
            }
            else
            {
                handler_result = _PrepareRequestCanFrame(can_frame, error);
            }
            break;

        case CAN_FUNC_SYSTEM_REQUEST_IN:
            handler_result = _PrepareSystemRequestCanFrame(can_frame, error);
            break;

        default:
            handler_result = CAN_RESULT_ERROR;
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION;
            error.function_id = CAN_FUNC_EVENT_ERROR;
            break;
        }
        // restoring ID in case an external handler has overwritten it
        can_frame.object_id = GetId();

        if (!can_frame.initialized && error.error_section == ERROR_SECTION_NONE && handler_result != CAN_RESULT_IGNORE)
        {
            handler_result = CAN_RESULT_ERROR;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_INCORRECT_FUNCTION_WORKFLOW;
            if (error.function_id == CAN_FUNC_NONE)
                error.function_id = CAN_FUNC_EVENT_ERROR;
        }

        return handler_result;
    };

    /// @brief Fills CAN frame from the object with specified data
    /// @param can_frame [OUT] CAN frame for filling
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @param function_id [IN] CAN function ID
    /// @param data [IN] Frame data to send in CAN frame
    /// @param data_length [IN] Frame data length
    /// @return The result of incoming can frame processing (should we send any CAN frames or not)
    virtual can_result_t FillRawCanFrame(can_frame_t &can_frame, can_error_t &error, can_function_id_t function_id, uint8_t *data = nullptr, uint8_t data_length = 0) override
    {
        return _PrepareRawCanFrame(can_frame, error, function_id, data, data_length);
    };

    /// @brief Returns CANObject ID
    /// @return Returns CANObject ID
    virtual can_object_id_t GetId() override
    {
        return _id;
    };

    /// @brief Returns the value of error events resending delay.
    /// @return Delay for the error evends in milliseconds.
    virtual uint16_t GetErrorEventDelay() override
    {
        return _error_period;
    };

    /// @brief Returns the value of timer's period.
    /// @return Timer's period in milliseconds.
    virtual uint16_t GetTimerPeriod() override
    {
        return _timer_period;
    };

    /// @brief Return the timer's mode.
    /// @return 'true' if timer works in flood mode, 'false' if timer works in frame limit mode.
    virtual bool IsTimerInFloodMode() override
    {
        return _flood_mode;
    };

    /// @brief Checks whether the data has been updated by SetValue() since the last frame was sent.
    /// @return 'true' if there is new data.
    virtual bool DoesTimerHaveNewData() override
    {
        return _has_new_data;
    };

    /// @brief Returns the type of the object.
    /// @return Type code of the object.
    virtual object_type_t GetObjectType() override
    {
        return _object_type;
    };

    /// @brief Checks if the object is the system one.
    /// @return 'true' if the object is the system one (not ordinary).
    virtual bool IsObjectTypeSystem() override
    {
        return (GetObjectType() == CAN_OBJECT_TYPE_SYSTEM_BLOCK_INFO) ||
               (GetObjectType() == CAN_OBJECT_TYPE_SYSTEM_BLOCK_HEALTH) ||
               (GetObjectType() == CAN_OBJECT_TYPE_SYSTEM_BLOCK_FEATURES) ||
               (GetObjectType() == CAN_OBJECT_TYPE_SYSTEM_BLOCK_ERROR);
    };

    /// @brief Checks if the object is ordinary.
    /// @return 'true' it the object is ordinary.
    virtual bool IsObjectTypeOrdinary() override
    {
        return GetObjectType() == CAN_OBJECT_TYPE_ORDINARY;
    };

    /// @brief Checks if the object is silent.
    /// @return 'true' it the object is silent.
    virtual bool IsObjectTypeSilent() override
    {
        return GetObjectType() == CAN_OBJECT_TYPE_SILENT;
    };

    /// @brief Checks if the object type is unknown.
    /// @return 'true' if the object type is unknown.
    virtual bool IsObjectTypeUnknown() override
    {
        return GetObjectType() == CAN_OBJECT_TYPE_UNKNOWN;
    };

    /// @brief Returns the current lock level of the object.
    /// @return Lock level code of the object.
    virtual lock_func_level_t GetLockLevel() override
    {
        return _lock_level;
    };

    /// @brief Returns number of data fields in the CANObject
    /// @return Returns number of data fields in the CANObject
    virtual uint8_t GetDataFieldCount() override
    {
        return _item_count;
    };

    /// @brief Returns size of the CANObject's one data field item
    /// @return Returns size of the CANObject's one data field item
    virtual uint8_t GetOneDataFieldSize() override
    {
        return sizeof(T);
    };

    /// @brief Universal setter for CANObject's data fields
    /// @param index Index of data field to set. If the index is out of range, nothing will be done.
    /// @param value Pointer to the variable with data. The size of data depends of CANObject.
    /// @param timer_type The type of value for timer. With this we can specify is value normal, in warning range or in critical range.
    /// @param event_type The type of value for event. With this we can specify whether an event and what kind of event it is.
    virtual void SetValue(uint8_t index, void *value,
                          timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                          event_type_t event_type = CAN_EVENT_TYPE_NONE) override
    {
        if (value == nullptr)
            return;

        SetValue(index, *(T *)value, timer_type, event_type);
    };

    /// @brief The variation of SetValue() method with typed value parameter.
    /// @param index Index of data field to set. If the index is out of range, nothing will be done.
    /// @param value The value to set to the specified data field.
    /// @param timer_type The type of value for timer. With this we can specify is value normal, in warning range or in critical range.
    /// @param event_type The type of value for event. With this we can specify whether an event and what kind of event it is.
    void SetValue(uint8_t index, T value,
                  timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                  event_type_t event_type = CAN_EVENT_TYPE_NONE)
    {
        if (index >= _item_count)
            return;

        _data_fields[index] = value;
        _states_of_data_fields[index] = timer_type | event_type;
        _has_new_data = true;

        // TODO: it is ugly =( Refactoring needed!
        if (_realtime_frame_interval > 0)
        {
            // This affects both silent objects and sender objects
            if (value != _realtime_zero_point && DoesRealtimeStopped())
            {
                _realtime_stopped = false;
            }
        }
    };

    /// @brief Universal getter for CANObject's data fields
    /// @param index Index of data field to get value from. If the index is out of range, nullpointer will be returned.
    /// @return Pointer to the data field value. If the index is out of range, nullpointer will be returned.
    virtual void *GetValuePtr(uint8_t index) override
    {
        if (index >= _item_count)
            return nullptr;

        return (void *)&_data_fields[index];
    };

    /// @brief The variation of GetValue() method, which returns typed value.
    /// @param index Index of data field to get value from.
    /// @return The value of the specified data field. If the index is out of range, zero value will be returned.
    T GetValue(uint8_t index)
    {
        if (index >= _item_count)
            return (T)0;

        return _data_fields[index];
    }

private:
    can_object_id_t _id = 0;

    // local data storage
    T _data_fields[_item_count] = {0};
    uint8_t _states_of_data_fields[_item_count] = {0};

    uint32_t _last_timer_time = 0;
    uint32_t _last_event_time = 0;
    uint32_t _last_realtime_frame_time = 0;
    uint8_t _realtime_frame_id = 0;
    bool _realtime_silent_should_ignore_frame_id_once = false;

    uint16_t _timer_period = CAN_TIMER_DISABLED;
    uint16_t _error_period = CAN_ERROR_DISABLED;
    error_code_hardware_t _error_code_hardware = 0;

    // uint16_t _realtime_frame_interval = CAN_REALTIME_DISABLED;
    uint16_t _realtime_frame_interval = 0;
    bool _realtime_stopped = false;

    bool _flood_mode = false;
    bool _has_new_data = false;

    T _realtime_zero_point = 0;
    uint8_t _realtime_frames_can_lost = 0;
    bool _realtime_has_error = false;

    object_type_t _object_type = CAN_OBJECT_TYPE_UNKNOWN;
    lock_func_level_t _lock_level = CAN_LOCK_LEVEL_UNLOCKED;

    event_handler_t _event_handler = nullptr;
    set_handler_t _set_handler = nullptr;
    set_realtime_handler_t _set_realtime_handler = nullptr;
    set_realtime_error_handler_t _set_realtime_error_handler = nullptr;
    timer_handler_t _timer_handler = nullptr;
    lock_handler_t _lock_handler = nullptr;
    request_handler_t _request_handler = nullptr;
    toggle_handler_t _toggle_handler = nullptr;
    action_handler_t _action_handler = nullptr;

    /// @brief Check if specified lock level is known
    /// @param lock_code The lock level to check
    /// @return 'true' if lock level is known; 'false' if not
    static bool _IsItKnownLockLevel(const lock_func_level_t lock_level)
    {
        return (lock_level == CAN_LOCK_LEVEL_UNLOCKED ||
                lock_level == CAN_LOCK_LEVEL_PARTIAL_LOCK ||
                lock_level == CAN_LOCK_LEVEL_TOTAL_LOCK);
    }

    /// @brief Checks if this function can be performed with the current lock level
    /// @param func_id Function ID to check with
    /// @return 'true' if the function can be performed; 'false' if not
    bool _IsLockedForFunction(const can_function_id_t func_id)
    {
        // CAN_FUNC_LOCK_IN & CAN_FUNC_SYSTEM_REQUEST_IN should always work
        if (func_id == CAN_FUNC_LOCK_IN || func_id == CAN_FUNC_SYSTEM_REQUEST_IN)
            return false;

        bool result = true;

        switch (_lock_level)
        {
        case CAN_LOCK_LEVEL_UNLOCKED:
            result = false;
            break;

        case CAN_LOCK_LEVEL_PARTIAL_LOCK:
            result = result && (func_id != CAN_FUNC_REQUEST_IN);
            break;

        case CAN_LOCK_LEVEL_TOTAL_LOCK:
        default:
            break;
        }

        return result;
    }

    /// @brief Fills CAN frame with event specific data
    /// @param event_type Type of the event
    /// @param can_frame CAN frame for filling with data.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of operation (should we send any CAN/Error frames or not)
    can_result_t _PrepareEventCanFrame(event_type_t event_type, can_frame_t &can_frame, can_error_t &error)
    {
        switch (event_type)
        {
        case CAN_EVENT_TYPE_NORMAL:
            return _PrepareRawCanFrame(can_frame, error, CAN_FUNC_EVENT_OK, _data_fields, sizeof(_data_fields));

        case CAN_EVENT_TYPE_ERROR:
            can_frame.initialized = false;
            if (_error_code_hardware != 0)
            {
                error.error_section = ERROR_SECTION_HARDWARE;
                error.error_code = _error_code_hardware;
            }
            else
            {
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_HARDWARE_ERROR_CODE_IS_MISSING;
            }
            return CAN_RESULT_ERROR;

        case CAN_EVENT_TYPE_NONE:
        case CAN_EVENT_TYPE_MASK:
        default:
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_UNSUPPORTED_EVENT_TYPE;
            break;
        }

        return CAN_RESULT_ERROR;
    }

    /// @brief Fills CAN frame with timer specific data.
    /// @param timer_type Type of the timer.
    /// @param can_frame CAN frame for filling with data.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of operation (should we send any CAN/Error frames or not)
    can_result_t _PrepareTimerCanFrame(timer_type_t timer_type, can_frame_t &can_frame, can_error_t &error)
    {
        can_function_id_t func_id = CAN_FUNC_NONE;

        switch (timer_type)
        {
        case CAN_TIMER_TYPE_NORMAL:
            func_id = CAN_FUNC_TIMER_NORMAL;
            break;

        case CAN_TIMER_TYPE_WARNING:
            func_id = CAN_FUNC_TIMER_WARNING;
            break;

        case CAN_TIMER_TYPE_CRITICAL:
            func_id = CAN_FUNC_TIMER_CRITICAL;
            break;

        case CAN_TIMER_TYPE_NONE:
        case CAN_TIMER_TYPE_MASK:
        default:
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_UNSUPPORTED_TIMER_TYPE;
            return CAN_RESULT_ERROR;
        }

        return _PrepareRawCanFrame(can_frame, error, func_id, _data_fields, sizeof(_data_fields));
    }

    /// @brief Fills CAN frame with request specific data.
    /// @param can_frame Incoming and outgoing CAN frame.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of operation (should we send any CAN/Error frames or not)
    can_result_t _PrepareRequestCanFrame(can_frame_t &can_frame, can_error_t &error)
    {
        if (can_frame.raw_data_length != 1)
        {
            can_frame.initialized = false;
            error.function_id = CAN_FUNC_EVENT_ERROR;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_INCORRECT_REQUEST;
            return CAN_RESULT_ERROR;
        }

        return _PrepareRawCanFrame(can_frame, error, CAN_FUNC_EVENT_OK, _data_fields, _item_count * sizeof(T));
    }

    /// @brief Fills CAN frame with system request specific data.
    /// @param can_frame Incoming and outgoing CAN frame.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of operation (should we send any CAN/Error frames or not)
    can_result_t _PrepareSystemRequestCanFrame(can_frame_t &can_frame, can_error_t &error)
    {
        if (can_frame.raw_data_length != 1)
        {
            can_frame.initialized = false;
            error.function_id = CAN_FUNC_EVENT_ERROR;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_SYSTEM_REQUEST_SHOULD_NOT_HAVE_DATA;
            return CAN_RESULT_ERROR;
        }

        return _PrepareRawCanFrame(can_frame, error, CAN_FUNC_SYSTEM_REQUEST_OUT_OK, &_object_type, sizeof(_object_type));
    }

    /// @brief Fills CAN frame with real-time data. This method is called for all real-time data sender objects.
    /// @param can_frame An outgoing CAN frame. It is initially empty.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of operation (should we send any CAN/Error frames or not)
    can_result_t _PrepareRealtimeCanFrame(can_frame_t &can_frame, can_error_t &error)
    {
        if (can_frame.initialized)
        {
            clear_can_frame_struct(can_frame);
        }

        if (_realtime_frame_id == UINT8_MAX)
        {
            _realtime_frame_id = 1;
        }
        else
        {
            ++_realtime_frame_id;
        }

        // uint8_t payload_size = _item_count * sizeof(T);
        // while (payload_size > CAN_FRAME_MAX_PAYLOAD - 2)
        //{
        //     payload_size = payload_size - sizeof(T);
        // }
        uint8_t payload_size = sizeof(T);
        if (payload_size > CAN_FRAME_MAX_PAYLOAD - 2)
            payload_size = 0;
        uint8_t frame_data[CAN_FRAME_MAX_PAYLOAD - 1] = {0};
        frame_data[0] = _realtime_frame_id;
        memcpy(&(frame_data[1]), _data_fields, payload_size);

        return _PrepareRawCanFrame(can_frame, error, CAN_FUNC_SET_REAL_TIME_IN, frame_data, payload_size + 1);
    }

    /// @brief Fills CAN frame with specified data
    /// @param can_frame [OUT] CAN frame for processing
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @param function_id [IN] CAN function ID
    /// @param data [IN] Frame data to send in CAN frame
    /// @param data_length [IN] Frame data length
    /// @return The result of incoming can frame processing (should we send any CAN frames or not)
    virtual can_result_t _PrepareRawCanFrame(can_frame_t &can_frame, can_error_t &error, can_function_id_t function_id, void *data = nullptr, uint8_t data_length = 0)
    {
        if (data == nullptr && data_length != 0)
        {
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_HAVE_NO_DATA;
            return CAN_RESULT_ERROR;
        }

        if (data_length > CAN_FRAME_MAX_PAYLOAD)
        {
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_INCORRECT_DATA_LENGTH;
            return CAN_RESULT_ERROR;
        }

        clear_can_frame_struct(can_frame);
        can_frame.object_id = GetId();
        can_frame.function_id = function_id;
        can_frame.raw_data_length = data_length + 1;
        if (data != nullptr)
        {
            memcpy(can_frame.data, data, data_length);
        }
        can_frame.initialized = true;

        return CAN_RESULT_CAN_FRAME;
    };
};
