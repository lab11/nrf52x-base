/**
 * Copyright (c) 2018 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stddef.h>

#include "zigbee_helpers.h"
#include "zb_error_handler.h"
#include "zboss_api.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "boards.h"
#include "nrfx_power.h"

#ifdef BOARD_PCA10056
#define ERASE_PIN    NRF_GPIO_PIN_MAP(1,9)                                          /**< Pin 1.09. */
#endif
#ifdef BOARD_PCA10059
#define ERASE_PIN    NRF_GPIO_PIN_MAP(1,6)                                          /**< Pin 1.06. */
#endif
#ifdef BOARD_PCA10100
#define ERASE_PIN    NRF_GPIO_PIN_MAP(1,6)                                          /**< Pin 1.06. */
#endif
#define READ_RETRIES 10                                                             /**< Number of retries until the pin value stabilizes. */

#ifndef ZB_DEV_REJOIN_TIMEOUT_MS
#define ZB_DEV_REJOIN_TIMEOUT_MS (1000 * 200)                                       /**< Timeout after which End Device stops to send beacons if can not join/rejoin a network. */
#endif
#define REJOIN_INTERVAL_MAX_S    (15 * 60)                                          /**< Maximum interval between join/rejoin attempts. */

#define RAM_START_ADDRESS           0x20000000UL                                    /**< Start address of RAM. */
#define RAM_BANK_0_7_SECTION_SIZE   0x1000                                          /**< Size of RAM section for banks 0-7. */
#define RAM_BANK_0_7_SECTIONS_NBR   2                                               /**< Number of sections in banks 0-7. */
#define RAM_BANK_8_SECTION_SIZE     0x8000                                          /**< Size of RAM section for bank 8. */
#define RAM_BANK_8_SECTIONS_NBR     6                                               /**< Number of sections in bank 8. */

/* Define address of RAM region which can be used by application.
 * GCC, SES compiler and Keil compiler has memory layout
 * with stack being placed at the end of the RAM.
 * For those compilers end of RAM can be determined by top of the stack.
 * IAR has different layout and address of end of RAM
 * is determined by linker symbol.
 */
#if defined ( __ICCARM__ )
#define RAM_END_ADDRESS ((uint32_t)&__ICFEDIT_region_RAM_end__)

extern char __ICFEDIT_region_RAM_end__;
#else
#define RAM_END_ADDRESS ((uint32_t)STACK_TOP)
#endif /* defined __ICCARM__ */

static zb_uint8_t           m_stack_initialised             = ZB_FALSE;
static zb_uint8_t           m_is_rejoin_procedure_started   = ZB_FALSE;
static zb_uint8_t           m_is_rejoin_stop_requested      = ZB_FALSE;
static zb_uint8_t           m_is_rejoin_in_progress         = ZB_FALSE;
static zb_uint8_t           m_rejoin_attempt_cnt            = 0;
#if defined ZB_ED_ROLE
static volatile zb_uint8_t  m_wait_for_user_input           = ZB_FALSE;
static volatile zb_uint8_t  m_is_rejoin_start_scheduled     = ZB_FALSE;
#endif

static void rejoin_the_network(zb_uint8_t param);
static void start_network_rejoin(void);
static void stop_network_rejoin(zb_uint8_t was_scheduled);

/**@brief Function to set the Erase persistent storage depending on the erase pin
 */
zb_void_t zigbee_erase_persistent_storage(zb_bool_t erase)
{
#if defined (BOARD_PCA10056) || defined (BOARD_PCA10059) || defined (BOARD_PCA10100)
    if (!erase)
    {
        nrf_gpio_cfg_input(ERASE_PIN, NRF_GPIO_PIN_PULLUP);

        int i;
        volatile uint32_t pin_state;

        /* Read a few times to make sure the value in the register has been updated. */
        for (i = 0; i < READ_RETRIES; i++)
        {
            pin_state = nrf_gpio_pin_read(ERASE_PIN);
        }

        if (pin_state == 0)
        {
            erase = ZB_TRUE;
            NRF_LOG_INFO("Forcing flash erasure due to pin state");
        }

        nrf_gpio_cfg_default(ERASE_PIN);
    }
#endif
    zb_set_nvram_erase_at_start(erase);
}

