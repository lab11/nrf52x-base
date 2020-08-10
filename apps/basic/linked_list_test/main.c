#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "linked_list.h"

int main(void) {

  // Initialize.
  ret_code_t error_code = NRF_SUCCESS;
  error_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  printf("Log initialized!\n");

  ll_node *start = NULL;
  for (uint16_t i = 0; i < 6; i++) {
    ll_push(&start, (void*)&i, sizeof(i));
  }

  ll_node *n = start;
  while(n) {
    printf("%d,", *((uint16_t*) n->data));
    n = n->next;
  }
  printf("\n");

  printf("Removing #2\n");
  n = start;
  while(n) {
    if (*((uint16_t*) n->data) == 2) {
      ll_remove(&start, n);
    }
    n = n->next;
  }

  n = start;
  while(n) {
    printf("%d,", *((uint16_t*) n->data));
    n = n->next;
  }
  printf("\n");

  // Enter main loop.
  while (1) {
    nrf_delay_ms(1000);
  }
}

