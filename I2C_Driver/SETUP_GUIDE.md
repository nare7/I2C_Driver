# I2C Communication: Raspberry Pi 4 ↔ STM32F401RE
# Complete Setup Guide

## Hardware Requirements
1. Raspberry Pi 4
2. STM32F401RE Nucleo board
3. Jumper wires (4 minimum)
4. Pull-up resistors (2x 4.7kΩ recommended for I2C lines)

## Pin Connections

### Raspberry Pi 4 (I2C-1)
- Pin 3  (GPIO 2)  → SDA
- Pin 5  (GPIO 3)  → SCL
- Pin 6  (GND)     → GND
- Pin 1  (3.3V)    → 3.3V (optional for pull-ups)

### STM32F401RE
- PB9 → SDA
- PB8 → SCL
- GND → GND

### Wiring Diagram
```
Raspberry Pi 4          STM32F401RE
--------------          ------------
Pin 3 (GPIO2/SDA) ----→ PB9 (SDA)
Pin 5 (GPIO3/SCL) ----→ PB8 (SCL)
Pin 6 (GND)       ----→ GND

Pull-up resistors (4.7kΩ):
- Between SDA and 3.3V
- Between SCL and 3.3V
(Note: Raspberry Pi has internal pull-ups, but external ones improve reliability)
```

═══════════════════════════════════════════════════════════════════
PART 1: RASPBERRY PI 4 SETUP
═══════════════════════════════════════════════════════════════════

## Step 1: Enable I2C on Raspberry Pi

1. Edit boot configuration:
```bash
sudo nano /boot/config.txt
```

2. Add/uncomment these lines:
```
dtparam=i2c_arm=on
dtparam=i2c1=on
```

3. Save and exit (Ctrl+X, Y, Enter)

4. Reboot:
```bash
sudo reboot
```

## Step 2: Install Required Tools

```bash
# Update system
sudo apt-get update
sudo apt-get upgrade

# Install I2C tools
sudo apt-get install -y i2c-tools

# Install kernel headers for driver compilation
sudo apt-get install -y raspberrypi-kernel-headers

# Install device tree compiler
sudo apt-get install -y device-tree-compiler
```

## Step 3: Verify I2C Bus

```bash
# Check if I2C devices are present
ls /dev/i2c*

# You should see: /dev/i2c-1 (and possibly /dev/i2c-0)

# Load I2C dev module if needed
sudo modprobe i2c-dev

# Scan I2C bus (STM32 should show at address 0x30)
sudo i2cdetect -y 1
```

## Step 4: Compile Device Tree Overlay

```bash
# Navigate to project directory
cd /path/to/project

# Compile device tree overlay
sudo dtc -@ -I dts -O dtb -o i2c1-stm32-overlay.dtbo i2c1-stm32-overlay.dts

# Copy to overlays directory
sudo cp i2c1-stm32-overlay.dtbo /boot/overlays/

# Edit config.txt to load overlay
sudo nano /boot/config.txt

# Add this line at the end:
dtoverlay=i2c1-stm32-overlay

# Save and reboot
sudo reboot
```

## Step 5: Compile the Kernel Driver

```bash
# Navigate to project directory
cd /path/to/project

# Compile the driver
make

# You should see: i2c_char_driver.ko

# Check kernel module info
modinfo i2c_char_driver.ko
```

## Step 6: Load the Driver

```bash
# Insert the module
sudo insmod i2c_char_driver.ko

# Verify module is loaded
lsmod | grep i2c_char

# Check kernel messages
dmesg | tail -20

# You should see messages like:
# "I2C Character Driver Loading..."
# "Device created: /dev/i2c_stm32"

# Verify device node exists
ls -l /dev/i2c_stm32
```

## Step 7: Compile Test Application

```bash
# Compile test program
gcc -o test_i2c test_i2c.c

# Give execution permission
chmod +x test_i2c
```

═══════════════════════════════════════════════════════════════════
PART 2: STM32F401RE SETUP
═══════════════════════════════════════════════════════════════════

## Step 1: Install ARM Toolchain

