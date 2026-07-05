#pragma once

#include "CanCommon.h"
#include "CanManagerInterface.h"

class CANObjectInterface
{
public:
    virtual ~CANObjectInterface() = default;

    virtual can_object_id_t GetId() const noexcept = 0;

    virtual void SetParent(CanManagerInterface &parent) noexcept = 0;
    virtual bool HasParent() const noexcept = 0;
    virtual CanManagerInterface *GetParent() const noexcept = 0;

    virtual void SetTimerPeriod(uint16_t period_ms) noexcept = 0;
    virtual uint16_t GetTimerPeriod() const noexcept = 0;
    virtual bool IsTimerEnabled() const noexcept = 0;

    virtual void Tick(uint32_t time) noexcept = 0;
    virtual void ProcessFrame(can_frame_t &can_frame) noexcept = 0;
};
