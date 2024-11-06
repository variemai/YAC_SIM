#include <utilities.hpp>
#include <iostream>
#include <cassert>

void test_isPowerOfTwo() {
    assert(isPowerOfTwo(1) == true);
    assert(isPowerOfTwo(2) == true);
    assert(isPowerOfTwo(3) == false);
    assert(isPowerOfTwo(16) == true);
    assert(isPowerOfTwo(31) == false);
    std::cout << "test_isPowerOfTwo passed\n";
}

void test_bitIndex1() {
    std::bitset<32> test1("00000000000000000000000000001000"); // First set bit at index 3
    std::bitset<32> test2("00000000000000000000000000000001"); // First set bit at index 0
    std::bitset<32> test3("00000000000000000000000000000000"); // No set bits

    assert(bitIndex1(test1) == 3);
    assert(bitIndex1(test2) == 0);
    assert(bitIndex1(test3) == 32);
    std::cout << "test_bitIndex1 passed\n";
}

void test_expOfPow2() {
    assert(expOfPow2(1) == 0);
    assert(expOfPow2(2) == 1);
    assert(expOfPow2(4) == 2);
    assert(expOfPow2(16) == 4);
    std::cout << "test_expOfPow2 passed\n";
}

int main() {
    test_isPowerOfTwo();
    test_bitIndex1();
    test_expOfPow2();
    std::cout << "All tests passed!\n";
    return 0;
}
