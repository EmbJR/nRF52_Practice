@echo off
REM Build script for nRF52832 GPIO P0.13 Toggle Application
REM This script builds the project for nrf52dk_nrf52832 board

echo.
echo ========================================
echo nRF52832 GPIO P0.13 Toggle Application
echo Build Script
echo ========================================
echo.

REM Check if build directory exists, if not create it
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

echo Building project for nrf52dk_nrf52832...
echo.

REM Run west build command
west build -b nrf52dk_nrf52832 -d build

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Build completed successfully!
    echo Hex file: build\zephyr\zephyr.hex
    echo.
    echo Next steps:
    echo 1. Connect nRF52 DK to your computer via USB
    echo 2. Run: west flash
    echo ========================================
) else (
    echo.
    echo Build failed! Check errors above.
    exit /b 1
)

pause
