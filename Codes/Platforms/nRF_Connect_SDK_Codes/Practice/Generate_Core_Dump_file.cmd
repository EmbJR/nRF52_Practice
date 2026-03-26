@echo off

set NORDIC_COREDUMP_SCRIPT=G:\New\Microcontrollers\nRF_Files\nRF_Connect_SDK\zephyr\scripts\coredump

"%NORDIC_COREDUMP_SCRIPT%\coredump_serial_log_parser.py" dump.log dump.bin