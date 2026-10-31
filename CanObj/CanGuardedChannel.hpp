#pragma once
#include <inttypes.h>
#include <CanObjectBase.h>

/*
 *	Канал критичных команд с контролем связи.
 *
 *	- Применяются только пакеты со счётчиком +1 или +2 от последнего применённого.
 *	- Счётчик в пределах ±3 (дубликат, старый пакет, +3) - не авария, но данные не применяются.
 *	- Счётчик за пределами ±3 - потеря синхронизации -> failsafe.
 *	- Валидных пакетов нет дольше TIMEOUT_MS - failsafe.
 *	- После failsafe (и при первом подключении) данные применяются только после
 *	  SYNC_PACKETS валидных пакетов подряд.
 *	- До первого подключения никакие callback'и не вызываются.
 *
 *	Для стендовых тестов: SetBypass(true). Любой вызов SetBypass в коде даёт warning при сборке.
 */

template <typename T, uint16_t TIMEOUT_MS = 300, uint8_t SYNC_PACKETS = 3> 
class CanGuardedChannel : public CANObjectBase
{
	struct __attribute__((packed)) set_real_time_t { uint8_t fId; uint8_t counter; T val; };
	
	static_assert(sizeof(set_real_time_t) <= 8, "set_real_time_t does not fit into CAN frame");
	static_assert(SYNC_PACKETS >= 1, "SYNC_PACKETS must be >= 1");
	
	using function_ctrl_t = void (*)(uint8_t idx, T value);
	using function_failsafe_t = void (*)(uint8_t idx);

	static constexpr int8_t COUNTER_STEP_MAX = 2;	// Применяем при +1..+2
	static constexpr int8_t COUNTER_WINDOW = 3;		// В пределах ±3 - не авария, но и не применяем

	public:
		
		CanGuardedChannel(can_object_id_t id, uint8_t idx, function_ctrl_t ctrl, function_failsafe_t failsafe) : CANObjectBase(id), _idx(idx), _Ctrl(ctrl), _Failsafe(failsafe)
		{

		}

		bool IsConnected() const
		{
			return _connected;
		}

		// Отключение всех проверок - только для стенда!
		__attribute__((deprecated("SAFETY BYPASS: CanGuardedChannel::SetBypass() is called. NOT FOR PRODUCTION!")))
		void SetBypass(bool bypass)
		{
			_bypass = bypass;

			return;
		}
		
	protected:
		
		void handlerSetRealTimeFunction(set_real_time_t *obj)
		{
			uint32_t time = GetParent()->GetTime();

			if(_bypass == true)
			{
				// Применяем всё подряд, но состояние ведём, чтобы после выключения bypass сразу работал таймаут
				_has_ref = true;
				_connected = true;
				_last_counter = obj->counter;
				_last_time = time;

				_Ctrl(_idx, obj->val);
				return;
			}

			bool time_ok = (time - _last_time) <= TIMEOUT_MS;

			// Таймаут мог наступить раньше, чем его заметил OnTick
			if(_connected && time_ok == false)
			{
				_EnterFailsafe();
			}

			// Нет опорного пакета или он устарел - берём этот за опорный, данные не применяем
			if(_has_ref == false || time_ok == false)
			{
				_SetReference(obj->counter, time);
				return;
			}

			int8_t diff = (int8_t)(obj->counter - _last_counter);

			if(diff >= 1 && diff <= COUNTER_STEP_MAX)
			{
				// Валидный пакет
				_last_counter = obj->counter;
				_last_time = time;

				if(_connected == false)
				{
					if(++_sync_count < SYNC_PACKETS) return;

					_connected = true;
				}

				_Ctrl(_idx, obj->val);
				return;
			}

			if(diff > COUNTER_STEP_MAX && diff <= COUNTER_WINDOW)
			{
				// Пропущено больше допустимого: сдвигаем опору, но не применяем и время не продлеваем.
				// Применение - только когда придёт строго +1/+2 от этого пакета.
				_last_counter = obj->counter;
				return;
			}

			if(diff >= -COUNTER_WINDOW && diff <= 0)
			{
				// Дубликат или старый пакет: игнорируем. Время не продлеваем - если валидных
				// так и не будет, сработает таймаут.
				return;
			}

			// Счётчик вне окна - потеряли синхронизацию
			if(_connected) _EnterFailsafe();
			_SetReference(obj->counter, time);

			return;
		}

		virtual void OnTick(uint32_t time) noexcept override
		{
			if(_bypass == true) return;

			if(_connected && (time - _last_time > TIMEOUT_MS))
			{
				// Таймаут - валидные пакеты перестали приходить
				_EnterFailsafe();
			}

			return;
		}

		virtual void OnProcessFrame(can_frame_t &can_frame) noexcept override
		{
			uint8_t fId = can_frame.raw_data[0];
			switch(fId)
			{
				case CAN_FUNC_SET_REAL_TIME_IN:
				{
					if(can_frame.raw_data_length != sizeof(set_real_time_t)) break;
					
					set_real_time_t *obj = (set_real_time_t *)can_frame.raw_data;
					handlerSetRealTimeFunction(obj);
					
					break;
				}
			}

			return;
		}

	private:
		
		void _EnterFailsafe()
		{
			_connected = false;
			_has_ref = false;
			_sync_count = 0;

			_Failsafe(_idx);

			return;
		}

		void _SetReference(uint8_t counter, uint32_t time)
		{
			_has_ref = true;
			_sync_count = 0;
			_last_counter = counter;
			_last_time = time;

			return;
		}

		uint8_t _idx;
		function_ctrl_t _Ctrl;
		function_failsafe_t _Failsafe;

		bool _connected = false;
		bool _has_ref = false;
		uint8_t _sync_count = 0;
		uint8_t _last_counter = 0;
		uint32_t _last_time = 0;
		bool _bypass = false;
};
