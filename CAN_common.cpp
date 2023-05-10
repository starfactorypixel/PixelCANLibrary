#include <string.h>
#include "CAN_common.h"
#include "logger.h"

/// @brief Clears all attributes of CAN frame
/// @param can_frame CAN frame to clear
void clear_can_frame_struct(can_frame_t &can_frame)
{
    can_frame.object_id = 0;
    memset(can_frame.raw_data, 0, sizeof(can_frame.raw_data));
    can_frame.raw_data_length = 0;
    can_frame.initialized = false;
}

/// @brief Clears all attributes of CAN error structure
/// @param error CAN error to clear
void clear_can_error_struct(can_error_t &error)
{
    error.function_id = CAN_FUNC_NONE;
    error.error_section = ERROR_SECTION_NONE;
    error.error_code = 0;
}

/// @brief Debug logger function: decodes function ID to to human-readable string.
/// @param function_id ID of the function.
/// @return Null-terminated string with name of the function
const char *get_function_name(can_function_id_t function_id)
{
#ifdef DEBUG
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
#else  // DEBUG
    return "detailed names are disabled";
#endif // DEBUG
}

/// @brief Debug logger function: decodes timer type to human-readable string.
/// @param timer_type Type of the timer.
/// @return Null-terminated string with name of the timer type.
const char *get_timer_type_name(timer_type_t timer_type)
{
#ifdef DEBUG
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
#else  // DEBUG
    return "detailed names are disabled";
#endif // DEBUG
}

/// @brief Debug logger function: decodes event type to human-readable string.
/// @param event_type Type of the event.
/// @return Null-terminated string with name of the event type.
const char *get_event_type_name(event_type_t event_type)
{
#ifdef DEBUG
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
#else  // DEBUG
    return "detailed names are disabled";
#endif // DEBUG
}

/// @brief Debug logger function: decodes error section & code to human-readable string.
/// @param error_section Error section to decode.
/// @param error_code Error code to decode.
/// @return Null-terminated string with name of the event type.
const char *get_error_code_name_for_section(error_section_t error_section, uint8_t error_code)
{
#ifdef DEBUG
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
        
        case ERROR_CODE_OBJECT_INCORRECT_FUNCTION_WORKFLOW:
            return "error: section [CANObject], code [incorrect function workflow]";

        case ERROR_CODE_OBJECT_SOMETHING_WRONG:
            return "error: section [CANObject], code [something went wrong]";

        default:
            return "error: section [CANObject], code [unknown]";
        }
        break;

    default:
        return "error: section [unknown], code [-]";
    }
#else  // DEBUG
    return "detailed names are disabled";
#endif // DEBUG
}

/// @brief Debug print of the CAN frame structure to the system console or to the UART serial
/// @param can_frame CAN frame to print
void log_can_frame(can_frame_t &can_frame)
{
#ifdef DEBUG
    LOGwoN("object id = 0x%04X, can frame: [", can_frame.object_id);
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
#endif // DEBUG
}

/// @brief Debug print of the CAN frame structure to the system console or to the UART serial
/// @param id CAN object ID to print
/// @param data CAN frame data to print
/// @param length The length of the CAN frame data
void log_can_frame(can_object_id_t id, uint8_t *data, uint8_t length)
{
#ifdef DEBUG
    can_frame_t can_frame;
    can_frame.object_id = id;
    memcpy(can_frame.raw_data, data, length);
    can_frame.raw_data_length = length;
    can_frame.initialized = true;
    log_can_frame(can_frame);
#endif // DEBUG
}
