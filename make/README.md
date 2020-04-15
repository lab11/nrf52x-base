nRF5x Make System
=================

This folder contains the GCC build system for the nRF5x platforms. It
originally comes from
[hlnd/nrf51-pure-gcc-setup](https://github.com/hlnd/nrf51-pure-gcc-setup).

We develop on Linux. This also works on Windows in either of the following ways:
1. Build on WSL (Windows Subsystem for Linux), and `flash` with WSL.
2. Build with SES (Segger Embedded Studio) and `flash` with WSL. Note: For this you will have to manually move the `.hex` file generated from SES to `_build/` directory. For e.g. `mv [your_application]/pca10040/s132/ses/Output/Release/Exe/[your_application.hex]  [your_application]/_build/[your_application.hex]`.

The currently supported SDK versions are: 9, 10, 11

The currently supported Softdevice versions are: s110_8.0.0, s120_2.1.0, and s130_2.0.0

Things to Install
-----------------
1. `gcc-arm-none-eabi`: https://launchpad.net/gcc-arm-embedded
2. gdb-arm-none-eabi
3. The [jlink tools](https://www.segger.com/jlink-software.html) for linux / windows for WSL
4. The jlink debuger for linux
5. The [nRF-Command-Line-Tools for Windows](https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF5-Command-Line-Tools/Download#infotabs)

Usage
-----

See the `apps/` folder for an example of how to use this build system. An
example Makefile is in the root of this repository.

Additionally, you can customize things inside of your application makefile.
The following variables can all be set:

These are required:
- `APPLICATION_SRCS`   : List of all .c files to be compiled.
- `LIBRARY_PATHS`      : List of directories with .h files
- `SOURCE_PATHS`       : List of directories with .c files
- `SOFTDEVICE_MODEL`   : s110 | s120 | s130 | s210 | s310 | or do not set for no softdevice

These are optional
- `SOFTDEVICE_VERSION` : Full version number of the softdevice.
- `SOFTDEVICE`         : Path to the softdevice to use
- `SERIALIZATION_MODE` : "connectivity" or "application" (only define if using serialization module)
- `START_CODE`         : .s file to execute first
- `SYSTEM_FILE`        : Base nRF .c file.
- `NRF_MODEL`          : nrf51 | nrf52  : Set by the softdevice used
- `BOARD`              : Hardware board being used. Will get set as #define.
- `RAM_KB`             : Size of RAM on chip    : Defaults to 16
- `FLASH_KB`           : Size of flash on chip  : Defaults to 256
- `SDK_VERSION`        : Major version number of the SDK to use. Defaults to 10
- `GDB_PORT_NUMBER`    : Defaults to 2331

If you want to use the GDB functionality with multiple J-Links, you should
make sure that all projects have a unique GDB port number defined in their
project Makefile.


Targets
-------
Most of the targets provided should be self explanatory, but some may use some
extra explanation:

- `flash`

    Build project and flash onto a chip. Also checks that the correct softdevice is
    already on the chip, and automatically runs flash-softdevice if not.

- `flash ID=XX:XX:XX:XX:XX:XX`

    Sets the Bluetooth ID for the chip to whatever replaces XX:XX:XX:XX:XX:XX (must
    be valid hex digits). Bluetooth ID is written to the top of flash and persists
    across future flashes (but not erase-alls). Application code needs to read the
    value from flash and actually set it as the Bluetooth ID.

- `erase-all`

    Does an erase all of a chip.

- `flash-softdevice`

    Used to flash a softdevice to a chip. (Note, this is done automatically by
    make flash). Flashes softdevice as specified in the project Makefile.

- `flash-softdevice SOFTDEVICE=<PATH_TO_SOFTDEVICE_WITHOUT_SPACES>`

    Flashes a specific version of the softdevice. The path to the softdevice hex
    needs to be without spaces, due to Make limitations.

- `debug`

    Makes with debug symbols. Use before startdebug.

- `startdebug`

    Starts a J-Link GDB Server in a separate terminal window, and then GDB
    also in a separate window. If you change the code, you can then make directly
    from gdb, and do load to run the new code.

    To use this feature you must enable the working path as safe! you can also
    enable all paths by adding 'set auto-load safe-path /' to ~/.gdbinit.

- `recover`

    Provides equal functionality to that of nrfjprog / nRFgo Studio on Windows.


If you have multiple J-Links connected to your system, you should
set the `SEGGER_SERIAL` variable to the serial number of your J-Link, so that
the programming Just Works (tm). It seems Segger on Linux isn't capable of
giving you a selection of J-Links, as on Windows.


Verbose Output
--------------

To see the full commands that are being run by make, do:

    make V=1


Loading the Softdevice
----------------------

As mentioned above, `make flash` automatically checks if the correct version of
the softdevice is installed on the nRF and uploads it if necessary.

This works by first downloading 16 bytes at address SOFTDEVICE_TEST_ADDR from the
flash of the nRF into the file downloaded_test.bin. Next, the file softdevice_test.bin
is created by using dd to copy 16 bytes from the same address of the softdevice binary.
Finally the two binary files are compared with diff to see if they are identical or not.
If they differ, the Makefile calls `$(MAKE) flash-softdevice` which runs erase-all,
then flash-softdevice. Finally after that is all complete, the application is uploaded.

Note: SOFTDEVICE_TEST_ADDR was selected through experimentation. At the time the code was
written, the 16 bytes at that address differed in every softdevice version. It's possible
that it will need to be changed for future softdevice versions.

License
-------

These files are modified from
[hlnd/nrf51-pure-gcc-setup](https://github.com/hlnd/nrf51-pure-gcc-setup) and
are licensed separately from the rest of the repository.
