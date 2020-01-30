/* ZBOSS Zigbee 3.0
 *
 * Copyright (c) 2012-2018 DSR Corporation, Denver CO, USA.
 * http://www.dsr-zboss.com
 * http://www.dsr-corporation.com
 * All rights reserved.
 *
 *
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
PURPOSE: MAC nRF52840 radio driver specific.
*/

#define ZB_TRACE_FILE_ID 28283
#include "zboss_api.h"

#include "nrf_802154_const.h"
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"
#endif
#include "nrf_802154.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#include "mac_nrf52_transceiver.h"

#if !defined NRF_802154_FRAME_TIMESTAMP_ENABLED || !NRF_802154_FRAME_TIMESTAMP_ENABLED
#error Must define NRF_802154_FRAME_TIMESTAMP_ENABLED!
#endif

#ifdef ENABLE_FEM
#include "nrf_gpio.h"

#if defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_PDN_PIN)
/** Macro with default configuration of the 3-pin FEM module. */
#ifndef ZB_FEM_SETTINGS
#define ZB_FEM_SETTINGS                                                 \
    {                                                                   \
        .fem_config =                                                   \
        {                                                               \
            .pa_time_gap_us  = NRF_FEM_PA_TIME_IN_ADVANCE_US,           \
            .lna_time_gap_us = NRF_FEM_LNA_TIME_IN_ADVANCE_US,          \
            .pdn_settle_us   = NRF_FEM_PDN_SETTLE_US,                   \
            .trx_hold_us     = NRF_FEM_TRX_HOLD_US,                     \
            .pa_gain_db      = 0,                                       \
            .lna_gain_db     = 0                                        \
        },                                                              \
        .pa_pin_config =                                                \
        {                                                               \
            .enable       = FEM_CONTROL_DEFAULT_ENABLE,                 \
            .active_high  = 1,                                          \
            .gpio_pin     = FEM_CONTROL_DEFAULT_PA_PIN,                 \
            .gpiote_ch_id = NRF_FEM_CONTROL_DEFAULT_PA_GPIOTE_CHANNEL   \
        },                                                              \
        .lna_pin_config =                                               \
        {                                                               \
            .enable       = FEM_CONTROL_DEFAULT_ENABLE,                 \
            .active_high  = 1,                                          \
            .gpio_pin     = FEM_CONTROL_DEFAULT_LNA_PIN,                \
            .gpiote_ch_id = NRF_FEM_CONTROL_DEFAULT_LNA_GPIOTE_CHANNEL  \
        },                                                              \
        .pdn_pin_config =                                               \
        {                                                               \
            .enable       = FEM_CONTROL_DEFAULT_ENABLE,                 \
            .active_high  = 1,                                          \
            .gpio_pin     = FEM_CONTROL_DEFAULT_PDN_PIN,                \
            .gpiote_ch_id = NRF_FEM_CONTROL_DEFAULT_PDN_GPIOTE_CHANNEL  \
        },                                                              \
        .ppi_ch_id_set = NRF_FEM_CONTROL_DEFAULT_SET_PPI_CHANNEL,       \
        .ppi_ch_id_clr = NRF_FEM_CONTROL_DEFAULT_CLR_PPI_CHANNEL,       \
        .ppi_ch_id_pdn = NRF_FEM_CONTROL_DEFAULT_PDN_PPI_CHANNEL        \
    }
#endif // ZB_FEM_SETTINGS

