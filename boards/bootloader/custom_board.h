// Pin definitions for nRF52 development kit (PCA10040)

#pragma once

#include "nrf_gpio.h"

// LED
#define LEDS_NUMBER 3
#define LEDS_ACTIVE_STATE 0
#define LED_1 NRF_GPIO_PIN_MAP(0,4)
#define LED_2 NRF_GPIO_PIN_MAP(0,5)
#define LED_3 NRF_GPIO_PIN_MAP(0,6)
#define LEDS_INV_MASK LEDS_MASK
#define LEDS_LIST { LED_1, LED_2, LED_3 }
#define BSP_LED_0 LED_1
#define BSP_LED_1 LED_2
#define BSP_LED_2 LED_3

// BUTTONS
#define BUTTONS_NUMBER 0
#define BUTTON_PULL 0

