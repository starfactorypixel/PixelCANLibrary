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

    virtual void delete_data_source();
    virtual bool set_data_source(void *data_source = nullptr, uint32_t array_item_count = 1);
    bool has_data_source();

    data_field_t get_source_type();
    virtual const char *get_source_type_name();

    uint32_t get_item_count();
    uint8_t get_item_size();
    uint32_t get_data_byte_array_length();
    virtual bool copy_data_to(void *destination, uint8_t destination_max_size);

    data_field_state_t get_state();
    void set_state(data_field_state_t state);
    data_field_state_t update_state();
    bool has_errors();
    const char *get_state_name();

    virtual void print(const char *prefix);

protected:
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

private:
    static const char *_source_type_name;
};

#endif // DATAFIELD_H