int to_hex_str(char * p_out, uint16_t out_size,
                  const uint8_t * p_in, uint8_t in_size,
                  bool reverse)
{
    int bytes_written = 0;
    int status;
    int i = reverse ? in_size - 1 : 0;

    for (; in_size > 0; in_size--)
    {
        status = snprintf(p_out + bytes_written,
                          out_size - bytes_written,
                          "%02x", p_in[i]);
        if (status < 0)
        {
            return status;
        }

        bytes_written += status;
        i += reverse ? -1 : 1;
    }

    return bytes_written;
}

int ieee_addr_to_str(char * p_str_buf, uint16_t buf_len, const zb_ieee_addr_t p_addr)
{
    return to_hex_str(p_str_buf, buf_len,
                      (const uint8_t *)p_addr, sizeof(zb_ieee_addr_t),
                      true);
}

uint8_t parse_hex_digit(const char c)
{
    uint8_t result = 0xff;

    if ((c >= '0') && (c <= '9'))
    {
        result = c - '0';
    }
    else if ((c >='a') && (c <= 'f'))
    {
        result = c - 'a' + 10;
    }
    else if ((c >='A') && (c <= 'F'))
    {
        result = c - 'A' + 10;
    }

    return result;
}

bool parse_hex_str(char const * p_in_str, uint8_t in_str_len,
                   uint8_t * p_out_buff, uint8_t out_buff_size,
                   bool reverse)
{
    uint8_t i     = 0;
    int8_t  delta = 1;

    // Skip 0x suffix if present.
    if ((in_str_len > 2) && (p_in_str[0] == '0') && (tolower(p_in_str[1]) == 'x'))
    {
        in_str_len -= 2;
        p_in_str += 2;
    }

    if (reverse)
    {
        p_in_str = p_in_str + in_str_len - 1;
        delta    = -1;
    }

    // Check if we have enough output space
    if (in_str_len > 2*out_buff_size)
    {
        return false;
    }

    memset(p_out_buff, 0, out_buff_size);

    while (i < in_str_len)
    {
        uint8_t nibble = parse_hex_digit(*p_in_str);
        if (nibble > 0x0f)
        {
            break;
        }

        if (i & 0x01)
        {
            *p_out_buff |= reverse ? nibble << 4 : nibble;
            p_out_buff++;
        }
        else
        {
            *p_out_buff = reverse ? nibble : nibble << 4;
        }

        i += 1;
        p_in_str += delta;
    }

    return (i == in_str_len);
}

addr_type_t parse_address(const char * input, zb_addr_u * addr, addr_type_t addr_type)
{
    addr_type_t result = ADDR_INVALID;
    size_t      len    = strlen(input);

    if (!input || !addr || !strlen(input))
    {
        return ADDR_INVALID;
    }

    // Skip 0x suffix if present.
    if ((input[0] == '0') && (tolower(input[1]) == 'x'))
    {
        input += 2;
        len   -= 2;
    }

    if ((len == 2 * sizeof(zb_ieee_addr_t)) &&
        (addr_type == ADDR_ANY || addr_type == ADDR_LONG))
    {
        result = ADDR_LONG;
    }
    else if ((len == 2 * sizeof(uint16_t)) &&
             (addr_type == ADDR_ANY || addr_type == ADDR_SHORT))
    {
        result = ADDR_SHORT;
    }
    else
    {
        return ADDR_INVALID;
    }

    return parse_hex_str(input, len, (uint8_t *)addr, len / 2, true) ? result : ADDR_INVALID;
}

