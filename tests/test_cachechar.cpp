#include "yacsim.hpp"
#include "utilities.hpp"
#include <iostream>

int main() {
	yacsim::MainMemory main_memory(1024, 4);
	yacsim::CacheSpecs cache_specs(128, 32, 1); // Use brace-enclosed initializer list
	yacsim::CacheCharacteristics cache_char(cache_specs, main_memory.getMemorySize());
	cache_char.printCharacteristics();
	//yacsim::CacheEntry cache_entry(4);
	yacsim::Cache cache(cache_specs, cache_char);
	std::cout << "Cache hits = " << cache.getHits() << '\n';
	std::cout << "Cache misses = " << cache.getMisses() << '\n';
	return 0;
}
