# TM1628 7‑Segment LED Display Kernel Driver for NXP i.MX93

This repository contains a Linux **kernel driver** for the **TM1628** 7-segment LED and keypad controller IC, designed for use with the **NXP FRDM-i.MX93** board.

## ✨ Features

- Control of 8-digit 7-segment LED display
- Keypad scanning support (4x4 matrix)
- GPIO-based bit-banging protocol (CLK, DIO, STB)
- Lightweight and modular Linux kernel driver

---

## 📁 Directory Structure

```text
tm1628_driver/
├── Kernel_Driver_tm1628/
│ ├── tm1628.c # Main kernel driver source
│ ├── Kconfig & Makefile
│ └── dts.txt # Device Tree snippet
├── tm1628_dts.txt # Extra DTS sample
├── TM1628_V1.1_EN.pdf # Official datasheet
└── TM1628_Driver_Guide.pdf # Project documentation
```

### 🛠️ How to Use This Driver

### 1️⃣ Add TM1628 Node in Device Tree

```bash
Edit your board’s DTS file (`imx93-*.dts`) and add:

tm1628@0 {
    compatible = "essae,tm1628";
    clk-gpio = <&gpio3 18 GPIO_ACTIVE_HIGH>;
    dio-gpio = <&gpio3 20 GPIO_ACTIVE_HIGH>;
    stb-gpio = <&gpio3 19 GPIO_ACTIVE_HIGH>;
};
✅ Ensure the GPIO pins match your hardware connections.
```
### 2️⃣ Build the Kernel Module

```bash
cd Kernel_Driver_tm1628/
make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
```

### 3️⃣ Insert the Module

```bash
sudo insmod tm1628.ko
dmesg | grep tm1628
```

### 4️⃣ Auto-Load at Boot (Optional)

```bash
# Move to kernel driver path
sudo cp tm1628.ko /lib/modules/$(uname -r)/kernel/drivers/misc/

# Register module to load at boot
echo "tm1628" | sudo tee -a /etc/modules

# Rebuild module dependency list
sudo depmod -a
```

### 🔌 Hardware Wiring
```text
Signal	TM1628 Pin	i.MX93 GPIO
CLK	SCLK	GPIO3_IO18
DIO	Data	GPIO3_IO20
STB	Strobe	GPIO3_IO19
```

### ✅ Ensure proper 3.3V or 5V supply and common GND

### ⚙️ Kernel Integration (Optional for Yocto/Linux Kernel)

### kconfig
```bash

config LEDS_TM1628
    tristate "TM1628 LED driver"
    default m
    help
      Kernel driver for TM1628 7-segment LED and keypad controller.
      Uses GPIO bit-banging for communication.
```

### makefile

```bash
obj-$(CONFIG_LEDS_TM1628) += tm1628.o
```

### 📚 References

- TM1628 Datasheet
- Driver Setup Guide

---

👨‍💼 Author

Developed by:

Venkatesh M – venkatesh.muninagaraju@essae.com

Embedded System Engineer

---

