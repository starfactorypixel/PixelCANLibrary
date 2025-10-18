cmake_minimum_required(VERSION 3.10)
project(TestClassProject LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_library(can_library INTERFACE)
target_include_directories(can_library INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()

find_package(GTest COMPONENTS gtest_main QUIET)
if (NOT GTest_FOUND)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG v1.13.0
	)
	FetchContent_MakeAvailable(googletest)
endif()

add_executable(test_runner)
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
target_sources(test_runner PRIVATE ${TEST_SOURCES})
target_compile_features(test_runner PRIVATE cxx_std_17)
target_link_libraries(test_runner PRIVATE can_library GTest::gtest_main)

add_test(NAME TestRunner COMMAND test_runner)
