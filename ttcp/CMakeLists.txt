cmake_minimum_required(VERSION 3.10)
project(ttcp)

set(CMAKE_CXX_STANDARD 11)

find_package(glog REQUIRED)

add_executable(ttcp main.cc common.cc)
target_link_libraries(ttcp PRIVATE glog::glog)