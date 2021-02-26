cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
set(CMake_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c")

project(QSaharaServer)


add_executable(
 QSaharaServer
 kickstart.c
 sahara_protocol.c
 comm.c
 comm.h
 common_protocol_defs.h
 external_utils.h
 external_utils.c
 kickstart_log.h
 kickstart_log.c
 kickstart_utils.h
 kickstart_utils.c
 sahara_protocol.h
)