zb_ret_t zigbee_default_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t  * p_sg_p      = NULL;
    zb_zdo_app_signal_type_t   sig         = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t                   status      = ZB_GET_APP_SIGNAL_STATUS(bufid);
    zb_nwk_device_type_t       role        = zb_get_network_role();
    zb_ret_t                   ret_code    = RET_OK;
    zb_bool_t                  comm_status = ZB_TRUE;

    switch(sig)
    {
        case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
            /* At this point Zigbee stack attempted to load production configuration from NVRAM.
             * This step is performed each time the stack is initialized.
             *
             * Note: if it is necessary for a device to have a valid production configuration to operate (e.g. due to legal reasons),
             *       the application should implement the customized logic for this signal (e.g. assert on the signal status code).
             */
            if (status != RET_OK)
            {
                NRF_LOG_INFO("Production configuration is not present or invalid (status: %d)", status);
            }
            else
            {
                zb_production_config_ver_1_t *prod_conf = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_production_config_ver_1_t);
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                NRF_LOG_INFO("Loading application production config");

                /* IEEE address is automatically parsed and set by the ZBOSS stack. */
                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), prod_conf->extended_address);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }
                NRF_LOG_INFO("\tIEEE address: %s", NRF_LOG_PUSH(ieee_addr_buf));

                /* Channel mask has to be parsed and applied manually. */
                zb_set_bdb_primary_channel_set(prod_conf->aps_channel_mask);
                zb_set_bdb_secondary_channel_set(prod_conf->aps_channel_mask);
                NRF_LOG_INFO("\tChannel mask %08lx", prod_conf->aps_channel_mask);

                NRF_LOG_INFO("Production configuration successfully loaded");
            }
            break;

        case ZB_ZDO_SIGNAL_SKIP_STARTUP:
            /* At this point Zigbee stack:
             *  - Initialized the scheduler.
             *  - Initialized and read NVRAM configuration.
             *  - Initialized all stack-related global variables.
             *
             * Next step: perform BDB initialization procedure (see BDB specification section 7.1).
             */
            m_stack_initialised = ZB_TRUE;
            NRF_LOG_INFO("Zigbee stack initialized");
            comm_status = bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
            break;

        case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
            /* At this point Zigbee stack is ready to operate and the BDB initialization procedure has finished.
             * There is no network configuration stored inside NVRAM.
             *
             * Next step:
             *  - If the device implements Zigbee router or Zigbee end device,
             *    perform network steering for a node not on a network (see BDB specification section 8.3).
             *  - If the device implements Zigbee coordinator,
             *    perform network formation (see BDB specification section 8.4).
             */
            NRF_LOG_INFO("Device started for the first time");
            if (status == RET_OK)
            {
                if (role != ZB_NWK_DEVICE_TYPE_COORDINATOR)
                {
                    NRF_LOG_INFO("Start network steering");
                    start_network_rejoin();
                }
                else
                {
                    NRF_LOG_INFO("Start network formation");
                    comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION);
                }
            }
            else
            {
                NRF_LOG_ERROR("Failed to initialize Zigbee stack (status: %d)", status);
            }
            break;

        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            /* At this point Zigbee stack is ready to operate and the BDB initialization procedure has finished.
             * There is network configuration stored inside NVRAM, so the device will try to rejoin.
             *
             * Next step: if the device implement Zigbee router or end device, and the initialization has failed,
             *            perform network steering for a node on a network (see BDB specification section 8.2).
             */
            if (status == RET_OK)
            {
                zb_ext_pan_id_t extended_pan_id;
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                zb_get_extended_pan_id(extended_pan_id);
                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), extended_pan_id);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }

                /* Device has joined the network so stop the network rejoin procedure. */
                stop_network_rejoin(ZB_FALSE);
                NRF_LOG_INFO("Joined network successfully on reboot signal (Extended PAN ID: %s, PAN ID: 0x%04hx)", NRF_LOG_PUSH(ieee_addr_buf), ZB_PIBCACHE_PAN_ID());
            }
            else
            {
                if (role != ZB_NWK_DEVICE_TYPE_COORDINATOR)
                {
                    NRF_LOG_INFO("Unable to join the network, start network steering");
                    start_network_rejoin();
                }
                else
                {
                    NRF_LOG_ERROR("Failed to initialize Zigbee stack using NVRAM data (status: %d)", status);
                }
            }
            break;

        case ZB_BDB_SIGNAL_STEERING:
            /* At this point the Zigbee stack has finished network steering procedure.
             * The device may have rejoined the network, which is indicated by signal's status code.
             *
             * Next step:
             *  - If the device implements Zigbee router and the steering is not successful,
             *    retry joining Zigbee network by starting network steering after 1 second.
             *  - It is not expected to finish network steering with error status if the device implements Zigbee coordinator (see BDB specification section 8.2).
             */
            if (status == RET_OK)
            {
                zb_ext_pan_id_t extended_pan_id;
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                zb_get_extended_pan_id(extended_pan_id);
                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), extended_pan_id);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }

                NRF_LOG_INFO("Joined network successfully (Extended PAN ID: %s, PAN ID: 0x%04hx)", NRF_LOG_PUSH(ieee_addr_buf), ZB_PIBCACHE_PAN_ID());

                /* Device has joined the network so stop the network rejoin procedure. */
                if (role != ZB_NWK_DEVICE_TYPE_COORDINATOR)
                {
                    stop_network_rejoin(ZB_FALSE);
                }
            }
            else
            {
                if (role != ZB_NWK_DEVICE_TYPE_COORDINATOR)
                {
                    NRF_LOG_INFO("Network steering was not successful (status: %d)", status);
                    start_network_rejoin();
                }
                else
                {
                    NRF_LOG_INFO("Network steering failed on Zigbee coordinator (status: %d)", status);
                }
            }
