#pragma once

#include <string.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionReactiveInterfaces.h"

/// @brief TOGGLE function: reactive function
///        It processes incoming CAN frames with the TOGGLE function ID.
class CANFunctionToggle : public CANFunctionReactiveInterface
{
protected:
    /// @brief Preprocessor stage
    ///        It is used to check the incoming CAN frame and prepare it for processing.
    ///        If the preprocessor returns an error, the outgoing CAN frame will be filled with the error data.
    /// @param can_object CAN object which received a CAN frame
    /// @param incoming_can_frame Incoming CAN frame
    /// @return can_result_t Result of the preprocessor
    virtual can_result_t _CanFramePreprocessor(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, error_code_object_t &error_code) override
    {
        if (incoming_can_frame.raw_data_length != sizeof(incoming_can_frame.function_id))
        {
            error_code = error_code_object_t::ERROR_CODE_OBJECT_INCORRECT_DATA_LENGTH;
            return can_result_t::CAN_RESULT_ERROR;
        }
        return can_result_t::CAN_RESULT_IGNORE;
    }

    /// @brief Processing stage
    ///        It processes the incoming CAN frame and generates an outgoing CAN frame.
    ///        If the processing is successful, the outgoing CAN frame will be filled with the data
    ///        If the processing fails, the outgoing CAN frame will be filled with an error
    ///        If the processing is ignored, the entire processing will be passed to the next function in the chain.
    /// @param can_object CAN object which received a CAN frame
    /// @param incoming_can_frame Incoming CAN frame
    /// @param outgoing_can_frame Outgoing CAN frame
    /// @return can_result_t Result of the processing
    virtual can_result_t _CanFrameProcessor(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame) override
    {
        // TODO: do something here!!!!!
        // TODO: Call collback to the hardware to toggle the state

        outgoing_can_frame.initialized = true;
        outgoing_can_frame.object_id = can_object.GetId();
        outgoing_can_frame.function_id = can_function_id_t::CAN_FUNC_EVENT_OK;
        outgoing_can_frame.raw_data_length = sizeof(outgoing_can_frame.function_id) + can_object.GetOneDataFieldSize();
        memcpy(outgoing_can_frame.data, can_object.GetValuePtr(0), can_object.GetOneDataFieldSize());

        return CAN_RESULT_CAN_FRAME;
    }

    /// @brief Postprocessor stage
    ///        It is used to perform additional actions after the processing of the incoming CAN frame
    ///        Here we can modify the outgoing CAN frame or perform additional checks.
    /// @param can_object CAN object which received a CAN frame
    /// @param outgoing_can_frame Outgoing CAN frame
    /// @return can_result_t Result of the postprocessing
    virtual can_result_t _CanFramePostprocessor(CANObjectInterface &can_object, can_frame_t &outgoing_can_frame) override
    {
        // No postprocessing needed for TOGGLE function
        // Just return the outgoing CAN frame as is
        return can_result_t::CAN_RESULT_CAN_FRAME;
    }

public:
    CANFunctionToggle()
    {
        this->SetFunctionID(can_function_id_t::CAN_FUNC_TOGGLE_IN);
    }
};
