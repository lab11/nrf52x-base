/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <string.h>
#include "tensorflow/lite/micro/examples/person_detection/main_functions.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/kernels/mem_logger.h"
#include "tensorflow/lite/micro/allocation_logger.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

// Pin definitions
#define LED NRF_GPIO_PIN_MAP(0,14)

namespace MemLogger{
 Event g_events[BUFFER_SIZE];
 int index=-1;
}

namespace AllocationLogger{
 AllocationInfo allocations[BUFFER_SIZE];
 int index=-1;
}
// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
int main(int argc, char* argv[]) {

  // memset((void *)0x200008f0, 0xaa, 0x34e7c);
  setup();
  nrf_gpio_cfg_output(LED);

  while (true) {
    loop();
    nrf_gpio_pin_toggle(LED);
    nrf_delay_ms(500);
    nrf_gpio_pin_toggle(LED);

  }
}
