#ifndef __UTILS_H_
#define __UTILS_H_

#include <bitset>
#include <locale>
#include <limits>
#include <cmath>
#include <iostream>
bool isPowerOfTwo(unsigned long);

/*Returns the index of the first bit that is equal to 1 in the bitset*/
unsigned powerof2(std::bitset<32>);

/*Returns the exponent of a number that is power of 2*/
unsigned pow2(unsigned long);

unsigned long get_sizeof_memory(void);
unsigned long get_sizeof_word(void);
unsigned long get_sizeof_cache(void);
unsigned long get_sizeof_cacheline(void);
unsigned long get_associativity(void);
#endif // __UTILS_H_
