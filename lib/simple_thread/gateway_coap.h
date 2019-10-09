#pragma once
#include "time.h"
#include "simple_thread.h"
#include "thread_coap.h"
#include "app_error.h"

#define GATEWAY_PACKET_VERSION 2

typedef struct {
  char* path;
  uint8_t id_len;
  uint8_t* id;
  uint32_t seq_no;
  struct timeval timestamp;
  uint8_t data_len;
  uint8_t* data;
} gateway_packet_t;

otError gateway_coap_send(otIp6Address* dest, const char* path, bool confirmable, const gateway_packet_t* packet);
