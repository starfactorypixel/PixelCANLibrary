#include "DataField.h"

/******************************************************************************************************************************
 *
 * DataField class: keep the data pointer, data type and data size of the data source
 *
 ******************************************************************************************************************************/
// 'unknown' for logging
const char *DataField::_value_unknown = "unknown";
const char *DataField::_source_type_name = "unknown";

// the data field state names for logging
const char *DataField::_state_data_field_ok = "ok";
const char *DataField::_state_data_field_alarm = "alarm";
const char *DataField::_state_data_field_error = "error";

DataField::DataField(void *data, uint32_t array_item_count)
{
    delete_data_source();
    set_data_source(data, array_item_count);
}

DataField::~DataField()
{
    delete_data_source();
}

bool DataField::operator==(const DataField &other)
{
    return this->_equals(other);
}

bool DataField::operator!=(const DataField &other)
{
    return !(*this == other);
}

bool DataField::_equals(DataField const &other) const
{
    if (typeid(*this) != typeid(other))
        return false;

    return (this->_source_type == other._source_type && this->_src_data_pointer == other._src_data_pointer && this->_array_item_count == other._array_item_count);
}

void DataField::delete_data_source()
{
    _src_data_pointer = nullptr;
    _source_type = DF_UNKNOWN;
    _array_item_count = 0;
    _array_item_size = 0;

    set_state(DFS_ERROR);
}

bool DataField::set_data_source(void *data, uint32_t array_item_count)
{
    if (data == nullptr || array_item_count == 0)
    {
        set_state(DFS_ERROR);
        return false;
    }
    delete_data_source();

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

/*
bool DataField::has_alarm_state()
{
    return get_state() == DFS_ALARM;
}
*/

void DataField::_set_source_type(data_field_t source_type)
{
    _source_type = source_type;
}

data_field_t DataField::get_source_type()
{
    return _source_type;
}

const char *DataField::get_source_type_name()
{
    return this->_source_type_name;
}

uint32_t DataField::get_item_count()
{
    return _array_item_count;
}

void DataField::_set_item_size(uint8_t item_size)
{
    _array_item_size = item_size;
}

uint8_t DataField::get_item_size()
{
    return _array_item_size;
}

void *DataField::_get_src_pointer()
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

    memcpy(destination, _get_src_pointer(), get_data_byte_array_length());

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

    /*
    case DFS_ALARM:
        return _state_data_field_alarm;
    */

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

/******************************************************************************************************************************
 *
 * DataFieldInt8 class: implements int8 data field
 *
 ******************************************************************************************************************************/
const char *DataFieldInt8::_source_type_name = "int8";

DataFieldInt8::DataFieldInt8(void *data, uint32_t array_item_count)
    : DataField(data, array_item_count)
{
    _set_source_type(DF_INT8);
    _set_item_size(sizeof(int8_t));
    update_state();
}

DataFieldInt8::~DataFieldInt8()
{
}

const char *DataFieldInt8::get_source_type_name()
{
    return this->_source_type_name;
}

/******************************************************************************************************************************
 *
 * DataFieldUint8 class: implements uint8 data field
 *
 ******************************************************************************************************************************/
const char *DataFieldUint8::_source_type_name = "uint8";

DataFieldUint8::DataFieldUint8(void *data, uint32_t array_item_count)
    : DataField(data, array_item_count)
{
    _set_source_type(DF_UINT8);
    _set_item_size(sizeof(uint8_t));
    update_state();
}

DataFieldUint8::~DataFieldUint8()
{
}

const char *DataFieldUint8::get_source_type_name()
{
    return this->_source_type_name;
}

/******************************************************************************************************************************
 *
 * DataFieldInt16 class: implements uint16 data field
 *
 ******************************************************************************************************************************/
const char *DataFieldInt16::_source_type_name = "int16";

DataFieldInt16::DataFieldInt16(void *data, uint32_t array_item_count)
    : DataField(data, array_item_count)
{
    _set_source_type(DF_INT16);
    _set_item_size(sizeof(int16_t));
    update_state();
}

DataFieldInt16::~DataFieldInt16()
{
}

const char *DataFieldInt16::get_source_type_name()
{
    return this->_source_type_name;
}

/******************************************************************************************************************************
 *
 * DataFieldUint16 class: implements uint16 data field
 *
 ******************************************************************************************************************************/
const char *DataFieldUint16::_source_type_name = "uint16";

DataFieldUint16::DataFieldUint16(void *data, uint32_t array_item_count)
    : DataField(data, array_item_count)
{
    _set_source_type(DF_UINT16);
    _set_item_size(sizeof(uint16_t));
    update_state();
}

DataFieldUint16::~DataFieldUint16()
{
}

const char *DataFieldUint16::get_source_type_name()
{
    return this->_source_type_name;
}

/******************************************************************************************************************************
 *
 * DataFieldInt32 class: implements uint32 data field
 *
 ******************************************************************************************************************************/
const char *DataFieldInt32::_source_type_name = "int32";

DataFieldInt32::DataFieldInt32(void *data, uint32_t array_item_count)
    : DataField(data, array_item_count)
{
    _set_source_type(DF_INT32);
    _set_item_size(sizeof(int32_t));
    update_state();
}

DataFieldInt32::~DataFieldInt32()
{
}

const char *DataFieldInt32::get_source_type_name()
{
    return this->_source_type_name;
}

/******************************************************************************************************************************
 *
 * DataFieldUint32 class: implements uint32 data field
 *
 ******************************************************************************************************************************/
const char *DataFieldUint32::_source_type_name = "uint32";

DataFieldUint32::DataFieldUint32(void *data, uint32_t array_item_count)
    : DataField(data, array_item_count)
{
    _set_source_type(DF_UINT32);
    _set_item_size(sizeof(uint32_t));
    update_state();
}

DataFieldUint32::~DataFieldUint32()
{
}

const char *DataFieldUint32::get_source_type_name()
{
    return this->_source_type_name;
}
