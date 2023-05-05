#include "CANFunction.h"
#include "logger.h"
#include "CANFrame.h"
#include "CANObject.h"
#include "CANManager.h"
#include <typeinfo>

/******************************************************************************************************************************
 *
 * CANFunction base class: base class for all CAN functions
 *
 ******************************************************************************************************************************/
const char *CANFunctionBase::_state_function_disabled = "disabled";
const char *CANFunctionBase::_state_function_active = "active";
const char *CANFunctionBase::_state_function_suspended = "suspended";
const char *CANFunctionBase::_value_unknown = "unknown";

const char *CANFunctionBase::_type_function_responding = "responding";
const char *CANFunctionBase::_type_function_automatic = "automatic";
const char *CANFunctionBase::_type_function_blended = "blended";
const char *CANFunctionBase::_type_function_indirect = "indirect";

CANFunctionBase::CANFunctionBase(CAN_function_id_t id,
                                 CANObject *parent,
                                 CAN_function_handler_t external_handler,
                                 CANFunctionBase *next_ok_function,
                                 CANFunctionBase *next_err_function)
    : _id(id), _state(CAN_FS_DISABLED), _parent(parent), _external_handler(external_handler),
      _next_ok_function(next_ok_function), _next_err_function(next_err_function), _name(nullptr)
{
}

CANFunctionBase::~CANFunctionBase()
{
}

bool CANFunctionBase::operator==(const CANFunctionBase &other)
{
    return this->_equals(other);
}

bool CANFunctionBase::operator!=(const CANFunctionBase &other)
{
    return !(*this == other);
}

bool CANFunctionBase::_equals(CANFunctionBase const &other) const
{
    if (typeid(*this) != typeid(other))
        return false;

    return (this->_id == other._id);
}

void CANFunctionBase::set_id(CAN_function_id_t id /*, const char *name*/)
{
    _id = id;
}

CAN_function_id_t CANFunctionBase::get_id()
{
    return _id;
}

void CANFunctionBase::set_parent(CANObject *parent)
{
    _parent = parent;
}

CANObject *CANFunctionBase::get_parent()
{
    return _parent;
}

void CANFunctionBase::set_external_handler(CAN_function_handler_t external_handler)
{
    _external_handler = external_handler;
}

CAN_function_handler_t CANFunctionBase::get_external_handler()
{
    return _external_handler;
}

bool CANFunctionBase::has_external_handler()
{
    return get_external_handler() != nullptr;
}

void CANFunctionBase::set_next_ok_function(CANFunctionBase *next_ok_function)
{
    _next_ok_function = next_ok_function;
}

CANFunctionBase *CANFunctionBase::get_next_ok_function()
{
    return _next_ok_function;
}

bool CANFunctionBase::has_next_ok_function()
{
    return get_next_ok_function() != nullptr;
}

void CANFunctionBase::set_next_err_function(CANFunctionBase *next_err_function)
{
    _next_err_function = next_err_function;
}

CANFunctionBase *CANFunctionBase::get_next_err_function()
{
    return _next_err_function;
}

bool CANFunctionBase::has_next_err_function()
{
    return get_next_err_function() != nullptr;
}

void CANFunctionBase::disable()
{
    _set_state(CAN_FS_DISABLED);
}

void CANFunctionBase::enable()
{
    _set_state(CAN_FS_ACTIVE);
}

void CANFunctionBase::suspend()
{
    _set_state(CAN_FS_SUSPENDED);
}

bool CANFunctionBase::is_active()
{
    return get_state() != CAN_FS_DISABLED;
}

bool CANFunctionBase::is_suspended()
{
    return get_state() == CAN_FS_SUSPENDED;
}

CAN_function_state_t CANFunctionBase::get_state()
{
    return _state;
}

const char *CANFunctionBase::get_state_name()
{
    switch (get_state())
    {
    case CAN_FS_DISABLED:
        return _state_function_disabled;

    case CAN_FS_ACTIVE:
        return _state_function_active;

    case CAN_FS_SUSPENDED:
        return _state_function_suspended;

    default:
        return _value_unknown;
    }
}

void CANFunctionBase::_set_state(CAN_function_state_t state)
{
    _state = state;
}

// function main handler
bool CANFunctionBase::process(CANFrame *can_frame)
{
    if (!is_active())
        return false;

    CAN_function_result_t result = CAN_RES_NONE;

    if (has_external_handler())
    {
        result = _before_external_handler(can_frame);
        if (result == CAN_RES_NEXT_OK)
        {
            result = (get_external_handler())(*get_parent(), *this, can_frame);
            result = _after_external_handler(result, can_frame);
        }
    }
    else
    {
        result = _default_handler(can_frame);
    }

    if (result == CAN_RES_NEXT_OK && has_next_ok_function())
    {
        get_next_ok_function()->process(can_frame);
    }
    else if (result == CAN_RES_NEXT_ERR && has_next_err_function())
    {
        get_next_err_function()->process(can_frame);
    }

    return result != CAN_RES_NONE;
}

bool CANFunctionBase::is_indirect_function()
{
    return !this->is_responding_function() && !this->is_automatic_function();
}

void CANFunctionBase::print(const char *prefix)
{
    LOG("%sFunction: id = 0x%02X (%s), state = %s, responding = %s, automatic = %s, indirect = %s",
        prefix, get_id(), get_name(), get_state_name(),
        is_responding_function() ? "yes" : "no",
        is_automatic_function() ? "yes" : "no",
        is_indirect_function() ? "yes" : "no");
}

