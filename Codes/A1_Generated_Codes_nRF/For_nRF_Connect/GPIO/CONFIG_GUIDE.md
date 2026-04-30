# nRF52832 Project Configuration Guide

## Project Overview

This document provides detailed configuration and customization options for the nRF52832 GPIO P0.13 toggle application.

## File Structure

```
GPIO/
├── CMakeLists.txt                 # Build system configuration
├── prj.conf                       # Zephyr kernel configuration
├── src/
│   ├── main.c                     # Basic GPIO toggle implementation
│   └── main_advanced.c            # Advanced version with button control
├── boards/
│   └── nrf52dk_nrf52832.overlay   # Device tree overlay (GPIO pin mapping)
├── .vscode/
│   ├── launch.json                # Debugging configuration
│   └── tasks.json                 # Build tasks
├── build.bat                      # Build helper script
├── flash.bat                      # Flash helper script
├── README.md                      # Main documentation
├── CONFIG_GUIDE.md                # This file
└── .gitignore                     # Git ignore patterns
```

## Configuration Files

### 1. prj.conf - Zephyr Configuration

This file contains Zephyr kernel configuration options. Edit to:

- **Enable/disable GPIO support:**
  ```
  CONFIG_GPIO=y           # Enable GPIO subsystem
  ```

- **Add logging support:**
  ```
  CONFIG_GPIO_LOG_LEVEL_INF=y     # GPIO logging (INF=Info level)
  ```

- **Add more features:**
  ```
  CONFIG_CONSOLE=y        # Enable console
  CONFIG_PRINTK=y         # Enable printk for debugging
  ```

### 2. boards/nrf52dk_nrf52832.overlay - Device Tree Overlay

Maps GPIO pins to the application. Example configurations:

**Basic GPIO P0.13 output:**
```
&gpio0 {
	status = "okay";
};

gpio_p013: gpio {
	compatible = "gpio-leds";
	label = "GPIO P0.13";
	
	p013_pin {
		gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>;
		label = "P0.13";
	};
};
```

**Add button on P0.11:**
```
button {
	compatible = "gpio-keys";
	label = "Button";
	
	button_pin {
		gpios = <&gpio0 11 GPIO_ACTIVE_LOW>;
		label = "Button";
	};
};
```

### 3. CMakeLists.txt - Build Configuration

Configures the build system:

```cmake
cmake_minimum_required(VERSION 3.20.0)
project(gpio_p013_toggle)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
target_sources(app PRIVATE src/main.c)
```

To add more source files:
```cmake
target_sources(app PRIVATE 
    src/main.c
    src/gpio_handler.c
    src/uart_handler.c
)
```

## Building Options

### Using West (Recommended)

```bash
# Build with debug symbols
west build -b nrf52dk_nrf52832 -- -DCMAKE_BUILD_TYPE=Debug

# Build with release optimization
west build -b nrf52dk_nrf52832 -- -DCMAKE_BUILD_TYPE=Release

# Clean build
west build -c clean
west build -b nrf52dk_nrf52832
```

### Using VS Code Tasks

- **Build**: Ctrl+Shift+B (or run "build" task)
- **Flash**: Run "flash" task (includes automatic build)
- **Clean**: Run "clean" task

## GPIO Pin Configuration

### Available GPIO Pins on nRF52832

- **GPIO Port 0 (P0)**: Pins 0-31
- **GPIO Port 1 (P1)**: Not available on nRF52832

### Pin Naming Convention

- **P0.00** → gpio0, pin 0
- **P0.13** → gpio0, pin 13
- **P0.31** → gpio0, pin 31

### Configuring Different Pins

To use a different GPIO pin:

1. Update `src/main.c`:
   ```c
   #define GPIO_PIN_NUMBER 13    // Change to desired pin
   ```

2. Update `boards/nrf52dk_nrf52832.overlay`:
   ```c
   gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>;  // Change pin number
   ```

## Toggle Interval Configuration

The default toggle interval is 1000 ms (1 second).

To change toggle interval:

In `src/main.c`:
```c
#define TOGGLE_INTERVAL_MS  1000  // Change this value

// Or in the main loop:
k_msleep(500);  // Toggle every 500 ms
```

## Advanced GPIO Modes

### GPIO Output Modes

- `GPIO_OUTPUT_ACTIVE` - Output configured, initially active (HIGH)
- `GPIO_OUTPUT_INACTIVE` - Output configured, initially inactive (LOW)
- `GPIO_OUTPUT` - General output

### GPIO Input Modes

