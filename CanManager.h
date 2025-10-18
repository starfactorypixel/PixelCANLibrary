#pragma once

#include <stdint.h>
#include <string.h> //memcpy
#include <cassert>
#include "CanCommon.h"
#include "CanHelpers.h"
#include "CanManagerInterface.h"
#include "CanObjectInterface.h"

/// @brief CANManager manages all the CANObject of the board/system and performs base frame processing
/// @tparam _max_objects — The maximum number of CANObjects which can be handle by CANManager
/// @tparam _can_frame_buffer_size — The size of buffer, measured in number of CAN frame structures
/// @tparam _tick_time — ms, the minimal period between CANManager::Process() informative calls
template <uint8_t _max_objects, uint8_t _can_frame_buffer_size = 16, uint8_t _tick_time = 10>
class CANManager: public CanManagerInterface
{
private:
    // buffers for incoming and outgoing can frames
    CanFrameRingBuffer<_can_frame_buffer_size> _rx_buffer;
    CanFrameRingBuffer<_can_frame_buffer_size> _tx_buffer;

    // registered CANObjects of the CANManager
    CANObjectInterface *_objects[_max_objects] = {nullptr};
    uint8_t _objects_idx = 0;
    static_assert(_max_objects > 0);          // 0 objects is not allowed
    static_assert(_max_objects <= UINT8_MAX); // static _objects_idx overflow check

    // can_send_function_t _send_func = nullptr;

    uint32_t _last_tick = 0;

    /// @brief Checks if specified CAN function is allowed in broadcast mode
    /// @param func_id CAN function ID for check
    /// @return 'true' if CAN function is allowed, 'false' if it is not.
    bool _isBroadcastFunctionAllowed(can_function_id_t func_id)
    {
        //  broadcast of "request", "system request" and "lock" functions only are allowed
        return func_id == CAN_FUNC_REQUEST_IN ||
               func_id == CAN_FUNC_SYSTEM_REQUEST_IN ||
               func_id == CAN_FUNC_LOCK_IN;
    }

public:
    /*
    /// @brief Default constructor is disabled
    CANManager() = delete;

    /// @brief Creates CANManager and specifies external function, which sends CAN frames
    /// @param can_send_func Pointer to an external CAN frames sending handler
    CANManager(can_send_function_t can_send_func)
        : _send_func(can_send_func) {};
    */
    CANManager() {};
    virtual ~CANManager() = default;

    /// @brief Registers specified CANObject
    /// @param can_object CANObject for registration
    /// @return 'true' if registration was successful, 'false' if not
    virtual bool registerObject(CANObjectInterface &can_object) override
    {
        if (_max_objects <= _objects_idx)
            return false;

        _objects[_objects_idx++] = &can_object;

        return true;
    }

    /// @brief Checks if CANObject is registered in CANManager
    /// @param id ID of the CANObject to check
    /// @return Return 'true' if the CANObject is registered, 'false' if it is not
    virtual bool hasCanObject(can_object_id_t id) override
    {
        return getCanObject(id) != nullptr;
    }

    /// @brief Searches for the CANObject among the registered ones
    /// @param id ID of the CANObject to search
    /// @return 'pointer to CANObjectInterface' if this object is registered,
    ///         'nullptr' if CANObject was not found.
    virtual CANObjectInterface *getCanObject(can_object_id_t id) override
    {
        for (uint8_t i = 0; i < _objects_idx; i++)
        {
            if (_objects[i]->getId() == id)
                return _objects[i];
        }
        return nullptr;
    }

    /// @brief Checks if there are CAN frames to send in the outgoing buffer
    /// @return 'true' if there are frames to send, 'false' if not
    virtual bool hasFrameToSend() override
    {
        return !_tx_buffer.isEmpty();
    }

    /// @brief Gets one CAN frame from the outgoing buffer to send it
    /// @param can_frame Reference to the CAN frame structure to fill with data
    /// @return 'true' if a CAN frame was retrieved, 'false' if the buffer is empty
    virtual bool getFrameToSend(can_frame_t &can_frame) override
    {
        return _tx_buffer.pop(can_frame);
    }

