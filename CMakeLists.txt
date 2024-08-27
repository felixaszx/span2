cmake_minimum_required(VERSION 3.20.0)
project(span2 VERSION 0.1.0)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/exe")

find_package(PkgConfig REQUIRED)
pkg_check_modules(TBB REQUIRED IMPORTED_TARGET tbb)


file(GLOB_RECURSE CPP_SRC "src/*.cpp")
add_executable(span2 ${CPP_SRC})

target_include_directories(span2 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(span2 PRIVATE PkgConfig::TBB)