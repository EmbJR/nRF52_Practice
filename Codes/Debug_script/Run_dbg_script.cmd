@echo off
set NORDIC_TOOLCHAIN_PATH=C:\ncs\toolchains\fd21892d0f\opt\zephyr-sdk\arm-zephyr-eabi\bin
set NORDIC_COREDUMP_SCRIPT=G:\New\Microcontrollers\nRF_Files\nRF_Connect_SDK\zephyr\scripts\coredump

"%NORDIC_COREDUMP_SCRIPT%\coredump_serial_log_parser.py" dump.log dump.bin
"%NORDIC_COREDUMP_SCRIPT%\coredump_gdbserver.py" zephyr.elf dump.bin -v

#"%NORDIC_TOOLCHAIN_PATH%\arm-zephyr-eabi-gdb" zephyr.elf