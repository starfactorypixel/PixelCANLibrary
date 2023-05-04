#include "CANManager.h"

/******************************************************************************************************************************
 *
 * CANManager class: implements CAN Manager interface
 *
 ******************************************************************************************************************************/
CANManager::CANManager(get_ms_tick_function_t tick_func)
{
    set_tick_func(tick_func);
}

CANManager::~CANManager()
{
}

void CANManager::set_tick_func(get_ms_tick_function_t tick_func)
{
    _tick_func = tick_func;
}

uint32_t CANManager::get_tick()
{
    if (_tick_func != nullptr)
        return _tick_func();

    return 0;
}

bool CANManager::process()
{
    bool result = true;

    bool co_update_result = true;
    for (CANObject &i : _can_objects_list)
    {
        if (i.is_initialized())
        {
            co_update_result = i.update();
            result = result && co_update_result;
        }
    }

    CANObject *co = nullptr;
    for (CANFrame &i : _rx_frame_queue)
    {
        if (i.is_initialized())
        {
            co = get_can_object_by_can_id(i.get_id());
            if (co != nullptr)
            {
                co->process_incoming_frame(i);
            }
            i.clear_frame();
        }
    }

    return result;
}

void CANManager::print(const char *prefix)
{
    char str[100];
    uint16_t item_counter = 0;
    LOG("%s-----------------------------------------------", prefix);
    LOG("%sCAN Manager data", prefix);

    // print CANObject data
    LOG("%s  CAN Objects:", prefix);
    item_counter = 0;
    // item_counter overflow check
    static_assert(CAN_MANAGER_MAX_CAN_OBJECTS <= UINT16_MAX);

    for (CANObject &i : _can_objects_list)
    {
        if (i.is_initialized())
        {
            sprintf(str, "%s    #%d: ", prefix, ++item_counter);
            i.print(str);
        }
    }

    // item_counter overflow check for RX and TX queues
    static_assert(CAN_MANAGER_RX_TX_QUEUE_SIZE <= UINT16_MAX);

    // print RX queue items
    LOG("%s  RX queue:", prefix);
    item_counter = 0;
    for (CANFrame &i : _rx_frame_queue)
    {
        if (i.is_initialized())
        {
            sprintf(str, "%s    #%d: ", prefix, ++item_counter);
            i.print(str);
        }
    }
    if (item_counter == 0)
    {
        LOG("%s    RX queue is empty.", prefix);
    }

    // print TX queue items
    LOG("%s  TX queue:", prefix);
    item_counter = 0;
    for (CANFrame &i : _tx_frame_queue)
    {
        if (i.is_initialized())
        {
            sprintf(str, "%s    #%d: ", prefix, ++item_counter);
            i.print(str);
        }
    }
    if (item_counter == 0)
    {
        LOG("%s    TX queue is empty.", prefix);
    }

    LOG("%s-----------------------------------------------", prefix);
}

bool CANManager::take_new_rx_frame(CANFrame &can_frame)
{
    if (!has_can_object(can_frame.get_id()))
        return false;

    for (CANFrame &i : _rx_frame_queue)
    {
        if (!i.is_initialized())
        {
            i.set_frame(can_frame);
            return true;
        }
    }
    LOG("WARNING: no free space in RX CAN frame queue (%d max)", CAN_MANAGER_RX_TX_QUEUE_SIZE);
    can_frame.print("Ignored ");

    return false;
}

bool CANManager::take_new_rx_frame(can_id_t id, uint8_t *data, uint8_t data_length)
{
    CANFrame new_frame(id, data, data_length);
    return take_new_rx_frame(new_frame);
}

uint8_t CANManager::get_rx_queue_size()
{
    // TODO: we can implement free_space_counter for RX and TX queues and avoid for loops
    uint8_t queue_size = 0;
    // queue_size overflow check
    static_assert(CAN_MANAGER_RX_TX_QUEUE_SIZE <= UINT8_MAX);

    for (CANFrame &i : _rx_frame_queue)
    {
        if (i.is_initialized())
        {
            ++queue_size;
        }
    }
    return queue_size;
}