void CANFunctionBase::_fill_error_can_frame(CANFrame &can_frame, pixel_error_section_t error_section, uint8_t error_code)
{
    struct __attribute__((__packed__)) error_frame_data_t
    {
        CAN_function_id_t func_id;
        pixel_error_section_t error_section;
        uint8_t error_code;
    };

    uint8_t error_data[CAN_MAX_PAYLOAD] = {0};
    error_frame_data_t *error_structured = (error_frame_data_t *)error_data;

    error_structured->func_id = get_id(); // default behaviour: id will be rewrited by sender function

    error_structured->error_section = error_section;
    error_structured->error_code = error_code;

    can_frame.clear_frame();
    can_frame.set_frame(get_parent()->get_id(), error_data, sizeof(error_frame_data_t));
}

/******************************************************************************************************************************
 *
 * CANFunctionTimerBase: class for timed messages
 *
 ******************************************************************************************************************************/
CANFunctionTimerBase::CANFunctionTimerBase(CAN_function_id_t id,
                                           CANObject *parent,
                                           uint32_t period_ms,
                                           CAN_function_handler_t external_handler,
                                           CANFunctionBase *next_ok_function,
                                           CANFunctionBase *next_err_function)
    : CANFunctionBase(id, parent, external_handler, next_ok_function, next_err_function),
      _period_ms(period_ms), _last_action_tick(0)
{
}

bool CANFunctionTimerBase::_equals(CANFunctionBase const &other) const
{
    if (typeid(*this) != typeid(other))
        return false;

    CANFunctionTimerBase const *that = static_cast<CANFunctionTimerBase const *>(&other);
    if (this->_period_ms != that->_period_ms)
        return false;
    return CANFunctionBase::_equals(other);
}

void CANFunctionTimerBase::set_period(uint32_t period_ms)
{
    _period_ms = period_ms;
}

uint32_t CANFunctionTimerBase::get_period()
{
    return _period_ms;
}

bool CANFunctionTimerBase::is_responding_function()
{
    return false;
}

bool CANFunctionTimerBase::is_automatic_function()
{
    return true;
}

// enables or suspend all timers of the object depending of the object's attention state
void CANFunctionTimerBase::_timers_family_validator()
{
    CANFunctionBase *timer = nullptr;
    CANObject &can_object = *get_parent();

    switch (can_object.get_max_attention_state())
    {
    case DF_ATTENTION_STATE_NORMAL:
        timer = can_object.get_function(CAN_FUNC_TIMER_NORMAL);
        if (timer != nullptr)
            timer->enable();
        timer = can_object.get_function(CAN_FUNC_TIMER_WARNING);
        if (timer != nullptr)
            timer->suspend();
        timer = can_object.get_function(CAN_FUNC_TIMER_CRITICAL);
        if (timer != nullptr)
            timer->suspend();
        break;

    case DF_ATTENTION_STATE_WARNING:
        timer = can_object.get_function(CAN_FUNC_TIMER_NORMAL);
        if (timer != nullptr)
            timer->suspend();
        timer = can_object.get_function(CAN_FUNC_TIMER_WARNING);
        if (timer != nullptr)
            timer->enable();
        timer = can_object.get_function(CAN_FUNC_TIMER_CRITICAL);
        if (timer != nullptr)
            timer->suspend();
        break;

    case DF_ATTENTION_STATE_CRITICAL:
        timer = can_object.get_function(CAN_FUNC_TIMER_NORMAL);
        if (timer != nullptr)
            timer->suspend();
        timer = can_object.get_function(CAN_FUNC_TIMER_WARNING);
        if (timer != nullptr)
            timer->suspend();
        timer = can_object.get_function(CAN_FUNC_TIMER_CRITICAL);
        if (timer != nullptr)
            timer->enable();
        break;

    case DF_ATTENTION_STATE_NONE:
    default:
        break;
    }
}

CAN_function_result_t CANFunctionTimerBase::_default_handler(CANFrame *can_frame)
{
    _timers_family_validator();

    if (!this->is_suspended() && get_parent()->get_tick() - _last_action_tick >= get_period())
    {
        _last_action_tick = get_parent()->get_tick();
        return _timer_handler();
    }

    return CAN_RES_NONE;
}

CAN_function_result_t CANFunctionTimerBase::_before_external_handler(CANFrame *can_frame)
{
    _timers_family_validator();

    if (!this->is_suspended() && get_parent()->get_tick() - _last_action_tick >= get_period())
    {
        _last_action_tick = get_parent()->get_tick();
        return CAN_RES_NEXT_OK;
    }

    return CAN_RES_FINAL;
}

/******************************************************************************************************************************
 *
 * CANFunctionTimerNormal: class for normal timed messages
 *
 ******************************************************************************************************************************/
const char *CANFunctionTimerNormal::_name = "CANFunctionTimerNormal";

