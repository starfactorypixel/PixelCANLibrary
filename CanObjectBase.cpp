#include "CanObjectBase.h"

bool CANObjectBase::SendFrame(can_frame_t &can_frame) noexcept
{
    if (!HasParent())
        return false;

    if (can_frame.raw_data[0] == (uint8_t)CAN_FUNC_NONE)
        return false;

    can_frame.object_id = this->GetId();
    return _parent->PushFrameToTX(can_frame);
}

bool CANObjectBase::SendFrame(uint8_t *data, uint8_t data_length) noexcept
{
    if (data == nullptr)
        return false;
    if (data_length == 0 || data_length > RAW_CAN_FRAME_MAX_PAYLOAD)
        return false;
    if (data[0] == (uint8_t)CAN_FUNC_NONE)
        return false;

    if (!HasParent())
        return false;

    return _parent->PushFrameToTX(this->GetId(), data, data_length);
}

void CANObjectBase::Tick(uint32_t time) noexcept
{
    this->OnTick(time);

    if (!this->IsTimerEnabled())
        return;

    if (time - _last_timer < _timer_period_ms)
        return;

    this->OnTimer();
    _last_timer = time;
}

void CANObjectBase::ProcessFrame(can_frame_t &can_frame) noexcept
{
    if (can_frame.object_id != this->GetId() && can_frame.object_id != CAN_SYSTEM_ID_BROADCAST)
        return;

    this->OnProcessFrame(can_frame);
}