### On Linux (Raspberry Pi or PC):
```bash
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi

# Verify installation
arm-none-eabi-gcc --version
```

### On Windows:
Download and install ARM GCC from:
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm

## Step 2: Install Flashing Tool

### Option A: ST-Link (Recommended)
```bash
# Install st-flash utility
sudo apt-get install -y stlink-tools

# Verify installation
st-flash --version
```

### Option B: OpenOCD
```bash
sudo apt-get install -y openocd

# Verify installation
openocd --version
```

## Step 3: Compile STM32 Firmware

```bash
# Navigate to STM32 project directory
cd /path/to/stm32_project

# Compile using Makefile
make -f Makefile_STM32

# You should see:
# - stm32_i2c_slave.bin
# - stm32_i2c_slave.hex
# - stm32_i2c_slave.elf

# Check size
arm-none-eabi-size stm32_i2c_slave.elf
```

## Step 4: Flash STM32

### Connect ST-Link Programmer:
Connect ST-Link V2 to STM32 Nucleo board:
- SWDIO → SWDIO (CN4 pin 2)
- SWCLK → SWCLK (CN4 pin 4)
- GND   → GND
- 3.3V  → 3.3V (optional)

### Flash using st-flash:
```bash
# Flash the binary
sudo st-flash write stm32_i2c_slave.bin 0x8000000

# You should see:
# "Flash written and verified! jolly good!"
```

### Flash using OpenOCD (alternative):
```bash
make -f Makefile_STM32 flash-openocd
```

## Step 5: Verify STM32 Operation

After flashing:
1. LED on PA5 should blink 3 times (startup indication)
2. LED turns off and waits for I2C data
3. When 0xAA is received, LED toggles

═══════════════════════════════════════════════════════════════════
PART 3: TESTING THE COMPLETE SYSTEM
═══════════════════════════════════════════════════════════════════

## Step 1: Power On Sequence

1. Connect all hardware (I2C wires between RPi and STM32)
2. Power on STM32 (via USB or ST-Link)
3. Power on Raspberry Pi
4. Wait for both to boot

## Step 2: Verify I2C Connection

```bash
# Scan I2C bus - STM32 should appear at 0x30
sudo i2cdetect -y 1

# Expected output:
#      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
# 00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
# 10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
# 20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
# 30: 30 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
# 40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
# ...
```

## Step 3: Load Driver and Run Test

```bash
# Load the driver
sudo insmod i2c_char_driver.ko

# Check device
ls -l /dev/i2c_stm32

# Run test application
sudo ./test_i2c

# Expected output:
# I2C Test Application
# ====================
# 
# Opening device /dev/i2c_stm32...
# Device opened successfully!
# 
# Sending data 0xAA to STM32...
# Data sent successfully! (1 bytes)
# 
# Device closed.
# Check STM32 LED - it should toggle when 0xAA is received!
```

## Step 4: Observe Results

**On STM32:**
- LED (PA5) should toggle each time 0xAA is received
- LED turns ON when I2C address is matched
- LED turns OFF when STOP condition is detected

**On Raspberry Pi:**
```bash
# Check kernel messages
dmesg | tail -30

# You should see:
# I2C write successful, sent 1 bytes
# Wrote 1 bytes to STM32
```

## Step 5: Manual Testing with i2cset

```bash
# Send 0xAA to STM32 directly using i2cset
sudo i2cset -y 1 0x30 0xAA

# STM32 LED should toggle!

# Send multiple bytes
sudo i2cset -y 1 0x30 0xAA 0x55 0x12 i

# First byte (0xAA) will toggle LED
```

═══════════════════════════════════════════════════════════════════
TROUBLESHOOTING
═══════════════════════════════════════════════════════════════════

## Problem: i2cdetect doesn't show device at 0x30

**Solutions:**
1. Check physical connections (SDA, SCL, GND)
2. Verify STM32 is powered and running
3. Check pull-up resistors on I2C lines
4. Verify STM32 code is flashed correctly
5. Reset STM32 and scan again

```bash
# Try slower I2C speed
sudo i2cdetect -y 1 -r  # Use read byte method
```

