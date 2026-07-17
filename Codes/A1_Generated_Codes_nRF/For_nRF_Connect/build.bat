@echo off
REM Build script for nRF52832 LED and UART Project

echo.
echo =========================================
echo nRF52832 LED and UART Project Builder
echo =========================================
echo.

REM Check if west is available
where west >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: west command not found
    echo Please ensure nRF Connect SDK is installed and in PATH
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build

echo Building project...
cd build

REM Configure and build
cmake -DBOARD=nrf52dk_nrf52832 ..
if %ERRORLEVEL% NEQ 0 (
    echo Build configuration failed
    pause
    exit /b 1
)

cmake --build .
if %ERRORLEVEL% NEQ 0 (
    echo Build failed
    pause
    exit /b 1
)

cd ..

echo.
echo =========================================
echo Build completed successfully!
echo =========================================
echo.
echo To flash the firmware, run:
echo   west flash
echo.
pause
