#include <string.h>
#include "CAN_common.h"
#include "CANObject.h"

/// @brief Clears all attributes of CAN frame
/// @param can_frame CAN frame to clear
void clear_can_frame_struct(can_frame_t &can_frame)
{
    can_frame.object_id = 0;
    memset(can_frame.raw_data, 0, sizeof(can_frame.raw_data));
    can_frame.raw_data_length = 0;
    can_frame.initialized = false;
    can_frame.time_ms = 0;
}

/// @brief Copies data from one CAN frame to another
/// @param dest_can_frame Destination CAN frame
/// @param src_can_frame Source CAN frame
void copy_can_frame_struct(can_frame_t &dest_can_frame, can_frame_t src_can_frame)
{
    memcpy(dest_can_frame.raw_data, src_can_frame.raw_data, sizeof(dest_can_frame.raw_data));
    dest_can_frame.initialized = src_can_frame.initialized;
    dest_can_frame.time_ms = src_can_frame.time_ms;
    dest_can_frame.object_id = src_can_frame.object_id;
    dest_can_frame.raw_data_length = src_can_frame.raw_data_length;
}

/// @brief Clears all attributes of CAN error structure
/// @param error CAN error to clear
void clear_can_error_struct(can_error_t &error)
{
    error.function_id = CAN_FUNC_NONE;
    error.error_section = ERROR_SECTION_NONE;
    error.error_code = 0;
}

/// @brief Common BlockInfo parameters will be applied to the specified CANObject.
///        All BlockInfo objects has:
///          - enabled timers (15000 ms period)
///          - enabled flood mode
///          - disabled events
///          - no external handlers for events, timers, set & request commends
/// @param block_sys_object Target CANObject
void set_block_info_params(CANObjectInterface &block_sys_object)
{
    block_sys_object.SetTimerFloodMode(true);
    block_sys_object.SetTimerPeriod(15000);
    block_sys_object.SetErrorEventDelay(CAN_ERROR_DISABLED);
    block_sys_object.SetObjectType(CAN_OBJECT_TYPE_SYSTEM_BLOCK_INFO);
    block_sys_object.RegisterFunctionEvent(nullptr);
    block_sys_object.RegisterFunctionRequest(nullptr);
    block_sys_object.RegisterFunctionSet(nullptr);
    block_sys_object.RegisterFunctionTimer(nullptr);
}

/// @brief Common BlockHealth parameters will be applied to the specified CANObject.
///        All BlockHealth objects has:
///          - disabled timers and flood mode
///          - enabled events (300 ms period)
///          - no external handlers for events, timers, set & request commends
/// @param block_sys_object Target CANObject
void set_block_health_params(CANObjectInterface &block_sys_object)
{
    block_sys_object.SetTimerFloodMode(false);
    block_sys_object.SetTimerPeriod(CAN_TIMER_DISABLED);
    block_sys_object.SetErrorEventDelay(300);
    block_sys_object.SetObjectType(CAN_OBJECT_TYPE_SYSTEM_BLOCK_HEALTH);
    block_sys_object.RegisterFunctionEvent(nullptr);
    block_sys_object.RegisterFunctionRequest(nullptr);
    block_sys_object.RegisterFunctionSet(nullptr);
    block_sys_object.RegisterFunctionTimer(nullptr);
};

/// @brief Common BlockFeatures parameters will be applied to the specified CANObject.
///        All BlockFeatures objects has:
///          - enabled timers (15000 ms period)
///          - enabled flood mode
///          - disabled events
///          - no external handlers for events, timers, set & request commends
/// @param block_sys_object Target CANObject
void set_block_features_params(CANObjectInterface &block_sys_object)
{
    block_sys_object.SetTimerFloodMode(true);
    block_sys_object.SetTimerPeriod(15000);
    block_sys_object.SetErrorEventDelay(CAN_ERROR_DISABLED);
    block_sys_object.SetObjectType(CAN_OBJECT_TYPE_SYSTEM_BLOCK_FEATURES);
    block_sys_object.RegisterFunctionEvent(nullptr);
    block_sys_object.RegisterFunctionRequest(nullptr);
    block_sys_object.RegisterFunctionSet(nullptr);
    block_sys_object.RegisterFunctionTimer(nullptr);
};

