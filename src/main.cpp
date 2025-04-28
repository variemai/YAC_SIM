#include "FileReader.hpp"
#include <cassert>
#include <iostream>

int main(int argc, char *argv[]) {

  FileReader filereader;
  Config config;
  if (argc < 2) {
    bool invalid_config = parseConfig(config);
    while (invalid_config) {
      invalid_config = parseConfig(config);
    }
    std::cout << "-------------------------------------------\n";
    printConfig(config);
  } else {
    filereader.loadConfig(argv[1], config);
    bool invalid_config = parseConfig(config);
    // Check if something is missing from the config and ask the user to fill it
    while (invalid_config) {
      invalid_config = parseConfig(config);
    }
    std::cout << "Loaded the Cache Configuration successfully\n";
    std::cout << "-------------------------------------------\n";
    printConfig(config);
  }

  return 0;
}
