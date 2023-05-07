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
    virtual void RegisterFunctionEvent(event_handler_t event_handler, uint16_t error_delay_ms) = 0;

    /// @brief Registers an external handler for set commands. It will be called when set command comes.
    /// @param set_handler Pointer to the set command handler.
    virtual void RegisterFunctionSet(set_handler_t set_handler) = 0;

    /// @brief Registers an external handler for timers. It will be called when timer occurs.
    /// @param timer_handler Pointer to the timer handler.
    /// @param period_ms Timers period in milliseconds.
    virtual void RegisterFunctionTimer(timer_handler_t timer_handler, uint16_t period_ms) = 0;

    /// @brief Registers an external handler for request commands. It will be called when request command comes.
    /// @param request_handler Pointer to the request command handler.
    virtual void RegisterFunctionRequest(request_handler_t request_handler) = 0;

    /// @brief Performs CANObjects processing
    /// @param time Current time
    /// @param can_frame CAN frame for storing the outgoing data
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    virtual void Process(uint32_t time, can_frame_t &can_frame, error_t &error) = 0;

    /// @brief Process incoming CAN frame
    /// @param can_frame CAN frame for processing
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    virtual void InputCanFrame(can_frame_t &can_frame, error_t &error) = 0;

    /// @brief Returns CANObject ID
    /// @return Returns CANObject ID
    virtual can_object_id_t GetId() = 0;

    /// @brief Sets the value of timers period.
    /// @param period_ms Timers period in milliseconds.
    virtual void SetTimerPeriod(uint16_t period_ms) = 0;

    /// @brief Sets the value of error events resending delay.
    /// @param delay_ms Delay for the error evends in milliseconds.
    virtual void SetErrorEventDelay(uint16_t delay_ms) = 0;

    /// @brief Universal setter for CANObject's data fields
    /// @param index Index of data field to set. If the index is out of range, nothing will be done.
    /// @param value Pointer to the variable with data. The size of data depends of CANObject.
    /// @param timer_type The type of value for timers. With this we can specify is value normal, in warning range or in critical range.
    /// @param event_type The type of value for events. With this we can specify whether an event and what kind of event it is.
    virtual void SetValue(uint8_t index, void *value,
                          timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                          event_type_t event_type = CAN_EVENT_TYPE_NONE) = 0;
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

    CANObject(can_object_id_t id, uint16_t timer_period_ms = CAN_TIMER_DISABLED, uint16_t error_period_ms = CAN_ERROR_DISABLED)
        : _id(id), _timer_period(timer_period_ms), _error_period(error_period_ms){};

    virtual ~CANObject() = default;

    /// @brief Registers an external handler for events. It will be called when event occurs.
    /// @param event_handler Pointer to the event handler.
    /// @param error_delay_ms Delay for the error evends in milliseconds.
    virtual void RegisterFunctionEvent(event_handler_t event_handler, uint16_t error_delay_ms) override
    {
        _event_handler = event_handler;
        SetErrorEventDelay(error_delay_ms);
    };

    /// @brief Registers an external handler for set commands. It will be called when set command comes.
    /// @param set_handler Pointer to the set command handler.
    virtual void RegisterFunctionSet(set_handler_t set_handler) override
    {
        _set_handler = set_handler;
    };

    /// @brief Registers an external handler for timers. It will be called when timer occurs.
    /// @param timer_handler Pointer to the timer handler.
    /// @param period_ms Timers period in milliseconds.
    virtual void RegisterFunctionTimer(timer_handler_t timer_handler, uint16_t period_ms) override
    {
        _timer_handler = timer_handler;
        SetTimerPeriod(period_ms);
    };

    /// @brief Registers an external handler for request commands. It will be called when request command comes.
    /// @param request_handler Pointer to the request command handler.
    virtual void RegisterFunctionRequest(request_handler_t request_handler) override
    {
        _request_handler = request_handler;
    };

    /// @brief Performs processing of CANObjects
    /// @param time Current time
    /// @param can_frame OUT: CAN frame for storing the outgoing data
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    virtual void Process(uint32_t time, can_frame_t &can_frame, error_t &error) override
    {
        timer_type_t max_timer_type = CAN_TIMER_TYPE_NONE;
        event_type_t max_event_type = CAN_EVENT_TYPE_NONE;
        // uint8_t event_normal_index = UINT8_MAX;
        for (uint8_t i = 0; i < _item_count; i++)
        {
            if ((_states_of_data_fields[i] & CAN_TIMER_TYPE_MASK) > max_timer_type)
                max_timer_type = (timer_type_t)(_states_of_data_fields[i] & (uint8_t)CAN_TIMER_TYPE_MASK);

            if ((_states_of_data_fields[i] & CAN_EVENT_TYPE_MASK) > max_event_type)
                max_event_type = (event_type_t)(_states_of_data_fields[i] & (uint8_t)CAN_EVENT_TYPE_MASK);

            // if (max_event_type == CAN_EVENT_TYPE_NORMAL)
            //     event_normal_index = i;
        }

        _ClearCanFrame(can_frame);
        if (max_event_type == CAN_EVENT_TYPE_NORMAL /* should send immediately, don't need to check time */)
        {
            if (_event_handler != nullptr)
            {
                _event_handler(can_frame, max_event_type, error);
            }
            else
            {
                _PrepareEventCanFrame(max_event_type, can_frame, error);
            }

            // we need to flush the NORMAL event state of all data fields
            for (uint8_t i = 0; i < _item_count; i++)
            {
                if ((_states_of_data_fields[i] & CAN_EVENT_TYPE_MASK) == CAN_EVENT_TYPE_NORMAL)
                    _states_of_data_fields[i] = (_states_of_data_fields[i] & (uint8_t)CAN_TIMER_TYPE_MASK) | CAN_EVENT_TYPE_NONE;
            }
        }
        else if (max_event_type > CAN_EVENT_TYPE_NORMAL && _error_period != CAN_ERROR_DISABLED && time - _last_event_time >= _error_period)
        {
            // TODO: A bug or a feature found =)
            // If error-event was sent and the time has not come for the next sending, then timer will be done regardless error state

            if (_event_handler != nullptr)
            {
                _event_handler(can_frame, max_event_type, error);
            }
            else
            {
                _PrepareEventCanFrame(max_event_type, can_frame, error);
            }
            _last_event_time = time;
        }
        else if (max_timer_type != CAN_TIMER_TYPE_NONE && _timer_period != CAN_TIMER_DISABLED && time - _last_timer_time >= _timer_period)
        {
            if (_timer_handler != nullptr)
            {
                _timer_handler(can_frame, max_timer_type, error);
            }
            else
            {
                _PrepareTimerCanFrame(max_timer_type, can_frame, error);
            }
            _last_timer_time = time;
        }
    };

    /// @brief Process incoming CAN frame
    /// @param can_frame CAN frame for processing
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    virtual void InputCanFrame(can_frame_t &can_frame, error_t &error) override
    {
        if (!can_frame.initialized)
            return;

        switch (can_frame.function_id)
        {
        case CAN_FUNC_SET_IN:
            if (_set_handler != nullptr)
            {
                _set_handler(can_frame, error);
            }
            else
            {
                can_frame.initialized = false;
                error.error_section = ERROR_SECTION_CAN_OBJECT;
                error.error_code = ERROR_CODE_OBJECT_SET_FUNCTION_IS_MISSING;
            }
            return;

        case CAN_FUNC_REQUEST_IN:
            if (_request_handler != nullptr)
            {
                _request_handler(can_frame, error);
            }
            else
            {
                _PrepareRequestCanFrame(can_frame, error);
            }
            return;

        default:
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION;
            return;
        }
    };

    /// @brief Returns CANObject ID
    /// @return Returns CANObject ID
    virtual can_object_id_t GetId() override
    {
        return _id;
    };

    /// @brief Sets the value of timers period.
    /// @param period_ms Timers period in milliseconds.
    virtual void SetTimerPeriod(uint16_t period_ms) override
    {
        _timer_period = period_ms;
    };

    /// @brief Sets the value of error events resending delay.
    /// @param delay_ms Delay for the error evends in milliseconds.
    virtual void SetErrorEventDelay(uint16_t delay_ms) override
    {
        _error_period = delay_ms;
    };

    // TODO: don't like it =( State for timer... Ok-event (send immediately)... Error-event (need error section & code)...
    /// @brief Universal setter for CANObject's data fields
    /// @param index Index of data field to set. If the index is out of range, nothing will be done.
    /// @param value Pointer to the variable with data. The size of data depends of CANObject.
    /// @param timer_type The type of value for timers. With this we can specify is value normal, in warning range or in critical range.
    /// @param event_type The type of value for events. With this we can specify whether an event and what kind of event it is.
    virtual void SetValue(uint8_t index, void *value,
                          timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                          event_type_t event_type = CAN_EVENT_TYPE_NONE) override
    {
        // Что не нравится:
        //
        //  OK-Event - это внешнее событие (например, багажник окрылся). Оно должно отправляться только в момент изменения?
        //             Тогда мы должны после отправки фрейма сбрасывать event_type, если он равен CAN_EVENT_TYPE_NORMAL...
        //             При этом если в CANObject несколько полей с изменившимися данными, то сбросить надо для всех и отправить один фрейм.
        //             А дребезг контактов? Программно фильтруем слишком частое изменение?
        //
        //  ERROR-Event - это ошибка значения. Например, за счет ошибки чтения (износа ячейки памяти) мы вместо 1 или 0
        //                для концевика двери получили какой-то мусор. То есть чтение на пине успешное, а вот ячейка памяти, в которую
        //                сохранили статус двери, умерла.
        //
        //  Предположим, у нас CANObject, в котором 7 байт состояния дверей, багажника, капота...
        //  Если для двери мы прочитали 200 вместо 1/0, то это событие будет иметь приоритет. С CAN будет отправлено сообщение об ошибке.
        //  При этом пока ошибка не будет устранена, никакие события открытия дверей отправлять в шину не будут...
        //
        if (value == nullptr)
            return;

        SetValue(index, *(T *)value, timer_type, event_type);
    };

    /// @brief The variation of SetValue() method with typed value parameter.
    /// @param index Index of data field to set. If the index is out of range, nothing will be done.
    /// @param value The value to set to the specified data field.
    /// @param timer_type The type of value for timers. With this we can specify is value normal, in warning range or in critical range.
    /// @param event_type The type of value for events. With this we can specify whether an event and what kind of event it is.
    void SetValue(uint8_t index, T value,
                  timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                  event_type_t event_type = CAN_EVENT_TYPE_NONE)
    {
        if (index >= _item_count)
            return;

        _data_fields[index] = value;
        _states_of_data_fields[index] = timer_type | event_type;
    };

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

    event_handler_t _event_handler = nullptr;
    set_handler_t _set_handler = nullptr;
    timer_handler_t _timer_handler = nullptr;
    request_handler_t _request_handler = nullptr;

    /// @brief Clears the CAN frame structure and assigns it the value "not initialized"
    /// @param can_frame CAN frame to clear
    void _ClearCanFrame(can_frame_t &can_frame)
    {
        memset(&can_frame, 0, sizeof(can_frame));
        can_frame.initialized = false;
    }

    /// @brief Fills CAN frame with event specific data
    /// @param event_type Type of the event
    /// @param can_frame CAN frame for filling with data.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    void _PrepareEventCanFrame(event_type_t event_type, can_frame_t &can_frame, error_t &error)
    {
        // _ClearCanFrame(can_frame);

        switch (event_type)
        {
        case CAN_EVENT_TYPE_NORMAL:
            can_frame.function_id = CAN_FUNC_EVENT_OK;
            memcpy(can_frame.data, _data_fields, sizeof(_data_fields));
            can_frame.raw_data_length = sizeof(can_frame.function_id) + sizeof(_data_fields);
            can_frame.initialized = true;
            return;

        case CAN_EVENT_TYPE_ERROR:
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_SOMETHING_WRONG;
            /* TODO: custom error code here...
            can_frame.function_id = CAN_FUNC_EVENT_ERROR;
            can_frame.raw_data_length = sizeof(can_frame.function_id); // + sizeof(error_t);
            can_frame.initialized = true;
            */
            return;

        case CAN_EVENT_TYPE_NONE:
        case CAN_EVENT_TYPE_MASK:
        default:
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_UNSUPPORTED_EVENT_TYPE;
            break;
        }
    }

    /// @brief Fills CAN frame with timer specific data.
    /// @param timer_type Type of the timer.
    /// @param can_frame CAN frame for filling with data.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    void _PrepareTimerCanFrame(timer_type_t timer_type, can_frame_t &can_frame, error_t &error)
    {
        // _ClearCanFrame(can_frame);
        switch (timer_type)
        {
        case CAN_TIMER_TYPE_NORMAL:
            can_frame.function_id = CAN_FUNC_TIMER_NORMAL;
            break;

        case CAN_TIMER_TYPE_WARNING:
            can_frame.function_id = CAN_FUNC_TIMER_WARNING;
            break;

        case CAN_TIMER_TYPE_CRITICAL:
            can_frame.function_id = CAN_FUNC_TIMER_CRITICAL;
            break;

        case CAN_TIMER_TYPE_NONE:
        case CAN_TIMER_TYPE_MASK:
        default:
            can_frame.initialized = false;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_UNSUPPORTED_TIMER_TYPE;
            return;
        }

        memcpy(can_frame.data, _data_fields, sizeof(_data_fields));
        can_frame.raw_data_length = sizeof(can_frame.function_id) + sizeof(_data_fields);
        can_frame.initialized = true;
    }

    /// @brief Fills CAN frame with request specific data.
    /// @param can_frame Incoming and outgoing CAN frame.
    /// @param error An outgoing error structure. It will be filled by object if something went wrong.
    void _PrepareRequestCanFrame(can_frame_t &can_frame, error_t &error)
    {
        if (can_frame.raw_data_length != 1)
        {
            can_frame.initialized = false;
            error.function_id = CAN_FUNC_REQUEST_OUT_ERR;
            error.error_section = ERROR_SECTION_CAN_OBJECT;
            error.error_code = ERROR_CODE_OBJECT_INCORRECT_REQUEST;
            return;
        }

        _ClearCanFrame(can_frame);
        can_frame.initialized = true;
        can_frame.function_id = CAN_FUNC_REQUEST_OUT_OK;
        memcpy(can_frame.data, _data_fields, _item_count * sizeof(T));
        can_frame.raw_data_length = 1 + _item_count * sizeof(T);
    }
};
