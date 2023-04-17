#ifndef DATAFIELD_H
#define DATAFIELD_H
// #pragma once

#include <stdint.h>
#include <string.h>

#include "logger.h"
#include "CAN_common.h"

/******************************************************************************************************************************
 *
 * DataField interface: common for all types of data fields
 *
 ******************************************************************************************************************************/
class DataField
{
public:
    DataField(void *data, uint32_t array_item_count);
    ~DataField();

    bool operator==(const DataField &other);
    bool operator!=(const DataField &other);

    void delete_data_source();
    bool set_data_source(void *data_source = nullptr, uint32_t array_item_count = 1);
    bool has_data_source();

    data_field_t get_source_type();
    virtual const char *get_source_type_name();

    uint32_t get_item_count();
    uint8_t get_item_size();
    uint32_t get_data_byte_array_length();
    bool copy_data_to(void *destination, uint8_t destination_max_size);
    bool copy_data_from(void *source, uint8_t source_size, uint16_t dest_offset = 0);

    data_field_state_t get_state();
    void set_state(data_field_state_t state);
    virtual data_field_state_t update_state();
    bool has_errors();
    const char *get_state_name();

    virtual void print(const char *prefix);

protected:
    virtual void _print_handler(const char *prefix) = 0;

    void _set_source_type(data_field_t source_type);
    void _set_item_size(uint8_t item_size);
    void *_get_src_pointer();

    // virtual method for correct and systematic comparison of derived classes
    // should be complement by derived class
    virtual bool _equals(DataField const &other) const;

    // 'unknown' for logging
    static const char *_value_unknown;

    // the data field state names for logging
    static const char *_state_data_field_ok;
    static const char *_state_data_field_alarm;
    static const char *_state_data_field_error;

private:
    data_field_state_t _state; // the state of data field
    data_field_t _source_type = DF_UNKNOWN;
    uint8_t _array_item_size = 0;   // sizeof one item of array; _type related
    uint32_t _array_item_count = 0; // number of items in the data source array; for single variable it is 1

    void *_src_data_pointer = nullptr;

    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldInt8 class: implements int8 data field
 *
 ******************************************************************************************************************************/
class DataFieldInt8 : public DataField
{
public:
    DataFieldInt8(void *data, uint32_t array_item_count);
    ~DataFieldInt8();

    virtual const char *get_source_type_name() override;

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldUint8 class: implements uint8 data field
 *
 ******************************************************************************************************************************/
class DataFieldUint8 : public DataField
{
public:
    DataFieldUint8(void *data, uint32_t array_item_count);
    ~DataFieldUint8();

    virtual const char *get_source_type_name() override;

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldInt16 class: implements uint16 data field
 *
 ******************************************************************************************************************************/
class DataFieldInt16 : public DataField
{
public:
    DataFieldInt16(void *data, uint32_t array_item_count);
    ~DataFieldInt16();

    virtual const char *get_source_type_name() override;

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldUint16 class: implements uint16 data field
 *
 ******************************************************************************************************************************/
class DataFieldUint16 : public DataField
{
public:
    DataFieldUint16(void *data, uint32_t array_item_count);
    ~DataFieldUint16();

    virtual const char *get_source_type_name() override;

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldInt32 class: implements uint32 data field
 *
 ******************************************************************************************************************************/
class DataFieldInt32 : public DataField
{
public:
    DataFieldInt32(void *data, uint32_t array_item_count);
    ~DataFieldInt32();

    virtual const char *get_source_type_name() override;

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldUint32 class: implements uint32 data field
 *
 ******************************************************************************************************************************/
class DataFieldUint32 : public DataField
{
public:
    DataFieldUint32(void *data, uint32_t array_item_count);
    ~DataFieldUint32();

    virtual const char *get_source_type_name() override;

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;
};

/******************************************************************************************************************************
 *
 * DataFieldRawData class: implements data field with uint8 multichunk data array
 *
 ******************************************************************************************************************************/
class DataFieldRawData : public DataField
{
public:
    DataFieldRawData(void *data, uint32_t array_item_count,
                     raw_data_free_space_handler_t external_handler_free_space = nullptr,
                     raw_data_open_tmp_file_handler_t external_handler_open_file = nullptr,
                     raw_data_write_chunk_handler_t external_handler_write_chunk = nullptr,
                     raw_data_close_and_rename_file_handler_t external_handler_close_file = nullptr,
                     raw_data_abort_operations_handler_t external_handler_abort = nullptr);
    ~DataFieldRawData();

    virtual const char *get_source_type_name() override;

    // external handler: free space checker
    void set_external_handler_free_space_checker(raw_data_free_space_handler_t external_handler);
    raw_data_free_space_handler_t get_external_handler_free_space_checker();
    bool has_external_handler_free_space_checker();

