# Board-specific configurations for the microbit v2 (uses nrf52833 like pca10100)

# Ensure that this file is only included once
ifndef BOARD_MAKEFILE
BOARD_MAKEFILE = 1

# Get directory of this makefile
BOARD_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
KEY_DIR := $(BOARD_DIR)/../keys/

# Include any files in this directory in the build process
BOARD_SOURCE_PATHS = $(BOARD_DIR)/.
BOARD_HEADER_PATHS = $(BOARD_DIR)/.
BOARD_LINKER_PATHS = $(BOARD_DIR)/.
BOARD_SOURCES = $(notdir $(wildcard $(BOARD_DIR)/./*.c))
BOARD_AS = $(notdir $(wildcard $(BOARD_DIR)/./*.s))

# Board-specific configurations
BOARD = PCA10100
USE_BLE = 0
USE_THREAD = 0

# Additional #define's to be added to code by the compiler
BOARD_VARS = \
	BOARD_$(BOARD)\
	USE_APP_CONFIG\
	DEBUG\
	DEBUG_NRF\

# Default SDK source files to be included
BOARD_SOURCES += \
	app_error_handler_gcc.c\
	app_scheduler.c\
	app_timer.c\
	app_util_platform.c\
	before_startup.c\
	hardfault_handler_gcc.c\
	hardfault_implementation.c\
	nrf_assert.c\
	nrf_atomic.c\
	nrf_balloc.c\
	nrf_drv_twi.c\
	nrf_drv_uart.c\
	nrf_fprintf.c\
	nrf_fprintf_format.c\
	nrf_log_backend_rtt.c\
	nrf_log_backend_serial.c\
	nrf_log_default_backends.c\
	nrf_log_frontend.c\
	nrf_log_str_formatter.c\
	nrf_pwr_mgmt.c\
	nrf_memobj.c\
	nrf_ringbuf.c\
	nrf_section_iter.c\
	nrf_serial.c\
	nrf_sdh.c\
	nrf_strerror.c\
	nrf_queue.c\
	nrf_drv_clock.c\
	nrf_nvmc.c\
	nrfx_gpiote.c\
	nrfx_ppi.c\
	nrfx_prs.c\
	nrfx_saadc.c\
	nrfx_timer.c\
	nrfx_twi.c\
	nrfx_twim.c\
	nrfx_clock.c\
	nrfx_uart.c\
	nrfx_uarte.c\
	app_fifo.c\
	app_uart_fifo.c\

endif

