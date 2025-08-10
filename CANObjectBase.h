#pragma once

#include <stdint.h>
#include "CAN_common.h"
#include "CANObjectInterface.h"
#include "CANFunctionProactiveInterfaces.h"
#include "CANFunctionReactiveInterfaces.h"

/// @brief Base class for all CAN objects.
///        It provides basic functionality for registering reactive and proactive functions,
///        processing incoming CAN frames, and managing the ID of the CAN object.
///        It is used as a base class for specific CAN objects.
/// @note This class should not be used directly, use derived classes instead.
class CANObjectBase : public CANObjectInterface
{
private:
    can_object_id_t _id = CAN_SYSTEM_ID_BROADCAST;

    /// @brief Pointer to the first reactive function in the chain of responsibility
    CANFunctionReactiveInterface *_reactive_functions = nullptr;

    /// @brief Pointer to the first proactive function in the chain of responsibility
    CANFunctionProactiveInterface *_proactive_functions = nullptr;

public:
    /// @brief Default constructor is forbidden.
    CANObjectBase() = delete;

    CANObjectBase(can_object_id_t object_id) : _id(object_id) {};

    virtual ~CANObjectBase() = default;

    /// @brief Registers reactive function in the CANObject
    ///        The function will be added to the end of the chain of reactive functions.
    /// @param reactive_function - Pointer to the reactive function to register
    /// @return Pointer to the added reactive function
    virtual CANFunctionReactiveInterface *RegisterReactiveFunction(CANFunctionReactiveInterface *reactive_function) override
    {
        if (reactive_function == nullptr)
            return nullptr;

        if (_reactive_functions != nullptr)
            return _reactive_functions->SetNextFunction(reactive_function);

        _reactive_functions = reactive_function;
        return _reactive_functions;
    };

    /// @brief Registers proactive function in the CANObject
    ///        The function will be added to the end of the chain of proactive functions.
    /// @param proactive_function - Pointer to the proactive function to register
    /// @return Pointer to the added proactive function
    virtual CANFunctionProactiveInterface *RegisterProactiveFunction(CANFunctionProactiveInterface *proactive_function) override
    {
        if (proactive_function == nullptr)
            return nullptr;

        if (_proactive_functions != nullptr)
            return _proactive_functions->SetNextFunction(proactive_function);

        _proactive_functions = proactive_function;
        return _proactive_functions;
    };

    /// @brief Processes all proactive functions of the CANObject
    ///        This method should be called periodically to allow proactive functions to work.
    ///        The method will call the first proactive function in the chain
    /// @param time - current time in milliseconds
    /// @param outgoing_can_frame - Reference to the outgoing CAN frame
    ///        The outgoing CAN frame will be filled with data from the proactive function.
    /// @return can_result_t - Result of the processing
    virtual can_result_t Process(uint32_t time, can_frame_t &outgoing_can_frame) override
    {
        if (_proactive_functions == nullptr)
            return can_result_t::CAN_RESULT_IGNORE;

        return _proactive_functions->Process(*this, time, outgoing_can_frame);
    };

    /// @brief Processes incoming CAN frame with the registered reactive functions.
    /// @param incoming_can_frame - Reference to the incoming CAN frame
    /// @param outgoing_can_frame - Reference to the outgoing CAN frame
    /// @return can_result_t - Result of the processing
    virtual can_result_t InputCanFrame(const can_frame_t &incoming_can_frame, can_frame_t &outgoing_can_frame) override
    {
        // TODO: надо разобраться с очередью отправки
        if (incoming_can_frame.object_id != CAN_SYSTEM_ID_BROADCAST &&
            incoming_can_frame.object_id != this->GetId())
            return can_result_t::CAN_RESULT_IGNORE;

        if (_reactive_functions == nullptr)
            return can_result_t::CAN_RESULT_IGNORE;

        return _reactive_functions->InputCanFrame(*this, incoming_can_frame, outgoing_can_frame);
    };

    /// @brief Returns the ID of the CANObject
    /// @return can_object_id_t - ID of the CANObject
    virtual can_object_id_t GetId() override
    {
        return _id;
    };
};
