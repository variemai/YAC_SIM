#include "yacsim.hpp"

int main() {
	yacsim::MainMemory main_memory(1024, 4);
	yacsim::CacheSpecs cache_specs(128, 32, 1);
	yacsim::CacheCharacteristics cache_char(cache_specs, main_memory.getMemorySize());
	cache_char.printCharacteristics();
	//yacsim::CacheEntry cache_entry(4);
	return 0;
}
