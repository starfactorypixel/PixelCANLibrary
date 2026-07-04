#pragma once

#include <cstring>
#include <CUtils_RingBuffer.h>
#include "CanCommon.h"
#include "CanManagerInterface.h"
#include "CanObjectInterface.h"

template <
    can_object_id_t _base_obj_id,
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

    can_send_function_t _send_func = nullptr;

    uint32_t _last_tick = 0;

    void _processTXBuffer()
    {
        size_t frames_to_send = _tx_buffer.Count();
        // We will never get stuck here if tx_buffer is replenished faster than we send data
        while (frames_to_send-- > 0 && tryToSendCANFrameFromTXQueue())
        {
        }
    }

    void _processRXBuffer()
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
                for (uint8_t i = 0; i < _max_objects; i++)
                {
                    if (_objects[i] != nullptr)
                    {
                        _objects[i]->processFrame(can_frame);
                    }
                }
            }
            else
            {
                CANObjectInterface *can_object = getCanObject(can_frame.object_id);
                if (can_object != nullptr)
                {
                    can_object->processFrame(can_frame);
                }
            }
        }
    }

    void _processObjectTicks(uint32_t time)
    {
        for (uint8_t i = 0; i < _max_objects; i++)
        {
            if (_objects[i] != nullptr)
            {
                _objects[i]->tick(time);
            }
        }
    }

public:
    CANManager() = delete;
    CANManager(const CANManager &) = delete;
    CANManager &operator=(const CANManager &) = delete;
    CANManager(can_send_function_t can_send_func) : _send_func(can_send_func) {};
    virtual ~CANManager() = default;

    virtual bool addObject(CANObjectInterface &can_object) override final
    {
        if (can_object.getId() < _base_obj_id)
            return false;
        if (can_object.getId() - _base_obj_id >= _max_objects)
            return false;
        if (_objects[can_object.getId() - _base_obj_id] != nullptr)
            return false;

        _objects[can_object.getId() - _base_obj_id] = &can_object;
		can_object.setParent(*this);
        return true;
    };

    virtual bool hasCanObject(can_object_id_t id) override final
    {
        if (id < _base_obj_id)
            return false;
        if (id - _base_obj_id >= _max_objects)
            return false;

        return _objects[id - _base_obj_id] != nullptr;
    }

    virtual CANObjectInterface *getCanObject(can_object_id_t id) override final
    {
        if (id < _base_obj_id)
            return nullptr;
        if (id - _base_obj_id >= _max_objects)
            return nullptr;

        return _objects[id - _base_obj_id];
    }

    virtual void tick(uint32_t time) override final
    {
        if (time - _last_tick < _tick_time)
            return;

        _last_tick = time;

        _processTXBuffer();
        _processRXBuffer();
        _processObjectTicks(time);
    }

    virtual bool tryToSendCANFrameFromTXQueue() override final
    {
        can_frame_t can_frame = {0};
        if (!_tx_buffer.Peek(can_frame))
        {
            return false;
        }

        if (_send_func(can_frame.object_id, can_frame.raw_data, can_frame.raw_data_length))
        {
            _tx_buffer.DeleteOne();
            return true;
        }

        return false;
    }

    virtual bool pushFrameToTX(can_frame_t &can_frame) override final
    {
        return _tx_buffer.Write(can_frame);
    }

    virtual bool pushFrameToTX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) override final
    {
        if (data == nullptr || length == 0 || length > RAW_CAN_FRAME_MAX_PAYLOAD)
            return false;

        can_frame_t can_frame;
        can_frame.object_id = id;
        memcpy(can_frame.raw_data, data, length);
		can_frame.raw_data_length = length;

        return pushFrameToTX(can_frame);
    }

    virtual bool pushFrameToRX(can_frame_t &can_frame) override final
    {
        return _rx_buffer.Write(can_frame);
    }

    virtual bool pushFrameToRX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) override final
    {
        if (data == nullptr || length == 0 || length > RAW_CAN_FRAME_MAX_PAYLOAD)
            return false;

        can_frame_t can_frame;
        can_frame.object_id = id;
        memcpy(can_frame.raw_data, data, length);
		can_frame.raw_data_length = length;

        return pushFrameToRX(can_frame);
    }
};
