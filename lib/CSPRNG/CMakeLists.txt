# CMakeLists.txt file to bind library CSPRNG from Michael Thomas Greer
# -> GitHub: https://github.com/Duthomhas
set(CMAKE_CXX_COMPILER g++)
set(SOURCES
    source/csprng.cpp
    source/csprng.c)
set(HEADERS
    source/duthomhas/csprng.h
    source/duthomhas/csprng.hpp
    source/duthomhas/is_iterable.hpp)

add_library(csprng STATIC ${SOURCES} ${HEADERS})
target_include_directories (csprng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source ${CMAKE_CURRENT_SOURCE_DIR}/source/duthomhas)

