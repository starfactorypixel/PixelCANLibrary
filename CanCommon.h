#pragma once

#include <stdint.h>

#define CAN_FRAME_MAX_PAYLOAD 7 // excluding the function ID

typedef uint16_t can_object_id_t;
const can_object_id_t CAN_SYSTEM_ID_BROADCAST = 0x0000;

// CAN Function IDs
enum can_function_id_t : uint8_t
{
    CAN_FUNC_NONE = 0x00,

    CAN_FUNC_SET_IN = 0x01,
    CAN_FUNC_TOGGLE_IN = 0x02,
    CAN_FUNC_ACTION_IN = 0x03,

    CAN_FUNC_SET_REAL_TIME_IN = 0x0A,

    CAN_FUNC_LOCK_IN = 0x10,
    CAN_FUNC_LOCK_OUT_OK = 0x50,
    CAN_FUNC_LOCK_OUT_ERR = 0xD0,

    CAN_FUNC_REQUEST_IN = 0x11,

    CAN_FUNC_TIMER_NORMAL = 0x61,
    CAN_FUNC_TIMER_WARNING = 0x62,
    CAN_FUNC_TIMER_CRITICAL = 0x63,

    CAN_FUNC_EVENT_OK = 0x65,
    CAN_FUNC_EVENT_ERROR = 0xE6,

    CAN_FUNC_SYSTEM_REQUEST_IN = 0x3A,
    CAN_FUNC_SYSTEM_REQUEST_OUT_OK = 0x7A,
    // CAN_FUNC_SYSTEM_REQUEST_OUT_ERR = not allowed, 0xFA

    // CAN_FUNC_FIRST_IN = 0x00, // == CAN_FUNC_NONE
    CAN_FUNC_FIRST_OUT_OK = 0x40,
    CAN_FUNC_FIRST_OUT_UNUSED = 0x80,
    CAN_FUNC_FIRST_OUT_ERR = 0xC0,
};

using can_send_function_t = void (*)(can_object_id_t id, uint8_t *data, uint8_t length);

struct can_frame_t
{
    can_object_id_t object_id = 0x0000;
    union
    {
        uint8_t raw_data[CAN_FRAME_MAX_PAYLOAD + 1] = {0};
        struct
        {
            can_function_id_t function_id;
            uint8_t data[CAN_FRAME_MAX_PAYLOAD];
        };
    };
    uint8_t raw_data_length = 0;
    bool initialized = false;
    uint32_t time_ms = 0;
};