    // external handler: open new temp file for writing
    void set_external_handler_open_file(raw_data_open_tmp_file_handler_t external_handler);
    raw_data_open_tmp_file_handler_t get_external_handler_open_file();
    bool has_external_handler_open_file();

    // external handler: write chunk
    void set_external_handler_write_chunk(raw_data_write_chunk_handler_t external_handler);
    raw_data_write_chunk_handler_t get_external_handler_write_chunk();
    bool has_external_handler_write_chunk();

    // external handler: close temp file and rename
    void set_external_handler_close_file(raw_data_close_and_rename_file_handler_t external_handler);
    raw_data_close_and_rename_file_handler_t get_external_handler_close_file();
    bool has_external_handler_close_file();

    // external handler: abort all
    void set_external_handler_abort(raw_data_abort_operations_handler_t external_handler);
    raw_data_abort_operations_handler_t get_external_handler_abort();
    bool has_external_handler_abort();

    uint16_t get_writing_delay();
    void set_writing_delay(uint16_t delay);

    uint8_t get_file_code();
    void set_file_code(uint8_t file_code);

    uint32_t get_expected_total_size();
    void set_expected_total_size(uint32_t total_size);

    uint16_t get_current_chunk_index();
    void set_current_chunk_index(uint16_t chunk_index);

    uint8_t get_current_chunk_size();
    void set_current_chunk_size(uint8_t chunk_size);

    uint16_t get_chunks_count();
    void set_chunks_count(uint16_t chunks_count);

    uint8_t get_expected_frame_index();
    uint8_t get_frames_count();

    // does it waiting for any frame (next ordinary or missed)
    bool is_waiting_for_frame();
    // does it waiting for the next ordinary frame
    bool is_waiting_for_next_frame();
    // does it waiting for missed frames
    bool is_waiting_for_missed_frame();

    uint8_t get_missed_frames_count();
    // return nullptr if there is no missed frames
    uint8_t *get_missed_frame_index();
    // are there missed frames in the current chunk
    bool has_missed_frames();

    virtual data_field_state_t update_state() override;

    // do we have enough free space for writing?
    bool has_free_space(uint32_t expected_total_size);
    // do write data sequence in progress? (file_code should be non-zero)
    bool is_sequence_in_progress();
    // initiates data writing operation (cheks free space, creates tmp-file, initializes variables)
    // file_code == 0 is reserved for 'no file' value
    bool start_data_writing(uint8_t file_code, uint32_t expected_total_size, uint8_t chunk_size);
    // initiates accumulation of the new one
    bool start_new_chunk_accumulation(uint16_t chunk_index, uint16_t chunks_count, uint8_t new_chunk_size);
    // load frame's data to the buffer
    // if frame_index is not equal to expected index, frames from expected till frame_index will be saved as missed
    bool process_new_frame(uint8_t frame_index, uint8_t *data, uint8_t data_length);
    // closes chunk accumulation process, writes complete data chunk from _src_data_pointer buffer to the file
    bool close_chunk_accumulation(uint8_t chunk_size, uint16_t chunk_index, uint16_t chunks_count);
    // finalizes file writing process (closes descriptors, resets to zero variables,
    // deletes old file, renames tmp-file to the correct file name)
    bool complete_data_writing();
    // it aborts all data manipulations, closes descriptors, deletes tmp-file, resets to zero variables, etc.
    void cancel_all();

protected:
    virtual void _print_handler(const char *prefix) override;

private:
    static const char *_source_type_name;

    // external handlers
    // free space checker
    raw_data_free_space_handler_t _ext_handler_free_space = nullptr;
    // open new temp file for writing
    raw_data_open_tmp_file_handler_t _ext_handler_open_file = nullptr;
    // write chunk
    raw_data_write_chunk_handler_t _ext_handler_write_chunk = nullptr;
    // close temp file and rename
    raw_data_close_and_rename_file_handler_t _ext_handler_close_file = nullptr;
    // abort all
    raw_data_abort_operations_handler_t _ext_handler_abort = nullptr;

    // the list of missed frames
    std::list<uint8_t> _missed_frames_list;

    // delay which is needed for chunk write operation
    uint16_t _delay_ms_for_writing = 200;

    // the size (bytes) of the file which is being received now
    uint32_t _current_total_data_size = 0;
    // the code (index) of the file which is being received now
    uint8_t _file_code = 0;

    // index of the current chunk
    uint16_t _current_chunk_index = 0;
    // the number of chunks in the current file
    uint16_t _chunks_count = 0;
    // the size of the chunk which is being received now
    uint8_t _current_chunk_size = 0;

    const uint8_t _frame_max_data_length = 6;
    // index of the expected frame (part of current chunk)
    uint8_t _expected_frame_index = 0;
    // expected number of frames in the current chunk
    uint8_t _frames_count = 0;
};

#endif // DATAFIELD_H