#pragma once

#include <string.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionReactiveInterfaces.h"

/// @brief SET function: reactive function
///        It processes incoming CAN frames with the SET function ID.
///        It sets the value of the data field at the specified index.
///        It is used to set the value of the data field in the CANObject.
class CANFunctionSet : public CANFunctionReactiveInterface
{
protected:
    /// @brief Frame processing stage. It processes the incoming CAN frame and generates an outgoing CAN frame.
    ///        If the processing is successful, the outgoing CAN frame will be filled with the data
    ///        If the processing fails, the outgoing CAN frame will be filled with an error
    ///        If the processing is ignored, the entire processing will be passed to the next function in the chain.
    /// @param can_object CAN object which received a CAN frame
    /// @param incoming_can_frame Incoming CAN frame
    /// @param outgoing_can_frame Outgoing CAN frame
    /// @return can_result_t Result of the processing
    virtual can_result_t _CanFrameProcessor(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame)
    {
        if (incoming_can_frame.raw_data_length != can_object.GetOneDataFieldSize() + sizeof(incoming_can_frame.function_id))
        {
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, error_code_object_t::ERROR_CODE_OBJECT_INCORRECT_DATA_LENGTH);
            return can_result_t::CAN_RESULT_ERROR;
        }

        can_object.SetValue(0, (void *)incoming_can_frame.data, incoming_can_frame.raw_data_length - sizeof(incoming_can_frame.function_id));
        outgoing_can_frame.initialized = true;
        outgoing_can_frame.object_id = can_object.GetId();
        outgoing_can_frame.function_id = can_function_id_t::CAN_FUNC_EVENT_OK;
        outgoing_can_frame.raw_data_length = sizeof(outgoing_can_frame.function_id) + can_object.GetOneDataFieldSize();
        memcpy(outgoing_can_frame.data, can_object.GetValuePtr(0), can_object.GetOneDataFieldSize());

        return CAN_RESULT_CAN_FRAME;
    }

public:
    CANFunctionSet() : CANFunctionReactiveInterface(can_function_id_t::CAN_FUNC_SET_IN) {};
};