#ifndef ZB_ED_ROLE
            zb_enable_auto_pan_id_conflict_resolution(ZB_FALSE);
#endif
            break;

        case ZB_BDB_SIGNAL_FORMATION:
            /* At this point the Zigbee stack has finished network formation procedure.
             * The device may have created a new Zigbee network, which is indicated by signal's status code.
             *
             * Next step:
             *  - If the device implements Zigbee coordinator and the formation is not successful,
             *    try to form a new Zigbee network by performing network formation after 1 second (see BDB specification section 8.4).
             *  - If the network formation was successful, open the newly created network for other devices to join by starting
             *    network steering for a node on a network (see BDB specification section 8.2).
             *  - If the device implements Zigbee router or end device, this signal is not expected.
             */
            if (status == RET_OK)
            {
                zb_ext_pan_id_t extended_pan_id;
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                zb_get_extended_pan_id(extended_pan_id);
                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), extended_pan_id);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }

                NRF_LOG_INFO("Network formed successfully, start network steering (Extended PAN ID: %s, PAN ID: 0x%04hx)", NRF_LOG_PUSH(ieee_addr_buf), ZB_PIBCACHE_PAN_ID());
                comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
            }
            else
            {
                NRF_LOG_INFO("Restart network formation (status: %d)", status);
                ret_code = ZB_SCHEDULE_APP_ALARM(
                                   (zb_callback_t)bdb_start_top_level_commissioning,
                                   ZB_BDB_NETWORK_FORMATION,
                                   ZB_TIME_ONE_SECOND
                               );
            }
            break;

        case ZB_ZDO_SIGNAL_LEAVE:
            /* This signal is generated when the device itself has left the network by sending leave command.
             *
             * Note: this signal will be generated if the device tries to join legacy Zigbee network and the TCLK
             *       exchange cannot be completed. In such situation, the ZB_BDB_NETWORK_STEERING signal will be generated
             *       afterwards, so this case may be left unimplemented.
             */
            if (status == RET_OK)
            {
                zb_zdo_signal_leave_params_t * p_leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_params_t);
                NRF_LOG_INFO("Network left (leave type: %d)", p_leave_params->leave_type);

                /* Start network rejoin procedure */
                start_network_rejoin();
            }
            else
            {
                NRF_LOG_ERROR("Unable to leave network (status: %d)", status);
            }
#ifndef ZB_ED_ROLE
            zb_enable_auto_pan_id_conflict_resolution(ZB_FALSE);
#endif
            break;

        case ZB_ZDO_SIGNAL_LEAVE_INDICATION:
            /* This signal is generated on the parent to indicate, that one of its child nodes left the network.
             */
            {
                zb_zdo_signal_leave_indication_params_t * p_leave_ind_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_indication_params_t);
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), p_leave_ind_params->device_addr);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }
                NRF_LOG_INFO("Child left the network (long: %s, rejoin flag: %d)", NRF_LOG_PUSH(ieee_addr_buf), p_leave_ind_params->rejoin);
            }
            break;

