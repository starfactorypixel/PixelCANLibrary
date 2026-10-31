#pragma once
#include <inttypes.h>

class ICanStreamRaw
{
	public:
		
		// удобные обёртки: тип выводится из аргумента
		template <typename U> bool Get(uint8_t idx, U &val) const noexcept { return GetRaw(idx, &val, sizeof(U)); }
		template <typename U> bool Set(uint8_t idx, const U &val) noexcept { return SetRaw(idx, &val, sizeof(U)); }
		
	protected:
		
		~ICanStreamRaw() {}
		
		virtual bool GetRaw(uint8_t idx, void *data, uint8_t size) const noexcept = 0;
		virtual bool SetRaw(uint8_t idx, const void *data, uint8_t size) noexcept = 0;
};
