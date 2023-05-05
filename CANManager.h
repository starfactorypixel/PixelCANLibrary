#pragma once

#include <stdint.h>
#include <cassert>
#include "CAN_common.h"
#include "CANObject.h"

/******************************************************************************************
 *
 ******************************************************************************************/
class CANManagerInterface
{
    using send_function_t = void (*)(can_id_t id, uint8_t *data, uint8_t length);

public:
    virtual ~CANManagerInterface() = default;

    /// @brief Registers specified CANObject
    /// @param can_object CANObject for registration
    /// @return 'true' if registration was successful, 'false' if not
    virtual bool RegisterObject(CANObjectInterface &can_object) = 0;

    /// @brief Registers low level function, that sends data via CAN bus
    /// @param can_send_func Pointer to the function
    virtual void RegisterSendFunction(send_function_t can_send_func) = 0;

    /// @brief Performs CANObjects processing
    /// @param time Current time
    virtual void Process(uint32_t time) = 0;

    /// @brief Processes incoming CAN frame (TODO: without any queues?)
    /// @param id CANObject ID from the CAN frame
    /// @param data Pointer to the data array
    /// @param length Data length
    /// @return true if CANObject with ID is registered, false if not
    virtual bool IncomingCANFrame(can_id_t id, uint8_t *data, uint8_t length) = 0;
};

/******************************************************************************************
 *
 ******************************************************************************************/
template <uint8_t _max_objects = 16>
class CANManager
{
    // don't perform data processing often
    // _tick_time is minimal delay in ms between the data processing
    static const uint8_t _tick_time = 5;
    using send_function_t = void (*)(can_id_t id, uint8_t *data, uint8_t length);

public:
    /// @brief
    /// @param param CANObject for registration
    /// @return 'true' if registration was successful, 'false' if not
    bool RegisterObject(CANObjectInterface &param)
    {
        if (_max_objects <= _objects_idx)
            return false;

        _objects[_objects_idx++] = &param;
        // _id_mask |= param.GetId();

        return true;
    }

    void RegisterSendFunction(send_function_t func)
    {
        _send_func = func;

        return;
    }

    /*
    bool HasObject(can_id_t id)
    {
        return ((id & _id_mask) == id);
    }

    can_id_t GetMask()
    {
        return _id_mask;
    }
    */

    void Processing(uint32_t time)
    {
        if (time - _last_tick < _tick_time)
            return;

        _last_tick = time;

        uint8_t flags;
        CANObjectInterface::packet_t packet;
        for (uint8_t i = 0; i < _objects_idx; ++i)
        {
            // Передавать time по ссылке и обновлять перед каждым вызовом?
            flags = _objects[i]->Processing(time, packet);
            if (flags & 0x02)
            {
                /*
                uint8_t *bytes;
                uint8_t length;
                _objects[i]->GetBytes(bytes, length);       // первый байт не верен
                _send_func(_objects[i]->GetId(), bytes, length);
                */

                _send_func(_objects[i]->GetId(), &packet.type, packet.length + 1);
            }
        }
    }

    /*
        @brief
        @return true если ID зарегистрирован, false если нет.
    */
    bool InputPacket(can_id_t id, uint8_t *data, uint8_t length)
    {
        bool result = false;

        for (uint8_t i = 0; i < _objects_idx; ++i)
        {
            if (_objects[i]->GetId() == id)
            {

                CANObjectInterface::packet_t packet;
                packet.length = length - 1;
                packet.type = data[0];
                memcpy(&packet.data, &data + 1, length - 1);

                _objects[i]->InputPacket(packet);

                _send_func(id, &packet.type, packet.length + 1);

                result = true;
                break;
            }
        }

        return result;
    }

private:
    CANObjectInterface *_objects[_max_objects];
    uint8_t _objects_idx = 0;
    static_assert(_max_objects <= UINT8_MAX); // static _objects_idx overflow check

    send_function_t _send_func;

    // can_id_t _id_mask = 0b0000000000000000;

    uint32_t _last_tick = 0;
};
