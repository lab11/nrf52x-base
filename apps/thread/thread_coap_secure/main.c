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

#include <openthread/coap.h>
#include <openthread/coap_secure.h>
#include <openthread/crypto.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/message.h>
#include <openthread/sntp.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/openthread-system.h>

#include "simple_thread.h"
#include "gc_iot.h"
#include "thread_coap.h"

#include "config.h"

/***************************************************************************************************
 * @section Example Configuration.
 **************************************************************************************************/

/**
 * APP scheduler settings.
 */
#define SCHED_QUEUE_SIZE                      32
#define SCHED_EVENT_DATA_SIZE                 APP_TIMER_SCHED_EVENT_DATA_SIZE

/**
 * APP timer objects.
 */
APP_TIMER_DEF(m_sntp_timer);
APP_TIMER_DEF(m_test_timer);

#define TEST_MEASUREMENT_INTERVAL             5000
#define SNTP_QUERY_INTERVAL                   120000

/**
 * SNTP server address.
 */
#define SNTP_SERVER_ADDRESS                   "64:ff9b::d8ef:2308"
#define SNTP_SERVER_PORT                      123

/**
 * The JSON representation of the header with ES256 algorithm.
 */
#define JWT_HEADER_TYPE_ES256 \
    "{\"alg\":\"ES256\",\"typ\":\"JWT\"}"

/**
 * The maximum size of the JWT signature.
 */
#define JWT_SIGNATURE_SIZE 64

/**
 * The size of key length for ES256.
 */
#define JWT_KEY_LENGTH_ES256 32

/**
 * The JWT delimeter used to separete header, claim and signature.
 *
 */
#define JWT_DELIMETER '.'

/**
 * this variable indicates that Thread node is attached.
 */
static bool m_is_attached;

/**
 * Time obtained from SNTP.
 */
static uint64_t m_unix_time;

/**
 * Test Counter.
 */
static int32_t test_counter = 0;

/**
 * Forward declarations.
 */
static void sntp_query(void);

static otIp6Address m_peer_address;
static otIp6Address m_sntp_address;

/***************************************************************************************************
 * @section Callbacks
 **************************************************************************************************/

#if GCP_COAP_SECURE_ENABLED
void coap_secure_connect_handler(bool connected, void *aContext) {
    if (connected) {
      NRF_LOG_INFO("CONNECTED!");
    } else {
      NRF_LOG_INFO("CONNECT FAILED!");
    }
}
#endif


/**@brief Thread state changed handler.
 */
void __attribute__((weak)) thread_state_changed_callback(uint32_t flags, void * p_context) {
    ret_code_t error = NRF_SUCCESS;

    NRF_LOG_INFO("State changed! Flags: 0x%08lx Current role: %d",
                 flags, otThreadGetDeviceRole(p_context));

    if (flags & OT_CHANGED_IP6_ADDRESS_ADDED && otThreadGetDeviceRole(p_context) == OT_DEVICE_ROLE_CHILD) {
      m_is_attached = true;

      // connect to server
      error = thread_coap_secure_connect(thread_get_instance(),
          &m_peer_address,
          GCP_COAP_IOT_CORE_SERVER_SECURE_PORT,
          (uint8_t*) GCP_COAP_SECURE_PSK_SECRET,
          strlen(GCP_COAP_SECURE_PSK_SECRET),
          (uint8_t*) GCP_COAP_SECURE_PSK_IDENTITY,
          strlen(GCP_COAP_SECURE_PSK_IDENTITY),
          coap_secure_connect_handler);
      NRF_LOG_ERROR("error connect: %d", error);

      // Send SNTP query.
      sntp_query();

      error = app_timer_start(m_test_timer, APP_TIMER_TICKS(TEST_MEASUREMENT_INTERVAL), NULL);
      ASSERT(error == NRF_SUCCESS);

      error = app_timer_start(m_sntp_timer, APP_TIMER_TICKS(SNTP_QUERY_INTERVAL), NULL);
      ASSERT(error == NRF_SUCCESS);
    }
    else if (otThreadGetDeviceRole(thread_get_instance()) == OT_DEVICE_ROLE_DETACHED) {
      m_is_attached = false;

      error = app_timer_stop(m_test_timer);
      ASSERT(error == NRF_SUCCESS);

      error = app_timer_stop(m_sntp_timer);
      ASSERT(error == NRF_SUCCESS);
    }
}

