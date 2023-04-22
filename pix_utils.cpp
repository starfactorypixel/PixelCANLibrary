#include "pix_utils.h"

/*******************************************************************************************\
 *
 * Reverse array with temp variable. XOR-method is about 2 times slower.
 *
\*******************************************************************************************/
void reverse_array(uint8_t *array, uint8_t array_size)
{
    uint8_t temp = 0;
    uint8_t i = 0;
    uint8_t j = array_size - 1;

    while (i < j)
    {
        // swap with temporary variable (9 ms with very big array)
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
        
        // swap with XOR (15-16 ms with very big array)
        //array[i] = array[i] ^ array[j];
        //array[j] = array[i] ^ array[j];
        //array[i] = array[i] ^ array[j];

        i++;
        j--;
    }
}
