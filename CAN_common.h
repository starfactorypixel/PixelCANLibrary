#ifndef CAN_COMMON_H
#define CAN_COMMON_H

#include <stdint.h>

// base CAN frame format uses 11-bit IDs (uint16)
// extended CAN frame format uses 29-bit IDs (uint32)
typedef uint16_t can_object_id_t;

#define CAN_FRAME_MAX_PAYLOAD 7 // excluding the function ID
#define CAN_TIMER_DISABLED UINT16_MAX
#define CAN_ERROR_DISABLED UINT16_MAX

// CAN Function IDs
enum can_function_id_t : uint8_t
{
    CAN_FUNC_NONE = 0x00,

    CAN_FUNC_SET_IN = 0x01,
    CAN_FUNC_SET_OUT_OK = 0x41,
    CAN_FUNC_SET_OUT_ERR = 0xC1,

    CAN_FUNC_REQUEST_IN = 0x11,
    CAN_FUNC_REQUEST_OUT_OK = 0x51,
    CAN_FUNC_REQUEST_OUT_ERR = 0xD1,

    CAN_FUNC_SEND_RAW_INIT_IN = 0x30,
    CAN_FUNC_SEND_RAW_INIT_OUT_OK = 0x70,
    CAN_FUNC_SEND_RAW_INIT_OUT_ERR = 0xF0,

    CAN_FUNC_SEND_RAW_CHUNK_START_IN = 0x31,
    CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK = 0x71,
    CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR = 0xF1,

    CAN_FUNC_SEND_RAW_CHUNK_DATA_IN = 0x32,
    // CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_OK = not allowed,
    CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR = 0xF2,

    CAN_FUNC_SEND_RAW_CHUNK_END_IN = 0x33,
    CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK = 0x73,
    CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR = 0xF3,

    CAN_FUNC_SEND_RAW_FINISH_IN = 0x34,
    CAN_FUNC_SEND_RAW_FINISH_OUT_OK = 0x74,
    CAN_FUNC_SEND_RAW_FINISH_OUT_ERR = 0xF4,

    CAN_FUNC_TIMER_NORMAL = 0x61,
    CAN_FUNC_TIMER_WARNING = 0x62,
    CAN_FUNC_TIMER_CRITICAL = 0x63,

    CAN_FUNC_EVENT_OK = 0x65,
    CAN_FUNC_EVENT_ERROR = 0xE6,

    // CAN_FUNC_FIRST_IN = 0x00, // == CAN_FUNC_NONE
    CAN_FUNC_FIRST_OUT_OK = 0x40,
    CAN_FUNC_FIRST_OUT_UNUSED = 0x80,
    CAN_FUNC_FIRST_OUT_ERR = 0xC0,
};

#ifdef DEBUG
const char *get_function_name(can_function_id_t function_id)
{
    switch (function_id)
    {
    case CAN_FUNC_NONE:
        return "none";

    case CAN_FUNC_SET_IN:
        return "set:in";

    case CAN_FUNC_SET_OUT_OK:
        return "set:out-ok";

    case CAN_FUNC_SET_OUT_ERR:
        return "set:out-error";

    case CAN_FUNC_REQUEST_IN:
        return "request:in";

    case CAN_FUNC_REQUEST_OUT_OK:
        return "request:out-ok";

    case CAN_FUNC_REQUEST_OUT_ERR:
        return "request:out-error";

    case CAN_FUNC_TIMER_NORMAL:
        return "timer:normal";

    case CAN_FUNC_TIMER_WARNING:
        return "timer:warning";

    case CAN_FUNC_TIMER_CRITICAL:
        return "timer:critical";

    case CAN_FUNC_EVENT_OK:
        return "event:normal";

    case CAN_FUNC_EVENT_ERROR:
        return "event:error";

    case CAN_FUNC_FIRST_OUT_OK:
    case CAN_FUNC_FIRST_OUT_UNUSED:
    case CAN_FUNC_FIRST_OUT_ERR:
    case CAN_FUNC_SEND_RAW_INIT_IN:
    case CAN_FUNC_SEND_RAW_INIT_OUT_OK:
    case CAN_FUNC_SEND_RAW_INIT_OUT_ERR:
    case CAN_FUNC_SEND_RAW_CHUNK_START_IN:
    case CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK:
    case CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR:
    case CAN_FUNC_SEND_RAW_CHUNK_DATA_IN:
    case CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR:
    case CAN_FUNC_SEND_RAW_CHUNK_END_IN:
    case CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK:
    case CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR:
    case CAN_FUNC_SEND_RAW_FINISH_IN:
    case CAN_FUNC_SEND_RAW_FINISH_OUT_OK:
    case CAN_FUNC_SEND_RAW_FINISH_OUT_ERR:
    default:
        return "unknown";
    }
}
#else  // DEBUG
const char *get_function_name(can_function_id_t function_id)
{
    return "";
}
#endif // DEBUG

