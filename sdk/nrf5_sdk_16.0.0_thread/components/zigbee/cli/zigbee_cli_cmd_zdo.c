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
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "nrf_cli.h"
#include "zboss_api.h"
#include "zb_error_handler.h"
#include "zigbee_cli.h"
#include "zigbee_cli_utils.h"

/**
 * @defgroup zb_cli_cmd_zdo ZDO commands
 * @ingroup zb_cli
 *
 * @{
 */

// Defines how many ZDO requests can be run concurrently.
#define ZIGBEE_CLI_ZDO_TSN                 3
// Defines how long to wait, in seconds, for Match Descriptor Response.
#define ZIGBEE_CLI_MATCH_DESC_RESP_TIMEOUT 5
// Defines how long to wait, in seconds, for Bind Response.
#define ZIGBEE_CLI_BIND_RESP_TIMEOUT       5
// Defines how long to wait, in seconds, for Network Addrees Response.
#define ZIGBEE_CLI_NWK_ADDR_RESP_TIMEOUT   5
// Defines how long to wait, in seconds, for IEEE (EUI64) Addrees Response.
#define ZIGBEE_CLI_IEEE_ADDR_RESP_TIMEOUT  5
// Defines how long to wait, in seconds, for mgmt_leave response
#define ZIGBEE_CLI_MGMT_LEAVE_RESP_TIMEOUT  5

typedef struct {
    zb_uint8_t  start_index; /*!< Starting Index for the requested elements. */
    zb_uint16_t dst_addr;   /*!< Destination address. */
} req_seq_t;

// This structure allows for binding ZBOSS transaction and CLI object.
typedef struct zdo_tsn_ctx {
    nrf_cli_t const * p_cli;
    bool              (*p_cb)(struct zdo_tsn_ctx * zdo_tsn_ctx, uint8_t param);
    uint8_t           tsn;
    bool              taken;
    bool              is_broadcast;

    union {
        // Extra context for commands which request tables.
        req_seq_t req_seq;
    } cmd_ctx;

} zdo_tsn_ctx_t;

static zdo_tsn_ctx_t m_tsn_ctx[ZIGBEE_CLI_ZDO_TSN];

/**
 * @brief Return a pointer to context with the given transaction sequence number.
 *
 * @param[in] tsn ZBOSS transaction sequence number.
 *
 * @return a pointer to context or NULL if context for given TSN wasn't found.
 */
static zdo_tsn_ctx_t * get_ctx_by_tsn(uint8_t tsn)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(m_tsn_ctx); i++)
    {
        if ((m_tsn_ctx[i].taken == true) && (m_tsn_ctx[i].tsn == tsn))
        {
            return &m_tsn_ctx[i];
        }
    }

    return NULL;
}

/**
 * @brief Get a pointer to a free context.
 *
 * @return a pointer to context structure or NULL if all contexts are taken.
 */
static zdo_tsn_ctx_t * get_free_ctx(void)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(m_tsn_ctx); i++)
    {
        if (!m_tsn_ctx[i].taken)
        {
            m_tsn_ctx[i].taken = true;

            return &m_tsn_ctx[i];
        }
    }

    return NULL;
}

/**
 * Invalidate context.
 *
 * @param[in] p_tsn_ctx a pointer to transaction context.
 */
static void invalidate_ctx(zdo_tsn_ctx_t * p_tsn_ctx)
{
    p_tsn_ctx->taken        = false;
    p_tsn_ctx->tsn          = 0xFF;
    p_tsn_ctx->p_cli        = NULL;
    p_tsn_ctx->p_cb         = NULL;
    p_tsn_ctx->is_broadcast = false;
}

/**
 * @brief Parse a list of cluster IDs.
 *
 * @param[in]  pp_argv Pointer to argument table.
 * @param[in]  num     Number of cluster IDs to scan.
 * @param[out] pp_id   Pointer to an array to store cluster IDs.
 *
 * @return 1 if parsing succeeded, 0 otherwise.
 *
 */
static int sscan_cluster_list(char ** pp_argv, uint8_t num, uint16_t * pp_id)
{
    uint16_t len = 0;

    while ((len < num) && parse_hex_u16(pp_argv[len], pp_id))
    {
        len += 1;
        pp_id += 1;
    }

    return (len == num);
}

/**
 * @brief Handles timeout error and invalidates match descriptor request transaction.
 *
 * @param[in] tsn ZBOSS transaction sequence number.
 */
static void cmd_zb_match_desc_timeout(zb_uint8_t tsn)
{
    zdo_tsn_ctx_t * p_tsn_ctx = get_ctx_by_tsn(tsn);

    if (!p_tsn_ctx)
    {
        return;
    }

    print_done(p_tsn_ctx->p_cli, ZB_TRUE);
    invalidate_ctx(p_tsn_ctx);
}

/**
 * @brief A callback called on match descriptor response.
 *
 * @param[in] bufid Reference number to ZBOSS memory buffer.
 */
static void cmd_zb_match_desc_cb(zb_bufid_t bufid)
{
    zb_zdo_match_desc_resp_t   * p_resp = (zb_zdo_match_desc_resp_t *)zb_buf_begin(bufid);
    zb_apsde_data_indication_t * p_ind  = ZB_BUF_GET_PARAM(bufid, zb_apsde_data_indication_t);
    zdo_tsn_ctx_t              * p_tsn_ctx;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);

    if (p_tsn_ctx)
    {
        if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
        {
            zb_uint8_t * p_match_ep = (zb_uint8_t *)(p_resp + 1);

            nrf_cli_fprintf(p_tsn_ctx->p_cli, NRF_CLI_NORMAL, "\r\n");
            while (p_resp->match_len > 0)
            {
                /* Match EP list follows right after response header */
                nrf_cli_fprintf(p_tsn_ctx->p_cli,
                                NRF_CLI_NORMAL,
                                "src_addr=%0hx ep=%d\r\n",
                                p_ind->src_addr,
                                *p_match_ep);

                p_match_ep += 1;
                p_resp->match_len -= 1;
            }

            if (!p_tsn_ctx->is_broadcast)
            {
                print_done(p_tsn_ctx->p_cli, ZB_TRUE);
                invalidate_ctx(p_tsn_ctx);
            }
        }
        else if (p_resp->status == ZB_ZDP_STATUS_TIMEOUT)
        {
            print_done(p_tsn_ctx->p_cli, ZB_TRUE);
            invalidate_ctx(p_tsn_ctx);
        }
    }

    zb_buf_free(bufid);
}

static zb_void_t cmd_zb_active_ep_cb(zb_bufid_t bufid)
{
    zb_zdo_ep_resp_t * p_resp = (zb_zdo_ep_resp_t *)zb_buf_begin(bufid);
    zdo_tsn_ctx_t    * p_tsn_ctx;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (!p_tsn_ctx)
    {
        zb_buf_free(bufid);
        return;
    }

    if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
    {
        nrf_cli_fprintf(p_tsn_ctx->p_cli,
                        NRF_CLI_NORMAL,
                        "\r\nsrc_addr=%0hx ",
                        p_resp->nwk_addr);

        PRINT_LIST(p_tsn_ctx->p_cli, "ep=", "%d", zb_uint8_t, 
                   (zb_uint8_t *)p_resp + sizeof(zb_zdo_ep_resp_t),
                   p_resp->ep_count);

        print_done(p_tsn_ctx->p_cli, ZB_TRUE);
    }
    else
    {
        print_error(p_tsn_ctx->p_cli, "Active ep request failed", ZB_TRUE);
    }

    invalidate_ctx(p_tsn_ctx);
    zb_buf_free(bufid);
}

