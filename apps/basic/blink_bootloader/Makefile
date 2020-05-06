PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52840
SDK_VERSION = 16
SOFTDEVICE_MODEL = blank
USE_BOOTLOADER = 1
BOOTLOADER = bg_bootloader

# Source and header files
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .
APP_SOURCES = $(notdir $(wildcard ./*.c))

NRF_BASE_DIR ?= ../../../

# Include board Makefile (if any)
include $(NRF_BASE_DIR)/boards/nrf52840dk_thread/Board.mk

PRIVATE_KEY := $(KEY_DIR)/private.pem

# Include main Makefile
include $(NRF_BASE_DIR)/make/AppMakefile.mk
