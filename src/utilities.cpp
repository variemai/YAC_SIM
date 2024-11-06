#include "utilities.hpp"

bool isPowerOfTwo(u32_t n)
{
#ifdef HAVE_BUILTIN_POPCOUNT
    return n > 0 && __builtin_popcount(n) == 1;
#else
    // Fallback implementation without intrinsics
    return n > 0 && (n & (n - 1)) == 0;
#endif
}

u32_t bitIndex1(const std::bitset<32>& bitset1)
{
    for (u32_t i = 0; i < bitset1.size(); ++i) {

        if (bitset1[i]) return i; // Return the index of the first set bit
    }

    return 32; // If no bits are set, return an out-of-range index
}

/*Returns the exponent of a number that is power of 2 */
u32_t expOfPow2(u32_t num)
{
#ifdef HAVE_BUILTIN_CTZ
    return __builtin_ctz(num);
#else
    // Fallback implementation without intrinsics
    u32_t exponent = 0;
    while (num > 1) {
        num >>= 1;
        ++exponent;
    }
    return exponent;
#endif
}
