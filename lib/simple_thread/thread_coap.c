#include "nrf_log.h"

#include "thread_coap.h"
#include "app_error.h"

static const otIp6Address m_unspecified_ipv6 =
{
    .mFields =
    {
        .m8 = {0}
    }
};

void __attribute__((weak)) thread_coap_receive_handler(
                                void                * p_context,
                                otMessage           * p_message,
                                const otMessageInfo * p_message_info)
{
    (void)p_context;
    (void)p_message;
    (void)p_message_info;

    NRF_LOG_INFO("Received CoAP message that does not match any request or resource\r\n");
}

void thread_coap_client_init(otInstance* instance, bool secure) {
    otError error = OT_ERROR_NONE;
    if (secure) {
      error = otCoapSecureStart(instance, OT_DEFAULT_COAP_SECURE_PORT);
    } else {
      error = otCoapStart(instance, OT_DEFAULT_COAP_PORT);
    }
    ASSERT(error == OT_ERROR_NONE);

    otCoapSetDefaultHandler(instance, thread_coap_receive_handler, NULL);
}

otError thread_coap_secure_connect(otInstance* instance,
                                   const otIp6Address* dest,
                                   const uint16_t port,
                                   const uint8_t* psk_secret,
                                   size_t psk_secret_len,
                                   const uint8_t* psk_identity,
                                   size_t psk_identity_len,
                                   otHandleCoapSecureClientConnect connect_handler)
{
    otError error = OT_ERROR_NONE;

    otSockAddr sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));

    sock_addr.mPort = port;
    memcpy(&sock_addr.mAddress, dest, sizeof(otIp6Address));

    otCoapSecureSetPsk(instance,
                       psk_secret,
                       psk_secret_len,
                       psk_identity,
                       psk_identity_len);

    error = otCoapSecureConnect(instance,
                                &sock_addr,
                                connect_handler,
                                NULL);

    return error;
}

otError thread_coap_send(otInstance* instance,
                         otCoapCode req,
                         otCoapType type,
                         const otIp6Address* dest,
                         const char* path,
                         const uint8_t* data,
                         size_t len,
                         bool secure,
                         otCoapResponseHandler response_handler) {
  otError       error = OT_ERROR_NONE;
  otMessage   * message;
  otMessageInfo message_info;

  if (otIp6IsAddressEqual(dest, &m_unspecified_ipv6))
  {
    NRF_LOG_ERROR("Failed to send the CoAP Request to the Unspecified IPv6 Address\r\n");
    return OT_ERROR_INVALID_ARGS;
  }

  if (secure && !otCoapSecureIsConnected(instance) && !otCoapSecureIsConnectionActive(instance))
  {
      NRF_LOG_ERROR("Not connected to CoAP peer\r\n");
      return OT_ERROR_INVALID_STATE;
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
  otCoapMessageSetPayloadMarker(message);


  error = otMessageAppend(message, data, len);
  if (error != OT_ERROR_NONE)
  {
    return error;
  }

  memset(&message_info, 0, sizeof(message_info));
  message_info.mPeerPort    = OT_DEFAULT_COAP_PORT;
  memcpy(&message_info.mPeerAddr, dest, sizeof(message_info.mPeerAddr));

  if (secure)
  {
    error = otCoapSecureSendRequest(instance,
                                    message,
                                    response_handler,
                                    NULL);
  }
  else {
    error = otCoapSendRequest(instance,
                              message,
                              &message_info,
                              response_handler,
                              NULL);
  }

  if (error != OT_ERROR_NONE && message != NULL)
  {
    NRF_LOG_INFO("Failed to send CoAP Request: %d\r\n", error);
    otMessageFree(message);
  }
  return error;
}
