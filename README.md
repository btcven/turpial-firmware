<p align="center">
  <a href="https://locha.io/">
  <img height="200px" src="doc/LogotipoTurpial-Color.20-09-19.svg">
  </a>
  <br>
  <a href="https://travis-ci.com/btcven/turpial-firmware">
    <img src="https://travis-ci.com/btcven/turpial-firmware.svg?branch=master" title="Build Status">
  </a>
</p>

<h1 align="center">Turpial Firmware</h1>

We are happy of your visit and that you can read more about us. Here you can
find the main firmware for your device compatible with Locha Mesh and be aware
of our development process.

## What's Locha Mesh

The Locha Mesh network is a secure radio network for text messaging and bitcoin
transactions. The main objetive is a long range network for everyone and
everywhere, for this reason, we are working not only in a protocol, also the
firmware for affordable devices like our "Turpial".

If you want to learn more about Locha Mesh feel free to read
[this repository](https://github.com/btcven/locha) or take a look at our website
[locha.io](https://www.locha.io).

## Development workflow

The development code is located on the `dev` branch, all the development
happens there and all of the Pull-Request should be pointed to that branch.
Make sure your Pull-Request follows the CONTRIBUITING.md guidelines.

## Compiling and Flashing

To compile the firmware (and to flash it) we use the
[Platform IO IDE](https://platformio.org/), it's available as a
[VS Code Extension](https://platformio.org/platformio-ide) primarily, however
it can be used as a standalone CLI too without the need to install another
editor, please refer to the Platform IO website to see supported installation
methods.

For example to build your project you can run this command on a CLI that has
PlatformIO:

```bash
pio run -e featheresp32 --target upload
```

That command will build the project and upload the binary to the ESP32 you have
connected, the `-e featherep32` specifies the board model you have. Currently
the supported boards are:

- `featheresp32`
- `wemosbat`
- `esp32doit-devkit-v1`

If your ESP32 development board isn't listed here you can modify the
`platformio.ini` file and submit a Pull-Request adding the board.

## Running tests

To ensure the safety of code we run some tests in the ESP32 to ensure the code
works as expected, the testing code lives in the `test/` directory. To compile
a test you can run the following command:

```bash
pio test -e featheresp23 -f test_wifi
```

Please note that you can change `featheresp32` for the board of your choice,
and you can change `-f test_wifi` to the test you want to run, or you can
remove it to run all tests.

## Compiling documentation

You can compile the code documentation by installing the following tools:

1. [Doxygen](http://www.doxygen.nl/download.html)
2. [Python 3](https://www.python.org/downloads/)
3. `pip -U install sphinx`
4. `pip -U install breathe`

Now you can just checkout to the `doc/` directory and run:

```bash
make html
```

The documentation will reside in `doc/build/_html/`, from there you can open
`index.html`.

## License
Copyright © 2019 Bitcoin Venezuela and Locha Mesh developers.

Licensed under the **Apache License, Version 2.0**

---
**A text quote is shown below**

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
___
Read the full text:
[Locha Mesh Apache License 2.0](LICENSE)
