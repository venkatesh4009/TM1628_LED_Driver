📁 Directory Structure
bash
Copy
Edit
tm1628_driver/
├── Kernel_Driver_tm1628/
│   ├── tm1628.c          # Main kernel driver (renamed from driver_tm1628.c)
│   ├── tm1628_keys.c     # Keypad handling logic
│   └── dts.txt           # Device Tree snippet for GPIO config
├── tm1628_dts.txt        # Additional DTS sample (optional)
├── TM1628_V1.1_EN.pdf    # Official TM1628 datasheet
└── TM1628_Driver_Guide.pdf  # Project setup and internal logic explained
🛠️ How to Use This Driver
✅ 1. Add TM1628 Node in Device Tree
Edit your board's DTS file (imx93-*.dts) and insert the following node:


tm1628@0 {
    compatible = "essae,tm1628";
    clk-gpio = <&gpio3 18 GPIO_ACTIVE_HIGH>;
    dio-gpio = <&gpio3 20 GPIO_ACTIVE_HIGH>;
    stb-gpio = <&gpio3 19 GPIO_ACTIVE_HIGH>;
};
📌 Match GPIO lines according to your hardware setup.

✅ 2. Build the Kernel Module
Navigate to the driver folder and run:


cd Kernel_Driver_tm1628/
make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
✅ 3. Insert the Driver into Kernel
bash
Copy
Edit
sudo insmod tm1628.ko
dmesg | grep tm1628
✅ 4. Auto-Load Driver at Boot (Optional)
bash
Copy
Edit
# Copy module to system path
sudo cp tm1628.ko /lib/modules/$(uname -r)/kernel/drivers/misc/

# Register it for boot
echo "tm1628" | sudo tee -a /etc/modules

# Update module dependency list
sudo depmod -a
🔧 Hardware Wiring (i.MX93 GPIO to TM1628)
Signal	TM1628 Pin	i.MX93 GPIO
CLK	SCLK	GPIO3_IO18 (18)
DIO	Data	GPIO3_IO20 (20)
STB	Strobe	GPIO3_IO19 (19)

Ensure the TM1628 is powered and grounded properly.

⚙️ Kernel Integration (Optional)
To integrate this driver into a custom kernel build:

📄 Kconfig
kconfig
Copy
Edit
config LEDS_TM1628
    tristate "TM1628 LED driver over GPIO"
    default m
    help
      This driver supports the TM1628 7-segment LED and key controller.
      Communication is done via GPIO bit-banging.
📄 Makefile
makefile
Copy
Edit
obj-$(CONFIG_LEDS_TM1628) += tm1628.o
📚 References
TM1628 Datasheet

Driver Code Documentation

DTS Example

👤 Author
Venkatesh M
Embedded Software Engineer at Essae-Teraoka
