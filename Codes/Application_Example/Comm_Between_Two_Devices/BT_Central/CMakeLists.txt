cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(Custom_client)

target_sources(app PRIVATE src/main.c src/custom_client.c) 

zephyr_library_include_directories(.)