#elif defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN)
/** Macro with default configuration of the 2-pin FEM module. */
#ifndef ZB_FEM_SETTINGS
#define ZB_FEM_SETTINGS                                                 \
    {                                                                   \
        .fem_config =                                                   \
        {                                                               \
            .pa_time_gap_us  = NRF_FEM_PA_TIME_IN_ADVANCE_US,           \
            .lna_time_gap_us = NRF_FEM_LNA_TIME_IN_ADVANCE_US,          \
            .pa_gain_db      = 0,                                       \
            .lna_gain_db     = 0                                        \
        },                                                              \
        .pa_pin_config =                                                \
        {                                                               \
            .enable       = FEM_CONTROL_DEFAULT_ENABLE,                 \
            .active_high  = 1,                                          \
            .gpio_pin     = FEM_CONTROL_DEFAULT_PA_PIN,                 \
            .gpiote_ch_id = NRF_FEM_CONTROL_DEFAULT_PA_GPIOTE_CHANNEL   \
        },                                                              \
        .lna_pin_config =                                               \
        {                                                               \
            .enable       = FEM_CONTROL_DEFAULT_ENABLE,                 \
            .active_high  = 1,                                          \
            .gpio_pin     = FEM_CONTROL_DEFAULT_LNA_PIN,                \
            .gpiote_ch_id = NRF_FEM_CONTROL_DEFAULT_LNA_GPIOTE_CHANNEL  \
        },                                                              \
        .ppi_ch_id_set = NRF_FEM_CONTROL_DEFAULT_SET_PPI_CHANNEL,       \
        .ppi_ch_id_clr = NRF_FEM_CONTROL_DEFAULT_CLR_PPI_CHANNEL        \
    }
#endif // ZB_FEM_SETTINGS

#else
#error Unsupported FEM type
#endif

#endif //ENABLE_FEM


static zb_uint8_t zb_transceiver_set_state(nrf_802154_state_t new_state);
extern void nrf_802154_transmit_csma_ca_raw(const uint8_t * p_data);

#ifdef ENABLE_FEM
ZB_WEAK_PRE void ZB_WEAK nrf_fem_gpio_configure(void)
{
#if defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_PDN_PIN)
    // Set default FEM pin direction.
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_PA_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_LNA_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_PDN_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_MODE_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_ANTSEL_PIN);

    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_MOSI_PIN);
    nrf_gpio_cfg_default(FEM_CONTROL_DEFAULT_MISO_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_CLK_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_CSN_PIN);

    // Set default FEM pin polarity.
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_PA_PIN);     // Disable PA, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_LNA_PIN);    // Disable LNA, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_PDN_PIN);    // Disable FEM, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_MODE_PIN);   // Use POUTA_PROD TX gain by default
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_ANTSEL_PIN); // Use antenna at "ANT1" port by default

    nrf_gpio_pin_set(FEM_CONTROL_DEFAULT_MOSI_PIN);     // SPI mode not used. Use high polarity by default.
    nrf_gpio_pin_set(FEM_CONTROL_DEFAULT_CLK_PIN);      // SPI mode not used. Use high polarity by default.
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_CSN_PIN);    // SPI mode not used. Use low polarity by default.

#elif defined(NRF_FEM_CONTROL_DEFAULT_PA_PIN) && defined(NRF_FEM_CONTROL_DEFAULT_LNA_PIN)
    // Set default FEM pin direction.
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_PA_PIN);     // CTX (PA, controlled by radio driver)
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_LNA_PIN);    // CRX (LNA, controlled by radio driver)
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_PDN_PIN);    // CSD (enable, managed by the application)
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_MODE_PIN);   // CHL (enables transmit high-power mode, managed by the application)
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_ANTSEL_PIN); // ANT_SEL (select antenna output, managed by the application)
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_CSN_PIN);    // CPS (enables bypass mode, managed by the application)

    // Set default FEM pin polarity.
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_PA_PIN);     // Disable PA, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_LNA_PIN);    // Disable LNA, radio driver will override this setting
    nrf_gpio_pin_set(FEM_CONTROL_DEFAULT_PDN_PIN);      // Enable FEM module by default
    nrf_gpio_pin_set(FEM_CONTROL_DEFAULT_MODE_PIN);     // Use transmit high-power mode by default
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_ANTSEL_PIN); // Use antenna at "ANT1" port by default
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_CSN_PIN);    // Disable bypass mode by default

#else
#error Unsupported FEM type
#endif
}
#endif