/// @brief Common BlockError parameters will be applied to the specified CANObject.
///        All BlockError objects has:
///          - disabled timers and flood mode
///          - enabled events (300 ms period)
///          - no external handlers for events, timers, set & request commends
/// @param block_sys_object Target CANObject
void set_block_error_params(CANObjectInterface &block_sys_object)
{
    block_sys_object.SetTimerFloodMode(false);
    block_sys_object.SetTimerPeriod(CAN_TIMER_DISABLED);
    block_sys_object.SetErrorEventDelay(300);
    block_sys_object.SetObjectType(CAN_OBJECT_TYPE_SYSTEM_BLOCK_ERROR);
    block_sys_object.RegisterFunctionEvent(nullptr);
    block_sys_object.RegisterFunctionRequest(nullptr);
    block_sys_object.RegisterFunctionSet(nullptr);
    block_sys_object.RegisterFunctionTimer(nullptr);
};

/// @brief Debug logger function: decodes function ID to to human-readable string.
/// @param function_id ID of the function.
/// @return Null-terminated string with name of the function
const char *get_function_name(can_function_id_t function_id)
{
#if defined(DEBUG) || defined(DETAILED_DEBUG)
    switch (function_id)
    {
    case CAN_FUNC_NONE:
        return "none";

    case CAN_FUNC_SET_IN:
        return "set: in";

    case CAN_FUNC_TOGGLE_IN:
        return "toggle: in";

    case CAN_FUNC_ACTION_IN:
        return "action: in";
    
    case CAN_FUNC_SET_REAL_TIME_IN:
        return "set realtime: in";

    case CAN_FUNC_LOCK_IN:
        return "lock: in";

    case CAN_FUNC_LOCK_OUT_OK:
        return "lock: ok answer";

    case CAN_FUNC_LOCK_OUT_ERR:
        return "lock: error answer";

    case CAN_FUNC_REQUEST_IN:
        return "request: in";

    case CAN_FUNC_TIMER_NORMAL:
        return "timer: normal";

    case CAN_FUNC_TIMER_WARNING:
        return "timer: warning";

    case CAN_FUNC_TIMER_CRITICAL:
        return "timer: critical";

    case CAN_FUNC_EVENT_OK:
        return "event: ok";

    case CAN_FUNC_EVENT_ERROR:
        return "event: error";

    case CAN_FUNC_SYSTEM_REQUEST_IN:
        return "system request: in";

    case CAN_FUNC_SYSTEM_REQUEST_OUT_OK:
        return "system request: ok answer";

    case CAN_FUNC_FIRST_OUT_OK:
    case CAN_FUNC_FIRST_OUT_UNUSED:
    case CAN_FUNC_FIRST_OUT_ERR:
        return "correct other: some first value";

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
        return "correct other: some send raw function";

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
#if defined(DEBUG) || defined(DETAILED_DEBUG)
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
#if defined(DEBUG) || defined(DETAILED_DEBUG)
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

/// @brief Debug logger function: decodes object type code to human-readable string.
/// @param object_type Type of the object.
/// @return Null-terminated string with name of the object type.
const char *get_object_type_name(object_type_t object_type)
{
#if defined(DEBUG) || defined(DETAILED_DEBUG)
    switch (object_type)
    {
    case CAN_OBJECT_TYPE_ORDINARY:
        return "object type: ordinary object";

    case CAN_OBJECT_TYPE_SYSTEM_BLOCK_INFO:
        return "object type: system object - BlockInfo";

    case CAN_OBJECT_TYPE_SYSTEM_BLOCK_HEALTH:
        return "object type: system object - BlockHealth";

    case CAN_OBJECT_TYPE_SYSTEM_BLOCK_FEATURES:
        return "object type: system object - BlockFeatures";

    case CAN_OBJECT_TYPE_SYSTEM_BLOCK_ERROR:
        return "object type: system object - BlockError";
    
    case CAN_OBJECT_TYPE_SILENT:
        return "object type: silent listener";

    case CAN_OBJECT_TYPE_UNKNOWN:
    default:
        return "object type: unknown";
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
#if defined(DEBUG) || defined(DETAILED_DEBUG)
    switch (error_section)
    {
    case ERROR_SECTION_NONE:
        return "error: section [none], code [-]";

    case ERROR_SECTION_CAN_MANAGER:
        switch ((error_code_manager_t)error_code)
        {
        case ERROR_CODE_MANAGER_NONE:
            return "error: section [CANManager], code [none]";

        case ERROR_CODE_MANAGER_CAN_FRAME_AND_ERROR_STRUCT_ARE_BOTH_BLANK:
            return "error: section [CANManager], code [CAN frame and error structure are both blank after handlers]";

        case ERROR_CODE_MANAGER_SOMETHING_WRONG:
            return "error: section [CANManager], code [something went wrong]";

        default:
            return "error: section [CANManager], code [unknown]";
        }
        break;

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

        case ERROR_CODE_OBJECT_HAVE_NO_DATA:
            return "error: section [CANObject], code [have no data]";

        case ERROR_CODE_OBJECT_INCORRECT_DATA_LENGTH:
            return "error: section [CANObject], code [incorrect data length]";

        case ERROR_CODE_OBJECT_SYSTEM_REQUEST_SHOULD_NOT_HAVE_DATA:
            return "error: section [CANObject], code [system request should not have any frame data]";

        case ERROR_CODE_OBJECT_TOGGLE_FUNCTION_IS_MISSING:
            return "error: section [CANObject], code [external toggle handler is missing]";

        case ERROR_CODE_OBJECT_TOGGLE_COMMAND_FRAME_SHOULD_NOT_HAVE_DATA:
            return "error: section [CANObject], code [toggle command frame should not have any frame data]";

        case ERROR_CODE_OBJECT_ACTION_FUNCTION_IS_MISSING:
            return "error: section [CANObject], code [external action handler is missing]";

        case ERROR_CODE_OBJECT_ACTION_COMMAND_FRAME_SHOULD_NOT_HAVE_DATA:
            return "error: section [CANObject], code [action command frame should not have any frame data]";

        case ERROR_CODE_OBJECT_LOCK_COMMAND_FRAME_DATA_LENGTH_ERROR:
            return "error: section [CANObject], code [lock command frame data length error]";

        case ERROR_CODE_OBJECT_LOCK_LEVEL_IS_UNKNOWN:
            return "error: section [CANObject], code [unknown lock level]";

        case ERROR_CODE_OBJECT_LOCKED:
            return "error: section [CANObject], code [object locked for this function]";

        case ERROR_CODE_OBJECT_BAD_INCOMING_CAN_FRAME:
            return "error: section [CANObject], code [incoming CAN frame not initialized]";
        
        case ERROR_CODE_OBJECT_HARDWARE_ERROR_CODE_IS_MISSING:
            return "error: section [CANObject], code [hardware error code is missing]";

        case ERROR_CODE_OBJECT_SOMETHING_WRONG:
            return "error: section [CANObject], code [something went wrong]";

        default:
            return "error: section [CANObject], code [unknown]";
        }
        break;
    
    case ERROR_SECTION_HARDWARE:
        return "error: section [hardware], code [description is hardware related]";

    default:
        return "error: section [unknown], code [-]";
    }
#else  // DEBUG
    return "detailed names are disabled";
#endif // DEBUG
}
