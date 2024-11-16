#include "utilities.hpp"
#include <iostream>
#include <cassert>

int main() {
    assert(isPowerOfTwo(1) == true);
    assert(isPowerOfTwo(2) == true);
    assert(isPowerOfTwo(3) == false);
    assert(isPowerOfTwo(16) == true);
    assert(isPowerOfTwo(31) == false);
	std::cout << "All tests in main passed!\n";
	return 0;
}