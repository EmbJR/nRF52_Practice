# GPIO P0.13 Toggle Application for nRF52832

This is a complete nRF Connect SDK project for the nRF52832 Development Kit that toggles GPIO pin P0.13 at 1-second intervals.

## Project Structure

```
GPIO/
├── CMakeLists.txt              # CMake build configuration
├── prj.conf                    # Zephyr project configuration
├── src/
│   └── main.c                  # Main application source code
├── boards/
│   └── nrf52dk_nrf52832.overlay    # Device tree overlay for GPIO pin
└── README.md                   # This file
```

## Features

- **GPIO P0.13 Toggle**: Toggles GPIO pin P0.13 every 1 second
- **Status Logging**: Prints the current state (HIGH/LOW) to console
- **Error Handling**: Includes error checking for GPIO operations
- **nRF Connect SDK**: Based on Zephyr RTOS framework

## Hardware Requirements

- **Board**: nRF52 DK (nRF52832)
- **Connection**: USB for debugging and power
- **GPIO Pin**: P0.13 (can be monitored with an LED and resistor, or oscilloscope)

## Prerequisites

1. **nRF Connect SDK** - Installed and configured
2. **Zephyr Environment** - Set up with proper environment variables
3. **West Build Tool** - Part of nRF Connect SDK
4. **Nordic Semiconductors Tools** - nRF Command Line Tools

## Building the Project

### Using VS Code with nRF Connect SDK Extension

1. Open the project folder in VS Code
2. Use the nRF Connect extension to:
   - Select the board: `nrf52dk_nrf52832`
   - Build the project
   - Flash to device

### Using Command Line

```bash
# From the project root directory
west build -b nrf52dk_nrf52832 -d build

# Flash to the device
west flash
```

## Flashing to the Device

### Method 1: Using West
```bash
west flash
```

### Method 2: Using nRF Command Line Tools
```bash
nrfjprog --erase
nrfjprog --program build/zephyr/zephyr.hex --reset
```

## Monitoring Output

Connect to the debug UART to view console output:

```bash
# Using minicom (Linux/macOS)
minicom -D /dev/ttyACM0 -b 115200

# Using PuTTY (Windows)
# - Serial line: COM3 (adjust to your port)
# - Speed: 115200
# - Connection type: Serial
```

Expected output:
```
nRF52832 GPIO P0.13 Toggle Application
========================================

GPIO P0.13 configured as output
Starting toggle loop...

GPIO P0.13 toggled - State: HIGH
GPIO P0.13 toggled - State: LOW
GPIO P0.13 toggled - State: HIGH
GPIO P0.13 toggled - State: LOW
...
```

## Pin Configuration

### GPIO P0.13 Details

- **Port**: GPIO0 (nRF52832 has one GPIO port)
- **Pin Number**: 13
- **Direction**: Output
- **Mode**: Active High
- **Toggle Interval**: 1000 ms (1 second)

## Modifying the Project

### Change Toggle Interval

Edit `src/main.c` and modify:
```c
#define TOGGLE_INTERVAL_MS	1000	/* Change this value */
```

### Use Different GPIO Pin

1. Update `src/main.c`:
   ```c
   #define GPIO_PIN_NUMBER		13	/* Change to desired pin */
   ```

2. Update `boards/nrf52dk_nrf52832.overlay`:
   ```c
   gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>;  /* Change pin number */
   ```

### Add Additional Functionality

- Add button input on another GPIO pin
- Implement PWM for brightness control
- Add ADC for analog input reading
- Integrate with other nRF52832 peripherals

## Resources

- **nRF Connect SDK Documentation**: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/
- **Zephyr Project**: https://www.zephyrproject.org/
- **nRF52832 Datasheet**: https://infocenter.nordicsemi.com/
- **nRF52 DK Documentation**: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrf52_dk%2FUG%2Fnrf52_DK_getting_started.html

## Troubleshooting

### Build Issues

1. **CMake Error**: Ensure Zephyr environment is set up:
   ```bash
   source ~/ncs/zephyr/zephyr-env.sh
   ```

2. **Device Not Found**: Check USB connection and drivers

### Flashing Issues

1. Erase the device first:
   ```bash
   nrfjprog --erase
   ```

2. Check board connection and J-Link drivers

### No Console Output

- Verify UART connections
- Check baud rate (115200)
- Ensure serial monitor is connected to correct COM port

## License

This project is provided as-is for educational and development purposes.

## Notes

- The toggle interval can be adjusted based on application requirements
- GPIO P0.13 can be monitored with an oscilloscope or LED
- Additional GPIO pins can be added following the same pattern
- The project uses Zephyr RTOS device tree format for hardware abstraction
