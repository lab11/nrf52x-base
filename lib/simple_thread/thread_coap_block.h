#pragma once

#include "openthread/coap.h"

typedef void (*block_finalize_cb)(uint8_t code, otError result);

typedef struct block_info {
    otCoapCode code;

    uint32_t etag;
    const uint8_t* data_addr;
    size_t data_len;
    size_t block_number;
    size_t last_block_number;
    otCoapBlockSize block_size;
    char path[64]; // arbitrary for now!
    block_finalize_cb callback;

} block_info;

void __attribute__((weak)) block_response_handler(
                                void                * p_context,
                                otMessage           * p_message,
                                const otMessageInfo * p_message_info,
                                otError               p_result);

otError start_blockwise_transfer(otInstance* instance,
                              const otIp6Address* dest,
                              block_info* b_info,
                              otCoapResponseHandler response_handler);
