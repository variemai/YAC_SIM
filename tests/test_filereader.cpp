#include "FileReader.hpp"
#include <iostream>

int main(int argc, char *argv[]){

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path> " << "<trace_file_path>\n";
        return 1;
    }

    std::string trace_file_path = argv[2];
    std::string config_file_path = argv[1];

    FileReader fr;
    Config config;

    fr.loadConfig(config_file_path, config);
    //print config
    fr.printConfig(config);
    std::vector<u32_t> trace;
    fr.loadTrace(trace_file_path, trace);
    //print trace using iterator
    fr.printTrace(trace);
    return 0;
}
