@echo off
:: Define paths
set NORDIC_TOOLCHAIN_PATH=C:\ncs\toolchains\fd21892d0f\opt\zephyr-sdk\arm-zephyr-eabi\bin
set NORDIC_COREDUMP_SCRIPT=G:\New\Microcontrollers\nRF_Files\nRF_Connect_SDK\zephyr\scripts\coredump

:: Define the name of the GDB command file
set GDB_INIT_FILE=gdb_commands.gdb

:: Create the GDB command file in the directory where this script is located
:: %~dp0 expands to the drive and path of the current batch script

:: 0. Turn on command echoing (This displays the commands in the window)
echo set trace-commands on > "%~dp0%GDB_INIT_FILE%"

:: 1. Connect to the remote server
echo target remote localhost:1234 >> "%~dp0%GDB_INIT_FILE%"

:: 2. Add informative text

:: 3. Print the backtrace (stack trace)
echo bt >> "%~dp0%GDB_INIT_FILE%"

:: Run GDB
:: -x: Execute commands from the file
"%NORDIC_TOOLCHAIN_PATH%\arm-zephyr-eabi-gdb" -x "%~dp0%GDB_INIT_FILE%" zephyr.elf
