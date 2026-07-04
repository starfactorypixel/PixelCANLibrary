#pragma once

#include "CanCommon.h"
#include "CanManagerInterface.h"

class CANObjectInterface
{
public:
    virtual ~CANObjectInterface() = default;

    virtual can_object_id_t getId() const noexcept = 0;

    virtual void setParent(CanManagerInterface &parent) noexcept = 0;
    virtual bool hasParent() const noexcept = 0;
    virtual CanManagerInterface *getParent() const noexcept = 0;

    virtual void setTimerPeriod(uint16_t period_ms) noexcept = 0;
    virtual uint16_t getTimerPeriod() const noexcept = 0;
    virtual bool isTimerEnabled() const noexcept = 0;

    virtual void tick(uint32_t time) noexcept = 0;
    virtual void processFrame(can_frame_t &can_frame) noexcept = 0;
};
