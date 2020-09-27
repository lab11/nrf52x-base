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

#include <mbedtls/sha256.h>
#include <mbedtls/base64.h>
#include <mbedtls/pk.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/ctr_drbg.h>

#include "simple_thread.h"
#include "thread_coap.h"

#include "config.h"

static char buffer[256] = "test";

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
 * Temporary buffer size.
 */
#define BUFFER_LENGTH                         512

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
static void coap_publish(void);
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
      //coap_publish();
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
    coap_publish();
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
 * @section JWT generation.
 **************************************************************************************************/

static otError base64_url_encode(uint8_t *p_output, uint16_t *p_output_len, const uint8_t *p_buff, uint16_t length)
{
    otError error = OT_ERROR_NONE;
    int     result;
    size_t  encoded_len = 0;

    result = mbedtls_base64_encode(p_output, *p_output_len, &encoded_len, p_buff, length);

    if (result != 0)
    {
        return OT_ERROR_NO_BUFS;
    }

    // JWT uses URI as defined in RFC4648, while mbedtls as is in RFC1421.
    for (uint32_t index = 0; index < encoded_len; index++)
    {
        if (p_output[index] == '+')
        {
            p_output[index] = '-';
        }
        else if (p_output[index] == '/')
        {
            p_output[index] = '_';
        }
        else if (p_output[index] == '=')
        {
            p_output[index] = 0;
            encoded_len  = index;
            break;
        }
    }

    *p_output_len = encoded_len;

    return error;
}

static otError jwt_create(uint8_t       * p_output,
                          uint16_t      * p_output_len,
                          const uint8_t * p_claims,
                          uint16_t        claims_len,
                          const uint8_t * p_private_key,
                          uint16_t        private_key_len)
{
    otError                error = OT_ERROR_NONE;
    uint8_t                hash[32];
    uint8_t                signature[JWT_SIGNATURE_SIZE];
    uint16_t               signature_size    = JWT_SIGNATURE_SIZE;
    uint16_t               output_max_length = *p_output_len;
    uint16_t               length;

    // Encode JWT Header using Base64 URL.
    length = output_max_length;

    error = base64_url_encode(p_output, &length, (const uint8_t *)JWT_HEADER_TYPE_ES256,
                              strlen(JWT_HEADER_TYPE_ES256));
    ASSERT(error == OT_ERROR_NONE);

    *p_output_len = length;

    // Append delimiter.
    p_output[*p_output_len] = JWT_DELIMETER;
    *p_output_len += 1;

    // Encode JWT Claim using Base64 URL.
    length = output_max_length - *p_output_len;

    error = base64_url_encode(p_output + *p_output_len, &length, p_claims, claims_len);
    ASSERT(error == OT_ERROR_NONE);

    *p_output_len += length;

    // Create SHA256 Hash from encoded JWT Header and JWT Claim.
    int err = mbedtls_sha256_ret(p_output, *p_output_len, hash, 0);
    ASSERT(err == 0);

    // Append delimiter.
    p_output[*p_output_len] = JWT_DELIMETER;
    *p_output_len += 1;


    //mbedtls_ecdsa_context ctx;
    //mbedtls_pk_context    pkCtx;
    //mbedtls_ecp_keypair  *keypair;
    //mbedtls_mpi           rMpi;
    //mbedtls_mpi           sMpi;

    //int ret = mbedtls_pk_parse_key(&pkCtx, p_private_key, private_key_len, NULL, 0);
    //NRF_LOG_INFO("PK PARSE ERROR: %d", ret);
    //int key_type = mbedtls_pk_get_type(&pkCtx);
    //NRF_LOG_INFO("PK key type: %d, %d", key_type, MBEDTLS_PK_ECKEY);
    //keypair = mbedtls_pk_ec(pkCtx);
    //NRF_LOG_INFO("ECDSA keypair: %x", keypair);
    //mbedtls_ecdsa_init(&ctx);
    //NRF_LOG_INFO("ECDSA INIT: %d", ret);
    //ret = mbedtls_ecdsa_from_keypair(&ctx, keypair);
    //NRF_LOG_INFO("ECDSA from keypair: %d", ret);
    //ret = mbedtls_ecdsa_sign_det(&ctx.grp, &rMpi, &sMpi, &ctx.d, hash, sizeof(hash), MBEDTLS_MD_SHA256);
    //NRF_LOG_INFO("ECDSA sign: %x", ret);
    //mbedtls_strerror(ret, buffer, 256);
    //printf("ECDSA sign: %s\n", buffer);


    // Create ECDSA Sign.
    error = otCryptoEcdsaSign(signature, &signature_size, hash, sizeof(hash), p_private_key, private_key_len);
    ASSERT(error == OT_ERROR_NONE);

    // Encode JWT Sign using Base64 URL.
    length = output_max_length - *p_output_len;

    error = base64_url_encode(p_output + *p_output_len, &length, signature, signature_size);
    ASSERT(error == OT_ERROR_NONE);

    *p_output_len += length;

    return error;
}

