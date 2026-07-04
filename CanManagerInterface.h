#pragma once

#include "CanCommon.h"

class CANObjectInterface;

class CanManagerInterface
{
private:
public:
    virtual ~CanManagerInterface() = default;

    virtual bool addObject(CANObjectInterface &can_object) noexcept = 0;
    virtual bool hasCanObject(can_object_id_t id) const noexcept = 0;
    virtual CANObjectInterface *getCanObject(can_object_id_t id) const noexcept = 0;

    virtual void process() noexcept = 0;

    virtual bool tryToSendCANFrameFromTXQueue() noexcept = 0;

    virtual uint32_t getTime() const noexcept = 0;

    virtual bool pushFrameToTX(can_frame_t &can_frame) noexcept = 0;
    virtual bool pushFrameToTX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) noexcept = 0;
    virtual uint16_t getTXQueueLength() const noexcept = 0;

    virtual bool pushFrameToRX(can_frame_t &can_frame) noexcept = 0;
    virtual bool pushFrameToRX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) noexcept = 0;
    virtual uint16_t getRXQueueLength() const noexcept = 0;
};
