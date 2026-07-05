#pragma once

#include <cstring>
#include <CUtils_RingBuffer.h>
#include "CanCommon.h"
#include "CanManagerInterface.h"
#include "CanObjectInterface.h"

// expected behavior for time source function: it returns time in ms
using get_time_ms_function_t = uint32_t (*)();

// expected behavior for CAN_Send function:
//     It puts CAN frame into hardware outgoing queue and returns `true`.
//     If the outgoing queue is full the CAN_Send function should return `false`.
using can_send_function_t = bool (*)(can_object_id_t id, uint8_t *data, uint8_t length);

// expected behavior for interrupt control function:
//     if enable = true, it shold enable interrupts
//     if enable = false, it should disable interrupts
using set_interrupts_enabled_t = void (*)(bool enable);

template <
    uint8_t _max_objects,
    uint8_t _can_frame_rx_buffer_size = 16,
    uint8_t _can_frame_tx_buffer_size = 16,
    uint8_t _tick_time = 10>
class CANManager : public CanManagerInterface
{
private:
    RingBuffer<_can_frame_rx_buffer_size, can_frame_t> _rx_buffer;
    RingBuffer<_can_frame_tx_buffer_size, can_frame_t> _tx_buffer;

    CANObjectInterface *_objects[_max_objects] = {nullptr};
    static_assert(_max_objects > 0, "Manager should be able to work with at least 1 CAN Object");

    can_send_function_t _SendFunc = nullptr;
    get_time_ms_function_t _Millis = nullptr;
    set_interrupts_enabled_t _SetInterruptEnabled = nullptr;

    uint32_t _last_tick = 0;

    void _ProcessTXBuffer() noexcept
    {
        size_t frames_to_send = _tx_buffer.Count();
        // We will never get stuck here if tx_buffer is replenished faster than we send data
        while (frames_to_send-- > 0 && TryToSendCANFrameFromTXQueue())
        {
        }
    }

    void _ProcessRXBuffer() noexcept
    {
        size_t frames_to_receive = _rx_buffer.Count();
        // We will never get stuck here if the CAN bus is working faster then we parse data
        while (frames_to_receive-- > 0)
        {
            can_frame_t can_frame;
            if (!_rx_buffer.Read(can_frame))
                break;

            if (can_frame.object_id == CAN_SYSTEM_ID_BROADCAST)
            {
                uint8_t index = 0;
                while (_objects[index] != nullptr)
                {
                    _objects[index]->ProcessFrame(can_frame);
                    index++;
                }
            }
            else
            {
                CANObjectInterface *can_object = GetCanObject(can_frame.object_id);
                if (can_object != nullptr)
                {
                    can_object->ProcessFrame(can_frame);
                }
            }
        }
    }

    void _ProcessObjects() const noexcept
    {
        uint8_t index = 0;
        while (_objects[index] != nullptr)
        {
            _objects[index]->Tick(_Millis());
            index++;
        }
    }

public:
    CANManager() = delete;
    CANManager(const CANManager &) = delete;
    CANManager &operator=(const CANManager &) = delete;

    CANManager(can_send_function_t can_send_func,
               get_time_ms_function_t millis,
               set_interrupts_enabled_t set_interrupt_enabled)
        : _SendFunc(can_send_func), _Millis(millis), _SetInterruptEnabled(set_interrupt_enabled) {};

    virtual ~CANManager() = default;

    virtual bool AddObject(CANObjectInterface &can_object) noexcept override final
    {
        uint8_t free_slot = 0;
        while (_objects[free_slot++] != nullptr)
        {
        }
        if (free_slot == _max_objects)
            return false;

        _objects[free_slot] = &can_object;
        can_object.SetParent(*this);

        return true;
    };

    virtual bool HasCanObject(can_object_id_t id) const noexcept override final
    {
        uint8_t index = 0;
        while (_objects[index] != nullptr)
        {
            if (_objects[index]->GetId() == id)
                return true;
            index++;
        }

        return false;
    }

    virtual CANObjectInterface *GetCanObject(can_object_id_t id) const noexcept override final
    {
        uint8_t index = 0;
        while (_objects[index] != nullptr)
        {
            if (_objects[index]->GetId() == id)
                return _objects[index];
            index++;
        }

        return nullptr;
    }

    virtual void Processing() noexcept override final
    {
        uint32_t time = _Millis();
        if (time - _last_tick < _tick_time)
            return;

        _last_tick = time;

        _ProcessRXBuffer();
        _ProcessObjects();
        _ProcessTXBuffer();
    }

    virtual bool TryToSendCANFrameFromTXQueue() noexcept override final
    {
        can_frame_t can_frame = {0};
        if (!_tx_buffer.Peek(can_frame))
        {
            return false;
        }

        _SetInterruptEnabled(false);
        bool result = _SendFunc(can_frame.object_id, can_frame.raw_data, can_frame.raw_data_length);
        if (result)
        {
            _tx_buffer.DeleteOne();
        }
        _SetInterruptEnabled(true);

        return result;
    }

    virtual uint32_t GetTime() const noexcept override final { return _Millis(); }

    virtual bool PushFrameToTX(can_frame_t &can_frame) noexcept override final
    {
        return _tx_buffer.Write(can_frame);
    }

    virtual bool PushFrameToTX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) noexcept override final
    {
        if (data == nullptr || length == 0 || length > RAW_CAN_FRAME_MAX_PAYLOAD)
            return false;

        can_frame_t can_frame;
        can_frame.object_id = id;
        memcpy(can_frame.raw_data, data, length);
        can_frame.raw_data_length = length;

        return _tx_buffer.Write(can_frame);
    }

    virtual uint16_t GetTXQueueLength() const noexcept override final { return _tx_buffer.Count(); }

    virtual bool PushFrameToRX(can_frame_t &can_frame) noexcept override final
    {
        return _rx_buffer.Write(can_frame);
    }

    virtual bool PushFrameToRX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) noexcept override final
    {
        if (data == nullptr || length == 0 || length > RAW_CAN_FRAME_MAX_PAYLOAD)
            return false;

        can_frame_t can_frame;
        can_frame.object_id = id;
        memcpy(can_frame.raw_data, data, length);
        can_frame.raw_data_length = length;

        return _rx_buffer.Write(can_frame);
    }

    virtual uint16_t GetRXQueueLength() const noexcept override final { return _rx_buffer.Count(); }
};
