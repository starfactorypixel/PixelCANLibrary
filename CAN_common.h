#ifndef CAN_COMMON_H
#define CAN_COMMON_H

#include <stdint.h>

#define CAN_FRAME_MAX_PAYLOAD 7 // excluding the function ID
#define CAN_TIMER_DISABLED UINT16_MAX
#define CAN_ERROR_DISABLED UINT16_MAX

// base CAN frame format uses 11-bit IDs (uint16)
// extended CAN frame format uses 29-bit IDs (uint32)
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

    CAN_FUNC_SYSTEM_REQUEST_IN = 0x3A,
    CAN_FUNC_SYSTEM_REQUEST_OUT_OK = 0x7A,
    // CAN_FUNC_SYSTEM_REQUEST_OUT_ERR = not allowed, 0xFA

    // CAN_FUNC_FIRST_IN = 0x00, // == CAN_FUNC_NONE
    CAN_FUNC_FIRST_OUT_OK = 0x40,
    CAN_FUNC_FIRST_OUT_UNUSED = 0x80,
    CAN_FUNC_FIRST_OUT_ERR = 0xC0,
};

using can_send_function_t = void (*)(can_object_id_t id, uint8_t *data, uint8_t length);

// CANFrame data structure
// It can be changed to class later (in case we need it)
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
// can_function_id_t must have a size of 1 byte
// otherwise we need to update can_frame_t structure
static_assert(sizeof(can_function_id_t) == 1);

/// @brief Clears all attributes of CAN frame
/// @param can_frame CAN frame to clear
void clear_can_frame_struct(can_frame_t &can_frame);

/// @brief Copies data from one CAN frame to another
/// @param dest_can_frame Destination CAN frame
/// @param src_can_frame Source CAN frame
void copy_can_frame_struct(can_frame_t &dest_can_frame, can_frame_t src_can_frame);

enum timer_type_t : uint8_t
{
    CAN_TIMER_TYPE_NONE = 0b00000000,
    CAN_TIMER_TYPE_NORMAL = 0b00000001,
    CAN_TIMER_TYPE_WARNING = 0b00000010,
    CAN_TIMER_TYPE_CRITICAL = 0b00000011,

    CAN_TIMER_TYPE_MASK = 0b00001111,
};

enum event_type_t : uint8_t
{
    CAN_EVENT_TYPE_NONE = 0b00000000,
    CAN_EVENT_TYPE_NORMAL = 0b00010000,
    CAN_EVENT_TYPE_ERROR = 0b00100000,

    CAN_EVENT_TYPE_MASK = 0b11110000,
};

enum object_type_t : uint8_t
{
    CAN_OBJECT_TYPE_UNKNOWN = 0x00,
    CAN_OBJECT_TYPE_ORDINARY = 0x01,
    CAN_OBJECT_TYPE_SYSTEM_BLOCK_INFO = 0x02,
    CAN_OBJECT_TYPE_SYSTEM_BLOCK_HEALTH = 0x03,
    CAN_OBJECT_TYPE_SYSTEM_BLOCK_FEATURES = 0x04,
    CAN_OBJECT_TYPE_SYSTEM_BLOCK_ERROR = 0x05,
    CAN_OBJECT_TYPE_SILENT = 0x06,
};

enum lock_func_level_t : uint8_t
{
    CAN_LOCK_LEVEL_UNLOCKED = 0x00,
    CAN_LOCK_LEVEL_PARTIAL_LOCK = 0x0F,
    CAN_LOCK_LEVEL_TOTAL_LOCK = 0xFF
};

enum error_section_t : uint8_t
{
    ERROR_SECTION_NONE = 0x00,
    ERROR_SECTION_CAN_MANAGER = 0x01,
    ERROR_SECTION_CAN_OBJECT = 0x02,
    ERROR_SECTION_HARDWARE = 0x03,
};

using error_code_hardware_t = uint8_t;

