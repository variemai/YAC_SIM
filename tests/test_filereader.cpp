#include "FileReader.hpp"
#include <iostream>

int main(int argc, char *argv[]){
    FileReader fr;
    std::vector<u32_t> trace;
    fr.loadTrace("../inputs/ex11_5_trace.txt", trace);
    //print trace using iterator
    for(auto it = trace.begin(); it != trace.end(); it++){
        std::cout << *it << std::endl;
    }

    return 0;
}
