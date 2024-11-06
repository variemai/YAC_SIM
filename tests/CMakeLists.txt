# Add the test executable
add_executable(test_utilities test_utilities.cpp)
target_link_libraries(test_utilities utilities)

# Register the test executable with CTest
add_test(NAME UtilitiesTest COMMAND test_utilities)
