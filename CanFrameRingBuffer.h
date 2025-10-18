#pragma once

#include "CanCommon.h"
#include <string.h>  // for memcpy

/// @brief Simple ring buffer for CAN frames
/// @tparam _buffer_size - Size of the buffer, measured in number of CAN frame structures
template <uint8_t _buffer_size>
class CanFrameRingBuffer
{
    static_assert(_buffer_size <= UINT8_MAX); // static overflow check

private:
    can_frame_t _buffer[_buffer_size];
    volatile uint8_t _head = 0;
    volatile uint8_t _tail = 0;

public:
    /// @brief Returns the capacity of the buffer
    /// @return Capacity of the buffer
    uint8_t capacity() const
    {
        return _buffer_size;
    }

    /// @brief Returns number of CAN frames currently stored in the buffer
    /// @return Number of CAN frames currently stored in the buffer
    uint8_t dataLength() const
    {
        if (_head >= _tail)
            return _head - _tail;
        else
            return _buffer_size - (_tail - _head);
    }

    /// @brief Pushes CAN frame into the buffer
    /// @param data - CAN frame to push
    /// @return 'true' if the frame was pushed, 'false' if the buffer is full or data is invalid
    bool push(can_frame_t data)
    {
        return push(data.object_id, data.raw_data, data.raw_data_length);
    }

    /// @brief Pushes CAN frame into the buffer
    /// @param id - CAN object ID
    /// @param data - Pointer to the data array
    /// @param length - Data length
    /// @return 'true' if the frame was pushed, 'false' if the buffer is full or data is invalid
    bool push(can_object_id_t id, uint8_t *data, uint8_t length)
    {
        if (isFull() || data == nullptr || length == 0 || length > CAN_FRAME_MAX_PAYLOAD + 1)
            return false;

        _buffer[_head].object_id = id;
        memcpy(_buffer[_head].raw_data, data, length);
        _buffer[_head].raw_data_length = length;
        _buffer[_head].initialized = true;

        _head = (_head + 1) % _buffer_size;
        return true;
    }

    /// @brief Returns reference to the next writable CAN frame in the buffer.
    ///        You need to check if the buffer is not full before write any data.
    /// @return Reference to the next writable CAN frame in the buffer
    can_frame_t &getWriteFrame()
    {
        return _buffer[_head];
    }

    /// @brief Pops one CAN frame from the buffer
    /// @param data - Reference to the CAN frame structure to fill with data
    /// @return 'true' if a CAN frame was popped, 'false' if the buffer is empty
    bool pop(can_frame_t &data)
    {
        if (isEmpty())
            return false;

        data = _buffer[_tail];
        _tail = (_tail + 1) % _buffer_size;
        return true;
    }

    /// @brief Removes one CAN frame from the buffer without copying it
    /// @return 'true' if a CAN frame was removed, 'false' if the buffer is empty
    bool remove()
    {
        if (isEmpty())
            return false;

        _tail = (_tail + 1) % _buffer_size;
        return true;
    }

    /// @brief Returns reference to the oldest CAN frame in the buffer
    ///        You need to check if the buffer is not empty before read any data.
    /// @return Reference to the oldest CAN frame in the buffer
    can_frame_t &peek()
    {
        return _buffer[_tail];
    }

    /// @brief Checks if the buffer is empty
    /// @return 'true' if the buffer is empty, 'false' if not
    bool isEmpty() const
    {
        return _head == _tail;
    }

    /// @brief Checks if the buffer is full
    /// @return 'true' if the buffer is full, 'false' if not
    bool isFull() const
    {
        return (_head + 1) % _buffer_size == _tail;
    }
};