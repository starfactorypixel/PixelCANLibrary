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
template <uint8_t _max_objects = 16>
class CANManager : public CANManagerInterface
{
    static_assert(_max_objects > 0); // 0 objects is not allowed
public:
    /// @brief Default constructor disabled
    CANManager() = delete;

    /// @brief Creates CANManager and specifies external function, which sends CAN frames
    /// @param can_send_func Pointer to an external CAN frames sending handler
    CANManager(can_send_function_t can_send_func)
    {
        RegisterSendFunction(can_send_func);
    };

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
    /// @return Returns the number of CANObjects, which are registered in CANManager
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

        can_frame_t can_frame;
        can_error_t error;
        error.function_id = CAN_FUNC_NONE;
        for (uint8_t i = 0; i < _objects_idx; ++i)
        {
            can_frame.initialized = false;
            _objects[i]->Process(time, can_frame, error);
            if (!can_frame.initialized && error.error_section != ERROR_SECTION_NONE)
            {
                _FillErrorCanFrame(can_frame, error);
            }
            _SendCanData(*_objects[i], can_frame);
        }
    }

    /// @brief Processes incoming CAN frame (without any queues?)
    /// @param id CANObject ID from the CAN frame
    /// @param data Pointer to the data array
    /// @param length Data length
    /// @return true if CANObject with ID is registered, false if not
    virtual bool IncomingCANFrame(can_object_id_t id, uint8_t *data, uint8_t length) override
    {
        if (length == 0)
            return false;

        CANObjectInterface *can_object = GetCanObject(id);
        if (can_object == nullptr)
            return false;

        can_error_t error;
        error.function_id = CAN_FUNC_NONE;
        can_frame_t can_frame;
        can_frame.raw_data_length = length;
        memcpy(&can_frame.raw_data, data, length);
        can_frame.initialized = true;

        can_object->InputCanFrame(can_frame, error);

        if (!can_frame.initialized && error.error_section != ERROR_SECTION_NONE)
        {
            _FillErrorCanFrame(can_frame, error);
        }
        _SendCanData(*can_object, can_frame);

        return true;
    }

private:
    // don't do data processing very often
    // _tick_time is minimal delay in ms between the data processing
    static const uint8_t _tick_time = 10;

    CANObjectInterface *_objects[_max_objects] = {nullptr};
    uint8_t _objects_idx = 0;
    static_assert(_max_objects <= UINT8_MAX); // static _objects_idx overflow check

    can_send_function_t _send_func = nullptr;

    uint32_t _last_tick = 0;

    /// @brief Sends data to the CAN bus with check if sending function exists
    /// @param can_object Sender object (its ID is used in the CAN frame)
    /// @param can_frame CAN frame data to send
    void _SendCanData(CANObjectInterface &can_object, can_frame_t &can_frame)
    {
        if (_send_func == nullptr || !can_frame.initialized)
            return;

        _send_func(can_object.GetId(), can_frame.raw_data, can_frame.raw_data_length);
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
