#include "DataField.h"

/******************************************************************************************************************************
 *
 * DataField class: keep the data pointer, data type and data size of the data source
 *
 ******************************************************************************************************************************/
// 'unknown' for logging
const char *DataField::_value_unknown = "unknown";
// the data field source type names for logging
const char *DataField::_source_type_int8 = "int8";
const char *DataField::_source_type_uint8 = "uint8";
const char *DataField::_source_type_int16 = "int16";
const char *DataField::_source_type_uint16 = "uint16";
const char *DataField::_source_type_int32 = "int32";
const char *DataField::_source_type_uint32 = "uint32";

// the data field state names for logging
const char *DataField::_state_data_field_ok = "ok";
const char *DataField::_state_data_field_alarm = "alarm";
const char *DataField::_state_data_field_error = "error";

DataField::DataField()
{
    delete_data_source();
}

DataField::DataField(data_field_t source_type, void *data_source, uint32_t array_item_count) : DataField()
{
    set_data_source(source_type, data_source, array_item_count);
}

DataField::~DataField()
{
    delete_data_source();
}

void DataField::delete_data_source()
{
    _src_data_pointer = nullptr;
    _source_type = DF_UNKNOWN;
    _array_item_count = 0;
    _array_item_size = 0;

    _checker_min.u32 = 0;
    _checker_max.u32 = 0;

    set_state(DFS_ERROR);
}

bool DataField::set_data_source(data_field_t source_type, void *data, uint32_t array_item_count)
{
    if (source_type == DF_UNKNOWN || data == nullptr || array_item_count == 0)
    {
        set_state(DFS_ERROR);
        return false;
    }
    delete_data_source();

    switch (source_type)
    {
    case DF_INT8:
        _array_item_size = sizeof(int8_t);
        break;

    case DF_UINT8:
        _array_item_size = sizeof(uint8_t);
        break;

    case DF_INT16:
        _array_item_size = sizeof(int16_t);
        break;

    case DF_UINT16:
        _array_item_size = sizeof(uint16_t);
        break;

    case DF_INT32:
        _array_item_size = sizeof(int32_t);
        break;

    case DF_UINT32:
        _array_item_size = sizeof(uint32_t);
        break;

    case DF_UNKNOWN:
    default:
        set_state(DFS_ERROR);
        // all data was set to zero below
        return false;
    }

    _source_type = source_type;
    _array_item_count = array_item_count;
    _src_data_pointer = data;

    update_state();

    return !has_errors();
}

bool DataField::has_data_source()
{
    if (_array_item_size == 0 || _array_item_count == 0)
        return false;

    if (_src_data_pointer == nullptr)
        return false;

    if (_source_type == DF_UNKNOWN)
        return false;

    return true;
}

bool DataField::has_alarm_state()
{
    return get_state() == DFS_ALARM;
}

data_field_t DataField::get_source_type()
{
    return _source_type;
}

const char *DataField::get_source_type_name()
{
    switch (get_source_type())
    {
    case DF_INT8:
        return _source_type_int8;

    case DF_UINT8:
        return _source_type_uint8;

    case DF_INT16:
        return _source_type_int16;

    case DF_UINT16:
        return _source_type_uint16;

    case DF_INT32:
        return _source_type_int32;

    case DF_UINT32:
        return _source_type_uint32;

    case DF_UNKNOWN:
    default:
        return _value_unknown;
    }
}

uint32_t DataField::get_item_count()
{
    return _array_item_count;
}

uint8_t DataField::get_item_size()
{
    return _array_item_size;
}

void *DataField::get_src_pointer()
{
    return _src_data_pointer;
}

uint32_t DataField::get_data_byte_array_length()
{
    return get_item_count() * get_item_size();
}

bool DataField::copy_data_to(void *destination, uint8_t destination_max_size)
{
    if (!has_data_source())
        return false;

    if (destination == nullptr || destination_max_size < get_data_byte_array_length())
        return false;

    memcpy(destination, get_src_pointer(), get_data_byte_array_length());

    return true;
}

data_field_state_t DataField::get_state()
{
    return _state;
}

void DataField::set_state(data_field_state_t state)
{
    _state = state;
}

data_field_state_t DataField::update_state()
{
    set_state(DFS_OK);

    if (!has_data_source())
    {
        set_state(DFS_ERROR);
    }

    return get_state();
}

bool DataField::has_errors()
{
    return get_state() == DFS_ERROR;
}

const char *DataField::get_state_name()
{
    switch (get_state())
    {
    case DFS_OK:
        return _state_data_field_ok;

    case DFS_ALARM:
        return _state_data_field_alarm;

    case DFS_ERROR:
        return _state_data_field_error;

    default:
        return _value_unknown;
    }
}

void DataField::print(const char *prefix)
{
    LOG("%sDataField: state = %s, item size = %d (%s), item count = %d", prefix, get_state_name(),
        get_item_size(), get_source_type_name(), get_item_count());
}