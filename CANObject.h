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

    /// @brief
    /// @param event_handler
    virtual void RegisterFunctionEvent(event_handler_t event_handler) = 0;

    /// @brief
    /// @param set_handler
    virtual void RegisterFunctionSet(set_handler_t set_handler) = 0;

    /// @brief
    /// @param timer_handler
    virtual void RegisterFunctionTimer(timer_handler_t timer_handler) = 0;

    /// @brief
    /// @param request_handler
    virtual void RegisterFunctionRequest(request_handler_t request_handler) = 0;

    /// @brief Performs CANObjects processing
    /// @param time Current time
    /// @param can_frame CAN frame for storing the outgoing data
    virtual void Process(uint32_t time, can_frame_t &can_frame) = 0;

    /// @brief Process incoming CAN frame
    /// @param can_frame CAN frame for processing
    virtual void InputCanFrame(can_frame_t &can_frame) = 0;

    /// @brief Returns CANObject ID
    /// @return Returns CANObject ID
    virtual can_object_id_t GetId() = 0;
};

/******************************************************************************************
 *
 ******************************************************************************************/
template <typename T, uint8_t _item_count = 1>
class CANObject : public CANObjectInterface
{
public:
    CANObject() = delete;

    CANObject(can_object_id_t id, uint16_t timer_period_ms, uint16_t error_period_ms)
        : _id(id), _timer_period(timer_period_ms), _error_period(error_period_ms){};

    virtual ~CANObject() = default;

    virtual void RegisterFunctionEvent(event_handler_t event_handler) override{};

    virtual void RegisterFunctionSet(set_handler_t set_handler) override{};

    virtual void RegisterFunctionTimer(timer_handler_t timer_handler) override{};

    virtual void RegisterFunctionRequest(request_handler_t request_handler) override{};

    /// @brief Performs CANObjects processing
    /// @param time Current time
    /// @param can_frame OUT: CAN frame for storing the outgoing data
    virtual void Process(uint32_t time, can_frame_t &can_frame) override
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
            _PrepareEventCanFrame(max_event_type, can_frame);
            for (uint8_t i = 0; i < _item_count; i++)
            {
                if ((_states_of_data_fields[i] & CAN_EVENT_TYPE_MASK) == CAN_EVENT_TYPE_NORMAL)
                    _states_of_data_fields[i] = (_states_of_data_fields[i] & (uint8_t)CAN_TIMER_TYPE_MASK) | CAN_EVENT_TYPE_NONE;
            }
        }
        else if (max_event_type > CAN_EVENT_TYPE_NORMAL && _error_period != UINT16_MAX && time - _last_event_time >= _error_period)
        {
            // TODO: Bug or feature found.
            // If error-event was sent and the time has not come for the next sending, then timer will be done regardless error state
            _PrepareEventCanFrame(max_event_type, can_frame);
            _last_event_time = time;
        }
        else if (_timer_period != UINT16_MAX && time - _last_timer_time >= _timer_period)
        {
            _PrepareTimerCanFrame(max_timer_type, can_frame);
            _last_timer_time = time;
        }
    };

    /// @brief Process incoming CAN frame
    /// @param can_frame CAN frame for processing
    virtual void InputCanFrame(can_frame_t &can_frame) override{};

    /// @brief Returns CANObject ID
    /// @return Returns CANObject ID
    virtual can_object_id_t GetId() override
    {
        return _id;
    };

    // TODO: don't like it =( State for timer... Ok-event (send immediately)... Error-event (need error section & code)...
    /// @brief
    /// @param index
    /// @param value
    /// @param timer_type
    /// @param event_type
    void SetValue(uint8_t index, T value,
                  timer_type_t timer_type = CAN_TIMER_TYPE_NONE,
                  event_type_t event_type = CAN_EVENT_TYPE_NONE)
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
        if (value == nullptr || index >= _item_count)
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

    uint16_t _timer_period = UINT16_MAX;
    uint16_t _error_period = UINT16_MAX;

    /// @brief 
    /// @param can_frame 
    void _ClearCanFrame(can_frame_t &can_frame)
    {
        memset(&can_frame, 0, sizeof(can_frame));
        can_frame.initialized = false;
    }

    /// @brief 
    /// @param event_type 
    /// @param can_frame 
    void _PrepareEventCanFrame(event_type_t event_type, can_frame_t &can_frame)
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
            can_frame.function_id = CAN_FUNC_EVENT_ERROR;
            // TODO: error code here...
            can_frame.raw_data_length = sizeof(can_frame.function_id); // + sizeof(/*error code & section*/);
            can_frame.initialized = true;
            return;

        case CAN_EVENT_TYPE_NONE:
        case CAN_EVENT_TYPE_MASK:
        default:
            break;
        }
    }

    /// @brief 
    /// @param timer_type 
    /// @param can_frame 
    void _PrepareTimerCanFrame(timer_type_t timer_type, can_frame_t &can_frame)
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
            return;
        }

        memcpy(can_frame.data, _data_fields, sizeof(_data_fields));
        can_frame.raw_data_length = sizeof(can_frame.function_id) + sizeof(_data_fields);
        can_frame.initialized = true;
    }
};
