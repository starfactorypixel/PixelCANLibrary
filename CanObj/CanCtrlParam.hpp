#pragma once
#include <inttypes.h>
#include <CanObjectBase.h>

// Данный объект реализует управляющий параметр, set, toggle, action, request, event

template <typename T> 
class CanCtrlParam : public CANObjectBase
{
	//using T = uint16_t;
	struct __attribute__((packed)) request_t { uint8_t fId; };
	struct __attribute__((packed)) set_t { uint8_t fId; T val; };
	struct __attribute__((packed)) toggle_t { uint8_t fId; };
	struct __attribute__((packed)) action_t { uint8_t fId; };
	struct __attribute__((packed)) event_ok_t { uint8_t fId = CAN_FUNC_EVENT_OK; T val; };
	
	using function_ctrl_t = void (*)(uint8_t fId, T value);
	using function_req_t = T (*)();
	
	public:
		CanCtrlParam(can_object_id_t id, function_ctrl_t ctrl, function_req_t request) : CANObjectBase(id), _FuncControl(ctrl), _FuncRequest(request)
		{
			return;
		}

		void EventOk()
		{
			event_ok_t answer = {};
			answer.val = _FuncRequest();
			this->SendFrame((uint8_t *)&answer, sizeof(answer));
			
			return;
		}
		
	protected:
		virtual void OnTick(uint32_t time) noexcept override
		{
			return;
		}

		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			uint8_t fId = can_frame.raw_data[0];
			switch(fId)
			{
				case CAN_FUNC_SET_IN:
				{
					set_t *obj = (set_t *)can_frame.raw_data;
					_FuncControl(fId, obj->val);
					
					break;
				}
				case CAN_FUNC_TOGGLE_IN:
				{
					//toggle_t *obj = (toggle_t *)can_frame.raw_data;
					_FuncControl(fId, 0);
					
					break;
				}
				case CAN_FUNC_ACTION_IN:
				{
					//action_t *obj = (action_t *)can_frame.raw_data;
					_FuncControl(fId, 0);
					
					break;
				}
				case CAN_FUNC_REQUEST_IN:
				{
					//request_t *obj = (request_t *)can_frame.raw_data;
					event_ok_t answer = {};
					answer.val = _FuncRequest();
					this->SendFrame((uint8_t *)&answer, sizeof(answer));
					
					break;
				}
			}
			
			return;
		}
		
		virtual void OnTimer() noexcept override
		{
			return;
		}

	private:
		function_ctrl_t _FuncControl;
		function_req_t _FuncRequest;
};
