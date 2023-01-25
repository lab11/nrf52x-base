#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "app_timer.h"
#include "app_scheduler.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "simple_thread.h"
#include "thread_dns.h"
#include "thread_coap.h"

APP_TIMER_DEF(coap_send_timer);

#define SCHED_QUEUE_SIZE 32
#define SCHED_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE

#define COAP_SERVER_HOSTNAME "google.com"
#define DNS_SERVER_ADDR "fdaa:bb:1::2"

#define LED0 NRF_GPIO_PIN_MAP(0,4)
#define LED1 NRF_GPIO_PIN_MAP(0,5)
#define LED2 NRF_GPIO_PIN_MAP(0,6)

#define DEFAULT_CHILD_TIMEOUT    40                                         /**< Thread child timeout [s]. */
#define DEFAULT_POLL_PERIOD      100                                        /**< Thread Sleepy End Device polling period when MQTT-SN Asleep. [ms] */
#define NUM_SLAAC_ADDRESSES      4                                          /**< Number of SLAAC addresses. */

static otIp6Address m_peer_address =
{
    .mFields =
    {
        .m8 = {0}
    }
};

static const otIp6Address m_unspecified_ipv6 =
{
    .mFields =
    {
        .m8 = {0}
    }
};

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void dns_response_handler(void         * p_context,
                          const char   * p_hostname,
                          const otIp6Address * p_resolved_address,
                          uint32_t       ttl,
                          otError        error)
{
    if (error != OT_ERROR_NONE)
    {
        NRF_LOG_INFO("DNS response error %d.", error);
        return;
    }

    NRF_LOG_INFO("Successfully resolved address");
    m_peer_address = *p_resolved_address;
}

void send_timer_callback() {
  otInstance* thread_instance = thread_get_instance();
  const uint8_t* data = (uint8_t*)"hello";

  // If IPv6 address of the server is unspecified try to resolve hostname.
  if (otIp6IsAddressEqual(&m_peer_address, &m_unspecified_ipv6))
  {
    if (thread_dns_hostname_resolve(thread_instance,
          DNS_SERVER_ADDR,
          COAP_SERVER_HOSTNAME,
          dns_response_handler,
          NULL) != OT_ERROR_NONE) {
        return;
    }
  }
  else {
      thread_coap_send(thread_instance, OT_COAP_CODE_PUT, OT_COAP_TYPE_NON_CONFIRMABLE, &m_peer_address, "test", data, strnlen((char*)data, 6), false, NULL);
      NRF_LOG_INFO("Sent test message!");
  }
}

int main(void) {
    nrf_power_dcdcen_set(1);

    log_init();

    // Initialize.
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_pin_set(LED2);

    otMasterKey masterkey = {.m8 = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}};

    thread_config_t thread_config = {
      .panid = 0xFACE,
      .masterkey = &masterkey,
      .channel = 25,
      .tx_power = 8,
      .sed = false,
      .poll_period = DEFAULT_POLL_PERIOD,
      .child_period = DEFAULT_CHILD_TIMEOUT,
      .autocommission = true,
    };

    thread_init(&thread_config);
    otInstance* thread_instance = thread_get_instance();
    thread_coap_client_init(thread_instance, false);

    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    app_timer_init();
    app_timer_create(&coap_send_timer, APP_TIMER_MODE_REPEATED, send_timer_callback);
    app_timer_start(coap_send_timer, APP_TIMER_TICKS(10000), NULL);

    // Enter main loop.
    while (1) {
        thread_process();
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false)
        {
          thread_sleep();
        }
    }
}