using can_send_function_t = void (*)(can_object_id_t id, uint8_t *data, uint8_t length);

// CANFrame data structure
// It can be changed to class later (in case we need it)
struct __attribute__((__packed__)) can_frame_t
{
    union
    {
        uint8_t raw_data[CAN_FRAME_MAX_PAYLOAD + 1];
        struct
        {
            can_function_id_t function_id;
            uint8_t data[CAN_FRAME_MAX_PAYLOAD];
        };
    };
    uint8_t raw_data_length;
    bool initialized;
};
// can_function_id_t must have a size of 1 byte
// otherwise we need to update can_frame_t structure
static_assert(sizeof(can_function_id_t) == 1);

// #define CAN_TIMER_TYPE_MASK 0b00001111
enum timer_type_t : uint8_t
{
    CAN_TIMER_TYPE_NONE = 0b00000000,
    CAN_TIMER_TYPE_NORMAL = 0b00000001,
    CAN_TIMER_TYPE_WARNING = 0b00000010,
    CAN_TIMER_TYPE_CRITICAL = 0b00000011,

    CAN_TIMER_TYPE_MASK = 0b00001111,
};

#ifdef DEBUG
const char *get_timer_type_name(timer_type_t timer_type)
{
    switch (timer_type)
    {
    case CAN_TIMER_TYPE_NONE:
        return "timer type: none";

    case CAN_TIMER_TYPE_NORMAL:
        return "timer type: normal";

    case CAN_TIMER_TYPE_WARNING:
        return "timer type: warning";

    case CAN_TIMER_TYPE_CRITICAL:
        return "timer type: critical";

    case CAN_TIMER_TYPE_MASK:
        return "timer type: mask";
    default:
        return "timer type: unknown";
    }
}
#else  // DEBUG
const char *get_timer_type_name(timer_type_t timer_type)
{
    return "";
}
#endif // DEBUG

// #define CAN_EVENT_TYPE_MASK 0b11110000
enum event_type_t : uint8_t
{
    CAN_EVENT_TYPE_NONE = 0b00000000,
    CAN_EVENT_TYPE_NORMAL = 0b00010000,
    CAN_EVENT_TYPE_ERROR = 0b00100000,

    CAN_EVENT_TYPE_MASK = 0b11110000,
};

#ifdef DEBUG
const char *get_event_type_name(event_type_t event_type)
{
    switch (event_type)
    {
    case CAN_EVENT_TYPE_NONE:
        return "event type: none";

    case CAN_EVENT_TYPE_NORMAL:
        return "event type: normal";

    case CAN_EVENT_TYPE_ERROR:
        return "event type: error";

    case CAN_EVENT_TYPE_MASK:
        return "event type: mask";
    default:
        return "event type: unknown";
    }
}
#else  // DEBUG
const char *get_event_type_name(event_type_t event_type)
{
    return "";
}
#endif // DEBUG

enum error_section_t : uint8_t
{
    ERROR_SECTION_NONE = 0x00,
    ERROR_SECTION_CAN_MANAGER = 0x01,
    ERROR_SECTION_CAN_OBJECT = 0x02,
};

enum error_code_object_t : uint8_t
{
    ERROR_CODE_OBJECT_NONE = 0x00,
    ERROR_CODE_OBJECT_UNSUPPORTED_EVENT_TYPE = 0x01,
    ERROR_CODE_OBJECT_UNSUPPORTED_TIMER_TYPE = 0x02,
    ERROR_CODE_OBJECT_SET_FUNCTION_IS_MISSING = 0x03,
    ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION = 0x04,
    ERROR_CODE_OBJECT_INCORRECT_REQUEST = 0x05,

