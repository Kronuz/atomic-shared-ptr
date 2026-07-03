# atomic-shared-ptr: atomic_shared_ptr<T> -- thread-safe atomic operations over a std::shared_ptr
#
#   cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && ctest --test-dir build
#
# Header-only, dependency-free. Standard-library only.

cmake_minimum_required(VERSION 3.16)
project(atomic-shared-ptr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

add_library(atomic-shared-ptr INTERFACE)
target_include_directories(atomic-shared-ptr INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(atomic-shared-ptr INTERFACE cxx_std_20)
add_library(atomic-shared-ptr::atomic-shared-ptr ALIAS atomic-shared-ptr)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	enable_testing()
	add_executable(atomic-shared-ptr_test test/test.cc)
	target_link_libraries(atomic-shared-ptr_test PRIVATE atomic-shared-ptr)
	add_test(NAME atomic-shared-ptr COMMAND atomic-shared-ptr_test)
endif()
