#ifndef CAN_BLOCK_COMMON_H
#define CAN_BLOCK_COMMON_H

#include <stdint.h>
#include "CANLibrary.h"
#include "CAN_common.h"

class CANManager;
class CANObject;

#ifdef __cplusplus
extern "C"
{
#endif

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
    struct __attribute__((__packed__)) block_health_t
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
    struct __attribute__((__packed__)) block_cfg_t
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
    struct __attribute__((__packed__)) block_error_t
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
    // Adds normal, warning and critical timers with the same period to the CANObject
    bool add_three_timers(CANObject &co, uint32_t period_ms);

    // initializes universal light CANObject with:
    //    - set function (external handler expected),
    //    - request function
    //    - event function (default event period is 3000 ms, it can be changed)
    //    - single uint8_t data field
    // Returns a pointer to the created CANObject
    CANObject *init_common_light_can_object(CANManager &cm, can_id_t can_object_id, const char *object_name, uint8_t &data_variable,
                                            CAN_function_handler_t set_external_handler, uint32_t event_period = 3000);

    // Universal block functions
    // initializes BlockInfo common CANObject
    bool init_block_info(CANManager &cm, uint16_t can_id, block_info_t &block_info, uint32_t timer_period = 15000);
    // initializes BlockHealth common CANObject
    bool init_block_health(CANManager &cm, uint16_t can_id, block_health_t &block_health, uint32_t event_period = 3000);
    // initializes BlockCfg common CANObject
    bool init_block_cfg(CANManager &cm, uint16_t can_id, block_cfg_t &block_cfg);
    // initializes BlockError common CANObject
    bool init_block_error(CANManager &cm, uint16_t can_id, block_error_t &block_error);

#ifdef __cplusplus
}
#endif

#endif // CAN_BLOCK_COMMON_H