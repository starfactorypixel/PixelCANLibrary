#ifndef DATA_FIELD_ATTENTION_CHECKER_H
#define DATA_FIELD_ATTENTION_CHECKER_H

#include "CAN_common.h"
#include "logger.h"

/******************************************************************************************************************************
 *
 * DataFieldAttentionCheckerBase interface: common interface for all attention checkers of data field
 *
 ******************************************************************************************************************************/
class DataFieldAttentionCheckerBase
{
public:
    DataFieldAttentionCheckerBase() = delete;
    DataFieldAttentionCheckerBase(uint8_t type_size,
                                  data_field_attention_state_t passed_state = DF_ATTENTION_STATE_NONE,
                                  data_field_attention_state_t not_passed_state = DF_ATTENTION_STATE_NONE)
        : _size(type_size),
          _passed_state(passed_state), _not_passed_state(not_passed_state){};

    virtual ~DataFieldAttentionCheckerBase(){};

    data_field_attention_state_t attention_check(void *data, uint8_t data_type_size)
    {
        if (data == nullptr || data_type_size != get_size())
        {
            return DF_ATTENTION_STATE_NONE;
        }

        return (this->_attention_check(data)) ? _passed_state : _not_passed_state;
    };

    void print(const char *prefix) { this->_print_handler(prefix); };

    uint8_t get_size() { return _size; };

    data_field_attention_state_t get_passed_state() { return _passed_state; };
    void set_passed_state(data_field_attention_state_t state) { _passed_state = state; };
    data_field_attention_state_t get_not_passed_state() { return _not_passed_state; };
    void set_not_passed_state(data_field_attention_state_t state) { _not_passed_state = state; };

protected:
    virtual void _print_handler(const char *prefix) = 0;
    virtual bool _attention_check(void *data) = 0;

    uint8_t _size = 0;
    data_field_attention_state_t _passed_state = DF_ATTENTION_STATE_NONE;
    data_field_attention_state_t _not_passed_state = DF_ATTENTION_STATE_NONE;
};

/******************************************************************************************************************************
 *
 * DataFieldInRangeChecker: is the DataField value placed within the range?
 *
 ******************************************************************************************************************************/
template <typename T>
class DataFieldInRangeChecker : public DataFieldAttentionCheckerBase
{
public:
    DataFieldInRangeChecker() = delete;
    DataFieldInRangeChecker(T min, T max,
                            data_field_attention_state_t passed_state = DF_ATTENTION_STATE_NONE,
                            data_field_attention_state_t not_passed_state = DF_ATTENTION_STATE_NONE)
        : DataFieldAttentionCheckerBase(sizeof(T), passed_state, not_passed_state), _min(min), _max(max){};

    virtual ~DataFieldInRangeChecker(){};

protected:
    virtual bool _attention_check(void *data) override { return (*(T *)data >= _min && *(T *)data <= _max); };

    virtual void _print_handler(const char *prefix) override
    {
        LOG("%sDataFieldInRangeChecker: min = %d, max = %d", prefix, _min, _max);
    };

private:
    T _min = 0;
    T _max = 0;
};

/******************************************************************************************************************************
 *
 * DataFieldMoreOrEqualChecker: is the DataField value more or equal then limit?
 *
 ******************************************************************************************************************************/
template <typename T>
class DataFieldMoreOrEqualChecker : public DataFieldAttentionCheckerBase
{
public:
    DataFieldMoreOrEqualChecker() = delete;
    DataFieldMoreOrEqualChecker(T limit,
                                data_field_attention_state_t passed_state = DF_ATTENTION_STATE_NONE,
                                data_field_attention_state_t not_passed_state = DF_ATTENTION_STATE_NONE)
        : DataFieldAttentionCheckerBase(sizeof(T), passed_state, not_passed_state), _limit(limit){};

    virtual ~DataFieldMoreOrEqualChecker(){};

protected:
    virtual bool _attention_check(void *data) override { return (*(T *)data >= _limit); };

    virtual void _print_handler(const char *prefix) override
    {
        LOG("%sDataFieldMoreOrEqualChecker: limit = %d", prefix, _limit);
    };

private:
    T _limit = 0;
};

/******************************************************************************************************************************
 *
 * DataFieldLessOrEqualChecker: is the DataField value less or equal then limit?
 *
 ******************************************************************************************************************************/
template <typename T>
class DataFieldLessOrEqualChecker : public DataFieldAttentionCheckerBase
{
public:
    DataFieldLessOrEqualChecker() = delete;
    DataFieldLessOrEqualChecker(T limit,
                                data_field_attention_state_t passed_state = DF_ATTENTION_STATE_NONE,
                                data_field_attention_state_t not_passed_state = DF_ATTENTION_STATE_NONE)
        : DataFieldAttentionCheckerBase(sizeof(T), passed_state, not_passed_state), _limit(limit){};

    virtual ~DataFieldLessOrEqualChecker(){};

protected:
    virtual bool _attention_check(void *data) override { return (*(T *)data <= _limit); };

    virtual void _print_handler(const char *prefix) override
    {
        LOG("%sDataFieldLessOrEqualChecker: limit = %d", prefix, _limit);
    };

private:
    T _limit = 0;
};

#endif // DATA_FIELD_ATTENTION_CHECKER_H