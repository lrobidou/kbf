cmake_minimum_required(VERSION 3.13.0)
project(qtf VERSION 0.1.0)

set(CONTACT "lucas.robidou@inria.fr")


set(CMAKE_CXX_STANDARD 17)  # TODO: find why this does not work if I overwrite CMAKE_CXX_COMPILER (even before)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# thirdparty source directories
# set(THIRDPARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)

# thirdparty build directories
# set(THIRDPARTY_BUILD_DIR ${PROJECT_BINARY_DIR}/thirdparty)

# include thridparty directories
include_directories(SYSTEM thirdparty/libbf)
include_directories(SYSTEM thirdparty/boost/install/include)
link_directories(thirdparty/bf/build/lib)
# add libbf
add_subdirectory(thirdparty/libbf)

add_executable(kbf src/main.cpp)

add_executable(kbf2only src/main2kbf.cpp)
target_link_libraries(kbf libbf_static)
target_link_libraries(kbf2only libbf_static)
