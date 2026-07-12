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
    virtual void OnTick(uint32_t time) noexcept {}

    // will be called if the object received CAN frame; you need to do all the stuff with CAN Frame here
    virtual void OnProcessFrame(can_frame_t &can_frame) noexcept {}

    // will be called when it is time to send timer's CAN frame
    // your responsobility not only to prepare the data but also to send it
    virtual void OnTimer() noexcept {}

    bool SendFrame(can_frame_t &can_frame) noexcept;

    bool SendFrame(uint8_t *data, uint8_t data_length) noexcept;

public:
    CANObjectBase() = delete;
    CANObjectBase(const CANObjectBase &) = delete;
    CANObjectBase &operator=(const CANObjectBase &) = delete;

    CANObjectBase(can_object_id_t id) : _id(id) {}

    virtual ~CANObjectBase() = default;

    virtual can_object_id_t GetId() const noexcept override final { return _id; }
    virtual void SetId(can_object_id_t id) noexcept override final { _id = id; }

    virtual void SetParent(CanManagerInterface &parent) noexcept override final { _parent = &parent; }
    virtual bool HasParent() const noexcept override final { return _parent != nullptr; }
    virtual CanManagerInterface *GetParent() const noexcept override final { return _parent; }

    virtual void SetTimerPeriod(uint16_t period_ms) noexcept override final { _timer_period_ms = period_ms; }
    virtual uint16_t GetTimerPeriod() const noexcept override final { return _timer_period_ms; }
    virtual bool IsTimerEnabled() const noexcept override final { return _timer_period_ms != TIMER_DISABLED; }

    virtual void Tick(uint32_t time) noexcept override final;

    virtual void ProcessFrame(can_frame_t &can_frame) noexcept override final;
};