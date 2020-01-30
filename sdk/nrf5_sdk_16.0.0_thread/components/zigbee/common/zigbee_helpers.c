/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
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
                    comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
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

                NRF_LOG_INFO("Joined network successfully on reboot signal (Extended PAN ID: %s, PAN ID: 0x%04hx)", NRF_LOG_PUSH(ieee_addr_buf), ZB_PIBCACHE_PAN_ID());
            }
            else
            {
                if (role != ZB_NWK_DEVICE_TYPE_COORDINATOR)
                {
                    NRF_LOG_INFO("Unable to join the network, start network steering");
                    comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
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
            }
            else
            {
                if (role != ZB_NWK_DEVICE_TYPE_COORDINATOR)
                {
                    NRF_LOG_INFO("Restart network steering after 1 second (status: %d)", status);
                    ret_code = ZB_SCHEDULE_APP_ALARM(
                                    (zb_callback_t)bdb_start_top_level_commissioning,
                                    ZB_BDB_NETWORK_STEERING,
                                    ZB_TIME_ONE_SECOND
                                   );
                }
                else
                {
                    NRF_LOG_INFO("Network steering failed on Zigbee coordinator (status: %d)", status);
                }
            }
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
            }
            else
            {
                NRF_LOG_ERROR("Unable to leave network (status: %d)", status);
            }
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

        case ZB_ZDO_SIGNAL_DEFAULT_START:
        case ZB_NWK_SIGNAL_DEVICE_ASSOCIATED:
            /* Obsolete signals, used for pre-R21 ZBOSS API. Ignore. */
            break;

        default:
            /* Unimplemented signal. For more information see: zb_zdo_app_signal_type_e and zb_ret_e. */
            NRF_LOG_INFO("Unimplemented signal (signal: %d, status: %d)", sig, status);
            break;
    }

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
