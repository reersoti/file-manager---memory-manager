cmake_minimum_required(VERSION 3.16)

project(file_memory_managers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(memory_manager Memory_Manager/os_mem_manager.cpp)
target_include_directories(memory_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Memory_Manager)

add_library(file_manager File_Manager/os_file_test.cpp)
target_include_directories(file_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/File_Manager)

add_executable(test_memory_manager tests/test_memory_manager.cpp)
target_link_libraries(test_memory_manager PRIVATE memory_manager)

add_executable(test_file_manager tests/test_file_manager.cpp)
target_link_libraries(test_file_manager PRIVATE file_manager)

include(CTest)
enable_testing()

add_test(NAME memory_manager_tests COMMAND test_memory_manager)
add_test(NAME file_manager_tests COMMAND test_file_manager)
