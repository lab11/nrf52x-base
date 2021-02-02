# Commands and configurations for loading code onto hardware with JTAG
# Included by AppMakefile.mk

# Ensure that this file is only included once
ifndef JTAG_MAKEFILE
JTAG_MAKEFILE = 1


# ---- JTAG configurations

# JTAG tools
JLINK = JLinkExe
JLINK_GDBSERVER = JLinkGDBServer
JLINK_RTTCLIENT = JLinkRTTClient

# nrfutil tools
MERGEHEX = python3 $(NRF_BASE_DIR)/make/intelhex/scripts/hexmerge.py
NRFUTIL = nrfutil
PRIVATE_KEY ?= private.pem

HW_VERSION ?= 52

# Default port for GDB
GDB_PORT_NUMBER ?= 2331

# Pick a random port for each RTT session
RAND_PORT_NUMBER := $(shell python3 -c 'from random import randint; print(randint(1023, 65535));' || echo '19021')

# Configuration flags for JTAG tools
JLINK_FLAGS = -device $(FULL_IC) -if swd -speed 4000
JLINK_GDBSERVER_FLAGS = -port $(GDB_PORT_NUMBER)
JLINK_RTT_PORT = -RTTTelnetPort $(RAND_PORT_NUMBER)

# Configuration flags for nrfutil tools
BOOTLOADER_DEV = /dev/ttyACM0
NRFUTIL_SETTINGS_GEN_FLAGS = settings generate --family $(NRF_IC_UPPER) --application-version-string "$(BARE_VERSION)" --bootloader-version 1 --bl-settings-version 2 --key-file $(PRIVATE_KEY) --app-boot-validation='VALIDATE_ECDSA_P256_SHA256' --application $(HEX) $(BOOTLOADER_SETTINGS)
NRFUTIL_PKG_GEN_FLAGS = pkg generate --hw-version $(HW_VERSION) --sd-req 0x0 --application-version-string "$(BARE_VERSION)" --application $(HEX) $(BUILDDIR)$(OUTPUT_NAME).zip
NRFUTIL_PKG_SIGNED_GEN_FLAGS = pkg generate --hw-version $(HW_VERSION) --sd-req 0x0 --application-version-string "$(BARE_VERSION)" --key-file $(PRIVATE_KEY) --application $(HEX) $(BUILDDIR)$(OUTPUT_NAME).zip --app-boot-validation='VALIDATE_ECDSA_P256_SHA256'
ifeq ($(USE_ZIGBEE), 1)
    NRFUTIL_PKG_SIGNED_GEN_FLAGS += --zigbee True --zigbee-manufacturer-id $(MANUF_ID) --zigbee-ota-hw-version $(HW_VERSION) --zigbee-image-type 0xFF01 --zigbee-comment $(OUTPUT_NAME)
endif
NRFUTIL_PKG_USB_DFU_FLAGS = dfu usb-serial -pkg $(BUILDDIR)$(OUTPUT_NAME).zip -p $(BOOTLOADER_DEV) -b 115200

# Allow users to select a specific JTAG device with a variable
ifdef SEGGER_SERIAL
  JLINK_FLAGS += -SelectEmuBySn $(SEGGER_SERIAL)
  JLINK_GDBSERVER_FLAGS += -select USB=$(SEGGER_SERIAL)
endif

# Configuration for test_softdevice command
# Download 16 bytes including the softdevice fimrware ID to disambiguate softdevices
# 16 bytes should be enough to distinguish from previously loaded non-softdevice code too
# https://devzone.nordicsemi.com/f/nordic-q-a/1171/how-do-i-access-softdevice-version-string
SOFTDEVICE_TEST_ADDR = 0x3000
SOFTDEVICE_TEST_LEN = 0x10

# Configure terminal command for Darwin and Linux
UNAME_S := $(shell uname -s)
ifeq ($(firstword $(origin TMUX)),environment)
	# In tmux
	ifeq ($(UNAME_S),Darwin)
		TERMINAL ?= /bin/sh -c 'tmux new-window
	else
		TERMINAL ?= tmux new-window
	endif
