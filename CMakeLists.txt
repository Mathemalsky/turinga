# Turinga is a simple symmetric encryption scheme based on ideas from enigma.
# Copyright (C) 2022  Mathemalsky, MilchRatchet
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

# project name is defined here
project(turinga21)

# not build in CI by default
OPTION(CIBuild "Configuration for build in CI" OFF)

message(STATUS "CIBuild=${CIBuild}")

# specify where the output should be compiled
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# set the compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -pthread -Wall -Wextra -pedantic")
if(${CIBuild} STREQUAL OFF)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# lists all sourcefiles to be compiled with the project
file(GLOB SOURCES "src/*.cpp")

#lists all header files to be included in the project
file(GLOB HEADERS "include/*.hpp")

add_subdirectory(lib/CSPRNG)

# print useful info about compiler setting
message(STATUS "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

#add all sourcefiles and headerfiles to the project
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# look for included files also in the following directories
target_include_directories(${PROJECT_NAME} PUBLIC include)

# link the csprng library to project
# standard library needs to be additionally linked in windows
if(WIN32)
  target_link_libraries(${PROJECT_NAME} lib_csprng stdc++)
else()
  target_link_libraries(${PROJECT_NAME} lib_csprng)
endif()

# on windows pack the needed dlls in the output directory
# CMake version 3.21 is required for TARGET_RUNTIM_DLLS command
# would return an empty list on linux
if(WIN32 AND ${CIBuild} STREQUAL ON AND ${CMAKE_VERSION} VERSION_GREATER "3.21")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
    )
endif()

