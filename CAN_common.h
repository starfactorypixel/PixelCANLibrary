#ifndef CAN_COMMON_H
#define CAN_COMMON_H

#include <stdint.h>

/******************************************************************************************************************************
 *
 * Common CAN related types
 *
 ******************************************************************************************************************************/
using get_ms_tick_function_t = uint32_t (*)();

// Library classes forward declarations
// class CANObject;
// class CANFrame;
// class CANFunctionBase;

/******************************************************************************************************************************
 *
 * DataField related data types
 *
 ******************************************************************************************************************************/
/*
union data_mapper_t
{
    uint8_t u8arr[4];
    int8_t i8arr[4];
    uint8_t u8;
    int8_t i8;
    uint16_t u16arr[2];
    int16_t i16arr[2];
    uint16_t u16;
    int16_t i16;
    uint32_t u32;
    int32_t i32;
};

enum data_field_t : uint8_t
{
    DF_UNKNOWN = 0x00,
    DF_INT8 = 0x01,
    DF_UINT8 = 0x02,
    DF_INT16 = 0x03,
    DF_UINT16 = 0x04,
    DF_INT32 = 0x05,
    DF_UINT32 = 0x06,

    // TODO: this type of data fields is incompatible with static memory allocation
    // we should to implement this functionality somewhere else
    // DF_RAW_DATA_ARRAY = 0x07,
};

enum data_field_state_t : uint8_t
{
    DFS_NOT_INITIALIZED = 0x00,
    DFS_OK = 0x01,
    DFS_ERROR = 0xFF,
};

enum data_field_attention_state_t : uint8_t
{
    DF_ATTENTION_STATE_NONE = 0x00,
    DF_ATTENTION_STATE_NORMAL = 0x01,
    DF_ATTENTION_STATE_WARNING = 0x02,
    DF_ATTENTION_STATE_CRITICAL = 0x03,
};

// external handlers for raw data fields
// free space checker
using raw_data_free_space_handler_t = bool (*)(uint32_t size_needed);
// open new temp file for writing
using raw_data_open_tmp_file_handler_t = bool (*)();
// write chunk
using raw_data_write_chunk_handler_t = bool (*)(uint8_t chunk_size, uint8_t *chunk_data);
// close temp file and rename
using raw_data_close_and_rename_file_handler_t = bool (*)(uint8_t file_code);
// abort all
using raw_data_abort_operations_handler_t = bool (*)();
*/

/******************************************************************************************************************************
 *
 * CANFrame types
 *
 ******************************************************************************************************************************/
#define CAN_MAX_PAYLOAD 8

// base CAN frame format uses 11-bit IDs (uint16)
// extended CAN frame format uses 29-bit IDs (uint32)
typedef uint16_t can_id_t;

enum can_frame_error_codes_t : uint8_t
{
    CAN_FRAME_OK = 0x00,
    CAN_FRAME_IS_NULL = 0x01,
    CAN_FRAME_SIZE_ERROR = 0x02,
    CAN_FRAME_NOT_EXPECTED = 0x0E,
    CAN_FRAME_UNKNOWN_ERROR = 0xFF,
};

/******************************************************************************************************************************
 *
 * CANObject related data types
 *
 ******************************************************************************************************************************/
// The maximum number of DataFields in the CANObject
#define CAN_OBJECT_DATA_FIELDS_MAX_COUNT 7

enum can_object_state_t : uint8_t
{
    COS_NOT_INITIALIZED = 0x00,
    COS_OK = 0x01,
    COS_DATA_FIELD_ERROR = 0x02,
    COS_DATA_BUFFER_SIZE_ERROR = 0x03,
    COS_UNKNOWN_ERROR = 0xFF,
};

/******************************************************************************************************************************
 *
 * CANManager related types
 *
 ******************************************************************************************************************************/
// The maximum number of CANFrames in CAN_RX/CAN_TX frame buffer
#define CAN_MANAGER_RX_TX_QUEUE_SIZE 16

