#include "CAN_block_common.h"

// Adds normal, warning and critical timers with the same period to the CANObject
bool add_three_timers(CANObject &co, uint32_t period_ms)
{
    CANFunctionTimerBase *func_timer = nullptr;
    func_timer = (CANFunctionTimerBase *)co.add_function(CAN_FUNC_TIMER_NORMAL);
    func_timer->set_period(period_ms);
    func_timer = (CANFunctionTimerBase *)co.add_function(CAN_FUNC_TIMER_WARNING);
    func_timer->set_period(period_ms);
    func_timer = (CANFunctionTimerBase *)co.add_function(CAN_FUNC_TIMER_CRITICAL);
    func_timer->set_period(period_ms);

    return true;
}

// initializes universal light CANObject with:
//    - set function (external handler expected),
//    - request function
//    - event function (default event period is 3000 ms, it can be changed)
//    - single uint8_t data field
// Returns a pointer to the created CANObject
CANObject *init_common_light_can_object(CANManager &cm, can_id_t can_object_id, const char *object_name, uint8_t &data_variable,
                                        CAN_function_handler_t set_external_handler, uint32_t event_period)
{
    CANObject *co = cm.add_can_object(can_object_id, object_name);
    
    co->add_data_field(DF_UINT8, &data_variable);
    
    co->add_function(CAN_FUNC_REQUEST_IN);
    
    CANFunctionSet *func_set = (CANFunctionSet *)co->add_function(CAN_FUNC_SET_IN);
    func_set->set_external_handler(set_external_handler);
    
    CANFunctionSimpleEvent *func_event = (CANFunctionSimpleEvent *)co->add_function(CAN_FUNC_EVENT_ERROR);
    func_event->set_period(event_period);

    return co;
}

// initializes BlockInfo common CANObject
bool init_block_info(CANManager &cm, uint16_t can_id, block_info_t &block_info, uint32_t timer_period)
{
    // *******************************************************************
    // BlockInfo
    // request | timer:15000
    // byte	1 + 7	{ type[0] data[1..7] }
    // Основная информация о блоке. См. "Системные параметры".
    // *******************************************************************

    CANObject *co = nullptr;

    co = cm.add_can_object(can_id, "BlockInfo");
    co->add_data_field(DF_UINT8, &block_info.board_data_byte);
    co->add_data_field(DF_UINT8, &block_info.software_data_byte);
    co->add_function(CAN_FUNC_REQUEST_IN);
    add_three_timers(*co, timer_period);

    return true;
}

// initializes BlockHealth common CANObject
bool init_block_health(CANManager &cm, uint16_t can_id, block_health_t &block_health, uint32_t event_period)
{
    // *******************************************************************
    // BlockHealth
    // request | event
    // byte	1 + 7	{ type[0] data[1..7] }
    // Информация о здоровье блока. См. "Системные параметры".
    // *******************************************************************

    CANObject *co = nullptr;
    CANFunctionSimpleEvent *func_event = nullptr;

    co = cm.add_can_object(can_id, "BlockHealth");
    co->add_data_field(DF_UINT16, &block_health.voltage);
    co->add_data_field(DF_INT16, &block_health.current);
    co->add_data_field(DF_INT8, &block_health.temperature);
    co->add_function(CAN_FUNC_REQUEST_IN);
    func_event = (CANFunctionSimpleEvent *)co->add_function(CAN_FUNC_EVENT_ERROR);
    func_event->set_period(event_period);

    return true;
}

// initializes BlockCfg common CANObject
bool init_block_cfg(CANManager &cm, uint16_t can_id, block_cfg_t &block_cfg)
{
    // *******************************************************************
    // BlockCfg
    // request
    // byte	1 + 1 + X	{ type[0] param[1] data[2..7] }
    // Чтение и запись настроек блока. См. "Системные параметры".
    // *******************************************************************

    CANObject *co = nullptr;

    co = cm.add_can_object(can_id, "BlockCfg");
    co->add_function(CAN_FUNC_REQUEST_IN);

    return true;
}

// initializes BlockError common CANObject
bool init_block_error(CANManager &cm, uint16_t can_id, block_error_t &block_error)
{
    // *******************************************************************
    // BlockError
    // request | event
    // byte	1 + X	{ type[0] data[1..7] }
    // Ошибки блока. См. "Системные параметры".
    // *******************************************************************

    CANObject *co = nullptr;

    co = cm.add_can_object(can_id, "BlockError");
    co->add_data_field(DF_UINT8, &block_error.code);
    co->add_function(CAN_FUNC_REQUEST_IN);

    return true;
}
