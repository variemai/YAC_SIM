#include "utilities.hpp"
#include "yacsim.hpp"
#include <iostream>
#include <cassert>
#include <bitset>

namespace yacsim {

	// Default constructor for MainMemory


    MainMemory::MainMemory(u32_t memory_size, u32_t word_size)
        : memory_size(memory_size), word_size(word_size), data(memory_size / word_size, 0) {
        // Ensure memory_size is a multiple of word_size
        if (memory_size % word_size != 0) {
            std::cerr << "Error: Memory size must be a multiple of word size\n";
            exit(1);
        }
    }

    u32_t MainMemory::getMemorySize() const {
		return memory_size;
    }

	u32_t MainMemory::getWordSize() const {
		return word_size;
	}

	// Constructor for Cache Characteristics
	CacheCharacteristics::CacheCharacteristics(const CacheSpecs& specs, const u32_t memory_size){

        std::bitset<32> index_mask(~0u); // All bits set

        // Calculate block offset (log2(block_size))
        block_offset = static_cast<u32_t>(std::log2(specs.block_size));

        // Calculate index size (log2(cache_size / (block_size * associativity)))
        index_size = static_cast<u32_t>(std::log2(specs.cache_size)) -
            static_cast<u32_t>(std::log2(specs.block_size)) -
            static_cast<u32_t>(std::log2(specs.associativity));

        // Adjust index mask for block offset
        index_mask <<= index_size;
        tmp = index_mask.to_ulong();  // Store index mask

        // Create tag mask by inverting index mask
        tmp_tag = ~index_mask.to_ulong();

        // Calculate tag shift (number of bits for block offset + index size)
        tag_shift = block_offset + index_size;

        // Calculate tag size
        tag_size = 32 - block_offset - index_size;

        // Calculate number of sets
        no_set = specs.cache_size / (specs.block_size * specs.associativity);

        // Store associativity
        associativity = specs.associativity;
	}


	void CacheCharacteristics::printCharacteristics() const {
		std::cout << "Cache Characteristics:\n";
		std::cout << "Block Offset: " << block_offset << '\n';
		std::cout << "Index Size: " << index_size << '\n';
		std::cout << "Tag Shift: " << tag_shift << '\n';
		std::cout << "Tag Size: " << tag_size << '\n';
		std::cout << "Number of Sets: " << no_set << '\n';
		std::cout << "Associativity: " << associativity << '\n';
	}

    Cache::Cache(const CacheSpecs& _specs, const CacheCharacteristics& _characteristics) {
		characteristics = _characteristics;
		specs = _specs;
		entries = std::vector<CacheEntry>(specs.cache_size / specs.block_size, CacheEntry(specs.associativity));
    }

    u32_t Cache::getHits() const {
        return hits;
    }

	u32_t Cache::getMisses() const {
		return misses;
	}



} // namespace yacsim