CANFunctionTimerNormal::CANFunctionTimerNormal(CANObject *parent, uint32_t period_ms, CAN_function_handler_t external_handler,
                                               CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionTimerBase(CAN_FUNC_TIMER_NORMAL, parent, period_ms, external_handler, next_ok_function, next_err_function)
{
    enable();
}

const char *CANFunctionTimerNormal::get_name()
{
    return this->_name;
}

CAN_function_result_t CANFunctionTimerNormal::_timer_handler()
{
    CANFrame can_frame;
    CANObject &can_object = *get_parent();
    can_object.fill_can_frame_with_data(can_frame, get_id());

    can_frame.print("CANFunctionTimerNormal: ");

    CANManager &can_manager = *can_object.get_parent();
    can_manager.add_tx_queue_item(can_frame);

    return CAN_RES_FINAL;
}

/******************************************************************************************************************************
 *
 * CANFunctionTimerWarning: class for the warning timed messages
 *
 ******************************************************************************************************************************/
const char *CANFunctionTimerWarning::_name = "CANFunctionTimerWarning";

CANFunctionTimerWarning::CANFunctionTimerWarning(CANObject *parent, uint32_t period_ms, CAN_function_handler_t external_handler,
                                                 CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionTimerBase(CAN_FUNC_TIMER_WARNING, parent, period_ms, external_handler, next_ok_function, next_err_function)
{
    enable();
}

const char *CANFunctionTimerWarning::get_name()
{
    return this->_name;
}

CAN_function_result_t CANFunctionTimerWarning::_timer_handler()
{
    CANFrame can_frame;
    CANObject &can_object = *get_parent();
    can_object.fill_can_frame_with_data(can_frame, get_id());

    can_frame.print("CANFunctionTimerWarning: ");

    CANManager &can_manager = *can_object.get_parent();
    can_manager.add_tx_queue_item(can_frame);

    return CAN_RES_FINAL;
}

/******************************************************************************************************************************
 *
 * CANFunctionTimerCritical: class for the critical timed messages
 *
 ******************************************************************************************************************************/
const char *CANFunctionTimerCritical::_name = "CANFunctionTimerCritical";

CANFunctionTimerCritical::CANFunctionTimerCritical(CANObject *parent, uint32_t period_ms, CAN_function_handler_t external_handler,
                                                   CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionTimerBase(CAN_FUNC_TIMER_CRITICAL, parent, period_ms, external_handler, next_ok_function, next_err_function)
{
    enable();
}

const char *CANFunctionTimerCritical::get_name()
{
    return this->_name;
}

CAN_function_result_t CANFunctionTimerCritical::_timer_handler()
{
    CANFrame can_frame;
    CANObject &can_object = *get_parent();
    can_object.fill_can_frame_with_data(can_frame, get_id());

    can_frame.print("CANFunctionTimerCritical: ");

    CANManager &can_manager = *can_object.get_parent();
    can_manager.add_tx_queue_item(can_frame);

    return CAN_RES_FINAL;
}

/******************************************************************************************************************************
 *
 * CANFunctionRequest: class for incoming request
 *
 ******************************************************************************************************************************/
const char *CANFunctionRequest::_name = "CANFunctionRequest";

CANFunctionRequest::CANFunctionRequest(CANObject *parent, CAN_function_handler_t external_handler,
                                       CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionBase(CAN_FUNC_REQUEST_IN, parent, external_handler, next_ok_function, next_err_function)
{
    enable();
}

bool CANFunctionRequest::is_responding_function()
{
    return true;
}

bool CANFunctionRequest::is_automatic_function()
{
    return false;
}

const char *CANFunctionRequest::get_name()
{
    return this->_name;
}

CAN_function_result_t CANFunctionRequest::_default_handler(CANFrame *can_frame)
{
    if (can_frame == nullptr)
        return CAN_RES_NEXT_ERR;

    can_frame->print("CANFunctionRequest [incoming frame]: ");

    if (can_frame->get_data_length() != 1)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        return CAN_RES_NEXT_ERR;
    }

    CANObject &co = *get_parent();
    can_object_state_t co_state = co.update_state();

    if (co.is_state_ok())
    {
        co.fill_can_frame_with_data(*can_frame, get_id());
        return CAN_RES_NEXT_OK;
    }

    _fill_error_can_frame(*can_frame, PIX_ERR_CAN_OBJECT, co_state);
    return CAN_RES_NEXT_ERR;
}

/******************************************************************************************************************************
 *
 * CANFunctionSimpleSender: just sends the CAN frame specified in the _default_handler() parameters,
 * overrides specified in the CAN frame function ID with value, returned by get_id()
 * May be useful as the base class for all functions with outcoming messages. But it is very useful itself.
 *
 ******************************************************************************************************************************/
const char *CANFunctionSimpleSender::_name = "CANFunctionSimpleSender";

CANFunctionSimpleSender::CANFunctionSimpleSender(CANObject *parent, CAN_function_handler_t external_handler,
                                                 CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionBase(CAN_FUNC_SIMPLE_SENDER, parent, external_handler, next_ok_function, next_err_function)
{
    enable();
}

bool CANFunctionSimpleSender::is_responding_function()
{
    return false;
}

bool CANFunctionSimpleSender::is_automatic_function()
{
    return false;
}

const char *CANFunctionSimpleSender::get_name()
{
    return this->_name;
}

CAN_function_result_t CANFunctionSimpleSender::_default_handler(CANFrame *can_frame)
{
    CANFrame cf;
    CAN_function_result_t result = CAN_RES_NONE;

    if (can_frame == nullptr)
    {
        result = CAN_RES_NEXT_ERR;
        _fill_error_can_frame(cf, PIX_ERR_CAN_FRAME, CAN_FRAME_IS_NULL);
    }
    else
    {
        cf.set_frame(*can_frame);
        result = CAN_RES_NEXT_OK;
    }

    CANObject &co = *get_parent();
    CANManager &cm = *co.get_parent();

    cf.set_id(co.get_id());
    cf.set_function_id(get_id());
    cf.print("CANFunctionSimpleSender: ");

    cm.add_tx_queue_item(cf);

    return result;
}

/******************************************************************************************************************************
 *
 * CANFunctionSimpleEvent: class for events
 *
 ******************************************************************************************************************************/
const char *CANFunctionSimpleEvent::_name = "CANFunctionSimpleEvent";

CANFunctionSimpleEvent::CANFunctionSimpleEvent(CANObject *parent, uint32_t period_ms,
                                               CAN_function_handler_t external_handler,
                                               CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionTimerBase(CAN_FUNC_EVENT_ERROR, parent, period_ms, external_handler, next_ok_function, next_err_function)
{
    enable();
}

const char *CANFunctionSimpleEvent::get_name()
{
    return this->_name;
}

// decorator for CANFunctionTimerBase::_default_handler
// before timeout _last_action_tick update we should check if there any alarm
CAN_function_result_t CANFunctionSimpleEvent::_default_handler(CANFrame *can_frame)
{
    #warning Alarm refactoring should be done

    /* TODO: alarm refactoring should be done
    CANObject &can_object = *get_parent();
    
    if (!can_object.has_data_fields_alarm())
        return CAN_RES_FINAL;
    */
    return CAN_RES_FINAL; // TODO: temporary disable events, event will not spam with messages

    // return CANFunctionTimerBase::_default_handler(can_frame);
}

// virtual function for correct and systematic comparison of derived classes
// should be complement by derived class
CAN_function_result_t CANFunctionSimpleEvent::_timer_handler()
{
    CANObject &can_object = *get_parent();
    CANFrame can_frame;
    can_object.fill_can_frame_with_data(can_frame, get_id());

    can_frame.print("CANFunctionSimpleEvent: ");

    CANManager &can_manager = *can_object.get_parent();
    can_manager.add_tx_queue_item(can_frame);

    return CAN_RES_FINAL;
}

/******************************************************************************************************************************
 *
 * CANFunctionSet: class for setter
 *
 ******************************************************************************************************************************/
const char *CANFunctionSet::_name = "CANFunctionSet";

CANFunctionSet::CANFunctionSet(CANObject *parent, CAN_function_handler_t external_handler,
                               CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionBase(CAN_FUNC_SET_IN, parent, external_handler, next_ok_function, next_err_function)
{
    enable();
}

bool CANFunctionSet::is_responding_function()
{
    return true;
}

bool CANFunctionSet::is_automatic_function()
{
    return false;
}

const char *CANFunctionSet::get_name()
{
    return this->_name;
}

// should return CAN_RES_NEXT_OK for external handler call performing
CAN_function_result_t CANFunctionSet::_before_external_handler(CANFrame *can_frame)
{
    // if we are here then there is an external handler specified, all checks was performed in CANFunctionBase::process() method
    // But we should check can_frame anyway because of possibility of external handler incorrect work
    if (can_frame == nullptr)
        return CAN_RES_NEXT_ERR;

    can_frame->print("CANFunctionSet [incoming frame]: ");

    CANObject &co = *get_parent();
    if (co.get_data_fields_count() != 1)
    {
        // only objects with exactly one data field are writable
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_READONLY_OBJECT);
        return CAN_RES_NEXT_ERR;
    }

    DataField *df = co.get_data_field(0);

    if (can_frame->get_data_length() != df->get_data_byte_array_length() + 1)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        return CAN_RES_NEXT_ERR;
    }

    return CAN_RES_NEXT_OK;
}

// the return value of _after_external_handler() overwrites value, returned by the external handler
// if you don't want change it, just return the same value
CAN_function_result_t CANFunctionSet::_after_external_handler(CAN_function_result_t external_handler_result, CANFrame *can_frame)
{
    // can_frame was checked by _before_external_handler()... but external handler may delete it or do something another bad
    // in this case we shouldn't return external_handler_result
    if (can_frame == nullptr)
        return CAN_RES_NEXT_ERR;

    if (external_handler_result == CAN_RES_NEXT_OK)
    {
        CANObject &co = *get_parent();
        can_object_state_t co_state = co.update_state();

        if (co.is_state_ok())
        {
            co.fill_can_frame_with_data(*can_frame, get_id());
            return CAN_RES_NEXT_OK;
        }
        else
        {
            // something wrong with data update
            external_handler_result = CAN_RES_NEXT_ERR;
            _fill_error_can_frame(*can_frame, PIX_ERR_CAN_OBJECT, co_state);
        }
    }
    else if (external_handler_result == CAN_RES_NEXT_ERR)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_UNKNOWN_SETTER_ERROR);
    }

    return external_handler_result;
}

CAN_function_result_t CANFunctionSet::_default_handler(CANFrame *can_frame)
{
    // if the default handler is called then there is no external handler specified
    // this is an error situation
    if (can_frame != nullptr)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_NO_EXTERNAL_HANDLER);
    }
    return CAN_RES_NEXT_ERR;
}

