/* Blink
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "nrf_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "simple_thread.h"

#include <openthread/thread.h>

#define LED0 NRF_GPIO_PIN_MAP(0,4)
#define LED1 NRF_GPIO_PIN_MAP(0,5)
#define LED2 NRF_GPIO_PIN_MAP(0,6)

#define DEFAULT_CHILD_TIMEOUT    40                                         /**< Thread child timeout [s]. */
#define DEFAULT_POLL_PERIOD      1000                                       /**< Thread Sleepy End Device polling period when MQTT-SN Asleep. [ms] */
#define NUM_SLAAC_ADDRESSES      4                                          /**< Number of SLAAC addresses. */

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void thread_state_changed_callback(uint32_t flags, void * p_context)
{

    NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d",
                 flags, otThreadGetDeviceRole(p_context));
    NRF_LOG_INFO("802.15.4 Channel: %d\n", otLinkGetChannel(p_context));
}

int main(void) {
    nrf_power_dcdcen_set(1);

    log_init();

    // Initialize.
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_pin_set(LED2);

    otMasterKey masterkey = {.m8 = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}};

    thread_config_t thread_config = {
      .panid = 0xFACE,
      .masterkey = &masterkey,
      .channel = 25,
      .tx_power = 8,
      .sed = false,
      .poll_period = DEFAULT_POLL_PERIOD,
      .child_period = DEFAULT_CHILD_TIMEOUT,
      .autocommission = true,
    };

    // Enter main loop.
    thread_init(&thread_config);

    while (true)
    {
      thread_process();

      if (NRF_LOG_PROCESS() == false)
      {
        thread_sleep();
      }
    }

}