#ifdef ZB_ED_ROLE
        case ZB_COMMON_SIGNAL_CAN_SLEEP:
            /* Zigbee stack can enter sleep state.
             * If the application wants to proceed, it should call zb_sleep_now() function.
             *
             * Note: if the application shares some resources between Zigbee stack and other tasks/contexts,
             *       device disabling should be overwritten by implementing one of the weak functions inside zb_nrf52840_common.c.
             */
            zb_sleep_now();
            break;
#endif

        case ZB_ZDO_SIGNAL_DEVICE_UPDATE:
            /* This signal notifies the Zigbee Trust center (usually implemented on the coordinator node)
             * or parent router application once a device joined, rejoined, or left the network.
             *
             * For more information see table 4.14 of the Zigbee Specification (R21).
             */
            {
                zb_zdo_signal_device_update_params_t * p_update_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_device_update_params_t);
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), p_update_params->long_addr);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }
                NRF_LOG_INFO("Device update received (short: 0x%04hx, long: %s, status: %d)",
                             p_update_params->short_addr, NRF_LOG_PUSH(ieee_addr_buf), p_update_params->status);
            }
            break;

        case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
            /* This signal is generated when a Device Announcement command is received by the device.
             * Such packet is generated whenever a node joins or rejoins the network, so this signal may be used to track the number of devices.
             *
             * Note: since the Device Announcement command is sent to the broadcast address, this method may miss some devices.
             *       The complete knowledge about nodes has only the coordinator.
             *
             * Note: it may happen, that a device broadcasts the Device Announcement command and is removed by the coordinator afterwards,
             *       due to security policy (lack of TCLK exchange).
             */
            {
                zb_zdo_signal_device_annce_params_t * dev_annce_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_device_annce_params_t);
                NRF_LOG_INFO("New device commissioned or rejoined (short: 0x%04hx)", dev_annce_params->device_short_addr);
            }
            break;

#ifndef ZB_ED_ROLE
        case ZB_ZDO_SIGNAL_DEVICE_AUTHORIZED:
            /* This signal notifies the Zigbee Trust center application (usually implemented on the coordinator node)
             * about authorization of a new device in the network.
             *
             * For Zigbee 3.0 (and newer) devices this signal is generated if:
             *  - TCKL exchange procedure was successful
             *  - TCKL exchange procedure timed out
             *
             * If the coordinator allows for legacy devices to join the network (enabled by zb_bdb_set_legacy_device_support(1) API call),
             * this signal is generated:
             *  - If the parent router generates Update Device command and the joining device does not perform TCLK exchange within timeout.
             *  - If the TCLK exchange is successful.
             */
            {
                zb_zdo_signal_device_authorized_params_t * p_authorize_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_device_authorized_params_t);
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), p_authorize_params->long_addr);
                if (addr_len < 0)
                {
                    strcpy(ieee_addr_buf, "unknown");
                }
                NRF_LOG_INFO("Device authorization event received (short: 0x%04hx, long: %s, authorization type: %d, authorization status: %d)",
                             p_authorize_params->short_addr, NRF_LOG_PUSH(ieee_addr_buf),
                             p_authorize_params->authorization_type, p_authorize_params->authorization_status);
            }
            break;
#endif

        case ZB_NWK_SIGNAL_NO_ACTIVE_LINKS_LEFT:
            /* This signal informs the application that all links to other routers has expired.
             * In such situation, the node can communicate only with its children.
             *
             * Example reasons of signal generation:
             *  - The device was brought too far from the rest of the network.
             *  - There was a power cut and the whole network suddenly disappeared.
             *
             * Note: This signal is not generated for the coordinator node, since it may operate alone
             *       in the network.
             */
            NRF_LOG_WARNING("Parent is unreachable");
            break;

        case ZB_BDB_SIGNAL_FINDING_AND_BINDING_TARGET_FINISHED:
            /* This signal informs the Finding & Binding target device that the procedure has finished
             * and the other device has been bound or the procedure timed out.
             */
            NRF_LOG_INFO("Find and bind target finished (status: %d)", status);
            break;

