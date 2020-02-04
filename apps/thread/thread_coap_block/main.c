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

#include "thread_dns.h"
#include "simple_thread.h"
#include "thread_coap.h"

APP_TIMER_DEF(coap_send_timer);

#define SCHED_QUEUE_SIZE 32
#define SCHED_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE

#define COAP_SERVER_HOSTNAME "coap-test.permamote.com"
#define DNS_SERVER_ADDR "fdaa:bb:1::2"

#define LED0 NRF_GPIO_PIN_MAP(0,4)
#define LED1 NRF_GPIO_PIN_MAP(0,5)
#define LED2 NRF_GPIO_PIN_MAP(0,6)

#define DEFAULT_CHILD_TIMEOUT    40                                         /**< Thread child timeout [s]. */
#define DEFAULT_POLL_PERIOD      1000                                       /**< Thread Sleepy End Device polling period when MQTT-SN Asleep. [ms] */
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

otError start_blockwise_transfer(otInstance* instance,
                              otCoapCode req,
                              otCoapType type,
                              const otIp6Address* dest,
                              const char* path,
                              const uint8_t* data,
                              size_t len,
                              size_t block_size,
                              otCoapResponseHandler response_handler) {
  otError       error = OT_ERROR_NONE;
  otMessage   * message;
  otMessageInfo message_info;

  if (otIp6IsAddressEqual(dest, &m_unspecified_ipv6))
  {
    NRF_LOG_INFO("Failed to send the CoAP Request to the Unspecified IPv6 Address\r\n");
    return OT_ERROR_INVALID_ARGS;
  }

  message = otCoapNewMessage(instance, NULL);
  if (message == NULL)
  {
    NRF_LOG_INFO("Failed to allocate message for CoAP Request\r\n");
    // Force an app error and subsequent reset - this shouldn't happen!
    return OT_ERROR_NO_BUFS;
  }

  otCoapMessageInit(message, type, req);
  otCoapMessageGenerateToken(message, 2);
  otCoapMessageAppendUriPathOptions(message, path);
  otCoapMessageAppendBlock1Option(message, 0, 0, block_size);
  otCoapMessageSetPayloadMarker(message);


  error = otMessageAppend(message, data, block_size);
  if (error != OT_ERROR_NONE)
  {
    return error;
  }

  memset(&message_info, 0, sizeof(message_info));
  message_info.mPeerPort    = OT_DEFAULT_COAP_PORT;
  memcpy(&message_info.mPeerAddr, dest, sizeof(message_info.mPeerAddr));

  error = otCoapSendRequest(instance,
      message,
      &message_info,
      response_handler,
      NULL);

  if (error != OT_ERROR_NONE && message != NULL)
  {
    NRF_LOG_INFO("Failed to send CoAP Request: %d\r\n", error);
    otMessageFree(message);
  }
  return error;

}

void send_timer_callback() {
  otInstance* thread_instance = thread_get_instance();

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
  } else {
    NRF_LOG_INFO("SENDING!");
    uint8_t data[320*320];
    size_t block_size = 512;
    for (size_t i = 0; i < sizeof(data)/block_size; i++) {
      memset(data+i*block_size, i, block_size);
    }

    start_blockwise_transfer(thread_instance, OT_COAP_CODE_PUT,
        OT_COAP_TYPE_CONFIRMABLE, &m_peer_address, "test", data, sizeof(data),
        block_size, NULL);
  }
}

int main(void) {
    nrf_power_dcdcen_set(1);

    log_init();

    // Initialize.
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_pin_set(LED2);

    thread_config_t thread_config = {
      .channel = 25,
      .panid = 0xFACE,
      .sed = true,
      .poll_period = DEFAULT_POLL_PERIOD,
      .child_period = DEFAULT_CHILD_TIMEOUT,
      .autocommission = true,
    };

    thread_init(&thread_config);
    otInstance* thread_instance = thread_get_instance();
    thread_coap_client_init(thread_instance);

    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    app_timer_init();
    app_timer_create(&coap_send_timer, APP_TIMER_MODE_REPEATED, send_timer_callback);
    app_timer_start(coap_send_timer, APP_TIMER_TICKS(5000), NULL);

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
