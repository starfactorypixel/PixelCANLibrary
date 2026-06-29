#pragma once

#include "CanCommon.h"

class CANObjectInterface;

class CanManagerInterface
{
private:
public:
    virtual ~CanManagerInterface() = default;

    virtual bool addObject(CANObjectInterface &can_object) = 0;
    virtual bool hasCanObject(can_object_id_t id) = 0;
    virtual CANObjectInterface *getCanObject(can_object_id_t id) = 0;

    virtual void tick(uint32_t time) = 0;

    virtual bool tryToSendCANFrameFromTXQueue() = 0;

    /*
    bool hasFrameToSend() { ... } // есть ли в TX буфере что отправлять
    bool getFrameToSend(can_frame_t &can_frame) { ... } // забираем фрейм на отправку (фрейм удаляется из буфера в момент забора, а не когда уже отправлен)
    */

    // bool hasSpaceInTX() { ... } // есть ли в очереди на отправку место для фрейма
    virtual bool pushFrameToTX(can_frame_t &can_frame) = 0;
    virtual bool pushFrameToTX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) = 0;

    // bool hasSpaceInRX() { ... } // есть ли в очереди на обработку место для фрейма
    virtual bool pushFrameToRX(can_frame_t &can_frame) = 0;
    virtual bool pushFrameToRX(can_object_id_t id, uint8_t *data = nullptr, uint8_t length = 0) = 0;
};
