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

#define TEST_LENGTH_SAMPLES 2048

extern float testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float testOutput[TEST_LENGTH_SAMPLES/2];
uint32_t fftSize = 1024;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
uint32_t refIndex = 213, testIndex = 0;

int main(void) {

    float maxValue;

    // Initialize.
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_pin_set(LED1);

    nrf_delay_ms(2000);

    printf("Original:\n");
    for(size_t i=0; i < 100; i++) {
        printf("%f ", testInput_f32_10khz[i]);
    }

    nrf_gpio_pin_toggle(LED1);
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, testInput_f32_10khz, ifftFlag, doBitReverse);
    nrf_gpio_pin_toggle(LED1);
    nrf_delay_ms(5);

    printf("\nAfter:\n");
    for(size_t i=0; i < 100; i++) {
        printf("%f ", testInput_f32_10khz[i]);
    }

    /* Process the data through the Complex Magnitude Module for
       calculating the magnitude at each bin */
    nrf_gpio_pin_toggle(LED1);
    arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize);
    nrf_gpio_pin_toggle(LED1);
    nrf_delay_ms(5);

    /* Calculates maxValue and returns corresponding BIN value */
    nrf_gpio_pin_toggle(LED1);
    arm_max_f32(testOutput, fftSize, &maxValue, &testIndex);
    nrf_gpio_pin_toggle(LED1);
    nrf_delay_ms(5);
    printf("\nreal index: 0x%lx\n", refIndex);
    printf("index: 0x%lx\n", testIndex);


    // Enter main loop.
    while (1) {
        nrf_delay_ms(2000);
    }
}

