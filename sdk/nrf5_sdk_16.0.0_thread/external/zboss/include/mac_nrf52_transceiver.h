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
PURPOSE: MAC definitions NRF52840 SoC.
*/
#ifndef MAC_NRF52840_TRANSCEIVER_H
#define MAC_NRF52840_TRANSCEIVER_H 1

/* mac_nrf52840_driver.c part */
void mac_nrf52_transmitted_raw(zb_uint8_t * p_ack);
void mac_nrf52_transmit_failed(zb_uint8_t error);
void mac_nrf52_received_timestamp_raw(zb_uint8_t * p_data, zb_int8_t power, zb_uint8_t lqi, zb_uint32_t time);

/* zb_nrf52840_transceiver.c part */
void zb_transceiver_hw_init(void);
void zb_transceiver_update_long_mac(zb_ieee_addr_t long_mac);
void zb_transceiver_start_get_rssi(zb_uint8_t scan_duration_bi);
void zb_transceiver_set_promiscuous_mode(zb_bool_t enabled);
void zb_transceiver_set_rx_on_off(int rx_on);
zb_bool_t zb_transceiver_is_idle(void);
zb_bool_t zb_transceiver_transmit(zb_uint8_t wait_type, zb_time_t tx_at, zb_uint8_t * tx_buf, zb_uint8_t current_channel);
void zb_transceiver_buffer_free(zb_uint8_t * p_buf);
zb_bool_t zb_transceiver_set_pending_bit(zb_uint8_t *addr, zb_bool_t value, zb_bool_t extended);

/* radio driver FEM support part */
void nrf_fem_gpio_configure(void);

#endif /* MAC_NRF52840_TRANSCEIVER_H */
