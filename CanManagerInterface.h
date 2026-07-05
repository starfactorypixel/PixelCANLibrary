#pragma once

#include "CanCommon.h"

class CANObjectInterface;

class CanManagerInterface
{
private:
public:
    virtual ~CanManagerInterface() = default;

    virtual bool AddObject(CANObjectInterface &can_object) noexcept = 0;
    virtual bool HasCanObject(can_object_id_t id) const noexcept = 0;
    virtual CANObjectInterface *GetCanObject(can_object_id_t id) const noexcept = 0;

    virtual void Processing() noexcept = 0;

    virtual bool TryToSendCANFrameFromTXQueue() noexcept = 0;

    virtual uint32_t GetTime() const noexcept = 0;

    virtual bool PushFrameToTX(can_frame_t &can_frame) noexcept = 0;
    virtual bool PushFrameToTX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) noexcept = 0;
    virtual uint16_t GetTXQueueLength() const noexcept = 0;

    virtual bool PushFrameToRX(can_frame_t &can_frame) noexcept = 0;
    virtual bool PushFrameToRX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) noexcept = 0;
    virtual uint16_t GetRXQueueLength() const noexcept = 0;
};
