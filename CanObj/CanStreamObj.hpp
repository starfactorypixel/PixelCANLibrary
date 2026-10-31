#pragma once
#include <inttypes.h>
#include <string.h>
#include <CanObjectBase.h>
#include "ICanStreamRaw.hpp"

template <typename T, uint8_t _count> 
class CanStreamObj : public CANObjectBase, public ICanStreamRaw
{
	//using T = int8_t; static constexpr uint8_t _count = 16;
	struct __attribute__((packed)) request_t { uint8_t fId; };
	struct __attribute__((packed)) stream_t { uint8_t fId = 0x65; uint8_t num; T val; };
	
	static_assert(sizeof(stream_t) <= 8, "stream_t does not fit in CAN frame");
	
	public:

		CanStreamObj(can_object_id_t id) : CANObjectBase(id)
		{

		};

		void SetInterval(uint16_t interval)
		{
			_interval = interval;
		}

	protected:
		
		virtual bool GetRaw(uint8_t idx, void *data, uint8_t size) const noexcept override
		{
			if(size != sizeof(T)) return false;
			if(idx >= _count) return false;
			
			memcpy(data, &_collection[idx], sizeof(T));
			return true;
		}
		
		virtual bool SetRaw(uint8_t idx, const void *data, uint8_t size) noexcept override
		{
			if(size != sizeof(T)) return false;
			if(idx >= _count) return false;
			
			memcpy(&_collection[idx], data, sizeof(T));
			return true;
		}
		
		virtual void OnTick(uint32_t time) noexcept override
		{
			if(_is_run == false) return;
			
			if((time - _last_tick) < _interval) return;
			_last_tick = time;
			
			_SendStreamElement(_send_idx);
			
			if(++_send_idx >= _count)
			{
				_is_run = false;
			}

			return;
		}
		
		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			uint8_t fId = can_frame.raw_data[0];
			switch(fId)
			{
				case CAN_FUNC_REQUEST_IN:
				{
					//request_t *obj = (request_t *)can_frame.raw_data;

					_last_tick = 0;
					_send_idx = 0;
					_is_run = true;
					
					break;
				}
			}

			return;
		}
		
	private:
		
		void _SendStreamElement(uint8_t idx)
		{
			stream_t answer = {};
			answer.num = idx + 1;
			answer.val = _collection[idx];
			this->SendFrame((uint8_t *)&answer, sizeof(answer));

			return;
		}
		
		T _collection[_count] = {};
		uint32_t _last_tick = 0;
		uint16_t _interval = 100;
		uint8_t _send_idx = 0;
		bool _is_run = false;
};