static zb_void_t cmd_zb_simple_desc_req_cb(zb_bufid_t bufid)
{
    zdo_tsn_ctx_t             * p_tsn_ctx;
    zb_zdo_simple_desc_resp_t * p_resp = (zb_zdo_simple_desc_resp_t *)zb_buf_begin(bufid);

    zb_uint8_t    in_cluster_cnt  = p_resp->simple_desc.app_input_cluster_count;
    zb_uint8_t    out_cluster_cnt = p_resp->simple_desc.app_output_cluster_count;
    zb_uint16_t * p_cluster_list  = (zb_uint16_t *)p_resp->simple_desc.app_cluster_list;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->hdr.tsn);
    if (!p_tsn_ctx)
    {
        zb_buf_free(bufid);
        return;
    }

    if (p_resp->hdr.status == ZB_ZDP_STATUS_SUCCESS)
    {
        nrf_cli_fprintf(p_tsn_ctx->p_cli,
                        NRF_CLI_NORMAL,
                        "\r\nsrc_addr=0x%0hx ep=%d profile_id=0x%04hx app_dev_id=0x%0hx app_dev_ver=0x%0hx ",
                        p_resp->hdr.nwk_addr,
                        p_resp->simple_desc.endpoint,
                        p_resp->simple_desc.app_profile_id,
                        p_resp->simple_desc.app_device_id,
                        p_resp->simple_desc.app_device_version);
        
        PRINT_LIST(p_tsn_ctx->p_cli, "in_clusters=", "0x%04hx", zb_uint16_t,
                   p_cluster_list, in_cluster_cnt);

        PRINT_LIST(p_tsn_ctx->p_cli, "out_clusters=", "0x%04hx", zb_uint16_t,
                   p_cluster_list + in_cluster_cnt, out_cluster_cnt);
        
        print_done(p_tsn_ctx->p_cli, ZB_TRUE);
    }
    else
    {
        print_error(p_tsn_ctx->p_cli, "Active ep request failed", ZB_TRUE);
    }

    invalidate_ctx(p_tsn_ctx);
    zb_buf_free(bufid);
}

/**
 * @brief Handles timeout error and invalidates binding transaction.
 *
 * @param[in] tsn ZBOSS transaction sequence number.
 */
static void cmd_zb_bind_unbind_timeout(zb_uint8_t tsn)
{
    zdo_tsn_ctx_t * p_tsn_ctx = get_ctx_by_tsn(tsn);

    if (!p_tsn_ctx)
    {
        return;
    }

    print_error(p_tsn_ctx->p_cli, "Bind/unbind request timed out", ZB_TRUE);
    invalidate_ctx(p_tsn_ctx);
}

/**
 * @brief A callback called on bind/unbind response.
 *
 * @param[in] bufid Reference number to ZBOSS memory buffer.
 */
zb_void_t cmd_zb_bind_unbind_cb(zb_bufid_t bufid)
{
    zb_zdo_bind_resp_t * p_resp = (zb_zdo_bind_resp_t *)zb_buf_begin(bufid);
    zdo_tsn_ctx_t      * p_tsn_ctx;
    zb_ret_t             zb_err_code;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (!p_tsn_ctx)
    {
        zb_buf_free(bufid);
        return;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(cmd_zb_bind_unbind_timeout, ZB_ALARM_ANY_PARAM);
    if (zb_err_code != RET_OK)
    {
        print_error(p_tsn_ctx->p_cli, "Unable to cancel timeout timer", ZB_TRUE);
    }

    if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
    {
        print_done(p_tsn_ctx->p_cli, ZB_TRUE);
    }
    else
    {
        nrf_cli_fprintf(p_tsn_ctx->p_cli,
                        NRF_CLI_ERROR,
                        "\r\nError: Unable to modify binding. Status: %d\r\n",
                        p_resp->status);
    }

    invalidate_ctx(p_tsn_ctx);
    zb_buf_free(bufid);
}

/**
 * @brief Handles timeout error and invalidates network address request transaction.
 *
 * @param[in] tsn ZBOSS transaction sequence number.
 */
static void cmd_zb_nwk_addr_timeout(zb_uint8_t tsn)
{
    zdo_tsn_ctx_t * p_tsn_ctx = get_ctx_by_tsn(tsn);

    if (!p_tsn_ctx)
    {
        return;
    }

    print_error(p_tsn_ctx->p_cli, "Network address request timed out", ZB_TRUE);
    invalidate_ctx(p_tsn_ctx);
}

/**
 * @brief A callback called on network address response.
 *
 * @param[in] bufid Reference number to ZBOSS memory buffer.
 */
zb_void_t cmd_zb_nwk_addr_cb(zb_bufid_t bufid)
{
    zb_zdo_nwk_addr_resp_head_t * p_resp = (zb_zdo_nwk_addr_resp_head_t *)zb_buf_begin(bufid);
    zdo_tsn_ctx_t               * p_tsn_ctx;
    zb_ret_t                      zb_err_code;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (!p_tsn_ctx)
    {
        zb_buf_free(bufid);
        return;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(cmd_zb_nwk_addr_timeout, ZB_ALARM_ANY_PARAM);
    if (zb_err_code != RET_OK)
    {
        print_error(p_tsn_ctx->p_cli, "Unable to cancel timeout timer", ZB_TRUE);
    }

    if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
    {
        zb_uint16_t nwk_addr;

        ZB_LETOH16(&nwk_addr, &(p_resp->nwk_addr));
        nrf_cli_fprintf(p_tsn_ctx->p_cli,
                        NRF_CLI_NORMAL,
                        "\r\n%hx",
                        nwk_addr);
        print_done(p_tsn_ctx->p_cli, ZB_TRUE);
    }
    else
    {
        nrf_cli_fprintf(p_tsn_ctx->p_cli,
                        NRF_CLI_ERROR,
                        "\r\nError: Unable to resolve EUI64 source address. Status: %d\r\n",
                        p_resp->status);
    }

    invalidate_ctx(p_tsn_ctx);
    zb_buf_free(bufid);
}

/**
 * @brief Handles timeout error and invalidates IEEE (EUI64) address request transaction.
 *
 * @param[in] tsn ZBOSS transaction sequence number.
 */
static void cmd_zb_ieee_addr_timeout(zb_uint8_t tsn)
{
    zdo_tsn_ctx_t * p_tsn_ctx = get_ctx_by_tsn(tsn);

    if (!p_tsn_ctx)
    {
        return;
    }

    print_error(p_tsn_ctx->p_cli, "IEEE address request timed out", ZB_TRUE);
    invalidate_ctx(p_tsn_ctx);
}

/**
 * @brief A callback called on IEEE (EUI64) address response.
 *
 * @param[in] bufid Reference number to ZBOSS memory buffer.
 */
zb_void_t cmd_zb_ieee_addr_cb(zb_bufid_t bufid)
{
    zb_zdo_ieee_addr_resp_t * p_resp = (zb_zdo_ieee_addr_resp_t *)zb_buf_begin(bufid);
    zdo_tsn_ctx_t           * p_tsn_ctx;
    zb_ret_t                  zb_err_code;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (!p_tsn_ctx)
    {
        zb_buf_free(bufid);
        return;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(cmd_zb_ieee_addr_timeout, ZB_ALARM_ANY_PARAM);
    if (zb_err_code != RET_OK)
    {
        print_error(p_tsn_ctx->p_cli, "Unable to cancel timeout timer", ZB_TRUE);
    }

    if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
    {
        zb_address_ieee_ref_t addr_ref;
        zb_ieee_addr_t        ieee_addr;
        zb_uint16_t           nwk_addr;
        zb_ret_t              ret;

        ZB_LETOH64(ieee_addr, p_resp->ieee_addr_remote_dev);
        ZB_LETOH16(&nwk_addr, &(p_resp->nwk_addr_remote_dev));

        // Update local IEEE address resolution table.
        ret = zb_address_update(ieee_addr, nwk_addr, ZB_TRUE, &addr_ref);
        if (ret == RET_OK)
        {
            print_eui64(p_tsn_ctx->p_cli, ieee_addr);
            print_done(p_tsn_ctx->p_cli, ZB_TRUE);
        }
        else
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_ERROR,
                            "\r\nError: Failed to updated address table. Status: %d\r\n",
                            ret);
        }
    }
    else
    {
        nrf_cli_fprintf(p_tsn_ctx->p_cli,
                        NRF_CLI_ERROR,
                        "\r\nError: Unable to resolve IEEE address. Status: %d\r\n",
                        p_resp->status);
    }

    invalidate_ctx(p_tsn_ctx);
    zb_buf_free(bufid);
}

/**
 * @brief Send Active Endpoint Request.
 *
 * @code
 * zdo active_ep <h:16-bit destination address> *
 * @endcode
 *
 * Send Active Endpoint Request to the node addressed by the short address.
 *
 * Example:
 * @code
 * > zdo active_ep 0xb4fc
 * > src_addr=B4FC ep=10,11,12
 * Done
 * @endcode
 *
 */
static void cmd_zb_active_ep(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_active_ep_req_t    * p_req;
    zdo_tsn_ctx_t             * p_tsn_cli;
    zb_bufid_t                  bufid;
    uint16_t                    addr;

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:16-bit destination address>\r\n");
        return;
    }

    if (argc != 2)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        return;
    }

    p_req = zb_buf_initial_alloc(bufid, sizeof(*p_req));

    if (!parse_hex_u16(argv[1], &addr))
    {
        print_error(p_cli, "Incorrect network address", ZB_FALSE);
        goto error;
    }
    p_req->nwk_addr = addr;

    p_tsn_cli = get_free_ctx();
    if (!p_tsn_cli)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->tsn   = zb_zdo_active_ep_req(bufid,
                                            cmd_zb_active_ep_cb);

    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send match descriptor request", ZB_FALSE);
        goto error;
    }

    return;