/**
   Reset transceiver HW at mlme-reset time
*/
void zb_transceiver_hw_init(void)
{
  nrf_802154_init();

  /*Configure FEM*/
#ifdef ENABLE_FEM
#ifdef FEM_CONTROL_DEFAULT_ENABLE
  if (FEM_CONTROL_DEFAULT_ENABLE)
  {
    nrf_fem_gpio_configure();
  }
#endif
  nrf_fem_interface_config_t fem_config = ZB_FEM_SETTINGS;
  int32_t err_code = nrf_fem_interface_configuration_set(&fem_config);
  ZB_ASSERT(err_code == NRF_SUCCESS);
#endif

  /*Perform radio driver configuration*/
  zb_nrf_802154_mac_osif_init();
  nrf_802154_auto_ack_set(ZB_TRUE);
  (void)zb_transceiver_set_state(NRF_802154_STATE_SLEEP);
  /* FIXME: Why it is needed here? Why it is called directly, not via src_match() API? */
  /* ZB_SRC_MATCH_TBL_DROP(); */
#ifdef ZB_MACSPLIT_TRANSPORT_USERIAL
  /* If in split and use USB, need forst to init HW to init timer, then init USB serial. */
  zb_osif_userial_init();
#endif
  nrf_802154_src_addr_matching_method_set(NRF_802154_SRC_ADDR_MATCH_ZIGBEE);
}


void zb_transceiver_update_long_mac(zb_ieee_addr_t long_mac)
{
  /*Update the long addr in the nRF radio driver*/
  nrf_802154_extended_address_set(long_mac);
}

void zb_transceiver_set_pan_id(zb_uint16_t pan_id)
{
  TRACE_MSG(TRACE_MAC1, "zb_transceiver_set_pan_id 0x%x", (FMT__D, pan_id));
  nrf_802154_pan_id_set((zb_uint8_t *) (&pan_id));
}

void zb_transceiver_set_short_addr(zb_uint16_t addr)
{
  TRACE_MSG(TRACE_MAC1, "zb_transceiver_set_short_addr 0x%x", (FMT__D, addr));
  nrf_802154_short_address_set((zb_uint8_t *) (&addr));
}

void zb_transceiver_start_get_rssi(zb_uint8_t scan_duration_bi)
{
  while(!nrf_802154_energy_detection(ZB_TIME_BEACON_INTERVAL_TO_USEC(scan_duration_bi)));
}

/**
   Set channel and go to the normal (not ed scan) mode
*/
void zb_transceiver_set_channel(zb_uint8_t channel_number)
{
  TRACE_MSG(TRACE_MAC1, "zb_transceiver_set_channel %d", (FMT__D, channel_number));
  nrf_802154_channel_set(channel_number);
}

void zb_transceiver_set_tx_power(zb_int8_t power)
{
  nrf_802154_tx_power_set(power);
}

void zb_transceiver_get_tx_power(zb_int8_t* power)
{
  *power = (zb_int8_t)nrf_802154_tx_power_get();
}

/**
   Notify Nordic radio driver if we configured as the PAN coordinator
*/
void zb_transceiver_set_pan_coord(zb_bool_t pan_coord)
{
  TRACE_MSG(TRACE_MAC3, "zb_transceiver_set_pan_coord %hd", (FMT__H, pan_coord));
  nrf_802154_pan_coord_set((bool )pan_coord);
}

/**
   Enable or disable Auto ACK
*/
void zb_transceiver_set_auto_ack(zb_bool_t auto_ack)
{
  nrf_802154_auto_ack_set(auto_ack);
}

void zb_transceiver_set_promiscuous_mode(zb_bool_t enabled)
{
  nrf_802154_promiscuous_set(enabled);
}

void zb_transceiver_set_rx_on_off(int rx_on)
{
  TRACE_MSG(TRACE_MAC1, "mac_nrf52840_trans_set_rx_on_off %d", (FMT__D, rx_on));

  if (!rx_on)
  {
    (void)nrf_802154_sleep_if_idle();
  }
  else
  {
    (void)nrf_802154_receive();
  }
}

zb_bool_t zb_transceiver_get_rx_on_off(void)
{
  if (nrf_802154_state_get() == NRF_802154_STATE_RECEIVE)
  {
    return ZB_TRUE;
  }
  else
  {
    return ZB_FALSE;
  }
}

zb_bool_t zb_transceiver_is_idle(void)
{
  if (nrf_802154_state_get() == NRF_802154_STATE_SLEEP)
  {
    return ZB_TRUE;
  }
  else
  {
    return ZB_FALSE;
  }
}