- `GPIO_INPUT` - Input only
- `GPIO_INPUT | GPIO_PULL_UP` - Input with pull-up
- `GPIO_INPUT | GPIO_PULL_DOWN` - Input with pull-down

### GPIO Interrupt Options

- `GPIO_INT_EDGE_FALLING` - Interrupt on falling edge (HIGH to LOW)
- `GPIO_INT_EDGE_RISING` - Interrupt on rising edge (LOW to HIGH)
- `GPIO_INT_EDGE_BOTH` - Interrupt on both edges
- `GPIO_INT_LEVEL_LOW` - Interrupt when level is LOW
- `GPIO_INT_LEVEL_HIGH` - Interrupt when level is HIGH

## Serial Communication Configuration

### UART Console Baud Rate

Default: 115200 bps

To change in `prj.conf`:
```
CONFIG_CONSOLE=y
CONFIG_CONSOLE_PREFERRED_IDX=0    # UART 0
```

### Logging Configuration

```
CONFIG_LOG=y                       # Enable logging
CONFIG_LOG_MODE_IMMEDIATE=y        # Immediate log output
CONFIG_LOG_DEFAULT_LEVEL=3         # Log level (0-4)
```

## Debugging Configuration

### VS Code Launch Configuration

Edit `.vscode/launch.json` for debugging:

```json
{
    "type": "cppdbg",
    "name": "nRF52832 Debug",
    "program": "${workspaceFolder}/build/zephyr/zephyr.elf",
    "miDebuggerPath": "arm-none-eabi-gdb"
}
```

### Debug Symbols

Build with debug symbols:
```bash
west build -b nrf52dk_nrf52832 -- -DCMAKE_BUILD_TYPE=Debug
```

## Performance Optimization

### Reduce Binary Size

```bash
# Build with release optimization
west build -b nrf52dk_nrf52832 -- -DCMAKE_BUILD_TYPE=Release
```

### Minimize RAM Usage

In `prj.conf`:
```
CONFIG_LOG=n            # Disable logging if not needed
CONFIG_DEBUG=n          # Disable debug features
```

## Power Management

### Enable Low Power Modes

Add to `prj.conf`:
```
CONFIG_PM=y
CONFIG_PM_DEVICE=y
CONFIG_SYS_PM_STATES=y
```

### Reduce Power Consumption

Adjust toggle interval:
```c
#define TOGGLE_INTERVAL_MS  5000  // Less frequent toggles
```

## Peripheral Addition Examples

### Add UART

1. Update `prj.conf`:
   ```
   CONFIG_UART_CONSOLE=y
   CONFIG_SERIAL=y
   ```

2. Update overlay:
   ```
   &uart0 {
       compatible = "nordic,nrf-uart";
       status = "okay";
   };
   ```

3. Use in code:
   ```c
   const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
   ```

### Add PWM for LED Brightness

1. Update `prj.conf`:
   ```
   CONFIG_PWM=y
   CONFIG_PWM_NRFX=y
   ```

2. Configure in overlay and use PWM APIs

### Add ADC for Analog Input

1. Update `prj.conf`:
   ```
   CONFIG_ADC=y
   CONFIG_ADC_NRFX=y
   ```

## Troubleshooting Configuration

### Issue: GPIO Not Responding

1. Verify pin configuration in overlay file
2. Check `prj.conf` has `CONFIG_GPIO=y`
3. Confirm device tree compilation: `west build -t menuconfig`

### Issue: Build Fails

1. Clean previous build: `west build -c clean`
2. Verify Zephyr environment: `source ~/ncs/zephyr/zephyr-env.sh`
3. Update west: `west update`

### Issue: Flash Fails

1. Check USB connection
2. Verify nRF52 DK drivers installed
3. Erase device: `nrfjprog --erase`

## Further Customization

1. **Multiple GPIO Pins**: Add more pins in overlay and code
2. **Interrupts**: Use GPIO interrupts for event-driven control
3. **Timers**: Use Zephyr timers for precise timing
4. **Real-time**: Use kernel configuration for deterministic behavior
5. **Drivers**: Create custom driver for complex GPIO scenarios

## Resources

- nRF Connect SDK: https://developer.nordicsemi.com/nRF_Connect_SDK/
- Zephyr Documentation: https://docs.zephyrproject.org/
- nRF52832 Reference: https://infocenter.nordicsemi.com/
- Device Tree Bindings: https://github.com/zephyrproject-rtos/zephyr/tree/main/dts/bindings

## Additional Notes

- Keep `prj.conf` clean - only add necessary configurations
- Use meaningful variable names in code
- Comment code sections for clarity
- Test incrementally when adding new features
- Version control your configurations
