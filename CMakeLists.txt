cmake_minimum_required(VERSION 3.16)
project(OffLaticeEdenModel LANGUAGES CXX)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include(FetchContent)
add_executable(OffLaticeEdenModel src/main.cpp
        src/Surface.h
        src/Cell.h
        src/CellPrimitive.h
)
target_compile_features(OffLaticeEdenModel PRIVATE cxx_std_23)


if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std=c++2a")
set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std=gnu++2a")
endif()

install(TARGETS OffLaticeEdenModel)