// The maximum number of CAN objects, that CANManager can manage
// Equals to double block size (2*16)
#define CAN_MANAGER_MAX_CAN_OBJECTS 32

/******************************************************************************************************************************
 *
 * CANFunction related types
 *
 ******************************************************************************************************************************/
// CAN Function result codes
enum CAN_function_result_t : uint8_t
{
    // nothing should be done, function will process this stuff next time
    CAN_RES_NONE = 0x00,

    // the next_ok_function will be called after handler
    CAN_RES_NEXT_OK = 0x01,

    // the next_err_function will be called after handler
    CAN_RES_NEXT_ERR = 0x02,

    // function processed all the things but won't to call next handler
    CAN_RES_FINAL = 0xFF,
};

// using CAN_function_handler_t = CAN_function_result_t (*)(CANObject &parent_object, CANFunctionBase &parent_function, CANFrame *can_frame);

// CAN Function IDs
enum CAN_function_id_t : uint8_t
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

    CAN_FUNC_SIMPLE_SENDER = 0xC0,

    CAN_FUNC_EVENT_ERROR = 0xE6,
};

// CAN Function state codes
enum CAN_function_state_t : uint8_t
{
    // function is OFF: it doesn't work, doesn't send any error response
    // for example: timer is stopped; event is suppressed
    CAN_FS_DISABLED = 0x00,

    // function is working in normal mode, doing some stuff and reacts to the inputs
    CAN_FS_ACTIVE = 0x01,

    // function is paused, it responds to the incoming calls with error
    // this state is useful for state machine implementation, when several functions
    // need to be called in the specified order
    CAN_FS_SUSPENDED = 0x02,

    // ATTENTION: only for automated tests purpose, may be deleted after release
    CAN_FS_IGNORED = 0xFF,
};

// CAN Function error codes
enum CAN_function_error_t : uint8_t
{
    CAN_FUNC_ERROR_NO_EXTERNAL_HANDLER = 0x01,
    CAN_FUNC_ERROR_UNKNOWN_SETTER_ERROR = 0x02,
    CAN_FUNC_ERROR_READONLY_OBJECT = 0x03, // only objects with exactly one data field are writable
    CAN_FUNC_ERROR_MISSING_NECESSARY_FUNCTION = 0x04,
    CAN_FUNC_ERROR_FUNCTION_UNAVAILABLE = 0x05,
    CAN_FUNC_ERROR_FILE_CODE = 0x06,
    CAN_FUNC_ERROR_DATA_SIZE = 0x07,
    CAN_FUNC_ERROR_INCORRECT_CHUNK_INDEX = 0x08,
    CAN_FUNC_ERROR_CHUNK_SIZE = 0x09,
    CAN_FUNC_ERROR_CHUNK_COUNT = 0x0A,
    CAN_FUNC_ERROR_HAVE_NOT_FREE_SPACE = 0x0B,
    CAN_FUNC_ERROR_WRITE_STARTING = 0x0C,
    CAN_FUNC_ERROR_START_NEW_CHUNK = 0x0D,
    CAN_FUNC_ERROR_CHUNK_SAVING = 0x0E,
    CAN_FUNC_ERROR_WRITE_FINISH = 0x0F,
};

/******************************************************************************************************************************
 *
 * Pixel related types
 *
 ******************************************************************************************************************************/
enum pixel_error_section_t : uint8_t
{
    PIX_ERR_NONE = 0x00,
    PIX_ERR_CAN_FRAME = 0x01,
    PIX_ERR_CAN_OBJECT = 0x02,
    PIX_ERR_DATA_FIELD = 0x03,
    PIX_ERR_FUNCTION = 0x04,
    PIX_ERR_CAN_MANAGER = 0x05,

    // not an error, just a flag that several frames was missed during the send raw process
    PIX_ERR_SEND_RAW_MISSED_FRAMES = 0xFF,
};

#endif // CAN_COMMON_H