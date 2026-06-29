#pragma once

#include <stdint.h>

#define RAW_CAN_FRAME_MAX_PAYLOAD 8

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

    // CAN_FUNC_SYSTEM_REQUEST_IN = 0x3A, // was deleted in last protocol definition
    // CAN_FUNC_SYSTEM_REQUEST_OUT_OK = 0x7A, // was deleted in last protocol definition
    // CAN_FUNC_SYSTEM_REQUEST_OUT_ERR = not allowed, 0xFA

    // CAN_FUNC_FIRST_IN = 0x00, // == CAN_FUNC_NONE
    CAN_FUNC_FIRST_OUT_OK = 0x40,
    CAN_FUNC_FIRST_OUT_UNUSED = 0x80,
    CAN_FUNC_FIRST_OUT_ERR = 0xC0,
};

// expected behavior for CAN_Send function:
//     It puts CAN frame into hardware outgoing queue and returns `true`.
//     If the outgoing queue is full the CAN_Send function should return `false`.
using can_send_function_t = bool (*)(can_object_id_t id, uint8_t *data, uint8_t length);

struct can_frame_t
{
    can_object_id_t object_id = 0x0000;
    uint8_t raw_data[RAW_CAN_FRAME_MAX_PAYLOAD] = {0};
    uint8_t raw_data_length = 0;
};
