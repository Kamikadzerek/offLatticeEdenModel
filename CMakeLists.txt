cmake_minimum_required(VERSION 3.16)
project(OffLaticeEdenModel LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
add_executable(OffLaticeEdenModel src/main.cpp
        src/Surface.h
        src/Cell.h
        src/CellPrimitive.h)
#        src/PlotsGenerators.cpp)
target_link_libraries(OffLaticeEdenModel PRIVATE sfml-graphics)
target_compile_features(OffLaticeEdenModel PRIVATE cxx_std_23)
find_package(Boost REQUIRED COMPONENTS iostreams system filesystem)
target_include_directories(OffLaticeEdenModel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(OffLaticeEdenModel PUBLIC ${Boost_LIBRARIES})

if(WIN32)
    add_custom_command(
        TARGET OffLaticeEdenModel
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:OffLaticeEdenModel>
        VERBATIM)
endif()

install(TARGETS OffLaticeEdenModel)
