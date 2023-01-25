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
#include "nrf_cli.h"
#include "zboss_api.h"
#include "zb_error_handler.h"
#include "zigbee_cli.h"
#include "zigbee_cli_utils.h"

/* Fixes to make Lint passable - radio driver headers are not linted */
#ifndef RAAL_SOFTDEVICE
#define RAAL_SOFTDEVICE 0
#endif /* RAAL_SOFTDEVICE */

#include "nrf_802154.h"
#include "fem/nrf_fem_control_config.h"
#include "mac_nrf52_transceiver.h"

/**
 * @defgroup zb_cli_cmd_radio Radio commands
 * @ingroup zb_cli
 *
 * @{
 */

/**@brief Activate the Front-End Modules (FEM) by enabling control lines.
 *
 * @code
 * radio fem enable
 * @endcode
 *
 * The configuration of the FEM is defined in external/zboss/osif/zb_nrf52_transceiver.c
 * (see the define ZB_FEM_SETTINGS).
 *
 * For more information, see the description of the FEM on the @link_radio_driver
 * Wiki and @ref shared_fem_feature page.
 */
static void cmd_zb_fem(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
#ifndef ENABLE_FEM
    print_error(p_cli, "FEM support disabled", ZB_FALSE);
#else
    nrf_fem_interface_config_t fem_config;
    int32_t                    err_code;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 1)
    {
        print_error(p_cli, "Wrong number of arguments", ZB_FALSE);
        return;
    }

    /* Read the current configuration. */
    err_code = nrf_fem_interface_configuration_get(&fem_config);
    if (err_code != NRF_SUCCESS)
    {
        print_error(p_cli, "Unable to read current FEM configuration", ZB_FALSE);
        return;
    }

    /* Check if FEM is enabled. FEM can be enabled only once. */
#if defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_PDN_PIN)
    if (fem_config.pa_pin_config.enable  ||
        fem_config.lna_pin_config.enable ||
        fem_config.pdn_pin_config.enable)
#else
    if (fem_config.pa_pin_config.enable ||
        fem_config.lna_pin_config.enable)
#endif
    {
        print_error(p_cli, "FEM already enabled", ZB_FALSE);
        return;
    }

    fem_config.pa_pin_config.enable  = 1;
    fem_config.lna_pin_config.enable = 1;
#if defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_PDN_PIN)
    fem_config.pdn_pin_config.enable = 1;
#endif

    /* Configure FEM control pins. */
    nrf_fem_gpio_configure();

    /* Update the configuration. */
    err_code = nrf_fem_interface_configuration_set(&fem_config);
    if (err_code != NRF_SUCCESS)
    {
        print_error(p_cli, "Unable to update FEM configuration", ZB_FALSE);
        return;
    }

    print_done(p_cli, ZB_FALSE);
#endif
}

/**@brief Configure FEM lines
 *
 * @code
 * radio fem <pa|lna> <pin|polarity> <d:pin|active_high>
 * @endcode
 *
 * The first argument selects the FEM line to configure. The available options are:
 * - pa: Power Amplifier
 * - lna: Low Noise Amplifier
 * - pdn: Power Down control pin
 *
 * The second argument selects which attribute will be changed:
 *  - pin: configures FEM pin number
 *  - polarity: configures FEM pin polarity
 *
 * The third argument is a value for the selected configuration attribute:
 *  - pin: selects the GPIO pin, which controls the FEM line
 *  - active_high: selects the polarity of the pin that activates the FEM line
 *    (Power Amplifier, Low Noise Amplifier or Power Down control, depending on the first argument).
 *
 * @note The FEM configuration may be applied only before the FEM control lines are enabled.
 */
static void cmd_zb_fem_line(nrf_cli_t const * p_cli, const char * p_line, size_t argc, char **argv)
{
#ifndef ENABLE_FEM
    print_error(p_cli, "FEM support disabled", ZB_FALSE);
#else
    nrf_fem_gpiote_pin_config_t * p_line_config = NULL;
    nrf_fem_interface_config_t    fem_config;
    int32_t                       err_code;
    uint8_t                       value;

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc != 2)
    {
        print_error(p_cli, "Wrong number of arguments", ZB_FALSE);
        return;
    }

    /* Read the current configuration. */
    err_code = nrf_fem_interface_configuration_get(&fem_config);
    if (err_code != NRF_SUCCESS)
    {
        print_error(p_cli, "Unable to read current FEM configuration", ZB_FALSE);
        return;
    }

    /* Check if FEM is enabled. FEM can be enabled only once. */
    if (fem_config.pa_pin_config.enable ||
        fem_config.lna_pin_config.enable)
    {
        print_error(p_cli, "Configuration may be changed only if FEM is disabled", ZB_FALSE);
        return;
    }

    /* Resolve line name to configuration structure. */
    if (strcmp(p_line, "PA") == 0)
    {
        p_line_config = &fem_config.pa_pin_config;
    }
    else if (strcmp(p_line, "LNA") == 0)
    {
        p_line_config = &fem_config.lna_pin_config;
    }
#if defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_PDN_PIN)
    else if (strcmp(p_line, "PDN") == 0)
    {
        p_line_config = &fem_config.pdn_pin_config;
    }