error:
    zb_buf_free(bufid);
}

/**
 * @brief Send Simple Descriptor Request.
 *
 * @code
 * zdo simple_desc_req <h:16-bit destination address> <d:endpoint>
 * @endcode
 *
 * Send Simple Descriptor Request to the given node and endpoint.
 *
 * Example:
 * @code
 * > zdo simple_desc_req 0xefba 10
 * > src_addr=0xEFBA ep=260 profile_id=0x0102 app_dev_id=0x0 app_dev_ver=0x5
 *   in_clusters=0x0000,0x0003,0x0004,0x0005,0x0006,0x0008,0x0300 out_clusters=0x0300
 * Done
 * @endcode
 *
 */
static void cmd_zb_simple_desc(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_simple_desc_req_t * p_req;
    zdo_tsn_ctx_t            * p_tsn_cli;
    zb_bufid_t                 bufid;
    zb_uint16_t                addr;

    if ((argc != 3) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:16-bit destination address> "
                    "<d:endpoint>\r\n");
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        return;
    }

    p_req = zb_buf_initial_alloc(bufid, sizeof(*p_req));

    if (!parse_hex_u16(argv[1], &addr))
    {
        print_error(p_cli, "Invalid network address", ZB_FALSE);
        goto error;
    }
    p_req->nwk_addr = addr;

    if (!sscan_uint8(argv[2], &(p_req->endpoint)))
    {
        print_error(p_cli, "Invalid endpoint", ZB_FALSE);
        goto error;
    }

    p_tsn_cli = get_free_ctx();
    if (!p_tsn_cli)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->tsn   = zb_zdo_simple_desc_req(bufid, cmd_zb_simple_desc_req_cb);

    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send match descriptor request", ZB_FALSE);
        goto error;
    }

    return;

error:
    zb_buf_free(bufid);
}

/**
 * @brief Send match descriptor request.
 *
 * @code
 * zdo match_desc <h:16-bit destination address>
                  <h:requested address/type> <h:profile ID>
                  <d:number of input clusters> [<h:input cluster IDs> ...]
                  <d:number of output clusters> [<h:output cluster IDs> ...]
                  [-t | --timeout <n seconds>]
 *
 * @endcode
 *
 * Send Match Descriptor Request to the `dst_addr` node that is a
 * query about the `req_addr` node of the `prof_id` profile ID,
 * which must have at least one of `n_input_clusters`(whose IDs are listed in `{...}`)
 * or `n_output_clusters` (whose IDs are listed in `{...}`).
 * The IDs can be either decimal values or hexadecimal strings.
 * Set the timeout of request with `-t` of `--timeout` optional parameter.
 *
 * Example:
 * @code
 * zdo match_desc 0xfffd 0xfffd 0x0104 1 6 0
 * @endcode
 *
 * In this example, the command sends a Match Descriptor Request to all non-sleeping
 * nodes regarding all non-sleeping nodes that have 1 input cluster ON/OFF (ID 6) and 0 output clusters.
 *
 */
static void cmd_zb_match_desc(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_match_desc_param_t * p_req;
    zdo_tsn_ctx_t             * p_tsn_cli;
    zb_bufid_t                  bufid;
    uint16_t                  * p_cluster_list = NULL;
    uint8_t                     len = sizeof(p_req->cluster_list);
    zb_ret_t                    zb_err_code;
    zb_bool_t                   use_timeout = ZB_FALSE;
    zb_uint16_t                 timeout = ZIGBEE_CLI_MATCH_DESC_RESP_TIMEOUT;
    int                         timeout_offset;
    zb_uint16_t                 temp;

    // We use p_cluster_list for calls to ZBOSS API but we're not using
    // p_cluster_list value in any way.
    UNUSED_VARIABLE(p_cluster_list);

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:16-bit destination address>\r\n"
                    "<h:requested address/type> <h:profile ID>\r\n"
                    "<d:number of input clusters> [<h:input cluster IDs> ...]\r\n"
                    "<d:number of output clusters> [<h:output cluster IDs> ...]\r\n"
                    "[--timeout d:number of seconds to wait for answers]\r\n");
        return;
    }

    if (!strcmp(argv[1], "-t") || !strcmp(argv[1], "--timeout"))
    {
        print_error(p_cli, "Place option 'timeout' at the end of input parameters", ZB_FALSE);
        return;
    }

    if (argc < 6)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        return;
    }

    p_req = zb_buf_initial_alloc(bufid, sizeof(*p_req));

    if (!parse_hex_u16(argv[1], &temp))
    {
        print_error(p_cli, "Incorrect network address", ZB_FALSE);
        goto error;
    }
    p_req->nwk_addr = temp;

    if (!parse_hex_u16(argv[2], &temp))
    {
        print_error(p_cli, "Incorrect address of interest", ZB_FALSE);
        goto error;
    }
    p_req->addr_of_interest = temp;

    if (!parse_hex_u16(argv[3], &temp))
    {
        print_error(p_cli, "Incorrect profile id", ZB_FALSE);
        goto error;
    }
    p_req->profile_id = temp;

    // The following functions don't perform any checks on the cluster list
    // assuming that the CLI isn't abused. In practice the list length is limited
    // by @p NRF_CLI_ARGC_MAX which defaults to 12 arguments.

    if (!sscan_uint8(argv[4], &(p_req->num_in_clusters)))
    {
        print_error(p_cli, "Incorrect number of input clusters", ZB_FALSE);
        goto error;
    }

    if (p_req->num_in_clusters)
    {
        // Allocate additional space for cluster IDs. Space for 1 one cluster ID
        // is already in the structure, hence we subtract len.
        p_cluster_list = zb_buf_alloc_right(bufid, p_req->num_in_clusters * sizeof(uint16_t) - len);

        // We have used the space, set to 0 so that space for output clusters
        // is calculated correctly.
        len = 0;

        // Use p_req->cluster_list as destination rather that p_cluster_list which
        // points to the second element.
        if (!sscan_cluster_list(argv + 5, p_req->num_in_clusters, (uint16_t *)p_req->cluster_list))
        {
            print_error(p_cli, "Failed to parse input cluster list", ZB_FALSE);
            goto error;
        }

    }

    if (!sscan_uint8(argv[5 + p_req->num_in_clusters], &(p_req->num_out_clusters)))
    {
        print_error(p_cli, "Incorrect number of output clusters", ZB_FALSE);
        goto error;
    }

    if (p_req->num_out_clusters)
    {
        p_cluster_list = zb_buf_alloc_right(bufid, p_req->num_out_clusters * sizeof(uint16_t) - len);

        if (!sscan_cluster_list(argv + 5 + p_req->num_in_clusters + 1,
                                p_req->num_out_clusters,
                                (uint16_t *)p_req->cluster_list + p_req->num_in_clusters))
        {
            print_error(p_cli, "Failed to parse output cluster list", ZB_FALSE);
            goto error;
        }
    }

    // Now let's check for timeout option
    timeout_offset = 6 + p_req->num_in_clusters + p_req->num_out_clusters;

    if (argc == timeout_offset + 2)
    {
        if (!strcmp(argv[timeout_offset], "-t") || !strcmp(argv[timeout_offset], "--timeout"))
        {
            use_timeout = ZB_TRUE;
            if (sscanf(argv[timeout_offset + 1], "%hd", &timeout) != 1)
            {
                /* Let's set the timeout to default in this case. */
                timeout = ZIGBEE_CLI_MATCH_DESC_RESP_TIMEOUT;
                nrf_cli_fprintf(p_cli, NRF_CLI_WARNING, "Could not parse the timeout value, setting to default.");
            }
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Timeout set to %d.\r\n", timeout);
        }
    }

    p_tsn_cli = get_free_ctx();
    if (!p_tsn_cli)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->is_broadcast = ZB_NWK_IS_ADDRESS_BROADCAST(p_req->nwk_addr);
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Sending %s request.\r\n", p_tsn_cli->is_broadcast ? "broadcast" : "unicast");
    p_tsn_cli->tsn   = zb_zdo_match_desc_req(bufid, cmd_zb_match_desc_cb);

    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send match descriptor request", ZB_FALSE);
        goto error;
    }

    if (use_timeout || !p_tsn_cli->is_broadcast)
    {
        /* In case of unicast we schedule callback to merely print the 'Done' thing if the response never comes.
         * The stack calls the callback function indicating the timeout only in case of broadcasts.
         * In case of unicast, we can treat the command as finished either when the timeout has elapsed, or when
         * the response has been received. With this in mind, we schedule the alarm with the timeout, which checks the context.
         * If the context has been deleted, then it means that it has been already freed by the response reception callback.
         */
        zb_err_code = ZB_SCHEDULE_APP_ALARM(cmd_zb_match_desc_timeout,
                                            p_tsn_cli->tsn,
                                            timeout * ZB_TIME_ONE_SECOND);
        if (zb_err_code != RET_OK)
        {
            print_error(p_cli, "Unable to schedule timeout timer", ZB_FALSE);
            invalidate_ctx(p_tsn_cli);
        }
    }

    return;

