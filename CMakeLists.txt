cmake_minimum_required(VERSION 3.28)

project(RST
    VERSION 3.1.0
    DESCRIPTION "Versatile C++20 utility library"
    HOMEPAGE_URL "https://github.com/NetcatRunner/RSTools"
    LANGUAGES CXX
)

option(RST_BUILD_TESTS "Build the unit tests" ${PROJECT_IS_TOP_LEVEL})
option(RST_BUILD_EXAMPLES "Build the examples" ${PROJECT_IS_TOP_LEVEL})
option(RST_BUILD_DOCS "Build the API documentation (requires Doxygen)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(RST::RST ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)

if (RST_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (RST_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (RST_BUILD_DOCS)
    add_subdirectory(docs)
endif()
