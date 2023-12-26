cmake_minimum_required(VERSION 3.10)
project(network_programming)

set(CMAKE_CXX_STANDARD 11)

set(EXEC_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_PATH})

add_subdirectory(ttcp)


