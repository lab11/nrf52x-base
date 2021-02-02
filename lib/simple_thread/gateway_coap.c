#include <stdio.h>
#include <string.h>

#include "nrf_log.h"

#include "gateway_coap.h"
#include "thread_coap.h"
#include "thread_dns.h"

#include <openthread/message.h>

#include "custom_board.h"
#include "config.h"

#include "pb_encode.h"

static otCoapResponseHandler response_handler = NULL;
static uint32_t seq_no = 0;
static uint8_t* message_buf = NULL;
static bool alloc = false;
static block_finalize_cb callback;
extern uint8_t device_id[6];

static const otIp6Address unspecified_ipv6 =
{
    .mFields =
    {
        .m8 = {0}
    }
};

inline void gateway_set_response_handler(otCoapResponseHandler handler) {
  response_handler = handler;
}

void gateway_block_finalize(uint8_t code, otError result) {
  if (alloc) {
    free(message_buf);
  }
  alloc = false;
  callback(code, result);
}

static void gateway_response_handler (void* context, otMessage* message, const
                                 otMessageInfo* message_info, otError result) {
  if (result == OT_ERROR_NONE) {
    //NRF_LOG_INFO("Sent Message Successfully!");
  } else {
    NRF_LOG_INFO("Failed to send message! 0x%x", result);
  }

  uint8_t code = otCoapMessageGetCode(message);
  uint8_t upper = code >> 5;
  uint8_t lower = code & 0x1F;

  if (upper == 4) {
    if (lower == 4) {
      NRF_LOG_INFO("404 Not Found!");
      // send discovery!
      Message msg = Message_init_default;
      strncpy(msg.data.discovery, GATEWAY_PARSE_ADDR, sizeof(msg.data.discovery));
      struct timeval dummy = {0};
      gateway_coap_send(&(message_info->mPeerAddr), "discovery", false, dummy, &msg);
    }
  }

  if (response_handler) {
    response_handler(context, message, message_info, result);
  }
}

otError gateway_coap_send(const otIp6Address* dest_addr,
    const char* path, bool confirmable, struct timeval time, Message* msg) {
  if (otIp6IsAddressEqual(dest_addr, &unspecified_ipv6)) {
    return OT_ERROR_ADDRESS_QUERY;
  }

  otInstance * thread_instance = thread_get_instance();
  Header header = Header_init_default;
  header.version = GATEWAY_PACKET_VERSION;
  memcpy(header.id.bytes, device_id, sizeof(device_id));
  strncpy(header.device_type, GATEWAY_DEVICE_TYPE, sizeof(header.device_type));
  header.id.size = sizeof(device_id);

  header.tv_sec = time.tv_sec;
  header.tv_usec = time.tv_usec;

  header.seq_no = seq_no;

  memcpy(&(msg->header), &header, sizeof(header));

  uint8_t packed_data [512];

  pb_ostream_t stream;
  stream = pb_ostream_from_buffer(packed_data, sizeof(packed_data));
  pb_encode(&stream, Message_fields, msg);
  size_t len = stream.bytes_written;
  APP_ERROR_CHECK_BOOL(len < 512);

  otCoapType coap_type = confirmable ? OT_COAP_TYPE_CONFIRMABLE : OT_COAP_TYPE_NON_CONFIRMABLE;

  otError error = thread_coap_send(thread_instance, OT_COAP_CODE_PUT, coap_type, dest_addr, path, packed_data, len, false, gateway_response_handler);

  // increment sequence number if successful
  if (error == OT_ERROR_NONE) {
    seq_no++;
  }

  return error;
}

otError gateway_coap_block_send(const otIp6Address* dest_addr, block_info* b_info,
    struct timeval time, Message* msg, block_finalize_cb cb, uint8_t* existing_buffer) {
  otInstance * thread_instance = thread_get_instance();
  callback = cb;

  Header header = Header_init_default;
  header.version = GATEWAY_PACKET_VERSION;
  memcpy(header.id.bytes, device_id, sizeof(device_id));
  strncpy(header.device_type, GATEWAY_DEVICE_TYPE, sizeof(header.device_type));
  header.id.size = sizeof(device_id);

  header.tv_sec = time.tv_sec;
  header.tv_usec = time.tv_usec;

  header.seq_no = seq_no;

  memcpy(&(msg->header), &header, sizeof(header));

  pb_ostream_t stream;
  if (existing_buffer != NULL) {
    message_buf = existing_buffer;
    alloc = false;
  } else {
    message_buf = malloc(b_info->data_len + 256);
    alloc = true;
  }
  if(message_buf == NULL) {
    return NRF_ERROR_INVALID_PARAM;
  }
  stream = pb_ostream_from_buffer(message_buf, b_info->data_len + 256);
  pb_encode(&stream, Message_fields, msg);
  b_info->data_addr = message_buf;
  b_info->data_len = stream.bytes_written;
  b_info->callback = gateway_block_finalize;

  otError error = start_blockwise_transfer(thread_instance, dest_addr, b_info, block_response_handler);

  // increment sequence number if successful
  if (error == OT_ERROR_NONE) {
    seq_no++;
  }

  return error;
}
