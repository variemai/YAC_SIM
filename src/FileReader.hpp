#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "utilities.hpp"


struct Config {
    u32_t memsize;
    u32_t cachesize;
    u32_t wordsize;
    u32_t linesize;
    u32_t associativity;
};

class FileReader {
public:
    Config loadConfig(const std::string& filename);
    int loadTrace(const std::string& filename, std::vector<u32_t>& trace);
    FileReader() = default;
};

#endif // FILEREADER_H_
