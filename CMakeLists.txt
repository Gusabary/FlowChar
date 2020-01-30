project(FlowChar)

cmake_minimum_required(VERSION 2.6)

aux_source_directory(src DIR_SRCS)

add_executable(flowchar ${DIR_SRCS})