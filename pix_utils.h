#ifndef PIX_UTILS_H
#define PIX_UTILS_H

#include <stdint.h>

/*******************************************************************************************\
 *
 * Big Endian → Little Endian → Big Endian functions
 *
\*******************************************************************************************/
// swaps uint16 endian
inline void swap_endian(uint16_t &val)
{
    val = (val << 8) | (val >> 8);
}

// swaps int16 endian
inline void swap_endian(int16_t &val)
{
    swap_endian(*(uint16_t *)&val);
}

// swaps uint32 endian
inline void swap_endian(uint32_t &val)
{
    val = (val << 24) | ((val << 8) & 0x00ff0000) |
          ((val >> 8) & 0x0000ff00) | (val >> 24);
}

// swaps int32 endian
inline void swap_endian(int32_t &val)
{
    swap_endian(*(uint32_t *)&val);
}

/*******************************************************************************************\
 *
 * Reverse array with temp variable. XOR-method is about 2 times slower.
 *
\*******************************************************************************************/
void reverse_array(uint8_t *array, uint8_t array_size);

#endif // PIX_UTILS_H