#pragma once

#include <openthread/dns_client.h>
#include <openthread/ip6.h>

void __attribute__((weak)) dns_response_handler(otError        error,
                          const otDnsAddressResponse *response,
                          void *context);

otError thread_dns_hostname_resolve(otInstance * p_instance,
                                    const char         * p_dns_addr,
                                    const char         * p_hostname,
                                    otDnsAddressCallback p_dns_address_callback,
                                    void               * p_context);
