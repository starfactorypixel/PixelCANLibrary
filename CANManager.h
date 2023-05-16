#pragma once

#include <stdint.h>
// #include <string.h>
#include <cassert>
#include "CAN_common.h"
#include "CANObject.h"

/******************************************************************************************
 *
 ******************************************************************************************/
class CANManagerInterface
{
public:
    virtual ~CANManagerInterface() = default;

    /// @brief Registers specified CANObject
    /// @param can_object CANObject for registration
    /// @return 'true' if registration was successful, 'false' if not
    virtual bool RegisterObject(CANObjectInterface &can_object) = 0;

    /// @brief Registers low level function, that sends data via CAN bus
    /// @param can_send_func Pointer to the function
    virtual void RegisterSendFunction(can_send_function_t can_send_func) = 0;

    /// @brief Performs CANObjects processing
    /// @param time Current time
    virtual void Process(uint32_t time) = 0;

    /// @brief Processes incoming CAN frame (without any queues?)
    /// @param id CANObject ID from the CAN frame
    /// @param data Pointer to the data array
    /// @param length Data length
    /// @return true if CANObject with ID is registered, false if not
    virtual bool IncomingCANFrame(can_object_id_t id, uint8_t *data, uint8_t length) = 0;
};

/******************************************************************************************
 *
 ******************************************************************************************/
template <uint8_t _max_objects = 16, uint8_t _can_frame_buffer_size = 16>
class CANManager : public CANManagerInterface
{
    static_assert(_max_objects > 0); // 0 objects is not allowed
public:
    /// @brief Default constructor is disabled
    CANManager() = delete;

    /// @brief Creates CANManager and specifies external function, which sends CAN frames
    /// @param can_send_func Pointer to an external CAN frames sending handler
    CANManager(can_send_function_t can_send_func)
        : _send_func(can_send_func){};

    /// @brief Registers specified CANObject
    /// @param can_object CANObject for registration
    /// @return 'true' if registration was successful, 'false' if not
    virtual bool RegisterObject(CANObjectInterface &can_object) override
    {
        if (_max_objects <= _objects_idx)
            return false;

        _objects[_objects_idx++] = &can_object;

        return true;
    }

    /// @brief Returns the number of CANObjects, which are registered in CANManager
    /// @return The number of CANObjects, which are registered in CANManager
    uint8_t GetObjectsCount()
    {
        return _objects_idx;
    }

    /// @brief Checks if CANObject is registered in CANManager
    /// @param id ID of the CANObject to check
    /// @return Return 'true' if the CANObject is registered, 'false' if it is not
    bool HasCanObject(can_object_id_t id)
    {
        return GetCanObject(id) != nullptr;
    }

    /// @brief Searches for the CANObject among the registered ones
    /// @param id ID of the CANObject to search
    /// @return 'pointer to CANObjectInterface' if this object is registered,
    ///         'nullptr' if CANObject was not found.
    CANObjectInterface *GetCanObject(can_object_id_t id)
    {
        for (uint8_t i = 0; i < _objects_idx; i++)
        {
            if (_objects[i]->GetId() == id)
                return _objects[i];
        }
        return nullptr;
    }

    /// @brief Returns The number of CAN frames stored in the buffer.
    /// @return The number of CAN frames stored in the buffer.
    uint8_t GetNumOfFramesInBuffer()
    {
        return _frame_buffer_index;
    }

    /// @brief Registers low level function, that sends data via CAN bus
    /// @param can_send_func Pointer to the function
    virtual void RegisterSendFunction(can_send_function_t can_send_func) override
    {
        if (can_send_func == nullptr)
            return;

        _send_func = can_send_func;
    }

