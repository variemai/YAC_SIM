#include "FileReader.hpp"
#include "utilities.hpp"
#include "yacsim.hpp"
#include <fstream>
#include <iostream>

// Implementation of the loadTrace function
// This function should be called in a while loop until the user provides a
// valid filename
int FileReader::loadTrace(const std::string &filename,
                          std::vector<u32_t> &trace) {
  int result = 1;
  std::ifstream infile(filename);

  if (infile.is_open()) {
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(infile, line)) {
      lineNumber++;
      try {
        u64_t value64 = std::stoull(line, nullptr, 10);
        if (!fitsin32(value64)) {
          std::cerr << "Error: Number on line " << lineNumber << "\n";
          infile.close();
          return -1;
        }

        trace.push_back(static_cast<u32_t>(value64));
      } catch (const std::invalid_argument &) {
        std::cerr << "Error: Invalid number format on line " << lineNumber
                  << ": '" << line << "'\n";
        infile.close();
        return -1;
      } catch (const std::out_of_range &) {
        std::cerr << "Error: Number on line " << lineNumber
                  << " is too large: '" << line << "'\n";
        infile.close();
        return -1;
      }
    }

    infile.close();
    result = 0;
  } else {
    std::cerr << "Error: Could not open file " << filename << ", try again\n";
    result = -1;
  }

  return result;
}

// Implementation of the loadConfig function

int FileReader::loadConfig(const std::string &filename, Config &config) {
  int result = 1;
  std::ifstream infile(filename);

  if (infile.is_open()) {
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(infile, line)) {
      lineNumber++;
      try {
        // Convert to unsigned long long first to handle all cases
        std::size_t pos = line.find(" ");
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        u64_t value64 = std::stoull(value, nullptr, 10);
        if (!fitsin32(value64)) {
          std::cerr << "Error: Number on line " << lineNumber << "\n";
          infile.close();
          return -1;
        }

        if (key == "memsize") {
          config.memsize = static_cast<u32_t>(value64);
        } else if (key == "wordsize") {
          config.wordsize = static_cast<u32_t>(value64);
        }

        else if (key == "cachesize") {
          config.cachesize = static_cast<u32_t>(value64);
        } else if (key == "blocksize" || key == "linesize") {
          config.linesize = static_cast<u32_t>(value64);
        } else if (key == "asso") {
          config.asso = static_cast<u32_t>(value64);
        } else {
          std::cerr << "Error: Invalid key on line " << lineNumber << ": '"
                    << key << "'\n";
          infile.close();
          return -1;
        }

      } catch (const std::invalid_argument &) {
        std::cerr << "Error: Invalid number format on line " << lineNumber
                  << ": '" << line << "'\n";
        infile.close();
        return -1;
      } catch (const std::out_of_range &) {
        std::cerr << "Error: Number on line " << lineNumber
                  << " is too large: '" << line << "'\n";
        infile.close();
        return -1;
      }
    }

    infile.close();
    result = 0;
  } else {
    std::cerr << "Error: Could not open file " << filename << ", try again\n";
    result = -1;
  }

  return result;
}
// Parses the config and asks the user to assign missing values
bool parseConfig(Config &config) {
  bool flag = false;


  if (config.memsize == 0) {
    std::cout << "Memory size not found or invalid\nEnter memory size (in bytes): ";
    std::cin >> config.memsize;
    flag = true;
  }
  if (config.wordsize == 0) {
    std::cout << "Word size not found or invalid\nEnter word size (in bytes): ";
    std::cin >> config.wordsize;
    flag = true;
  }
  if (config.cachesize == 0) {
    std::cout << "Cache size not found or invalid\nEnter cache size (in bytes): ";
    std::cin >> config.cachesize;
    flag = true;
  }
  if (config.linesize == 0) {
    std::cout << "Cache line (block) size not found or invalid\nEnter cache line (block) size (in bytes): ";
    std::cin >> config.linesize;
    flag = true;
  }
  if (config.asso == 0) {
    std::cout << "Associativity not found or invalid\nEnter associativity (1 is for direct mapped): ";
    std::cin >> config.asso;
    flag = true;
  }
  return flag;
}

void printConfig(const Config &config) {
  std::cout << "Cache Configuration:\n";
  std::cout << "Memory size: " << config.memsize << "\n";
  std::cout << "Word size: " << config.wordsize << "\n";
  std::cout << "Cache size: " << config.cachesize << "\n";
  std::cout << "Cache Line (Block) size: " << config.linesize << "\n";
  std::cout << "Associativity: " << config.asso << "\n";
}

void printTrace(const std::vector<u32_t> &trace) {
  std::cout << "Trace:\n";
  for (auto it = trace.begin(); it != trace.end(); it++) {
    std::cout << *it << std::endl;
  }
}
