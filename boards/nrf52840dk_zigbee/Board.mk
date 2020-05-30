# Board-specific configurations for the nRF52 development kit (pca10040)

# Ensure that this file is only included once
ifndef BOARD_MAKEFILE
BOARD_MAKEFILE = 1

# Get directory of this makefile
BOARD_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
KEY_DIR := $(BOARD_DIR)/../../keys/

# Include any files in this directory in the build process
BOARD_SOURCE_PATHS = $(BOARD_DIR)/.. $(BOARD_DIR)/../../keys/ $(BOARD_DIR)/../../../lib/
BOARD_HEADER_PATHS = $(BOARD_DIR)/.
BOARD_HEADER_PATHS += $(BOARD_DIR)/..
BOARD_HEADER_PATHS += $(BOARD_DIR)/../../../lib/
BOARD_LINKER_PATHS = $(BOARD_DIR)/.
BOARD_LINKER_PATHS += $(BOARD_DIR)/..
#LINKER_SCRIPT = zigbee_multi_sensor_gcc_nrf52.ld
BOARD_SOURCES = $(notdir $(wildcard $(BOARD_DIR)/../*.c))
BOARD_AS = $(notdir $(wildcard $(BOARD_DIR)/../*.s))

# Board-specific configurations
BOARD = PCA10056
USE_BLE = 0
HW_VERSION = 52
MANUF_ID = 0

# Additional #define's to be added to code by the compiler
BOARD_VARS = \
	BOARD_$(BOARD)\
	NRF_DFU_SETTINGS_VERSION=1\
	USE_APP_CONFIG\
	DEBUG\
	DEBUG_NRF\

# Default SDK source files to be included
BOARD_SOURCES += \
	bsp.c\
	boards.c\
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
	nrf_drv_spi.c\
	nrf_drv_rng.c\
	nrf_twi_mngr.c\
	nrf_spi_mngr.c\
	nrf_fprintf.c\
	nrf_fprintf_format.c\
	nrf_log_backend_rtt.c\
	nrf_log_backend_serial.c\
	nrf_log_default_backends.c\
	nrf_log_frontend.c\
	nrf_log_str_formatter.c\
	nrf_pwr_mgmt.c\
	nrf_memobj.c\
	mem_manager.c\
	nrf_ringbuf.c\
	nrf_section_iter.c\
	nrf_sdh.c\
	nrf_strerror.c\
	nrf_queue.c\
	nrf_drv_clock.c\
	nrf_nvmc.c\
	nrf_drv_ppi.c\
	nrf_ecb.c\
	nrfx_gpiote.c\
	nrfx_saadc.c\
	nrfx_timer.c\
	nrfx_twi.c\
	nrfx_twim.c\
	nrfx_spi.c\
	nrfx_spim.c\
	nrfx_clock.c\
	nrfx_ppi.c\
	nrfx_rtc.c\
	nrfx_rng.c\
	SEGGER_RTT.c\
	SEGGER_RTT_Syscalls_GCC.c\
	SEGGER_RTT_printf.c\
	device_id.c\
	nrf_fstorage.c \
	nrf_fstorage_nvmc.c \
	crc32.c \
	pb_common.c\
	pb_decode.c\
	pb_encode.c \
	fds.c \
	nrf_atfifo.c \
	flash_storage.c \

ifeq ($(USE_ZIGBEE), 1)
	BOARD_SOURCES += \
		sensorsim.c\
		zb_zcl_pressure_measurement.c\
		zigbee_helpers.c\
		zigbee_logger_eprxzcl.c\
		zb_nrf52_common.c \
		zb_nrf52_nrf_logger.c \
		zb_nrf52_nvram.c \
		zb_nrf52_sdk_config_deps.c \
		zb_nrf52_timer.c \
		zb_nrf52_transceiver.c \
		zb_zcl_common_addons.c \
		zb_zcl_ota_upgrade_addons.c \
		zb_error_to_string.c \
		simple_zigbee.c
endif

ifeq ($(USE_BOOTLOADER), 1)
	BOARD_SOURCES += \
		zigbee_dfu_transport.c \
		dfu-cc.pb.c \
		nrf_dfu.c \
		nrf_dfu_mbr.c \
		nrf_dfu_flash.c \
		nrf_dfu_handling_error.c \
		nrf_dfu_req_handler.c \
		nrf_dfu_settings.c \
		nrf_dfu_transport.c \
		nrf_dfu_utils.c \
		nrf_dfu_validation.c \
		nrf_dfu_ver_validation.c \
		background_dfu_block.c \
		background_dfu_operation.c \
		background_dfu_state.c \
		nrf_crypto_ecc.c \
		nrf_crypto_ecdsa.c \
		nrf_crypto_hash.c \
		nrf_crypto_init.c \
		nrf_crypto_shared.c \
		sha256.c \
		cc310_bl_backend_hash.c \
		cc310_bl_backend_ecc.c \
		cc310_bl_backend_ecdsa.c \
		cc310_bl_backend_init.c \
		cc310_bl_backend_shared.c

endif

endif

