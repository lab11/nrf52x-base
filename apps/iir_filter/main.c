// Blink app
//
// Blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "nrf52840dk.h"

#include "arm_math.h"
#include "arm_const_structs.h"

#define TEST_LENGTH_SAMPLES 1260

extern float voltage[TEST_LENGTH_SAMPLES];
extern float current[TEST_LENGTH_SAMPLES];
static float filt_voltage[TEST_LENGTH_SAMPLES];
static float filt_current[TEST_LENGTH_SAMPLES];
static arm_fir_instance_f32 filter;
static float state[599+TEST_LENGTH_SAMPLES-1];
extern float coeff[599];

int main(void) {

    // Initialize.
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_pin_set(LED1);
    nrf_delay_ms(2000);

    printf("init\n");
    arm_fir_init_f32(&filter, 599, coeff, state, TEST_LENGTH_SAMPLES);
    nrf_gpio_pin_toggle(LED1);
    printf("filter\n");
    arm_fir_f32(&filter, current, filt_current, TEST_LENGTH_SAMPLES);
    nrf_gpio_pin_toggle(LED1);
    printf("done\n");

    printf("[ ");
    for (size_t i = 0; i < TEST_LENGTH_SAMPLES; i++) {
        printf("%f, ", filt_current[i]);
        if ((i+1) % 5 == 0) {
            printf("\n");
            nrf_delay_ms(100);
        }
    }
    printf("]\n");

    // Enter main loop.
    while (1) {
        nrf_delay_ms(2000);
    }
}