zb_bool_t zb_transceiver_get_radio_on_off(void)
{
  TRACE_MSG(TRACE_MAC1, "mac_nrf52840_trans_get_radio_on_off", (FMT__0));
  if (nrf_802154_state_get() == NRF_802154_STATE_SLEEP)
  {
    return ZB_FALSE;
  }
  else
  {
    return ZB_TRUE;
  }
}

/*Set nRF driver radio state with checking the previouse state*/
static zb_uint8_t zb_transceiver_set_state(nrf_802154_state_t new_state)
{
  nrf_802154_state_t cur_state;
  zb_uint8_t ret = ZB_FALSE;
  cur_state = nrf_802154_state_get();
  if (cur_state != new_state)
  {
    switch (new_state)
    {
      case NRF_802154_STATE_SLEEP:
        if (cur_state == NRF_802154_STATE_RECEIVE)
        {
          ret = nrf_802154_sleep();
        }
        else
        {
          TRACE_MSG(TRACE_MAC1, "Can't set sleep state, RF not in Receive mode ", (FMT__0));
        }
        break;

      case NRF_802154_STATE_RECEIVE:
        if ((cur_state == NRF_802154_STATE_SLEEP) || (cur_state == NRF_802154_STATE_TRANSMIT))
        {
          (void)nrf_802154_receive();
        }
        else
        {
          TRACE_MSG(TRACE_MAC1, "Can't set sleep state, RF not in Receive mode ", (FMT__0));
        }
        break;
      case NRF_802154_STATE_TRANSMIT:
        break;
      case NRF_802154_STATE_ENERGY_DETECTION:
        break;
      case NRF_802154_STATE_CCA:
        break;
      default:
        TRACE_MSG(TRACE_MAC1, "nrf_set_state: bad param: %x ", (FMT__D, new_state));
    }
  }
  return ret;
}

zb_bool_t zb_transceiver_transmit(zb_uint8_t wait_type, zb_time_t tx_at, zb_uint8_t * tx_buf, zb_uint8_t current_channel)
{
  zb_bool_t transmit_status = ZB_FALSE;
#ifndef ZB_ENABLE_ZGP_DIRECT
  ZVUNUSED(tx_at);
  ZVUNUSED(current_channel);
#endif

  switch (wait_type)
  {
    case ZB_MAC_TX_WAIT_CSMACA:
#if !NRF_802154_CSMA_CA_ENABLED
      while(!transmit_status)
      {
        transmit_status = (zb_bool_t) nrf_802154_transmit_raw(tx_buf);
      }
      break;
#else
      transmit_status = ZB_TRUE;
      nrf_802154_transmit_csma_ca_raw(tx_buf);
#endif
      break;

#ifdef ZB_ENABLE_ZGP_DIRECT
    case ZB_MAC_TX_WAIT_ZGP:
      transmit_status = (zb_bool_t)nrf_802154_transmit_raw_at(
        tx_buf,
        0, /* cca */
      /* Our upper layers are already calculatet exact time to transmit. But
       * Nordic radio driver itseld is doing some calculations subtracting from
       * dt parameter. We do not want to change our upper layers, so let's just
       * subtract back from tx_at.
       * We may not care about tx_at overflows: if it overflowed in
       * cgp_data_req() in zgp_stub.c,
       * it goes back gere.
       * Yes, this is ugly, but we must use Nordic API which is not very good
       * for our upper logic.
       */
        tx_at - ZB_GPD_TX_OFFSET_US, /* t0 */
        ZB_GPD_TX_OFFSET_US,     /* dt */
        current_channel);
      break;
#endif
    case ZB_MAC_TX_WAIT_NONE:
      /* First transmit attempt without CCA. */
      transmit_status = (zb_bool_t) nrf_802154_transmit_raw(tx_buf, ZB_FALSE);
      break;
    default:
      TRACE_MSG(TRACE_MAC1, "Illegal wait_type parameter", (FMT__0));
      ZB_ASSERT(0);
      break;
  }
  return transmit_status;
}

