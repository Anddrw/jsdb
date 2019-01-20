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
