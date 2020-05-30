PROJECT_NAME     := ot_nrf52833_libopenthread_ftd_optimized_ftd_nrf52833
TARGETS          := libopenthread_ftd_optimized_ftd_nrf52833
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../../..
PROJ_DIR := ../../..


# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/../openthread/src/core/thread/address_resolver.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/aes_ccm.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/aes_ecb.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/announce_begin_client.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/announce_begin_server.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/announce_sender.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/backbone_router_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/backbone_router_ftd_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/border_agent.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/border_router_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/channel_manager.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/channel_manager_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/channel_mask.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/channel_monitor.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/channel_monitor_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/child_supervision.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/child_supervision_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/child_table.cpp \
  $(PROJ_DIR)/../openthread/src/core/coap/coap.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/coap_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/coap/coap_message.cpp \
  $(PROJ_DIR)/../openthread/src/core/coap/coap_secure.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/coap_secure_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/commissioner.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/commissioner_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/crc16.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/crypto_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/data_poll_handler.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/data_poll_sender.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/dataset.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/dataset_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/dataset_ftd_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/dataset_local.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/dataset_manager.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/dataset_manager_ftd.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/dhcp6_client.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/dhcp6_server.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/diags_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/dns_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/dns_client.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/dtls.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/ecdsa.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/energy_scan_client.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/energy_scan_server.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/entropy_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/diags/factory_diags.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/flash.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/heap.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/heap_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/hmac_sha256.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/icmp6.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/icmp6_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/indirect_sender.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/instance.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/instance_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/ip6.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/ip6_address.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/ip6_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/ip6_filter.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/ip6_headers.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/ip6_mpl.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/jam_detection_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/jam_detector.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/joiner.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/joiner_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/joiner_router.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/key_manager.cpp \
  $(PROJ_DIR)/../openthread/src/core/backbone_router/leader.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/link_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/link_quality.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/link_raw.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/link_raw_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/backbone_router/local.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/logging.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/logging_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/lowpan.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/mac.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/mac_filter.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/mac_frame.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/mac_types.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/mbedtls.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/mesh_forwarder.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/mesh_forwarder_ftd.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/mesh_forwarder_mtd.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/meshcop.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/meshcop_leader.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/meshcop_tlvs.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/message.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/message_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/mle.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/mle_router.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/mle_types.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/netdata_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/netdiag_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/netif.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/network_data.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/network_data_leader.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/network_data_leader_ftd.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/network_data_local.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/network_diagnostic.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/network_time_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/notifier.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/otns.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/panid_query_client.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/panid_query_server.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/parse_cmdline.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/pbkdf2_cmac.cpp \
  $(PROJ_DIR)/../openthread/src/core/radio/radio.cpp \
  $(PROJ_DIR)/../openthread/src/core/radio/radio_callbacks.cpp \
  $(PROJ_DIR)/../openthread/src/core/radio/radio_platform.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/random_crypto_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/random_manager.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/random_noncrypto_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/router_table.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/server_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/settings.cpp \
  $(PROJ_DIR)/../openthread/src/core/crypto/sha256.cpp \
  $(PROJ_DIR)/../openthread/src/core/utils/slaac_address.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/sntp_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/sntp_client.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/src_match_controller.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/string.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/sub_mac.cpp \
  $(PROJ_DIR)/../openthread/src/core/mac/sub_mac_callbacks.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/tasklet.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/tasklet_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/thread_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/thread_ftd_api.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/thread_netif.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/time_sync_service.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/timer.cpp \
  $(PROJ_DIR)/../openthread/src/core/meshcop/timestamp.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/tlvs.cpp \
  $(PROJ_DIR)/../openthread/src/core/thread/topology.cpp \
  $(PROJ_DIR)/../openthread/src/core/common/trickle_timer.cpp \
  $(PROJ_DIR)/../openthread/src/core/net/udp6.cpp \
  $(PROJ_DIR)/../openthread/src/core/api/udp_api.cpp \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/../openthread/third_party/mbedtls/repo/include/mbedtls \
  $(PROJ_DIR)/../config \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(PROJ_DIR) \
  $(PROJ_DIR)/../openthread/examples/platforms/nrf528xx/nrf52833 \
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
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DHAVE_CONFIG_H
CFLAGS += -DMBEDTLS_CONFIG_FILE="\"mbedtls-config.h\""
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE="\"nrf52833-mbedtls-config.h\""
CFLAGS += -DNRF52833_XXAA
CFLAGS += -DOPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ECDSA_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
CFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_JAM_DETECTION_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_JOINER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_THREAD_VERSION=2
CFLAGS += -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1
CFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
CFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52833-config-check.h\""
CFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
CFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
CFLAGS += -DOPENTHREAD_EXAMPLES_SIMULATION=0
CFLAGS += -DOPENTHREAD_FTD=1
CFLAGS += -DOPENTHREAD_MTD=0
CFLAGS += -DOPENTHREAD_PLATFORM_POSIX=0
CFLAGS += -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0
CFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52833-config.h\""
CFLAGS += -DOPENTHREAD_RADIO=0
CFLAGS += -DUART_AS_SERIAL_TRANSPORT=1
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
ASMFLAGS += -DOPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ECDSA_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=1
ASMFLAGS += -DOPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_JAM_DETECTION_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_JOINER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_NCP_UART_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_THREAD_VERSION=2
ASMFLAGS += -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
ASMFLAGS += -DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="\"openthread-core-nrf52833-config-check.h\""
ASMFLAGS += -DOPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
ASMFLAGS += -DOPENTHREAD_ENABLE_VENDOR_EXTENSION=0
ASMFLAGS += -DOPENTHREAD_EXAMPLES_SIMULATION=0
ASMFLAGS += -DOPENTHREAD_FTD=1
ASMFLAGS += -DOPENTHREAD_MTD=0
ASMFLAGS += -DOPENTHREAD_PLATFORM_POSIX=0
ASMFLAGS += -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=0
ASMFLAGS += -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="\"openthread-core-nrf52833-config.h\""
ASMFLAGS += -DOPENTHREAD_RADIO=0
ASMFLAGS += -DUART_AS_SERIAL_TRANSPORT=1


libopenthread_ftd_optimized_ftd_nrf52833: CFLAGS += -D__HEAP_SIZE=4096
libopenthread_ftd_optimized_ftd_nrf52833: CFLAGS += -D__STACK_SIZE=4096
libopenthread_ftd_optimized_ftd_nrf52833: ASMFLAGS += -D__HEAP_SIZE=4096
libopenthread_ftd_optimized_ftd_nrf52833: ASMFLAGS += -D__STACK_SIZE=4096

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm -lstdc++


.PHONY: default help

# Default target - first one defined
default: libopenthread_ftd_optimized_ftd_nrf52833

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		libopenthread_ftd_optimized_ftd_nrf52833

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(call define_library, $(TARGETS), $(PROJ_DIR)/core/optim-ftd/armgcc/libopenthread-ftd-optimized-ftd-nrf52833.a)

define create_library
@echo Creating library: $($@)
$(NO_ECHO)$(AR) $($@) $^
@echo Done
endef
libopenthread_ftd_optimized_ftd_nrf52833:
	$(create_library)

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