error:
    zb_buf_free(bufid);
}

/**
 * @brief Create or remove a binding between two endpoints on two nodes.
 *
 * @code
 * zdo bind {on,off} <h:source eui64> <d:source ep> <h:destination addr>
 *                   <d:destination ep> <h:source cluster id> <h:request dst addr>`
 * @endcode
 *
 * Create bound connection between a device identified by `source eui64` and
 * endpoint `source ep`, and a device identified by `destination addr` and
 * endpoint `destination ep`. The connection is created for ZCL commands and
 * attributes assigned to the ZCL cluster `source cluster id` on the
 * `request dst addr` node (usually short address corresponding to `source eui64` argument).
 *
 * Example:
 * @code
 * zdo bind on 0B010E0405060708 1 0B010E4050607080 2 8
 * @endcode
 *
 */
static void cmd_zb_bind(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_bind_req_param_t * p_req;
    zdo_tsn_ctx_t           * p_tsn_cli = NULL;
    zb_bufid_t                bufid;
    zb_ret_t                  zb_err_code;
    zb_bool_t                 bind;

    if (strcmp(argv[0], "on") == 0)
    {
        bind = ZB_TRUE;
    }
    else
    {
        bind = ZB_FALSE;
    }

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:source eui64> <d:source ep>\r\n"
                    "<h:destination addr> <d:destination ep>\r\n"
                    "<h:source cluster id> <h:request dst addr>");
        return;
    }

    if (argc != 7)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        return;
    }

    p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_bind_req_param_t);

    if (!parse_long_address(argv[1], p_req->src_address))
    {
        print_error(p_cli, "Incorrect EUI64 source address format", ZB_FALSE);
        goto error;
    }

    if (!sscan_uint8(argv[2], &(p_req->src_endp)))
    {
        print_error(p_cli, "Incorrect source endpoint", ZB_FALSE);
        goto error;
    }

    p_req->dst_addr_mode = parse_address(argv[3], &(p_req->dst_address), ADDR_ANY);
    if (p_req->dst_addr_mode == ADDR_INVALID)
    {
        print_error(p_cli, "Incorrect destination address format", ZB_FALSE);
        goto error;
    }

    if (!sscan_uint8(argv[4], &(p_req->dst_endp)))
    {
        print_error(p_cli, "Incorrect destination endpoint", ZB_FALSE);
        goto error;
    }

    if (!parse_hex_u16(argv[5], &(p_req->cluster_id)))
    {
        print_error(p_cli, "Incorrect cluster ID", ZB_FALSE);
        goto error;
    }

    if (!parse_short_address(argv[6], &(p_req->req_dst_addr)))
    {
        print_error(p_cli, "Incorrect destination network address for the request", ZB_FALSE);
        goto error;
    }

    p_tsn_cli = get_free_ctx();
    if (!p_tsn_cli)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    // Initialize context and send a request.
    p_tsn_cli->p_cli = p_cli;

    if (bind)
    {
        p_tsn_cli->tsn = zb_zdo_bind_req(bufid, cmd_zb_bind_unbind_cb);
    }
    else
    {
        p_tsn_cli->tsn = zb_zdo_unbind_req(bufid, cmd_zb_bind_unbind_cb);
    }

    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send request", ZB_FALSE);
        goto error;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM(cmd_zb_bind_unbind_timeout,
                                        p_tsn_cli->tsn,
                                        ZIGBEE_CLI_BIND_RESP_TIMEOUT * ZB_TIME_ONE_SECOND);
    if (zb_err_code != RET_OK)
    {
        print_error(p_cli, "Unable to schedule timeout timer", ZB_FALSE);
        invalidate_ctx(p_tsn_cli);
    }

    return;

error:
    if (p_tsn_cli != NULL)
    {
        invalidate_ctx(p_tsn_cli);
    }
    zb_buf_free(bufid);
}

/**
 * @brief Resolve eui64 address to a short network address.
 *
 * @code
 * zdo nwk_addr <h:eui64>
 * @endcode
 *
 * Example:
 * @code
 * zdo nwk_addr 0B010E0405060708
 * @endcode
 *
 */
static void cmd_zb_nwk_addr(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_nwk_addr_req_param_t * p_req;
    zdo_tsn_ctx_t               * p_tsn_cli = NULL;
    zb_bufid_t                    bufid;
    zb_ret_t                      zb_err_code;

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0], "<h:EUI64>");
        return;
    }

    if (argc != 2)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        return;
    }

    p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_nwk_addr_req_param_t);

    if (!parse_long_address(argv[1], p_req->ieee_addr))
    {
        print_error(p_cli, "Incorrect EUI64 address format", ZB_FALSE);
        goto error;
    }

    p_tsn_cli = get_free_ctx();
    if (!p_tsn_cli)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    // Construct network address request.
    p_req->dst_addr     = ZB_NWK_BROADCAST_ALL_DEVICES;
    p_req->request_type = ZB_ZDO_SINGLE_DEVICE_RESP;
    p_req->start_index  = 0;

    // Initialize context and send a request.
    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->tsn   = zb_zdo_nwk_addr_req(bufid, cmd_zb_nwk_addr_cb);
    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send request", ZB_FALSE);
        goto error;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM(cmd_zb_nwk_addr_timeout,
                                        p_tsn_cli->tsn,
                                        ZIGBEE_CLI_NWK_ADDR_RESP_TIMEOUT * ZB_TIME_ONE_SECOND);
    if (zb_err_code != RET_OK)
    {
        print_error(p_cli, "Unable to schedule timeout timer", ZB_FALSE);
        invalidate_ctx(p_tsn_cli);
    }

    return;

