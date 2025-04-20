#include "utilities.hpp"
#include "FileReader.hpp"
#include <iostream>
#include <cassert>

int main(int argc, char* argv[]) {

    FileReader filereader;
    Config config;
    filereader.loadConfig(argv[1], config);
    bool invalid_config = filereader.parseConfig(config);
    // Check if something is missing from the config and ask the user to fill it
    while (invalid_config) {
        invalid_config = filereader.parseConfig(config);
    }
    std::cout << "Loaded the Cache Configuration successfully\n";
    filereader.printConfig(config);

	return 0;
}
