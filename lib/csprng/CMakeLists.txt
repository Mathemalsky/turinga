
set(CMAKE_CXX_COMPILER g++)
set(SOURCES
    csprng.cpp
    csprng.c)
set(HEADERS
    include/csprng.h
    include/csprng.hpp
    include/is_iterable.hpp)

add_library(csprng_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories (csprng_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