error:
    if (p_tsn_cli != NULL)
    {
        invalidate_ctx(p_tsn_cli);
    }
    zb_buf_free(bufid);
}

/**
 * @brief Resolve EUI64 by sending IEEE address request.
 *
 * @code
 * zdo ieee_addr <h:short_addr>
 * @endcode
 *
 */
static void cmd_zb_ieee_addr(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_ieee_addr_req_param_t * p_req = NULL;
    zdo_tsn_ctx_t                * p_tsn_cli = NULL;
    zb_bufid_t                     bufid;
    zb_ret_t                       zb_err_code;
    zb_uint16_t                    addr;

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:short address>");
        return;
    }

    if (argc != 2)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        return;
    }

    // Create new IEEE address request and fill with default values.
    p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_ieee_addr_req_param_t);
    p_req->start_index  = 0;
    p_req->request_type = 0;

    if (!parse_hex_u16(argv[1], &addr))
    {
        print_error(p_cli, "Incorrect network address", ZB_FALSE);
        goto error;
    }
    p_req->nwk_addr = addr;
    p_req->dst_addr = p_req->nwk_addr;

    p_tsn_cli = get_free_ctx();
    if (!p_tsn_cli)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    // Initialize context and send a request.
    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->tsn   = zb_zdo_ieee_addr_req(bufid, cmd_zb_ieee_addr_cb);
    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send request", ZB_FALSE);
        goto error;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM(cmd_zb_ieee_addr_timeout,
                                        p_tsn_cli->tsn,
                                        ZIGBEE_CLI_IEEE_ADDR_RESP_TIMEOUT * ZB_TIME_ONE_SECOND);
    if (zb_err_code != RET_OK)
    {
        print_error(p_cli, "Unable to schedule timeout timer", ZB_FALSE);
        invalidate_ctx(p_tsn_cli);
    }

    return;

error:
    if (p_tsn_cli != NULL)
    {
        invalidate_ctx(p_tsn_cli);
    }
    zb_buf_free(bufid);
}

/**
 * @brief Get the short 16-bit address of the Zigbee device.
 *
 * @code
 * > zdo short
 * 0000
 * Done
 * @endcode
 */
static void cmd_zb_short(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argv);

    zb_ieee_addr_t addr;
    zb_uint16_t short_addr;
    int i;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 1)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    zb_get_long_address(addr);

    short_addr = zb_address_short_by_ieee(addr);
    if (short_addr != ZB_UNKNOWN_SHORT_ADDR)
    {
        /* We got a valid address */
        for (i = sizeof(zb_uint16_t) - 1; i >= 0; i--)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "%02x", *((zb_uint8_t*)(&short_addr) + i));
        }

        print_done(p_cli, ZB_TRUE);
    }
    else
    {
        /* Most probably there was no network to join */
        print_error(p_cli, "Check if device was commissioned", ZB_FALSE);
    }
}

/**
 * @brief Get or set the EUI64 address of the Zigbee device.
 *
 * @code
 * > zdo eui64 [<h:eui64>]
 * 0b010eaafd745dfa
 * Done
 * @endcode
 */
static void cmd_zb_eui64(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_ieee_addr_t addr;

    UNUSED_PARAMETER(argv);

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 2)
    {
        print_error(p_cli, "Incorrect number of arguments", ZB_FALSE);
        return;
    }

    if (argc == 2)
    {
        if (parse_long_address(argv[1], addr))
        {
            zb_set_long_address(addr);
        }
        else
        {
            print_error(p_cli, "Incorrect EUI64 address format", ZB_FALSE);
        }
    }
    else
    {
        zb_get_long_address(addr);
    }

    print_eui64(p_cli, addr);
    print_done(p_cli, ZB_TRUE);
}

/**
 * @brief Callback called, when mgmt_leave operation takes too long
 * @param tsn[in] tsn value obtained as result of zdo_mgmt_leave_req, transaction sequence number
 */
static void cmd_zb_mgmt_leave_timeout_cb(zb_uint8_t tsn)
{
    zdo_tsn_ctx_t * p_tsn_ctx = get_ctx_by_tsn(tsn);

    if (p_tsn_ctx == NULL)
    {
        return;
    }

    print_error(p_tsn_ctx->p_cli, "mgmt_leave request timed out", ZB_TRUE);

    invalidate_ctx(p_tsn_ctx);
}

/**
 * @brief Callback called when response to mgmt_leave is received
 * @param bufid[in] zboss buffer reference
 */
static void cmd_zb_mgmt_leave_cb(zb_bufid_t bufid)
{
    zb_zdo_mgmt_leave_res_t * p_resp = (zb_zdo_mgmt_leave_res_t*)zb_buf_begin(bufid);
    zdo_tsn_ctx_t           * p_tsn_ctx;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (p_tsn_ctx != NULL)
    {
        zb_ret_t zb_err_code;

        zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(cmd_zb_mgmt_leave_timeout_cb, p_resp->tsn);
        if (zb_err_code != RET_OK)
        {
            print_error(p_tsn_ctx->p_cli, "Unable to cancel timeout timer", ZB_TRUE);
        }

        if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
        {
            print_done(p_tsn_ctx->p_cli, ZB_TRUE);
        }
        else
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_ERROR,
                            "\r\nError: Unable to remove device. Status: %u\r\n",
                            (uint32_t)p_resp->status);
        }

        invalidate_ctx(p_tsn_ctx);
    }

    zb_buf_free(bufid);
}

/**
 * @brief Parses command line arguments for zdo mgmt_leave comand
 * @param p_req[out]    Request do be filled in according to command line arguments
 * @param p_cli[in]     Pointer to cli instance, used to produce errors if neccessary
 * @param argc[in]      Number of arguments in argv
 * @param argv[in]      Arguments from cli to the command
 * @return true, if arguments were parsed correctly and p_req has been filled up <br>
 *         false, if arguments were incorrect
 *
 * @sa @ref cmd_zb_mgmt_leave
 */
static bool cmd_zb_mgmt_leave_parse(zb_zdo_mgmt_leave_param_t *p_req, const nrf_cli_t *p_cli, size_t argc, char **argv)
{
    size_t      arg_idx;
    zb_uint16_t addr;

    ZB_MEMSET(p_req, 0, sizeof(*p_req));

    arg_idx = 1U;   /* Let it be index of the first argument to parse */
    if (arg_idx >= argc)
    {
        print_error(p_cli, "Lack of dst_addr parameter", ZB_FALSE);
        return false;
    }

    if (parse_hex_u16(argv[arg_idx], &addr) != 1)
    {
        print_error(p_cli, "Incorrect dst_addr", ZB_FALSE);
        return false;
    }

    p_req->dst_addr = addr;
    arg_idx++;

    /* Try parse device_address */
    if (arg_idx < argc)
    {
        const char *curr_arg = argv[arg_idx];
        if (curr_arg[0] != '-')
        {
            if (!parse_long_address(curr_arg, p_req->device_address))
            {
                print_error(p_cli, "Incorrect device_address", ZB_FALSE);
                return false;
            }

            arg_idx++;
        }
        else
        {
            /* No device_address field */
        }
    }

    /* Parse optional fields */
    while (arg_idx < argc)
    {
        const char *curr_arg = argv[arg_idx];
        if (strcmp(curr_arg, "--children") == 0)
        {
            p_req->remove_children = ZB_TRUE;
        }
        else if (strcmp(curr_arg, "--rejoin") == 0)
        {
            p_req->rejoin = ZB_TRUE;
        }
        else
        {
            print_error(p_cli, "Incorrect argument", ZB_FALSE);
            return false;
        }
        arg_idx++;
    }


    return true;
}

