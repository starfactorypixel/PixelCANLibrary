#include "CANObject.h"

/******************************************************************************************************************************
 *
 * CANObject class: CAN data object with ID and several DataFields
 *
 ******************************************************************************************************************************/
const char *CANObject::_value_unknown = "unknown";
const char *CANObject::_state_object_ok = "ok";
const char *CANObject::_state_data_field_error = "data field error";
const char *CANObject::_state_data_buffer_size_error = "data buffer size error";
const char *CANObject::_state_unknown_error = "unknown error";

CANObject::CANObject()
    : _id(0), _parent(nullptr), _state(COS_UNKNOWN_ERROR), _name(nullptr)
{
    _data_fields_list.clear();
    _functions_list.clear();
}

CANObject::CANObject(uint16_t id, CANManager &parent, const char *name) : CANObject()
{
    set_id(id, name);
    set_parent(parent);
    _set_state(COS_OK);
}

CANObject::~CANObject()
{
    for (DataField *i : _data_fields_list)
    {
        if (i != nullptr)
            delete i;
    }
    _data_fields_list.clear();

    for (CANFunctionBase *i : _functions_list)
    {
        if (i != nullptr)
            delete i;
    }
    _functions_list.clear();

    delete_name();
}

bool CANObject::operator==(const CANObject &can_object)
{
    return (get_id() == can_object._id);
}

can_id_t CANObject::get_id()
{
    return _id;
}

void CANObject::set_id(can_id_t id, const char *name)
{
    _id = id;
    set_name(name);
}

CANManager *CANObject::get_parent()
{
    return _parent;
}

void CANObject::set_parent(CANManager &parent)
{
    _parent = &parent;
}

uint32_t CANObject::get_tick()
{
    if (_parent == nullptr)
        return 0;

    return get_parent()->get_tick();
}

uint8_t CANObject::get_data_fields_count()
{
    return _data_fields_list.size();
}

bool CANObject::has_data_fields()
{
    return (get_data_fields_count() != 0);
}

DataField *CANObject::_add_data_field(DataField *data_field)
{
    _data_fields_list.push_back(data_field);

    update_state();

    return data_field;
}

DataField *CANObject::add_data_field(data_field_t type, void *data, uint32_t array_item_count)
{
    DataField *new_data_field = nullptr;

    switch (type)
    {
    case DF_INT8:
        new_data_field = new DataFieldInt8(data, array_item_count);
        break;

    case DF_UINT8:
        new_data_field = new DataFieldUint8(data, array_item_count);
        break;

    case DF_INT16:
        new_data_field = new DataFieldInt16(data, array_item_count);
        break;

    case DF_UINT16:
        new_data_field = new DataFieldUint16(data, array_item_count);
        break;

    case DF_INT32:
        new_data_field = new DataFieldInt32(data, array_item_count);
        break;

    case DF_UINT32:
        new_data_field = new DataFieldUint32(data, array_item_count);
        break;

    case DF_RAW_DATA_ARRAY:
        new_data_field = new DataFieldRawData(data, array_item_count);
        break;

    case DF_UNKNOWN:
    default:
        break;
    }

    if (new_data_field != nullptr)
    {
        new_data_field = _add_data_field(new_data_field);
        new_data_field->update_state();
    }
    update_state();
    return new_data_field;
}

bool CANObject::delete_data_field(uint8_t index)
{
    if (index >= get_data_fields_count())
        return false;

    std::list<DataField *>::iterator it = _data_fields_list.begin();
    std::advance(it, index);
    _data_fields_list.erase(it);

    update_state();

    return true;
}

DataField *CANObject::get_data_field(uint8_t index)
{
    if (index >= get_data_fields_count())
        return nullptr;

    std::list<DataField *>::iterator it = _data_fields_list.begin();
    std::advance(it, index);

    return *it;
}

/*
bool CANObject::has_data_fields_alarm()
{
    // TODO: alarm refactoring needed!
    for (DataField &i : _data_fields_list)
    {
        i.update_state();
        if (i.has_alarm_state())
            return true;
    }
    return false;
}
*/

uint8_t CANObject::calculate_all_data_size()
{
    if (!has_data_fields())
        return 0;

    uint8_t result = 0;
    for (DataField *i : _data_fields_list)
    {
        result += i->get_data_byte_array_length();
    }

    // 1 additional byte for FUNC_TYPE
    return result + 1;
}

can_object_state_t CANObject::get_state()
{
    return _state;
}

