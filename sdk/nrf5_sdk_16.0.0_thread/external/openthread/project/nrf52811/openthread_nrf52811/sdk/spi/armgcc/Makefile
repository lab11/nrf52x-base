PROJECT_NAME     := ot_nrf52811_libopenthread_nrf52811_sdk_spi
TARGETS          := libopenthread_nrf52811_sdk_spi
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../../../..
PROJ_DIR := ../../../..


# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/alarm.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/diag.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/entropy.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/fem.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/flash.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/flash_nosd.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/logging.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/misc.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/radio.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/spi-slave.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/system.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/temp.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/uart.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/../openthread/examples/platforms \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/fem/three_pin_gpio \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/atfifo \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/mbedtls_plat_config \
  $(PROJ_DIR)/../openthread/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/common \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/dependencies \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/systick \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/mdk \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/nrf52811 \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/utf_converter \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/rsch \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor \
  $(PROJ_DIR)/../openthread/src/core \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/mac_features/ack_generator \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/rsch/raal \
  $(PROJ_DIR) \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/hal \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include/mbedtls \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/mac_features \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/atomic \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/crypto \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/power \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/drivers/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/app_error \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/block_dev \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/delay \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/cmsis \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/drivers \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/hal \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/clock \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/soc \
  $(PROJ_DIR)/../config \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g1
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DDISABLE_CC310=1
CFLAGS += -DFLOAT_ABI_SOFT
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52811-mbedtls-config.h\""
CFLAGS += -DNRF52811_XXAA
CFLAGS += -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DIAG_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
CFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_LINK_RAW_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_NCP_SPI_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=0
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
CFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52811-config-check.h\""
CFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
CFLAGS += -DOPENTHREAD_ENABLE_POSIX_APP_DAEMON=0
CFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
CFLAGS += -DOPENTHREAD_PLATFORM_POSIX_APP=0
CFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52811-config.h\""
CFLAGS += -DRAAL_SINGLE_PHY=1
CFLAGS += -DSPIS_AS_SERIAL_TRANSPORT=1
CFLAGS += -D_BSD_SOURCE=1
CFLAGS += -D_DEFAULT_SOURCE=1
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=soft
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT) -fno-rtti -fno-exceptions
# Assembler flags common to all targets
ASMFLAGS += -g1
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=soft
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DDISABLE_CC310=1
ASMFLAGS += -DFLOAT_ABI_SOFT
ASMFLAGS += -DHAVE_CONFIG_H
ASMFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
ASMFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52811-mbedtls-config.h\""
ASMFLAGS += -DNRF52811_XXAA
ASMFLAGS += -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DIAG_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
ASMFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_LINK_RAW_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_NCP_SPI_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=0
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52811-config-check.h\""
ASMFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
ASMFLAGS += -DOPENTHREAD_ENABLE_POSIX_APP_DAEMON=0
ASMFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
ASMFLAGS += -DOPENTHREAD_PLATFORM_POSIX_APP=0
ASMFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52811-config.h\""
ASMFLAGS += -DRAAL_SINGLE_PHY=1
ASMFLAGS += -DSPIS_AS_SERIAL_TRANSPORT=1
ASMFLAGS += -D_BSD_SOURCE=1
ASMFLAGS += -D_DEFAULT_SOURCE=1


libopenthread_nrf52811_sdk_spi: CFLAGS += -D__HEAP_SIZE=4096
libopenthread_nrf52811_sdk_spi: CFLAGS += -D__STACK_SIZE=4096
libopenthread_nrf52811_sdk_spi: ASMFLAGS += -D__HEAP_SIZE=4096
libopenthread_nrf52811_sdk_spi: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: libopenthread_nrf52811_sdk_spi

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		libopenthread_nrf52811_sdk_spi

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(call define_library, $(TARGETS), $(PROJ_DIR)/openthread_nrf52811/sdk/spi/armgcc/libopenthread-nrf52811-sdk-spi.a)

define create_library
@echo Creating library: $($@)
$(NO_ECHO)$(AR) $($@) $^
@echo Done
endef
libopenthread_nrf52811_sdk_spi:
	$(create_library)

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