/**
 * @brief Send a request to a remote device in order to leave network through zdo mgmt_leave_req (see spec. 2.4.3.3.5)
 *
 * @code
 * zdo mgmt_leave <h:16-bit dst_addr> [h:device_address eui64] [--children] [--rejoin]
 * @endcode
 *
 * Send @c mgmt_leave_req to a remote node specified by @c dst_addr. If @c device_address is omitted
 * or it has value @c 0000000000000000, the remote device at address @c dst_addr will remove itself
 * from the network. If @c device_address has other value, it must be a long address corresponding
 * to @c dst_addr or a long address of child node of @c dst_addr.
 * The request is sent with <em>Remove Children</em> and <em>Rejoin</em> flags set to @c 0 by default. Use options
 * @c \--children or @c \--rejoin do change respective flags to @c 1.
 * For more details, see section 2.4.3.3.5 of the specification.
 *
 * Examples:
 * @code
 * zdo mgmt_leave 0x1234
 * @endcode
 * Sends @c mgmt_leave_req to the device with short address @c 0x1234, asking it to remove itself from the network. @n
 * @code
 * zdo mgmt_leave 0x1234 --rejoin
 * @endcode
 * Sends @c mgmt_leave_req to device with short address @c 0x1234, asking it to remove itself from the network and perform rejoin.@n
 * @code
 * zdo mgmt_leave 0x1234 0b010ef8872c633e
 * @endcode
 * Sends @c mgmt_leave_req to device with short address @c 0x1234, asking it to remove device @c 0b010ef8872c633e from the network.
 * If the target device with short address @c 0x1234 has also a long address @c 0b010ef8872c633e, it will remove itself from the network
 * If the target device with short address @c 0x1234 has a child with long address @c 0b010ef8872c633e, it will remove the child from the network.@n
 * @code
 * zdo mgmt_leave 0x1234 --children
 * @endcode
 * Sends @c mgmt_leave_req to the device with short address @c 0x1234, asking it to remove itself and all its children from the network.@n
 */
static void cmd_zb_mgmt_leave(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_mgmt_leave_param_t * p_req;

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:16-bit dst_addr> [h:device_address eui64] [--children]\r\n"
                    "[--rejoin]"
                );
        return;
    }

    zb_bufid_t      bufid     = 0;
    zdo_tsn_ctx_t * p_tsn_cli = NULL;

    bufid = zb_buf_get_out();
    if (bufid == 0)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        goto error;
    }

    p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_mgmt_leave_param_t);
    if (!cmd_zb_mgmt_leave_parse(p_req, p_cli, argc, argv))
    {
        /* The error message has already been printed by cmd_zb_mgmt_leave_parse */
        goto error;
    }

    p_tsn_cli = get_free_ctx();
    if (p_tsn_cli == NULL)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->tsn = zdo_mgmt_leave_req(bufid, cmd_zb_mgmt_leave_cb);
    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send request", ZB_FALSE);
        goto error;
    }

    zb_ret_t zb_err_code;
    zb_err_code = ZB_SCHEDULE_APP_ALARM(cmd_zb_mgmt_leave_timeout_cb,
                                        p_tsn_cli->tsn,
                                        ZIGBEE_CLI_MGMT_LEAVE_RESP_TIMEOUT * ZB_TIME_ONE_SECOND);
    if (zb_err_code != RET_OK)
    {
        print_error(p_cli, "Unable to schedule timeout timer", ZB_FALSE);
        invalidate_ctx(p_tsn_cli);
    }

    return;
error:
    if (bufid != 0)
    {
        zb_buf_free(bufid);
    }
    if (p_tsn_cli != NULL)
    {
        invalidate_ctx(p_tsn_cli);
    }
}

/**
 * @brief Request timeout callback.
 * @param tsn[in] ZDO transaction sequence number returned by request.
 */
static void ctx_timeout_cb(zb_uint8_t tsn)
{
    zdo_tsn_ctx_t * p_tsn_ctx = get_ctx_by_tsn(tsn);

    if (p_tsn_ctx == NULL)
    {
        NRF_LOG_ERROR("Unable to find context for ZDO request %u", tsn);
        return;
    }

    nrf_cli_fprintf(p_tsn_ctx->p_cli,
                    NRF_CLI_ERROR,
                    "\r\nError: ZDO request %u timed out\r\n",
                    tsn);

    invalidate_ctx(p_tsn_ctx);
}

/**
 * @brief A generic ZDO request callback.
 *
 * This will print status code for the message and, if not overridden, free
 * resources associated with the request.
 *
 * @param bufid[in] ZBOSS buffer id
 */
static void zdo_request_cb(zb_bufid_t bufid)
{
    zb_zdo_callback_info_t * p_resp = (zb_zdo_callback_info_t *)zb_buf_begin(bufid);
    zdo_tsn_ctx_t          * p_tsn_ctx;
    bool                     is_request_complete;
    zb_ret_t                 zb_err_code;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (p_tsn_ctx == NULL)
    {
        NRF_LOG_ERROR("Unable to find context for TSN %d", p_resp->tsn);
        zb_buf_free(bufid);
        return;
    }

    zb_err_code = ZB_SCHEDULE_APP_ALARM_CANCEL(ctx_timeout_cb, p_resp->tsn);
    ASSERT(zb_err_code == RET_OK);

    // Call custom callback if set. If the callback returns false, i.e., request isn't
    // complete, then don't print status, invalidate context, or free input buffer.
    //
    // Request might not be complete if more messages must be send, e.g.,
    // to get multiple table entries from a remote device.
    if (p_tsn_ctx->p_cb != NULL)
    {
        is_request_complete = p_tsn_ctx->p_cb(p_tsn_ctx, bufid);
    }
    else
    {
        is_request_complete = true;
    }

    if (is_request_complete)
    {
        // We can free all resources.
        if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_NORMAL,
                            "\r\nZDO request %u complete\r\n",
                            p_resp->tsn);
            print_done(p_tsn_ctx->p_cli, ZB_TRUE);
        }
        else
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_ERROR,
                            "\r\nError: ZDO request %u failed with status %u\r\n",
                            (uint32_t)p_resp->tsn,
                            (uint32_t)p_resp->status);
        }
    }
    else
    {
        // The request isn't complete, i.e., another ZDO transaction went out,
        // hence we need to reschedule a timeout callback.
        zb_err_code = ZB_SCHEDULE_APP_ALARM(ctx_timeout_cb,
                                            p_tsn_ctx->tsn,
                                            ZIGBEE_CLI_MGMT_LEAVE_RESP_TIMEOUT * ZB_TIME_ONE_SECOND);
        if (zb_err_code != RET_OK)
        {
            print_error(p_tsn_ctx->p_cli, "Unable to schedule timeout callback", ZB_TRUE);
            is_request_complete = true;
        }
    }

    if (is_request_complete)
    {
        invalidate_ctx(p_tsn_ctx);
        zb_buf_free(bufid);
    }
}

/**@brief Prints one binding table record.
 * @param[out] p_cli     The CLI the output is printed to.
 * @param[in]  idx       Record index in binding table.
 * @param[in]  p_record  Record to be printed out.
 */
