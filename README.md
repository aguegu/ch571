@aguegu/ch581
===

**Firmware Development Environment for CH571/CH572/CH573 on Linux**

>  **Non-Commercial License**: This project is free to use for educational and personal purposes. Commercial use is **not permitted** without explicit written permission. See the `LICENSE` file for details.

This repository provides a quick and minimal Linux-based development environment for WCH's CH571/CH572/CH573 microcontrollers—without relying on `Mounriver Studio`.

Since CH571, CH572, and CH573 share common datasheets and official EVT examples, this environment should work with all three.

---

## 1. Setup Toolchains

### 1.1 Install GCC Toolchain

Download `MRS_Toolchain_Linux_x64_V210.tar.xz` from [Mounriver](http://mounriver.com/download), extract it, and create a symbolic link:

```bash
mkdir tools
cd tools
wget http://file-oss.mounriver.com/tools/MRS_Toolchain_Linux_x64_V210.tar.xz
tar xJf MRS_Toolchain_Linux_x64_V210.tar.xz
ln -s RISC-V\ Embedded\ GCC12/ ../riscv-embedded-gcc
````

### 1.2 Install USB ISP Driver

Download and extract the **WCHISPTool\_CMD** from [WCH’s website](https://www.wch.cn/downloads/WCHISPTool_CMD_ZIP.html):

```bash
cd tools
wget https://www.wch.cn/download/file?id=424 -O WCHISPTool_CMD.ZIP
# Extract manually or via GUI
ln -s WCHISPTool_CMD/Linux/bin/x64/WCHISPTool_CMD ..
```

#### Compile and Install Driver

> Requires `linux-headers` for your current kernel (e.g., `linux610-headers`).

**Note**: On rolling-release distros like Arch, you’ll need to recompile and install the driver after kernel updates.

```bash
cd tools/WCHISPTool_CMD/Linux/driver
sudo make install
cd ../../../..
```

### 1.3 official WCH ch573 EVT from https://www.wch.cn/downloads/CH573EVT_ZIP.html

```bash
cd tools
wget https://www.wch.cn/download/file?id=337 -O CH573EVT.ZIP
unzip CH573EVT.ZIP
mv EVT ..
```

the examples would refer to libraries and headers in it to compile.

## 2. Compile Demo Code

Navigate to one of the `examples`, such as `gpio-toggle`:

```bash
cd examples/gpio-toggle
make
```

This will generate a `.hex` file in the `dist/` folder.

## 3. Flash in ISP Mode

To flash the firmware, the chip must be in ISP mode. On the **[SuperMini CH57x EVT board](doc/supermini_ch57x.png)**, follow these steps:

  3.1. **Press and hold** the onboard `Download` button.

  3.2 **Connect** the USB-C port to your PC using a standard USB-A to USB-C cable.

   * If the connection is correct, a USB device should appear:

     ```bash
     lsusb
     Bus XXX Device YYY: ID 4348:55e0 WinChipHead
     ```

   * If the driver installed correctly, check:

     ```bash
     ls -l /dev/ch37*
     crw------- 1 root root 180, 200 Nov  1 16:24 /dev/ch37x0
     ```

   > The MCU stays in ISP mode for only **\~5 seconds**.

   3.3. During this short window, run:

```bash
make flash
```

Expected output:

```bash
sudo ../../isp/WCHISPTool_CMD -p /dev/ch37x0 -c ../../isp/config.ini -o program -f dist/gpio-toggle.hex

=====ISP_Command_Tool=====
TOOL VERSION:  V3.60

{"Device":"/dev/ch37x0","Status":"Ready"}
{"Device":"/dev/ch37x0", "Status":"Programming", "Progress":100%}
{"Device":"/dev/ch37x0", "Status":"Finished", "Code":0,"Message":"Succeed"}
```

  3.4. If you want to flash it again, the supermini ch57x needs to be unplugged (to power off) and restart from step 3.1.

---

Enjoy and have fun.
