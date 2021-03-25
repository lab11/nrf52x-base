// Pin definitions for microbit v2 (nrf52833)
// Taken from https://tech.microbit.org/hardware/schematic/
// Schematic: https://github.com/microbit-foundation/microbit-v2-hardware/blob/main/V2/MicroBit_V2.0.0_S_schematic.PDF

#pragma once

#include "nrf_gpio.h"

// LED with microphone. Drive with high strength
#define LED_MIC NRF_GPIO_PIN_MAP(0,20)

// LED Rows. Drive high to enable LED
#define LED_ROW1 NRF_GPIO_PIN_MAP(0,21)
#define LED_ROW2 NRF_GPIO_PIN_MAP(0,22)
#define LED_ROW3 NRF_GPIO_PIN_MAP(0,15)
#define LED_ROW4 NRF_GPIO_PIN_MAP(0,24)
#define LED_ROW5 NRF_GPIO_PIN_MAP(0,19)

// LED Columns. Drive low to enable LED
#define LED_COL1 NRF_GPIO_PIN_MAP(0,28)
#define LED_COL2 NRF_GPIO_PIN_MAP(0,11)
#define LED_COL3 NRF_GPIO_PIN_MAP(0,31)
#define LED_COL4 NRF_GPIO_PIN_MAP(1, 5)
#define LED_COL5 NRF_GPIO_PIN_MAP(0,30)

// Push buttons
#define BTN_A NRF_GPIO_PIN_MAP(0,14)
#define BTN_B NRF_GPIO_PIN_MAP(0,23)

// Touch sensitive pins
#define TOUCH_LOGO  NRF_GPIO_PIN_MAP(1, 4)
#define TOUCH_RING0 NRF_GPIO_PIN_MAP(0, 2)
#define TOUCH_RING1 NRF_GPIO_PIN_MAP(0, 3)
#define TOUCH_RING2 NRF_GPIO_PIN_MAP(0, 4)

// Serial communication pins (Transmit and Receive)
#define UART_TXD NRF_GPIO_PIN_MAP(0, 6)
#define UART_RXD NRF_GPIO_PIN_MAP(1, 8)

// I2C communication bus (internal)
// Also includes a shared active-low interrupt (requires pull-up)
#define I2C_SCL NRF_GPIO_PIN_MAP(0, 8)
#define I2C_SDA NRF_GPIO_PIN_MAP(0,16)
#define SENSOR_INTERRUPT NRF_GPIO_PIN_MAP(0,25)

// Speaker output
#define SPEAKER_OUT NRF_GPIO_PIN_MAP(0, 0)

// Microphone
// Power is the same as the LED pin
#define MIC_POWER NRF_GPIO_PIN_MAP(0,20)
#define MIC_IN    NRF_GPIO_PIN_MAP(0  5)

// Edge connector pins (includes duplicates from above)
#define EDGE_P0  NRF_GPIO_PIN_MAP(0, 2)
#define EDGE_P1  NRF_GPIO_PIN_MAP(0, 3)
#define EDGE_P2  NRF_GPIO_PIN_MAP(0, 4)
#define EDGE_P3  NRF_GPIO_PIN_MAP(0,31)
#define EDGE_P4  NRF_GPIO_PIN_MAP(0,28)
#define EDGE_P5  NRF_GPIO_PIN_MAP(0,14)
#define EDGE_P6  NRF_GPIO_PIN_MAP(1, 5)
#define EDGE_P7  NRF_GPIO_PIN_MAP(0,11)
#define EDGE_P8  NRF_GPIO_PIN_MAP(0,10)
#define EDGE_P9  NRF_GPIO_PIN_MAP(0, 9)
#define EDGE_P10 NRF_GPIO_PIN_MAP(0,30)
#define EDGE_P11 NRF_GPIO_PIN_MAP(0,23)
#define EDGE_P12 NRF_GPIO_PIN_MAP(0,12)
#define EDGE_P13 NRF_GPIO_PIN_MAP(0,17)
#define EDGE_P14 NRF_GPIO_PIN_MAP(0, 1)
#define EDGE_P15 NRF_GPIO_PIN_MAP(0,13)
#define EDGE_P16 NRF_GPIO_PIN_MAP(1, 2)
// 17 and 18 are replaced with 3V
#define EDGE_P19 NRF_GPIO_PIN_MAP(0,26)
#define EDGE_P20 NRF_GPIO_PIN_MAP(1, 0)