enum error_code_object_t : uint8_t
{
    ERROR_CODE_OBJECT_NONE = 0x00,
    ERROR_CODE_OBJECT_UNSUPPORTED_EVENT_TYPE = 0x01,
    ERROR_CODE_OBJECT_UNSUPPORTED_TIMER_TYPE = 0x02,
    ERROR_CODE_OBJECT_SET_FUNCTION_IS_MISSING = 0x03,
    ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION = 0x04,
    ERROR_CODE_OBJECT_INCORRECT_REQUEST = 0x05,
    ERROR_CODE_OBJECT_INCORRECT_FUNCTION_WORKFLOW = 0x06,
    ERROR_CODE_OBJECT_HAVE_NO_DATA = 0x07,
    ERROR_CODE_OBJECT_INCORRECT_DATA_LENGTH = 0x08,
    ERROR_CODE_OBJECT_SYSTEM_REQUEST_SHOULD_NOT_HAVE_DATA = 0x09,
    ERROR_CODE_OBJECT_TOGGLE_FUNCTION_IS_MISSING = 0x0A,
    ERROR_CODE_OBJECT_TOGGLE_COMMAND_FRAME_SHOULD_NOT_HAVE_DATA = 0x0B,
    ERROR_CODE_OBJECT_ACTION_FUNCTION_IS_MISSING = 0x0C,
    ERROR_CODE_OBJECT_ACTION_COMMAND_FRAME_SHOULD_NOT_HAVE_DATA = 0x0D,
    ERROR_CODE_OBJECT_LOCK_COMMAND_FRAME_DATA_LENGTH_ERROR = 0x0E,
    ERROR_CODE_OBJECT_LOCK_LEVEL_IS_UNKNOWN = 0x0F,
    ERROR_CODE_OBJECT_LOCKED = 0x10,
    ERROR_CODE_OBJECT_BAD_INCOMING_CAN_FRAME = 0x11,
    ERROR_CODE_OBJECT_HARDWARE_ERROR_CODE_IS_MISSING = 0x12,

    // NOTE: used for debug and as a temporary value; should not be used in release code
    ERROR_CODE_OBJECT_SOMETHING_WRONG = 0xFF,
};

enum error_code_manager_t : uint8_t
{
    ERROR_CODE_MANAGER_NONE = 0x00,
    ERROR_CODE_MANAGER_CAN_FRAME_AND_ERROR_STRUCT_ARE_BOTH_BLANK = 0x01,

    // NOTE: used for debug and as a temporary value; should not be used in release code
    ERROR_CODE_MANAGER_SOMETHING_WRONG = 0xFF,
};

struct can_error_t
{
    can_function_id_t function_id = CAN_FUNC_NONE;
    error_section_t error_section = ERROR_SECTION_NONE;
    uint8_t error_code = 0;
};

/// @brief Clears all attributes of CAN error structure
/// @param error CAN error to clear
void clear_can_error_struct(can_error_t &error);

enum can_result_t : uint8_t
{
    CAN_RESULT_IGNORE = 0x00,
    CAN_RESULT_CAN_FRAME = 0x01,
    CAN_RESULT_ERROR = 0x02,
};

using event_handler_t = can_result_t (*)(can_frame_t &can_frame, event_type_t event_type, can_error_t &error);
using timer_handler_t = can_result_t (*)(can_frame_t &can_frame, timer_type_t timer_type, can_error_t &error);
using lock_handler_t = can_result_t (*)(can_frame_t &can_frame, can_error_t &error);
using request_handler_t = can_result_t (*)(can_frame_t &can_frame, can_error_t &error);
using set_handler_t = can_result_t (*)(can_frame_t &can_frame, can_error_t &error);
using set_realtime_handler_t = can_result_t (*)(can_frame_t &can_frame, can_error_t &error);
using set_realtime_error_handler_t = void (*)(uint32_t time_has_passed_ms);
using toggle_handler_t = can_result_t (*)(can_frame_t &can_frame, can_error_t &error);
using action_handler_t = can_result_t (*)(can_frame_t &can_frame, can_error_t &error);

/*************************************************************************************************
 *
 * Common helper functions
 *
 *************************************************************************************************/
class CANObjectInterface;
void set_block_info_params(CANObjectInterface &block_sys_object);
void set_block_health_params(CANObjectInterface &block_sys_object);
void set_block_features_params(CANObjectInterface &block_sys_object);
void set_block_error_params(CANObjectInterface &block_sys_object);

/*************************************************************************************************
 *
 * Logger related functions.
 * All of them are disabled without DEBUG definition.
 *
 *************************************************************************************************/
const char *get_function_name(can_function_id_t function_id);
const char *get_timer_type_name(timer_type_t timer_type);
const char *get_event_type_name(event_type_t event_type);
const char *get_object_type_name(object_type_t object_type);
const char *get_error_code_name_for_section(error_section_t error_section, uint8_t error_code);

#endif // CAN_COMMON_H