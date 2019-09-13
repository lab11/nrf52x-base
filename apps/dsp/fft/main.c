// Blink app
//
// Blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"
#include "nrf_uarte.h"
#include "nrf_serial.h"
#include "app_timer.h"

#include "nrf52840dk.h"

#include "arm_math.h"
#include "arm_const_structs.h"

#define TEST_LENGTH_SAMPLES 8192

NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uart0_drv_config,
                      NRF_GPIO_PIN_MAP(0,6), NRF_GPIO_PIN_MAP(0,8),
                      0, 0,
                      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED,
                      NRF_UART_BAUDRATE_115200,
                      UART_DEFAULT_CONFIG_IRQ_PRIORITY);

#define SERIAL_FIFO_TX_SIZE 256
#define SERIAL_FIFO_RX_SIZE 256

NRF_SERIAL_QUEUES_DEF(serial_queues, SERIAL_FIFO_TX_SIZE, SERIAL_FIFO_RX_SIZE);


#define SERIAL_BUFF_TX_SIZE 1
#define SERIAL_BUFF_RX_SIZE 1

NRF_SERIAL_BUFFERS_DEF(serial_buffs, SERIAL_BUFF_TX_SIZE, SERIAL_BUFF_RX_SIZE);

NRF_SERIAL_CONFIG_DEF(serial_config, NRF_SERIAL_MODE_DMA,
                      &serial_queues, &serial_buffs, NULL, NULL);

NRF_SERIAL_UART_DEF(serial_uart, 0);

extern float testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float input[TEST_LENGTH_SAMPLES] = {0};
static float testOutput[TEST_LENGTH_SAMPLES/2];
uint32_t fftSizes[6] = {128, 256, 512, 1024, 2048, 4096};
const arm_cfft_instance_f32* instances[6] = {&arm_cfft_sR_f32_len128,
    &arm_cfft_sR_f32_len256, &arm_cfft_sR_f32_len512, &arm_cfft_sR_f32_len1024,
    &arm_cfft_sR_f32_len2048, &arm_cfft_sR_f32_len4096};
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
uint32_t refIndex = 213, testIndex = 0;

int main(void) {

    float maxValue;

    // Initialize.
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_pin_set(LED1);

    uint32_t err_code;

    err_code = nrf_drv_clock_init();
    if (err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED){
      APP_ERROR_CHECK(err_code);
    }

    nrf_drv_clock_lfclk_request(NULL);
    err_code = app_timer_init();
    if (err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED){
      APP_ERROR_CHECK(err_code);
    }

    err_code = nrf_serial_init(&serial_uart, &m_uart0_drv_config, &serial_config);
    printf("err_code = %x\n", err_code);

    for (size_t i = 0; i < 6; i++) {
        printf("\n\n\nSize: %u\n", fftSizes[i]);
        printf("\nOriginal:\n");
        for(size_t j=0; j < 2; j++) {
            printf("%f ", testInput_f32_10khz[j]);
        }
        memcpy(input, testInput_f32_10khz, sizeof(float)*TEST_LENGTH_SAMPLES);
        nrf_gpio_pin_toggle(LED1);
        arm_cfft_f32(instances[i], input, 0, doBitReverse);
        nrf_gpio_pin_toggle(LED1);
        nrf_delay_ms(1);

        printf("\nraw fft bins:\n");
        for(size_t j=0; j < 5; j++) {
            printf("%f ", input[j]);
        }
        printf("\n");
        for(size_t j=fftSizes[i]-1; j >= fftSizes[i]-1-5; j--) {
            printf("%f ", input[j]);
        }

        /* Process the data through the Complex Magnitude Module for
           calculating the magnitude at each bin */
        nrf_gpio_pin_toggle(LED1);
        arm_cmplx_mag_f32(input, testOutput, fftSizes[i]);
        nrf_gpio_pin_toggle(LED1);
        nrf_delay_ms(1);
        printf("\nmag fft bins:\n");
        for(size_t j=0; j < 5; j++) {
            printf("%f ", testOutput[j]);
        }
        printf("\n");
        for(size_t j=fftSizes[i]-1; j >= fftSizes[i]-1-4; j--) {
            printf("%f ", testOutput[j]);
        }


        //nrf_gpio_pin_toggle(LED1);
        //arm_cfft_f32(instances[i], input, 1, doBitReverse);
        //nrf_gpio_pin_toggle(LED1);
        //nrf_delay_ms(5);
        //printf("\ninverted:\n");
        //for(size_t j=0; j < 10; j++) {
        //    printf("%f ", input[j]);
        //}

        //float error[TEST_LENGTH_SAMPLES] = {0};
        //float total = 0;
        //for(size_t j=0; j < TEST_LENGTH_SAMPLES; j++) {
        //    float diff = input[j] - testInput_f32_10khz[j];
        //    error[j] = diff*diff;
        //    total += error[j];
        //}
        //printf("\ninverted error = %f\n", total);

        /* Calculates maxValue and returns corresponding BIN value */
        nrf_gpio_pin_toggle(LED1);
        arm_max_f32(testOutput, fftSizes[i], &maxValue, &testIndex);
        nrf_gpio_pin_toggle(LED1);
        nrf_delay_ms(1);
        printf("\nreal index: 0x%lx\n", refIndex);
        printf("index: 0x%lx\n", testIndex);

        // Send over UART
        uint32_t len = sizeof(float) * fftSizes[i];
        uint8_t *buf = malloc(len+6);
        //[sizeof(float)*TEST_LENGTH_SAMPLES/2] = {0};
        buf[0] = 0xAA;
        buf[1] = 0xBB;
        memcpy(buf+2, &len, sizeof(float));
        memcpy(buf+6, testOutput, len);
        nrf_serial_write(&serial_uart, buf, len+6, NULL, NRF_SERIAL_MAX_TIMEOUT);
        free(buf);

        // Send over UART
        len = sizeof(float) * 2*fftSizes[i];
        buf = malloc(len+6);
        //[sizeof(float)*TEST_LENGTH_SAMPLES/2] = {0};
        buf[0] = 0xAA;
        buf[1] = 0xCC;
        memcpy(buf+2, &len, sizeof(float));
        memcpy(buf+6, input, len);
        nrf_serial_write(&serial_uart, buf, len+6, NULL, NRF_SERIAL_MAX_TIMEOUT);
        free(buf);

        //nrf_delay_ms(500);
    }
    // Enter main loop.
    while(1){
        nrf_delay_ms(2000);
    }
}

