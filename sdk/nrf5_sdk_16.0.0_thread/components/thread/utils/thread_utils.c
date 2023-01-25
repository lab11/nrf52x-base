/**
 * Copyright (c) 2018 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "thread_utils.h"

#include "app_util_platform.h"
#include "nrf_assert.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_soc.h"

#include "sdk_config.h"

#if defined(MULTIPROTOCOL_802154_CONFIG_PRESENT)
#include "multiprotocol_802154_config.h"
#endif

#include <openthread/cli.h>
#include <openthread/link.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/platform/openthread-system.h>
#include <openthread/platform/platform-fem.h>

#include <mbedtls/platform.h>
#include <openthread/heap.h>

#ifdef FREERTOS
#include "FreeRTOS.h"
#else
#include "mem_manager.h"
#endif

#if !OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
#include "nrf_cc310_platform_abort.h"
#include "nrf_cc310_platform_mutex.h"
#endif /* OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS */

/**@brief Pointer to the OpenThread instance. */
static otInstance * mp_ot_instance;

#if !defined OPENTHREAD_RADIO

static void* ot_calloc(size_t n, size_t size)
{
    void *p_ptr = NULL;

#ifdef FREERTOS
    p_ptr = pvPortMalloc(n * size);

    memset(p_ptr, 0, n * size);
#else
    p_ptr = nrf_calloc(n, size);
#endif

    return p_ptr;
}

static void ot_free(void *p_ptr)
{
#ifdef FREERTOS
    vPortFree(p_ptr);
#else
    nrf_free(p_ptr);
#endif
}

static void platform_init(void)
{
#if !defined FREERTOS
    APP_ERROR_CHECK(nrf_mem_init());
#endif

#if !OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS
    int ret;

    ret = mbedtls_platform_set_calloc_free(ot_calloc, ot_free);
    ASSERT(ret == 0);

#ifdef FREERTOS
    nrf_cc310_platform_abort_init();
    nrf_cc310_platform_mutex_init();
#endif

    ret = mbedtls_platform_setup(NULL);
    ASSERT(ret == 0);

#endif /* OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS */

    otHeapSetCAllocFree(ot_calloc, ot_free);
}

#endif /* OPENTHREAD_RADIO */

#if FEM_CONTROL_DEFAULT_ENABLE
static void fem_init(void)
{
    PlatformFemConfigParams config = PLATFORM_FEM_DEFAULT_CONFIG;

    // Configure main FEM pins.
    config.mPaCfg.mGpioPin  = FEM_CONTROL_DEFAULT_PA_PIN;
    config.mLnaCfg.mGpioPin = FEM_CONTROL_DEFAULT_LNA_PIN;
    config.mPdnCfg.mGpioPin = FEM_CONTROL_DEFAULT_PDN_PIN;

    // Set default FEM pin direction.
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_PA_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_LNA_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_PDN_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_MODE_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_ANTSEL_PIN);

    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_MOSI_PIN);
    nrf_gpio_cfg_default(FEM_CONTROL_DEFAULT_MISO_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_CLK_PIN);
    nrf_gpio_cfg_output(FEM_CONTROL_DEFAULT_CSN_PIN);

    // Set default FEM pin polarity.
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_PA_PIN);     // Disable PA, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_LNA_PIN);    // Disable LNA, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_PDN_PIN);    // Disable FEM, radio driver will override this setting
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_MODE_PIN);   // Use POUTA_PROD TX gain by default
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_ANTSEL_PIN); // Use antenna at "ANT1" port by default

    nrf_gpio_pin_set(FEM_CONTROL_DEFAULT_MOSI_PIN);     // SPI mode not used. Use high polarity by default.
    nrf_gpio_pin_set(FEM_CONTROL_DEFAULT_CLK_PIN);      // SPI mode not used. Use high polarity by default.
    nrf_gpio_pin_clear(FEM_CONTROL_DEFAULT_CSN_PIN);    // SPI mode not used. Use low polarity by default.

    PlatformFemSetConfigParams(&config);
}
#endif /* FEM_CONTROL_DEFAULT_ENABLE */

