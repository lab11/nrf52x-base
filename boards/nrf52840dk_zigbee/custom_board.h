#pragma once

#include "nrf_gpio.h"

// LED
#define LEDS_NUMBER 3
#define LEDS_ACTIVE_STATE 0
#define LED_1 NRF_GPIO_PIN_MAP(0,13)
#define LED_2 NRF_GPIO_PIN_MAP(0,14)
#define LED_3 NRF_GPIO_PIN_MAP(0,15)
#define LED_4 NRF_GPIO_PIN_MAP(0,16)
#define LEDS_INV_MASK LEDS_MASK
#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }
#define BSP_LED_0 LED_1
#define BSP_LED_1 LED_2
#define BSP_LED_2 LED_3
#define BSP_LED_3 LED_4