static void print_bind_resp_record(const nrf_cli_t * p_cli, uint32_t idx, const zb_zdo_binding_table_record_t * p_record)
{
    char ieee_address_str[sizeof(p_record->src_address)*2U + 1U];

    if (ieee_addr_to_str(ieee_address_str, sizeof(ieee_address_str), p_record->src_address) <= 0)
    {
        strcpy(ieee_address_str, "(error)         ");
    }
    ieee_address_str[sizeof(ieee_address_str)-1U] = '\0';   // Ensure null-terminated string

    /* Note: Fields in format string are scattered to match position in the header, printed by print_bind_resp_records_header */
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL,
                    "[%3u] %s      %3u     0x%04x",
                    (uint32_t)idx,
                    ieee_address_str,
                    (uint32_t)p_record->src_endp,
                    (uint32_t)p_record->cluster_id);

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL,
                    "           %3u ",
                    (uint32_t)p_record->dst_addr_mode);

    switch (p_record->dst_addr_mode)
    {
        case ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT:     /* 16-bit group address for DstAddress and DstEndp not present */
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "          0x%4x      N/A",
                    (uint32_t)p_record->dst_address.addr_short);
            break;

        case ZB_APS_ADDR_MODE_64_ENDP_PRESENT:     /* 64-bit extended address for DstAddress and DstEndp present */
            if (ieee_addr_to_str(ieee_address_str, sizeof(ieee_address_str), p_record->dst_address.addr_long) <= 0)
            {
                strcpy(ieee_address_str, "(error)         ");
            }
            ieee_address_str[sizeof(ieee_address_str)-1U] = '\0';   // Ensure null-terminated string

            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL,
                    "%s      %3u",
                    ieee_address_str,
                    (uint32_t)p_record->dst_endp);
            break;

        default:
            /* This should not happen, as the above case values are the only ones allowed by Zigbee r21 specification */
            nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "            N/A      N/A");
            break;
    }

    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "\r\n");
}

/**@brief   Prints header for binding records table
 * @param[out] p_cli    The CLI the output is printed to.
 */
static void print_bind_resp_records_header(const nrf_cli_t * p_cli)
{
    /* Note: Position of fields matches corresponding fields printed by print_bind_resp_record */
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL,
            "\r\n[idx] src_address      src_endp cluster_id dst_addr_mode dst_addr         dst_endp\r\n");
}

/**@brief   Prints records of binding table received from zdo_mgmt_bind_resp
 * @param[out] p_cli    The CLI the output is printed to.
 * @param[in]  p_resp   Response received from remote device to be printed out
 *
 * @note Records of type @ref zb_zdo_binding_table_record_t are located just after
 * the @ref zb_zdo_mgmt_bind_resp_t structure pointed by p_resp parameter.
 */
static void print_bind_resp(const nrf_cli_t * p_cli, const zb_zdo_mgmt_bind_resp_t * p_resp)
{
    uint32_t next_start_index = (uint32_t)p_resp->start_index + p_resp->binding_table_list_count;

    const zb_zdo_binding_table_record_t *p_record;
    p_record = (const zb_zdo_binding_table_record_t *)(p_resp + 1);
    for (uint32_t idx = p_resp->start_index; idx < next_start_index; ++idx, ++p_record)
    {
        print_bind_resp_record(p_cli, idx, p_record);
    }
}

/**@brief Callback terminating single mgmt_bind_req transaction
 * @note
 * When the binding table is too large to fit into a single mgmt_bind_rsp command frame, this function
 * will issue a new mgmt_bind_req_t with start_index increased by the number of just received entries to
 * download remaining part of the binding table. This process may involve several round trips of
 * mgmt_bind_req followed by mgmt_bind_rsp until the whole binding table is downloaded.
 *
 * @param bufid     Reference to ZBOSS buffer (as required by Zigbee stack API)
 */
static void cmd_zb_mgmt_bind_cb(zb_bufid_t bufid)
{
    zb_zdo_mgmt_bind_resp_t * p_resp = (zb_zdo_mgmt_bind_resp_t*)zb_buf_begin(bufid);
    zdo_tsn_ctx_t           * p_tsn_ctx;

    p_tsn_ctx = get_ctx_by_tsn(p_resp->tsn);
    if (p_tsn_ctx != NULL)
    {
        if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
        {
            if ((p_resp->start_index == p_tsn_ctx->cmd_ctx.req_seq.start_index))
            {
                print_bind_resp_records_header(p_tsn_ctx->p_cli);
            }
            print_bind_resp(p_tsn_ctx->p_cli, p_resp);

            uint32_t next_start_index = p_resp->start_index;
            next_start_index += p_resp->binding_table_list_count;

            if ((next_start_index < p_resp->binding_table_entries) &&
                (next_start_index < 0xFFU) &&
                (p_resp->binding_table_list_count != 0U))
            {
                /* We have more entries to get */
                UNUSED_RETURN_VALUE(zb_buf_reuse(bufid));
                zb_zdo_mgmt_bind_param_t * p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_mgmt_bind_param_t);
                p_req->dst_addr    = p_tsn_ctx->cmd_ctx.req_seq.dst_addr;
                p_req->start_index = next_start_index;

                p_tsn_ctx->tsn = zb_zdo_mgmt_bind_req(bufid, cmd_zb_mgmt_bind_cb);
                if (p_tsn_ctx->tsn == ZB_ZDO_INVALID_TSN)
                {
                    print_error(p_tsn_ctx->p_cli, "Failed to send request", ZB_TRUE);
                    goto finish;
                }

                bufid     = 0;    /* bufid reused, mark NULL not to free it */
                p_tsn_ctx = NULL; /* p_tsn_ctx reused, mark NULL not to free it */
            }
            else
            {
                nrf_cli_fprintf(p_tsn_ctx->p_cli, NRF_CLI_NORMAL, "Total entries for the binding table: %u",
                        (uint32_t)p_resp->binding_table_entries);
                print_done(p_tsn_ctx->p_cli, ZB_TRUE);
            }
        }
        else
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_ERROR,
                            "\r\nError: Unable to get binding table. Status: %u\r\n",
                            (uint32_t)p_resp->status);
        }
    }

finish:
    if (bufid != 0)
    {
        zb_buf_free(bufid);
    }

    if (p_tsn_ctx != NULL)
    {
        invalidate_ctx(p_tsn_ctx);
    }
}

/**
 * @brief Send a request to a remote device in order to read the binding table through zdo mgmt_bind_req (see spec. 2.4.3.3.4)
 *
 * @note If whole binding table does not fit into single @c mgmt_bind_resp frame, the request initiates a series of
 * requests performing full binding table download.
 *
 * @code
 * zdo mgmt_bind <h:short> [d:start_index]
 * @endcode
 *
 * Example:
 * @code
 * zdo mgmt_bind 0x1234
 * @endcode
 * Sends @c mgmt_bind_req to the device with short address @c 0x1234, asking it to return its binding table.
 */
static void cmd_zb_mgmt_bind(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    size_t                     arg_idx = 1U;
    zb_zdo_mgmt_bind_param_t * p_req;

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:short> [d:start_index]");
        return;
    }

    zdo_tsn_ctx_t * p_tsn_ctx = NULL;
    zb_bufid_t      bufid     = 0;

    p_tsn_ctx = get_free_ctx();
    if (p_tsn_ctx == NULL)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }
    p_tsn_ctx->p_cli = p_cli;

    if (arg_idx < argc)
    {
        if (!parse_short_address(argv[arg_idx], &(p_tsn_ctx->cmd_ctx.req_seq.dst_addr)))
        {
            print_error(p_cli, "Incorrect dst_addr", ZB_FALSE);
            goto error;
        }
        arg_idx++;
    }
    else
    {
        print_error(p_cli, "dst_addr parameter missing", ZB_FALSE);
        goto error;
    }

    if (arg_idx < argc)
    {
        if (!sscan_uint8(argv[arg_idx], &p_tsn_ctx->cmd_ctx.req_seq.start_index))
        {
            print_error(p_cli, "Incorrect start_index", ZB_FALSE);
            goto error;
        }
        arg_idx++;
    }
    else
    {
        /* This parameter was optional, no error */
        p_tsn_ctx->cmd_ctx.req_seq.start_index = 0;
    }

    if (arg_idx < argc)
    {
        print_error(p_cli, "Unexpected extra parameters", ZB_FALSE);
        goto error;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to execute command (buf alloc failed)", ZB_FALSE);
        goto error;
    }

    p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_mgmt_bind_param_t);
    ZB_BZERO(p_req, sizeof(*p_req));
    p_req->start_index = p_tsn_ctx->cmd_ctx.req_seq.start_index;
    p_req->dst_addr    = p_tsn_ctx->cmd_ctx.req_seq.dst_addr;

    p_tsn_ctx->tsn = zb_zdo_mgmt_bind_req(bufid, cmd_zb_mgmt_bind_cb);
    if (p_tsn_ctx->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send request", ZB_FALSE);
        goto error;
    }

    return;
