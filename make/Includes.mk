# Included folders and source files for building nrf applications
# Included by Configuration.mk

# Ensure that this file is only included once
ifndef INCLUDES_MAKEFILE
INCLUDES_MAKEFILE = 1


# ---- This repo's files
REPO_HEADER_PATHS += $(NRF_BASE_DIR)/lib/
REPO_HEADER_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/drivers/*/))
REPO_HEADER_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/*/))
REPO_HEADER_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/simple_ble/*/))
REPO_HEADER_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/simple_logger/*/))
REPO_HEADER_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/simple_thread/*/))
REPO_HEADER_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/google_cloud/*/))
REPO_HEADER_PATHS += $(NRF_BASE_DIR)/lib/nanopb/
REPO_SOURCE_PATHS += $(NRF_BASE_DIR)/lib/
REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/drivers/*/))
REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/*/))
REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/simple_ble/*/))
REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/simple_logger/*/))
REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/simple_thread/*/))
REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/google_cloud/*/))
REPO_SOURCE_PATHS += $(NRF_BASE_DIR)/lib/nanopb/
REPO_SOURCE_PATHS += $(PROTO_DIR)
REPO_HEADER_PATHS += $(NRF_BASE_DIR)/lib/CMSIS_5/CMSIS/Core/Include/
REPO_HEADER_PATHS += $(NRF_BASE_DIR)/lib/CMSIS_5/CMSIS/DSP/Include/

# ---- CMSIS DSP Library
ifeq ($(USE_DSP), 1)
  REPO_SOURCE_PATHS += $(dir $(wildcard $(NRF_BASE_DIR)/lib/CMSIS_5/CMSIS/DSP/Source/*/))
  #REPO_SOURCES = $(notdir $(wildcard $(NRF_BASE_DIR)lib/CMSIS_5/CMSIS/DSP/Source/*/arm*.c))
  REPO_SOURCES += BasicMathFunctions.c
  REPO_SOURCES += MatrixFunctions.c
  REPO_SOURCES += TransformFunctions.c
  REPO_SOURCES += FilteringFunctions.c
  REPO_SOURCES += ComplexMathFunctions.c
  REPO_SOURCES += StatisticsFunctions.c
  REPO_SOURCES += CommonTables.c
endif

# ---- SDK files