/******************************************************************************************************************************
 *
 * CANFunctionSendRawBase: base function for all the family of the send raw functions
 *
 ******************************************************************************************************************************/
/*
CANFunctionSendRawBase::CANFunctionSendRawBase(CAN_function_id_t id, CANObject *parent, CAN_function_handler_t external_handler,
                                               CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionBase(id, parent, external_handler, next_ok_function, next_err_function)
{
}

bool CANFunctionSendRawBase::is_responding_function()
{
    return true;
}

bool CANFunctionSendRawBase::is_automatic_function()
{
    return false;
}

// validates common conditions of the functions family before _send_raw_handler() call
CAN_function_result_t CANFunctionSendRawBase::_default_handler(CANFrame *can_frame)
{
    if (can_frame == nullptr)
        return CAN_RES_NEXT_ERR;

    can_frame->print("CANFunctionSendRawBase [incoming frame]: ");

    if (this->is_suspended())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_FUNCTION_UNAVAILABLE);
        return CAN_RES_NEXT_ERR;
    }

    CANObject &co = *get_parent();
    can_object_state_t co_state = co.update_state();

    if (!co.is_state_ok())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_OBJECT, co_state);
        return CAN_RES_NEXT_ERR;
    }

    if (co.get_data_fields_count() != 1)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_READONLY_OBJECT);
        return CAN_RES_NEXT_ERR;
    }

    DataField *df = co.get_data_field(0);
    if (df == nullptr || df->get_source_type() != DF_RAW_DATA_ARRAY)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_OBJECT, COS_DATA_FIELD_ERROR);
        return CAN_RES_NEXT_ERR;
    }

    uint8_t func_counter = 0;
    CANFunctionBase *can_func = nullptr;

    // CANObject should have CAN_FUNC_SEND_RAW_INIT_IN
    can_func = co.get_function(CAN_FUNC_SEND_RAW_INIT_IN);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_INIT_OUT_OK
    can_func = co.get_function(CAN_FUNC_SEND_RAW_INIT_OUT_OK);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_INIT_OUT_ERR
    can_func = co.get_function(CAN_FUNC_SEND_RAW_INIT_OUT_ERR);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_START_IN
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_DATA_IN
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_END_IN
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_FINISH_IN
    can_func = co.get_function(CAN_FUNC_SEND_RAW_FINISH_IN);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_FINISH_OUT_OK
    can_func = co.get_function(CAN_FUNC_SEND_RAW_FINISH_OUT_OK);
    func_counter += (can_func == nullptr) ? 0 : 1;

    // CANObject should have CAN_FUNC_SEND_RAW_FINISH_OUT_ERR
    can_func = co.get_function(CAN_FUNC_SEND_RAW_FINISH_OUT_ERR);
    func_counter += (can_func == nullptr) ? 0 : 1;

    if (func_counter != 14 || !this->_functions_family_state_validator())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_MISSING_NECESSARY_FUNCTION);
        return CAN_RES_NEXT_ERR;
    }

    CAN_function_result_t send_raw_handler_result = this->_send_raw_handler(can_frame);
    this->_set_functions_family_states(send_raw_handler_result);
    return send_raw_handler_result;
}
*/
/******************************************************************************************************************************
 *
 * CANFunctionSendInit: function which starts send raw data sequence
 *
 ******************************************************************************************************************************/
