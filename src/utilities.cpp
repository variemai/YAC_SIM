#include "utilities.hpp"

bool isPowerOfTwo(u32_t n){
    return n > 0 && __builtin_popcount(n) == 1;
}

u32_t bitIndex1(const std::bitset<32>& bitset1) {

    for (u32_t i = 0; i < bitset1.size(); ++i) {

        if (bitset1[i]) return i; // Return the index of the first set bit
    }

    return 32; // If no bits are set, return an out-of-range index
}

/*Returns the exponent of a number that is power of 2 */
u32_t expOfPow2(u32_t num){
    return __builtin_ctz(num);
}
