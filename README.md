<p align="center">
  <a href="https://locha.io/">
  <img height="200px" src="doc/LogotipoTurpial-Color.20-09-19.svg">
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
pinout is yet to be decided in the final complete board design. See the
[radio-firmware](https://github.com/btcven/radio-firmware) repository for more
information on the radio module we use.

## Table of Contents

[Turpial Firmware](#turpial-firmware)

[About Locha Mesh](#about-locha-mesh)

[Sponsor](#sponsor)

[Supported ESP32 boards](#supported-esp32-boards)

* [Getting Started](#getting-started)
   - [Development Workflow.](#development-workflow)
   - [Installing ESP-IDF.](#installing-esp-idf)
   - [Compiling.](#compiling)
   - [Flashing the Firmware](#flashing-the-firmware)
   - [Running Tests](#running-tests)
   - [Compiling Documentation.](#compiling-documentation)

[License](#license)

## Getting Started

Since this is an open source project you can contribute with your code and
knowledge, or by making changes to the documentation. We invite and welcome all contributions. Below is a brief
description on how to work with the code and to compile

### Development Workflow

Development happens in the `dev` branch. All Pull-Requests should be
pointed to that branch. Please, make sure you follow the
[CONTRIBUTING.md](blob/master/CONTRIBUTING.md) guidelines. All Pull-Requests
require that at least two developers review them first before merging to `dev`
branch.

### Installing ESP-IDF

In order to compile the firmware (and to flash it) you need to install the
[ESP-IDF](https://github.com/espressif/esp-idf/) development framework, which
brings the compiler and necessary tools in order to correctly build the
project.

### Compiling

We use the idf.py command line tool provided by ESP-IDF to build the project.
It's pretty simple to build the project:

```bash
idf.py build
```

### Flashing the Firmware

To upload the binary to the ESP32 you have connected to your computer, you
need to use the following command:

```bash
idf.py -p PORT flash
```

Where `PORT` specifies on what port you have connected your ESP32 development
board.

### Running Tests

To ensure the safety of code we run some tests in the ESP32 to ensure the code
works as expected. The testing code lives in the `test/` directory. To compile
a test you can run the following command:

```bash
cd test/
idf.py build -D "TEST_COMPONENTS=yourcomponent"
```

You can change `yourcomponent` to the test component you want to run tests for.
Don't forget to flash the test binary to run the code. See
[*Flashing the binary.*](#flashing-the-binary).

### Compiling Documentation

You can compile the code documentation by installing the following tools:

1. [Doxygen](http://www.doxygen.nl/download.html)
2. [Python 3](https://www.python.org/downloads/)
3. `pip install -U sphinx`
4. `pip install -U sphinx-rtd-theme`
5. `pip install -U breathe`

Now you can just checkout to the `doc/` directory and run:

```bash
make html
```

The documentation will reside in `doc/build/_html/`, from there you can open
`index.html`.

## License
Copyright © 2019 **Bitcoin Venezuela** and **Locha Mesh** developers.

Licensed under the **Apache License, Version 2.0**

---
**A text quote is shown below**

>Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
___
Read the full text:
[Locha Mesh Apache License 2.0](LICENSE)
