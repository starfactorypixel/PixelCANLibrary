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

    if (destination == nullptr)
        return false;

    uint8_t bytes_to_copy = (destination_max_size < get_data_byte_array_length()) ? destination_max_size : get_data_byte_array_length();

    memcpy(destination, _get_src_pointer(), bytes_to_copy);

    return true;
}

bool DataField::copy_data_from(void *source, uint8_t source_size, uint16_t dest_offset)
{
    if (!has_data_source())
        return false;

    if (source == nullptr || source_size + dest_offset > get_data_byte_array_length())
        return false;

    memcpy(((uint8_t *)_get_src_pointer() + dest_offset), source, source_size);

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

    this->_print_handler(prefix);
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

void DataFieldInt8::_print_handler(const char *prefix)
{
    LOGwoN("%sDataFieldInt8: [", prefix);
    uint32_t items_count = get_item_count();
    for (uint32_t i = 0; i < items_count; i++)
    {
        LOGstring("%d%s", ((int8_t *)_get_src_pointer())[i], (i == items_count - 1) ? "" : ", ");
    }
    LOGstring("]\n");
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

void DataFieldUint8::_print_handler(const char *prefix)
{
    LOGwoN("%sDataFieldUint8: [", prefix);
    uint32_t items_count = get_item_count();
    for (uint32_t i = 0; i < items_count; i++)
    {
        LOGstring("%u%s", ((uint8_t *)_get_src_pointer())[i], (i == items_count - 1) ? "" : ", ");
    }
    LOGstring("]\n");
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

void DataFieldInt16::_print_handler(const char *prefix)
{
    LOGwoN("%sDataFieldInt16: [", prefix);
    uint32_t items_count = get_item_count();
    for (uint32_t i = 0; i < items_count; i++)
    {
        LOGstring("%d%s", ((int16_t *)_get_src_pointer())[i], (i == items_count - 1) ? "" : ", ");
    }
    LOGstring("]\n");
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

void DataFieldUint16::_print_handler(const char *prefix)
{
    LOGwoN("%sDataFieldUint16: [", prefix);
    uint32_t items_count = get_item_count();
    for (uint32_t i = 0; i < items_count; i++)
    {
        LOGstring("%u%s", ((uint16_t *)_get_src_pointer())[i], (i == items_count - 1) ? "" : ", ");
    }
    LOGstring("]\n");
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

void DataFieldInt32::_print_handler(const char *prefix)
{
    LOGwoN("%sDataFieldInt32: [", prefix);
    uint32_t items_count = get_item_count();
    for (uint32_t i = 0; i < items_count; i++)
    {
        LOGstring("%d%s", ((int32_t *)_get_src_pointer())[i], (i == items_count - 1) ? "" : ", ");
    }
    LOGstring("]\n");
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

void DataFieldUint32::_print_handler(const char *prefix)
{
    LOGwoN("%sDataFieldUint32: [", prefix);
    uint32_t items_count = get_item_count();
    for (uint32_t i = 0; i < items_count; i++)
    {
        LOGstring("%lu%s", ((uint32_t *)_get_src_pointer())[i], (i == items_count - 1) ? "" : ", ");
    }
    LOGstring("]\n");
}

/******************************************************************************************************************************
 *
 * DataFieldRawData class: implements data field with uint8 multichunk data array
 *
 ******************************************************************************************************************************/
const char *DataFieldRawData::_source_type_name = "multichunk raw data";

DataFieldRawData::DataFieldRawData(void *data, uint32_t array_item_count,
                                   raw_data_free_space_handler_t external_handler_free_space,
                                   raw_data_open_tmp_file_handler_t external_handler_open_file,
                                   raw_data_write_chunk_handler_t external_handler_write_chunk,
                                   raw_data_close_and_rename_file_handler_t external_handler_close_file,
                                   raw_data_abort_operations_handler_t external_handler_abort)
    : DataField(data, array_item_count), _ext_handler_free_space(external_handler_free_space),
      _ext_handler_open_file(external_handler_open_file), _ext_handler_write_chunk(external_handler_write_chunk),
      _ext_handler_close_file(external_handler_close_file), _ext_handler_abort(external_handler_abort)
{
    _set_source_type(DF_RAW_DATA_ARRAY);
    _set_item_size(sizeof(uint8_t));
    update_state();
}

DataFieldRawData::~DataFieldRawData()
{
}

const char *DataFieldRawData::get_source_type_name()
{
    return this->_source_type_name;
}

// external handler: free space checker
void DataFieldRawData::set_external_handler_free_space_checker(raw_data_free_space_handler_t external_handler)
{
    _ext_handler_free_space = external_handler;
}

raw_data_free_space_handler_t DataFieldRawData::get_external_handler_free_space_checker()
{
    return _ext_handler_free_space;
}

bool DataFieldRawData::has_external_handler_free_space_checker()
{
    return get_external_handler_free_space_checker() != nullptr;
}

// external handler: open new temp file for writing
void DataFieldRawData::set_external_handler_open_file(raw_data_open_tmp_file_handler_t external_handler)
{
    _ext_handler_open_file = external_handler;
}

raw_data_open_tmp_file_handler_t DataFieldRawData::get_external_handler_open_file()
{
    return _ext_handler_open_file;
}

bool DataFieldRawData::has_external_handler_open_file()
{
    return get_external_handler_open_file() != nullptr;
}

// external handler: write chunk
void DataFieldRawData::set_external_handler_write_chunk(raw_data_write_chunk_handler_t external_handler)
{
    _ext_handler_write_chunk = external_handler;
}

raw_data_write_chunk_handler_t DataFieldRawData::get_external_handler_write_chunk()
{
    return _ext_handler_write_chunk;
}

bool DataFieldRawData::has_external_handler_write_chunk()
{
    return get_external_handler_write_chunk() != nullptr;
}

// external handler: close temp file and rename
void DataFieldRawData::set_external_handler_close_file(raw_data_close_and_rename_file_handler_t external_handler)
{
    _ext_handler_close_file = external_handler;
}

raw_data_close_and_rename_file_handler_t DataFieldRawData::get_external_handler_close_file()
{
    return _ext_handler_close_file;
}

bool DataFieldRawData::has_external_handler_close_file()
{
    return get_external_handler_close_file() != nullptr;
}

// external handler: abort all
void DataFieldRawData::set_external_handler_abort(raw_data_abort_operations_handler_t external_handler)
{
    _ext_handler_abort = external_handler;
}

raw_data_abort_operations_handler_t DataFieldRawData::get_external_handler_abort()
{
    return _ext_handler_abort;
}

bool DataFieldRawData::has_external_handler_abort()
{
    return get_external_handler_abort() != nullptr;
}

uint16_t DataFieldRawData::get_writing_delay()
{
    return _delay_ms_for_writing;
}

void DataFieldRawData::set_writing_delay(uint16_t delay)
{
    _delay_ms_for_writing = delay;
}

uint8_t DataFieldRawData::get_file_code()
{
    return _file_code;
}

void DataFieldRawData::set_file_code(uint8_t file_code)
{
    _file_code = file_code;
}

uint32_t DataFieldRawData::get_expected_total_size()
{
    return _current_total_data_size;
}

void DataFieldRawData::set_expected_total_size(uint32_t total_size)
{
    _current_total_data_size = total_size;
}

uint16_t DataFieldRawData::get_current_chunk_index()
{
    return _current_chunk_index;
}

void DataFieldRawData::set_current_chunk_index(uint16_t chunk_index)
{
    _current_chunk_index = chunk_index;
}

uint8_t DataFieldRawData::get_current_chunk_size()
{
    return _current_chunk_size;
}

void DataFieldRawData::set_current_chunk_size(uint8_t chunk_size)
{
    if (chunk_size > get_data_byte_array_length())
    {
        set_state(DFS_ERROR);
        return;
    }
    _current_chunk_size = chunk_size;
    _frames_count = chunk_size / _frame_max_data_length;
    if (chunk_size % _frame_max_data_length != 0)
        ++_frames_count;
}

uint16_t DataFieldRawData::get_chunks_count()
{
    return _chunks_count;
}

void DataFieldRawData::set_chunks_count(uint16_t chunks_count)
{
    _chunks_count = chunks_count;
}

uint8_t DataFieldRawData::get_expected_frame_index()
{
    return _expected_frame_index;
}

uint8_t DataFieldRawData::get_frames_count()
{
    return _frames_count;
}

// does it waiting for any frame (next ordinary or missed)
bool DataFieldRawData::is_waiting_for_frame()
{
    return is_waiting_for_next_frame() || is_waiting_for_missed_frame();
}

// does it waiting for the next ordinary frame
bool DataFieldRawData::is_waiting_for_next_frame()
{
    return (_expected_frame_index < _frames_count);
}

// does it waiting for missed frames
bool DataFieldRawData::is_waiting_for_missed_frame()
{
    return get_missed_frames_count() > 0;
}

uint8_t DataFieldRawData::get_missed_frames_count()
{
    return _missed_frames_list.size();
}

// return nullptr if there is no missed frames
uint8_t *DataFieldRawData::get_missed_frame_index()
{
    if (has_missed_frames())
    {
        return &_missed_frames_list.front();
    }

    return nullptr;
}

// are there missed frames in the current chunk
bool DataFieldRawData::has_missed_frames()
{
    return !_missed_frames_list.empty();
}

data_field_state_t DataFieldRawData::update_state()
{
    DataField::update_state();

    return get_state();
}

// do we have enough free space for writing?
bool DataFieldRawData::has_free_space(uint32_t expected_total_size)
{
    bool result = false;
    if (has_external_handler_free_space_checker())
        result = get_external_handler_free_space_checker()(expected_total_size);

    LOG("DataFieldRawData has_free_space(): need %d bytes of free space, result = %s", expected_total_size, result ? "available" : "no free space");

    return result;
}

bool DataFieldRawData::is_sequence_in_progress()
{
    return get_file_code() != 0;
}

// initiates data writing operation (cheks free space, creates tmp-file, initializes variables)
bool DataFieldRawData::start_data_writing(uint8_t file_code, uint32_t expected_total_size, uint8_t chunk_size)
{
    LOG("DataFieldRawData start_data_writing(): file %d, total %d bytes, chunk size %d bytes", file_code, expected_total_size, chunk_size);

    // file_code == 0 is reserved for 'no file' value
    if (file_code == 0 || !has_free_space(expected_total_size))
    {
        cancel_all();
        return false;
    }

    if (chunk_size > get_data_byte_array_length())
    {
        cancel_all();
        return false;
    }

    // abort all previous data writings
    if (is_sequence_in_progress())
    {
        cancel_all();
    }

    if (has_external_handler_open_file() && !get_external_handler_open_file()())
    {
        cancel_all();
        return false;
    }

    set_current_chunk_size(chunk_size);
    set_file_code(file_code);
    set_expected_total_size(expected_total_size);

    uint16_t chunks_count = expected_total_size / chunk_size;
    if (expected_total_size % chunk_size != 0)
    {
        chunks_count++;
    }
    set_chunks_count(chunks_count);

    return true;
}

// initiates accumulation of the new one
bool DataFieldRawData::start_new_chunk_accumulation(uint16_t chunk_index, uint16_t chunks_count, uint8_t new_chunk_size)
{
    // data writing isn't started
    if (get_file_code() == 0)
    {
        return false;
    }

    if (new_chunk_size > get_data_byte_array_length() || new_chunk_size > get_current_chunk_size() ||
        chunk_index != get_current_chunk_index() ||
        chunks_count != get_chunks_count())
    {
        cancel_all();
        return false;
    }

    set_current_chunk_size(new_chunk_size);
    _expected_frame_index = 0;

    return true;
}

// load frame's data to the buffer
// if frame_index is not equal to expected index, frames from expected till frame_index will be saved as missed
bool DataFieldRawData::process_new_frame(uint8_t frame_index, uint8_t *data, uint8_t data_length)
{
    if (data == nullptr || data_length == 0 ||
        frame_index >= get_frames_count() ||
        (frame_index < get_frames_count() - 1 && data_length != _frame_max_data_length)) // only last one may have data length smaller then max
    {
        cancel_all();
        return false;
    }

    // this frame is expected,
    if (frame_index == get_expected_frame_index())
    {
        // but we should check if there enough free space in the buffer
        if (frame_index * _frame_max_data_length + data_length > get_data_byte_array_length())
        {
            cancel_all();
            return false; // haven't space for data
        }
        else
        {
            _expected_frame_index++; // count current frame
        }
    }
    // this frame was missed and now it has been repeated
    else if (frame_index < get_expected_frame_index())
    {
        bool frame_really_missed = false;
        for (uint8_t i : _missed_frames_list)
        {
            if (i == frame_index)
            {
                _missed_frames_list.remove(frame_index);
                frame_really_missed = true;
                break;
            }
        }

        if (!frame_really_missed)
        {
            // received frame wasn't really missed
            cancel_all();
            return false;
        }
    }
    // some frames were missed
    else if (frame_index > get_expected_frame_index())
    {
        uint8_t start_index = get_expected_frame_index();
        for (uint8_t i = start_index; i < frame_index; i++)
        {
            _expected_frame_index++;
            _missed_frames_list.push_back(i);
        }
        _missed_frames_list.unique();
        _expected_frame_index++; // count current frame too
    }

    if (!copy_data_from(data, data_length, frame_index * _frame_max_data_length))
    {
        cancel_all();
        return false;
    }
    return true;
}

// closes chunk accumulation process, writes complete data chunk from _src_data_pointer buffer to the file
bool DataFieldRawData::close_chunk_accumulation(uint8_t chunk_size, uint16_t chunk_index, uint16_t chunks_count)
{
    if (chunk_index != get_current_chunk_index() || chunks_count != get_chunks_count())
    {
        cancel_all();
        return false;
    }

    // if some last frames was lost
    while (is_waiting_for_next_frame())
    {
        _missed_frames_list.push_back(_expected_frame_index);
        _expected_frame_index++;
    }

    if (has_missed_frames())
    {
        // don't cancel data sequence and wait for missing frames
        return false;
    }

    if (has_external_handler_write_chunk() && !get_external_handler_write_chunk()(chunk_size, (uint8_t *)_get_src_pointer()))
    {
        cancel_all();
        return false;
    }

    // waiting for the next chunk
    set_current_chunk_index(chunk_index + 1);

    return true;
}

// finalizes file writing process (closes descriptors, resets to zero variables,
// deletes old file, renames tmp-file to the correct file name)
bool DataFieldRawData::complete_data_writing()
{
    if (has_external_handler_close_file() && !get_external_handler_close_file()(get_file_code()))
    {
        cancel_all();
        return false;
    }

    return true;
}

// it aborts all data manipulations, closes descriptors, deletes tmp-file, resets to zero variables, etc.
void DataFieldRawData::cancel_all()
{
    if (has_external_handler_abort())
    {
        get_external_handler_abort()();
    }
    
    _current_total_data_size = 0;
    _file_code = 0;
    _current_chunk_index = 0;
    _chunks_count = 0;
    _current_chunk_size = 0;
    _expected_frame_index = 0;
    _frames_count = 0;
    _missed_frames_list.clear();
    update_state();
}

void DataFieldRawData::_print_handler(const char *prefix)
{
    LOG("%sDataFieldRawData:", prefix);

    LOG("%s    file = %d, write delay = %d ms, total data size = %d bytes, chunk = %d of %d (%d bytes), frame = %d of %d, max frame data length = %d", prefix,
        get_file_code(), get_writing_delay(), get_expected_total_size(),
        get_current_chunk_index(), get_chunks_count(), get_current_chunk_size(),
        _expected_frame_index, get_frames_count(), _frame_max_data_length);

    LOG("%s    external handlers:", prefix);
    LOG("%s        free space checker = %s", prefix, has_external_handler_free_space_checker() ? "yes" : "no");
    LOG("%s        open temp file = %s", prefix, has_external_handler_open_file() ? "yes" : "no");
    LOG("%s        write chunk = %s", prefix, has_external_handler_write_chunk() ? "yes" : "no");
    LOG("%s        close file and rename = %s", prefix, has_external_handler_close_file() ? "yes" : "no");
    LOG("%s        abort operations = %s", prefix, has_external_handler_abort() ? "yes" : "no");
}