void thread_init(const thread_configuration_t * p_config)
{
    otError error;

    otSysInit(0, NULL);

#if defined(MULTIPROTOCOL_802154_CONFIG_PRESENT) && defined(MULTIPROTOCOL_802154_MODE)
    uint32_t retval = multiprotocol_802154_mode_set((multiprotocol_802154_mode_t)MULTIPROTOCOL_802154_MODE);
    ASSERT(retval == NRF_SUCCESS);
#endif

#if !defined OPENTHREAD_RADIO
    platform_init();
#endif

#if FEM_CONTROL_DEFAULT_ENABLE
    fem_init();
#endif

    mp_ot_instance = otInstanceInitSingle();
    ASSERT(mp_ot_instance != NULL);

    NRF_LOG_INFO("Thread version   : %s", (uint32_t)otGetVersionString());
    NRF_LOG_INFO("Network name     : %s",
                 (uint32_t)otThreadGetNetworkName(mp_ot_instance));

    if (!otDatasetIsCommissioned(mp_ot_instance) && p_config->autocommissioning)
    {
        error = otLinkSetChannel(mp_ot_instance, THREAD_CHANNEL);
        ASSERT(error == OT_ERROR_NONE);

        error = otLinkSetPanId(mp_ot_instance, THREAD_PANID);
        ASSERT(error == OT_ERROR_NONE);
    }

    if (!p_config->autostart_disable)
    {
        otLinkModeConfig mode;
        memset(&mode, 0, sizeof(mode));

        if (p_config->radio_mode == THREAD_RADIO_MODE_RX_OFF_WHEN_IDLE)
        {
            mode.mRxOnWhenIdle       = false; // Join network as SED.
            mode.mSecureDataRequests = true;

            error = otLinkSetPollPeriod(mp_ot_instance, p_config->poll_period);
            ASSERT(error == OT_ERROR_NONE);
        }
        else
        {
            mode.mRxOnWhenIdle       = true;
            mode.mSecureDataRequests = true;
#ifdef OPENTHREAD_FTD
            mode.mDeviceType         = true;
            mode.mNetworkData        = true;
#endif
        }

        error = otThreadSetLinkMode(mp_ot_instance, mode);
        ASSERT(error == OT_ERROR_NONE);

        if (p_config->default_child_timeout != 0)
        {
            otThreadSetChildTimeout(mp_ot_instance, p_config->default_child_timeout);
        }

        error = otIp6SetEnabled(mp_ot_instance, true);
        ASSERT(error == OT_ERROR_NONE);

        if (otDatasetIsCommissioned(mp_ot_instance) || p_config->autocommissioning)
        {
            error = otThreadSetEnabled(mp_ot_instance, true);
            ASSERT(error == OT_ERROR_NONE);

            NRF_LOG_INFO("Thread interface has been enabled.");
            NRF_LOG_INFO("802.15.4 Channel : %d", otLinkGetChannel(mp_ot_instance));
            NRF_LOG_INFO("802.15.4 PAN ID  : 0x%04x", otLinkGetPanId(mp_ot_instance));
            NRF_LOG_INFO("Radio mode       : %s", otThreadGetLinkMode(mp_ot_instance).mRxOnWhenIdle ?
                                            "rx-on-when-idle" : "rx-off-when-idle");
        }
    }
}

void thread_cli_init(void)
{
    ASSERT(mp_ot_instance != NULL);

    otCliUartInit(mp_ot_instance);
}

void thread_deinit(void)
{
    ASSERT(mp_ot_instance != NULL);

    otInstanceFinalize(mp_ot_instance);
    otSysDeinit();
    mp_ot_instance = NULL;
}

void thread_soft_deinit(void)
{
    ASSERT(mp_ot_instance != NULL);

    otInstanceFinalize(mp_ot_instance);
    mp_ot_instance = NULL;
}

void thread_process(void)
{
    ASSERT(mp_ot_instance != NULL);

    otTaskletsProcess(mp_ot_instance);
    otSysProcessDrivers(mp_ot_instance);
}

#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
static void fpu_sleep_prepare(void)
{
    uint32_t original_fpscr;

    CRITICAL_REGION_ENTER();
    original_fpscr = __get_FPSCR();
    /*
     * Clear FPU exceptions.
     * Without this step, the FPU interrupt is marked as pending,
     * preventing system from sleeping. Exceptions cleared:
     * - IOC - Invalid Operation cumulative exception bit.
     * - DZC - Division by Zero cumulative exception bit.
     * - OFC - Overflow cumulative exception bit.
     * - UFC - Underflow cumulative exception bit.
     * - IXC - Inexact cumulative exception bit.
     * - IDC - Input Denormal cumulative exception bit.
     */
    __set_FPSCR(original_fpscr & ~0x9Fu);
    __DMB();
    NVIC_ClearPendingIRQ(FPU_IRQn);
    CRITICAL_REGION_EXIT();

    /*
     * The last chance to indicate an error in FPU to the user
     * as the FPSCR is now cleared
     *
     * This assert is related to previous FPU operations
     * and not power management.
     *
     * Critical FPU exceptions signaled:
     * - IOC - Invalid Operation cumulative exception bit.
     * - DZC - Division by Zero cumulative exception bit.
     * - OFC - Overflow cumulative exception bit.
     */

    ASSERT((original_fpscr & 0x7) == 0);
}
#endif // (__FPU_PRESENT == 1) && (__FPU_USED == 1)

void thread_sleep(void)
{
    ASSERT(mp_ot_instance != NULL);

    // Enter sleep state if no more tasks are pending.
    if (!otTaskletsArePending(mp_ot_instance))
    {
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
        fpu_sleep_prepare();
#endif

#ifdef SOFTDEVICE_PRESENT
        ret_code_t err_code = sd_app_evt_wait();
        ASSERT(err_code == NRF_SUCCESS);
#else
        __WFE();
#endif
    }
}

otInstance * thread_ot_instance_get(void)
{
    ASSERT(mp_ot_instance != NULL);

    return mp_ot_instance;
}

void thread_state_changed_callback_set(otStateChangedCallback handler)
{
    ASSERT(mp_ot_instance != NULL);

    otError error = otSetStateChangedCallback(mp_ot_instance, handler, mp_ot_instance);
    ASSERT(error == OT_ERROR_NONE);
}

bool thread_soft_reset_was_requested(void)
{
    return otSysPseudoResetWasRequested();
}