else
	# Not in tmux
	ifeq ($(UNAME_S),Darwin)
		TERMINAL ?= osascript -e 'tell application "Terminal" to do script
	else
		TERMINAL ?= x-terminal-emulator -e
	endif
endif

# ---- ID FLASH LOCATION
ifdef ID
  # Write the ID to flash as well
  ID_BYTES = $(subst :, ,$(ID))
  NUM_ID_BYTES = $(words $(ID_BYTES))
  ifneq ($(NUM_ID_BYTES),6)
    $(error "Invalid number of bytes in ID string (expecting 6)")
  endif
  ID_FIRST = $(word 1,$(ID_BYTES))$(word 2,$(ID_BYTES))
  ID_SECON = $(word 3,$(ID_BYTES))$(word 4,$(ID_BYTES))$(word 5,$(ID_BYTES))$(word 6,$(ID_BYTES))
endif

# ---- JTAG rules

.PHONY: flash
ifeq ($(USE_BOOTLOADER),1)
flash: all bootloader test_softdevice flash_bootloader
else
flash: all test_softdevice
endif
	$(Q)printf "r\n" > $(BUILDDIR)flash.jlink
ifdef ID
	$(Q)printf "w4 $(ID_FLASH_LOCATION), 0x$(ID_SECON) 0x$(ID_FIRST)\n" >> $(BUILDDIR)flash.jlink
endif
ifeq ($(USE_BOOTLOADER),1)
	$(Q)printf "loadfile $(MERGED_HEX) \nr\ng\nexit\n" >> $(BUILDDIR)flash.jlink
else
	$(Q)printf "loadfile $(HEX) \nr\ng\nexit\n" >> $(BUILDDIR)flash.jlink
endif
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)flash.jlink

.PHONY: test_softdevice
test_softdevice: $(BUILDDIR)
ifneq ($(SOFTDEVICE_MODEL),blank)
	$(Q)printf "r\nsavebin $(BUILDDIR)downloaded_test.bin $(SOFTDEVICE_TEST_ADDR) $(SOFTDEVICE_TEST_LEN)\nexit\n" > $(BUILDDIR)test-softdevice.jlink
	$(Q)touch $(BUILDDIR)downloaded_test.bin
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)test-softdevice.jlink
	$(Q)$(OBJCOPY) -Iihex -Obinary $(SOFTDEVICE_PATH) $(BUILDDIR)softdevice_bin.bin
	$(Q)dd skip=$(shell printf "%d" $(SOFTDEVICE_TEST_ADDR)) count=$(shell printf "%d" $(SOFTDEVICE_TEST_LEN)) if=$(BUILDDIR)/softdevice_bin.bin of=$(BUILDDIR)softdevice_test.bin bs=1
	$(Q)rm -f $(BUILDDIR)/softdevice_bin.bin
	$(Q)diff -q $(BUILDDIR)downloaded_test.bin $(BUILDDIR)softdevice_test.bin || $(MAKE) flash_softdevice
endif

.PHONY: flash_debug
flash_debug: debug test_softdevice
	$(Q)printf "r\n" > $(BUILDDIR)debug-flash.jlink
ifdef ID
	$(Q)printf "w4 $(ID_FLASH_LOCATION), 0x$(ID_SECON) 0x$(ID_FIRST)\n" >> $(BUILDDIR)debug-flash.jlink
endif
	$(Q)printf "loadfile $(DEBUG_HEX) \nr\ng\nexit\n" >> $(BUILDDIR)debug-flash.jlink
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)debug-flash.jlink

.PHONY: flash_softdevice
flash_softdevice: $(BUILDDIR) $(SOFTDEVICE_PATH)
	$(Q)printf "w4 4001e504 1\nloadfile $(SOFTDEVICE_PATH) \nr\ng\nexit\n" > $(BUILDDIR)flash_softdevice.jlink
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)flash_softdevice.jlink

.PHONY: flash_mbr
flash_mbr: $(BUILDDIR) $(MBR_PATH)
	$(Q)printf "loadfile $(MBR_PATH) \nr\ng\nexit\n" > $(BUILDDIR)flash_mbr.jlink
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)flash_mbr.jlink

