# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Jsdb")

if(WIN32)
	set(BOOST_ROOT "C:\\local\\boost_1_67_0")
endif()

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.67.0 COMPONENTS system thread filesystem regex REQUIRED) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Include dependencies
include_directories(dep)
# Include main source
include_directories(Jsdb)

# Include sub-projects.
add_subdirectory ("Jsdb")
