cmake_minimum_required(VERSION 3.16)

project(simple_example VERSION 0.1.0)

set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
set (Asio_INCLUDE_DIR "/home/yjh/MYSOFTWARE/asio_1_18/asio/include")
set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS_DEBUG " -O0 -g")

message("asio header only include path is ${Asio_INCLUDE_DIR}")

add_executable(${PROJECT_NAME}
	src/SimpleExample.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${ASIO_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		pthread
)