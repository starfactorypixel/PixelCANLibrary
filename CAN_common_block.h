#ifndef CAN_COMMON_BLOCK_H
#define CAN_COMMON_BLOCK_H

#include <stdint.h>

// BlockInfo
// request | timer:15000
// byte	1 + 7	{ type[0] data[1..7] }
// Основная информация о блоке. См. "Системные параметры".
struct __attribute__((__packed__)) block_info_t
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

    // byte 3..7
    uint8_t unused[5];
};

// BlockHealth
// request | event
// byte	1 + 7	{ type[0] data[1..7] }
// Информация о здоровье блока. См. "Системные параметры".
struct __attribute__((__packed__)) block_health_t
{
    uint16_t voltage;
    int16_t current;
    int8_t temperature;
    uint8_t unused[2];
};

// BlockCfg
// request
// byte	1 + 1 + X	{ type[0] param[1] data[2..7] }
// Чтение и запись настроек блока. См. "Системные параметры".
struct __attribute__((__packed__)) block_cfg_t
{
    uint8_t unused[7];
};

// BlockError
// request | event
// byte	1 + X	{ type[0] data[1..7] }
// Ошибки блока. См. "Системные параметры".
struct __attribute__((__packed__)) block_error_t
{
    uint8_t code;
    uint8_t unused[6];
};

#endif // CAN_COMMON_BLOCK_H