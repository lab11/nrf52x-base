#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "nrf_timer.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "simple_thread.h"
#include "thread_coap.h"

APP_TIMER_DEF(coap_send_timer);

#define SCHED_QUEUE_SIZE 32
#define SCHED_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE

/**
 * CoAP transport configuration.
 * Must be configured by the user.
 */
#define GCP_COAP_SECURE_ENABLED               1
#define GCP_COAP_SECURE_PSK_SECRET            "b1lt3$t"
#define GCP_COAP_SECURE_PSK_IDENTITY          "test"

/**
 * Google Cloud Platform CoAP server parameters.
 */
#define GCP_COAP_IOT_CORE_SERVER_PORT         5683
#define GCP_COAP_IOT_CORE_SERVER_SECURE_PORT  5684

/**
 * Google Cloud Platform project configuration.
 * Must be configured by the user.
 */
#define GCP_COAP_IOT_CORE_SERVER_ADDRESS      "64:ff9b::23ee:2ab0"
#define GCP_COAP_IOT_CORE_PATH                "gcp"
#define GCP_COAP_IOT_CORE_PROJECT_ID          "blue-iris-labs-289016"
#define GCP_COAP_IOT_CORE_REGISTRY_ID         "pupil"
#define GCP_COAP_IOT_CORE_REGION              "us-central1"
#define GCP_COAP_IOT_CORE_PUBLISH             "publishEvent"
#define GCP_COAP_IOT_CORE_CONFIG              "config"

/**
 * Google Cloud Platform device configuration.
 * Must be configured by the user.
 */
#define GCP_COAP_IOT_CORE_DEVICE_ID          "example"
#define GCP_COAP_IOT_CORE_DEVICE_KEY                                   \
"-----BEGIN EC PRIVATE KEY-----\r\n"                                   \
"MHcCAQEEIIQvsYV7bLafjmXsmw+xXrSrFWjb6McsKOobxKSHPZ0UoAoGCCqGSM49\r\n" \
"AwEHoUQDQgAE4vbexsaPttmfbCZNu0jhwAiAfxH0VZ9qDDKKr15ZHqMXSLkukDAv\r\n" \
"+N+C+Qgg6vN4ZfgM+wGerLalElDJObn0gQ==\r\n"                             \
"-----END EC PRIVATE KEY-----\r\n"

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


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


void send_timer_callback() {
  const uint8_t* data = (uint8_t*)"hello";
  otInstance* thread_instance = thread_get_instance();
  thread_coap_send(thread_instance, OT_COAP_CODE_PUT, OT_COAP_TYPE_NON_CONFIRMABLE, &m_peer_address, "test", data, strnlen((char*)data, 6), true, NULL);
  NRF_LOG_INFO("Sent test message!");
}

void coap_secure_connect_handler(bool connected, void *aContext) {
  NRF_LOG_INFO("CONNECTED: %d", connected);
}

void __attribute__((weak)) thread_state_changed_callback(uint32_t flags, void * p_context) {
    NRF_LOG_INFO("State changed! Flags: 0x%08lx Current role: %d",
                 flags, otThreadGetDeviceRole(p_context));

    if (flags & OT_CHANGED_IP6_ADDRESS_ADDED && otThreadGetDeviceRole(p_context) == 2) {
      NRF_LOG_INFO("We have internet connectivity!");
      otError error = thread_coap_secure_connect(thread_get_instance(),
          &m_peer_address,
          GCP_COAP_IOT_CORE_SERVER_SECURE_PORT,
          (uint8_t*) GCP_COAP_SECURE_PSK_SECRET,
          strlen(GCP_COAP_SECURE_PSK_SECRET),
          (uint8_t*) GCP_COAP_SECURE_PSK_IDENTITY,
          strlen(GCP_COAP_SECURE_PSK_IDENTITY),
          coap_secure_connect_handler);
      NRF_LOG_ERROR("error connect: %d", error);


    }
}

int main(void) {
    nrf_power_dcdcen_set(1);

    log_init();

    // Initialize.
    otIp6AddressFromString(GCP_COAP_IOT_CORE_SERVER_ADDRESS, &m_peer_address);

    for(size_t i = 0; i < 8; i++) {
      printf("%x ", m_peer_address.mFields.m16[i]);
    }
    printf("\n");

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

    thread_coap_client_init(thread_instance, true);
    thread_process();
    //app_sched_execute();


    // Enter main loop.
    while (1) {
        thread_process();
        //app_sched_execute();

        if (NRF_LOG_PROCESS() == false)
        {
          thread_sleep();
        }
    }
}
