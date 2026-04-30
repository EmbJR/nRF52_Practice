@echo off
REM Flash script for nRF52832 GPIO P0.13 Toggle Application
REM This script flashes the built hex file to the nrf52dk_nrf52832 board

echo.
echo ========================================
echo nRF52832 GPIO P0.13 Toggle Application
echo Flash Script
echo ========================================
echo.

REM Check if hex file exists
if not exist "build\zephyr\zephyr.hex" (
    echo Error: Hex file not found!
    echo Please build the project first by running build.bat
    echo.
    pause
    exit /b 1
)

echo Connecting to nRF52 DK...
echo.

REM Erase the device
echo Erasing device...
nrfjprog --erase

if %ERRORLEVEL% NEQ 0 (
    echo Error: Could not erase device. Check USB connection and drivers.
    echo.
    pause
    exit /b 1
)

echo Device erased successfully.
echo.

REM Program the hex file
echo Programming hex file...
nrfjprog --program build\zephyr\zephyr.hex --reset

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Flash completed successfully!
    echo.
    echo GPIO P0.13 should now be toggling at 1-second intervals.
    echo.
    echo To view console output:
    echo 1. Connect to the debug UART (COM port)
    echo 2. Use terminal software (PuTTY, TeraTerm) at 115200 baud
    echo.
    echo ========================================
) else (
    echo.
    echo Flash failed! Check errors above.
    pause
    exit /b 1
)

pause