/**
 * @brief Notify that frame was transmitted.
 *
 * @note If ACK was requested for transmitted frame this function is called after proper ACK is
 *       received. If ACK was not requested this function is called just after transmission is
 *       ended.
 * @note Buffer pointed by the @p p_ack pointer is not modified by the radio driver (and can't
 *       be used to receive a frame) until @sa nrf_802154_buffer_free_raw() function is
 *       called.
 * @note Buffer pointed by the @p p_ack pointer may be modified by the function handler (and other
 *       modules) until @sa nrf_802154_buffer_free_raw() function is called.
 * @note The next higher layer should handle @sa nrf_802154_transmitted_raw() or @sa
 *       nrf_802154_transmitted() function. It should not handle both.
 *
 * @param[in]  p_ack  Pointer to received ACK buffer. Fist byte in the buffer is length of the
 *                    frame (PHR) and following bytes are the ACK frame itself (PSDU). Length byte
 *                    (PHR) includes FCS. FCS is already verified by the hardware and may be
 *                    modified by the hardware.
 *                    If ACK was not requested @p p_ack is set to NULL.
 * @param[in]  power  RSSI of received frame or 0 if ACK was not requested.
 * @param[in]  lqi    LQI of received frame or 0 if ACK was not requested.
 */
void nrf_802154_transmitted_raw(const uint8_t * p_frame, uint8_t * p_ack, int8_t power, uint8_t lqi)
{
  ZVUNUSED(p_frame);
  ZVUNUSED(power);
  ZVUNUSED(lqi);

#if defined ZB_NRF52_RADIO_STATISTICS
  zb_nrf52_get_radio_stats()->tx_successful++;
#endif /* defined ZB_NRF52_RADIO_STATISTICS */

  mac_nrf52_transmitted_raw(p_ack);
}

void zb_transceiver_buffer_free(zb_uint8_t * p_buf)
{
  nrf_802154_buffer_free_raw(p_buf);
}

/**
 * @brief Notify that frame was not transmitted due to busy channel.
 *
 * This function is called if transmission procedure fails.
 *
 * @param[in]  error  Reason of the failure.
 */

void nrf_802154_transmit_failed(uint8_t const *p_frame, nrf_802154_tx_error_t error)
{
  ZVUNUSED(p_frame);
#if defined ZB_NRF52_RADIO_STATISTICS
  switch (error)
  {
    case NRF_802154_TX_ERROR_NONE:
      zb_nrf52_get_radio_stats()->tx_err_none++;
      break;
    case NRF_802154_TX_ERROR_BUSY_CHANNEL:
      zb_nrf52_get_radio_stats()->tx_err_busy_channel++;
      break;
    case NRF_802154_TX_ERROR_INVALID_ACK:
      zb_nrf52_get_radio_stats()->tx_err_invalid_ack++;
      break;
    case NRF_802154_TX_ERROR_NO_MEM:
      zb_nrf52_get_radio_stats()->tx_err_no_mem++;
      break;
    case NRF_802154_TX_ERROR_TIMESLOT_ENDED:
      zb_nrf52_get_radio_stats()->tx_err_timeslot_ended++;
      break;
    case NRF_802154_TX_ERROR_NO_ACK:
      zb_nrf52_get_radio_stats()->tx_err_no_ack++;
      break;
    case NRF_802154_TX_ERROR_ABORTED:
      zb_nrf52_get_radio_stats()->tx_err_aborted++;
      break;
    case NRF_802154_TX_ERROR_TIMESLOT_DENIED:
      zb_nrf52_get_radio_stats()->tx_err_timeslot_denied++;
      break;
  }
#endif /* defined ZB_NRF52_RADIO_STATISTICS */

  switch(error)
  {
    case NRF_802154_TX_ERROR_NO_MEM:
    case NRF_802154_TX_ERROR_ABORTED:
    case NRF_802154_TX_ERROR_TIMESLOT_DENIED:
    case NRF_802154_TX_ERROR_TIMESLOT_ENDED:
    case NRF_802154_TX_ERROR_BUSY_CHANNEL:
      mac_nrf52_transmit_failed(ZB_TRANS_CHANNEL_BUSY_ERROR);
      break;

    case NRF_802154_TX_ERROR_INVALID_ACK:
    case NRF_802154_TX_ERROR_NO_ACK:
      mac_nrf52_transmit_failed(ZB_TRANS_NO_ACK);
      break;
  }
}

