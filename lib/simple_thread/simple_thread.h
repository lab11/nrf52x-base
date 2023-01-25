#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <openthread/thread.h>
#include <openthread/ip6.h>
#include <openthread/tasklet.h>
#include <openthread/dataset.h>

typedef struct {
  otExtAddress* ext_addr;
  otMasterKey* masterkey;
  uint8_t   channel;
  uint16_t  panid;
  int8_t    tx_power;
  bool      sed; // sleepy end device
  uint32_t  poll_period;
  uint32_t  child_period;
  bool      autocommission;
} thread_config_t;

void thread_state_changed_callback(uint32_t flags, void * p_context);

void thread_init(const thread_config_t* config);
void thread_process(void);
void thread_sleep(void);
void thread_reset_active_timestamp();
otInstance * thread_get_instance(void);
