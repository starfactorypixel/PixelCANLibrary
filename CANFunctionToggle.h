#pragma once

#include <string.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionReactiveInterfaces.h"

/*****************************************************************************************\

 TOGGLE function: reactive function

\*****************************************************************************************/
class CANFunctionToggle : public CANFunctionReactiveInterface
{
public:
    virtual can_result_t InputCanFrame(CANObjectInterface &can_object, const can_frame_t incoming_can_frame, can_frame_t &outgoing_can_frame) override
    {
        if (incoming_can_frame.function_id != CAN_FUNC_TOGGLE_IN)
            return CANFunctionReactiveInterface::InputCanFrame(can_object, incoming_can_frame, outgoing_can_frame);

        if (incoming_can_frame.raw_data_length != sizeof(incoming_can_frame.function_id))
        {
            fill_can_frame_with_error_data(outgoing_can_frame, ERROR_SECTION_CAN_OBJECT, ERROR_CODE_OBJECT_INCORRECT_DATA_LENGTH);
            outgoing_can_frame.object_id = can_object.GetId();
            return CAN_RESULT_CAN_FRAME;
        }

        // TODO: do something here!!!!!

        outgoing_can_frame.initialized = true;
        outgoing_can_frame.object_id = can_object.GetId();
        outgoing_can_frame.function_id = can_function_id_t::CAN_FUNC_EVENT_OK;
        outgoing_can_frame.raw_data_length = sizeof(outgoing_can_frame.function_id) + can_object.GetOneDataFieldSize();
        memcpy(outgoing_can_frame.data, can_object.GetValuePtr(0), can_object.GetOneDataFieldSize());

        return CAN_RESULT_CAN_FRAME;
    };
};
