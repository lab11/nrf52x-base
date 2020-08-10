PROJECT_NAME = $(shell basename "$(realpath ./)")

# Configurations
NRF_IC = nrf52840
SDK_VERSION = 16
SOFTDEVICE_MODEL = blank

# Source and header files
APP_HEADER_PATHS += .
APP_SOURCE_PATHS += .
APP_SOURCES = $(notdir $(wildcard ./*.c))


# Include main Makefile
NRF_BASE_DIR ?= ../../../

include $(NRF_BASE_DIR)/boards/nrf52840dk/Board.mk
include $(NRF_BASE_DIR)/make/AppMakefile.mk