ifneq (,$(filter $(NRF_IC),nrf52832 nrf52833 nrf52840))
  ifeq ($(SDK_VERSION), 15)

    # Set the path
    ifeq ($(USE_THREAD), 1)
      SDK_ROOT = $(NRF_BASE_DIR)/sdk/nrf5_sdk_15.3.0_thread/
    else
      SDK_ROOT = $(NRF_BASE_DIR)/sdk/nrf5_sdk_15.3.0/
    endif

    MBR_PATH ?= $(SDK_ROOT)components/softdevice/mbr/$(NRF_IC)/hex/mbr_$(NRF_MODEL)_$(MBR_VERSION)_mbr.hex

    # default files for ICs
    # Note: nrf52833 doesn't exist in SDK15
    ifeq ($(NRF_IC),nrf52832)
      SDK_SOURCES += system_nrf52.c
      SDK_AS = gcc_startup_nrf52.S
    else ifeq ($(NRF_IC),nrf52840)
      SDK_SOURCES += system_nrf52840.c
      SDK_AS = gcc_startup_nrf52840.S
    endif

    # default C files necessary for any application
    #XXX: are there other C files that I can include here?
    #SDK_SOURCES += app_error.c
    SDK_SOURCES += app_error_weak.c

    # To make SEGGER RTT retarget to printf correctly, you need to get around
    # the silly ANSI C compatibility stuff that breaks our GCC builds
    PARAMS_DEFINE = -D_PARAMS\(paramlist\)=paramlist

    # Add paths for sdk-specific linker files
    SDK_LINKER_PATHS += $(SDK_ROOT)modules/nrfx/mdk/

    # Path for default sdk_config.h
    SDK_CONFIG_DEFAULT ?= $(NRF_BASE_DIR)/make/config/$(NRF_IC)/config/
    SDK_HEADER_PATHS += $(SDK_CONFIG_DEFAULT)
    #SDK_HEADER_PATHS += $(NRF_BASE_DIR)/make/config/$(NRF_IC)/config/

    # Need to add the paths for all the directories in the SDK.
    # Note that we do not use * because some folders have conflicting files.
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/mdk/
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/hal/
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/drivers/include/
    SDK_HEADER_PATHS += $(SDK_ROOT)integration/nrfx/
    SDK_HEADER_PATHS += $(SDK_ROOT)integration/nrfx/legacy/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/atomic/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/atomic_fifo/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/atomic_flags/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/balloc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/block_dev/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/bootloader/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/bootloader/dfu/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/background_dfu/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/background_dfu/transport/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/background_dfu/transport/coap/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/coap/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/common/
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/libraries/log/src/)
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/bsp/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/button/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/cli/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crc16/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crc32/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crypto/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/csense/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/csense_drv/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/delay/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/ecc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/libuarte/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries//log/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/memobj/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/mpu/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/ringbuf/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/experimental_section_vars/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/stack_guard/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/task_manager/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/fds/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/fifo/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/fstorage/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/gfx/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/gpiote/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/hardfault/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/hardfault/$(NRF_MODEL)/handler/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/hci/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/led_softblink/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/low_power_pwm/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/mem_manager/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/mutex/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/pwm/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/pwr_mgmt/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/queue/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/scheduler/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sdcard/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sensorsim/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/serial/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sha256/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/simple_timer/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/slip/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sortlist/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/spi_mngr/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/stack_info/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/strerror/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/svc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/timer/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/twi_mngr/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/twi_sensor/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/uart/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/usbd/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/drivers_nrf/usbd/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/usbd/class/nrf_dfu_trigger/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/util/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/boards/
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/libraries/log/src/)
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crypto/backend/cc310/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crypto/backend/cc310_bl/
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/libraries/crypto/backend/*/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/adc/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/ble_flash/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/clock/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/common/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/delay/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/gpiote/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/hal/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/lpcomp/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/ppi/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/pstorage/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/pstorage/config/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/radio_config/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/rng/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/rtc/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/saadc/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/sdio/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/spi_master/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/spi_slave/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/swi/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/timer/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/twi_master/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/uart/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/wdt/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/validation/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_ext/*/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/device/)
    SDK_HEADER_PATHS += $(SDK_ROOT)external/fprintf/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/segger_rtt/
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)external/*/include/)
    SDK_HEADER_PATHS += $(SDK_ROOT)components/toolchain/gcc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/toolchain/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/common/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/cifra_AES128-EAX/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/nrf_tls/

    SDK_SOURCE_PATHS += $(SDK_ROOT)components/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/mdk/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/hal/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/drivers/src/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/drivers/src/prs/
    SDK_SOURCE_PATHS += $(SDK_ROOT)integration/nrfx/legacy/
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)modules/nrfx/*/)
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/*/)
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/atomic/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/atomic_fifo/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/atomic_flags/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/balloc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/block_dev/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/bootloader/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/bootloader/dfu/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/background_dfu/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/background_dfu/transport/coap/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/coap/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/common/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/bsp/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/button/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/cli/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crc16/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crc32/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crypto/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/csense/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/csense_drv/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/delay/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/ecc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/libuarte/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/log/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/memobj/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/mpu/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/ringbuf/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/experimental_section_vars/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/stack_guard/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/task_manager/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/fds/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/fifo/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/fstorage/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/gfx/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/gpiote/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/hardfault/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/hardfault/$(NRF_MODEL)/handler/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/hci/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/led_softblink/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/low_power_pwm/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/mem_manager/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/mutex/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/pwm/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/pwr_mgmt/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/queue/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/scheduler/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sdcard/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sensorsim/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/serial/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sha256/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/simple_timer/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/slip/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sortlist/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/spi_mngr/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/stack_info/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/strerror/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/svc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/timer/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/twi_mngr/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/twi_sensor/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/uart/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/usbd/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/drivers_nrf/usbd/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/usbd/class/nrf_dfu_trigger/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/util/
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/libraries/log/src/)
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crypto/backend/cc310/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crypto/backend/cc310_bl/
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/*/)
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/drivers_ext/*/)
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/toolchain/gcc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/common/
    SDK_SOURCE_PATHS += $(SDK_ROOT)external/fprintf/
    SDK_SOURCE_PATHS += $(SDK_ROOT)external/segger_rtt/
    SDK_SOURCE_PATHS += $(SDK_ROOT)external/cifra_AES128-EAX/

    ifdef SERIALIZATION_MODE
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/*/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/common/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/hal/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/transport/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/config/)

      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/*/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/common/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/hal/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/transport/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/config/)

      # Add defines based on if we are "connectivity" or "application"
      ifeq ($(SERIALIZATION_MODE), connectivity)
        SDK_VARS += SER_CONNECTIVITY APP_SCHEDULER_WITH_PAUSE BLE_STACK_SUPPORT_REQD HCI_TIMER2 SOFTDEVICE_PRESENT
      endif
      ifeq ($(SERIALIZATION_MODE), application)
        SDK_VARS += SVCALL_AS_NORMAL_FUNCTION BLE_STACK_SUPPORT_REQD
      endif
    endif

    ifeq ($(USE_BLE),1)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/ble/*/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/ble/ble_services/*/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/ble/*/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/ble/ble_services/*/)
      SDK_VARS += BLE_STACK_SUPPORT_REQD SOFTDEVICE_PRESENT NRF_SD_BLE_API_VERSION=6
    endif

    ifneq ($(SOFTDEVICE_MODEL), blank)
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/common/softdevice_handler/
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/nrf52

      SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/common/softdevice_handler/
      SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/
      SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/nrf52

      ifdef SERIALIZATION_MODE
        # Load the sources from the serialization library
        SDK_HEADER_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/middleware/
        SDK_HEADER_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/serializers/
        SDK_HEADER_PATHS += $(SDK_ROOT)components/serialization/common/struct_ser/$(SOFTDEVICE_MODEL)/

        SDK_SOURCE_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/middleware/
        SDK_SOURCE_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/serializers/
        SDK_SOURCE_PATHS += $(SDK_ROOT)components/serialization/common/struct_ser/$(SOFTDEVICE_MODEL)/
      endif
    else
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/nrf_soc_nosd/)
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/mbr/$(NRF_IC)/headers
    endif

    ifeq ($(USE_ANT),1)
      CFLAGS += -DANT_STACK_SUPPORT_REQD
    endif

    ifeq ($(USE_GZP),1)
      USE_GZLL = 1
      SDK_SOURCE_PATHS += $(SDK_SOURCE_PATH)proprietary_rf/gzll
      SDK_SOURCES += nrf_gzp.c
      SDK_SOURCES += nrf_gzp_device.c
      SDK_SOURCES += nrf_gzp_host.c
      SDK_SOURCES += nrf_gzp_host_nrf5x.c
    endif

    ifeq ($(USE_GZLL),1)
      SDK_HEADER_PATHS += $(SDK_INCLUDE_PATH)proprietary_rf/gzll
      LIBS += $(SDK_INCLUDE_PATH)proprietary_rf/gzll/gcc/gzll_nrf52_sd_resources_gcc.a
    endif

    ifeq ($(USE_ESB),1)
      SDK_SOURCE_PATHS += $(SDK_SOURCE_PATH)proprietary_rf/esb
      SDK_SOURCES += nrf_esb.c
    endif

    ifeq ($(USE_THREAD),1)
      ifdef THREAD_FTD
        THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-ftd.a
      else
        THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-mtd.a
      endif

      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-sdk.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-transport.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-platform-utils.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libnordicsemi-nrf52840-radio-driver.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/nrf_cc310/lib/libnrf_cc310_0.9.10.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/nrf_cc310_bl/lib/libnrf_cc310_bl_0.9.10.a
      LIBS += $(THREAD_LIB_FILES)

      REPO_HEADER_PATHS += $(SDK_ROOT)external/openthread/include/

      SDK_VARS += OPENTHREAD_ENABLE_APPLICATION_COAP OPENTHREAD_MTD=1
    endif

  endif # SDK 15
  ifeq ($(SDK_VERSION), 16)

    # Set the path
    ifeq ($(USE_THREAD), 1)
      SDK_ROOT = $(NRF_BASE_DIR)/sdk/nrf5_sdk_16.0.0_thread/
    else ifeq ($(USE_ZIGBEE), 1)
      SDK_ROOT = $(NRF_BASE_DIR)/sdk/nrf5_sdk_16.0.0_thread/
    else
      SDK_ROOT = $(NRF_BASE_DIR)/sdk/nrf5_sdk_16.0.0/
    endif

    MBR_PATH ?= $(SDK_ROOT)components/softdevice/mbr/hex/mbr_$(NRF_MODEL)_$(MBR_VERSION)_mbr.hex

    # default files for ICs
    ifeq ($(NRF_IC),nrf52832)
      SDK_SOURCES += system_nrf52.c
      SDK_AS = gcc_startup_nrf52.S
    else ifeq ($(NRF_IC),nrf52833)
      SDK_SOURCES += system_nrf52833.c
      SDK_AS = gcc_startup_nrf52833.S
    else ifeq ($(NRF_IC),nrf52840)
      SDK_SOURCES += system_nrf52840.c
      SDK_AS = gcc_startup_nrf52840.S
    endif

    # default C files necessary for any application
    #XXX: are there other C files that I can include here?
    #SDK_SOURCES += app_error.c
    SDK_SOURCES += app_error_weak.c

    # To make SEGGER RTT retarget to printf correctly, you need to get around
    # the silly ANSI C compatibility stuff that breaks our GCC builds
    PARAMS_DEFINE = -D_PARAMS\(paramlist\)=paramlist

    # Add paths for sdk-specific linker files
    SDK_LINKER_PATHS += $(SDK_ROOT)modules/nrfx/mdk/

    # Path for default sdk_config.h
    SDK_CONFIG_DEFAULT ?= $(NRF_BASE_DIR)/make/config/$(NRF_IC)/config/
    SDK_HEADER_PATHS += $(SDK_CONFIG_DEFAULT)
    #SDK_HEADER_PATHS += $(NRF_BASE_DIR)/make/config/$(NRF_IC)/config/

    # Need to add the paths for all the directories in the SDK.
    # Note that we do not use * because some folders have conflicting files.
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/mdk/
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/hal/
    SDK_HEADER_PATHS += $(SDK_ROOT)modules/nrfx/drivers/include/
    SDK_HEADER_PATHS += $(SDK_ROOT)integration/nrfx/
    SDK_HEADER_PATHS += $(SDK_ROOT)integration/nrfx/legacy/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/atomic/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/atomic_fifo/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/atomic_flags/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/balloc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/block_dev/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/block_dev/ram/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/block_dev/qspi/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/bootloader/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/bootloader/dfu/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/background_dfu/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/background_dfu/transport/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/background_dfu/transport/coap/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/coap/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/iot/common/
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/libraries/log/src/)
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/bsp/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/button/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/cli/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crc16/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crc32/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/crypto/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/csense/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/csense_drv/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/delay/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/ecc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/libuarte/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries//log/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/memobj/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/mpu/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/ringbuf/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/experimental_section_vars/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/stack_guard/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/task_manager/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/fds/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/fifo/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/fstorage/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/gfx/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/gpiote/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/hardfault/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/hardfault/$(NRF_MODEL)/handler/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/hci/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/led_softblink/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/low_power_pwm/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/mem_manager/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/mutex/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/pwm/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/pwr_mgmt/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/queue/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/scheduler/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sdcard/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sensorsim/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/serial/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sha256/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/simple_timer/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/slip/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/sortlist/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/spi_mngr/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/stack_info/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/strerror/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/svc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/timer/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/twi_mngr/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/twi_sensor/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/uart/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/usbd/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/drivers_nrf/usbd/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/usbd/class/nrf_dfu_trigger/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/libraries/util/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/boards/
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/libraries/log/src/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/libraries/crypto/backend/*)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/adc/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/ble_flash/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/clock/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/common/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/delay/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/gpiote/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/hal/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/lpcomp/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/ppi/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/pstorage/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/pstorage/config/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/radio_config/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/rng/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/rtc/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/saadc/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/sdio/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/spi_master/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/spi_slave/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/swi/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/timer/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/twi_master/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/uart/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/wdt/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/validation/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_ext/*/)
    SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/device/)
    SDK_HEADER_PATHS += $(SDK_ROOT)external/fprintf/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/segger_rtt/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/nrf_cc310_bl/include/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/nrf_cc310/include/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/toolchain/gcc/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/toolchain/
    SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/common/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/cifra_AES128-EAX/
    SDK_HEADER_PATHS += $(SDK_ROOT)external/nrf_tls/
    SDK_HEADER_PATHS += $(SDK_ROOT)/external/fatfs/port/
    SDK_HEADER_PATHS += $(SDK_ROOT)/external/fatfs/src/

    SDK_SOURCE_PATHS += $(SDK_ROOT)components/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/mdk/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/hal/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/drivers/src/
    SDK_SOURCE_PATHS += $(SDK_ROOT)modules/nrfx/drivers/src/prs/
    SDK_SOURCE_PATHS += $(SDK_ROOT)integration/nrfx/legacy/
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)modules/nrfx/*/)
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/*/)
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/atomic/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/atomic_fifo/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/atomic_flags/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/balloc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/block_dev/
    APP_SOURCE_PATHS += $(SDK_ROOT)/components/libraries/block_dev/ram/
    APP_SOURCE_PATHS += $(SDK_ROOT)/components/libraries/block_dev/qspi/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/bootloader/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/bootloader/dfu/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/background_dfu/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/background_dfu/transport/coap/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/coap/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/iot/common/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/bsp/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/button/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/cli/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crc16/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crc32/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/crypto/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/csense/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/csense_drv/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/delay/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/ecc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/libuarte/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/log/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/memobj/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/mpu/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/ringbuf/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/experimental_section_vars/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/stack_guard/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/task_manager/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/fds/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/fifo/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/fstorage/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/gfx/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/gpiote/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/hardfault/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/hardfault/$(NRF_MODEL)/handler/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/hci/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/led_softblink/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/low_power_pwm/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/mem_manager/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/mutex/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/pwm/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/pwr_mgmt/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/queue/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/scheduler/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sdcard/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sensorsim/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/serial/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sha256/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/simple_timer/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/slip/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/sortlist/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/spi_mngr/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/stack_info/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/strerror/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/svc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/timer/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/twi_mngr/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/twi_sensor/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/uart/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/usbd/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/drivers_nrf/usbd/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/usbd/class/nrf_dfu_trigger/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/libraries/util/
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/libraries/log/src/)
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/libraries/crypto/backend/*)
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/*/)
    SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/drivers_ext/*/)
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/toolchain/gcc/
    SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/common/
    SDK_SOURCE_PATHS += $(SDK_ROOT)external/fprintf/
    SDK_SOURCE_PATHS += $(SDK_ROOT)external/segger_rtt/
    SDK_SOURCE_PATHS += $(SDK_ROOT)external/cifra_AES128-EAX/
    SDK_SOURCE_PATHS += $(SDK_ROOT)/external/fatfs/port/
    SDK_SOURCE_PATHS += $(SDK_ROOT)/external/fatfs/src/

    ifdef SERIALIZATION_MODE
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/*/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/common/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/hal/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/transport/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/config/)

      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/*/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/common/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/hal/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/transport/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/serialization/common/transport/ser_phy/config/)

      # Add defines based on if we are "connectivity" or "application"
      ifeq ($(SERIALIZATION_MODE), connectivity)
        SDK_VARS += SER_CONNECTIVITY APP_SCHEDULER_WITH_PAUSE BLE_STACK_SUPPORT_REQD HCI_TIMER2 SOFTDEVICE_PRESENT
      endif
      ifeq ($(SERIALIZATION_MODE), application)
        SDK_VARS += SVCALL_AS_NORMAL_FUNCTION BLE_STACK_SUPPORT_REQD
      endif
    endif

    ifeq ($(USE_BLE),1)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/ble/*/)
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/ble/ble_services/*/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/ble/*/)
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/ble/ble_services/*/)
      SDK_VARS += BLE_STACK_SUPPORT_REQD SOFTDEVICE_PRESENT NRF_SD_BLE_API_VERSION=6
    endif

    ifneq ($(SOFTDEVICE_MODEL), blank)
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/common/softdevice_handler/
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/nrf52

      SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/common/softdevice_handler/
      SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/
      SDK_SOURCE_PATHS += $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/headers/nrf52

      ifdef SERIALIZATION_MODE
        # Load the sources from the serialization library
        SDK_HEADER_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/middleware/
        SDK_HEADER_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/serializers/
        SDK_HEADER_PATHS += $(SDK_ROOT)components/serialization/common/struct_ser/$(SOFTDEVICE_MODEL)/

        SDK_SOURCE_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/middleware/
        SDK_SOURCE_PATHS += $(SDK_ROOT)components/serialization/$(SERIALIZATION_MODE)/codecs/$(SOFTDEVICE_MODEL)/serializers/
        SDK_SOURCE_PATHS += $(SDK_ROOT)components/serialization/common/struct_ser/$(SOFTDEVICE_MODEL)/
      endif
    else
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/drivers_nrf/nrf_soc_nosd/)
      SDK_HEADER_PATHS += $(SDK_ROOT)components/softdevice/mbr/headers
    endif

    ifeq ($(USE_ANT),1)
      CFLAGS += -DANT_STACK_SUPPORT_REQD
    endif

    ifeq ($(USE_GZP),1)
      USE_GZLL = 1
      SDK_SOURCE_PATHS += $(SDK_SOURCE_PATH)proprietary_rf/gzll
      SDK_SOURCES += nrf_gzp.c
      SDK_SOURCES += nrf_gzp_device.c
      SDK_SOURCES += nrf_gzp_host.c
      SDK_SOURCES += nrf_gzp_host_nrf5x.c
    endif

    ifeq ($(USE_GZLL),1)
      SDK_HEADER_PATHS += $(SDK_INCLUDE_PATH)proprietary_rf/gzll
      LIBS += $(SDK_INCLUDE_PATH)proprietary_rf/gzll/gcc/gzll_nrf52_sd_resources_gcc.a
    endif

    ifeq ($(USE_ESB),1)
      SDK_SOURCE_PATHS += $(SDK_SOURCE_PATH)proprietary_rf/esb
      SDK_SOURCES += nrf_esb.c
    endif

    NRF_SECURITY = $(SDK_ROOT)external/openthread/nrf_security/
    ifeq ($(USE_THREAD),1)
      ifeq ($(THREAD_FTD),1)
        THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-ftd.a
        SDK_VARS += OPENTHREAD_FTD=1
      else
        THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-mtd.a
        SDK_VARS += OPENTHREAD_MTD=1
      endif

      SDK_HEADER_PATHS += $(SDK_ROOT)components/thread/utils/
      SDK_SOURCE_PATHS += $(SDK_ROOT)components/thread/utils/

      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-sdk.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-nrf52840-transport.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libnordicsemi-nrf52840-radio-driver.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/openthread/lib/nrf52840/gcc/libopenthread-platform-utils.a

      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedcrypto_glue.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedcrypto_glue_cc310.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedcrypto_glue_vanilla.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedcrypto_cc310_backend.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedcrypto_vanilla_backend.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedtls_tls_vanilla.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedtls_x509_vanilla.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libmbedtls_base_vanilla.a
      THREAD_LIB_FILES += $(NRF_SECURITY)lib/libnrf_cc310_platform_0.9.1.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.12.a
      THREAD_LIB_FILES += $(SDK_ROOT)external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.12.a
      LIBS += $(THREAD_LIB_FILES)

      SDK_HEADER_PATHS += $(SDK_ROOT)external/openthread/include
      SDK_HEADER_PATHS += $(SDK_ROOT)external/openthread/project/nrf52840
      SDK_HEADER_PATHS += $(SDK_ROOT)external/openthread/project/config
      SDK_HEADER_PATHS += $(NRF_SECURITY)mbedtls_plat_config
      SDK_HEADER_PATHS += $(NRF_SECURITY)include
      SDK_HEADER_PATHS += $(NRF_SECURITY)config
      SDK_HEADER_PATHS += $(NRF_SECURITY)/nrf_cc310_plat/include

      SDK_VARS += OPENTHREAD_ENABLE_APPLICATION_COAP
      SDK_VARS += OPENTHREAD_CONFIG_FILE=\"openthread-config-wrap.h\"
      SDK_VARS += MBEDTLS_USER_CONFIG_FILE=\"nrf52840-mbedtls-config.h\"
      SDK_VARS += MBEDTLS_CONFIG_FILE=\"nrf-config.h\"
      SDK_VARS += OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=0
      SDK_VARS += ENABLE_FEM
    endif


    ifeq ($(USE_ZIGBEE), 1)
      SDK_SOURCE_PATHS += $(SDK_ROOT)/external/zboss/zb_error
      SDK_SOURCE_PATHS += $(SDK_ROOT)/external/zboss/osif
      SDK_SOURCE_PATHS += $(SDK_ROOT)/external/zboss/addons/zcl
      SDK_SOURCE_PATHS += $(wildcard $(SDK_ROOT)components/zigbee/*/)
      SDK_SOURCE_PATHS += $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src

      SDK_HEADER_PATHS += $(SDK_ROOT)/external/zboss/osif
      SDK_HEADER_PATHS += $(SDK_ROOT)/external/zboss/include
      SDK_HEADER_PATHS += $(dir $(wildcard $(SDK_ROOT)/external/zboss/include/*/))
      SDK_HEADER_PATHS += $(SDK_ROOT)/external/zboss/addons
      SDK_HEADER_PATHS += $(SDK_ROOT)/external/zboss/zb_error
      SDK_HEADER_PATHS += $(wildcard $(SDK_ROOT)components/zigbee/*/)
      SDK_HEADER_PATHS += $(SDK_ROOT)/external/nRF-IEEE-802.15.4-radio-driver/src

      LIBS += $(NRF_SECURITY)lib/libmbedcrypto_glue.a
      LIBS += $(NRF_SECURITY)lib/libmbedcrypto_glue_cc310.a
      LIBS += $(NRF_SECURITY)lib/libmbedcrypto_glue_vanilla.a
      LIBS += $(NRF_SECURITY)lib/libmbedcrypto_cc310_backend.a
      LIBS += $(NRF_SECURITY)lib/libmbedcrypto_vanilla_backend.a
      LIBS += $(NRF_SECURITY)lib/libmbedtls_tls_vanilla.a
      LIBS += $(NRF_SECURITY)lib/libmbedtls_x509_vanilla.a
      LIBS += $(NRF_SECURITY)lib/libmbedtls_base_vanilla.a
      LIBS += $(NRF_SECURITY)lib/libnrf_cc310_platform_0.9.1.a
      LIBS += $(SDK_ROOT)external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.12.a
      LIBS += $(SDK_ROOT)external/nrf_cc310_bl/lib/cortex-m4/hard-float/libnrf_cc310_bl_0.9.12.a

    ifeq ($(ZIGBEE_ED), 1)
      LIBS += $(SDK_ROOT)/external/zboss/lib/gcc/libzboss.ed.a
      SDK_VARS += ZB_ED_ROLE
    else
      LIBS += $(SDK_ROOT)/external/zboss/lib/gcc/libzboss.a
    endif

      LIBS += $(SDK_ROOT)/external/zboss/lib/gcc/nrf52840/nrf_radio_driver.a

      SDK_VARS += ZB_TRACE_LEVEL=0 ZB_TRACE_MASK=0

    endif

  endif # SDK 16
endif # nrf52


# ---- Create variables for Configuration use

# Location of softdevice
SOFTDEVICE_PATH ?= $(SDK_ROOT)components/softdevice/$(SOFTDEVICE_MODEL)/hex/$(SOFTDEVICE_MODEL)_nrf52_$(SOFTDEVICE_VERSION)_softdevice.hex

# Flags for compiler
HEADER_INCLUDES = $(addprefix -I,$(SDK_HEADER_PATHS)) $(addprefix -I,$(REPO_HEADER_PATHS)) $(addprefix -I,$(BOARD_HEADER_PATHS)) $(addprefix -I,$(APP_HEADER_PATHS))
LINKER_INCLUDES = $(addprefix -L,$(SDK_LINKER_PATHS)) $(addprefix -L,$(BOARD_LINKER_PATHS))
SDK_DEFINES = $(addprefix -D,$(SDK_VARS)) $(addprefix -D,$(BOARD_VARS)) $(addprefix -D,$(APP_VARS)) $(PARAMS_DEFINE)

# Directories make searches for prerequisites
VPATH = $(SDK_SOURCE_PATHS) $(REPO_SOURCE_PATHS) $(BOARD_SOURCE_PATHS) $(APP_SOURCE_PATHS)

SOURCES = $(notdir $(APP_SOURCES)) $(notdir $(BOARD_SOURCES)) $(notdir $(SDK_SOURCES)) $(notdir $(REPO_SOURCES))
PBSRCS = $(filter %.proto,$(SOURCES))
PBOPTS = $(filter %.options,$(SOURCES))
PBGENS = $(PBSRCS:.proto=.pb.c) $(PBSRCS:.proto=.pb.h)
CSOURCES = $(filter %.c,$(SOURCES))
CSOURCES += $(filter %.c,$(PBGENS))
OBJS = $(addprefix $(BUILDDIR), $(CSOURCES:.c=.o))
DEBUG_OBJS = $(addprefix $(BUILDDIR), $(CSOURCES:.c=.o-debug))
DEPS = $(addprefix $(BUILDDIR), $(CSOURCES:.c=.d))

SOURCES_AS = $(notdir $(SDK_AS)) $(notdir $(BOARD_AS)) $(notdir $(APP_AS))
OBJS_AS = $(addprefix $(BUILDDIR), $(SOURCES_AS:.S=.os))
DEBUG_OBJS_AS = $(addprefix $(BUILDDIR), $(SOURCES_AS:.S=.os-debug))

endif

