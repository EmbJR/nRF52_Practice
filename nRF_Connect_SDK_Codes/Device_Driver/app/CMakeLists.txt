# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(device_driver)

list(APPEND EXTRA_ZEPHYR_MODULES 
    &{CMAKE_CURRENT_SOURCE_DIR}/../custom_driver
)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

target_sources(app PRIVATE src/main.c)
