#ifndef CAN_BLOCK_COMMON_H
#define CAN_BLOCK_COMMON_H

#include <stdint.h>
#include "CANLibrary.h"

class CANManager;

#ifdef __cplusplus
extern "C"
{
#endif

// BlockInfo
// request | timer:15000
// byte	1 + 7	{ type[0] data[1..7] }
// Основная информация о блоке. См. "Системные параметры".
struct __attribute__ ((__packed__)) block_info_t
{
    // byte 1
    union
    {
        struct
        {
            uint8_t board_type : 5;
            uint8_t board_version : 3;
        };
        uint8_t board_data_byte;
    };

    // byte 2
    union
    {
        struct
        {
            uint8_t firmware_version : 6;
            uint8_t protocol_version : 2;
        };
        uint8_t software_data_byte;
    };

    // byte 3
    // uint8_t unused1;

    // byte 4
    // uint8_t unused2;

    // byte 5
    // uint8_t unused3;

    // byte 6
    // uint8_t unused4;

    // byte 7
    // uint8_t unused5;
};

// BlockHealth
// request | event
// byte	1 + 7	{ type[0] data[1..7] }
// Информация о здоровье блока. См. "Системные параметры".
struct __attribute__ ((__packed__)) block_health_t
{
    // byte 1 & 2
    uint16_t voltage;

    // byte 3 & 4
    int16_t current;

    // byte 5
    int8_t temperature;

    // byte 6
    // uint8_t unused1;

    // byte 7
    // uint8_t unused2;
};

// BlockCfg
// request
// byte	1 + 1 + X	{ type[0] param[1] data[2..7] }
// Чтение и запись настроек блока. См. "Системные параметры".
struct __attribute__ ((__packed__)) block_cfg_t
{
    // byte 1
    uint8_t unused1;

    // byte 2
    uint8_t unused2;

    // byte 3
    uint8_t unused3;

    // byte 4
    uint8_t unused4;

    // byte 5
    uint8_t unused5;

    // byte 6
    uint8_t unused6;

    // byte 7
    uint8_t unused7;
};

// BlockError
// request | event
// byte	1 + X	{ type[0] data[1..7] }
// Ошибки блока. См. "Системные параметры".
struct __attribute__ ((__packed__)) block_error_t
{
    // byte 1
    uint8_t code;

    // byte 2
    // uint8_t unused1;

    // byte 3
    // uint8_t unused2;

    // byte 4
    // uint8_t unused3;

    // byte 5
    // uint8_t unused4;

    // byte 6
    // uint8_t unused5;

    // byte 7
    // uint8_t unused6;
};

// **********************************************************************************************************
// Universal block functions
bool init_block_info(CANManager &cm, uint16_t can_id, block_info_t &block_info, uint32_t timer_period = 15000);
bool init_block_health(CANManager &cm, uint16_t can_id, block_health_t &block_health, uint32_t event_period = 3000);
bool init_block_cfg(CANManager &cm, uint16_t can_id, block_cfg_t &block_cfg);
bool init_block_error(CANManager &cm, uint16_t can_id, block_error_t &block_error);

#ifdef __cplusplus
}
#endif

#endif // CAN_BLOCK_COMMON_H