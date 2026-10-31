#pragma once
#include "CanStream.hpp"

template <typename T, uint8_t _count> 
class CanStreamObj : public CanStream<T>
{
	static_assert(_count > 0, "collection size must be > 0");
	
	public:

		CanStreamObj(can_object_id_t id) : CanStream<T>(id, _storage, _count)
		{

		};

	private:
		
		T _storage[_count] = {};
};