/*
const char *CANFunctionSendInit::_name = "CANFunctionSendInit";

CANFunctionSendInit::CANFunctionSendInit(CANObject *parent, CAN_function_handler_t external_handler,
                                         CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionSendRawBase(CAN_FUNC_SEND_RAW_INIT_IN, parent, external_handler, next_ok_function, next_err_function)
{
    enable();

    if (parent == nullptr)
        return;

    CANObject &can_object = *parent;

    CANFunctionBase *cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_INIT_OUT_OK);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_INIT_OUT_OK);
    }
    set_next_ok_function(cfunc);

    cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_INIT_OUT_ERR);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_INIT_OUT_ERR);
    }
    set_next_err_function(cfunc);
}

const char *CANFunctionSendInit::get_name()
{
    return this->_name;
}

bool CANFunctionSendInit::_functions_family_state_validator()
{
    CANObject &co = *this->get_parent();

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN)->is_suspended())
        return false;

    co.get_function(CAN_FUNC_SEND_RAW_FINISH_IN)->enable();

    return true;
}

CAN_function_result_t CANFunctionSendInit::_send_raw_handler(CANFrame *can_frame)
{
    struct __attribute__((__packed__)) incoming_can_frame_data_struct_t
    {
        CAN_function_id_t func_id;
        uint8_t offering_chunk_size;
        uint32_t total_size;
        uint8_t file_code;
    };

    CANObject &co = *get_parent();
    DataFieldRawData *raw_data_field = (DataFieldRawData *)co.get_data_field(0);
    raw_data_field->cancel_all();

    can_frame->print("CANFunctionSendInit [incoming frame]: ");

    if (can_frame->get_data_length() != sizeof(incoming_can_frame_data_struct_t))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        return CAN_RES_NEXT_ERR;
    }

    incoming_can_frame_data_struct_t *can_frame_data = (incoming_can_frame_data_struct_t *)can_frame->get_data_pointer();

    if (can_frame_data->offering_chunk_size == 0 || can_frame_data->total_size < can_frame_data->offering_chunk_size)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_DATA_SIZE);
        return CAN_RES_NEXT_ERR;
    }

    if (!raw_data_field->has_free_space(can_frame_data->total_size))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_HAVE_NOT_FREE_SPACE);
        return CAN_RES_NEXT_ERR;
    }

    if (can_frame_data->file_code == 0)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_FILE_CODE);
        return CAN_RES_NEXT_ERR;
    }

    uint8_t final_chunk_size = (can_frame_data->offering_chunk_size > raw_data_field->get_data_byte_array_length()) ? raw_data_field->get_data_byte_array_length() : can_frame_data->offering_chunk_size;

    if (!raw_data_field->start_data_writing(can_frame_data->file_code, can_frame_data->total_size, final_chunk_size))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_WRITE_STARTING);
        return CAN_RES_NEXT_ERR;
    }

    // outgoing frame generation
    can_frame->clear_frame();
    can_frame->set_frame(co.get_id(), 2, 0, final_chunk_size);

    return CAN_RES_NEXT_OK;
}

void CANFunctionSendInit::_set_functions_family_states(CAN_function_result_t send_raw_handler_result)
{
    CANFunctionBase *can_func = nullptr;
    CANObject &co = *this->get_parent();

    // all functions exist, so we don't need to check if can_func == null

    // own state
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? this->suspend() : this->enable();

    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->enable() : can_func->suspend();

    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN);
    can_func->suspend();

    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN);
    can_func->suspend();
}
*/
/******************************************************************************************************************************
 *
 * CANFunctionChunkStart: it initiates the process of chunk receiving
 *
 ******************************************************************************************************************************/
