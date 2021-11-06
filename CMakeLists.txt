cmake_minimum_required(VERSION 3.10)

# project name is defined here
project(turinga21)

# specify where the output should be compiled
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# set the compiler to gcc
set(CMAKE_CXX_COMPILER g++)

#set build mode to debug
set(CMAKE_BUILD_TYPE=Release)

# set the compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -march=native -pthread -lstdc++fs -Wall -Wextra -pedantic")

# lists all sourcefiles to be compiled with the project
file(GLOB SOURCES "src/*.cpp")

#lists all header files to be included in the project
file(GLOB HEADERS "include/*.hpp")

add_subdirectory(lib/CSPRNG)

#add all sourcefiles and headerfiles to the project
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# look for included files also in the following directories
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} lib_csprng)