/**
 * @brief Notify that frame was received.
 *
 * @note Buffer pointed by the p_data pointer is not modified by the radio driver (and can't
 *       be used to receive a frame) until nrf_802154_buffer_free_raw() function is called.
 * @note Buffer pointed by the p_data pointer may be modified by the function handler (and other
 *       modules) until @sa nrf_802154_buffer_free_raw() function is called.
 * @note The next higher layer should handle @sa nrf_802154_received_raw() or @sa
 *       nrf_802154_received() function. It should not handle both.
 *
 * p_data
 * v
 * +-----+-----------------------------------------------------------+------------+
 * | PHR | MAC Header and payload                                    | FCS        |
 * +-----+-----------------------------------------------------------+------------+
 *       |                                                                        |
 *       | <---------------------------- PHR -----------------------------------> |
 *
 * @param[in]  p_data  Pointer to the buffer containing received data (PHR + PSDU). First byte in
 *                     the buffer is length of the frame (PHR) and following bytes is the frame
 *                     itself (PSDU). Length byte (PHR) includes FCS. FCS is already verified by
 *                     the hardware and may be modified by the hardware.
 * @param[in]  power   RSSI of received frame.
 * @param[in]  lqi     LQI of received frame.
 */
void nrf_802154_received_timestamp_raw(uint8_t * p_data, int8_t power, uint8_t lqi, uint32_t time)
{
#if defined ZB_NRF52_RADIO_STATISTICS
  zb_nrf52_get_radio_stats()->rx_successful++;
#endif /* defined ZB_NRF52_RADIO_STATISTICS */

  mac_nrf52_received_timestamp_raw(p_data, power, lqi, time);
}

/**
 * @brief Notify that reception of a frame failed.
 *
 * @param[in]  error  An error code that indicates reason of the failure.
 */
void nrf_802154_receive_failed(nrf_802154_rx_error_t error)
{
#if defined ZB_NRF52_RADIO_STATISTICS
  switch (error)
  {
    case NRF_802154_RX_ERROR_NONE:
      zb_nrf52_get_radio_stats()->rx_err_none++;
      break;

    case NRF_802154_RX_ERROR_INVALID_FRAME:
      zb_nrf52_get_radio_stats()->rx_err_invalid_frame++;
      break;
    case NRF_802154_RX_ERROR_INVALID_FCS:
      zb_nrf52_get_radio_stats()->rx_err_invalid_fcs++;
      break;
    case NRF_802154_RX_ERROR_INVALID_DEST_ADDR:
      zb_nrf52_get_radio_stats()->rx_err_invalid_dest_addr++;
      break;
    case NRF_802154_RX_ERROR_RUNTIME:
      zb_nrf52_get_radio_stats()->rx_err_runtime++;
      break;
    case NRF_802154_RX_ERROR_TIMESLOT_ENDED:
      zb_nrf52_get_radio_stats()->rx_err_timeslot_ended++;
      break;
    case NRF_802154_RX_ERROR_ABORTED:
      zb_nrf52_get_radio_stats()->rx_err_aborted++;
      break;
  }
#endif /* defined ZB_NRF52_RADIO_STATISTICS */

  /* Should be disabled in MAC certification tests, so TRACE_MAC1 is used. */
  TRACE_ENTER_INT();
  TRACE_MSG(TRACE_MAC1, ">> nrf_802154_receive_failed, %d ", (FMT__D, error));
  TRACE_LEAVE_INT();
}

zb_bool_t zb_transceiver_set_pending_bit(zb_uint8_t *addr, zb_bool_t value, zb_bool_t extended)
{
  return ((value) ?
    (zb_bool_t)nrf_802154_pending_bit_for_addr_clear((const uint8_t *)addr, extended) :
    (zb_bool_t)nrf_802154_pending_bit_for_addr_set((const uint8_t *)addr, extended));
}

void zb_transceiver_src_match_tbl_drop(void)
{
  TRACE_MSG(TRACE_MAC3, ">>mac_nrf52840_src_match_tbl_drop", (FMT__0));
  nrf_802154_pending_bit_for_addr_reset(ZB_FALSE); /* reset for short addresses */
  nrf_802154_pending_bit_for_addr_reset(ZB_TRUE);  /* reset for long addresses */
  TRACE_MSG(TRACE_MAC3, "<<mac_nrf52840_src_match_tbl_drop", (FMT__0));
}