/***************************************************************************************************
 * @section CoAP messages.
 **************************************************************************************************/

static void coap_header_proxy_uri_append(otMessage * p_message, const char * p_action)
{
    otError error = OT_ERROR_NONE;
    char    jwt[BUFFER_LENGTH];
    char    claims[BUFFER_LENGTH];

    memset(jwt, 0, sizeof(jwt));
    memset(claims, 0, sizeof(claims));

    uint16_t offset = snprintf(jwt, sizeof(jwt), "%s/%s/%s/%s/%s?jwt=",
                               GCP_COAP_IOT_CORE_PROJECT_ID, GCP_COAP_IOT_CORE_REGION,
                               GCP_COAP_IOT_CORE_REGISTRY_ID, GCP_COAP_IOT_CORE_DEVICE_ID,
                               p_action);

    uint16_t output_len = sizeof(jwt) - offset;

    uint64_t timeout = m_unix_time + (SNTP_QUERY_INTERVAL/1000) * 2;

    uint16_t length = snprintf(claims, sizeof(claims), "{\"iat\":%ld,\"exp\":%ld,\"aud\":\"%s\"}",
                               (uint32_t)(m_unix_time), (uint32_t)(timeout), GCP_COAP_IOT_CORE_PROJECT_ID);
    ASSERT(length > 0);

    error = jwt_create((uint8_t *)&jwt[offset], &output_len, (const uint8_t *)claims, strlen(claims),
                               (const uint8_t *)GCP_COAP_IOT_CORE_DEVICE_KEY, sizeof(GCP_COAP_IOT_CORE_DEVICE_KEY));
    ASSERT(error == OT_ERROR_NONE);

    error = otCoapMessageAppendProxyUriOption(p_message, jwt);
    ASSERT(error == OT_ERROR_NONE);
}


/*
 * So they harcode adding the relevent data to the coap packet here
*/
static void coap_payload_append(otMessage * p_message) {
    char payload[BUFFER_LENGTH];

    memset(payload, 0, sizeof(payload));

    uint16_t length = snprintf(payload, sizeof(payload), "{\"test\":%ld}",
                               test_counter);

    otError error = otMessageAppend(p_message, payload, length);
    ASSERT(error == OT_ERROR_NONE);
}

static void coap_publish(void)
{
    otError       error = OT_ERROR_NONE;
    otMessage   * p_message;
    otMessageInfo message_info;

#if GCP_COAP_SECURE_ENABLED
    if (!otCoapSecureIsConnected(thread_get_instance()) && !otCoapSecureIsConnectionActive(thread_get_instance()))
    {
      // connect to server
      error = thread_coap_secure_connect(thread_get_instance(),
          &m_peer_address,
          GCP_COAP_IOT_CORE_SERVER_SECURE_PORT,
          (uint8_t*) GCP_COAP_SECURE_PSK_SECRET,
          strlen(GCP_COAP_SECURE_PSK_SECRET),
          (uint8_t*) GCP_COAP_SECURE_PSK_IDENTITY,
          strlen(GCP_COAP_SECURE_PSK_IDENTITY),
          coap_secure_connect_handler);
        return;
    }
#endif

    do
    {
        p_message = otCoapNewMessage(thread_get_instance(), NULL);
        if (p_message == NULL)
        {
            break;
        }

        otCoapMessageInit(p_message, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_POST);
        otCoapMessageGenerateToken(p_message, 2);

        error = otCoapMessageAppendUriPathOptions(p_message, GCP_COAP_IOT_CORE_PATH);
        ASSERT(error == OT_ERROR_NONE);

        coap_header_proxy_uri_append(p_message, GCP_COAP_IOT_CORE_PUBLISH);

        error = otCoapMessageSetPayloadMarker(p_message);
        ASSERT(error == OT_ERROR_NONE);

        coap_payload_append(p_message);

        // Set message info structure to point on GCP server.
        memset(&message_info, 0, sizeof(message_info));
        message_info.mPeerPort = GCP_COAP_IOT_CORE_SERVER_PORT;

        error = otIp6AddressFromString(GCP_COAP_IOT_CORE_SERVER_ADDRESS, &message_info.mPeerAddr);
        ASSERT(error == OT_ERROR_NONE);

#if GCP_COAP_SECURE_ENABLED
        error = otCoapSecureSendRequest(thread_get_instance(),
                                        p_message,
                                        coap_response_handler,
                                        NULL);
#else
        error = otCoapSendRequest(thread_get_instance(),
                                  p_message,
                                  &message_info,
                                  coap_response_handler,
                                  NULL);
#endif
    } while (false);

    if (error != OT_ERROR_NONE && p_message != NULL)
    {
        otMessageFree(p_message);
    }
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
