# Quick Start Guide - nRF52832 GPIO P0.13 Toggle

## 5-Minute Setup

### Prerequisites
- nRF52 DK board
- nRF Connect SDK installed
- USB cable

### Step 1: Build the Project
```bash
cd GPIO
west build -b nrf52dk_nrf52832 -d build
```

### Step 2: Connect Hardware
1. Plug nRF52 DK into USB
2. Ensure J-Link drivers are installed

### Step 3: Flash the Project
```bash
west flash
```

### Step 4: Monitor Output
Open serial terminal (115200 baud):
```
PuTTY, TeraTerm, or minicom
COM port: Check Device Manager
Baud: 115200
```

Expected output:
```
GPIO P0.13 toggled - State: HIGH
GPIO P0.13 toggled - State: LOW
GPIO P0.13 toggled - State: HIGH
...
```

## What's Happening?

- **GPIO P0.13** toggles between HIGH and LOW every 1 second
- Status is printed to console
- Pin can be monitored with oscilloscope or LED+resistor

## Next Steps

### Change Toggle Speed
Edit `src/main.c`:
```c
#define TOGGLE_INTERVAL_MS  500  // Change from 1000 to 500 ms
```
Rebuild and flash.

### Add Button Control
See `src/main_advanced.c` for button interrupt example.

### Monitor with LED
- Connect LED+ to P0.13 (through 470Ω resistor)
- Connect LED- to GND
- LED will blink at 1 second intervals

### Monitor with Oscilloscope
- Channel 1: P0.13 (1V logic level)
- Ground: GND
- Square wave at 1 kHz (500 ms HIGH + 500 ms LOW)

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Build fails | Run `west update` and `source zephyr-env.sh` |
| Flash fails | Check USB, run `nrfjprog --erase` |
| No console | Verify COM port and baud rate (115200) |
| No toggle | Check P0.13 pin connection |

## File Descriptions

| File | Purpose |
|------|---------|
| `src/main.c` | Basic GPIO toggle (use this) |
| `src/main_advanced.c` | Advanced version with button |
| `boards/*.overlay` | GPIO pin mapping |
| `prj.conf` | Kernel configuration |
| `CMakeLists.txt` | Build configuration |
| `build.bat` | Build helper script |
| `flash.bat` | Flash helper script |

## Useful Commands

```bash
# Build only
west build -b nrf52dk_nrf52832 -d build

# Build and flash
west build -b nrf52dk_nrf52832 -d build
west flash

# Clean build
west build -c clean
west build -b nrf52dk_nrf52832 -d build

# View configuration options
west build -t menuconfig
```

## Support Resources

- **Documentation**: See README.md and CONFIG_GUIDE.md
- **SDK Docs**: https://developer.nordicsemi.com/nRF_Connect_SDK/
- **Zephyr Docs**: https://docs.zephyrproject.org/
- **nRF Community**: https://devzone.nordicsemi.com/

---

**Ready to go!** Your GPIO P0.13 should be toggling now. 🚀
