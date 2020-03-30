
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "simple_thread.h"
#include "thread_coap_block.h"

void __attribute__((weak)) block_response_handler(
                                void                * p_context,
                                otMessage           * p_message,
                                const otMessageInfo * p_message_info,
                                otError               p_result)
{
    block_info* b_info = (block_info*) p_context;
    uint8_t code = otCoapMessageGetCode(p_message);
    uint8_t upper = code >> 5;
    uint8_t lower = code & 0x1F;
    //NRF_LOG_INFO("CoAP Result: %d", p_result);
    //NRF_LOG_INFO("Received CoAP Response: %d.%2d", upper, lower);
    //NRF_LOG_INFO("\t code: %d\r\n", otCoapMessageGetType(p_message));

    if (upper == 2) {
      if (lower == 31) {
        // This is a continue code!
        //NRF_LOG_INFO("CONTINUE");
        //otCoapOption* option = otCoapMessageGetFirstOption(p_message);
        //while (option != NULL && option->mNumber != OT_COAP_OPTION_BLOCK1) {
        //  otCoapOption* option = otCoapMessageGetNextOption(p_message);
        //}
        //if (option != NULL) {
        start_blockwise_transfer(thread_get_instance(), &(p_message_info->mPeerAddr), b_info, block_response_handler);
        //}
      } else if (lower == 4) {
        // This is a changed code!
        // did we send all the blocks?
        // if we did, we are done, time to clean up!
        if (b_info->callback) {
          b_info->callback(code, p_result);
        }
      }
    }
    // in cases of errors or error responses, let callback handle it
    else if (upper == 4 || p_result != OT_ERROR_NONE) {
      if (b_info->callback) {
        b_info->callback(code, p_result);
      }
    }
}


otError start_blockwise_transfer(otInstance* instance,
                              const otIp6Address* dest,
                              block_info* b_info,
                              otCoapResponseHandler response_handler) {

  otError       error = OT_ERROR_NONE;
  otMessage   * message;
  otMessageInfo message_info;

  size_t actual_block_size = 1<<(b_info->block_size+4);
  size_t remaining_size = b_info->data_len - b_info->block_number * actual_block_size;
  bool more = actual_block_size < remaining_size;

  //NRF_LOG_INFO("block: %d/%d/%d out of %d", b_info->block_number, more, actual_block_size, remaining_size);

  message = otCoapNewMessage(instance, NULL);
  if (message == NULL)
  {
    NRF_LOG_INFO("Failed to allocate message for CoAP Request\r\n");
    // Force an app error and subsequent reset - this shouldn't happen!
    return OT_ERROR_NO_BUFS;
  }

  otCoapMessageInit(message, OT_COAP_TYPE_CONFIRMABLE, b_info->code);
  otCoapMessageGenerateToken(message, 2);
  // append etag option
  error = otCoapMessageAppendOption(message, 4, sizeof(b_info->etag), &(b_info->etag));
  APP_ERROR_CHECK(error);
  error = otCoapMessageAppendUriPathOptions(message, b_info->path);
  APP_ERROR_CHECK(error);
  error = otCoapMessageAppendBlock1Option(message,
                                          b_info->block_number,
                                          more,
                                          b_info->block_size);
  APP_ERROR_CHECK(error);
  error = otCoapMessageSetPayloadMarker(message);
  APP_ERROR_CHECK(error);

  error = otMessageAppend(message, b_info->data_addr + b_info->block_number * actual_block_size, more ? actual_block_size : remaining_size);
  if (error != OT_ERROR_NONE)
  {
    return error;
  }

  b_info->block_number++;

  memset(&message_info, 0, sizeof(message_info));
  message_info.mPeerPort    = OT_DEFAULT_COAP_PORT;
  memcpy(&message_info.mPeerAddr, dest, sizeof(message_info.mPeerAddr));

  error = otCoapSendRequest(instance,
      message,
      &message_info,
      response_handler,
      b_info);

  if (error != OT_ERROR_NONE && message != NULL)
  {
    NRF_LOG_INFO("Failed to send CoAP Request: %d\r\n", error);
    otMessageFree(message);
  }
  return error;

}

