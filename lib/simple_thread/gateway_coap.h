#pragma once
#include "time.h"
#include "simple_thread.h"
#include "thread_coap_block.h"
#include "thread_coap.h"
#include "app_error.h"

#include "parse.pb.h"

#define GATEWAY_PACKET_VERSION 3

otError gateway_coap_send(const otIp6Address* dest, const char* path, bool confirmable, Message* msg);

otError gateway_coap_block_send(const otIp6Address* dest_addr, block_info* b_info,
    Message* msg, block_finalize_cb cb, const uint8_t* existing_buffer);
