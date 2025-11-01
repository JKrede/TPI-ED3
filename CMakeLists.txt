cmake_minimum_required(VERSION 3.28)
project(
  TPI-ED3
  VERSION 1.0.0
  LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/CMSISv2p00_LPC17xx/Drivers/inc)
include_directories(${CMAKE_SOURCE_DIR}/inc)
include_directories(${CMAKE_SOURCE_DIR}/CMSISv2p00_LPC17xx/inc)

add_executable(main src/main.c)
