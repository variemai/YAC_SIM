#ifndef YACSIM_H_
#define YACSIM_H_

#include <vector>
#include <cstdint>

namespace yacsim {

// Represents the main memory specifications and provides basic access methods
class MainMemory {

    public:
        MainMemory(uint32_t memory_size, uint32_t word_size);

        uint32_t getMemorySize() const;
        uint32_t getWordSize() const;

    private:
        uint32_t memory_size;
        uint32_t word_size;
        std::vector<uint32_t> data;
};

// Stores the basic cache specifications provided by the user
struct CacheSpecs {
    uint32_t cache_size;
    uint32_t block_size;
    uint32_t memory_size;
    uint32_t associativity;

    CacheSpecs(uint32_t cache_size, uint32_t block_size,
               uint32_t memory_size, uint32_t associativity)
        : cache_size(cache_size), block_size(block_size),
          memory_size(memory_size), associativity(associativity) {}
};

// Holds the derived characteristics of the cache based on CacheSpecs
struct CacheCharacteristics {
    uint32_t tmp_tag;
    uint32_t tmp;
    uint32_t block_offset;
    uint32_t tag_shift;
    uint32_t tag_size;
    uint32_t index_size;
    uint32_t no_set;
    uint32_t associativity;

    CacheCharacteristics(const CacheSpecs& specs)
		: tmp_tag(specs.cache_size / (specs.block_size * specs.associativity)),
		tmp(tmp_tag* specs.block_size),
		block_offset(expOfPow2(specs.block_size)),
		tag_shift(expOfPow2(tmp_tag)),
		tag_size(32 - block_offset - tag_shift),
		index_size(expOfPow2(tmp_tag)),
		no_set(specs.cache_size / specs.block_size),
		associativity(specs.associativity) {}
};


// Represents a single cache entry with validity, tag, and LRU info
class CacheEntry {

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

        void accessEntry(uint32_t tag);
        uint32_t getTag(size_t index) const;

    private:
        std::vector<uint16_t> valid;
        std::vector<uint32_t> tag;
        std::vector<uint16_t> LRU;
};

// Represents the cache itself, integrating cache profiling
class Cache {

    public:
        Cache(const CacheSpecs& specs, const CacheCharacteristics& characteristics);

        // Cache access and profile management
        void access(uint32_t address);
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
        void applyLRUPolicy(uint32_t address);
};


} // namespace yacsim

#endif // YACSIM_H_