const char *CANObject::get_state_name()
{
    switch (get_state())
    {
    case COS_OK:
        return _state_object_ok;

    case COS_DATA_FIELD_ERROR:
        return _state_data_field_error;

    case COS_DATA_BUFFER_SIZE_ERROR:
        return _state_data_buffer_size_error;

    case COS_UNKNOWN_ERROR:
        return _state_unknown_error;

    default:
        return _value_unknown;
    }
}

bool CANObject::is_state_ok()
{
    return get_state() == COS_OK;
}

void CANObject::_set_state(can_object_state_t state)
{
    _state = state;
}

const char *CANObject::get_name()
{
    return _name;
}

void CANObject::set_name(const char *name)
{
    if (name == nullptr)
        return;

    _name = new char[strlen(name) + 1];
    strcpy(_name, name);
}

bool CANObject::has_name()
{
    return get_name() != nullptr;
}

void CANObject::delete_name()
{
    if (has_name())
        delete[] _name;
}

// only updates states of data fields and transfer CANObject in error state if there is at least one erroneous DataField
can_object_state_t CANObject::update_state()
{
    if (has_data_fields())
    {
        _max_attention_state = DF_ATTENTION_STATE_NORMAL;
        for (DataField *i : _data_fields_list)
        {
            i->update_state();
            if (i->get_attention_state() > _max_attention_state)
            {
                _max_attention_state = i->get_attention_state();
            }
        }

        _set_state(COS_OK);
        DataField *erroneous_data_field = get_first_erroneous_data_field();
        if (erroneous_data_field != nullptr)
            _set_state(COS_DATA_FIELD_ERROR);
    }
    else
    {
        _set_state(COS_DATA_FIELD_ERROR);
    }

    return get_state();
}

// updates local data storage, performs active and automatic/blended functions calls
bool CANObject::update()
{
    update_state();
    if (!is_state_ok())
        return false;

    // work with functions
    for (CANFunctionBase *i : _functions_list)
    {
        // call only active and automatic/blended functions
        if (i->is_active() && i->is_automatic_function())
            i->process();
    }

    return true;
}

void CANObject::print(const char *prefix)
{
    LOG("%sCAN object: id = 0x%04X (%s), state = %s, num of fields = %d", prefix, get_id(),
        has_name() ? get_name() : _value_unknown, get_state_name(), get_data_fields_count());

    uint8_t count = 0;
    char str_buff[70] = {0};

    LOG("%s    Data fields: %s", prefix, _data_fields_list.empty() ? "no data fields" : "");
    for (DataField *i : _data_fields_list)
    {
        sprintf(str_buff, "%s        #%d ", prefix, ++count);
        i->print(str_buff);
    }

    LOG("%s    Functions: %s", prefix, _functions_list.empty() ? "no functions" : "");
    count = 0;
    for (CANFunctionBase *i : _functions_list)
    {
        sprintf(str_buff, "%s        #%d ", prefix, ++count);
        i->print(str_buff);
    }
}

DataField *CANObject::get_first_erroneous_data_field()
{
    if (!has_data_fields())
        return nullptr;

    for (DataField *i : _data_fields_list)
    {
        i->update_state();
        if (i->has_errors())
            return i;
    }

    return nullptr;
}

data_field_attention_state_t CANObject::get_max_attention_state()
{
    return _max_attention_state;
}

