cmake_minimum_required(VERSION 3.10)

# project name is defined here
project(turinga21)

# specify where the output should be compiled
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# set the compiler to gcc
set(CMAKE_CXX_COMPILER g++)

#set build mode to debug
set(CMAKE_BUILD_TYPE=Release)

# set the compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -march=native -pthread -lstdc++fs -Wall -Wextra -pedantic")

set(SOURCES
        src/colors.cpp
        src/fileinteraction.cpp
        src/main.cpp
        src/mainerror.cpp
        src/measurement.cpp
        src/rotate.cpp
        src/turinga.cpp
    )
set(HEADERS
        include/colors.hpp
        include/fileinteraction.hpp
        include/mainerror.hpp
        include/measurement.hpp
        include/rotate.hpp
        include/turinga.hpp
        include/types.hpp
    )

add_subdirectory(lib/csprng)

#add all sourcefiles and headerfiles to the project
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# look for included files also in the following directories
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} csprng_lib)
