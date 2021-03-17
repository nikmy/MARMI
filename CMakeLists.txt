cmake_minimum_required(VERSION 3.16)
project(Multiple_Access_Resource_Management_Interface)
set(CMAKE_CXX_STANDARD 20)

set(GENERICS_SOURCES sources/generics/gendef.h sources/generics/resource.h sources/generics/manager.h)
set(QUEUE_SOURCES sources/queue/queue.h)
set(SERVER_SOURCES sources/queue/queue.h sources/server/bd_request.cpp sources/server/bd_request.h sources/server/bd_request_handler.cpp sources/server/bd_request_handler.h sources/server/bd_request_generator.cpp sources/server/bd_request_generator.h sources/server/echo_server.cpp sources/server/echo_server.h sources/server/bd_request_counter.cpp sources/server/bd_request_counter.h)
set(TESTS_SOURCES sources/tests/test_generics.h sources/tests/test_queue.h sources/tests/test_server.h sources/tests/progress_bar.h)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined")

include_directories(sources/generics sources/queue sources/server sources/tests sources)

add_definitions(-DQUEUE_TEST)
#add_definitions(-DGENERICS_TEST)
#add_definitions(-DSERVER_TEST)

add_executable(${PROJECT_NAME} ${GENERICS_SOURCES} ${QUEUE_SOURCES} ${SERVER_SOURCES} ${TESTS_SOURCES} sources/main.cpp)