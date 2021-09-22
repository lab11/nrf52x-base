PROJECT_NAME     := ot_nrf52811_libopenthread_radio_nrf52811
TARGETS          := libopenthread_radio_nrf52811
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../../..
PROJ_DIR := ../../..


# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/../openthread/src/core/api/diags_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/diags/factory_diags.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/heap_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/instance.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/instance_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/link_quality.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/link_raw.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/link_raw_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/logging.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/logging_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/mac_frame.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/mac_types.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/parse_cmdline.cpp \
  $(PROJ_DIR)/../openthread/src/core/radio/radio.cpp \
  $(PROJ_DIR)/../openthread/src/core/radio/radio_callbacks.cpp \
  $(PROJ_DIR)/../openthread/src/core/radio/radio_platform.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/random_manager.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/random_noncrypto_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/string.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/sub_mac.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/sub_mac_callbacks.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/tasklet.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/tasklet_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/timer.cpp \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include/mbedtls \
  $(PROJ_DIR)/../config \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/nrf52811 \
  $(PROJ_DIR) \
  $(PROJ_DIR)/../openthread/src/core \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/mdk \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/mbedtls_plat_config \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/cmsis \
  $(PROJ_DIR)/../openthread/include \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g1
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
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
CFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_THREAD_VERSION=2
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
CFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52811-config-check.h\""
CFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
CFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
CFLAGS += -DOPENTHREAD_EXAMPLES_SIMULATION=0
CFLAGS += -DOPENTHREAD_FTD=0
CFLAGS += -DOPENTHREAD_MTD=0
CFLAGS += -DOPENTHREAD_PLATFORM_POSIX=0
CFLAGS += -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0
CFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52811-config.h\""
CFLAGS += -DOPENTHREAD_RADIO=1
CFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
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
ASMFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_THREAD_VERSION=2
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52811-config-check.h\""
ASMFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
ASMFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
ASMFLAGS += -DOPENTHREAD_EXAMPLES_SIMULATION=0
ASMFLAGS += -DOPENTHREAD_FTD=0
ASMFLAGS += -DOPENTHREAD_MTD=0
ASMFLAGS += -DOPENTHREAD_PLATFORM_POSIX=0
ASMFLAGS += -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0
ASMFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52811-config.h\""
ASMFLAGS += -DOPENTHREAD_RADIO=1
ASMFLAGS += -DUART_AS_SERIAL_TRANSPORT=1


libopenthread_radio_nrf52811: CFLAGS += -D__HEAP_SIZE=4096
libopenthread_radio_nrf52811: CFLAGS += -D__STACK_SIZE=4096
libopenthread_radio_nrf52811: ASMFLAGS += -D__HEAP_SIZE=4096
libopenthread_radio_nrf52811: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: libopenthread_radio_nrf52811

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		libopenthread_radio_nrf52811

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(call define_library, $(TARGETS), $(PROJ_DIR)/core/radio/armgcc/libopenthread-radio-nrf52811.a)

define create_library
@echo Creating library: $($@)
$(NO_ECHO)$(AR) $($@) $^
@echo Done
endef
libopenthread_radio_nrf52811:
	$(create_library)

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
