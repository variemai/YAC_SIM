#include "utilities.hpp"
#include "yacsim.hpp"
#include <iostream>
#include <cassert>

// Constructor for MainMemory
yacsim::MainMemory::MainMemory(uint32_t memory_size, uint32_t word_size)
    : memory_size(memory_size), word_size(word_size), data(memory_size / word_size, 0) {
    // Ensure memory_size is a multiple of word_size
    if (memory_size % word_size != 0) {
        std::cerr << "Error: Memory size must be a multiple of word size\n";
		exit(1);
    }
}

// Constructor for CacheEntry