# CMakeList.txt : CMake project for Jsdb, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add components subdirectory
add_subdirectory("components")
# Add components subdirectory to includes
include_directories("components")

# Add source to this project's executable.
add_executable (Jsdb "Jsdb.cpp" "Jsdb.hpp")

# Link library
target_link_libraries(Jsdb JsdbComponents ${Boost_LIBRARIES})
# TODO: Add tests and install targets if needed.

# Post-Build - clone jsdb.config-dist to the binary path
add_custom_command(
    TARGET Jsdb POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/jsdb.json-dist
            ${CMAKE_CURRENT_BINARY_DIR}/jsdb.json
)