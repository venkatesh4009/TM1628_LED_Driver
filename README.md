# TM1628 7‑Segment LED Display Kernel Driver for i.MX93

This repository provides a Linux **kernel driver** for the **TM1628** 7-segment LED and keypad controller IC, developed and tested on the **NXP FRDM-i.MX93** board.

The TM1628 driver enables:
- Display control of **8-digit 7-segment LED**
- Keypad scanning of **16 keys (4x4 matrix)**
- GPIO-based bit-banging protocol (CLK, DIO, STB)

---

## 📦 Files Included

tm1628_docs/
├── Kernel_Driver_tm1628/
│ ├── driver_tm1628.c # Main kernel driver
│ ├── tm1628_keys.c # Keypad scan logic
│ └── dts.txt # Device tree snippet
├── TM1628 7‑Segment LED Driver Code.pdf # Developer documentation
├── TM1628_V1.1_EN.pdf # Official TM1628 datasheet
└── tm1628_dts.txt # Additional DTS sample



---

## 🛠️ Step-by-Step: How to Use the Driver

### 1️⃣ Add TM1628 Node in Device Tree

Edit your board DTS file (e.g. `imx93-frdm.dts`) and include this node:

```dts
tm1628@0 {
    compatible = "essae,tm1628";
    clk-gpio = <&gpio3 18 GPIO_ACTIVE_HIGH>;
    dio-gpio = <&gpio3 20 GPIO_ACTIVE_HIGH>;
    stb-gpio = <&gpio3 19 GPIO_ACTIVE_HIGH>;
};
💡 Use the correct GPIO lines from your hardware. See tm1628_dts.txt for reference.

2️⃣ Build the Kernel Module
Move into the driver folder:


cd Kernel_Driver_tm1628/
Build against your current kernel:


make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
3️⃣ Insert the Driver into Kernel

sudo insmod driver_tm1628.ko
Verify driver loaded:


dmesg | grep tm1628
4️⃣ Auto-load with System Boot (Optional)
To make the driver auto-load at boot:

Copy .ko file to kernel modules:


sudo cp driver_tm1628.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
Add module name to /etc/modules:


echo "driver_tm1628" | sudo tee -a /etc/modules
Update module dependencies:


sudo depmod -a
🔧 Hardware Setup
Signal	TM1628 Pin	Connected GPIO (i.MX93)
CLK	SCLK	GPIO3_18
DIO	Data	GPIO3_20
STB	Strobe	GPIO3_19

🔌 Connect the TM1628 correctly to GPIOs. Bit-banged protocol used in driver.

🧾 References
TM1628 Datasheet (V1.1)

Project Document PDF

Kernel Driver: driver_tm1628.c

👨‍💻 Author
Venkatesh M
Embedded Software Engineer – Essae-Teraoka
