// Copied from SDK16 retarget.c
// with modifications to support Micro:bit v2 printing over UART
// Requires logging to first be initialized before printing

#include "sdk_common.h"

#if NRF_MODULE_ENABLED(RETARGET)
#if !defined(NRF_LOG_USES_RTT) || NRF_LOG_USES_RTT != 1
#if !defined(HAS_SIMPLE_UART_RETARGET)

#include <stdio.h>
#include <stdint.h>
#include "app_uart.h"
#include "nrf_error.h"
#include "nrf_drv_uart.h"

extern nrf_drv_uart_t m_uart;

int _write(int file, const char * p_char, int len)
{
    UNUSED_PARAMETER(file);

    uint8_t len8 = len & 0xFF;
    nrf_drv_uart_tx(&m_uart, (const uint8_t*)p_char, len8);
    return len8;
}

int _read(int file, char * p_char, int len)
{
    UNUSED_PARAMETER(file);

    ret_code_t result = nrf_drv_uart_rx(&m_uart, (uint8_t*)p_char, 1);
    if (result == NRF_SUCCESS) {
        return 1;
    } else {
        return -1;
    }
}

#endif // !defined(HAS_SIMPLE_UART_RETARGET)
#endif // NRF_LOG_USES_RTT != 1
#endif //NRF_MODULE_ENABLED(RETARGET)
