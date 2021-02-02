// Pin definitions for nRF52 development kit (PCA10056)

#pragma once

#include "nrf_gpio.h"

#define LED1 NRF_GPIO_PIN_MAP(0,13)
#define LED2 NRF_GPIO_PIN_MAP(0,14)
#define LED3 NRF_GPIO_PIN_MAP(0,15)
#define LED4 NRF_GPIO_PIN_MAP(0,16)

#define BUTTON1 NRF_GPIO_PIN_MAP(0,11)
#define BUTTON2 NRF_GPIO_PIN_MAP(0,12)
#define BUTTON3 NRF_GPIO_PIN_MAP(0,24)
#define BUTTON4 NRF_GPIO_PIN_MAP(0,25)

#define UART_TXD NRF_GPIO_PIN_MAP(0,6)
#define UART_RXD NRF_GPIO_PIN_MAP(0,8)