uint8_t CANManager::get_tx_queue_size()
{
    // TODO: we can implement free_space_counter for RX and TX queues and avoid for loops
    uint8_t queue_size = 0;
    // queue_size overflow check
    static_assert(CAN_MANAGER_RX_TX_QUEUE_SIZE <= UINT8_MAX);

    for (CANFrame &i : _tx_frame_queue)
    {
        if (i.is_initialized())
        {
            ++queue_size;
        }
    }
    return queue_size;
}

bool CANManager::has_rx_frames_in_queue()
{
    return get_rx_queue_size() > 0;
}

bool CANManager::add_tx_queue_item(CANFrame &can_frame)
{
    for (CANFrame &i : _tx_frame_queue)
    {
        if (!i.is_initialized())
        {
            i.set_frame(can_frame);
            return true;
        }
    }

    LOG("WARNING: no free space in TX CAN frame queue (%d max)", CAN_MANAGER_RX_TX_QUEUE_SIZE);
    can_frame.print("Ignored ");

    return false;
}

bool CANManager::has_tx_frames_for_transmission()
{
    return get_tx_queue_size() > 0;
}

bool CANManager::give_tx_frame(CANFrame &can_frame)
{
    if (!has_tx_frames_for_transmission())
        return false;

    for (CANFrame &i : _tx_frame_queue)
    {
        if (i.is_initialized())
        {
            can_frame.set_frame(i);
            i.clear_frame();
            return true;
        }
    }
    return false;
}

bool CANManager::give_tx_frame(can_id_t &id, uint8_t *data, uint8_t &data_length)
{
    if (!has_tx_frames_for_transmission())
        return false;

    CANFrame can_frame;
    give_tx_frame(can_frame);

    id = can_frame.get_id();
    data_length = can_frame.get_data_length();
    memcpy(data, can_frame.get_data_pointer(), data_length);

    return true;
}

uint8_t CANManager::get_can_objects_count()
{
    // TODO: we can implement object_counter and avoid for loops
    uint8_t can_objects_count = 0;
    // can_objects_count overflow check
    static_assert(CAN_MANAGER_MAX_CAN_OBJECTS <= UINT8_MAX);

    for (CANObject &i : _can_objects_list)
    {
        if (i.is_initialized())
        {
            ++can_objects_count;
        }
    }
    return can_objects_count;
}

CANObject *CANManager::add_can_object(can_id_t id, const char *name)
{
    CANObject *new_can_object = get_can_object_by_can_id(id);

    if (new_can_object != nullptr)
        return new_can_object;

    for (CANObject &i : _can_objects_list)
    {
        if (!i.is_initialized())
        {
            i.set_id(id);
            i.set_parent(*this);
            i.set_name(name);
            i.update_state();
            return &i;
        }
    }

    LOG("WARNING: not enough space. No more than %d CANObjects are allowed.", CAN_MANAGER_MAX_CAN_OBJECTS);
    return nullptr;
}

CANObject *CANManager::get_can_object_by_index(uint8_t index)
{
    // index consistency check
    static_assert(CAN_MANAGER_MAX_CAN_OBJECTS <= UINT8_MAX);

    if (index >= CAN_MANAGER_MAX_CAN_OBJECTS || !_can_objects_list[index].is_initialized())
        return nullptr;

    return &_can_objects_list[index];
}

CANObject *CANManager::get_can_object_by_can_id(can_id_t id)
{
    if (!has_can_objects())
        return nullptr;

    for (CANObject &i : _can_objects_list)
    {
        if (id == i.get_id())
            return &i;
    }

    return nullptr;
}

bool CANManager::delete_can_object(can_id_t id)
{
    if (!has_can_objects())
        return false;

    for (CANObject &i : _can_objects_list)
    {
        if (id == i.get_id())
        {
            i.delete_object();
            return true;
        }
    }
    return false;
}

bool CANManager::has_can_objects()
{
    return (get_can_objects_count() != 0);
}

bool CANManager::has_can_object(can_id_t id)
{
    return (nullptr != get_can_object_by_can_id(id));
}
