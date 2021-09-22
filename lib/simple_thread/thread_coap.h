#pragma once

#include <stdint.h>

#include <openthread/coap.h>
#include <openthread/coap_secure.h>

void thread_coap_client_init(otInstance* instance, bool secure);

otError thread_coap_secure_connect(otInstance* instance,
                                   const otIp6Address* dest,
                                   const uint16_t port,
                                   const uint8_t* psk_secret,
                                   size_t psk_secret_len,
                                   const uint8_t* psk_identity,
                                   size_t psk_identity_len,
                                   otHandleCoapSecureClientConnect connect_handler);

otError thread_coap_send(otInstance* instance,
                         otCoapCode req,
                         otCoapType type,
                         const otIp6Address* dest,
                         const char* path,
                         const uint8_t* data,
                         size_t len,
                         bool secure,
                         otCoapResponseHandler response_handler);
