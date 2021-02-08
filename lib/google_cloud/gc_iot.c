#include "string.h"
#include "stdio.h"

#include "nrf_log.h"

#include "gc_iot.h"
#include "base64/base64.h"
#include "simple_thread.h"

#include <mbedtls/sha256.h>
#include <mbedtls/base64.h>
#include <mbedtls/pk.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/ctr_drbg.h>

#define TIMEOUT_INTERVAL                   120000

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

#define BUFFER_LENGTH 512

static const char*  GCP_COAP_IOT_CORE_PATH;
static const char*  GCP_COAP_IOT_CORE_DEVICE_ID;
static const char*  GCP_COAP_IOT_CORE_DEVICE_KEY;
static const char*  GCP_COAP_IOT_CORE_PROJECT_ID;
static const char*  GCP_COAP_IOT_CORE_REGISTRY_ID;
static const char*  GCP_COAP_IOT_CORE_REGION;
static const char*  GCP_COAP_IOT_CORE_PUBLISH;
static const char*  GCP_COAP_IOT_CORE_CONFIG;

void gc_iot_coap_set_core_params(const char* path,
                                 const char* device_id,
                                 const char* device_key,
                                 const char* project_id, const char* registry_id,
                                 const char* region, const char* event,
                                 const char* config)
{
  GCP_COAP_IOT_CORE_PATH = path;
  GCP_COAP_IOT_CORE_DEVICE_ID = device_id;
  GCP_COAP_IOT_CORE_DEVICE_KEY = device_key;
  GCP_COAP_IOT_CORE_PROJECT_ID = project_id;
  GCP_COAP_IOT_CORE_REGISTRY_ID = registry_id;
  GCP_COAP_IOT_CORE_REGION = region;
  GCP_COAP_IOT_CORE_PUBLISH = event;
  GCP_COAP_IOT_CORE_CONFIG = config;
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
        //NRF_LOG_INFO("MBEDTLS b64 encode: %d", result);
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
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("jwt header b64 url encode fail: %d, len: %d:%d", error, length, output_max_length);
      return error;
    }

    *p_output_len = length;

    // Append delimiter.
    p_output[*p_output_len] = JWT_DELIMETER;
    *p_output_len += 1;

    // Encode JWT Claim using Base64 URL.
    length = output_max_length - *p_output_len;

    error = base64_url_encode(p_output + *p_output_len, &length, p_claims, claims_len);
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("jwt claim b64 url encode fail");
      return error;
    }

    *p_output_len += length;

    // Create SHA256 Hash from encoded JWT Header and JWT Claim.
    int err = mbedtls_sha256_ret(p_output, *p_output_len, hash, 0);
    if (err != 0) {
      NRF_LOG_INFO("jwt sha256 hash fail encode fail");
      return error;
    }

    // Append delimiter.
    p_output[*p_output_len] = JWT_DELIMETER;
    *p_output_len += 1;

    // Create ECDSA Sign.
    error = otCryptoEcdsaSign(signature, &signature_size, hash, sizeof(hash), p_private_key, private_key_len);
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("jwt ecdsa sign fail");
      return error;
    }

    // Encode JWT Sign using Base64 URL.
    length = output_max_length - *p_output_len;

    error = base64_url_encode(p_output + *p_output_len, &length, signature, signature_size);
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("jwt sign b64 encode fail");
      return error;
    }
    *p_output_len += length;

    return error;
}

/***************************************************************************************************
 * @section CoAP messages.
 **************************************************************************************************/