error:
    if (bufid != 0)
    {
        zb_buf_free(bufid);
    }
    if (p_tsn_ctx != NULL)
    {
        invalidate_ctx(p_tsn_ctx);
    }
}

/**@brief Callback for a single mgmt_lqi_req transaction
 *
 * @note
 * When the lqi table is too large to fit into a single mgmt_bind_rsp command
 * frame, this function will issue a new mgmt_lqi_req to download reminder of
 * the table. This process may involve several round trips of mgmt_lqi_req
 * followed by mgmt_lqi_resp until the whole binding table is downloaded.
 *
 * @param bufid     Reference to ZBOSS buffer (as required by Zigbee stack API)
 */
static bool zdo_mgmt_lqi_cb(struct zdo_tsn_ctx * p_tsn_ctx, zb_bufid_t bufid)
{
    bool                      result = true;
    zb_zdo_mgmt_lqi_resp_t  * p_resp = (zb_zdo_mgmt_lqi_resp_t *)zb_buf_begin(bufid);

    if (p_resp->status == ZB_ZDP_STATUS_SUCCESS)
    {
        if (p_resp->start_index == p_tsn_ctx->cmd_ctx.req_seq.start_index)
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_NORMAL,
                            "\r\n[idx] ext_pan_id       ext_addr         "
                            "short_addr flags permit_join depth lqi\r\n");
        }

        zb_zdo_neighbor_table_record_t * p_record = (zb_zdo_neighbor_table_record_t *)
                                                    ((uint8_t *)p_resp + sizeof(*p_resp));

        for (uint8_t i = 0; i < p_resp->neighbor_table_list_count; i++, p_record++)
        {
            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_NORMAL,
                            "[%3u] ", p_resp->start_index + i);

            print_eui64(p_tsn_ctx->p_cli, p_record->ext_pan_id);

            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_NORMAL,
                            " ");
            print_eui64(p_tsn_ctx->p_cli, p_record->ext_addr);

            nrf_cli_fprintf(p_tsn_ctx->p_cli,
                            NRF_CLI_NORMAL,
                            " 0x%04x     0x%02x  "
                            "%u           %u     %u\r\n",
                            p_record->network_addr,
                            p_record->type_flags,
                            p_record->permit_join,
                            p_record->depth,
                            p_record->lqi);
        }

        uint16_t next_index = p_resp->start_index + p_resp->neighbor_table_list_count;

        // Get next portion of lqi table if needed.
        if ((next_index < p_resp->neighbor_table_entries) &&
            (next_index < 0xff) &&
            (p_resp->neighbor_table_list_count > 0))
        {
            zb_zdo_mgmt_lqi_param_t * p_req;

            UNUSED_RETURN_VALUE(zb_buf_reuse(bufid));
            p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_mgmt_lqi_param_t);

            p_req->start_index = next_index;
            p_req->dst_addr    = p_tsn_ctx->cmd_ctx.req_seq.dst_addr;

            p_tsn_ctx->tsn = zb_zdo_mgmt_lqi_req(bufid, zdo_request_cb);
            if (p_tsn_ctx->tsn != ZB_ZDO_INVALID_TSN)
            {
                // The request requires further communication, hence the outer
                // callback shoudn't free resources.
                result = false;
            }
        }
    }

    return result;
}

/**
 * @brief Send a ZDO Mgmt_Lqi_Req command to a remote device.
 *
 * @code
 * zdo mgmt_lqi <h:short> [d:start index]
 * @endcode
 *
 * Example:
 * @code
 * zdo mgmt_lqi 0x1234
 * @endcode
 * Sends @c mgmt_lqi_req to the device with short address @c 0x1234, asking it to return its neighbor table.
 */
static void cmd_zb_mgmt_lqi(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    zb_zdo_mgmt_lqi_param_t * p_req;
    zb_bufid_t                bufid     = 0;
    zdo_tsn_ctx_t           * p_tsn_cli = NULL;

    if ((argc == 1) || (nrf_cli_help_requested(p_cli)))
    {
        print_usage(p_cli, argv[0],
                    "<h:short> [d:start index]");
        return;
    }

    bufid = zb_buf_get_out();
    if (!bufid)
    {
        print_error(p_cli, "Failed to allocate request buffer", ZB_FALSE);
        goto error;
    }

    p_req = ZB_BUF_GET_PARAM(bufid, zb_zdo_mgmt_lqi_param_t);

    if (!parse_short_address(argv[1], &(p_req->dst_addr)))
    {
        print_error(p_cli, "Failed to parse destination address", ZB_FALSE);
        goto error;
    }

    if (argc >= 3)
    {
        if (!sscan_uint8(argv[2], &(p_req->start_index)))
        {
            print_error(p_cli, "Failed to start index", ZB_FALSE);
            goto error;
        }
    }
    else
    {
        p_req->start_index = 0;
    }

    p_tsn_cli = get_free_ctx();
    if (p_tsn_cli == NULL)
    {
        print_error(p_cli, "Too many ZDO transactions", ZB_FALSE);
        goto error;
    }

    p_tsn_cli->p_cli = p_cli;
    p_tsn_cli->p_cb  = zdo_mgmt_lqi_cb;
    p_tsn_cli->tsn   = zb_zdo_mgmt_lqi_req(bufid, zdo_request_cb);

    if (p_tsn_cli->tsn == ZB_ZDO_INVALID_TSN)
    {
        print_error(p_cli, "Failed to send request", ZB_FALSE);
        goto error;
    }

    zb_ret_t zb_err_code;
    zb_err_code = ZB_SCHEDULE_APP_ALARM(ctx_timeout_cb,
                                        p_tsn_cli->tsn,
                                        ZIGBEE_CLI_MGMT_LEAVE_RESP_TIMEOUT * ZB_TIME_ONE_SECOND);
    if (zb_err_code != RET_OK)
    {
        print_error(p_cli, "Unable to schedule timeout callback", ZB_FALSE);
        invalidate_ctx(p_tsn_cli);
    }

    return;

error:
    if (bufid != 0)
    {
        zb_buf_free(bufid);
    }

    if (p_tsn_cli != NULL)
    {
        invalidate_ctx(p_tsn_cli);
    }
}

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_bind)
{
    NRF_CLI_CMD(on, NULL, "create bind entry", cmd_zb_bind),
    NRF_CLI_CMD(off, NULL, "remove bind entry", cmd_zb_bind),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_zdo)
{
    NRF_CLI_CMD(active_ep, NULL, "send active endpoint request", cmd_zb_active_ep),
    NRF_CLI_CMD(simple_desc_req, NULL, "send simple descriptor request", cmd_zb_simple_desc),
    NRF_CLI_CMD(match_desc, NULL, "send match descriptor request", cmd_zb_match_desc),
    NRF_CLI_CMD(nwk_addr, NULL, "resolve EUI64 address to short network address", cmd_zb_nwk_addr),
    NRF_CLI_CMD(ieee_addr, NULL, "resolve network short address to EUI64 address", cmd_zb_ieee_addr),
    NRF_CLI_CMD(eui64, NULL, "get the eui64 address of the node", cmd_zb_eui64),
    NRF_CLI_CMD(short, NULL, "get the short address of the node", cmd_zb_short),
    NRF_CLI_CMD(bind, &m_sub_bind, "create or remove the binding entry in the remote node", NULL),
    NRF_CLI_CMD(mgmt_bind, NULL, "get binding table (see spec. 2.4.3.3.4)", cmd_zb_mgmt_bind),
    NRF_CLI_CMD(mgmt_leave, NULL, "perform mgmt_leave_req (see spec. 2.4.3.3.5)", cmd_zb_mgmt_leave),
    NRF_CLI_CMD(mgmt_lqi, NULL, "perform mgmt_lqi_req", cmd_zb_mgmt_lqi),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CMD_REGISTER(zdo, &m_sub_zdo, "ZDO manipulation", NULL);


/** @} */
