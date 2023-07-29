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

    /// @brief Registers an external handler for request commands. It will be called when request command comes.
    /// @param request_handler Pointer to the request command handler.
    /// @return CANObjectInterface reference
    virtual CANObjectInterface &RegisterFunctionRequest(request_handler_t request_handler) = 0;

    /// @brief Checks whether the external request function handler is set.
    /// @return 'true' if the external handler exists, `false` if not
    virtual bool HasExternalFunctionRequest() = 0;

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
    virtual can_result_t FillRawCanFrame(can_frame_t &can_frame, can_error_t &error, can_function_id_t function_id, uint8_t *data = nullptr, uint8_t data_length = 0 ) = 0;

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
    virtual void *GetValue(uint8_t index) = 0;
};

/******************************************************************************************
 *
 ******************************************************************************************/
template <typename T, uint8_t _item_count = 1>
class CANObject : public CANObjectInterface
{
    static_assert(_item_count > 0); // 0 data fields isn't allowed
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
    CANObject(can_object_id_t id, uint16_t timer_period_ms = CAN_TIMER_DISABLED, uint16_t error_period_ms = CAN_ERROR_DISABLED, bool flood_mode = false)
        : _id(id), _timer_period(timer_period_ms), _error_period(error_period_ms), _flood_mode(flood_mode)
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
        return _request_handler;
    };

    /// @brief Performs processing of CANObjects
    /// @param time Current time
    /// @param can_frame [OUT] CAN frame for storing the outgoing data
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of CANObject processing (should we send any CAN frames or not)
    virtual can_result_t Process(uint32_t time, can_frame_t &can_frame, can_error_t &error) override
    {
        timer_type_t max_timer_type = CAN_TIMER_TYPE_NONE;
        event_type_t max_event_type = CAN_EVENT_TYPE_NONE;
        for (uint8_t i = 0; i < _item_count; i++)
        {
            if ((_states_of_data_fields[i] & CAN_TIMER_TYPE_MASK) > max_timer_type)
                max_timer_type = (timer_type_t)(_states_of_data_fields[i] & (uint8_t)CAN_TIMER_TYPE_MASK);

            if ((_states_of_data_fields[i] & CAN_EVENT_TYPE_MASK) > max_event_type)
                max_event_type = (event_type_t)(_states_of_data_fields[i] & (uint8_t)CAN_EVENT_TYPE_MASK);
        }

        can_result_t handler_result = CAN_RESULT_IGNORE;

        clear_can_frame_struct(can_frame);
        if (max_event_type == CAN_EVENT_TYPE_NORMAL)
        {
            // CAN_EVENT_TYPE_NORMAL should be sent immediately, we don't need to check the time
            if (_event_handler != nullptr)
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
            // enclosed if fixes that: 
            // A bug or a feature found =)
            // If error-event was sent and the time has not come for the next sending, then timer will be done regardless error state
            if (time - _last_event_time >= _error_period)
            {
                if (_event_handler != nullptr)
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
                if (_timer_handler != nullptr)
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
    /// @return The result of incoming can frame processing (should we send any CAN frames or not)
    virtual can_result_t InputCanFrame(can_frame_t &can_frame, can_error_t &error) override
    {
        if (!can_frame.initialized)
            return CAN_RESULT_ERROR;

        can_result_t handler_result = CAN_RESULT_ERROR;

        switch (can_frame.function_id)
        {
        case CAN_FUNC_SET_IN:
            if (_set_handler != nullptr)
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
            if (!can_frame.initialized && error.error_section == ERROR_SECTION_NONE)
            {
                error.function_id = CAN_FUNC_EVENT_ERROR;
            }
            break;

        case CAN_FUNC_REQUEST_IN:
            if (_request_handler != nullptr)
            {
                handler_result = _request_handler(can_frame, error);
            }
            else
            {
                handler_result = _PrepareRequestCanFrame(can_frame, error);
            }
            if (!can_frame.initialized && error.error_section == ERROR_SECTION_NONE)
            {
                error.function_id = CAN_FUNC_EVENT_ERROR;
            }
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

        if (!can_frame.initialized && error.error_section == ERROR_SECTION_NONE)
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
    virtual can_result_t FillRawCanFrame(can_frame_t &can_frame, can_error_t &error, can_function_id_t function_id, uint8_t *data = nullptr, uint8_t data_length = 0 ) override
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
    };

    /// @brief Universal getter for CANObject's data fields
    /// @param index Index of data field to get value from. If the index is out of range, nullpointer will be returned.
    /// @return Pointer to the data field value. If the index is out of range, nullpointer will be returned.
    virtual void *GetValue(uint8_t index) override
    {
        if (index >= _item_count)
            return nullptr;

        return (void *)&_data_fields[index];
    };

    /// @brief The variation of GetValue() method, which returns typed value.
    /// @param index Index of data field to get value from.
    /// @return The value of the specified data field. If the index is out of range, zero value will be returned.
    T GetTypedValue(uint8_t index)
    {
        if (index >= _item_count)
            return (T)0;

        return _data_fields[index];
    }

private:
    can_object_id_t _id = 0;

    // local data storage
    T _data_fields[_item_count] = {0};
    static_assert(_item_count * sizeof(T) <= 7); // static data size validation (to fit it into the can frame)
    uint8_t _states_of_data_fields[_item_count] = {0};

    uint32_t _last_timer_time = 0;
    uint32_t _last_event_time = 0;

    uint16_t _timer_period = CAN_TIMER_DISABLED;
    uint16_t _error_period = CAN_ERROR_DISABLED;
    
    bool _flood_mode = false;
    bool _has_new_data = false;

    event_handler_t _event_handler = nullptr;
    set_handler_t _set_handler = nullptr;
    timer_handler_t _timer_handler = nullptr;
    request_handler_t _request_handler = nullptr;

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
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_SOMETHING_WRONG;
            /* TODO: custom error code here like that:
            can_frame.function_id = CAN_FUNC_EVENT_ERROR;
            can_frame.raw_data_length = sizeof(can_frame.function_id); // + sizeof(can_error_t);
            can_frame.initialized = true;
            */
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


/******************************************************************************************
 * CANObjectSystem: class of system CANObjects (like BlockInfo)
 * 
 * The difference with CANObjects:
 *   - all timer frames has CAN_FUNC_SYSTEM_TIMER_xxx function IDs
 *     instead of CAN_FUNC_TIMER_xxx
 ******************************************************************************************/
template <typename T, uint8_t _item_count = 1>
class CANObjectSystem : public CANObject<T, _item_count>
{
    static_assert(_item_count > 0); // 0 data fields isn't allowed
public:
    /// @brief Default constructor is forbidden.
    CANObjectSystem() = delete;

    /// @brief Constructor of the CANObject
    /// @param id ID of the object
    /// @param timer_period_ms Period of a timer function. If CAN_TIMER_DISABLED then timer is disabled
    /// @param error_period_ms Repeation delay of an error event. If CAN_ERROR_DISABLED then error event is disabled
    /// @param flood_mode 'true' for work in flood mode: timer will send frame every period regardless of actual data updates
    ///                   'false' for work in frame limit mode: timer will send frames every period when the data was changed; but not more often than actual data updates.
    ///                   Example #1: timer in the frame limit mode, period is 250 ms, data updates every 30 ms, frame will be sent every 250 ms.
    ///                   Example #2: timer in the frame limit mode, period is 250 ms, data updates every 800 ms, frame will be sent every 800 ms.
    ///                   Example #3: timer in the flood mode, period is 250 ms, data was updated once on boot, frame will be sent every 250 ms.
    CANObjectSystem(can_object_id_t id, uint16_t timer_period_ms = CAN_TIMER_DISABLED, uint16_t error_period_ms = CAN_ERROR_DISABLED, bool flood_mode = false)
        : CANObject<T, _item_count>(id, timer_period_ms, error_period_ms, flood_mode)
    {};

    virtual ~CANObjectSystem() = default;

    /// @brief Performs processing of CANObjects
    /// @param time Current time
    /// @param can_frame [OUT] CAN frame for storing the outgoing data
    /// @param error [OUT] An outgoing error structure. It will be filled by object if something went wrong.
    /// @return The result of CANObject processing (should we send any CAN frames or not)
    virtual can_result_t Process(uint32_t time, can_frame_t &can_frame, can_error_t &error) override
    {
        can_result_t handler_result = CANObject<T, _item_count>::Process(time, can_frame, error);

        if (handler_result == CAN_RESULT_CAN_FRAME && can_frame.initialized)
        {
            switch (can_frame.function_id)
            {
            case CAN_FUNC_TIMER_NORMAL:
                can_frame.function_id = CAN_FUNC_SYSTEM_TIMER_NORMAL;
                break;
            
            case CAN_FUNC_TIMER_WARNING:
                can_frame.function_id = CAN_FUNC_SYSTEM_TIMER_WARNING;
                break;

            case CAN_FUNC_TIMER_CRITICAL:
                can_frame.function_id = CAN_FUNC_SYSTEM_TIMER_CRITICAL;
                break;
            
            default:
                break;
            }
        }

        return handler_result;
    };
};