/*
const char *CANFunctionChunkStart::_name = "CANFunctionChunkStart";

CANFunctionChunkStart::CANFunctionChunkStart(CANObject *parent, CAN_function_handler_t external_handler,
                                             CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionSendRawBase(CAN_FUNC_SEND_RAW_CHUNK_START_IN, parent, external_handler, next_ok_function, next_err_function)
{
    suspend(); // initially suspended, should be enabled by other functions

    if (parent == nullptr)
        return;

    CANObject &can_object = *parent;

    CANFunctionBase *cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_OK);
    }
    set_next_ok_function(cfunc);

    cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_CHUNK_START_OUT_ERR);
    }
    set_next_err_function(cfunc);
}

const char *CANFunctionChunkStart::get_name()
{
    return this->_name;
}

bool CANFunctionChunkStart::_functions_family_state_validator()
{
    CANObject &co = *this->get_parent();

    if (!co.get_function(CAN_FUNC_SEND_RAW_INIT_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN)->is_suspended())
        return false;

    co.get_function(CAN_FUNC_SEND_RAW_FINISH_IN)->enable();

    return true;
}

CAN_function_result_t CANFunctionChunkStart::_send_raw_handler(CANFrame *can_frame)
{
    struct __attribute__((__packed__)) incoming_can_frame_data_struct_t
    {
        CAN_function_id_t func_id;
        uint16_t chunk_index;
        uint16_t chunks_count;
        uint8_t chunk_size;
    };

    CANObject &co = *get_parent();
    DataFieldRawData *raw_data_field = (DataFieldRawData *)co.get_data_field(0);

    can_frame->print("CANFunctionChunkStart [incoming frame]: ");

    if (can_frame->get_data_length() != sizeof(incoming_can_frame_data_struct_t))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    incoming_can_frame_data_struct_t *can_frame_data = (incoming_can_frame_data_struct_t *)can_frame->get_data_pointer();

    if (can_frame_data->chunk_size > raw_data_field->get_data_byte_array_length())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_CHUNK_SIZE);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    // expected that DataField will do current_chunk_index++ on EndOfChunk function call
    if (can_frame_data->chunk_index != raw_data_field->get_current_chunk_index())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_INCORRECT_CHUNK_INDEX);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (raw_data_field->get_chunks_count() != can_frame_data->chunks_count)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_CHUNK_COUNT);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (!raw_data_field->start_new_chunk_accumulation(can_frame_data->chunk_index, can_frame_data->chunks_count, can_frame_data->chunk_size))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_START_NEW_CHUNK);
        return CAN_RES_NEXT_ERR;
    }

    can_frame->clear_frame();
    can_frame->set_frame(co.get_id(), 1, 0);

    return CAN_RES_NEXT_OK;
}

void CANFunctionChunkStart::_set_functions_family_states(CAN_function_result_t send_raw_handler_result)
{
    CANFunctionBase *can_func = nullptr;
    CANObject &co = *this->get_parent();

    // all functions exist, so we don't need to check if can_func == null

    can_func = co.get_function(CAN_FUNC_SEND_RAW_INIT_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->suspend() : can_func->enable();

    // own state
    this->suspend();

    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->enable() : can_func->suspend();

    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->enable() : can_func->suspend();
}
*/
/******************************************************************************************************************************
 *
 * CANFunctionChunkData: it receives small portion of chunk data (can frame with data)
 *
 ******************************************************************************************************************************/
