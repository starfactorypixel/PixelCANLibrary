#pragma once
#include <inttypes.h>

class ICanStream
{
	public:
		
		// удобные обёртки: тип выводится из аргумента
		template <typename U> bool Get(uint8_t idx, U &val) const noexcept { return GetRaw(idx, &val, sizeof(U)); }
		template <typename U> bool Set(uint8_t idx, const U &val) noexcept { return SetRaw(idx, &val, sizeof(U)); }
		template <typename U> bool GetMin(U &val) const noexcept { return GetMinRaw(&val, sizeof(U)); }
		template <typename U> bool GetMax(U &val) const noexcept { return GetMaxRaw(&val, sizeof(U)); }
		
	protected:
		
		~ICanStream() {}
		
		virtual bool GetRaw(uint8_t idx, void *data, uint8_t size) const noexcept = 0;
		virtual bool SetRaw(uint8_t idx, const void *data, uint8_t size) noexcept = 0;
		virtual bool GetMinRaw(void *data, uint8_t size) const noexcept = 0;
		virtual bool GetMaxRaw(void *data, uint8_t size) const noexcept = 0;
};
