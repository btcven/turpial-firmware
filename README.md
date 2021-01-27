<p align="center">
  <a href="https://locha.io/">
  <img height="200px" src="doc/logo.png">
  </a>
  <br>
  <a href="https://travis-ci.com/btcven/turpial-firmware">
    <img src="https://travis-ci.com/btcven/turpial-firmware.svg?branch=master" title="Build Status">
  </a>
</p>

<p align="center">
  <a href="https://locha.io/">Project Website</a> |
  <a href="https://btcven.github.io/turpial-firmware/">Code Documentation</a> |
  <a href="https://locha.io/donate">Donate</a> |
  <a href="https://github.com/sponsors/rdymac">Sponsor</a> |
  <a href="https://locha.io/buy">Buy</a>
</p>

<h1 align="center">Turpial Firmware</h1>

Welcome, and thank you for your visit. Here you can
find the main firmware for the Turpial device, compatible with Locha Mesh and be aware
of our development process.

## About Locha Mesh

The Locha Mesh network is a secure radio network for off-grid messaging, Bitcoin and other
cryptocurrencies transactions without access to the Internet. The main objective is a
long range network for everyone and
everywhere. For this reason, we are working not only in a protocol, but also in the
firmware for affordable devices like our **Turpial**.

If you want to learn more about Locha Mesh, feel free to read
[the Locha Mesh main repository](https://github.com/btcven/locha) or take a
look at our website [locha.io](https://www.locha.io).

## Sponsor

If you want to support this project you can make a donation to the Locha Mesh
effort to build private censorship-resistant mesh network devices for Bitcoin and other
cryptocurrencies transactions without Internet.

Here are some places if you want to support us:

- Donate: https://locha.io/donate
- Buy Turpial devices: https://locha.io/buy

## Supported ESP32 boards

Currently we use the [ESP-WROVER-KIT](https://www.espressif.com/en/products/hardware/esp-wrover-kit/overview)
development board to test the code and the radio module, although other ESP32
boards can be used as long as it fits on the flash memory. It should work on
both the WROOM and WROVER chips without changes on the code.

The board connects over UART to the
[SimpleLink CC1312R](https://www.ti.com/product/CC1312R) radio module, the
pinout is yet to be decided in the final complete board design.

## Table of Contents

[Turpial Firmware](#turpial-firmware)

[About Locha Mesh](#about-locha-mesh)

[Sponsor](#sponsor)

[Supported ESP32 boards](#supported-esp32-boards)

* [Getting Started](#getting-started)
   - [Development Workflow.](#development-workflow)
   - [Installing the toolchain.](#installing-the-toolchain)
   - [Clone this repository](#clone-this-repository)
   - [Compiling.](#compiling)
   - [Flashing the Firmware](#flashing-the-firmware)

[License](#license)

## Getting Started

Since this is an open source project you can contribute with your code and
knowledge, or by making changes to the documentation. We invite and welcome
all contributions. Below is a brief description on how to work with the code
and to compile

- :warning: _The code is evolving each day, if something doesn't work when
compiling please file and issue and we'll solve it_.

### Development Workflow

Development happens in the `master` branch. All Pull Requests should be pointed
to that branch. Please, make sure you follow the [CONTRIBUTING.md](CONTRIBUTING.md)
guidelines. All Pull Requests require that at least two developers review them
first before merging.

### Installing the toolchain

The firmware is based on RIOT-OS, it's a real-time multi-threading operating
system that supports a range of devices that are typically found in the
Internet of Things (IoT): 8-bit, 16-bit and 32-bit microcontrollers.

We use it for the CC1312 and ESP32 microcontrollers.

Currently only it is only compatible with Linux, we highly recommend you to
use Debian based distributions, although the compilation process might work
for on other distributions, if it doesn't works, please file an issue
on this repository, or provide a pull request, be perpared to touch some
files. For Mac OS you can search for alternatives like docker, or install
all the necessary tools to build the firmware.

First, you need to install the ESP32 toolchain to build the firmware,
not to be confused to the one provided by Espressif (ESP-IDF). Please follow
[this guide](https://doc.riot-os.org/group__cpu__esp32.html#esp32_toolchain)
to install the necessary tools on your Linux system.

To install the development tools for building the CC1312 firmware you will
need to follow [this guide](https://doc.riot-os.org/group__boards__cc1312__launchpad.html),
see especially the _Flashing and debugging_ section on the guide.

You have to install the ARM-GCC toolchain to build the CC1312 firmware, follow
[this guide](https://github.com/RIOT-OS/RIOT/wiki/Family:-ARM) to install
the toolchain for your operating system.

### Clone this repository

```
git clone https://github.com/btcven/turpial-firmware.git
```

### Compiling

 1. Enter the firmware directory: `cd turpial-firmware`
 2. Update the submodules: `git submodule update --init --recurisve`
 3. Build the mesh firmware (for the CC1312): `make -C mesh BOARD=...`,
    where `BOARD` is the board you are using, for example, the CC1312
    LaunchPad provided by Texas Instruments, is `cc1312-launchpad`.
    For example: `make -C mesh BOARD=cc1312-launchpad`.
 4. Build the wifi firmware (for the ESP32): `make -C wifi BOARD=...`,
    where `BOARD` is the board you are using, for a generic ESP32 WROOM
    board, use `esp32-wroom-32`.
    For example: `make -C wifi BOARD=esp32-wroom-32`.


### Flashing the firmware

To upload the binary to the ESP32 you have connected to your computer, you
need to use the following command:

```bash
make -C wifi flash PORT=/dev/ttyUSB0
```

Where `PORT` specifies on what port you have connected your ESP32 development
board.

To upload the binary to the CC1312, you need to use the following command:

```bash
make -C mesh flash PROGRAMMER=uniflash
```

If you have installed TI version of OpenOCD, use `PROGRAMMER=openocd`.

See also [btcven/ti-bootloader](https://github.com/btcven/ti-bootloader),
to flash the firmware onto the CC1312 using the serial port only. And
not through JTAG.

## License

```
Copyright 2021 btcven and Locha Mesh developers

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

Read the full text:

[Apache License 2.0](LICENSE)
