#include "FileReader.hpp"
#include <iostream>

int main(int argc, char *argv[]){

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <trace_file_path>\n";
        return 1;
    }

    std::string trace_file_path = argv[1];

    FileReader fr;
    std::vector<u32_t> trace;
    fr.loadTrace(trace_file_path, trace);
    //print trace using iterator
    for(auto it = trace.begin(); it != trace.end(); it++){
        std::cout << *it << std::endl;
    }

    return 0;
}