#endif
    else
    {
        print_error(p_cli, "Unsupported line name", ZB_FALSE);
        return;
    }

    /* Parse user input. */
    err_code = sscan_uint8(argv[1], &value);
    if (err_code == 0)
    {
        print_error(p_cli, "Incorrect value", ZB_FALSE);
        return;
    }

    /* Resolve configuration value. */
    if (strcmp(argv[0], "pin") == 0)
    {
        p_line_config->gpio_pin = (uint8_t)value;
    }
    else if (strcmp(argv[0], "polarity") == 0)
    {
        p_line_config->active_high = (value ? true : false);
    }
    else
    {
        print_error(p_cli, "Unsupported line configuration option", ZB_FALSE);
        return;
    }

    /* Update the configuration. */
    err_code = nrf_fem_interface_configuration_set(&fem_config);
    if (err_code != NRF_SUCCESS)
    {
        print_error(p_cli, "Unable to update FEM configuration", ZB_FALSE);
        return;
    }

    print_done(p_cli, ZB_FALSE);
#endif
}

/**@brief Subcommand to configure Power Amplifier line of FEM module.
 *
 * @note For more information see @ref fem_configure_pin and @ref fem_configure_polarity
 *       commands description.
 */
static void cmd_zb_fem_line_pa(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    cmd_zb_fem_line(p_cli, "PA", argc, argv);
}

/**@brief Subcommand to configure Low Noise Amplifier line of FEM module.
 *
 * @note For more information see @ref fem_configure_pin and @ref fem_configure_polarity
 *       commands description.
 */
static void cmd_zb_fem_line_lna(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    cmd_zb_fem_line(p_cli, "LNA", argc, argv);
}

/**@brief Subcommand to configure Power Down line of FEM module.
 *
 * @note For more information see @ref fem_configure_pin and @ref fem_configure_polarity
 *       commands description.
 */
static void cmd_zb_fem_line_pdn(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    cmd_zb_fem_line(p_cli, "PDN", argc, argv);
}

/**@brief Function to set the 802.15.4 channel directly.
 *
 * @code
 * radio channel set <n>
 * @endcode
 *
 * The <n> has to be between 11 and 26 included, since these channels are supported by the driver.
 *
 * @note This function sets the channel directly at runtime, contrary to the `bdb channel` function,
 *       which defines the channels allowed for the Zigbee network formation.
 */
static void cmd_zb_channel_set(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc == 2)
    {
        uint8_t channel;
        if (!sscan_uint8(argv[1], &channel))
        {
            print_error(p_cli, "Invalid channel", ZB_FALSE);
        }
        else if ((channel < 11) || (channel > 26))
        {
            print_error(p_cli, "Only channels from 11 to 26 are supported", ZB_FALSE);
        }
        else
        {
            nrf_802154_channel_set(channel);
            print_done(p_cli, ZB_TRUE);
        }
    }
    else
    {
        print_error(p_cli, "Wrong number of arguments", ZB_FALSE);
    }
}

/**@brief Function to get the current 802.15.4 channel.
 *
 * @code
 * radio channel get
 * @endcode
 * 
 */
static void cmd_zb_channel_get(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL, "Current operating channel: %d", nrf_802154_channel_get());
    print_done(p_cli, ZB_TRUE);
}


NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_fem_line_pa)
{
    NRF_CLI_CMD(pin, NULL, "select pin number to use", cmd_zb_fem_line_pa),
    NRF_CLI_CMD(polarity, NULL, "select active polarity", cmd_zb_fem_line_pa),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_fem_line_lna)
{
    NRF_CLI_CMD(pin, NULL, "select pin number to use", cmd_zb_fem_line_lna),
    NRF_CLI_CMD(polarity, NULL, "select active polarity", cmd_zb_fem_line_lna),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_fem_line_pdn)
{
    NRF_CLI_CMD(pin, NULL, "select pin number to use", cmd_zb_fem_line_pdn),
    NRF_CLI_CMD(polarity, NULL, "select active polarity", cmd_zb_fem_line_pdn),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_fem)
{
    NRF_CLI_CMD(pa, &m_sub_fem_line_pa, "configure PA control line", NULL),
    NRF_CLI_CMD(lna, &m_sub_fem_line_lna, "configure LNA control pin", NULL),
    NRF_CLI_CMD(pdn, &m_sub_fem_line_pdn, "configure PDN control pin", NULL),
    NRF_CLI_CMD(enable, NULL, "enable FEM", cmd_zb_fem),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_channel)
{
    NRF_CLI_CMD(set, NULL, "set 802.15.4 channel", cmd_zb_channel_set),
    NRF_CLI_CMD(get, NULL, "get 802.15.4 channel", cmd_zb_channel_get),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(m_sub_radio)
{
    NRF_CLI_CMD(fem, &m_sub_fem, "front-end module", NULL),
    NRF_CLI_CMD(channel, &m_sub_channel, "get/set channel", NULL),
    NRF_CLI_SUBCMD_SET_END
};
NRF_CLI_CMD_REGISTER(radio, &m_sub_radio, "Radio manipulation", NULL);

/** @} */
