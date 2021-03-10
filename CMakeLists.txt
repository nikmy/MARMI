cmake_minimum_required(VERSION 3.16)
project(Multiple_Access_Resource_Management_Interface)
set(CMAKE_CXX_STANDARD 20)

set(GENERICS_SOURCES sources/generics/gendef.h sources/generics/resource.h sources/generics/handler.h sources/generics/supplier.h sources/generics/manager.h)
set(QUEUE_SOURCES sources/queue/queue.h)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -fsanitize=address -fsanitize=undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -Wall -Wextra -fsanitize=address -fsanitize=undefined")

include_directories(sources/generics sources/queue sources/tests sources)

set(TESTS_SOURCES sources/tests/test_generics.h sources/tests/test_queue.h)

add_definitions(-DQUEUE_TEST)
add_definitions(-DGENERICS_TEST)

add_executable(${PROJECT_NAME} ${GENERICS_SOURCES} ${QUEUE_SOURCES} ${TESTS_SOURCES} sources/main.cpp)