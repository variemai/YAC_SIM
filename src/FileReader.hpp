#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <string>
#include <vector>
#include "utilities.hpp"


struct Config {
    u32_t memsize = 0;
    u32_t cachesize = 0;
    u32_t wordsize = 0;
    u32_t linesize = 0;
    u32_t asso = 0;
};

class FileReader {
public:
    int loadConfig(const std::string& filename, Config& config);
    int loadTrace(const std::string& filename, std::vector<u32_t>& trace);
    void printConfig(const Config& config);
    void printTrace(const std::vector<u32_t> &trace);
    void parseConfig(Config& config);
    FileReader() = default;
};

#endif // FILEREADER_H_
