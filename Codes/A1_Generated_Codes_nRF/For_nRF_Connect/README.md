# nRF52832 LED and UART Project

A Zephyr/nRF Connect SDK based project for the nRF52832 microcontroller demonstrating LED control and UART communication.

## Hardware Configuration

- **LED**: GPIO P0.13 (Active High)
- **UART**: P0.24 (TX) and P0.25 (RX) at 115200 baud

## Project Structure

```
.
├── CMakeLists.txt              # CMake build configuration
├── prj.conf                    # Zephyr project configuration
├── nrf52dk_nrf52832.overlay   # Device tree overlay for GPIO/UART
├── src/
│   └── main.c                 # Main application
└── README.md                  # This file
```

## Prerequisites

- nRF Connect SDK (v2.0.0 or later)
- Zephyr RTOS
- GCC ARM Embedded Toolchain
- nRF Command Line Tools

## Building the Project

### Using west (Recommended)

```bash
# Initialize west workspace (if not already done)
west init -m https://github.com/nrfconnect/sdk-nrf.git

# Update your workspace
west update

# Navigate to project directory
cd path/to/A1_Generated_Codes_nRF/For_nRF_Connect

# Build for nRF52832 DK
west build -b nrf52dk_nrf52832

# Flash to device
west flash
```

### Using CMake directly

```bash
mkdir build
cd build
cmake -DBOARD=nrf52dk_nrf52832 ..
make
```

## Features

✅ **GPIO LED Control**
- LED on P0.13
- Blinking pattern with status messages

✅ **UART Communication**
- Configured on P0.24 (TX) and P0.25 (RX)
- 115200 baud rate
- Console output for debugging

✅ **Logging**
- Real-time logging via UART
- Toggle counters and LED status

## Operation

1. Build and flash the project to your nRF52832 DK
2. Connect a serial terminal at 115200 baud to see console output
3. LED will toggle every 1 second
4. Console will print LED status with counter

### Serial Terminal Connection

```
Port: COM3 (Windows) or /dev/ttyUSB0 (Linux)
Baudrate: 115200
Data bits: 8
Stop bits: 1
Parity: None
```

## Pin Assignment

| Function | Pin | Description |
|----------|-----|-------------|
| LED | P0.13 | Red LED (Active High) |
| UART TX | P0.24 | Serial transmission |
| UART RX | P0.25 | Serial reception |

## Configuration Options

Edit `prj.conf` to modify:
- Log level: `CONFIG_LOG_DEFAULT_LEVEL`
- UART baudrate: change in `.overlay` file
- Heap size: `CONFIG_HEAP_MEM_POOL_SIZE`

## Troubleshooting

**LED not blinking:**
- Verify GPIO pin configuration in overlay
- Check device is ready in main.c
- Confirm nrf52dk_nrf52832.overlay is being applied

**No UART output:**
- Verify baud rate matches terminal settings
- Check TX/RX pins are correctly connected
- Ensure UART is enabled: `CONFIG_UART_CONSOLE=y`

**Build errors:**
- Verify Zephyr environment variables are set
- Check nRF Connect SDK is properly installed
- Run `west update` to sync repositories

## References

- [nRF Connect SDK Documentation](https://developer.nordicsemi.com/nRF_Connect_SDK/)
- [Zephyr RTOS Documentation](https://docs.zephyrproject.org/)
- [nRF52832 Product Brief](https://www.nordicsemi.com/Products/nRF52832)

## License

This project is provided as-is for educational purposes.
