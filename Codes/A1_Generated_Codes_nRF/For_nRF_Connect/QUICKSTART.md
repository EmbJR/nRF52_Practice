# Quick Start Guide - nRF52832 LED & UART Project

## 5-Minute Setup

### 1. **Prerequisites Check**
Ensure you have installed:
- [ ] nRF Connect SDK
- [ ] Zephyr RTOS  
- [ ] GCC ARM Embedded Toolchain
- [ ] nRF Command Line Tools

### 2. **Build the Project**

**Windows:**
```batch
build.bat
```

**Linux/macOS:**
```bash
chmod +x build.sh
./build.sh
```

Or manually:
```bash
west build -b nrf52dk_nrf52832
```

### 3. **Flash to Device**
```bash
west flash
```

### 4. **Connect Serial Terminal**
Open any serial terminal (PuTTY, CoolTerm, minicom, etc.):
- **Port**: COM3 (adjust based on your device)
- **Baud**: 115200
- **Data bits**: 8
- **Stop bits**: 1
- **Parity**: None

### 5. **Expected Output**
```
nRF52832 LED and UART Demo Started
LED configured on GPIO P0.13
UART initialized on P0.24 (TX) and P0.25 (RX)
UART Baudrate: 115200

[1] LED toggled. LED is now ON
[2] LED toggled. LED is now OFF
[3] LED toggled. LED is now ON
...
```

---

## Pin Connections

```
┌─────────────────────────────────┐
│      nRF52832 Development Kit   │
├─────────────────────────────────┤
│                                 │
│  P0.13 ─── [LED] ─── GND       │  <- LED (Red)
│                                 │
│  P0.24 (TX) ─── Serial Adapter  │
│  P0.25 (RX) ─── Serial Adapter  │
│  GND ───────── Serial Adapter   │
│                                 │
└─────────────────────────────────┘
```

---

## Features Working

✅ LED blinking on P0.13 (1 second period)
✅ UART output on P0.24@115200
✅ UART input capable on P0.25
✅ Logging and debugging via UART

---

## Project Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build configuration |
| `prj.conf` | Zephyr settings |
| `nrf52dk_nrf52832.overlay` | Hardware pinout |
| `src/main.c` | Application code |
| `README.md` | Detailed documentation |
| `APP_NOTES.md` | Technical specifications |

---

## Common Commands

```bash
# Build only
west build -b nrf52dk_nrf52832

# Build and flash
west build -b nrf52dk_nrf52832 && west flash

# Clean build
rm -rf build && west build -b nrf52dk_nrf52832

# View build options
west config
```

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Command `west` not found | Add nRF Connect SDK to PATH |
| Board `nrf52dk_nrf52832` not found | Update SDK: `west update` |
| Serial terminal shows garbage | Check baud rate is 115200 |
| LED not blinking | Check GPIO configuration in `.overlay` file |

---

## Next Steps

1. **Read [README.md](README.md)** for detailed documentation
2. **Check [APP_NOTES.md](APP_NOTES.md)** for technical details
3. **Modify code** to add your own features
4. **Reference [src/main.c](src/main.c)** for API usage examples

---

**Ready to build? Run `build.bat` (Windows) or `./build.sh` (Linux/macOS)**
