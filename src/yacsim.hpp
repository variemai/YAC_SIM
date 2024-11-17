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

    CacheSpecs(u32_t cache_size, u32_t block_size, u32_t associativity)
        : cache_size(cache_size), block_size(block_size), 
        associativity(associativity) {}
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

    CacheCharacteristics(const CacheSpecs& specs, const u32_t memory_size);

	void printCharacteristics() const;
};


// Represents a single cache entry with validity, tag, and LRU info
struct CacheEntry {

	std::vector<uint16_t> valid;

    public:
        // Constructor: IMPORTANT: initialize everything with 0
        CacheEntry(size_t associativity);

        // Copy constructor: allows creating a new CacheEntry as a copy of an existing one
        CacheEntry(const CacheEntry &) = default;

        // Move constructor: allows transferring resources from a temporary CacheEntry to this one
        CacheEntry(CacheEntry &&) = default;

        // Copy assignment operator: enables assigning one CacheEntry to another
        CacheEntry &operator=(const CacheEntry &) = default;

        // Move assignment operator: enables moving resources from a temporary CacheEntry to this one
        CacheEntry &operator=(CacheEntry &&) = default;

        void accessEntry(u32_t tag);
        u32_t getTag(size_t index) const;

    private:
        std::vector<uint16_t> valid;
        std::vector<u32_t> tag;
        std::vector<uint16_t> LRU;
};

// Represents the cache itself, integrating cache profiling
class Cache {

    public:
        Cache(const CacheSpecs& specs, const CacheCharacteristics& characteristics);

        // Cache access and profile management
        void access(u32_t address);
        void resetProfile();
        int getHits() const;
        int getMisses() const;

    private:
        CacheSpecs specs;
        CacheCharacteristics characteristics;
        std::vector<CacheEntry> entries;

        int check = 0;
        int hit = 0;
        int miss = 0;

        // Internal method for applying the LRU policy, etc.
        void applyLRUPolicy(u32_t address);
};


} // namespace yacsim

#endif // YACSIM_H_