#ifndef ZB_ED_ROLE
        case ZB_NWK_SIGNAL_PANID_CONFLICT_DETECTED:
            /* This signal informs the Router and Coordinator that conflict PAN ID has been detected
             * and needs to be resolved. In order to do that *zb_start_pan_id_conflict_resolution* is called.
             */
            {
                NRF_LOG_INFO("PAN ID conflict detected, trying to resolve... ");

                zb_bufid_t buf_copy = zb_buf_get_out();
                if (buf_copy)
                {
                    zb_buf_copy(buf_copy, bufid);
                    ZVUNUSED(ZB_ZDO_SIGNAL_CUT_HEADER(buf_copy));

                    zb_start_pan_id_conflict_resolution(buf_copy);
                }
                else
                {
                    NRF_LOG_ERROR("No free buffer available, skipping conflict resolving this time.")
                }
            }
            break;
#endif

        case ZB_ZDO_SIGNAL_DEFAULT_START:
        case ZB_NWK_SIGNAL_DEVICE_ASSOCIATED:
            /* Obsolete signals, used for pre-R21 ZBOSS API. Ignore. */
            break;

        default:
            /* Unimplemented signal. For more information see: zb_zdo_app_signal_type_e and zb_ret_e. */
            NRF_LOG_INFO("Unimplemented signal (signal: %d, status: %d)", sig, status);
            break;
    }

    /* If configured, process network rejoin procedure. */
    rejoin_the_network(0);

    if ((ret_code == RET_OK) && (comm_status != ZB_TRUE))
    {
        ret_code = RET_ERROR;
    }

    return ret_code;
}

void zigbee_led_status_update(zb_bufid_t bufid, uint32_t led_idx)
{
    zb_zdo_app_signal_hdr_t  * p_sg_p = NULL;
    zb_zdo_app_signal_type_t   sig    = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t                   status = ZB_GET_APP_SIGNAL_STATUS(bufid);

    switch (sig)
    {
        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            /* fall-through */
        case ZB_BDB_SIGNAL_STEERING:
            if (status == RET_OK)
            {
                bsp_board_led_on(led_idx);
            }
            else
            {
                bsp_board_led_off(led_idx);
            }
            break;

        case ZB_ZDO_SIGNAL_LEAVE:
            /* Update network status LED */
            bsp_board_led_off(led_idx);
            break;

        default:
            break;
    }
}

/**@brief Start network steering.
 */
static void start_network_steering(zb_uint8_t param)
{
    ZVUNUSED(param);
    ZVUNUSED(bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING));
}

/**@brief Process rejoin procedure. To be called in signal handler.
 */
static void rejoin_the_network(zb_uint8_t param)
{
    ZVUNUSED(param);

    if (m_stack_initialised && m_is_rejoin_procedure_started)
    {
        if (m_is_rejoin_stop_requested)
        {
            m_is_rejoin_procedure_started = ZB_FALSE;
            m_is_rejoin_stop_requested = ZB_FALSE;

#if defined ZB_ED_ROLE
            NRF_LOG_INFO("Network rejoin procedure stopped as %sscheduled.", (m_wait_for_user_input) ? "" : "NOT ");
#elif defined ZB_ROUTER_ROLE
            NRF_LOG_INFO("Network rejoin procedure stopped.");
#endif
        }
        else if (!m_is_rejoin_in_progress)
        {
            /* Calculate new timeout */
            zb_time_t timeout_s;
            if ((1 << m_rejoin_attempt_cnt) > REJOIN_INTERVAL_MAX_S)
            {
                timeout_s = REJOIN_INTERVAL_MAX_S;
            }
            else
            {
                timeout_s = (1 << m_rejoin_attempt_cnt);
                m_rejoin_attempt_cnt++;
            }

            zb_ret_t zb_err_code = ZB_SCHEDULE_APP_ALARM(start_network_steering, ZB_FALSE, ZB_MILLISECONDS_TO_BEACON_INTERVAL(timeout_s * 1000));
            ZB_ERROR_CHECK(zb_err_code);

            m_is_rejoin_in_progress= ZB_TRUE;
        }
    }
}

/**@brief Function for starting rejoin network procedure.
 *
 * @note  For Router device if stack is initialised, device is not joined
 *        and rejoin procedure is not running, start rejoin procedure.
 *
 * @note  For End Device if stack is initialised, rejoin procedure is not running,
 *        device is not joined and device is not waiting for the user input, start rejoin procedure.
 *        Additionally, schedule alarm to stop rejoin procedure after the timeout defined by ZB_DEV_REJOIN_TIMEOUT_MS.
 */