    ERROR_CODE_OBJECT_SOMETHING_WRONG = 0xFF, // TODO: used for debug and as a temporary value; should be replaced later with correct code
};

#ifdef DEBUG
const char *get_error_code_name_for_section(error_section_t error_section, uint8_t error_code)
{
    switch (error_section)
    {
    case ERROR_SECTION_NONE:
        return "error: section [none], code [-]";

    case ERROR_SECTION_CAN_MANAGER:
        return "error: section [CANManager], code [-]";

    case ERROR_SECTION_CAN_OBJECT:
        switch ((error_code_object_t)error_code)
        {
        case ERROR_CODE_OBJECT_NONE:
            return "error: section [CANObject], code [none]";

        case ERROR_CODE_OBJECT_UNSUPPORTED_EVENT_TYPE:
            return "error: section [CANObject], code [unsupported event type]";

        case ERROR_CODE_OBJECT_UNSUPPORTED_TIMER_TYPE:
            return "error: section [CANObject], code [unsupported timer type]";

        case ERROR_CODE_OBJECT_SET_FUNCTION_IS_MISSING:
            return "error: section [CANObject], code [set function is missing]";

        case ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION:
            return "error: section [CANObject], code [unsupported function]";

        case ERROR_CODE_OBJECT_INCORRECT_REQUEST:
            return "error: section [CANObject], code [incorrect request]";

        case ERROR_CODE_OBJECT_SOMETHING_WRONG:
            return "error: section [CANObject], code [something went wrong]";

        default:
            return "error: section [CANObject], code [unknown]";
        }
        break;

    default:
        return "error: section [unknown], code [-]";
    }
}
#else  // DEBUG
const char *get_error_code_name_for_section(error_section_t error_section, uint8_t error_code)
{
    return "";
}
#endif // DEBUG

struct can_error_t
{
    can_function_id_t function_id;
    error_section_t error_section;
    uint8_t error_code;
};

using event_handler_t = void (*)(can_frame_t &can_frame, event_type_t event_type, can_error_t &error);
using timer_handler_t = void (*)(can_frame_t &can_frame, timer_type_t timer_type, can_error_t &error);
using request_handler_t = void (*)(can_frame_t &can_frame, can_error_t &error);
using set_handler_t = void (*)(can_frame_t &can_frame, can_error_t &error);

#ifdef DEBUG
void log_can_frame(can_frame_t &can_frame)
{
    LOGwoN("can frame: [");
    for (uint8_t i = 0; i < can_frame.raw_data_length; i++)
    {
        if (i == 0)
        {
            LOGstring("0x%02X (%s)%s", can_frame.function_id,
                      get_function_name(can_frame.function_id),
                      (i == can_frame.raw_data_length - 1) ? "" : ", ");
        }
        else if (can_frame.function_id == CAN_FUNC_EVENT_ERROR && i == 2)
        {
            LOGstring("0x%02X (%s)%s", can_frame.raw_data[i],
                      get_error_code_name_for_section((error_section_t)can_frame.raw_data[1], can_frame.raw_data[2]),
                      (i == can_frame.raw_data_length - 1) ? "" : ", ");
        }
        else
        {
            LOGstring("0x%02X%s", can_frame.raw_data[i], (i == can_frame.raw_data_length - 1) ? "" : ", ");
        }
    }
    LOGstring("]\n");
}

void log_can_frame(can_object_id_t id, uint8_t *data, uint8_t length)
{
    can_frame_t can_frame;
    memcpy(can_frame.raw_data, data, length);
    can_frame.raw_data_length = length;
    can_frame.initialized = true;
    LOGwoN("object id = 0x%04X, ", id);
    log_can_frame(can_frame);
}
#else  // DEBUG
void log_can_frame(can_frame_t &can_frame)
{
}

void log_can_frame(can_object_id_t id, uint8_t *data, uint8_t length)
{
}
#endif // DEBUG

#endif // CAN_COMMON_H