/*
const char *CANFunctionChunkData::_name = "CANFunctionChunkData";

CANFunctionChunkData::CANFunctionChunkData(CANObject *parent, CAN_function_handler_t external_handler,
                                           CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionSendRawBase(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN, parent, external_handler, next_ok_function, next_err_function)
{
    suspend(); // initially suspended, should be enabled by other functions

    if (parent == nullptr)
        return;

    CANObject &can_object = *parent;

    CANFunctionBase *cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_OUT_ERR);
    }
    set_next_err_function(cfunc);
    set_next_ok_function(nullptr);
}

const char *CANFunctionChunkData::get_name()
{
    return this->_name;
}

bool CANFunctionChunkData::_functions_family_state_validator()
{
    CANObject &co = *this->get_parent();

    if (!co.get_function(CAN_FUNC_SEND_RAW_INIT_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN)->is_active())
        return false;

    co.get_function(CAN_FUNC_SEND_RAW_FINISH_IN)->enable();

    return true;
}

CAN_function_result_t CANFunctionChunkData::_send_raw_handler(CANFrame *can_frame)
{
    const uint8_t MAX_FRAME_DATA_LENGTH = 6;

    struct __attribute__((__packed__)) incoming_can_frame_data_struct_t
    {
        CAN_function_id_t func_id;
        uint8_t frame_index;
        uint8_t data[MAX_FRAME_DATA_LENGTH];
    };

    CANObject &co = *get_parent();
    DataFieldRawData *raw_data_field = (DataFieldRawData *)co.get_data_field(0);
    incoming_can_frame_data_struct_t *can_frame_data = (incoming_can_frame_data_struct_t *)can_frame->get_data_pointer();

    can_frame->print("CANFunctionChunkData [incoming frame]: ");

    if (!raw_data_field->is_waiting_for_frame())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_NOT_EXPECTED);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (!raw_data_field->is_waiting_for_missed_frame() && raw_data_field->get_expected_frame_index() > can_frame_data->frame_index)
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_NOT_EXPECTED);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (can_frame->get_data_length() < 3 || // 3 bytes is a minimal correct frame data length
        (can_frame_data->frame_index < raw_data_field->get_frames_count() - 1 && can_frame->get_data_length() != MAX_FRAME_DATA_LENGTH + 2))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    raw_data_field->process_new_frame(can_frame_data->frame_index, can_frame_data->data, can_frame->get_data_length() - 2);
    // raw_data_field->copy_data_from(can_frame_data->data, can_frame->get_data_length() - 2, can_frame_data->frame_index * MAX_FRAME_DATA_LENGTH);

    can_frame->clear_frame();
    // this function doesn't assume an OK-answer

    return CAN_RES_NEXT_OK;
}

void CANFunctionChunkData::_set_functions_family_states(CAN_function_result_t send_raw_handler_result)
{
    CANFunctionBase *can_func = nullptr;
    CANObject &co = *this->get_parent();

    // all functions exist, so we don't need to check if can_func == null

    can_func = co.get_function(CAN_FUNC_SEND_RAW_INIT_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->suspend() : can_func->enable();

    co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN)->suspend();

    // don't suspend till all data frames are received
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN);
    (send_raw_handler_result != CAN_RES_NEXT_OK) ? can_func->suspend() : can_func->enable();

    // the EndOfChunk function must also be enabled permanently during the data receiving
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN);
    (send_raw_handler_result != CAN_RES_NEXT_OK) ? can_func->suspend() : can_func->enable();
}
*/
/******************************************************************************************************************************
 *
 * CANFunctionChunkEnd: it finalizes the process of chunk receiving
 *
 ******************************************************************************************************************************/
/*
const char *CANFunctionChunkEnd::_name = "CANFunctionChunkEnd";

CANFunctionChunkEnd::CANFunctionChunkEnd(CANObject *parent, CAN_function_handler_t external_handler,
                                         CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionSendRawBase(CAN_FUNC_SEND_RAW_CHUNK_END_IN, parent, external_handler, next_ok_function, next_err_function)
{
    suspend(); // initially suspended, should be enabled by other functions

    if (parent == nullptr)
        return;

    CANObject &can_object = *parent;

    CANFunctionBase *cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK);
    }
    set_next_ok_function(cfunc);

    cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR);
    }
    set_next_err_function(cfunc);
}

const char *CANFunctionChunkEnd::get_name()
{
    return this->_name;
}

bool CANFunctionChunkEnd::_functions_family_state_validator()
{
    CANObject &co = *this->get_parent();

    if (!co.get_function(CAN_FUNC_SEND_RAW_INIT_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN)->is_suspended())
        return false;

    if (!co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN)->is_active())
        return false;

    co.get_function(CAN_FUNC_SEND_RAW_FINISH_IN)->enable();

    return true;
}

CAN_function_result_t CANFunctionChunkEnd::_send_raw_handler(CANFrame *can_frame)
{
    struct __attribute__((__packed__)) incoming_can_frame_data_struct_t
    {
        CAN_function_id_t func_id;
        uint16_t chunk_index;
        uint16_t chunks_count;
        uint8_t chunk_size;
    };

    struct __attribute__((__packed__)) ok_response_t
    {
        CAN_function_id_t func_id;
        uint16_t delay;
    };

    struct __attribute__((__packed__)) err_frames_missing_t
    {
        CAN_function_id_t func_id;
        uint8_t error_section;
        uint8_t num_of_broken_frames;
        uint8_t broken_frame_index;
    };

    CANObject &co = *get_parent();
    DataFieldRawData *raw_data_field = (DataFieldRawData *)co.get_data_field(0);

    can_frame->print("CANFunctionChunkEnd [incoming frame]: ");

    if (can_frame->get_data_length() != sizeof(incoming_can_frame_data_struct_t))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    incoming_can_frame_data_struct_t *can_frame_data = (incoming_can_frame_data_struct_t *)can_frame->get_data_pointer();

    if (can_frame_data->chunk_size > raw_data_field->get_data_byte_array_length())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_CHUNK_SIZE);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (can_frame_data->chunk_index != raw_data_field->get_current_chunk_index())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_INCORRECT_CHUNK_INDEX);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (!raw_data_field->close_chunk_accumulation(can_frame_data->chunk_size, can_frame_data->chunk_index, can_frame_data->chunks_count))
    {
        can_frame->clear_frame();
        can_frame->set_id(co.get_id());
        if (raw_data_field->is_waiting_for_missed_frame())
        {
            err_frames_missing_t *err_missed_frames = (err_frames_missing_t *)can_frame->get_data_pointer();
            // func id will be rewrited by sender
            // err_missed_frames->func_id = CAN_FUNC_SEND_RAW_CHUNK_END_OUT_ERR;
            err_missed_frames->num_of_broken_frames = raw_data_field->get_missed_frames_count();
            err_missed_frames->broken_frame_index = *raw_data_field->get_missed_frame_index();
            err_missed_frames->error_section = PIX_ERR_SEND_RAW_MISSED_FRAMES;
            can_frame->set_data_length(sizeof(err_frames_missing_t));
        }
        else
        {
            _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_CHUNK_SAVING);
            raw_data_field->cancel_all();
            return CAN_RES_NEXT_ERR;
        }
    }
    else
    {
        can_frame->clear_frame();
        can_frame->set_id(co.get_id());
        ok_response_t *ok_response = (ok_response_t *)can_frame->get_data_pointer();
        // func id will be rewrited by sender
        // ok_response->func_id = CAN_FUNC_SEND_RAW_CHUNK_END_OUT_OK;
        ok_response->delay = raw_data_field->get_writing_delay();
        can_frame->set_data_length(sizeof(ok_response_t));
    }

    return CAN_RES_NEXT_OK;
}

void CANFunctionChunkEnd::_set_functions_family_states(CAN_function_result_t send_raw_handler_result)
{
    CANFunctionBase *can_func = nullptr;
    CANObject &co = *this->get_parent();

    // all functions exist, so we don't need to check if can_func == null

    can_func = co.get_function(CAN_FUNC_SEND_RAW_INIT_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->suspend() : can_func->enable();

    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN);
    (send_raw_handler_result == CAN_RES_NEXT_OK) ? can_func->enable() : can_func->suspend();

    DataFieldRawData *raw_data_field = (DataFieldRawData *)co.get_data_field(0);
    can_func = co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN);
    if (send_raw_handler_result == CAN_RES_NEXT_OK)
    {
        can_func->suspend();
    }
    else
    {
        (raw_data_field->is_waiting_for_missed_frame()) ? can_func->enable() : can_func->suspend();
    }

    // own state
    co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN)->suspend();
}
*/
/******************************************************************************************************************************
 *
 * CANFunctionSendFinish: finalizes send raw data sequence
 *
 ******************************************************************************************************************************/
