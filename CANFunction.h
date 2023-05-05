#pragma once

#include <stdint.h>
#include "CAN_common.h"

/******************************************************************************************
 *
 ******************************************************************************************/
class CANFunctionInterface
{
public:
    virtual ~CANFunctionInterface() = default;

    /// @brief Registers external handler for the СФТАгтсешщт
    /// @param function_handler Pointer to the external handler
    virtual void RegisterFunctionHandler(CAN_function_handler_t function_handler) = 0;

    /// @brief Performs function processing
    /// @param time Current time (for antiflood, timers, events etc)
    /// @param can_frame Pointer to the CANFrame structure. It may be nullptr if function don't need incoming frame
    /// @return Returns function result
    virtual CAN_function_result_t Process(uint32_t time, can_frame_t *can_frame = nullptr) = 0;
    
};
