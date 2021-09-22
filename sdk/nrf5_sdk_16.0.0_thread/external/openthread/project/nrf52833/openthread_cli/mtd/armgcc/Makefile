PROJECT_NAME     := ot_nrf52833_libopenthread_cli_mtd_nrf52833
TARGETS          := libopenthread_cli_mtd_nrf52833
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../../..
PROJ_DIR := ../../..


# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/../openthread/src/cli/cli.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_coap.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_coap_secure.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_commissioner.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_console.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_dataset.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_joiner.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_server.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_uart.cpp \
  $(PROJ_DIR)/../openthread/src/cli/cli_udp.cpp \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include/mbedtls \
  $(PROJ_DIR)/../config \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(PROJ_DIR) \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/nrf52833 \
  $(PROJ_DIR)/../openthread/src \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/libraries/crypto \
  $(PROJ_DIR)/../openthread/src/core \
  $(PROJ_DIR)/../openthread/third_party/NordicSemiconductor/nrfx/mdk \
  $(PROJ_DIR)/../openthread/src/cli \
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
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52833-mbedtls-config.h\""
CFLAGS += -DNRF52833_XXAA
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
CFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
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
CFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52833-config-check.h\""
CFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
CFLAGS += -DOPENTHREAD_ENABLE_POSIX_APP_DAEMON=0
CFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
CFLAGS += -DOPENTHREAD_MTD=1
CFLAGS += -DOPENTHREAD_PLATFORM_POSIX_APP=0
CFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52833-config.h\""
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
ASMFLAGS += -DDISABLE_CC310=1
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DHAVE_CONFIG_H
ASMFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
ASMFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52833-mbedtls-config.h\""
ASMFLAGS += -DNRF52833_XXAA
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
ASMFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
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
ASMFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52833-config-check.h\""
ASMFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
ASMFLAGS += -DOPENTHREAD_ENABLE_POSIX_APP_DAEMON=0
ASMFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
ASMFLAGS += -DOPENTHREAD_MTD=1
ASMFLAGS += -DOPENTHREAD_PLATFORM_POSIX_APP=0
ASMFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52833-config.h\""
ASMFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
ASMFLAGS += -D_BSD_SOURCE=1
ASMFLAGS += -D_DEFAULT_SOURCE=1


libopenthread_cli_mtd_nrf52833: CFLAGS += -D__HEAP_SIZE=4096
libopenthread_cli_mtd_nrf52833: CFLAGS += -D__STACK_SIZE=4096
libopenthread_cli_mtd_nrf52833: ASMFLAGS += -D__HEAP_SIZE=4096
libopenthread_cli_mtd_nrf52833: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: libopenthread_cli_mtd_nrf52833

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		libopenthread_cli_mtd_nrf52833

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(call define_library, $(TARGETS), $(PROJ_DIR)/openthread_cli/mtd/armgcc/libopenthread-cli-mtd-nrf52833.a)

define create_library
@echo Creating library: $($@)
$(NO_ECHO)$(AR) $($@) $^
@echo Done
endef
libopenthread_cli_mtd_nrf52833:
	$(create_library)

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
