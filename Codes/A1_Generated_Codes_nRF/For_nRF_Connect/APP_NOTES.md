# LED and UART Application Notes for nRF52832

## Overview

This document provides detailed technical information about the LED and UART application running on the nRF52832 microcontroller.

## Hardware Specifications

### Target Board
- **Board**: nRF52 DK (nrf52dk_nrf52832)
- **MCU**: Nordic Semiconductor nRF52832
- **Flash**: 512 KB
- **RAM**: 64 KB

### LED Configuration

| Parameter | Value |
|-----------|-------|
| PIN | P0.13 |
| MODE | GPIO Output |
| POLARITY | Active High |
| COLOR | Red |
| Blink Period | 2000 ms (1000ms ON + 1000ms OFF) |

**Schematic Connection:**
```
        3.3V
         │
        [R]  (≈330Ω-470Ω)
         │
        ├─── LED Anode
         │
        LED Cathode ──────── P0.13
                             │
                            GND
```

### UART Configuration

| Parameter | Value |
|-----------|-------|
| TX PIN | P0.24 |
| RX PIN | P0.25 |
| BAUDRATE | 115200 bps |
| DATA BITS | 8 |
| STOP BITS | 1 |
| PARITY | None |
| FLOW CONTROL | None |

**Schematic Connection:**
```
TX (P0.24) ──────── RX of Serial Adapter
RX (P0.25) ──────── TX of Serial Adapter
GND ───────────── GND of Serial Adapter
```

## Software Architecture

### Directory Structure

```
project_root/
├── CMakeLists.txt              # Build system configuration
├── prj.conf                    # Zephyr config options
├── nrf52dk_nrf52832.overlay   # Device tree overlay
├── build.bat                   # Windows build script
├── build.sh                    # Linux build script
├── src/
│   └── main.c                 # Main application
├── README.md                  # User guide
└── APP_NOTES.md              # This file
```

### Main Application Flow

```
Startup
  └─ Initialize GPIO (LED)
      └─ Configure GPIO P0.13 as output
  └─ Initialize UART
      └─ Configure UART0: P0.24 (TX), P0.25 (RX), 115200 baud
  └─ Print startup messages via UART
  
Main Loop (infinite)
  └─ Toggle LED pin
  └─ Print LED status with counter
  └─ Sleep for 1 second
  └─ Repeat
```

## Zephyr Framework Usage

### Kernel APIs Used

1. **GPIO API** (`zephyr/drivers/gpio.h`)
   - `gpio_is_ready_dt()` - Check GPIO device readiness
   - `gpio_pin_configure_dt()` - Configure pin as output
   - `gpio_pin_toggle_dt()` - Toggle GPIO state
   - `gpio_pin_get_dt()` - Read GPIO state

2. **Device API** (`zephyr/device.h`)
   - `DEVICE_DT_GET()` - Get device reference
   - `device_is_ready()` - Check device readiness

3. **Kernel API** (`zephyr/kernel.h`)
   - `k_msleep()` - Sleep in milliseconds

4. **Printing API** (`zephyr/sys/printk.h`)
   - `printk()` - Print formatted output to console

### Device Tree Bindings

The project uses Zephyr's device tree for hardware configuration:

```yaml
/ {
  aliases {
    led0 = &led0;
  };
  
  leds {
    compatible = "gpio-leds";
    led0: led_0 {
      gpios = <&gpio0 13 GPIO_ACTIVE_HIGH>;
    };
  };
};

&uart0 {
  status = "okay";
  current-speed = <115200>;
  tx-pin = <24>;
  rx-pin = <25>;
};
```

## Configuration Details

### prj.conf Options

```
CONFIG_GPIO=y                           # Enable GPIO driver
CONFIG_UART_CONSOLE=y                   # Use UART for console
CONFIG_SERIAL=y                         # Enable serial support
CONFIG_LOG=y                            # Enable logging
CONFIG_PRINTK=y                         # Enable printk
CONFIG_UART_CONSOLE_ON_DEV_NAME="UART_0"  # Use UART_0 for console
```

## Test Procedures

### Visual Test (LED)
1. Flash firmware to device
2. Observe LED on P0.13 blinking

### Serial Communication Test
1. Connect USB-to-UART adapter to P0.24 (TX) and P0.25 (RX)
2. Open serial terminal at 115200 baud
3. Observe counter messages and LED status

### Expected Console Output
```
[000] LED toggled. LED is now ON
[001] LED toggled. LED is now OFF
[002] LED toggled. LED is now ON
[003] LED toggled. LED is now OFF
...
```

## Power Consumption

| State | Typical Current |
|-------|-----------------|
| Active (CPU awake) | ~3-5 mA |
| With LED ON | +2-3 mA |

## Debugging

### Enable Debug Logging

Edit `prj.conf`:
```
CONFIG_LOG_DEFAULT_LEVEL=4  # DEBUG level (0-4)
```

### Print GPIO Status

Call in main loop:
```c
printk("GPIO P0.13 state: %d\n", gpio_pin_get_dt(&led));
```

### Monitor UART Traffic

Use any serial terminal or logic analyzer to observe:
- TX pin (P0.24) for outgoing messages
- RX pin (P0.25) for incoming data (if implemented)

## Memory Usage

Typical memory footprint:

| Type | Used | Available |
|------|------|-----------|
| Flash | ~20-30 KB | 512 KB |
| RAM | ~2-4 KB | 64 KB |
| Heap | 2 KB (configurable) | \- |

## Common Issues and Solutions

### Issue: LED not blinking
**Solution**: 
- Verify P0.13 is configured correctly in overlay
- Check GPIO is properly initialized
- Verify LED polarity

### Issue: UART not working
**Solution**:
- Check P0.24 and P0.25 connections
- Verify 115200 baud rate in terminal
- Check UART device is ready in main.c

### Issue: Build fails
**Solution**:
- Update nRF Connect SDK: `west update`
- Clear build directory: `rm -rf build`
- Check Zephyr environment: `echo $ZEPHYR_BASE`

## Performance Characteristics

- **Boot time**: ~100-200 ms
- **LED toggle latency**: < 1 ms
- **UART throughput**: Up to 115200 bps
- **Blink frequency**: 0.5 Hz (1 second period)

## Future Enhancements

- [ ] Button interrupt handling
- [ ] UART RX interrupt and command parsing
- [ ] Multiple LED patterns
- [ ] PWM fade effects
- [ ] Low power modes
- [ ] Temperature sensor integration

## References

1. [nRF52832 Product Specification](https://infocenter.nordicsemi.com/pdf/nRF52832_PS_v1.4.pdf)
2. [Zephyr Kernel Services Documentation](https://docs.zephyrproject.org/latest/reference/kernel/index.html)
3. [nRF Connect SDK Getting Started](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/index.html)
4. [Device Tree Overview](https://docs.zephyrproject.org/latest/guide/dts/index.html)

## Support and Contact

For issues or questions regarding this project, please refer to:
- Nordic Semiconductor Support: https://www.nordicsemi.com/support
- Zephyr Project: https://www.zephyrproject.org/
- GitHub Issues: Check the project repository

---
**Document Version**: 1.0  
**Last Updated**: 2024