    /// @brief Performs CANObjects processing
    /// @param time Current time
    virtual void tick(uint32_t time) override
    {
        if (time - _last_tick < _tick_time)
            return;

        _last_tick = time;

        // TODO: do we need some status reporting here?
        if (_tx_buffer.isFull())
            return;

        CANObjectInterface *can_object = nullptr;
        can_frame_t *can_frame = nullptr;
        while (!_rx_buffer.isEmpty())
        {
            can_frame = &_rx_buffer.peek();
            can_frame->time_ms = time; // assume CAN frame comes now

            // transfer broadcast frames to all registered CAN-Objects
            // don't check if function is allowed, it was checked on input
            if (can_frame->object_id == CAN_SYSTEM_ID_BROADCAST)
            {
                for (uint8_t i = 0; i < _objects_idx; i++)
                {
                    _objects[i]->setRxFrame(*can_frame);
                    // TODO: need to check if tx frame was filled 
                    _objects[i]->setTxFrame(_tx_buffer.getWriteFrame());
                    _objects[i]->process(time);
                }
            }
            else
            {
                can_object = getCanObject(can_frame->object_id);
                if (can_object != nullptr)
                {
                    can_object->setRxFrame(*can_frame);
                    can_object->setTxFrame(_tx_buffer.getWriteFrame());
                    can_object->process(time);
                }
            }

            _rx_buffer.remove();
        }

        /*
        for (uint8_t i = 0; i < _frame_buffer_index; i++)
        {

            // transfer broadcast frames to all registered CAN-Objects
            // don't check if function is allowed, it was checked on input
            if (_can_frame_buffer[i].object_id == CAN_SYSTEM_ID_BROADCAST)
            {
                if (!_IsBroadcastFunctionAllowed(_can_frame_buffer[i].function_id))
                    continue;

                can_frame_t broadcast_can_frame;
                for (uint8_t obj_idx = 0; obj_idx < _objects_idx; ++obj_idx)
                {
                    clear_can_frame_struct(broadcast_can_frame);
                    clear_can_frame_struct(outgoing_can_frame);
                    copy_can_frame_struct(broadcast_can_frame, _can_frame_buffer[i]);
                    if (CAN_RESULT_IGNORE == _objects[obj_idx]->InputCanFrame(broadcast_can_frame, outgoing_can_frame))
                        continue;

                    _ValidateAndFillErrorCanFrame(outgoing_can_frame, _tx_error);
                    outgoing_can_frame.object_id = _objects[obj_idx]->GetId();
                    _SendCanData(outgoing_can_frame);
                }
            }
            // process all frames for specific CAN-Objects
            else
            {
                can_object = GetCanObject(_can_frame_buffer[i].object_id);
                if (can_object == nullptr)
                    continue;
                clear_can_frame_struct(outgoing_can_frame);
                if (CAN_RESULT_IGNORE == can_object->InputCanFrame(_can_frame_buffer[i], outgoing_can_frame))
                    continue;

                _ValidateAndFillErrorCanFrame(outgoing_can_frame, _tx_error);
                outgoing_can_frame.object_id = can_object->GetId();
                _SendCanData(outgoing_can_frame);
            }
            _can_frame_buffer[i].initialized = false;
        }
        */

        /*
        can_frame_t outgoing_can_frame;

        // Process all incoming CAN frames in the buffer
        if (_frame_buffer_index > 0)
        {
            CANObjectInterface *can_object = nullptr;
            for (uint8_t i = 0; i < _frame_buffer_index; i++)
            {

                // set time for canframe (assume CAN frame comes now)
                _can_frame_buffer[i].time_ms = time;

                // transfer broadcast frames to all registered CAN-Objects
                if (_can_frame_buffer[i].object_id == CAN_SYSTEM_ID_BROADCAST)
                {
                    if (!_IsBroadcastFunctionAllowed(_can_frame_buffer[i].function_id))
                        continue;

                    can_frame_t broadcast_can_frame;
                    for (uint8_t obj_idx = 0; obj_idx < _objects_idx; ++obj_idx)
                    {
                        clear_can_frame_struct(broadcast_can_frame);
                        clear_can_frame_struct(outgoing_can_frame);
                        copy_can_frame_struct(broadcast_can_frame, _can_frame_buffer[i]);
                        if (CAN_RESULT_IGNORE == _objects[obj_idx]->InputCanFrame(broadcast_can_frame, outgoing_can_frame))
                            continue;

                        _ValidateAndFillErrorCanFrame(outgoing_can_frame, _tx_error);
                        outgoing_can_frame.object_id = _objects[obj_idx]->GetId();
                        _SendCanData(outgoing_can_frame);
                    }
                }
                // process all frames for specific CAN-Objects
                else
                {
                    can_object = GetCanObject(_can_frame_buffer[i].object_id);
                    if (can_object == nullptr)
                        continue;
                    clear_can_frame_struct(outgoing_can_frame);
                    if (CAN_RESULT_IGNORE == can_object->InputCanFrame(_can_frame_buffer[i], outgoing_can_frame))
                        continue;

                    _ValidateAndFillErrorCanFrame(outgoing_can_frame, _tx_error);
                    outgoing_can_frame.object_id = can_object->GetId();
                    _SendCanData(outgoing_can_frame);
                }
                _can_frame_buffer[i].initialized = false;
            }
            // buffer is clear, we can write new frame(s) at the first position of buffer
            _frame_buffer_index = 0;
        }

        clear_can_error_struct(_tx_error);
        clear_can_frame_struct(outgoing_can_frame);

        // Process automatic functions of CANObjects
        for (uint8_t i = 0; i < _objects_idx; ++i)
        {
            if (CAN_RESULT_IGNORE == _objects[i]->Process(time, outgoing_can_frame))
                continue;

            _ValidateAndFillErrorCanFrame(outgoing_can_frame, _tx_error);

            // restoring ID (if it was overwritten by the handler)
            outgoing_can_frame.object_id = _objects[i]->GetId();

            _SendCanData(outgoing_can_frame);
        }
        */
    }

    /// @brief Stores incoming CAN frame in the buffer.
    ///        Frame processing will start when the Process() method is called the next time.
    /// @param id CANObject ID from the CAN frame
    /// @param data Pointer to the data array
    /// @param length Data length
    /// @return true if data length exceeds 0 and a CANObject with the ID is registered, false if not
    virtual bool incomingCANFrame(can_object_id_t id, uint8_t *data, uint8_t length) override
    {
        if (data == nullptr || length == 0)
            return false;

        if (!hasCanObject(id) && id != CAN_SYSTEM_ID_BROADCAST)
            return false;

        if (id == CAN_SYSTEM_ID_BROADCAST && !_isBroadcastFunctionAllowed((can_function_id_t)data[0]))
            return false;

        return _rx_buffer.push(id, data, length);
    }

    /// @brief Gets the current length of the receive frame buffer
    /// @return Number of CAN frames currently stored in the receive buffer
    virtual uint8_t getRxFrameBufferLength() override
    {
        return _rx_buffer.dataLength();
    }

    /// @brief Gets the current length of the transmit frame buffer
    /// @return Number of CAN frames currently stored in the transmit buffer
    virtual uint8_t getTxFrameBufferLength() override
    {
        return _tx_buffer.dataLength();
    }
};
