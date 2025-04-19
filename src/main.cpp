#include "utilities.hpp"
#include "FileReader.hpp"
#include <iostream>
#include <cassert>

int main(int argc, char* argv[]) {

    FileReader filereader;
    Config config;
    filereader.loadConfig(argv[1], config);
    filereader.printConfig(config);
    // Check if something is missing from the config and ask the user to fill it


	return 0;
}
