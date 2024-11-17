#include "FileReader.hpp"
#include "utilities.hpp"
#include <iostream>

// Implementation of the loadTrace function
// This function should be called in a while loop until the user provides a valid filename
int FileReader::loadTrace(const std::string& filename, std::vector<u32_t>& trace) {
    int result = 1;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        size_t lineNumber = 0;

        while (std::getline(file, line)) {
            lineNumber++;
            try {
                // Convert to unsigned long long first to handle all cases
                unsigned long long value = std::stoull(line, nullptr, 10);

                // Check if value fits in u32_t
                if (value > std::numeric_limits<u32_t>::max()) {
                    std::cerr << "Error: Number on line " << lineNumber
                             << " exceeds maximum value for u32_t ("
                             << std::numeric_limits<u32_t>::max() << ")\n";
                    file.close();
                    return -1;
                }

                trace.push_back(static_cast<u32_t>(value));
            }
            catch (const std::invalid_argument&) {
                std::cerr << "Error: Invalid number format on line " << lineNumber << ": '" << line << "'\n";
                file.close();
                return -1;
            }
            catch (const std::out_of_range&) {
                std::cerr << "Error: Number on line " << lineNumber << " is too large: '" << line << "'\n";
                file.close();
                return -1;
            }
        }

        file.close();
        result = 0;
    }
    else {
        std::cerr << "Error: Could not open file " << filename << ", try again\n";
        result = -1;
    }

    return result;
}
