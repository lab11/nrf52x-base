#pragma once
#include "time.h"
#include "simple_thread.h"
#include "thread_coap_block.h"
#include "thread_coap.h"
#include "app_error.h"

#include "parse.pb.h"

#define GATEWAY_PACKET_VERSION 3

// set response handler for all gateway messages
void gateway_set_response_handler(otCoapResponseHandler handler);

otError gateway_coap_send(const otIp6Address* dest, const char* path, bool confirmable, struct timeval time, Message* msg);

otError gateway_coap_block_send(const otIp6Address* dest_addr, block_info* b_info,
    struct timeval time, Message* msg, block_finalize_cb cb, uint8_t* existing_buffer);
