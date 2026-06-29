#pragma once

#include "CanCommon.h"
#include "CanManagerInterface.h"

class CANObjectInterface
{
public:
    virtual ~CANObjectInterface() = default;

    virtual can_object_id_t getId() = 0;

    virtual void setParent(CanManagerInterface &parent) = 0;
    virtual bool hasParent() noexcept = 0;
    virtual CanManagerInterface *getParent() noexcept = 0;

    virtual void setTimerPeriod(uint16_t period_ms) = 0;
    virtual uint16_t getTimerPeriod() noexcept = 0;
    virtual bool isTimerEnabled() noexcept = 0;

    virtual void tick(uint32_t time) = 0;
    virtual void processFrame(can_frame_t &can_frame) = 0;
};
