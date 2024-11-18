#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <cstdint>
#include <bitset>

#define u32_t uint32_t
#define u64_t uint64_t
#define u16_t uint16_t

/*Returns true if the number is a power of 2*/
bool isPowerOfTwo(u32_t);

/*Returns the index of the first bit that is equal to 1 in the bitset*/
u32_t bitIndex1(const std::bitset<32>& );

/*Returns the exponent of a number that is power of 2*/
u32_t expOfPow2(u32_t);

// Function to check if a number fits in a u32_t
bool fitsin32(u64_t);


#endif // UTILITIES_H_