// if function already exists then existing one will be returned
// for automatic and indirect functions there are no such limitations
CANFunctionBase *CANObject::add_function(CAN_function_id_t func_id)
{
    if (has_function(func_id))
    {
        // return existing one only for responding or blended functions
        return get_function(func_id);
    }

    CANFunctionBase *cf = nullptr;
    switch (func_id)
    {
    case CAN_FUNC_SET_IN:
        cf = new CANFunctionSet(this);
        // Next OK handler
        cf->set_next_ok_function(add_function(CAN_FUNC_SET_OUT_OK));
        // Next Error handler
        cf->set_next_err_function(add_function(CAN_FUNC_SET_OUT_ERR));
        break;

    case CAN_FUNC_SET_OUT_OK:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SET_OUT_OK);
        break;

    case CAN_FUNC_SET_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SET_OUT_ERR);
        break;

    case CAN_FUNC_REQUEST_IN:
        cf = new CANFunctionRequest(this);
        // Next OK handler
        cf->set_next_ok_function(add_function(CAN_FUNC_REQUEST_OUT_OK));
        // Next Error handler
        cf->set_next_err_function(add_function(CAN_FUNC_REQUEST_OUT_ERR));
        break;

    case CAN_FUNC_REQUEST_OUT_OK:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_REQUEST_OUT_OK);
        break;

    case CAN_FUNC_REQUEST_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_REQUEST_OUT_ERR);
        break;

    case CAN_FUNC_SEND_RAW_INIT_IN:
        cf = new CANFunctionSendInit(this);
        // next OK and ERROR handlers are created by constructor
        break;

    case CAN_FUNC_SEND_RAW_INIT_OUT_OK:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_INIT_OUT_OK);
        break;

    case CAN_FUNC_SEND_RAW_INIT_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_INIT_OUT_ERR);
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_START_IN:
        cf = new CANFunctionChunkStart(this);
        // next OK and ERROR handlers are created by constructor
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK);
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR);
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_DATA_IN:
        cf = new CANFunctionChunkData(this);
        // next ERROR handler is created by constructor
        // next OK handler is forbidden
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR);
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_END_IN:
        cf = new CANFunctionChunkEnd(this);
        // next OK and ERROR handlers are created by constructor
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK);
        break;

    case CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR);
        break;

    case CAN_FUNC_SEND_RAW_FINISH_IN:
        cf = new CANFunctionSendFinish(this);
        // next OK and ERROR handlers are created by constructor
        break;

    case CAN_FUNC_SEND_RAW_FINISH_OUT_OK:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_FINISH_OUT_OK);
        break;

    case CAN_FUNC_SEND_RAW_FINISH_OUT_ERR:
        cf = new CANFunctionSimpleSender(this);
        cf->set_id(CAN_FUNC_SEND_RAW_FINISH_OUT_ERR);
        break;

    case CAN_FUNC_TIMER_NORMAL:
        cf = new CANFunctionTimerNormal(this, UINT32_MAX);
        break;

    case CAN_FUNC_TIMER_WARNING:
        cf = new CANFunctionTimerWarning(this, UINT32_MAX);
        break;

    case CAN_FUNC_TIMER_CRITICAL:
        cf = new CANFunctionTimerCritical(this, UINT32_MAX);
        break;

    case CAN_FUNC_EVENT_ERROR:
        cf = new CANFunctionSimpleEvent(this, UINT32_MAX);
        break;

    case CAN_FUNC_NONE:
    default:
        return nullptr;
    }

    if (cf == nullptr)
        return nullptr;

    cf->set_parent(this);
    _functions_list.push_back(cf);

    return _functions_list.back();
}

CANFunctionBase *CANObject::get_function(CAN_function_id_t func_id)
{
    for (CANFunctionBase *i : _functions_list)
    {
        if (i->get_id() == func_id)
            return i;
    }

    return nullptr;
}

CANFunctionBase *CANObject::get_function(CANFunctionBase &function)
{
    for (CANFunctionBase *i : _functions_list)
    {
        if (*i == function)
            return i;
    }

    return nullptr;
}

bool CANObject::has_function(CAN_function_id_t func_id)
{
    return get_function(func_id) != nullptr;
}

bool CANObject::has_function(CANFunctionBase &function)
{
    return get_function(function) != nullptr;
}

uint8_t CANObject::get_functions_count()
{
    return _functions_list.size();
}

// only fills specified CANFrame with data from local storage
// no error state checks, data updates or other additional stuff
bool CANObject::fill_can_frame_with_data(CANFrame &can_frame, CAN_function_id_t func_id)
{
    can_frame.clear_frame();

    if (can_frame.get_max_data_length() < calculate_all_data_size())
        return false;

    // use second byte as the start point for DataFields
    // because we should reserve the first byte for CAN frame FUNC_TYPE
    uint8_t destination_offset = 1;
    int16_t remaining_data = 0;
    uint8_t *data_pointer = can_frame.get_data_pointer() + destination_offset;
    for (DataField *i : _data_fields_list)
    {
        remaining_data = can_frame.get_max_data_length() - destination_offset;
        if (remaining_data <= 0)
        {
            _set_state(COS_DATA_BUFFER_SIZE_ERROR);
            return false;
        }

        i->update_state();
        if (i->has_errors())
            return false;

        i->copy_data_to(data_pointer, remaining_data);

        data_pointer += i->get_data_byte_array_length();
        destination_offset += i->get_data_byte_array_length();
    }
    can_frame.set_data_length(calculate_all_data_size());
    can_frame.set_function_id(func_id);
    can_frame.set_id(get_id());
    can_frame.set_initialized_flag(true);

    return true;
}

bool CANObject::process_incoming_frame(CANFrame &can_frame)
{
    if (can_frame.get_id() != get_id())
        return false;

    for (CANFunctionBase *i : _functions_list)
    {
        if (i->get_id() == can_frame.get_function_id() && i->is_responding_function())
            return i->process(&can_frame);
    }
    return true;
}