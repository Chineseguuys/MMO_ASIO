cmake_minimum_required(VERSION 3.10)

project(net VERSION 0.1.0)


# clang version 10.0.0-4ubuntu1
set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
# asio is a header only library for c and c++(using the version which been detached from Boost library)
# for version 1.18.1
set (Asio_INCLUDE_DIR "/home/yjh/MYSOFTWARE/asio_1_18/asio/include")
set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++17")

# 输出详细命令
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable( "${PROJECT_NAME}_server"
	src/SimpleServer.cpp
)

target_include_directories( "${PROJECT_NAME}_server"
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${Asio_INCLUDE_DIR}
)
 

target_link_libraries( "${PROJECT_NAME}_server"
	PUBLIC
		pthread
) 


add_executable( "${PROJECT_NAME}_client"
	src/SimpleClient.cpp
)

target_include_directories( "${PROJECT_NAME}_client"
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
		${Asio_INCLUDE_DIR}
)

target_link_libraries( "${PROJECT_NAME}_client"
	PUBLIC
		pthread
)