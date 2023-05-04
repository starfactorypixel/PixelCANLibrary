#ifndef CANMANAGER_H
#define CANMANAGER_H
// #pragma once

#include "CAN_common.h"
#include "CANFrame.h"
#include "CANObject.h"

/******************************************************************************************************************************
 *
 * CANManager class: implements CAN Manager interface
 *
 ******************************************************************************************************************************/
class CANManager
{
public:
    CANManager() = delete; // No default constructor because we should have the tick_function!
    CANManager(get_ms_tick_function_t tick_func);
    ~CANManager();

    void set_tick_func(get_ms_tick_function_t tick_func);
    uint32_t get_tick();

    bool process();
    void print(const char *prefix);

    bool take_new_rx_frame(CANFrame &can_frame);
    bool take_new_rx_frame(can_id_t id, uint8_t *data, uint8_t data_length);

    uint8_t get_rx_queue_size();
    uint8_t get_tx_queue_size();
    bool has_rx_frames_in_queue();
    bool has_tx_frames_for_transmission();
    bool add_tx_queue_item(CANFrame &can_frame);

    bool give_tx_frame(CANFrame &can_frame);
    bool give_tx_frame(can_id_t &id, uint8_t *data, uint8_t &data_length);

    uint8_t get_can_objects_count();
    bool has_can_objects();
    bool has_can_object(can_id_t id);
    CANObject *add_can_object(can_id_t id, const char *name = nullptr);
    CANObject *get_can_object_by_index(uint8_t index);
    CANObject *get_can_object_by_can_id(can_id_t id);
    bool delete_can_object(can_id_t id);

protected:
    CANFrame _rx_frame_queue[CAN_MANAGER_RX_TX_QUEUE_SIZE];
    CANFrame _tx_frame_queue[CAN_MANAGER_RX_TX_QUEUE_SIZE];

private:
    CANObject _can_objects_list[CAN_MANAGER_MAX_CAN_OBJECTS];

    get_ms_tick_function_t _tick_func = nullptr;
};

#endif // CANMANAGER_H