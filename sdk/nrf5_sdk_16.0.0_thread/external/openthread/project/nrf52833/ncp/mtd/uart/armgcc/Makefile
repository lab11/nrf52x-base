PROJECT_NAME     := ot_nrf52833_libopenthread_ncp_mtd_nrf52833
TARGETS          := libopenthread_ncp_mtd_nrf52833
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../../../..
PROJ_DIR := ../../../..


# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/../openthread/src/ncp/changed_props_set.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_base.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_base_dispatcher.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_base_ftd.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_base_mtd.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_base_radio.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_spi.cpp \
  $(PROJ_DIR)/../openthread/src/ncp/ncp_uart.cpp \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/../openthread/src \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/nrf_security/mbedtls_plat_config \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include/mbedtls \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/cmsis \
  $(PROJ_DIR)/../openthread/include \
  $(PROJ_DIR)/../openthread/third_party \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(PROJ_DIR)/../openthread/src/core \
  $(PROJ_DIR)/../config \
  $(PROJ_DIR)/../openthread/src/ncp \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/nrf52833 \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/mdk \
  $(PROJ_DIR) \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g1
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DDISABLE_CC310=1
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52833-mbedtls-config.h\""
CFLAGS += -DNRF52833_XXAA
CFLAGS += -DOPENTHREAD_CONFIG_BORDER_AGENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COMMISSIONER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DIAG_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ECDSA_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
CFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_JAM_DETECTION_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_JOINER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_LINK_RAW_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_THREAD_VERSION=2
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
CFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52833-config-check.h\""
CFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
CFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
CFLAGS += -DOPENTHREAD_EXAMPLES_SIMULATION=0
CFLAGS += -DOPENTHREAD_FTD=0
CFLAGS += -DOPENTHREAD_MTD=1
CFLAGS += -DOPENTHREAD_PLATFORM_POSIX=0
CFLAGS += -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0
CFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52833-config.h\""
CFLAGS += -DOPENTHREAD_RADIO=0
CFLAGS += -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1
CFLAGS += -DSPINEL_PLATFORM_HEADER="\"spinel_platform.h\""
CFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
CFLAGS += -D_GNU_SOURCE
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
ASMFLAGS += -DDISABLE_CC310=1
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DHAVE_CONFIG_H
ASMFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
ASMFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52833-mbedtls-config.h\""
ASMFLAGS += -DNRF52833_XXAA
ASMFLAGS += -DOPENTHREAD_CONFIG_BORDER_AGENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COMMISSIONER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DIAG_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ECDSA_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
ASMFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_JAM_DETECTION_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_JOINER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_LINK_RAW_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_THREAD_VERSION=2
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52833-config-check.h\""
ASMFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
ASMFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
ASMFLAGS += -DOPENTHREAD_EXAMPLES_SIMULATION=0
ASMFLAGS += -DOPENTHREAD_FTD=0
ASMFLAGS += -DOPENTHREAD_MTD=1
ASMFLAGS += -DOPENTHREAD_PLATFORM_POSIX=0
ASMFLAGS += -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0
ASMFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52833-config.h\""
ASMFLAGS += -DOPENTHREAD_RADIO=0
ASMFLAGS += -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1
ASMFLAGS += -DSPINEL_PLATFORM_HEADER="\"spinel_platform.h\""
ASMFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
ASMFLAGS += -D_GNU_SOURCE


libopenthread_ncp_mtd_nrf52833: CFLAGS += -D__HEAP_SIZE=4096
libopenthread_ncp_mtd_nrf52833: CFLAGS += -D__STACK_SIZE=4096
libopenthread_ncp_mtd_nrf52833: ASMFLAGS += -D__HEAP_SIZE=4096
libopenthread_ncp_mtd_nrf52833: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: libopenthread_ncp_mtd_nrf52833

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		libopenthread_ncp_mtd_nrf52833

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(call define_library, $(TARGETS), $(PROJ_DIR)/ncp/mtd/uart/armgcc/libopenthread-ncp-mtd-nrf52833.a)

define create_library
@echo Creating library: $($@)
$(NO_ECHO)$(AR) $($@) $^
@echo Done
endef
libopenthread_ncp_mtd_nrf52833:
	$(create_library)

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
