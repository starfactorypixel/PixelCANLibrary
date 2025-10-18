#pragma once

#include "CanCommon.h"

class CANObjectInterface;

class CanManagerInterface
{
private:
public:
    CanManagerInterface() = default;
    ~CanManagerInterface() = default;

    virtual bool registerObject(CANObjectInterface &can_object) = 0;
    virtual bool hasCanObject(can_object_id_t id) = 0;
    virtual CANObjectInterface *getCanObject(can_object_id_t id) = 0;

    virtual bool hasFrameToSend() = 0;
    virtual bool getFrameToSend(can_frame_t &can_frame) = 0;

    virtual void tick(uint32_t time) = 0;
    virtual bool incomingCANFrame(can_object_id_t id, uint8_t *data, uint8_t length) = 0;

    virtual uint8_t getRxFrameBufferLength() = 0;
    virtual uint8_t getTxFrameBufferLength() = 0;

    /*
    // ----------------------------------------------------------------- 
    // Other required methods from API.md
    // ----------------------------------------------------------------- 
    virtual void PutFrame(can_frame_t &frame) = 0;
    virtual void PutFrame(can_object_id_t id, uint8_t *data, uint8_t length) = 0;

    virtual bool GetFrame(can_frame_t &frame) = 0;
    virtual bool GetFrame(can_object_id_t &id, uint8_t *&data, uint8_t &length) = 0;

    virtual void SendRaw(can_frame_t &frame) = 0;
    virtual void SendRaw(can_object_id_t id, uint8_t *data, uint8_t length) = 0;
    // ----------------------------------------------------------------- 
    */
};