void thread_coap_receive_handler(void                * p_context,
                                 otMessage           * p_message,
                                 const otMessageInfo * p_message_info)
{
    (void)p_context;
    (void)p_message;
    (void)p_message_info;

    NRF_LOG_INFO("Received CoAP message that does not match any request or resource\r\n");
}


/**@brief SNTP timer handler. */
static void sntp_timer_handler(void * p_context)
{
    (void)p_context;

    sntp_query();
}


/**@brief SNTP response handler. */
static void sntp_response_handler(void *p_context, uint64_t unix_time, otError result)
{
    (void)p_context;

    if (result == OT_ERROR_NONE)
    {
        m_unix_time = unix_time;
    }
}


/**@brief CoAP response handler. */
static void coap_response_handler(void                * p_context,
                                  otMessage           * p_message,
                                  const otMessageInfo * p_message_info,
                                  otError               result)
{
    (void)p_message_info;
}


/**@brief Test data timer handler */
static void test_timer_handler(void * p_context)
{
    (void)p_context;
    test_counter++;
    char test[64];
    snprintf(test, 64, "counter: %ld", test_counter);
    gc_iot_coap_publish(&m_peer_address, GCP_COAP_IOT_CORE_SERVER_SECURE_PORT, m_unix_time, (uint8_t*) test, strlen(test), coap_response_handler);
}

/***************************************************************************************************
 * @section Simple Network Time Protocol query.
 **************************************************************************************************/

static void sntp_query(void)
{
    otError error = OT_ERROR_NONE;

    otMessageInfo message_info;
    memset(&message_info, 0, sizeof(message_info));
    message_info.mPeerPort = SNTP_SERVER_PORT;

    error = otIp6AddressFromString(SNTP_SERVER_ADDRESS, &message_info.mPeerAddr);
    ASSERT(error == OT_ERROR_NONE);

    otSntpQuery query;
    query.mMessageInfo = &message_info;

    error = otSntpClientQuery(thread_get_instance(), &query, sntp_response_handler, NULL);
    ASSERT(error == OT_ERROR_NONE);
}


/***************************************************************************************************
 * @section Initialization
 **************************************************************************************************/

/**@brief Function for initializing the Application Timer Module.
 */
static void timer_init(void)
{
    uint32_t error_code = app_timer_init();
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&m_sntp_timer,
                                  APP_TIMER_MODE_REPEATED,
                                  sntp_timer_handler);
    APP_ERROR_CHECK(error_code);

    error_code = app_timer_create(&m_test_timer,
                                  APP_TIMER_MODE_REPEATED,
                                  test_timer_handler);
    APP_ERROR_CHECK(error_code);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}



/**@brief Function for initializing scheduler module.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/***************************************************************************************************
 * @section Main
 **************************************************************************************************/

int main(int argc, char * argv[])
{
    nrf_power_dcdcen_set(1);

    log_init();
    scheduler_init();
    timer_init();

    // Initialize.
    otIp6AddressFromString(GCP_COAP_IOT_CORE_SERVER_ADDRESS, &m_peer_address);
    NRF_LOG_INFO("TEST\n\n");
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

    gc_iot_coap_set_core_params(GCP_COAP_IOT_CORE_PATH,
        GCP_COAP_IOT_CORE_DEVICE_ID, GCP_COAP_IOT_CORE_DEVICE_KEY,
        GCP_COAP_IOT_CORE_PROJECT_ID, GCP_COAP_IOT_CORE_REGISTRY_ID,
        GCP_COAP_IOT_CORE_REGION, GCP_COAP_IOT_CORE_PUBLISH,
        GCP_COAP_IOT_CORE_CONFIG);

    while (true)
    {
        thread_process();
        app_sched_execute();

        if (NRF_LOG_PROCESS() == false)
        {
            thread_sleep();
        }
    }
}

/**
 *@}
 **/
