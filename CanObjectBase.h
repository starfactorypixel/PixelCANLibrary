#pragma once

#include "CanCommon.h"
#include "CanObjectInterface.h"

class CANObjectBase : public CANObjectInterface
{
private:

protected:

public:
    CANObjectBase() = delete;
    CANObjectBase(can_object_id_t id) : CANObjectInterface::CANObjectInterface(id) {};
    virtual ~CANObjectBase() = default;

};