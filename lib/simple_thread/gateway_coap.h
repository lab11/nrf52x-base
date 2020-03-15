#pragma once
#include "time.h"
#include "simple_thread.h"
#include "thread_coap.h"
#include "app_error.h"

#include "parse.pb.h"

#define GATEWAY_PACKET_VERSION 3

otError gateway_coap_send(otIp6Address* dest, const char* path, const char* device_type, bool confirmable, Message* msg);