    /// @brief Performs CANObjects processing
    /// @param time Current time
    virtual void Process(uint32_t time) override
    {
        if (time - _last_tick < _tick_time)
            return;

        _last_tick = time;

        // Process all CAN frames in the buffer
        if (_frame_buffer_index > 0)
        {
            CANObjectInterface *can_object = nullptr;
            for (uint8_t i = 0; i < _frame_buffer_index; i++)
            {
                can_object = GetCanObject(_can_frame_buffer[i].object_id);
                if (CAN_RESULT_IGNORE == can_object->InputCanFrame(_can_frame_buffer[i], _tx_error))
                {
                    _can_frame_buffer[i].initialized = false;
                    continue;
                }

                if (!_can_frame_buffer[i].initialized && _tx_error.error_section != ERROR_SECTION_NONE)
                {
                    _FillErrorCanFrame(_can_frame_buffer[i], _tx_error);
                }
                _SendCanData(_can_frame_buffer[i]);
                _can_frame_buffer[i].initialized = false;
            }
            // buffer is clear, we can write frame in the first item of buffer
            _frame_buffer_index = 0;
        }

        clear_can_error_struct(_tx_error);
        clear_can_frame_struct(_tx_can_frame);

        // Process automatic functions of CANObjects
        for (uint8_t i = 0; i < _objects_idx; ++i)
        {
            if (CAN_RESULT_IGNORE == _objects[i]->Process(time, _tx_can_frame, _tx_error))
                continue;

            if (!_tx_can_frame.initialized && _tx_error.error_section != ERROR_SECTION_NONE)
            {
                _FillErrorCanFrame(_tx_can_frame, _tx_error);
            }

            // restoring ID (if it was overwritten by the handler)
            _tx_can_frame.object_id = _objects[i]->GetId();

            _SendCanData(_tx_can_frame);
        }
    }

    /// @brief Stores incoming CAN framein the buffer.
    ///        Frame processing will start when the Process() method is called the next time.
    /// @param id CANObject ID from the CAN frame
    /// @param data Pointer to the data array
    /// @param length Data length
    /// @return true if data length exceeds 0 and a CANObject with the ID is registered, false if not
    virtual bool IncomingCANFrame(can_object_id_t id, uint8_t *data, uint8_t length) override
    {
        if (length == 0 || !HasCanObject(id))
            return false;

        _can_frame_buffer[_frame_buffer_index].object_id = id;
        memcpy(_can_frame_buffer[_frame_buffer_index].raw_data, data, length);
        _can_frame_buffer[_frame_buffer_index].raw_data_length = length;
        _can_frame_buffer[_frame_buffer_index].initialized = true;

        _frame_buffer_index++;
        if (_frame_buffer_index >= _can_frame_buffer_size)
            _frame_buffer_index = 0;

        return true;
    }

private:
    // don't do data processing very often
    // _tick_time is minimal delay in ms between the data processing
    static const uint8_t _tick_time = 10;

    // data structures for outgoing CAN frames & errors
    can_frame_t _tx_can_frame = {};
    can_error_t _tx_error = {};

    // buffer for incoming can frames
    // new frames will overwrite old ones regardless of can manager's processing speed
    can_frame_t _can_frame_buffer[_can_frame_buffer_size] = {};
    uint8_t _frame_buffer_index = 0;
    static_assert(_can_frame_buffer_size <= UINT8_MAX); // static _frame_buffer_index overflow check

    // registered CANObjects of the CANManager
    CANObjectInterface *_objects[_max_objects] = {nullptr};
    uint8_t _objects_idx = 0;
    static_assert(_max_objects <= UINT8_MAX); // static _objects_idx overflow check

    can_send_function_t _send_func = nullptr;

    uint32_t _last_tick = 0;

    /// @brief Sends data to the CAN bus with check if sending callback function is setted
    /// @param can_frame CAN frame data to send
    void _SendCanData(can_frame_t &can_frame)
    {
        if (_send_func == nullptr || !can_frame.initialized)
            return;

        _send_func(can_frame.object_id, can_frame.raw_data, can_frame.raw_data_length);

        clear_can_error_struct(_tx_error);
        clear_can_frame_struct(_tx_can_frame);
    }

    void _FillErrorCanFrame(can_frame_t &can_frame, can_error_t error)
    {
        if (error.error_section == ERROR_SECTION_NONE)
        {
            can_frame.initialized = false;
            return;
        }

        can_frame.initialized = true;
        if (error.function_id != CAN_FUNC_NONE)
        {
            can_frame.function_id = error.function_id;
        }
        else
        {
            can_frame.function_id = CAN_FUNC_EVENT_ERROR;
        }
        can_frame.data[0] = error.error_section;
        can_frame.data[1] = error.error_code;
        can_frame.raw_data_length = sizeof(can_frame.function_id) + 2;
    }
};