/*
const char *CANFunctionSendFinish::_name = "CANFunctionSendFinish";

CANFunctionSendFinish::CANFunctionSendFinish(CANObject *parent, CAN_function_handler_t external_handler,
                                             CANFunctionBase *next_ok_function, CANFunctionBase *next_err_function)
    : CANFunctionSendRawBase(CAN_FUNC_SEND_RAW_FINISH_IN, parent, external_handler, next_ok_function, next_err_function)
{
    enable(); // should be enabled always

    if (parent == nullptr)
        return;

    CANObject &can_object = *parent;

    CANFunctionBase *cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_FINISH_OUT_OK);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_FINISH_OUT_OK);
    }
    set_next_ok_function(cfunc);

    cfunc = can_object.get_function(CAN_FUNC_SEND_RAW_FINISH_OUT_ERR);
    if (cfunc == nullptr)
    {
        cfunc = can_object.add_function(CAN_FUNC_SEND_RAW_FINISH_OUT_ERR);
    }
    set_next_err_function(cfunc);
}

const char *CANFunctionSendFinish::get_name()
{
    return this->_name;
}

bool CANFunctionSendFinish::_functions_family_state_validator()
{
    // CAN_FUNC_SEND_RAW_FINISH_IN should work always
    this->enable();
    // state of other functions doesn't affect anithing

    return true;
}

CAN_function_result_t CANFunctionSendFinish::_send_raw_handler(CANFrame *can_frame)
{
    // incoming CAN frame data structure
    struct __attribute__((__packed__)) incoming_can_frame_data_struct_t
    {
        CAN_function_id_t func_id;
        uint32_t total_size;
        uint8_t file_code;
    };

    struct __attribute__((__packed__)) ok_response_t
    {
        CAN_function_id_t func_id;
    };

    CANObject &co = *get_parent();
    DataFieldRawData *raw_data_field = (DataFieldRawData *)co.get_data_field(0);

    can_frame->print("CANFunctionSendFinish [incoming frame]: ");

    if (can_frame->get_data_length() != sizeof(incoming_can_frame_data_struct_t))
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_CAN_FRAME, CAN_FRAME_SIZE_ERROR);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    incoming_can_frame_data_struct_t *can_frame_data = (incoming_can_frame_data_struct_t *)can_frame->get_data_pointer();

    if (can_frame_data->total_size != raw_data_field->get_expected_total_size())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_DATA_SIZE);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (can_frame_data->file_code != raw_data_field->get_file_code())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_FILE_CODE);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    if (!raw_data_field->complete_data_writing())
    {
        _fill_error_can_frame(*can_frame, PIX_ERR_FUNCTION, CAN_FUNC_ERROR_WRITE_FINISH);
        raw_data_field->cancel_all();
        return CAN_RES_NEXT_ERR;
    }

    can_frame->clear_frame();
    can_frame->set_id(co.get_id());
    can_frame->set_data_length(sizeof(ok_response_t));
    // func id will be rewrited by sender
    // can_frame->set_function_id(CAN_FUNC_SEND_RAW_FINISH_OUT_OK);

    return CAN_RES_NEXT_OK;
}

void CANFunctionSendFinish::_set_functions_family_states(CAN_function_result_t send_raw_handler_result)
{
    CANObject &co = *this->get_parent();

    // all functions exist, so we don't need to check if can_func == null

    co.get_function(CAN_FUNC_SEND_RAW_INIT_IN)->enable();

    co.get_function(CAN_FUNC_SEND_RAW_CHUNK_START_IN)->suspend();

    co.get_function(CAN_FUNC_SEND_RAW_CHUNK_DATA_IN)->suspend();

    co.get_function(CAN_FUNC_SEND_RAW_CHUNK_END_IN)->suspend();

    // own state - CAN_FUNC_SEND_RAW_FINISH_IN
    this->enable();
}
*/
