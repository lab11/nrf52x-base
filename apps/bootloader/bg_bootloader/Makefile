PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52840
SDK_VERSION = 16
SOFTDEVICE_MODEL = blank
USE_THREAD = 1
USE_MBR = 1
USE_BLE = 0
LINKER_SCRIPT = nrf52840_background_bootloader.ld
LDFLAGS += --specs=nano.specs
CFLAGS += -flto
#CFLAGS += -D__HEAP_SIZE=0

KEY_DIR ?= $(NRF_BASE_DIR)/boards/keys/

# Source and header files
SDK_CONFIG_DEFAULT =
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .
APP_SOURCES = $(notdir $(wildcard ./*.c))

NRF_BASE_DIR ?= ../../../
# Include board Makefile (if any)
include $(NRF_BASE_DIR)/boards/bootloader/Board.mk
# Include main Makefile
include $(NRF_BASE_DIR)/make/AppMakefile.mk