static void coap_header_proxy_uri_append(otMessage * p_message, const char * p_action, uint64_t unix_time)
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

    uint64_t timeout = unix_time + (TIMEOUT_INTERVAL/1000) * 2;

    uint16_t length = snprintf(claims, sizeof(claims), "{\"iat\":%ld,\"exp\":%ld,\"aud\":\"%s\"}",
                               (uint32_t)(unix_time), (uint32_t)(timeout), GCP_COAP_IOT_CORE_PROJECT_ID);
    if (length <= 0) {
      NRF_LOG_INFO("json sprintf failed");
      return;
    }

    // strlen for device key +1 for null byte
    error = jwt_create((uint8_t *)&jwt[offset], &output_len, (const uint8_t *)claims, strlen(claims),
                               (const uint8_t *)GCP_COAP_IOT_CORE_DEVICE_KEY, strlen(GCP_COAP_IOT_CORE_DEVICE_KEY) + 1);
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("jwt create failed: %d", error);
      return;
    }

    error = otCoapMessageAppendProxyUriOption(p_message, jwt);
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("message append failed");
      return;
    }
}


/*
 * So they harcode adding the relevent data to the coap packet here
*/
static void gc_iot_coap_payload_append(otMessage * p_message, uint8_t* data, size_t len, uint64_t unix_time) {
    char payload[BUFFER_LENGTH];
    char b64encoded[BUFFER_LENGTH];

    memset(payload, 0, sizeof(payload));
    bintob64(b64encoded, data, len);

    uint16_t length = snprintf(payload, sizeof(payload), "{\"d\":\"%s\"}",
                               b64encoded);
    //NRF_LOG_INFO("sent payload: %s", payload);
    otError error = otMessageAppend(p_message, payload, length);
    if (error != OT_ERROR_NONE) {
      NRF_LOG_INFO("message append failed: %d", error);
      return;
    }
}

otError gc_iot_coap_publish(otIp6Address* dest, uint16_t port, uint64_t unix_time, uint8_t* data, size_t len, otCoapResponseHandler handler)
{
    otError       error = OT_ERROR_NONE;
    otMessage   * p_message;
    otMessageInfo message_info;

//#if GCP_COAP_SECURE_ENABLED
    if (!otCoapSecureIsConnected(thread_get_instance()) && !otCoapSecureIsConnectionActive(thread_get_instance()))
    {
      // connect to server
      //error = thread_coap_secure_connect(thread_get_instance(),
      //    &m_peer_address,
      //    GCP_COAP_IOT_CORE_SERVER_SECURE_PORT,
      //    (uint8_t*) GCP_COAP_SECURE_PSK_SECRET,
      //    strlen(GCP_COAP_SECURE_PSK_SECRET),
      //    (uint8_t*) GCP_COAP_SECURE_PSK_IDENTITY,
      //    strlen(GCP_COAP_SECURE_PSK_IDENTITY),
      //    coap_secure_connect_handler);
      return OT_ERROR_INVALID_STATE;
    }
//#endif

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
        if (error != OT_ERROR_NONE) {
          return error;
        }

        coap_header_proxy_uri_append(p_message, GCP_COAP_IOT_CORE_PUBLISH, unix_time);

        error = otCoapMessageSetPayloadMarker(p_message);
        if (error != OT_ERROR_NONE) {
          return error;
        }

        gc_iot_coap_payload_append(p_message, data, len, unix_time);

        // Set message info structure to point on GCP server.
        memset(&message_info, 0, sizeof(message_info));
        message_info.mPeerPort = port;
        memcpy(&message_info.mPeerAddr, dest, sizeof(otIp6Address));
        //error = otIp6AddressFromString(GCP_COAP_IOT_CORE_SERVER_ADDRESS, &message_info.mPeerAddr);
        //ASSERT(error == OT_ERROR_NONE);

//#if GCP_COAP_SECURE_ENABLED
        error = otCoapSecureSendRequest(thread_get_instance(),
                                        p_message,
                                        handler,
                                        NULL);
//#else
        //error = otCoapSendRequest(thread_get_instance(),
        //                          p_message,
        //                          &message_info,
        //                          coap_response_handler,
        //                          NULL);
//#endif
    } while (false);

    if (error != OT_ERROR_NONE && p_message != NULL)
    {
        otMessageFree(p_message);
    }
  return OT_ERROR_NONE;
}


