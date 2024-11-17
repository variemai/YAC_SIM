#ifndef YACSIM_H_
#define YACSIM_H_

#include <vector>
#include <cstdint>
#include "utilities.hpp"

namespace yacsim {

// Represents the main memory specifications and provides basic access methods
class MainMemory {

    public:
        MainMemory(u32_t memory_size, u32_t word_size);

        u32_t getMemorySize() const;
        u32_t getWordSize() const;

    private:
        u32_t memory_size;
        u32_t word_size;
        std::vector<u32_t> data;
};

// Stores the basic cache specifications provided by the user
struct CacheSpecs {
    u32_t cache_size;
    u32_t block_size;
    u32_t associativity;

	// Default constructor
    CacheSpecs() = default;
    
	CacheSpecs(u32_t _cache_size, u32_t _block_size, u32_t _associativity)
		: cache_size(_cache_size), block_size(_block_size), associativity(_associativity) {
	}
};

// Holds the derived characteristics of the cache based on CacheSpecs
struct CacheCharacteristics {
    u32_t tmp_tag;
    u32_t tmp;
    u32_t block_offset;
    u32_t tag_shift;
    u32_t tag_size;
    u32_t index_size;
    u32_t no_set;
    u32_t associativity;

    CacheCharacteristics() = default;
    CacheCharacteristics(const CacheSpecs& specs, const u32_t memory_size);

	void printCharacteristics() const;
};


// Represents a single cache entry with validity, tag, and LRU info
struct CacheEntry {

	std::vector<u16_t> valid;
	std::vector<u32_t> tag;
	std::vector<u32_t> LRU;

    // Constructor: Initializes all fields to 0
    CacheEntry(u32_t associativity)
        : valid(associativity, 0), // Initialize all elements in 'valid' to 0
        tag(associativity, 0),  // Initialize all elements in 'tag' to 0
        LRU(associativity, 0)   // Initialize all elements in 'LRU' to 0
    {
    }
};

// Represents the cache itself, integrating cache profiling
class Cache {

    public:
        Cache(const CacheSpecs& _specs, const CacheCharacteristics& _characteristics);

        // Cache access and profile management
        void access(u32_t address);
        void resetProfile();
        u32_t getHits() const;
        u32_t getMisses() const;

    private:
        CacheSpecs specs;
        CacheCharacteristics characteristics;
        std::vector<CacheEntry> entries;

        u32_t check = 0;
        u32_t hits = 0;
        u32_t misses = 0;

        // Internal method for applying the LRU policy, etc.
        void applyLRUPolicy(u32_t address);
};


} // namespace yacsim

#endif // YACSIM_H_
