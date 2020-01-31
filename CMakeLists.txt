project(FlowChar)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug")

aux_source_directory(src DIR_SRCS)

add_executable(flowchar ${DIR_SRCS})