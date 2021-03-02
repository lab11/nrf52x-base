/* Blink
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "nrf_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include <openthread/dataset_ftd.h>
#include <openthread/thread_ftd.h>
#include <openthread/channel_monitor.h>
#include <openthread/channel_manager.h>
#include <openthread/joiner.h>

#include "simple_thread.h"
#include "custom_board.h"

APP_TIMER_DEF(monitor_timer);

#define SCHED_QUEUE_SIZE 32
#define SCHED_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE

#define DEFAULT_CHILD_TIMEOUT    40                                         /**< Thread child timeout [s]. */
#define DEFAULT_POLL_PERIOD      1000                                       /**< Thread Sleepy End Device polling period when MQTT-SN Asleep. [ms] */

void __attribute__((weak)) thread_state_changed_callback(uint32_t flags, void * p_context)
{
    uint32_t role = otThreadGetDeviceRole(p_context);
    NRF_LOG_INFO("State changed! Flags: 0x%08lx Current role: %d\r\n",
                 flags, otThreadGetDeviceRole(p_context));
    nrf_gpio_pin_set(LED1);
    nrf_gpio_pin_set(LED2);
    nrf_gpio_pin_set(LED3);
    if (role == OT_DEVICE_ROLE_CHILD) {
        nrf_gpio_pin_clear(LED1);
    }
    if (role == OT_DEVICE_ROLE_ROUTER) {
        nrf_gpio_pin_clear(LED2);
    }
    if (role == OT_DEVICE_ROLE_LEADER) {
        nrf_gpio_pin_clear(LED3);
    }
}

void join_callback(otError error, void* context) {
  NRF_LOG_INFO("join error: %d\n", error);
}

void monitor_timer_callback() {
  printf("channel occupancy:\n");
  for (uint8_t i = 11; i < 27; i++) {
    uint16_t occ = otChannelMonitorGetChannelOccupancy(thread_get_instance(), i);
    printf("\t%u:\t%.2f\n", i, (float)occ/(float)0xffff);
  }
  printf("samples: %lu\n", otChannelMonitorGetSampleCount(thread_get_instance()));
  printf("current channel: %u\n", otLinkGetChannel(thread_get_instance()));
  printf("----------------\n");

}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void) {
    nrf_power_dcdcen_set(1);

    log_init();

    // Initialize.
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED4);
    nrf_gpio_pin_set(LED2);
    nrf_gpio_pin_set(LED3);
    nrf_gpio_pin_set(LED4);
    nrf_gpio_pin_clear(LED1);

    thread_config_t thread_config = {
      .tx_power = 8,
      .panid = 0xFACE,
      .masterkey.m8 = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff},
      .has_masterkey = true,
      .sed = false,
      .poll_period = 10,
      .child_period = 30000,
      .autocommission = true,
    };

    thread_init(&thread_config);
    bool mon_enabled = otChannelMonitorIsEnabled(thread_get_instance());
    uint32_t mon_interval = otChannelMonitorGetSampleInterval(thread_get_instance());
    uint32_t mon_window = otChannelMonitorGetSampleWindow(thread_get_instance());
    int8_t  mon_thresh = otChannelMonitorGetRssiThreshold(thread_get_instance());
    NRF_LOG_INFO("channel monitoring enabled: %d, %ums, %u samples, %ddBm", mon_enabled, mon_interval, mon_window, mon_thresh);

    otChannelManagerSetAutoChannelSelectionEnabled(thread_get_instance(), true);
    bool manager_enabled = otChannelManagerGetAutoChannelSelectionEnabled(thread_get_instance());
    uint16_t manager_delay = otChannelManagerGetDelay(thread_get_instance());
    uint32_t manager_interval = otChannelManagerGetAutoChannelSelectionInterval(thread_get_instance());
    NRF_LOG_INFO("channel manager enabled: %d, interval: %lus, delay: %us", manager_enabled, manager_interval, manager_delay);

    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    app_timer_init();
    app_timer_create(&monitor_timer, APP_TIMER_MODE_REPEATED, monitor_timer_callback);
    app_timer_start(monitor_timer, APP_TIMER_TICKS(60000), NULL);

    // Enter main loop.
    while (1) {
        thread_process();
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false)
        {
          thread_sleep();
        }
    }
}
