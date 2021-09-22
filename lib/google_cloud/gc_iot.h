#pragma once

#include <openthread/coap.h>
#include <openthread/coap_secure.h>
#include <openthread/crypto.h>
#include <openthread/message.h>
#include <openthread/thread.h>

void gc_iot_coap_set_core_params(const char* path,
                                 const char* device_id,
                                 const char* device_key,
                                 const char* project_id, const char* registry_id,
                                 const char* region, const char* event,
                                 const char* config);

otError gc_iot_coap_publish(otIp6Address* dest, uint16_t port, uint64_t unix_time,
                            uint8_t* data, size_t len, otCoapResponseHandler handler);

