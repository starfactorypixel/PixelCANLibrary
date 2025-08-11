#pragma once

#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionInterface.h"

/// @brief These functions respond to incoming CAN-frames
///        They are used to process incoming CAN frames and perform actions based on the received data.
class CANFunctionReactiveInterface : public CANFunctionInterface
{
private:
    /// @brief Pointer to the next reactive function in the chain of responsibility
    ///        It is used to pass the CAN frame to the next function in the chain if the current function cannot handle it.
    ///        If this pointer is nullptr, it means that there are no more functions in the chain,
    ///        and the current function should handle the CAN frame
    /// @note This pointer is set by the SetNextFunction() method.
    CANFunctionReactiveInterface *_next_reactive_function = nullptr;

    /// @brief Calls the next reactive function in the chain of responsibility
    ///        If there is no next function, it fills the outgoing CAN frame with an error
    /// @param can_object CAN object which received a CAN frame
    /// @param incoming_can_frame Incoming CAN frame
    /// @param outgoing_can_frame Outgoing CAN frame
    /// @return can_result_t Result of the processing
    can_result_t _CallNextFunction(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame)
    {
        if (this->_next_reactive_function == nullptr)
        {
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, ERROR_CODE_OBJECT_UNSUPPORTED_FUNCTION);
            outgoing_can_frame.object_id = can_object.GetId();
            return can_result_t::CAN_RESULT_CAN_FRAME;
        }

        return this->_next_reactive_function->InputCanFrame(can_object, incoming_can_frame, outgoing_can_frame);
    }

protected:
    /// @brief Preprocessor stage. It is used to check the incoming CAN frame and prepare it for processing.
    ///        If the preprocessor returns can_result_t::CAN_RESULT_IGNORE,
    ///        it means that the incoming CAN frame was processed but nothing should be done according to that.
    ///        If the preprocessor returns can_result_t::CAN_RESULT_ERROR,
    ///        it means that the incoming CAN frame cannot be processed by this function,
    ///        and the outgoing CAN frame will be filled with the error data.
    ///        If the preprocessor returns can_result_t::CAN_RESULT_CAN_FRAME,
    ///        it means that the incoming CAN frame can be processed by this function,
    ///        and the outgoing CAN frame will be filled with the data.
    /// @param can_object 
    /// @param incoming_can_frame 
    /// @param error_code 
    /// @return 
    virtual can_result_t _CanFramePreprocessor(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, error_code_object_t &error_code) = 0;
    virtual can_result_t _CanFrameProcessor(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame) = 0;
    virtual can_result_t _CanFramePostprocessor(CANObjectInterface &can_object, can_frame_t &outgoing_can_frame) = 0;

public:
    CANFunctionReactiveInterface() : _next_reactive_function(nullptr) {};
    virtual ~CANFunctionReactiveInterface() = default;

    /// @brief Sets next reactive function in the chain of responsibility
    /// @param can_reactive_function Pointer to the reactive function which should be added to the end of chain
    /// @return Pointer to the reactive function so man can link functions in a convenient way
    CANFunctionReactiveInterface *SetNextFunction(CANFunctionReactiveInterface *can_reactive_function)
    {
        if (this == can_reactive_function)
            return this;

        if (this->_next_reactive_function != nullptr)
            return this->_next_reactive_function->SetNextFunction(can_reactive_function);

        this->_next_reactive_function = can_reactive_function;
        return can_reactive_function;
    };

    /// @brief Processes the CAN frame
    /// @param parent_can_object CAN object which received a CAN frame
    /// @param incoming_can_frame Incoming CAN frame
    /// @param outgoing_can_frame Outgoing CAN frame
    /// @return TODO: Do we actually need can_result now?
    can_result_t InputCanFrame(CANObjectInterface &parent_can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame)
    {
        if (incoming_can_frame.function_id != this->GetFunctionID())
            return this->_CallNextFunction(parent_can_object, incoming_can_frame, outgoing_can_frame);

        clear_can_frame_struct(outgoing_can_frame);

        // Preprocessor stage
        // It is used to check if the incoming CAN frame can be processed by this function
        error_code_object_t error_code = error_code_object_t::ERROR_CODE_OBJECT_NONE;
        switch (this->_CanFramePreprocessor(parent_can_object, incoming_can_frame, error_code))
        {
        case can_result_t::CAN_RESULT_IGNORE:
            break;

        case can_result_t::CAN_RESULT_ERROR:
        case can_result_t::CAN_RESULT_CAN_FRAME:
        default:
            if (error_code == error_code_object_t::ERROR_CODE_OBJECT_NONE)
                error_code = error_code_object_t::ERROR_CODE_OBJECT_FUNCTION_PREPROCESSOR_ERROR;
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, error_code);
            outgoing_can_frame.object_id = parent_can_object.GetId();
            return can_result_t::CAN_RESULT_CAN_FRAME;
        }

        // Processing stage
        // It is used to process the incoming CAN frame and generate an outgoing CAN frame
        // If the processing is successful, the outgoing CAN frame will be filled with the data
        // If the processing fails, the outgoing CAN frame will be filled with an error
        // If the processing is ignored, the entire processing will be passed to the next function in the chain.
        switch (this->_CanFrameProcessor(parent_can_object, incoming_can_frame, outgoing_can_frame))
        {
        case can_result_t::CAN_RESULT_IGNORE:
            return this->_CallNextFunction(parent_can_object, incoming_can_frame, outgoing_can_frame);

        case can_result_t::CAN_RESULT_CAN_FRAME:
            outgoing_can_frame.object_id = parent_can_object.GetId();
            break;

        case can_result_t::CAN_RESULT_ERROR:
        default:
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, ERROR_CODE_OBJECT_INCORRECT_FUNCTION_WORKFLOW);
            outgoing_can_frame.object_id = parent_can_object.GetId();
            return can_result_t::CAN_RESULT_CAN_FRAME;
        }

        // Postprocessor stage
        // It is used to perform additional actions after the processing of the incoming CAN frame
        // Here we can modify the outgoing CAN frame or perform additional checks
        switch (this->_CanFramePostprocessor(parent_can_object, outgoing_can_frame))
        {
        case can_result_t::CAN_RESULT_IGNORE:
            return this->_CallNextFunction(parent_can_object, incoming_can_frame, outgoing_can_frame);

        case can_result_t::CAN_RESULT_CAN_FRAME:
            break;

        case can_result_t::CAN_RESULT_ERROR:
        default:
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, ERROR_CODE_OBJECT_FUNCTION_POSTPROCESSOR_ERROR);
            outgoing_can_frame.object_id = parent_can_object.GetId();
            return CAN_RESULT_CAN_FRAME;
        }

        return can_result_t::CAN_RESULT_CAN_FRAME;
    };
};
