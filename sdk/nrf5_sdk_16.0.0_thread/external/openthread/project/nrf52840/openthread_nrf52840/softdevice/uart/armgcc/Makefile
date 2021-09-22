PROJECT_NAME     := ot_nrf52840_libopenthread_nrf52840_softdevice_sdk
TARGETS          := libopenthread_nrf52840_softdevice_sdk
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
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/flash_sd.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/logging.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/misc.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/radio.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/softdevice.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/spi-slave.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/system.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/temp.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/uart.c \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src/usb-cdc-uart.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/../openthread/examples/platforms \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/rsch/raal/softdevice \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/fem/three_pin_gpio \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/config \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/hal \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/atfifo \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/mbedtls_plat_config \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/src \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/common \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/dependencies \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/systick \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/softdevice/s140/headers \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/usb/class/cdc \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/mdk \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/nrf_cc310_plat/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/utf_converter \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/rsch \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor \
  $(PROJ_DIR)/../openthread/src/core \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/mac_features/ack_generator \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/rsch/raal \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/usb \
  $(PROJ_DIR) \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/usb/class/cdc/acm \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/mac_features \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/atomic \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/nrf52840 \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/drivers/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/usbd \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/power \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/usb/config \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/app_error \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/block_dev \
  $(PROJ_DIR)/../openthread/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/delay \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/cmsis \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/drivers \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/radio/hal \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/drivers/clock \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/soc \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/softdevice/s140/headers/nrf52 \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g1
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -DMBEDTLS_CONFIG_FILE="\"nrf-config.h\""
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52840-mbedtls-config.h\""
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DOPENTHREAD_CONFIG_BORDER_AGENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COMMISSIONER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DIAG_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ECDSA_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=0
CFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_JOINER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_LINK_RAW_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
CFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52840-config-check.h\""
CFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
CFLAGS += -DOPENTHREAD_ENABLE_POSIX_APP_DAEMON=0
CFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
CFLAGS += -DOPENTHREAD_PLATFORM_POSIX_APP=0
CFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52840-config.h\""
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
CFLAGS += -D_BSD_SOURCE=1
CFLAGS += -D_DEFAULT_SOURCE=1
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT) -fno-rtti -fno-exceptions
# Assembler flags common to all targets
ASMFLAGS += -g1
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DHAVE_CONFIG_H
ASMFLAGS += -DMBEDTLS_CONFIG_FILE="\"nrf-config.h\""
ASMFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52840-mbedtls-config.h\""
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DOPENTHREAD_CONFIG_BORDER_AGENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COMMISSIONER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DIAG_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ECDSA_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=0
ASMFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_JOINER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_LINK_RAW_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52840-config-check.h\""
ASMFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
ASMFLAGS += -DOPENTHREAD_ENABLE_POSIX_APP_DAEMON=0
ASMFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
ASMFLAGS += -DOPENTHREAD_PLATFORM_POSIX_APP=0
ASMFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52840-config.h\""
ASMFLAGS += -DS140
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
ASMFLAGS += -D_BSD_SOURCE=1
ASMFLAGS += -D_DEFAULT_SOURCE=1


libopenthread_nrf52840_softdevice_sdk: CFLAGS += -D__HEAP_SIZE=4096
libopenthread_nrf52840_softdevice_sdk: CFLAGS += -D__STACK_SIZE=4096
libopenthread_nrf52840_softdevice_sdk: ASMFLAGS += -D__HEAP_SIZE=4096
libopenthread_nrf52840_softdevice_sdk: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: libopenthread_nrf52840_softdevice_sdk

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		libopenthread_nrf52840_softdevice_sdk

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(call define_library, $(TARGETS), $(PROJ_DIR)/openthread_nrf52840/softdevice/uart/armgcc/libopenthread-nrf52840-softdevice-sdk.a)

define create_library
@echo Creating library: $($@)
$(NO_ECHO)$(AR) $($@) $^
@echo Done
endef
libopenthread_nrf52840_softdevice_sdk:
	$(create_library)

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