.PHONY: flash_bootloader
flash_bootloader: bootloader flash_mbr $(BUILDDIR) $(MBR_PATH)
	$(Q)printf "loadfile $(BOOTLOADER_HEX) \nr\ng\nexit\n" > $(BUILDDIR)flash_bootloader.jlink
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)flash_bootloader.jlink

#https://devzone.nordicsemi.com/f/nordic-q-a/12484/approtect-and-dap
.PHONY: erase
erase: $(BUILDDIR)
	$(Q)printf "SWDSelect\nSWDWriteDP 1 0x50000000\nSWDWriteDP 2 0x01000000\nSWDWriteAP 1 0x00000001\n sleep 100\nr\nexit\n" > $(BUILDDIR)erase.jlink
	$(Q)$(JLINK) $(JLINK_FLAGS) $(BUILDDIR)erase.jlink

.PHONY: gdb
gdb:
ifeq ($(UNAME_S),Darwin)
	$(Q)$(TERMINAL) "cd $(PWD) && $(JLINK_GDBSERVER) $(JLINK_FLAGS) $(JLINK_GDBSERVER_FLAGS)"'
	$(Q)sleep 1
	$(Q)printf "target remote localhost:$(GDB_PORT_NUMBER)\nload\nmon reset\nbreak main\ncontinue\n" > .gdbinit
ifneq ("$(wildcard $(DEBUG_ELF))","")
	$(Q)$(TERMINAL) "cd $(PWD) && $(GDB) -x .gdbinit $(DEBUG_ELF)"'
else ifneq ("$(wildcard $(ELF))","")
	$(Q)$(TERMINAL) "cd $(PWD) && $(GDB) -x .gdbinit $(ELF)"'
else
	$(Q)$(TERMINAL) "cd $(PWD) && $(GDB) -x .gdbinit"'
endif
else
	$(Q)$(TERMINAL) "$(JLINK_GDBSERVER) $(JLINK_FLAGS) $(JLINK_GDBSERVER_FLAGS)"
	$(Q)sleep 1
	$(Q)printf "target remote localhost:$(GDB_PORT_NUMBER)\nload\nmon reset\nbreak main\ncontinue\n" > .gdbinit
ifneq ("$(wildcard $(DEBUG_ELF))","")
	$(Q)$(TERMINAL) "$(GDB) -x .gdbinit $(DEBUG_ELF)"
else ifneq ("$(wildcard $(ELF))","")
	$(Q)$(TERMINAL) "$(GDB) -x .gdbinit $(ELF)"
else
	$(Q)$(TERMINAL) "$(GDB) -x .gdbinit"
endif
endif

.PHONY: rtt
rtt:
ifeq ($(UNAME_S),Darwin)
	$(TERMINAL) "$(JLINK) $(JLINK_FLAGS) $(JLINK_RTT_PORT) -AutoConnect 1"'
	sleep 1
	$(TERMINAL) "$(JLINK_RTTCLIENT) $(JLINK_RTT_PORT)"'
else
	$(Q)$(TERMINAL) "$(JLINK) $(JLINK_FLAGS) $(JLINK_RTT_PORT) -AutoConnect 1"
	$(Q)sleep 1
	$(Q)$(TERMINAL) "$(JLINK_RTTCLIENT) $(JLINK_RTT_PORT)"
endif

# ---- nrfutil bootloader rules
.PHONY: pkg
pkg: all
	$(NRFUTIL) $(NRFUTIL_PKG_GEN_FLAGS)
.PHONY: pkg_signed
pkg_signed: all
	$(NRFUTIL) $(NRFUTIL_PKG_SIGNED_GEN_FLAGS)
.PHONY: dfu
usb_dfu: all pkg
	until $(NRFUTIL) $(NRFUTIL_PKG_USB_DFU_FLAGS); do sleep 0.5; done;

.PHONY: clean
clean::
	$(Q)rm -rf JLink.log .gdbinit

print-%  : ; @echo $* = $($*)

endif

