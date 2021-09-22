#include "nrf.h"
#include "nrf_log.h"

#include "zboss_api.h"

#include "simple_zigbee.h"

void fix_errata_78_in_nrf_802154(void)
{
    /* Temporary workaround for anomaly 78. When timer is stopped using STOP task
     * its power consumption will be higher.
     * Issuing task SHUTDOWN fixes this condition. This have to be removed when
     * proper fix will be pushed to nRF-IEEE-802.15.4-radio-driver.
     */
    //if ((otPlatRadioGetState(m_ot_instance) == OT_RADIO_STATE_SLEEP) ||
    //    (otPlatRadioGetState(m_ot_instance) == OT_RADIO_STATE_DISABLED))
    //{
        //nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_SHUTDOWN);
    //}
    #if (__FPU_USED == 1)
    __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
    (void) __get_FPSCR();
    NVIC_ClearPendingIRQ(FPU_IRQn);
    #endif
}

void __attribute__((weak)) zigbee_sleep(void)
{
    // Enter sleep state if no more tasks are pending.
    fix_errata_78_in_nrf_802154();

//#ifdef SOFTDEVICE_PRESENT
//        ret_code_t err_code = sd_app_evt_wait();
//        ASSERT(err_code == NRF_SUCCESS);
//#else
        //__WFE();
    zb_sleep_now();
//#endif
}
