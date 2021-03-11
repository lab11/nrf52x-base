#include "thread_dns.h"

#include <string.h>
#include "nrf_log.h"

void __attribute__((weak)) dns_response_handler(otError        error,
                          const otDnsAddressResponse *response,
                          void *context)
{
    if (error != OT_ERROR_NONE)
    {
        NRF_LOG_INFO("DNS response error %d.", error);
        return;
    }

    NRF_LOG_INFO("Successfully resolved address");
    otDnsAddressResponseGetAddress(response, 0, context, NULL);
}

otError thread_dns_hostname_resolve(otInstance * p_instance,
                                    const char         * p_dns_addr,
                                    const char         * p_hostname,
                                    otDnsAddressCallback p_dns_address_callback,
                                    void               * p_context)
{
    otError       error;
    otDnsQueryConfig config;
    memcpy(&config, otDnsClientGetDefaultConfig(p_instance), sizeof(config));

    otIp6AddressFromString(p_dns_addr, &config.mServerSockAddr.mAddress);
    error = otDnsClientResolveAddress(p_instance, p_hostname, p_dns_address_callback, p_context, &config);

    if (error != OT_ERROR_NONE)
    {
        NRF_LOG_INFO("Failed to perform DNS Query.\r\n");
    }

    return error;
}