## Problem: Driver fails to load

**Solutions:**
1. Check kernel headers match running kernel:
```bash
uname -r
ls /lib/modules/
```

2. Rebuild with correct headers:
```bash
make clean
make
```

3. Check for module conflicts:
```bash
lsmod | grep i2c
```

## Problem: Permission denied when accessing /dev/i2c_stm32

**Solutions:**
```bash
# Add user to i2c group
sudo usermod -a -G i2c $USER

# Change device permissions
sudo chmod 666 /dev/i2c_stm32

# Or run with sudo
sudo ./test_i2c
```

## Problem: STM32 LED doesn't toggle

**Solutions:**
1. Verify correct data is being sent (0xAA)
2. Check STM32 code is running (startup blinks)
3. Use logic analyzer to verify I2C signals
4. Add debug print to STM32 code (if UART available)
5. Check I2C pull-up resistors

## Problem: I2C communication errors

**Solutions:**
```bash
# Check for I2C errors
dmesg | grep -i i2c

# Reduce I2C speed in device tree (change to 10kHz):
clock-frequency = <10000>;

# Rebuild and reload overlay
```

═══════════════════════════════════════════════════════════════════
ADVANCED TESTING
═══════════════════════════════════════════════════════════════════

## Continuous Testing Script

Create a script to continuously send data:

```bash
#!/bin/bash
# continuous_test.sh

echo "Continuous I2C Test - Press Ctrl+C to stop"
count=0

while true; do
    sudo ./test_i2c
    count=$((count + 1))
    echo "Test #$count completed"
    sleep 1
done
```

## Using Python for Testing

```python
#!/usr/bin/env python3
import time

# Open device
with open('/dev/i2c_stm32', 'wb') as dev:
    while True:
        # Send 0xAA
        dev.write(bytes([0xAA]))
        dev.flush()
        print("Sent 0xAA")
        time.sleep(1)
```

═══════════════════════════════════════════════════════════════════
CLEANUP AND REMOVAL
═══════════════════════════════════════════════════════════════════

## Unload Driver

```bash
# Remove module
sudo rmmod i2c_char_driver

# Verify removal
lsmod | grep i2c_char

# Check messages
dmesg | tail -10
```

## Remove Device Tree Overlay

```bash
# Edit config.txt
sudo nano /boot/config.txt

# Remove or comment out:
# dtoverlay=i2c1-stm32-overlay

# Reboot
sudo reboot
```

═══════════════════════════════════════════════════════════════════
ADDITIONAL NOTES
═══════════════════════════════════════════════════════════════════

## I2C Bus Specifications

- **Standard Mode:** 100 kHz (default in code)
- **Fast Mode:** 400 kHz (optional)
- **Slave Address:** 0x30 (7-bit)
- **Pull-up Resistors:** 4.7kΩ recommended

## Code Customization

### Change I2C Slave Address:
In `i2c_char_driver.c`:
```c
#define STM32_I2C_ADDR 0x30  // Change to desired address
```

In `stm32_i2c_slave.c`:
```c
I2C1_OAR1 = (0x30 << 1);  // Change to match driver
```

### Change I2C Speed:
In `i2c1-stm32-overlay.dts`:
```
clock-frequency = <100000>;  // 100kHz
// or
clock-frequency = <400000>;  // 400kHz for fast mode
```

## Performance Tips

1. Use external pull-up resistors for better signal integrity
2. Keep I2C wires short (<30cm recommended)
3. Avoid running I2C wires parallel to power lines
4. Use twisted pair for SDA/SCL if distance >30cm
5. Add 100nF capacitors near STM32 VDD pins

═══════════════════════════════════════════════════════════════════
REFERENCE DOCUMENTATION
═══════════════════════════════════════════════════════════════════

- STM32F401RE Reference Manual: RM0368
- STM32F401RE Datasheet: DS9716
- I2C Specification: NXP UM10204
- Raspberry Pi I2C: https://www.raspberrypi.org/documentation/
- Linux I2C: https://www.kernel.org/doc/Documentation/i2c/

Good luck with your I2C communication project!
