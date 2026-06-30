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
    virtual void loop(uint32_t time) {}
    virtual void frameProcessor(can_frame_t &can_frame) {}
    virtual void craftTimerFrame(can_frame_t &can_frame) {}

    bool sendFrame(can_frame_t &can_frame)
    {
        if (hasParent())
        {
            can_frame.object_id = this->getId();
            return _parent->pushFrameToTX(can_frame);
        }
        return false;
    }

    bool sendFrame(uint8_t *data, uint8_t data_length)
    {
        if (data == nullptr)
            return false;
        if (data_length == 0 || data_length > RAW_CAN_FRAME_MAX_PAYLOAD)
            return false;

        if (hasParent())
        {
            return _parent->pushFrameToTX(this->getId(), data, data_length);
        }
        return false;
    }

public:
    CANObjectBase() = delete;
    CANObjectBase(const CANObjectBase &) = delete;
    CANObjectBase &operator=(const CANObjectBase &) = delete;
    CANObjectBase(can_object_id_t id) : _id(id) {}
    virtual ~CANObjectBase() = default;

    virtual can_object_id_t getId() override final { return _id; }

    virtual void setParent(CanManagerInterface &parent) override final { _parent = &parent; }
    virtual bool hasParent() noexcept override final { return _parent != nullptr; }
    virtual CanManagerInterface *getParent() noexcept override final { return _parent; }

    virtual void setTimerPeriod(uint16_t period_ms) override final { _timer_period_ms = period_ms; }
    virtual uint16_t getTimerPeriod() noexcept override final { return _timer_period_ms; }
    virtual bool isTimerEnabled() noexcept override final { return _timer_period_ms != TIMER_DISABLED; }

    virtual void tick(uint32_t time) override final
    {
        if (this->isTimerEnabled())
        {
            if (time - _last_timer >= _timer_period_ms)
            {
                can_frame_t timer_frame = {0};
                this->craftTimerFrame(timer_frame);
                if (timer_frame.raw_data[0] != CAN_FUNC_NONE &&
                    timer_frame.raw_data_length > 0 &&
                    timer_frame.raw_data_length <= RAW_CAN_FRAME_MAX_PAYLOAD)
                {
                    timer_frame.object_id = this->getId();
                    this->sendFrame(timer_frame);
                }

                _last_timer = time;
            }
        }

        this->loop(time);
    }

    virtual void processFrame(can_frame_t &can_frame) override final
    {
        if (can_frame.object_id != this->getId() && can_frame.object_id != CAN_SYSTEM_ID_BROADCAST)
            return;

        this->frameProcessor(can_frame);
    }
};