static void start_network_rejoin(void)
{
#if defined ZB_ED_ROLE
    if (!ZB_JOINED() && m_stack_initialised && !m_wait_for_user_input)
#elif defined ZB_ROUTER_ROLE
    if (!ZB_JOINED() && m_stack_initialised)
#endif
    {
        m_is_rejoin_in_progress         = ZB_FALSE;

        if (!m_is_rejoin_procedure_started)
        {
            m_is_rejoin_procedure_started   = ZB_TRUE;
            m_is_rejoin_stop_requested      = ZB_FALSE;
            m_is_rejoin_in_progress         = ZB_FALSE;
            m_rejoin_attempt_cnt            = 0;

#if defined ZB_ED_ROLE
            m_wait_for_user_input           = ZB_FALSE;
            m_is_rejoin_start_scheduled     = ZB_FALSE;

            zb_ret_t zb_err_code = ZB_SCHEDULE_APP_ALARM(stop_network_rejoin, ZB_TRUE, ZB_MILLISECONDS_TO_BEACON_INTERVAL(ZB_DEV_REJOIN_TIMEOUT_MS));
            ZB_ERROR_CHECK(zb_err_code);
#endif

            NRF_LOG_INFO("Started network rejoin procedure.");
        }
    }
}

/**@brief Function for stopping rejoin network procedure and related scheduled alarms.
 *
 * @param[in] was_scheduled   Zigbee flag to indicate if the function was scheduled or called directly.
 */
static void stop_network_rejoin(zb_uint8_t was_scheduled)
{
    /* For Router and End Device:
     *   Try to stop scheduled network steering. Stop rejoin procedure or if no network steering was scheduled,
     *   request rejoin stop on next rejoin_the_network() call.
     * For End Device only:
     *   If stop_network_rejoin() was called from scheduler, the rejoin procedure has reached timeout,
     *   set m_wait_for_user_input to ZB_TRUE so the rejoin procedure can only be started by calling user_input_indicate().
     *   If not, set m_wait_for_user_input to ZB_FALSE.
     */

    zb_ret_t zb_err_code;

#if defined ZB_ED_ROLE
    /* Set m_wait_for_user_input depending on if the device should retry joining on user_input_indication() */
    m_wait_for_user_input = was_scheduled;
#elif defined ZB_ROUTER_ROLE
    ZVUNUSED(was_scheduled);
#endif

    if (m_is_rejoin_procedure_started)
    {
        zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(start_network_steering, ZB_ALARM_ANY_PARAM);
        if (zb_err_code == RET_OK)
        {
            /* Stop rejoin procedure */
            m_is_rejoin_procedure_started = ZB_FALSE;
            m_is_rejoin_stop_requested = ZB_FALSE;
#if defined ZB_ED_ROLE
            NRF_LOG_INFO("Network rejoin procedure stopped as %sscheduled.", (m_wait_for_user_input) ? "" : "not ");
#elif defined ZB_ROUTER_ROLE
            NRF_LOG_INFO("Network rejoin procedure stopped.");
#endif
        }
        else
        {
            /* Request rejoin procedure stop */
            m_is_rejoin_stop_requested = ZB_TRUE;
        }
    }

#if defined ZB_ED_ROLE
    /* Make sure scheduled stop alarm is canceled. */
    zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(stop_network_rejoin, ZB_ALARM_ANY_PARAM);
    if (zb_err_code != RET_NOT_FOUND)
    {
        ZB_ERROR_CHECK(zb_err_code);
    }
#endif
}

#if defined ZB_ED_ROLE
/* Function to be scheduled when user_input_indicate() is called and m_wait_for_user_input is ZB_TRUE */
static void start_network_rejoin_ED(zb_uint8_t param)
{
    ZVUNUSED(param);
    if (!ZB_JOINED() && m_wait_for_user_input)
    {
        zb_ret_t zb_err_code;

        m_wait_for_user_input = ZB_FALSE;
        start_network_rejoin();

        zb_err_code = ZB_SCHEDULE_APP_ALARM(rejoin_the_network, 0, ZB_TIME_ONE_SECOND);
        ZB_ERROR_CHECK(zb_err_code);
    }
    m_is_rejoin_start_scheduled = ZB_FALSE;
}

