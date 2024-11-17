# Add the test executable
add_executable(test_utilities test_utilities.cpp)
add_executable(test_cachechar test_cachechar.cpp)
add_executable(test_filereader test_filereader.cpp)
target_link_libraries(test_utilities utilities)
target_link_libraries(test_cachechar utilities)
target_link_libraries(test_filereader utilities)

# Register the test executable with CTest
add_test(NAME UtilitiesTest COMMAND test_utilities)
add_test(NAME CacheCharacteristicsTest COMMAND test_cachechar)
add_test(NAME FileReaderTest COMMAND test_filereader)
