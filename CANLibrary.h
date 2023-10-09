#ifndef CANLIBRARY_H
#define CANLIBRARY_H

#include "pix_utils.h"

#include "CAN_common.h"
#include "CANManager.h"
#include "CAN_common_block.h"

#warning In CANLibrary v0.1.12 names of methods was changed. CANObject.GetValue() is CANObject.GetValuePtr() now, CANObject.GetTypedValue() is CANObject.GetValue() now. Be careful, it may cause the runtime error.

#endif // CANLIBRARY_H