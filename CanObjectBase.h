#pragma once

#include "CanCommon.h"
#include "CanObjectInterface.h"

class CANObjectBase : public CANObjectInterface
{
public:
    const uint16_t TIMER_DISABLED = UINT16_MAX;

private:
    can_object_id_t _id = CAN_SYSTEM_ID_BROADCAST;
    CanManagerInterface *_parent = nullptr;

    uint16_t _timer_period_ms = TIMER_DISABLED;
    uint32_t _last_timer = 0;

protected:
    // will be called from tick() method; use it if you need to do something independently from timers or incoming CAN frames
    virtual void loop(uint32_t time) noexcept {}

    // will be called if the object received CAN frame; you need to do all the stuff with CAN Frame here
    virtual void frameProcessor(can_frame_t &can_frame) noexcept {}

    // will be called when it is time to send timer's CAN frame
    // your responsobility not only to prepare the data but also to send it
    virtual void processTimer() noexcept {}

    bool sendFrame(can_frame_t &can_frame) noexcept
    {
        if (!hasParent())
            return false;
        
        if (can_frame.raw_data[0] == (uint8_t)CAN_FUNC_NONE)
            return false;

        can_frame.object_id = this->getId();
        return _parent->pushFrameToTX(can_frame);
    }

    bool sendFrame(uint8_t *data, uint8_t data_length) noexcept
    {
        if (data == nullptr)
            return false;
        if (data_length == 0 || data_length > RAW_CAN_FRAME_MAX_PAYLOAD)
            return false;
        if (data[0] == (uint8_t)CAN_FUNC_NONE)
            return false;

        if (!hasParent())
            return false;
        
        return _parent->pushFrameToTX(this->getId(), data, data_length);
    }

public:
    CANObjectBase() = delete;
    CANObjectBase(const CANObjectBase &) = delete;
    CANObjectBase &operator=(const CANObjectBase &) = delete;

    CANObjectBase(can_object_id_t id) : _id(id) {}

    virtual ~CANObjectBase() = default;

    virtual can_object_id_t getId() const noexcept override final { return _id; }

    virtual void setParent(CanManagerInterface &parent) noexcept override final { _parent = &parent; }
    virtual bool hasParent() const noexcept override final { return _parent != nullptr; }
    virtual CanManagerInterface *getParent() const noexcept override final { return _parent; }

    virtual void setTimerPeriod(uint16_t period_ms) noexcept override final { _timer_period_ms = period_ms; }
    virtual uint16_t getTimerPeriod() const noexcept override final { return _timer_period_ms; }
    virtual bool isTimerEnabled() const noexcept override final { return _timer_period_ms != TIMER_DISABLED; }

    virtual void tick(uint32_t time) noexcept override final
    {
        this->loop(time);

        if (!this->isTimerEnabled())
            return;

        if (time - _last_timer < _timer_period_ms)
            return;

        this->processTimer();
        _last_timer = time;
    }

    virtual void processFrame(can_frame_t &can_frame) noexcept override final
    {
        if (can_frame.object_id != this->getId() && can_frame.object_id != CAN_SYSTEM_ID_BROADCAST)
            return;

        this->frameProcessor(can_frame);
    }
};