/* Function to be called by an application e.g. inside button handler function */
void user_input_indicate(void)
{
    if (m_wait_for_user_input && !(m_is_rejoin_start_scheduled))
    {
        zb_ret_t zb_err_code = RET_OK;

        zb_err_code = ZB_SCHEDULE_APP_CALLBACK(start_network_rejoin_ED, 0);
        ZB_ERROR_CHECK(zb_err_code);

        /* Prevent scheduling multiple rejoin starts */
        if (!zb_err_code)
        {
            m_is_rejoin_start_scheduled = ZB_TRUE;
        }
    }
}
#endif

/**@bried Calculate bottom addresss of RAM bank with given id.
 *
 * @param[in] bank_id  ID of RAM bank to get start address of.
 *
 * @return    Start address of RAM bank.
 */
static inline uint32_t ram_bank_bottom_addr(uint8_t bank_id)
{
    return RAM_START_ADDRESS + bank_id * RAM_BANK_0_7_SECTION_SIZE * RAM_BANK_0_7_SECTIONS_NBR;
}

/* Function for powering down unused RAM. */
void zigbee_power_down_unused_ram(void)
{
#if defined(NRF52811_XXAA) || defined(NRF52833_XXAA) || defined(NRF52840_XXAA)

    /* ID of top RAM bank */
#if defined(NRF52811_XXAA)
    uint8_t     bank_id                 = 3;
#elif defined(NRF52833_XXAA) || defined(NRF52840_XXAA)
    uint8_t     bank_id                 = 8;
#endif
    uint8_t     section_id              = 5;
    uint32_t    section_size            = 0;
    /* Mask to power down whole RAM bank */
    uint32_t    ram_bank_power_off_mask = 0xFFFFFFFF;

    /* Power off banks with unused RAM only */
    while (RAM_END_ADDRESS <= ram_bank_bottom_addr(bank_id))
    {
        NRF_LOG_DEBUG("Powering off bank: %d.", bank_id);
#ifdef SOFTDEVICE_PRESENT
        ret_code_t ret_val;

        ret_val = sd_power_ram_power_clr(bank_id, ram_bank_power_off_mask);
        APP_ERROR_CHECK(ret_val);
#else
        nrf_power_rampower_mask_off(bank_id, ram_bank_power_off_mask);
#endif /* defined SOFTDEVICE_PRESENT*/
        bank_id--;
    }

    /* Set id of top section and section size for given bank */
    section_id = (bank_id == 8) ? (RAM_BANK_8_SECTIONS_NBR - 1) : (RAM_BANK_0_7_SECTIONS_NBR - 1);
    section_size = (bank_id == 8) ? RAM_BANK_8_SECTION_SIZE : RAM_BANK_0_7_SECTION_SIZE;

    /* Power off remaining sections of unused RAM */
    while (RAM_END_ADDRESS <= (ram_bank_bottom_addr(bank_id) + section_id * section_size))
    {
        NRF_LOG_DEBUG("Powering off section %d of bank %d.", section_id, bank_id);
#ifdef SOFTDEVICE_PRESENT
        ret_code_t ret_val;

        ret_val = sd_power_ram_power_clr(bank_id, ((NRF_POWER_RAMPOWER_S0POWER << section_id) |
                                                   (NRF_POWER_RAMPOWER_S0RETENTION << section_id)));
        APP_ERROR_CHECK(ret_val);
#else
        nrf_power_rampower_mask_off(bank_id, ((NRF_POWER_RAMPOWER_S0POWER << section_id) |
                                              (NRF_POWER_RAMPOWER_S0RETENTION << section_id)));
#endif /* defined SOFTDEVICE_PRESENT*/

        section_id--;
    }

#else
#warning "Unsupported MCU - No RAM is powered down"
#endif /* defined NRF52811_XXAA || defined NRF52833_XXAA || defined NRF52840_XXAA */
}
