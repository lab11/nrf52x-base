Nordic nRF52x Support Files
==========================
[![Build Status](https://travis-ci.com/lab11/nrf52x-base.svg?branch=master)](https://travis-ci.com/lab11/nrf52x-base)

This repository is a starting point and shared code for Nordic nRF52x BLE/Thread platforms. This repo is
a collection of libraries, SDKs, Softdevices, and Makefiles to be included
within other projects using the Nordic platfroms. Pull requests welcome.

The currently supported SDK versions are: 15.3.0, 16.0.0, and the SDK for Thread and Zigbee v3.2.0 and v4.0.0

The currently supported Softdevice versions are:
s132_6.1.1, 7.0.1 and s140_6.1.1, 7.0.1

Support for nRF51x devices are deprecated and not maintained. The old version
of this repository with support for nRF51x devices can be found in the
[`legacy-nrf51x`](https://github.com/lab11/nrf5x-base/tree/legacy-nrf51x)
branch on the original `nrf5x-base` repo.

Usage
-----
First, install [git-lfs](https://github.com/git-lfs/git-lfs/wiki/Installation)
to properly download the CMSIS libraries. When adding the submodule, or cloning
your project with an nrf52x-base submodule, you may need to navigate to
`nrf52x-base/lib/CMSIS_5/` and run `git lfs pull`.

Next, add this project as a submodule inside of your repo with your
nRF5x code.

    git submodule add https://github.com/lab11/nrf52x-base

Then write an application for the nRF5x SoC you are using and include
a Makefile that looks like this:

```make
PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52832
SDK_VERSION = 15
SOFTDEVICE_MODEL = s132

# Source and header files
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .
APP_SOURCES = $(notdir $(wildcard ./*.c))

# Include board Makefile (if any)
#include $(NRF_BASE_DIR)/boards/<BOARD_NAME_HERE>

# Include main Makefile
NRF_BASE_DIR ?= ../../
include $(NRF_BASE_DIR)/make/AppMakefile.mk
```

Generally, the expected directory structure for your project is:
```
    /apps
        /<application 1>
        /<application 2>
        ...
    /src
        various platform-level code (e.g. functions shared between applications)
    /include
        various platform-level headers (e.g. platform pin mappings)
    /nrf5x-base (submodule)
```

Example Applications
--------------------

This repo has several example and test applications. See the
[apps](https://github.com/lab11/nrf5x-base/tree/master/apps)
folder.

Flash an Application
--------------------

To build and flash an application, there is some setup
you must do.

1. Install the [gcc-arm-none-eabi toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

    On Ubuntu:

        cd /tmp \
        && wget -c https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 \
        && tar xjf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 \
        && sudo mv gcc-arm-none-eabi-9-2020-q2-update /opt/gcc-arm-none-eabi-9-2020-q2-update \
        && rm gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 \
        && sudo ln -s /opt/gcc-arm-none-eabi-9-2020-q2-update/bin/* /usr/local/bin/.

    On MacOS:

        brew tap ArmMbed/homebrew-formulae
        brew install arm-none-eabi-gcc

2. Install the JLink [software](https://www.segger.com/jlink-software.html)
for your platform. You want the "Software and documentation pack". As of April
2020, some of the more recent JLink software is unstable on Linux. Version
6.34b has been verified to work reliably on Linux and MacOS.

3. Acquire a [JLink JTAG programmer](https://www.segger.com/jlink-general-info.html).
The "EDU" edition works fine.

4. Program an app! With the JLink box attached to the target board:

        make flash

    will write the app and softdevice to the device. You can erase
    a chip with:

        make erase-all

    See the [make](https://github.com/lab11/nrf5x-base/tree/master/make) folder
    for a complete list of commands.

    Most of our boards use a [TagConnect header](http://www.tag-connect.com/TC2030-IDC-NL)
    instead of the way-too-large ARM JTAG header. We use [our own](https://github.com/lab11/jtag-tagconnect)
    adapter, but Segger also makes [one](https://www.segger.com/jlink-6-pin-needle-adapter.html).

Git Submodules
--------------

If you're using submodules in your project, you may want to use this to make
git automatically update them:
https://gist.github.com/brghena/fc4483a2df83c47660a5

Other Features
--------------

The build system supports generating protobuf \*.c/\*.h files with
[nanopb](https://github.com/lab11/nrf5x-base/tree/master/lib/nanopb/). To
enable this, you must install the protobuf compiler `protoc` and build nanopb.

On Ubuntu:
```
sudo apt install python python3 libprotobuf-dev libprotoc-dev protobuf-compiler python3-protobuf
```
On Mac Os:
```
brew install protobuf
```

Build nanopb:
```
cd nrf52x-base/lib/nanopb/generator/proto
make
```

BLE Tools for Other Platforms
-----------------

When developing a BLE application, several tools exist to make your life easier.
The easiest option is nRF Connect: [for Android](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_US)
and for [iOS](https://apps.apple.com/us/app/nrf-connect/id1054362403)
Alternatively,
[noble](https://github.com/abandonware/noble) is a NodeJS library for interacting with BLE that can run from
a Linux or Mac computer.

License
-------

The files in this repository are licensed under the [MIT License](LICENSE)
unless otherwise noted by the local directory's README and license files.

